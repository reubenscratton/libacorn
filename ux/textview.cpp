//
//  libacorn
//
#include "../libacorn.h"

extern float g_backingScaleFactor;

@interface _NSTextView : NSTextView {
@public
    View* _view;
}
@end
@implementation _NSTextView
- (id)init {
    self = [super init];
    self.editable = NO;
    self.selectable = NO;
    self.backgroundColor = nil;
    self.textContainer.maximumNumberOfLines = 1;
    self.textContainer.lineBreakMode = NSLineBreakByTruncatingTail;
    return self;
}
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

/*- (void)drawRect:(NSRect)dirtyRect {
    CGContextRef c = [NSGraphicsContext currentContext].CGContext;
    CGContextTranslateCTM(c, 0, 12);
    [super drawRect:dirtyRect];
    CGContextTranslateCTM(c, 0, -12);
}*/
@end



class TextView : public View {
public:
    virtual void measure(rect& r) override {
        _NSTextView* ns = ((_NSTextView*)_nsview);
    
        ns.textContainer.size = NSMakeSize(r.size.width, r.size.height);
        [ns.layoutManager glyphRangeForTextContainer:ns.textContainer];
        [ns.layoutManager ensureLayoutForTextContainer:ns.textContainer];
        NSRect used = [ns.layoutManager usedRectForTextContainer:ns.textContainer];
        _contentSize.width = used.size.width * g_backingScaleFactor;
        _contentSize.height = used.size.height * g_backingScaleFactor;

        r.size.width = ceilf(_contentSize.width);
        r.size.height = ceilf(_contentSize.height);
    };
    virtual void layout(rect& r) override {
        View::layout(r);
        // ugly hack to center vertically, something NSTextView can't do by default
        _NSTextView* ns = (_NSTextView*)_nsview;
        float vdiff = (r.size.height-_contentSize.height) / g_backingScaleFactor;
        [ns setTextContainerInset:NSMakeSize(0,vdiff/2)];
    }
    TextView() : View() {
    }
protected:
    
    void onGravityChanged() override {
        _NSTextView* ns = (_NSTextView*)_nsview;
        if (_gravityH<=0.25f) {
            ns.alignment =  NSTextAlignmentLeft;
        } else if (_gravityH <= 0.75f) {
            ns.alignment = NSTextAlignmentCenter;
        } else {
            ns.alignment = NSTextAlignmentRight;
        }

    }
    void createNSView() override {
        _NSTextView* nsview = [[_NSTextView alloc] init];
        nsview->_view = this;
        _nsview = nsview;
    }

    virtual bool applyProp(const string& key, val& v) override {
        if (key == "text") {
            [((_NSTextView*)_nsview) setString:[NSString stringWithUTF8String:v.stringVal().c_str()]];
            return true;
        }
        if (key == "text-size" || key == "font-size") {
            [((_NSTextView*)_nsview) setFont:[NSFont systemFontOfSize:v.floatVal()]];
            return true;
        }
        if (key == "text-align") {
            return View::applyProp("gravity", v);
        }
        return View::applyProp(key, v);
    }
};

DECLARE_DYNCREATE(TextView);
