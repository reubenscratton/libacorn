//
//  acorn
//


class Window : public JSObj {
public:
    
    Window(const string& uiAsset);
    ~Window();
    
    DECLPROP_SP_RO(rootView, View);

    
    void show();
    sp<View> findView(const string& id);
    
protected:
    virtual bool applyProp(const string& key, val& v) override;
#ifdef __OBJC__
    NSWindow* _wnd;
#else
    void* _wnd;
#endif
    
private:
    Window();
};

template<>
inline void js_init<Window>(qjs::Context::Module& m) {
    m.class_<Window>("Window")
    .constructor<string>()
    .fun<&Window::show>("show")
    .fun<&Window::findView>("findView")
    .property<&Window::get_rootView>("rootView")
    ;
}

