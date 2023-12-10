//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../libacorn.h"


bytestream::bytestream() {
    offsetRead = 0;
    offsetWrite = 0;
}
bytestream::bytestream(int cb) : bytearray(cb) {
    offsetRead = 0;
    offsetWrite = 0;
}
bytestream::bytestream(bytearray& src) : bytearray(src) {
    offsetRead = 0;
    offsetWrite = 0;
}

void bytestream::setWriteOffset(size_t offset) {
    offsetWrite = offset;
}

bytearray bytestream::getWrittenBytes() {
    return bytearray(_p, (int)offsetWrite);
}


bool bytestream::eof() {
    return offsetRead >= _cb;
}
bool bytestream::hasMoreToRead() {
    return offsetRead < _cb;
}

bool bytestream::writeBytes(size_t cb, const void* bytes) {
    size_t cbFree = _cb - offsetWrite;
    if (cb > cbFree) {
        size_t new_cb = _cb + (cb-cbFree);
        new_cb = (new_cb+255) & 0xffffff00;
        uint8_t* new_data = (uint8_t*)malloc(new_cb);
        memcpy(new_data, _p, _cb);
        free(_p);
        _p = new_data;
        _cb = new_cb;
    }
    memcpy(_p+offsetWrite, bytes, cb);
    offsetWrite += cb;
    return true;
}

bool bytestream::readBytes(size_t cb, void* bytes) {
    size_t cbRead = MIN(cb, _cb - offsetRead);
    memcpy(bytes, _p+offsetRead, cbRead);
    offsetRead += cbRead;
    return cbRead==cb;
}


template <>
bool bytestream::read(bytearray& val) {
    uint32_t cb;
    if (!read(cb)) {
        return false;
    }
    val.resize(cb);
    return readBytes(cb, val.data());
}
template <>
bool bytestream::write(const bytearray& val) {
    uint32_t cb = val.size();
    if (!write(cb)) {
        return false;
    }
    return writeBytes(cb, val.data());
}
template <>
bool bytestream::read(string& val) {
    uint32_t cb;
    if (!readBytes(sizeof(cb), (uint8_t*)&cb)) {
        val._cb = 0;
        return false;
    }
    val.alloc(cb);
    return readBytes(cb, val._buf);
}
template <>
bool bytestream::write(const string& val) {
    uint32_t cb = val._cb;
    if (!writeBytes(sizeof(cb), (uint8_t*)&cb)) {
        return false;
    }
    return writeBytes(cb, (uint8_t*)val.start());
}

template <>
bool bytestream::read(val& v) {
    enum val::type type;
    if (!readBytes(sizeof(type), &type)) {
        return false;
    }
    v.setType(type);
    switch (type) {
        case val::EMPTY: return true;
        case val::INT32: return readBytes(4, &v._i32);
        case val::INT64: return readBytes(8, &v._i64);
        case val::UINT32: return readBytes(4, &v._u32);
        case val::UINT64: return readBytes(8, &v._u64);
        case val::FLOAT32: return readBytes(sizeof(float), &v._f32);
        case val::FLOAT64: return readBytes(sizeof(double), &v._f64);
        case val::STRING: return read(v._str);
        case val::MEASUREMENT: {
            float f;
            measurement::unit type;
            if (readBytes(4, &f) && readBytes(2, &type)) {
                v._measurement = measurement(f, type);
                return true;
            }
            return false;
        }
        case val::BYTEARRAY: return read(v._bytearray);
        case val::ARRAY: {
            uint32_t num = 0;
            if (!read(num)) {
                return false;
            }
            for (int i=0 ; i<num ; i++) {
                val elem;
                if (!read(elem)) {
                    return false;
                }
                v._vec->push_back(elem);
            }
            return true;
        }
        case val::MAP: {
            uint32_t num = 0;
            if (!read(num)) {
                return false;
            }
            for (int i=0 ; i<num ; i++) {
                string key;
                if (!read(key)) {
                    return false;
                }
                val value;
                if (!read(value)) {
                    return false;
                }
                v._map->insert(make_pair(key,value));
            }
            return true;
        }
        case val::OBJECT:
            assert(0); // unpossible!
            return false;
        case val::ERROR: {
            read(v._err->_code);
            read(v._err->_msg);
            return true;
        }
    }
    return false;
}
template <>
bool bytestream::write(const val& v) {
    if (!writeBytes(sizeof(v.type), &v.type)) {
        return false;
    }
    switch (v.type) {
    case val::EMPTY: return true;
    case val::INT32: return writeBytes(4, &v._i32);
    case val::INT64: return writeBytes(8, &v._i64);
    case val::UINT32: return writeBytes(4, &v._u32);
    case val::UINT64:return writeBytes(8, &v._u64);
    case val::FLOAT32:return writeBytes(sizeof(float), &v._f32);
    case val::FLOAT64:return writeBytes(sizeof(double), &v._f64);
    case val::STRING:return write(*v._str);
    case val::MEASUREMENT: {
        float f = v._measurement._unitVal;
        short type = v._measurement._unit;
        if (writeBytes(4, &f) && writeBytes(2, &type)) {
            return true;
        }
        return false;
    }
    case val::BYTEARRAY:return write(*v._bytearray);
    case val::ARRAY: {
        uint32_t num = (uint32_t)v._vec->size();
        if (!write(num)) {
            return false;
        }
        for (auto& it : *v._vec) {
            if (!write(it)) {
                return false;
            }
        }
        return true;
    }
    case val::MAP: {
        uint32_t num = (uint32_t)v._map->size();
        if (!write(num)) {
            return false;
        }
        for (auto& it : *v._map) {
            if (!write(it.first)) {
                return false;
            }
            const val& v2 = it.second;
            if (!write(v2)) {
                return false;
            }
        }
        return true;
    }
            
    case val::OBJECT:
        assert(0);
        return false;

    case val::ERROR:
        write(v._err->_code);
        write(v._err->_msg);
        return true;

    }
    return false;
}


