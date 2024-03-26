//
//  libacorn
//
#include "../libacorn.h"
/*
JSValue promise_executor(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    JSValue resolve_func = argv[0];
    JSValue reject_func = argv[1];

    JSValue this_val2 = JS_DupValue(ctx, this_val);
    JSValue resolve_func2 = JS_DupValue(ctx, resolve_func);

    const char* url = "https://www.google.com/";
    NSURLSession* session = [NSURLSession sharedSession];
    NSURLSessionTask* task = [session downloadTaskWithURL:[NSURL URLWithString:str2ns(acorn::string(url, -1))] completionHandler:^(NSURL* location, NSURLResponse* response, NSError* error) {
        NSLog(@"completion handler");
        dispatch_async(dispatch_get_main_queue(), ^{
            JSValue resval = JS_NewObject(ctx);
            JS_Call(ctx, resolve_func2, this_val2, 1, &resval);
            JS_FreeValue(ctx, resolve_func2);
            JS_FreeValue(ctx, this_val2);
        });

    }];
    [task resume];

    return JS_UNDEFINED;
}
*/




struct request_options {
    string method;
    map<string,val> headers;
    bytearray body;
    
    template <typename T>
    inline void copyIfPresent(T& target, map<string,val>& m, const char* name) {
        string sname(name,-1);
        if (!m.contains(sname)) {
            return;
        }
        const val& v = m[sname];
        if constexpr (std::is_same<T,string>()) {
            target = v.stringVal();
        } else if constexpr (std::is_same<T,map<string,string>>()) {
            target = v.compoundRef();
        } else if constexpr (std::is_same<T,bytearray>()) {
            target = v.bytearrayRef();
        } else {
            assert(false);
        }
    }
    void updateFromVal(const val& v) {
        if (!v.isCompound()) {
            throw std::runtime_error("Expected map type");
        }
        auto m = v.compoundRef();
        copyIfPresent(method, m, "method");
        copyIfPresent(headers, m, "headers");
        copyIfPresent(body, m, "body");
    }
};



template <typename T>
inline bool objIsClass(JSValue v) {
    return JS_GetClassID(v) == qjs::js_traits<std::shared_ptr<T>>::QJSClassId;
}
template <typename T>
inline std::shared_ptr<T> objGet(JSContext* ctx, JSValue v) {
    if (JS_GetClassID(v) == qjs::js_traits<std::shared_ptr<T>>::QJSClassId) {
        return qjs::js_traits<std::shared_ptr<T>>::unwrap(ctx, v);
    }
    return nullptr;
}

// NB: This is the fetch promise executor function, not the actual global 'fetch' function
JSValue js_httpreq(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    assert(argc==4);
    JSValue this_val2 = JS_DupValue(ctx, this_val);
    JSValue url = argv[0];
    val options = qjs::js_traits<val>::unwrap(ctx, argv[1]);
    JSValue resolve = JS_DupValue(ctx, argv[2]);
    JSValue reject = JS_DupValue(ctx, argv[3]);
    
    if (!JS_IsString(url)) {
        JS_ThrowTypeError(ctx, "Only string urls supported");
        return JS_EXCEPTION;
    }
        
    if (!options.isEmpty()) {
        if (!options.isCompound()) {
            JS_ThrowTypeError(ctx, "Options param should be a plain object");
            return JS_EXCEPTION;
        }
    }

    // MACOS
    NSURLSession* session = [NSURLSession sharedSession];
    NSURL* nsurl = [NSURL URLWithString:[NSString stringWithUTF8String:JS_ToCString(ctx, url)]];
    NSMutableURLRequest* req = [[NSMutableURLRequest alloc] initWithURL:nsurl];
    req.HTTPMethod = str2ns(options["method"].stringRef());
    val headers = options["headers"];
    // todo: propagate headers
    val body = options["body"];
    if (!body.isEmpty()) {
        req.HTTPBody = [NSData dataWithBytesNoCopy:body.bytearrayRef().data() length:body.bytearrayRef().length()];
    }
    NSURLSessionTask* task = [session downloadTaskWithRequest:req completionHandler:^(NSURL* location, NSURLResponse* res, NSError* error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            NSHTTPURLResponse* httpres = (NSHTTPURLResponse*)res;
            JSValue resval = JS_NewObject(ctx);
            JS_SetPropertyStr(ctx, resval, "status", JS_NewInt32(ctx, (int)httpres.statusCode));
            // todo: propagate headers
            JS_Call(ctx, resolve, this_val2, 1, &resval);
            JS_FreeValue(ctx, this_val2);
            JS_FreeValue(ctx, resolve);
            JS_FreeValue(ctx, reject);
            JS_FreeValue(ctx, resval);
        });
    }];
    [task resume];

    return JS_UNDEFINED;
}
void js_init_fetch(qjs::Context* c, qjs::Context::Module& m) {
    
    m.add("httpreq", JS_NewCFunction(c->ctx, js_httpreq, "httpreq", 4));

}



