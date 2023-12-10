//
//  Window.hpp
//  acorn
//
//  Created by Reuben on 06/11/2023.
//


#ifdef __OBJC__
@interface NSWindow_ : NSWindow <NSWindowDelegate>
@end
#endif

class Window : public ScriptObj {
public:
    static Window* create(const val& props);
    
    void show();
    
protected:
    virtual bool applyProp(const string& key, val& v) override;
#ifdef __OBJC__
    NSWindow_* _wnd;
#else
    void* _wnd;
#endif
    
private:
    Window();
    sp<View> _rootView;
};


