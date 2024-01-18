
//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../libacorn.h"

color::color() : r(0),g(0),b(0),a(0) {
}
color::color(uint32_t rgba) {
    r = (rgba & 0xff) / 255.0f;
    g = ((rgba & 0xff00) >> 8) / 255.0f;
    b = ((rgba & 0xff0000) >> 16) / 255.0f;
    a = ((rgba & 0xff000000) >> 24) / 255.0f;
}

color::color(float ar, float ag, float ab, float aa) : r(ar), g(ag), b(ab), a(aa) {
}
uint32_t color::toRgbaUint32() {
    uint8_t ir = r * 255;
    uint8_t ig = g * 255;
    uint8_t ib = b * 255;
    uint8_t ia = a * 255;
    return ir | (ig<<8) | (ib<<16) | (ia<<24);
}
color val::toColor() const {
    if (type == EMPTY) {
        return color();
    }
    if (type == UINT32 || type==UINT64 || type==INT32 || type==INT64) {
        return color(_u32);
    }
    if (type == STRING) {
        static map<string, uint32_t> s_cssColors = {
            {"aliceblue", 0xfff0f8ff},
            {"antiquewhite", 0xfffaebd7},
            {"aqua", 0xff00ffff},
            {"aquamarine", 0xff7fffd4},
            {"azure", 0xfff0ffff},
            {"beige", 0xfff5f5dc},
            {"bisque", 0xffffe4c4},
            {"black", 0xff000000},
            {"blanchedalmond", 0xffffebcd},
            {"blue", 0xff0000ff},
            {"blueviolet", 0xff8a2be2},
            {"brown", 0xffa52a2a},
            {"burlywood", 0xffdeb887},
            {"cadetblue", 0xff5f9ea0},
            {"chartreuse", 0xff7fff00},
            {"chocolate", 0xffd2691e},
            {"coral", 0xffff7f50},
            {"cornflowerblue", 0xff6495ed},
            {"cornsilk", 0xfffff8dc},
            {"crimson", 0xffdc143c},
            {"cyan", 0xff00ffff},
            {"darkblue", 0xff00008b},
            {"darkcyan", 0xff008b8b},
            {"darkgoldenrod", 0xffb8860b},
            {"darkgray", 0xffa9a9a9},
            {"darkgreen", 0xff006400},
            {"darkgrey", 0xffa9a9a9},
            {"darkkhaki", 0xffbdb76b},
            {"darkmagenta", 0xff8b008b},
            {"darkolivegreen", 0xff556b2f},
            {"darkorange", 0xffff8c00},
            {"darkorchid", 0xff9932cc},
            {"darkred", 0xff8b0000},
            {"darksalmon", 0xffe9967a},
            {"darkseagreen", 0xff8fbc8f},
            {"darkslateblue", 0xff483d8b},
            {"darkslategray", 0xff2f4f4f},
            {"darkslategrey", 0xff2f4f4f},
            {"darkturquoise", 0xff00ced1},
            {"darkviolet", 0xff9400d3},
            {"deeppink", 0xffff1493},
            {"deepskyblue", 0xff00bfff},
            {"dimgray", 0xff696969},
            {"dimgrey", 0xff696969},
            {"dodgerblue", 0xff1e90ff},
            {"firebrick", 0xffb22222},
            {"floralwhite", 0xfffffaf0},
            {"forestgreen", 0xff228b22},
            {"fuchsia", 0xffff00ff},
            {"gainsboro", 0xffdcdcdc},
            {"ghostwhite", 0xfff8f8ff},
            {"gold", 0xffffd700},
            {"goldenrod", 0xffdaa520},
            {"gray", 0xff808080},
            {"green", 0xff008000},
            {"greenyellow", 0xffadff2f},
            {"grey", 0xff808080},
            {"honeydew", 0xfff0fff0},
            {"hotpink", 0xffff69b4},
            {"indianred", 0xffcd5c5c},
            {"indigo", 0xff4b0082},
            {"ivory", 0xfffffff0},
            {"khaki", 0xfff0e68c},
            {"lavender", 0xffe6e6fa},
            {"lavenderblush", 0xfffff0f5},
            {"lawngreen", 0xff7cfc00},
            {"lemonchiffon", 0xfffffacd},
            {"lightblue", 0xffadd8e6},
            {"lightcoral", 0xfff08080},
            {"lightcyan", 0xffe0ffff},
            {"lightgoldenrodyellow", 0xfffafad2},
            {"lightgray", 0xffd3d3d3},
            {"lightgreen", 0xff90ee90},
            {"lightgrey", 0xffd3d3d3},
            {"lightpink", 0xffffb6c1},
            {"lightsalmon", 0xffffa07a},
            {"lightseagreen", 0xff20b2aa},
            {"lightskyblue", 0xff87cefa},
            {"lightslategray", 0xff778899},
            {"lightslategrey", 0xff778899},
            {"lightsteelblue", 0xffb0c4de},
            {"lightyellow", 0xffffffe0},
            {"lime", 0xff00ff00},
            {"limegreen", 0xff32cd32},
            {"linen", 0xfffaf0e6},
            {"magenta", 0xffff00ff},
            {"maroon", 0xff800000},
            {"mediumaquamarine", 0xff66cdaa},
            {"mediumblue", 0xff0000cd},
            {"mediumorchid", 0xffba55d3},
            {"mediumpurple", 0xff9370db},
            {"mediumseagreen", 0xff3cb371},
            {"mediumslateblue", 0xff7b68ee},
            {"mediumspringgreen", 0xff00fa9a},
            {"mediumturquoise", 0xff48d1cc},
            {"mediumvioletred", 0xffc71585},
            {"midnightblue", 0xff191970},
            {"mintcream", 0xfff5fffa},
            {"mistyrose", 0xffffe4e1},
            {"moccasin", 0xffffe4b5},
            {"navajowhite", 0xffffdead},
            {"navy", 0xff000080},
            {"oldlace", 0xfffdf5e6},
            {"olive", 0xff808000},
            {"olivedrab", 0xff6b8e23},
            {"orange", 0xffffa500},
            {"orangered", 0xffff4500},
            {"orchid", 0xffda70d6},
            {"palegoldenrod", 0xffeee8aa},
            {"palegreen", 0xff98fb98},
            {"paleturquoise", 0xffafeeee},
            {"palevioletred", 0xffdb7093},
            {"papayawhip", 0xffffefd5},
            {"peachpuff", 0xffffdab9},
            {"peru", 0xffcd853f},
            {"pink", 0xffffc0cb},
            {"plum", 0xffdda0dd},
            {"powderblue", 0xffb0e0e6},
            {"purple", 0xff800080},
            {"red", 0xffff0000},
            {"rosybrown", 0xffbc8f8f},
            {"royalblue", 0xff4169e1},
            {"saddlebrown", 0xff8b4513},
            {"salmon", 0xfffa8072},
            {"sandybrown", 0xfff4a460},
            {"seagreen", 0xff2e8b57},
            {"seashell", 0xfffff5ee},
            {"sienna", 0xffa0522d},
            {"silver", 0xffc0c0c0},
            {"skyblue", 0xff87ceeb},
            {"slateblue", 0xff6a5acd},
            {"slategray", 0xff708090},
            {"slategrey", 0xff708090},
            {"snow", 0xfffffafa},
            {"springgreen", 0xff00ff7f},
            {"steelblue", 0xff4682b4},
            {"tan", 0xffd2b48c},
            {"teal", 0xff008080},
            {"thistle", 0xffd8bfd8},
            {"tomato", 0xffff6347},
            {"turquoise", 0xff40e0d0},
            {"violet", 0xffee82ee},
            {"wheat", 0xfff5deb3},
            {"white", 0xffffffff},
            {"whitesmoke", 0xfff5f5f5},
            {"yellow", 0xffffff00},
            {"yellowgreen", 0xff9acd32},
        };
        auto it = s_cssColors.find(*_str);
        if (it != s_cssColors.end()) {
            color c = color(it->second);
            auto t = c.r;
            c.r = c.b;
            c.b = t;
            return c;
        }
    }
    log_warn("Could not coerce val to color");
    return color();
}

extern float g_backingScaleFactor;
extern float g_systemFontSize;

float measurement::value() const {
    if (_unit==PX) return _unitVal;
    if (_unit==PT) return _unitVal * g_backingScaleFactor;
    if (_unit==EM) return _unitVal * g_systemFontSize;
    if (_unit==PC) return _unitVal/100;
    return _unitVal;
}
float measurement::valuePx(float containerLength) const {
    if (_unit==PX) return _unitVal;
    if (_unit==PT) return _unitVal * g_backingScaleFactor;
    if (_unit==EM) return _unitVal * g_systemFontSize;
    if (_unit==PC) return (_unitVal / 100) * containerLength;
    return _unitVal;
}
float measurement::valuePt(float containerLength) const {
    if (_unit==PX) return _unitVal / g_backingScaleFactor;
    if (_unit==PT) return _unitVal;
    if (_unit==EM) return _unitVal * g_systemFontSize;
    if (_unit==PC) return (_unitVal / 100) * containerLength;
    return _unitVal;
}

string measurement::toString() const {
    const char* szUnit;
    switch (_unit) {
        case PX: szUnit="px"; break;
        case PT: szUnit="pt"; break;
        case EM: szUnit="em"; break;
        case PC: szUnit="pc"; break;
        default: szUnit="??"; break;
    }
    return string::format("%f%s", _unitVal, szUnit);
}

/*
template <>
Bitmap* val::getObject<Bitmap>(const string& key) const {
    auto val = _map->find(key);
    if (val == _map->end() || val->second.type != val::MAP) {
        return NULL;
    }
    Bitmap* obj = Bitmap::create(0,0,0);
    obj->fromVal(val->second);
    return obj;
}*/

val val::empty() {
    return val();
}

val::val() : type(EMPTY) {
}
val::val(const val& var) : val(var.type) {
    assign(var);
}
val::val(val&& var) noexcept : type(var.type), _f64(var._f64) {
    var.type = EMPTY;
    var._f64 = 0;
}
val::val(int32_t val) : type(INT32), _i32(val) {
}
val::val(int64_t val) : type(INT64), _i64(val) {
}
val::val(uint32_t val) : type(UINT32), _u32(val) {
}
val::val(uint64_t val) : type(UINT64), _u64(val) {
}
val::val(float val) : type(FLOAT32), _f32(val) {
}
val::val(double val) : type(FLOAT64), _f64(val) {
}
//val::val(const char* val) : type(STRING), _str(new string(val)) {
//}
val::val(const string& val) : type(STRING), _str(new string(val)) {
}
val::val(const measurement& val) : type(MEASUREMENT), _measurement(val) {
}
val::val(const class bytearray& val) : type(BYTEARRAY), _bytearray(new class bytearray(val)) {
}
/*val::val(ISerializeToVal* val) : type(val?MAP:EMPTY) {
    if (val) {
        _map = new map<string, val>();
        val->toVal(*this);
    }
}*/

val::val(const vector<val>& v) : type(ARRAY) {
    _vec = new vector<val>(v.begin(), v.end());
}

val::val(const vector<pair<string,val>>& vals) : type(MAP) {
    _map = new map<string, val>();
    for (auto it : vals) {
        _map->insert(it);
    }
}
val::val(const error& val) : type(ERROR) {
    _err = new error(val);
}


val::~val() {
    if (type == STRING) {
        delete _str;
    } else if (type == BYTEARRAY) {
        delete _bytearray;
    } else if (type == ARRAY) {
        delete _vec;
    } else if (type == MAP) {
        delete _map;
    } else if (type == OBJECT) {
        _obj->reset();
    } else if (type == ERROR) {
        delete _err;
    }
}
void val::setType(enum type newType) {
    if (type == newType) return;
    
    // Handle pointer type changes
    if (type == STRING) {
        delete _str;
        _str = NULL;
    } else if (type == BYTEARRAY) {
        delete _bytearray;
        _bytearray = NULL;
    } else if (type == ARRAY) {
        delete _vec;
        _vec = NULL;
    } else if (type == MAP) {
        delete _map;
        _map = NULL;
    } else if (type == OBJECT) {
        delete _obj;
        _obj = NULL;
    } else if (type == ERROR) {
        delete _err;
        _err = NULL;
    }
    
    
    if (newType == STRING) {
        _str = new string();
    } else if (newType == BYTEARRAY) {
        _bytearray = new class bytearray();
    } else if (newType == ARRAY) {
        _vec = new vector<val>();
    } else if (newType == MAP) {
        _map = new map<string,val>();
    } else if (newType == ERROR) {
        _err = new error();
    }
    
    type = newType;

}
void val::assign(const val& src) {
    setType(EMPTY);
    switch (src.type) {
        case EMPTY: break;
        case INT32: _i32 = src._i32; break;
        case INT64: _i64 = src._i64; break;
        case UINT32: _u32 = src._u32; break;
        case UINT64: _u64 = src._u64; break;
        case FLOAT32: _f32 = src._f32; break;
        case FLOAT64: _f64 = src._f64; break;
        case STRING: _str = new string(*src._str); break;
        case MEASUREMENT: _measurement = src._measurement; break;
        case BYTEARRAY: _bytearray = new class bytearray(*src._bytearray); break;
        case ARRAY: _vec = new vector<val>(src._vec->begin(), src._vec->end()); break;
        case MAP: _map = new map<string,val>(*src._map); break;
        case OBJECT: _obj = src._obj; break;
        case ERROR: _err = new error(*src._err); break;
    }
    type = src.type;
}
val& val::operator=(const val& var) {
    assign(var);
    return *this;
}
bool val::operator<(const val& rhs) const {
    assert(type == rhs.type); // no cross-type comparison allowed
    switch (type) {
        case EMPTY: return false;
        case INT32: return _i32 < rhs._i32;
        case INT64: return _i64 < rhs._i64;
        case UINT32: return _u32 < rhs._u32;
        case UINT64: return _u64 < rhs._u64;
        case FLOAT32: return _f32 < rhs._f32;
        case FLOAT64: return _f64 < rhs._f64;
        case STRING: return *_str < *(rhs._str);
        case MEASUREMENT: return _measurement.value() < (rhs._measurement.value());
        case BYTEARRAY: {
            size_t cb = MIN(_bytearray->size(), rhs._bytearray->size());
            int cv = memcmp(_bytearray->data(), rhs._bytearray->data(), cb);
            if (cv != 0) return cv<0;
            return _bytearray->size() < rhs._bytearray->size();
        }
        case ARRAY: {
            for (int i=0 ; i<MIN(_vec->size(), rhs._vec->size()) ; i++) {
                if (_vec[i] < rhs._vec[i]) {
                    return true;
                }
            }
            return _vec->size() < rhs._vec->size();
        }
        case MAP: assert(0); break; // how would this work??
        case OBJECT: assert(0); break; // how would this work??
        case ERROR: return *_err < *rhs._err;
    }

}


int val::intVal() const {
    switch (type) {
        case EMPTY: return 0;
        case INT32: return _i32;
        case INT64: return (int)_i64;
        case UINT32: return _u32;
        case UINT64: return (int)_u64;
        case FLOAT32: return (int)_f32;
        case FLOAT64: return (int)_f64;
        case STRING: return atoi(_str->c_str());
        case MEASUREMENT: return (int)_measurement.value();
        default: break;
    }
    log_warn("intVal() called on non-numeric Val");
    return 0;
}
bool val::boolVal() const {
    switch (type) {
        case EMPTY: return false;
        case INT32: return _i32!=0;
        case INT64: return _i64!=0;
        case UINT32: return _u32!=0;
        case UINT64: return _u64!=0;
        case FLOAT32: return _f32!=0.f;
        case FLOAT64: return _f64!=0.;
        case STRING: {
            if (_str->compare("false")==0) return false;
            else if (_str->compare("true")==0) return true;
            break;
        }
        case MEASUREMENT: return _measurement.value()!=0.f;
        default: break;
    }
    log_warn("boolVal() type coerce failed");
    return false;
}

int val::intVal(const string& name) const {
    return get(name).intVal();
}
bool val::boolVal(const string& name) const {
    return get(name).boolVal();
}

static void skipWhitespaceAndComments(const string& str, uint32_t& it) {
    bool newLine = false;
    while (it != str.end()) {
        char32_t ch = str.peekChar(it);
        if (ch=='\r' || ch=='\n') {
            newLine = true;
            str.readChar(it);
        } else if (ch==' ' || ch=='\t') {
            str.readChar(it);
        } else {
            if (newLine) {
                if (ch=='#' || (ch=='/' && str.skipString(it, "//"))) {
                    while (it != str.end()) {
                        ch = str.peekChar(it);
                        if (ch=='\r' || ch=='\n') {
                            break;
                        } else {
                            str.readChar(it);
                        }
                    }
                    continue;
                }
            }
            break;
        }
    }
}

static void skipSpacesAndTabs(const string& str, uint32_t& o) {
    while(o<str.lengthInBytes()) {
        char32_t ch = str.peekChar(o);
        if (ch==' ' || ch=='\t') {
            str.readChar(o);
        } else {
            break;
        }
    }
}

float val::floatVal() const {
    switch (type) {
        case EMPTY: return 0.f;
        case INT32: return (float)_i32;
        case INT64: return (float)_i64;
        case UINT32: return (float)_u32;
        case UINT64: return (float)_u64;
        case FLOAT32: return _f32;
        case FLOAT64: return (float)_f64;
        case MEASUREMENT: return _measurement.value();
        case STRING: return atof(_str->c_str());
        default: break;
    }
    log_warn("floatVal() called on non-numeric Val");
    return 0.f;
}
float val::floatVal(const string& name) const {
    return get(name).floatVal();
}

double val::doubleVal() const {
    switch (type) {
        case EMPTY: return 0;
        case INT32: return (double)_i32;
        case INT64: return (double)_i64;
        case UINT32: return (double)_u32;
        case UINT64: return (double)_u64;
        case FLOAT32: return _f32;
        case FLOAT64: return _f64;
        case MEASUREMENT: _measurement.value();
        case STRING: return atof(_str->c_str());
        default: break;
    }
    log_warn("doubleVal() called on non-numeric Val");
    return 0.;
}
double val::doubleVal(const string& name) const {
    return get(name).doubleVal();
}

measurement val::measurementVal() const {
    if (type == MEASUREMENT) {
        return _measurement;
    }
    if (isNumeric()) {
        return measurement(floatVal(), measurement::PT);
    }
    if (!isEmpty()) {
        log_warn("measurement() type coerce failed");
    }
    return measurement(0,measurement::PX);
}


string val::stringVal() const {
    switch (type) {
        case EMPTY: return "";
        case INT32: return string::format("%d", _i32);
        case INT64: return string::format("%ld", _i64);
        case UINT32: return string::format("%u", _u32);
        case UINT64: return string::format("%lu", _u64);
        case FLOAT32: return string::format("%f", (double)_f32);
        case FLOAT64: return string::format("%f", _f64);
        case STRING: return *_str;
        default: break;
    }
    if (type==ARRAY) {
        // todo: might be useful to concat the element stringVals...
    }
    log_warn("stringVal() called on non-stringable StyleValue");
    return "";
}
string val::stringVal(const string& name) const {
    return get(name).stringVal();
}
vector<string> val::stringArrayVal() const {
    vector<string> vec;
    if (isArray()) {
        auto& a = arrayRef();
        for (auto e : a) {
            vec.push_back(e.stringVal());
        }
    } else if (!isEmpty()) {
        vec.push_back(stringVal());
    }
    return vec;
}

vector<string> val::stringArrayVal(const string& name) const {
    return get(name).stringArrayVal();
}

bool val::isEmpty() const {
    return type == EMPTY;
}

bool val::isNumeric() const {
    return type == INT32 || type == INT64
        || type == UINT32 || type == UINT64
        || type == FLOAT32 || type == FLOAT64
        || type == MEASUREMENT;
}
bool val::isFloatingPoint() const {
    return type == FLOAT32 || type == FLOAT64;
}
bool val::isArray() const {
    return type == ARRAY;
}
bool val::isString() const {
    return type == STRING;
}
bool val::isByteArray() const {
    return type == BYTEARRAY;
}
bool val::isMeasurement() const {
    return type == MEASUREMENT;
}
bool val::isCompound() const {
    return type == MAP;
}
bool val::isObject() const {
    return type == OBJECT;
}
bool val::isError() const {
    return type == ERROR;
}

static vector<val> s_emptyVec;
static map<string, val> s_emptyCompound;
static string s_emptyStr;
static error s_defaultError = error::none();

string& val::stringRef() const {
    if (type == STRING) {
        return *_str;
    }
    log_warn("stringRef() called on non-string type");
    return s_emptyStr;
}

vector<val>& val::arrayRef() const {
    if (type==ARRAY) {
        return *_vec;
    }
    log_warn("arrayRef() called on non-array type");
    return s_emptyVec;
}

map<string, val>& val::compoundRef() const {
    if (type==MAP) {
        return *_map;
    }
    log_warn("compoundRef() called on non-compound type");
    return s_emptyCompound;
}

error& val::errorRef() const {
    if (type==ERROR) {
        return *_err;
    }
    log_warn("errorRef() called on incompatible type");
    return s_defaultError;
}

void val::appendVal(const val& v) {
    if (type==EMPTY) {
        setType(ARRAY);
    }
    assert(type==ARRAY);
    _vec->push_back(std::move(v));
}

vector<val>& val::arrayRef(const string& name) const {
    return get(name).arrayRef();
}
map<string, val>& val::compoundRef(const string& name) const {
    return get(name).compoundRef();
}

bytearray& val::bytearrayRef() const {
    assert (type==BYTEARRAY); // todo: autoconvert single-value types like string etc
    return *_bytearray;
}

bytearray& val::bytearrayRef(const string& name) const {
    return get(name).bytearrayRef();
}


bool val::hasVal(const string& name) const {
    if (type!=MAP) {
        return false;
    }
    return _map->find(name)!=_map->end();
}

static val s_emptyVar;

val& val::get(const string& key) const {
    if (type==EMPTY) {
        return s_emptyVar;
    }
    assert(type==MAP);
    auto val = _map->find(key);
    if (val == _map->end()) {
        return s_emptyVar;
    }
    return val->second;
}
void val::set(const string& key, const val& val) {
    if (type==EMPTY) {
        setType(MAP);
    }
    assert(type==MAP);
    auto it = _map->emplace(std::move(key), val);
    if (!it.second) {
        it.first->second = val;
    }
}

void val::set(const string& key, ISerializeToVal* ival) {
    if (type==EMPTY) {
        setType(MAP);
    }
    assert(type==MAP);
    if (!ival) {
        return;
    }
    val v;
    ival->toVal(v);
    (*_map)[key] = v;
}

void val::unset(const string& key) {
    if (type==EMPTY) {
        return;
    }
    assert(type==MAP);
    _map->erase(std::move(key));
}

void val::clear() {
    setType(EMPTY);
    //_map->clear();
}


#if DEBUG
const char* val::debugString() const {
    string j = toJson();
    char* foo = (char*)malloc(j.lengthInBytes()+1);
    memcpy(foo, j.c_str(), j.lengthInBytes()+1);
    return foo;
}
#endif

uint32_t val::getRamCost() const {
    uint32_t cost = sizeof(val);
    switch (type) {
        case EMPTY: break;
        case INT32: cost += 4; break;
        case UINT32: cost += 4; break;
        case INT64: cost += 8; break;
        case UINT64: cost += 8; break;
        case FLOAT32: cost += 4; break;
        case FLOAT64: cost += 8; break;
        case STRING: cost += _str->lengthInBytes(); break;
        case BYTEARRAY: cost += _bytearray->size(); break;
        case OBJECT: cost += (*_obj)->getRamCost(); break;
        case ARRAY: {
            if (_vec) {
                for (auto it=_vec->begin() ; it != _vec->end() ; it++) {
                    cost += it->getRamCost();
                }
            }
            break;
        }
        case MAP: {
            if (_map) {
                for (auto it=_map->begin() ; it != _map->end() ; it++) {
                    cost += it->first.lengthInBytes();
                    cost += it->second.getRamCost();
                }
            }
            break;
        }
        default:
            break;
    }
    return cost;
}

string val::toJson() const {
    switch (type) {
        case EMPTY: return "";
        case INT32: return string::format("%d", _i32);
        case UINT32: return string::format("%u", _u32);
        case INT64: return string::format("%ld", _i64);
        case UINT64: return string::format("%lu", _u64);
        case FLOAT32: return string::format("%f", (double)_f32);
        case FLOAT64: return string::format("%f", _f64);
        case STRING: return string::format("\"%s\"", _str->c_str());
        case MEASUREMENT: return string::format("\"%s\"", _measurement.toString().c_str());
        case BYTEARRAY: {
            log_warn("TODO! bytearray json representation needed");
            break;
        }
        case ARRAY: {
            string s = "[";
            if (_vec) {
                for (auto it=_vec->begin() ; it != _vec->end() ; it++) {
                    if (it != _vec->begin()) s.append(",");
                    auto& elem = it;
                    s.append(elem->toJson());
                }
            }
            s.append("]");
            return s;
        
        }
        case MAP: {
            string s = "{";
            if (_map) {
                for (auto it=_map->begin() ; it != _map->end() ; it++) {
                    if (it != _map->begin()) s.append(",");
                    auto& field = it;
                    s.append(string::format("\"%s\":", field->first.c_str()));
                    s.append(field->second.toJson());
                }
            }
            s.append("}");
            return s;
        }
        default:
            assert(0); // Non-JSONable type
            break;
    }
    return "";
}


static string parseString(const string& s, uint32_t& o) {
    string str;
    bool quoted = false, endquote = false;
    char32_t ch = s.peekChar(o);
    quoted = (ch == '\"');
    if (quoted) {
        s.readChar(o);
    }
    while (o < s.lengthInBytes()) {
        ch = s.peekChar(o);
        if (!quoted) {
            if (ch=='\n' || ch=='\r' || ch==' ' || ch == ',' || ch==':' || ch=='}') {
                break;
            }
        }
        ch = s.readChar(o);
        if (ch==0) {
            break;
        }
        if (quoted && ch=='\"') {
            endquote = true;
            break;
        }
        // Escapes
        if (ch=='\\') {
            // charStart++;
            ch = s.readChar(o);
            bool legalEscape = (ch=='\"' || ch=='\'' || ch=='\\' || ch=='n');
            if (!quoted) {
                legalEscape |= (ch == ',' || ch==':' || ch=='}');
            }
            if (!legalEscape) {
                log_warn("Illegal escape \'\\%c\'", ch);
                continue;
            }
            if (ch == 'n') ch='\n';
        }
        //str.append(string(charStart, it.current()));
        str.append(ch);
    }
    if (quoted && !endquote) {
        log_warn("Expected closing \"");
    }
    
    return str;
}


// I cannot get strtol or stoi to work on Emscripten... they return garbage. Hence
// rolling my own...
val val::parseNumber(const string& str, uint32_t& o) {
    val val = uint64_t(0ULL);
    uint32_t ostart = o;
    
    // Handle leading '+' and '-'
    bool neg = false;
    char32_t ch = str.peekChar(o);
    while (ch=='+' || ch=='-') {
        if (ch=='-') {
            neg=!neg;
        }
        str.readChar(o);
        str.skipWhitespace(o);
        ch = str.peekChar(o);
    }
    
    int base = 10;
    
    // Hex?
    if (str.skipChar(o, '0')) {
        if (str.skipChar(o, 'x')) {
            base = 16;
        }
    }
    
    bool isFloat = false;
    double fraction=0;
    for (;;) {
        char32_t ch = str.peekChar(o);
        if ((ch>='0' && ch<='9') || (base==16 && ((ch>='a'&&ch<='f')||(ch>='A'&&ch<='F')))) {
            int digit = (ch-'0');
            if (ch>='a' && ch<='f') digit = (ch+10-'a');
            else if (ch>='A' && ch<='F') digit = (ch+10-'A');
            if (fraction==0) {
                val._i64 = val._i64*base + digit;
            } else {
                fraction/=base;
                val._f64 += digit*fraction;
            }
            str.readChar(o);
        } else if (ch=='.') {
            isFloat = true;
            fraction = 1;
            val._f64 = val._i64;
            str.readChar(o);
        }
        
        else {
            break;
        }
    }
    if (neg) {
        if (isFloat) val._f64 = -val._f64;
        else val._i64 = -val._i64;
    }
    
    // Choose appropriate size, 64 or 32
    if (isFloat) {
        if (val._f64 < FLT_MIN || val._f64 > FLT_MAX) {
            val.type = val::FLOAT64;
        } else {
            val.type = val::FLOAT32;
            val._f32 = (float)val._f64;
        }
    } else {
        if (val._i64 < INT_MIN || val._i64 > INT_MAX) {
            val.type = val::INT64;
        } else {
            val.type = val::INT32;
            val._i32 = (int32_t)val._i64;
        }
    }

    
    // Handle measurement suffixes
    if (str.skipChar(o, '%')) {
        val._measurement = measurement(val.floatVal(), measurement::PC);
        val.type=val::MEASUREMENT;
    } else if (str.skipString(o, "pt")) {
        val._measurement = measurement(val.floatVal(), measurement::PT);
        val.type=val::MEASUREMENT;
    } else if (str.skipString(o, "em")) {
        val._measurement = measurement(val.floatVal(), measurement::EM);
        val.type=val::MEASUREMENT;
    } else if (str.skipString(o, "px")) {
        val._measurement = measurement(val.floatVal(), measurement::PX);
        val.type=val::MEASUREMENT;
    }

    // Handle kilo/mega/gigabyte numbers
    else {
        uint64_t mul = 0;
        if (str.skipString(o, "KB")) {
            mul = 1024;
        }
        else if (str.skipString(o, "MB")) {
            mul = 1024 * 1024;
        }
        else if (str.skipString(o, "GB")) {
            mul = 1024 * 1024 * 1024;
        }
        if (mul) {
            if (val.type==val::INT32) val._i32 *= mul;
            else if (val.type==val::INT64) val._i64 *= mul;
            else if (val.type==val::FLOAT32) val._f32 *= mul;
            else if (val.type==val::FLOAT64) val._f64 *= mul;
        }
    }
    
    return val;
}

val val::parse(const string& str, int flags) {
    uint32_t it = 0;
    return parse(str, it, flags);
}

val val::parse(const string& str, uint32_t& it, int flags) {
    if (it>=str.end()) {
        return val();
    }

    val v;
    
    skipWhitespaceAndComments(str, it);

    // Parse a compound value
    char32_t ch = str.peekChar(it);
    if (ch == '{') {
        str.readChar(it);
        v.type = MAP;
        v._map = new map<string,val>();
        skipWhitespaceAndComments(str, it);
        while (str.peekChar(it) != '}' && it<str.lengthInBytes()) {
            skipWhitespaceAndComments(str, it);
            string fieldName = parseString(str, it);
            // log_dbg(fieldName.c_str());
            if (!fieldName) {
                log_error("Invalid json: field name expected");
                return v;
            }
            skipWhitespaceAndComments(str, it);
            if (str.readChar(it) != ':') {
                log_error("Invalid json: ':' expected");
                return v;
            }
            val fieldValue = parse(str, it, flags);
            
            v._map->operator[](std::move(fieldName)) = std::move(fieldValue);
            /*auto tt = val._map->emplace(std::move(fieldName), std::move(fieldValue));
            if (!tt.second) {
                tt.first.first = val;
            }*/
            
            skipWhitespaceAndComments(str, it);

            // JSON object fields are separated by commas
            if (flags & PARSEFLAG_JSON) {
                if (str.skipChar(it, ',')) {
                    str.skipWhitespace(it);
                }
            }
            //val.set(fieldName, std::move(fieldValue));
            /*it.skipWhitespace();
            char32_t sep = it.peek();
            if (sep == ',') {
                it.next();
            } else {
                if (sep != '}') {
                    log_error("Invalid json: ',' or object end expected");
                }
                break;
            }*/
        }
        str.readChar(it);
    }
    
    // Parse an explicit array
    else if (ch == '[') {
        str.readChar(it);
        v.type = ARRAY;
        v._vec = new vector<val>();
        skipWhitespaceAndComments(str, it);
        while (str.peekChar(it) != ']' && it<str.lengthInBytes()) {
            val element = val::parse(str, it, flags);
            v._vec->push_back(std::move(element));
            skipWhitespaceAndComments(str, it);
            char32_t sep = str.peekChar(it);
            if (sep == ',') {
                str.readChar(it);
            } else {
                if (sep != ']') {
                    log_error("Invalid array: ',' or ']' expected");
                }
                break;
            }
        }
        str.skipChar(it, ']');
    }
    
    // Scalar values
    else if ((ch>='0' && ch<='9') || ch=='-' || ch=='+' || ch=='.') {
        v = parseNumber(str, it);
    } else if (ch == 'n' && str.skipString(it, "null")) {
        v.setType(EMPTY); // should we have null? is it really meaningful?
    } else if (ch == 't' && str.skipString(it, "true")) {
        v.setInt32(1);
    } else if (ch == 'f' && str.skipString(it, "false")) {
        v.setInt32(0);
    } else {
        v = parseString(str, it);
    }

    // Handle implicit arrays, i.e. no square brackets and elements are delimited by spaces or commas.
    // Most often used for compound measurements (e.g. padding) or CSS styles
    if (!(flags&PARSEFLAG_JSON) && !(flags&PARSEFLAG_IN_IMPLICIT_ARRAY)) {
        for (;;) {
            skipSpacesAndTabs(str, it);
            char32_t ch = str.peekChar(it);
            if (ch=='\0' || ch==']' || ch=='}' || ch==',' || ch=='\r' || ch=='\n') {
                break;
            }
            if (ch == ',') it++;
            if (!v.isArray()) {
                val firstElem(std::move(v));
                v.setType(ARRAY);
                v._vec->push_back(std::move(firstElem));
            }
            val elem = val::parse(str, it, flags|PARSEFLAG_IN_IMPLICIT_ARRAY);
            v._vec->push_back(std::move(elem));
        }
    }
    

    
    return v;
}


