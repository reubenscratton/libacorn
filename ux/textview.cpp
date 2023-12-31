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
    self.drawsBackground = NO;
    self.textContainer.maximumNumberOfLines = 1;
    self.textContainer.lineBreakMode = NSLineBreakByTruncatingTail;
    self.verticallyResizable = NO;
    return self;
}
@end

#endif



void applyPosition(float& existing, val& pos, float parentLength, float ownLength);

size TextView::measure(const rect& r) {
    
    NSTextView_* ns = ((NSTextView_*)_nsview);

    ns.textContainer.size = NSMakeSize(r.size.width, r.size.height);
    [ns.layoutManager glyphRangeForTextContainer:ns.textContainer];
    [ns.layoutManager ensureLayoutForTextContainer:ns.textContainer];
    NSRect used = [ns.layoutManager usedRectForTextContainer:ns.textContainer];
    _contentSize.width = used.size.width * g_backingScaleFactor;
    _contentSize.height = used.size.height * g_backingScaleFactor;

    size s;
    s.width = ceilf(_contentSize.width);
    s.height = ceilf(_contentSize.height);
    return s;
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
