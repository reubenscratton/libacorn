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
/*- (BOOL)isFlipped {
    return YES;
}*/
@end


class Button : public View {
public:

    virtual size measure(const rect& r) override {
        
        CGSize size = [((_NSButton*)_nsview) intrinsicContentSize];
        size.width *= g_backingScaleFactor;
        size.height *= g_backingScaleFactor;
        _contentSize.width = size.width;
        _contentSize.height = size.height;

        struct size s;
        s.width = ceilf(_contentSize.width);
        s.height = ceilf(_contentSize.height);
        
        auto pad = getActualPadding(r);
        s.width += pad.left + pad.right;
        s.height += pad.top + pad.bottom;

        return s;
    };
    //virtual void layout(float4& rect) override {
        
    //}
    Button() : View() {
        createNSView();
    }
protected:
    

    void createNSView() override {
        _NSButton* nsview = [[_NSButton alloc] init];
        [nsview setBezelStyle:NSBezelStyleRounded];
        [nsview setButtonType:NSButtonTypeMomentaryPushIn];
        [nsview setKeyEquivalent:@"\r"];
        nsview->_view = this;
        _nsview = nsview;
    }

    virtual bool applyProp(const string& key, val& v) override {
        if (key == "text") {
            set_text(v.stringVal());
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
