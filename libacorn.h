// MIT License
//
// Copyright (c) 2023 Sandcastle Software Ltd
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <stdio.h>
#ifdef __cplusplus
#include <cmath>
#include <cstdarg>
#include <cfloat>
#include <cctype>
#else
#include <math.h>
#include <stdarg.h>
#endif
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <semaphore.h>
#include <poll.h>
#ifdef __cplusplus
#include <typeinfo>
#include <cstring>
#include <map>
#include <unordered_map>
#include <iterator>
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <algorithm>
#include <set>
#include <functional>
#include <assert.h>
#include <chrono>
#include <thread>
//#include <atomic>

#ifndef MIN
#define MIN(a,b) (((a)<(b)) ? (a) : (b))
#define MAX(a,b) (((a)>(b)) ? (a) : (b))
#endif

using std::map;
using std::unordered_map;
using std::list;
using std::vector;
using std::pair;
using std::make_pair;
using std::set;
using std::stack;
using std::function;
template <class T>
using sp = std::shared_ptr<T>;

// Platform headers
#ifdef __OBJC__
#import <AppKit/AppKit.h>
#include <objc/runtime.h>
#endif

#ifdef PLATFORM_ANDROID
#define PLATFORM android
#endif
#ifdef PLATFORM_APPLE
#define PLATFORM apple
#endif
#if PLATFORM_LINUX
#undef linux
#define PLATFORM linux
#endif
#define __xincstr(x) #x
#define __incstr(x) __xincstr(x)

namespace acorn {
void main();
#include "base/logging.h"
#include "base/bytearray.h"
#include "base/bytestream.h"
#include "base/hash.h"
#include "base/string_.h"
#include "base/error.h"
#include "base/object.h"
//#include "base/url.h"
//#include "base/timer.h"
//#include "base/task.h"
//#include "base/semaphore.h"
//#include "base/urlrequest.h"
//#include "graphics/color.h"
//#include "graphics/vectors.h"
//#include "graphics/matrix.h"
//#include "graphics/bitmap.h"
}

#include "js/quickjspp.hpp"
#include "js/quickjs-libc.h"

template <typename T>
void js_init(qjs::Context::Module&);

namespace acorn {
#include "base/val.h"

#if __OBJC__
extern string ns2str(NSString* s);
extern NSString* str2ns(const string& s);
#endif

}

/** Conversion traits for acorn::string */
template <>
struct qjs::js_traits<acorn::string>
{
    static acorn::string unwrap(JSContext * ctx, JSValueConst v)
    {
        size_t plen;
        const char * ptr = JS_ToCStringLen(ctx, &plen, v);
        return acorn::string(ptr, plen);
    }

    static JSValue wrap(JSContext * ctx, const acorn::string& str) noexcept
    {
        return JS_NewStringLen(ctx, str.c_str(), str.length());
    }
};

/** Conversion traits for acorn::val */
template <>
struct qjs::js_traits<acorn::val>
{
    static acorn::val unwrap(JSContext * ctx, JSValueConst v)
    {
        auto tag = JS_VALUE_GET_TAG(v);
        switch (tag) {
            case JS_TAG_INT: return JS_VALUE_GET_INT(v);
            case JS_TAG_BOOL: return JS_VALUE_GET_BOOL(v);
            case JS_TAG_NULL: return acorn::val();
            case JS_TAG_FLOAT64: {
                double d = JS_VALUE_GET_FLOAT64(v);
                if (rint(d) == d) { // if its integral
                    int64_t i64 = (int64_t)d;
                    if (i64>= -0x7fffffffffffffffLL && i64 <= 0x7fffffffffffffffLL) {
                        if (i64 >= -0x7fffffff && i64 <= 0x7fffffff) {
                            return (int32_t)i64;
                        }
                        return i64;
                    }
                }
                return d;
            }
            case JS_TAG_STRING: {
                size_t plen;
                const char * ptr = JS_ToCStringLen(ctx, &plen, v);
                return acorn::string(ptr, (int)plen);
            }
            case JS_TAG_OBJECT: {
                if (JS_IsArray(ctx, v)) {
                    vector<acorn::val> vec = js_traits<vector<acorn::val>>::unwrap(ctx, v);
                    return vec;
                }
                auto clsid = JS_GetClassID(v);
                // Anonymous JS object: promote to map
                if (clsid == 1 /*JS_CLASS_OBJECT*/) {
                    acorn::val rv;
                    JSPropertyEnum* props = nullptr;
                    uint32_t numProps = 0;
                    if (0 == JS_GetOwnPropertyNames(ctx, &props, &numProps, v, JS_GPN_STRING_MASK | JS_GPN_SYMBOL_MASK | JS_GPN_PRIVATE_MASK)) {
                        for (int i=0 ; i<numProps ; i++) {
                            const char* fieldName = JS_AtomToCString(ctx, props[i].atom);
                            JSValue fieldVal = JS_GetProperty(ctx, v, props[i].atom);
                            rv.set(acorn::string(fieldName,-1), unwrap(ctx, fieldVal));
                        }
                    } else {
                        assert(0);
                    }
                    return rv;
                }
                // Should be a known C++ class.
                shared_ptr<acorn::Object>* ptr = (shared_ptr<acorn::Object>*)JS_GetOpaque(v, clsid);
                return acorn::val(*ptr);
            }
            case JS_TAG_UNDEFINED: {
                return acorn::val();
            }
            default:
                assert(false);
                break;
        }
        
    }

    static JSValue wrap(JSContext * ctx, const acorn::val& v) noexcept
    {
        using namespace acorn;
        switch (v.type) {
            case val::EMPTY: return JS_UNDEFINED;
            case val::INT32: return JS_MKVAL(JS_TAG_INT, v._i32);
            case val::UINT32: return JS_NewUint32(ctx, v._u32);
            case val::INT64: return JS_NewInt64(ctx, v._i64);
            case val::UINT64: return JS_NewBigUint64(ctx, v._u64);
            case val::FLOAT32: return JS_NewFloat64(ctx, (double)v._f32);
            case val::FLOAT64: return JS_NewFloat64(ctx, v._f64);
            case val::STRING: return qjs::js_traits<acorn::string>::wrap(ctx, *v._str);
                /*case val::MEASUREMENT: return string::format("\"%s\"", _measurement.toString().c_str());
                 case val::BYTEARRAY: {
                 log_warn("TODO! bytearray json representation needed");
                 break;
                 }*/
            case val::ARRAY: {
                if (!v._vec) {
                    return JS_NULL;
                }
                auto rv = JS_NewArray(ctx);
                int i=0;
                for (auto it=v._vec->begin() ; it != v._vec->end() ; it++) {
                    auto& vv = v._vec->at(i);
                    JS_DefinePropertyValue(ctx, rv, i++, wrap(ctx, vv), JS_PROP_C_W_E);
                }
                return rv;
            }
            case val::MAP: {
                if (!v._map) {
                    return JS_NULL;
                }
                assert(false);
                auto rv = JS_NewObject(ctx);
                for (auto it=v._map->begin() ; it != v._map->end() ; it++) {
                    auto& field = it;
                    //s.append(string::format("\"%s\":", field->first.c_str()));
                    //s.append(field->second.toJson());
                }
                return rv;
            }
            default:
                assert(false); // todo: implement
                break;
        }
        return JS_NULL;
    }
};



#include "js/quickjs-libc.h"


#define DECLPROP(name, type) \
    type _##name; \
    type get_##name(); \
    type set_##name(const type& v);
#define DECLPROP_SP_RO(name, type) \
    std::shared_ptr<type> _##name; \
    std::shared_ptr<type> get_##name();
#define PROPERTY(C, x) .property<&C::get_##x, &C::set_##x>(#x)

using namespace acorn;

extern float g_backingScaleFactor;

#include "lib/fetch.h"
#include "ux/view.h"
#include "ux/window.h"
#include "ux/textview.h"
#include "ux/imageview.h"

extern bool resolvePath(string& path);
#define countof(x) (sizeof(x) / sizeof((x)[0]))



namespace std {
template <>
struct hash<acorn::string> {
    size_t operator()( const acorn::string& k ) const {
        return k.hash();
    }
};
}


#endif
