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

// Platform headers
#ifdef __OBJC__
#import <AppKit/AppKit.h>
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
#include "base/val.h"
//#include "base/task.h"
//#include "base/semaphore.h"
//#include "base/urlrequest.h"
//#include "graphics/color.h"
//#include "graphics/vectors.h"
//#include "graphics/matrix.h"
//#include "graphics/bitmap.h"
}
using namespace acorn;

#include "ux/view.h"
#include "ux/window.h"


namespace std {
template <>
struct hash<acorn::string> {
    size_t operator()( const acorn::string& k ) const {
        return k.hash();
    }
};
}


#endif
