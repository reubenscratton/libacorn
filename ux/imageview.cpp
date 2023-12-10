//
//  libacorn
//
#include "../libacorn.h"


@interface _NSImageView : NSImageView {
@public
    View* _view;
}
@end
@implementation _NSImageView
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

class ImageView : public View {
public:
    virtual void measure(rect& rect) override {
        CGSize size = [((_NSImageView*)_nsview) intrinsicContentSize];
        size.width *= g_backingScaleFactor;
        size.height *= g_backingScaleFactor;
        //_contentSizeMax.width = size.width;
        //_contentSizeMax.height = size.height;
        _contentSize.width = size.width;
        _contentSize.height = size.height;

        // NSImageScaleProportionallyDown
        if (_contentSize.height > rect.size.height) {
            _contentSize.width *=  rect.size.height / _contentSize.height;
            _contentSize.height = rect.size.height;
        }
        if (_contentSize.width > rect.size.width) {
            _contentSize.height *=  rect.size.width / _contentSize.width;
            _contentSize.width = rect.size.width;
        }
        
        rect.size.width = ceilf(_contentSize.width);
        rect.size.height = ceilf(_contentSize.height);
    };
    //virtual void layout(float4& rect) override {
        
    //}
    ImageView() : View() {
    }
protected:
    
    void createNSView() override {
        _NSImageView* nsview = [[_NSImageView alloc] init];
        nsview->_view = this;
        _nsview = nsview;
    }

    virtual bool applyProp(const string& key, val& v) override {
        if (key == "image") {
            NSString* path = [[NSBundle mainBundle].resourcePath stringByAppendingFormat:@"/%s", v.stringVal().c_str()];
            NSImage* image = [[NSImage alloc] initWithContentsOfFile:path];
            [((_NSImageView*)_nsview) setImage:image];
            //[((_NSImageView*)_view) setImageScaling:NSImageScaleProportionallyDown];
            
        }
        return View::applyProp(key, v);
    }
};

DECLARE_DYNCREATE(ImageView);
