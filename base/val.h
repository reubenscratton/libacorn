//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#define PARSEFLAG_JSON 1
#define PARSEFLAG_IN_IMPLICIT_ARRAY 2


class color {
public:
    float r,g,b,a;
    
    color();
    color(uint32_t rgba);
    color(float r, float g, float b, float a);
    
    uint32_t toRgbaUint32();
};

/**
 */
class measurement {
public:
    
    /** Returns the measurement value in pixels */
    float value() const;
    float valuePx(float containerLength) const;
    float valuePt(float containerLength) const;

    float _unitVal;
    enum unit {
        PX, // hardware pixels (backing store pixels in macos terms)
        PT, // points (aka logical pixels in macos, 'dp's in android).
        EM, // 1em = default text size
        PC, // %age of container
    } _unit;
    measurement(float v, unit unit) : _unitVal(v), _unit(unit) {}
    measurement(const measurement& m) : _unitVal(m._unitVal), _unit(m._unit) {}
    string toString() const;
};

/**
 * Interface for objects that serialize to and from Val.
 */
class ISerializeToVal {
public:
    virtual void fromVal(const class val& v) =0;
    virtual void toVal(val& v) =0; /* ideally this would be const */
};

/**
 * @ingroup base
 * @class val
 * A discriminated union of all elementary numeric types, a string, a blob,
 * an array of vals, and a compound map of string-val pairs.
 * Mainly used for serializing data and JSON.
 *
 * val is a value type, i.e. it does not extend Object and
 * constructors and operator=() all make copies rather than share references.
 * Be mindful of this when dealing with large amounts of data...
 *
 */

class val {
private:
    union {
        int32_t _i32;
        int64_t _i64;
        uint32_t _u32;
        uint64_t _u64;
        float _f32;
        double _f64;
        measurement _measurement;
        string* _str;
        bytearray* _bytearray;
        vector<val>* _vec;
        map<string, val>* _map;
        sp<Object>* _obj;
        error* _err;
    };
public:
    enum type {
        EMPTY,
        INT32,  INT64,
        UINT32, UINT64,
        FLOAT32, FLOAT64,
        MEASUREMENT,
        STRING,
        BYTEARRAY,
        ARRAY,
        MAP,
        OBJECT,
        ERROR,
    } type;
    
    static val empty();
    void setType(enum type newType);

    val();
    val(int32_t val);
    val(int64_t val);
    val(uint32_t val);
    val(uint64_t val);
    val(float val);
    val(double val);
    val(const bytearray& val);
    template<size_t N>
    val(const char(&p)[N]) : type(STRING), _str(new string(p)) { }
    val(const string& val);
    val(const measurement& val);
    val(const val& var);
    val(val&& var) noexcept;
    //val(class ISerializeToVal* val);
    val(const vector<val>& val);
    val(const vector<pair<string,val>>& vals);
    template <class T>
    val(const sp<T>& obj) : type(OBJECT) {
        _obj = new sp<T>(obj);
    }
    val(const error& val);
    val& operator=(const val& rhs);
    bool operator<(const val& rhs) const;
    ~val();

    // Type tests
    bool isEmpty() const;
    bool isNumeric() const;
    bool isFloatingPoint() const;
    bool isString() const;
    bool isByteArray() const;
    bool isMeasurement() const;
    bool isArray() const;
    bool isCompound() const;
    bool isObject() const;
    bool isError() const;

    // Comparison
    bool operator ==(float val) {return type==FLOAT32 && _f32==val; }
    bool operator ==(int val) {return (type==INT32 && _i32==val) || (type==UINT32 && _u32==val); }
    bool operator ==(const string& val) {return type==STRING && _str->compare(val)==0; }
    
    // Accessors (by value, some types are coerced)
    int intVal() const;
    int intVal(const string& name) const;
    string stringVal() const;
    string stringVal(const string& name) const;
    bool boolVal() const;
    bool boolVal(const string& name) const;
    float floatVal() const;
    float floatVal(const string& name) const;
    double doubleVal() const;
    double doubleVal(const string& name) const;
    measurement measurementVal() const;
    vector<string> stringArrayVal() const;
    vector<string> stringArrayVal(const string& name) const;
    template <typename T>
    sp<T> objectVal() const {
        static_assert(std::is_base_of<Object, T>::value, "type must be Object-derived");
        return (type==OBJECT && _obj != nullptr) ? _obj->get() : nullptr;
    }

    // Accessors (by reference, no coercion)
    string& stringRef() const;
    vector<val>& arrayRef() const;
    vector<val>& arrayRef(const string& name) const;
    bytearray& bytearrayRef() const;
    bytearray& bytearrayRef(const string& name) const;
    map<string, val>& compoundRef() const;
    map<string, val>& compoundRef(const string& name) const;
    error& errorRef() const;

    template <class T>
    vector<sp<T>> arrayOf(const string& name) const {
        static_assert(std::is_base_of<ISerializeToVal, T>::value, "T must implement ISerializeToVal");
        vector<sp<T>> vec;
        auto& a = arrayRef(name);
        for (auto& e : a) {
            T* t = new T();
            t->fromVal(e);
            vec.push_back(std::make_shared(t));
        }
        return vec;
    }
    template <typename T>
    T get() const {
        static_assert(std::is_base_of<ISerializeToVal, T>::value, "T must implement ISerializeToVal");
        T obj;
        obj.fromVal(*this);
        return obj;
    }
    template <typename T>
    T get(const string& key) const {
        static_assert(std::is_base_of<ISerializeToVal, T>::value, "T must implement ISerializeToVal");
        assert(type==MAP);
        auto val = _map->find(key);
        if (val == _map->end() || val->second.type != val::MAP) {
            assert(0);
        }
        T obj;
        obj.fromVal(val->second);
        return obj;
    }
    template <typename T>
    T* getObject(const string& key) const {
        static_assert(std::is_base_of<ISerializeToVal, T>::value, "T must implement ISerializeToVal");
        static_assert(std::is_base_of<Object, T>::value, "T must extend Object");
        assert(type==MAP);
        auto val = _map->find(key);
        if (val == _map->end() || val->second.type != val::MAP) {
            return NULL;
        }
        T* obj = new T();
        obj->fromVal(val->second);
        return obj;
    }

    // No const operator[] cos it appears impossible to avoid copying
    /*const val operator[](const char* key) const {
        assert(type==MAP);
        return *get(string(key));
    }
    const val operator[](const string& key) const {
         assert(type==MAP);
        return *get(key);
    }*/
    val& operator[](const string& key) {
        if (type==EMPTY) {
            type = MAP;
            _map = new map<string, val>();
        }
        assert(type==MAP);
        return (*((_map->insert(std::make_pair(key,val()))).first)).second;
    }
    auto mapBegin() const { return _map->begin(); }
    auto mapEnd() const { return _map->end(); }

    val& get(const string& keypath) const;
    bool hasVal(const string& key) const;
    void set(const string& key, const val& val);
    void set(const string& key, ISerializeToVal* object);
    void unset(const string& key);
    void clear();
    void appendVal(const val& v);
    
    // Helpful coercion
    color toColor() const;
    

    // Parsing from JSON and config files
    static val parse(const string& str, int flags);
    string toJson() const;
    static val parseNumber(const string& str, uint32_t& offset);

    
    void setByteArray(uint8_t* p, int cb) {
        setType(BYTEARRAY);
        _bytearray->append(p, cb);
    }
    void setString(uint8_t* p, int cb) {
        setType(STRING);
        _str->assign((char*)p, cb);
    }
    void setInt32(int32_t i) {
        setType(INT32);
        _i32 = i;
    }
    void setDouble(double d) {
        setType(FLOAT64);
        _f64 = d;
    }

    uint32_t getRamCost() const;
    
    friend class bytestream;
    friend class Task;
    
#if DEBUG
    const char* debugString() const;
#endif
    
    //JSValue toQJSValue(JSContext * ctx);
    friend qjs::js_traits<val>;
    //friend JSValue qjs::js_traits<val>::wrap(struct JSContext* ctx, const val& v) noexcept;
    
private:
    inline void assign(const val& src);
    static val parse(const string& str, uint32_t& o, int flags);
};

//template <>
//class Bitmap* val::getObject<Bitmap>(const string& key) const;

