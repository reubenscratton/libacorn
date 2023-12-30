//
//  libacorn
//
#include "../libacorn.h"
#include "nsview.h"

#ifdef __OBJC__

DECLARE_NSVIEW_TYPE(NSTextView, TextView);

@implementation NSTextView_
#include "nsview.objc.inc"
- (id)init {
    self = [super init];
    self.editable = NO;
    self.selectable = NO;
    self.backgroundColor = nil;
    self.textContainer.maximumNumberOfLines = 1;
    self.textContainer.lineBreakMode = NSLineBreakByTruncatingTail;
    return self;
}
@end

#endif



void applyPosition(float& existing, val& pos, float parentLength, float ownLength);

void TextView::measure(rect& r) {
    auto c = r.size;
    
    NSTextView_* ns = ((NSTextView_*)_nsview);

    ns.textContainer.size = NSMakeSize(r.size.width, r.size.height);
    [ns.layoutManager glyphRangeForTextContainer:ns.textContainer];
    [ns.layoutManager ensureLayoutForTextContainer:ns.textContainer];
    NSRect used = [ns.layoutManager usedRectForTextContainer:ns.textContainer];
    _contentSize.width = used.size.width * g_backingScaleFactor;
    _contentSize.height = used.size.height * g_backingScaleFactor;

    r.size.width = ceilf(_contentSize.width);
    r.size.height = ceilf(_contentSize.height);
    
    applyPosition(r.origin.x, _x, c.width, r.size.width);
    applyPosition(r.origin.y, _y, c.height, r.size.height);
};

void TextView::layout(rect& r) {
    View::layout(r);
    // ugly hack to center vertically, something NSTextView can't do by default
    NSTextView_* ns = (NSTextView_*)_nsview;
    float vdiff = (r.size.height-_contentSize.height) / g_backingScaleFactor;
    [ns setTextContainerInset:NSMakeSize(0,vdiff/2)];
}
TextView::TextView() : View() {
}

void TextView::onGravityChanged() {
    NSTextView_* ns = (NSTextView_*)_nsview;
    if (_gravityH<=0.25f) {
        ns.alignment =  NSTextAlignmentLeft;
    } else if (_gravityH <= 0.75f) {
        ns.alignment = NSTextAlignmentCenter;
    } else {
        ns.alignment = NSTextAlignmentRight;
    }
}
void TextView::createNSView() {
    NSTextView_* nsview = [[NSTextView_ alloc] init];
    nsview->_view = this;
    _nsview = nsview;
}

bool TextView::applyProp(const string& key, val& v) {
    if (key == "text") {
        [((NSTextView_*)_nsview) setString:[NSString stringWithUTF8String:v.stringVal().c_str()]];
        return true;
    }
    if (key == "text-size" || key == "font-size") {
        [((NSTextView_*)_nsview) setFont:[NSFont systemFontOfSize:v.floatVal()]];
        return true;
    }
    if (key == "text-align") {
        return View::applyProp("gravity", v);
    }
    return View::applyProp(key, v);
}


DECLARE_DYNCREATE(TextView);
