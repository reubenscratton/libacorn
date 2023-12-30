//
//  libacorn
//
#include "../libacorn.h"
#include "nsview.h"


#ifdef __OBJC__
DECLARE_NSVIEW_TYPE(NSImageView, ImageView);
@implementation NSImageView_
#include "nsview.objc.inc"
@end
#endif


void ImageView::measure(rect& rect) {
    CGSize size = [((NSImageView_*)_nsview) intrinsicContentSize];
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

ImageView::ImageView() : View() {
}

void ImageView::createNSView() {
    NSImageView_* nsview = [[NSImageView_ alloc] init];
    nsview->_view = this;
    _nsview = nsview;
}

bool ImageView::applyProp(const string& key, val& v) {
    if (key == "image") {
        string imagePath = v.stringVal();
        resolvePath(imagePath);
        //NSString* path = [[NSBundle mainBundle].resourcePath stringByAppendingFormat:@"/%s", v.stringVal().c_str()];
        NSImage* image = [[NSImage alloc] initWithContentsOfFile:[NSString stringWithUTF8String:imagePath.c_str()]];
        [((NSImageView_*)_nsview) setImage:image];
        //[((_NSImageView*)_view) setImageScaling:NSImageScaleProportionallyDown];
        
    }
    return View::applyProp(key, v);
}

DECLARE_DYNCREATE(ImageView);
