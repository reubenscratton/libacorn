//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class Object {
public:
	Object();

    virtual ~Object();
    
	void* operator new(size_t sz);
    
    /**
     Return an estimated cost (in bytes) of this object in memory. Should be inclusive, i.e. include the cost of all sub objects.
     */
    virtual uint32_t getRamCost() const;
    
#ifdef DEBUG
    virtual string debugDescription();
#endif
    
    static std::map<string, Object* (*)()>* s_classRegister;

    template<typename T, class ...ARGS>
    class Registrar {
    private: static Object* createT(ARGS... args) {return new T(args...); }
    public:
        Registrar(const string& className) {
            if (!s_classRegister) {
                s_classRegister = new std::map<string, Object*(*)()>();
            }
            s_classRegister->insert(std::make_pair(className, reinterpret_cast<Object*(*)()>(&createT)));
        }
    };


    template<class ...ARGS>
    static Object* createByName(const string& className, ARGS... args) {
        const auto& constructor = s_classRegister->find(className);
        assert(constructor != s_classRegister->end()); // oops!
        auto real_constructor = reinterpret_cast<Object*(*)(ARGS...)>(constructor->second);
        return real_constructor(args...);
    }

    template<class ReturnType, class... Ts>
    ReturnType callFunction(void *function, const Ts&... args) {
    }
    
    friend class Task;
};

/**
 Declare a class as being dynamically creatable with Object::createByName("className",...).
 The class must have a public constructor that takes the same argument types as listed after
 the class name.
 */
#define STRINGIFY_(x) #x##_S
#define STRINGIFY(x) STRINGIFY_(x)
#define VA_ARGS(...) , ##__VA_ARGS__
#define DECLARE_DYNCREATE(X, ...) static Object::Registrar<X VA_ARGS(__VA_ARGS__)> s_classReg##X(STRINGIFY(X))



