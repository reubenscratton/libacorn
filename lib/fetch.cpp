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


static const char* s_MethodVals[] = {
    "GET",
    "POST",
    "HEAD",
    "PATCH",
    "DELETE",
    NULL
};
static const char* s_RequestCacheVals[] = {
    "default",
    "no-store",
    "reload",
    "no-cache",
    "force-cache",
    "only-if-cached",
    NULL
};
static const char* s_ModeVals[] = {
    "cors",
    "no-cors",
    "same-origin",
    "navigate",
    NULL
};
static const char* s_CredentialsVals[] = {
    "same-origin",
    "omit",
    "include",
    NULL
};
static const char* s_RedirectVals[] = {
    "follow",
    "error",
    "manual",
    NULL
};
static const char* s_PriorityVals[] = {
    "auto",
    "high",
    "low",
    NULL
};

class stringEnum {
public:
    stringEnum(const char** vals) : _vals(vals), _val(-1) {
    }
    stringEnum(const char** vals, int val) : _vals(vals), _val(val) {
    }
    stringEnum& operator =(const char* val) {
        set(val);
        return *this;
    }
    stringEnum& operator =(const string& val) {
        set(val.c_str());
        return *this;
    }
    void set(const char* sz) {
        for (int i=0 ; _vals[i] ; i++) {
            if (0==strcmp(sz, _vals[i])) {
                _val = i;
                return;
            }
        }
        throw std::runtime_error("bad enum value");
    }
    const char* get() const {
        return _vals[_val];
    }
private:
    int _val;
    const char** _vals;
};

class AbortSignal : public JSObj {
public:
    
};

class Headers : public JSObj {
public:
    Headers(const val& init) {
        if (init.isEmpty()) {
        }
        else if (init.isCompound()) {
            auto& m = init.compoundRef();
            _headers.insert(m.begin(), m.end());
        } else if (init.isObject()) {
            sp<Headers> src = init.objectVal<Headers>();
            _headers = src->_headers;
        } else {
            throw std::runtime_error("Bad arg to Headers()");
        }
    }

    map<string,string> _headers;
};

class Request : public JSObj {
public:
    struct options {
        stringEnum method { s_MethodVals, 0 };
        sp<Headers> headers { std::make_shared<Headers>() };
        bytearray body;
        stringEnum mode { s_ModeVals, 0 };
        stringEnum cache { s_RequestCacheVals, 0 };
        stringEnum credentials { s_CredentialsVals, 0 };
        stringEnum redirect { s_RedirectVals, 0 };
        string referrer; // 'no-referrer', 'client', or a URL
        string referrerPolicy;
        string integrity;
        bool keepalive;
        std::optional<std::shared_ptr<AbortSignal>> signal;
        stringEnum priority { s_PriorityVals, 0 };
        
        template <typename T>
        inline void copyIfPresent(T& target, map<string,val>& m, const char* name) {
            string sname(name,-1);
            if (!m.contains(sname)) {
                return;
            }
            const val& v = m[sname];
            if constexpr (std::is_same<T,string>()) {
                target = v.stringVal();
            } else if constexpr(std::is_same<T,stringEnum>()) {
                target = v.stringVal();
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
            if (m.contains("headers")) headers = m["headers"];
            if (m.contains("body")) body = m["body"].bytearrayRef();
            if (m.contains("mode")) mode = m["mode"].stringVal();
            if (m.contains("cache")) cache = m["cache"].stringVal();
            if (m.contains("credentials")) credentials = m["credentials"].stringVal();
            if (m.contains("redirect")) redirect = m["redirect"].stringVal();
            if (m.contains("referrer")) referrer = m["referrer"].stringVal();
            if (m.contains("referrerPolicy")) referrerPolicy = m["referrerPolicy"].stringVal();
            if (m.contains("integrity")) integrity = m["integrity"].stringVal();
            if (m.contains("keepalive")) keepalive = m["keepalive"].boolVal();
            if (m.contains("signal")) signal = m["signal"].objectVal<AbortSignal>();
            if (m.contains("priority")) priority = m["priority"].stringVal();
        }
    };
    
    Request(const val& input, const val& options) {
        if (input.isString()) {
            _url = input.stringRef();
        } else if (input.isObject()) {
            sp<Request> src = input.objectVal<Request>();
            _url = src->_url;
            _options = src->_options;
        } else {
            throw std::runtime_error("Bad arg to Request()");
        }
        
        if (!options.isEmpty()) {
            _options.updateFromVal(options);
        }
    }
    
    
    
//protected:
    string _url;
    options _options;
    
};


JSValue jspp_fetch(const val& resource, const val& options, std::function<void()> resolve, std::function<void()> reject) {
    
    /*JSValue promise_ctor = JS_GetPropertyStr(ctx, JS_GetGlobalObject(ctx), "Promise");
    JSValue promise_resolve_func = JS_GetPropertyStr(ctx, promise_ctor, "resolve");
    JSValue promise_reject_func = JS_GetPropertyStr(ctx, promise_ctor, "reject");
    JSValue promise_executor_func = JS_NewCFunction(ctx, promise_executor, "executor", 2);

    JSValue promise = JS_CallConstructor(ctx, promise_ctor, 1, &promise_executor_func);

    return promise;*/
    //return JS_EXCEPTION;
    return JS_UNDEFINED;
}

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
JSValue js_fetch(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    assert(argc==4);
    JSValue this_val2 = JS_DupValue(ctx, this_val);
    JSValue resource = argv[0];
    val options = qjs::js_traits<val>::unwrap(ctx, argv[1]);
    JSValue resolve = JS_DupValue(ctx, argv[2]);
    JSValue reject = JS_DupValue(ctx, argv[3]);
    
    sp<Request> req;
    if (JS_IsString(resource)) {
        req = std::make_shared<Request>(val(qjs::js_traits<string>::unwrap(ctx, resource)), val());
    }
    else if (JS_IsObject(resource)) {
        if (objIsClass<Request>(resource)) {
            req = objGet<Request>(ctx, resource);
        }
    }
    
    if (!req) {
        JS_ThrowTypeError(ctx, "Expected string or stringable object or Request object");
        return JS_EXCEPTION;
    }
    
    if (!options.isEmpty()) {
        if (!options.isCompound()) {
            JS_ThrowTypeError(ctx, "Options param should be a plain object");
            return JS_EXCEPTION;
        }
        req->_options.updateFromVal(options);
    }

    // MACOS
    NSURLSession* session = [NSURLSession sharedSession];
    NSURL* url = [NSURL URLWithString:str2ns(req->_url)];
    NSMutableURLRequest* urlreq = [[NSMutableURLRequest alloc] initWithURL:url];
    urlreq.HTTPMethod = [NSString stringWithUTF8String:req->_options.method.get()];
    NSURLSessionTask* task = [session downloadTaskWithRequest:urlreq completionHandler:^(NSURL* location, NSURLResponse* response, NSError* error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            JSValue resval = JS_NewObject(ctx);
            JS_Call(ctx, resolve, this_val2, 1, &resval);
            JS_FreeValue(ctx, this_val2);
            JS_FreeValue(ctx, resolve);
            JS_FreeValue(ctx, reject);
        });
    }];
    [task resume];

    /*if (argc<1 || argc>2) {
        return JS_EXCEPTION;
    }
    const char* url = JS_ToCString(ctx, argv[0]);
    if (!url)
        return JS_EXCEPTION;

    JSValue ctor = JS_GetPropertyStr(ctx, JS_GetGlobalObject(ctx), "Promise");
    JSValue executor_func = JS_NewCFunction(ctx, promise_executor, "executor", 2);

    JSValue promise = JS_CallConstructor(ctx, ctor, 1, &executor_func);

    return promise;*/
    return JS_UNDEFINED;
}
void js_init_fetch(qjs::Context* c, qjs::Context::Module& m) {
    
    m.add("fetch", JS_NewCFunction(c->ctx, js_fetch, "fetch", 1));

    m.class_<Headers>("Headers")
    .constructor<const val&>()
    ;
    
    m.class_<Request>("Request")
     .constructor<const val&, const val&>()
        //.fun<&Request::arrayBuffer>("arrayBuffer")
        //PROPERTY(View, padding)
    ;
}



