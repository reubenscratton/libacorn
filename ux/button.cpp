//
//  libacorn
//
#include "../libacorn.h"


@interface _NSButton : NSButton {
@public
    View* _view;
}
@end
@implementation _NSButton
- (BOOL)autoresizesSubviews {
    return NO;
}
- (BOOL)isFlipped {
    return YES;
}
- (void)layout {
    if (_view->_isRoot) {
        rect r = {0,0,(float)self.frame.size.width,(float)self.frame.size.height};
        _view->layout(r);
    }
}
@end

extern float g_backingScaleFactor;
extern bool resolvePath(string& path);


class Button : public View {
public:

    virtual void measure(rect& rect) override {
        CGSize size = [((_NSButton*)_nsview) intrinsicContentSize];
        size.width *= g_backingScaleFactor;
        size.height *= g_backingScaleFactor;
        _contentSize.width = size.width;
        _contentSize.height = size.height;

        rect.size.width = ceilf(_contentSize.width);
        rect.size.height = ceilf(_contentSize.height);
    };
    //virtual void layout(float4& rect) override {
        
    //}
    Button() : View() {
        createNSView();
    }
protected:
    

    void createNSView() override {
        _NSButton* nsview = [[_NSButton alloc] init];
        nsview->_view = this;
        _nsview = nsview;
    }

    virtual bool applyProp(const string& key, val& v) override {
        if (key == "text") {
            string text = v.stringVal();
            [((_NSButton*)_nsview) setTitle:str2ns(text)];
            return true;
        }
        return View::applyProp(key, v);
    }

    string get_text() {
        string t = ns2str([((_NSButton*)_nsview) title]);
        return t;
    }
    void set_text(const string& t) {
        [((_NSButton*)_nsview) setTitle:str2ns(t)];
    }
    
    friend void js_init<Button>(qjs::Context::Module&);

};

template <>
void js_init<Button>(qjs::Context::Module& m) {
    m.class_<Button>("Button")
            .base<View>()
            .constructor<>()
            PROPERTY(Button, text)
    ;

}

DECLARE_DYNCREATE(Button);
