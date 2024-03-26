//
//  Window.cpp
//  acorn
//

#include "../libacorn.h"
#include <CoreGraphics/CoreGraphics.h>

float g_backingScaleFactor = 1.0f;
float g_systemFontSize = 16.0f;

// MacBook Pro 2015 - native res is 2880x1800 Retina
// Logical display is 1440x900, with backing scale 2.0

/*
 NSWindow_ : wraps NSWindow and makes it it's own delegate as well
 as container of a root View which becomes the NSWindow's contentView.
 */

@interface NSWindow_ : NSWindow <NSWindowDelegate> {
    Window* _cwindow;
}
@end

@implementation NSWindow_
- (id)initWithCWindow:(Window*)cwindow {
    self = [self initWithContentRect:NSMakeRect(100,100,400,300)
                                       styleMask: NSWindowStyleMaskTitled |
                                                NSWindowStyleMaskClosable |
                                          NSWindowStyleMaskMiniaturizable |
                                               NSWindowStyleMaskResizable
                                         backing:NSBackingStoreBuffered
                                           defer:NO];
    self->_cwindow = cwindow;
    self.delegate = self;
    return self;
}
- (NSSize)windowWillResize:(NSWindow *)sender
                    toSize:(NSSize)frameSize {
    return frameSize;
}
- (void)windowDidResize:(NSNotification *)notification {
}
- (void)close {
    [super close];
    if (_cwindow->_onclose) {
        _cwindow->_onclose();
    }

    exit(0);
}
@end

Window::Window(const string& uiAsset) : Window() {
    string filepath = uiAsset;
    resolvePath(filepath);
    
    size_t buf_len;
    const char* buf = (const char*)js_load_file(nullptr, &buf_len, filepath.c_str());

    auto props = val::parse(string(buf,-1), 0);
    applyProps(props);

}



Window::Window() : JSObj() {
    _rootView = View::create<View>({});
    _rootView->_isRoot = true;
    _wnd = [[NSWindow_ alloc] initWithCWindow:this];
    g_backingScaleFactor = _wnd.backingScaleFactor;
    g_systemFontSize = [NSFont preferredFontForTextStyle:NSFontTextStyleBody options:@{}].pointSize;
    _wnd.contentView = _rootView->_nsview;
}
Window::~Window() {
}

bool Window::applyProp(const string& key, val& v) {
    
    if (key == "width") {
        CGSize size = _wnd.contentView.frame.size;
        size.width = v.measurementVal().valuePt(size.width);
        [_wnd setContentSize:size];
        return true;
    }
    if (key == "height") {
        CGSize size = _wnd.contentView.frame.size;
        size.height = v.measurementVal().valuePt(size.height);
        [_wnd setContentSize:size];
        return true;
    }
    if (key == "title") {
        [_wnd setTitle:[NSString stringWithUTF8String:v.stringVal().c_str()]];
        return true;
    }
    
    // Let things like subviews and background color etc be directly handled by the root view
    return _rootView->applyProp(key, v);
}

void Window::show() {
    [_wnd makeKeyAndOrderFront:nil];
}

CLOSEFUNC Window::get_onclose()  {
    return _onclose;
}
CLOSEFUNC Window::set_onclose(const CLOSEFUNC& f) {
    _onclose = f; return _onclose;
}

sp<View> Window::get_rootView() {
    return _rootView;
}
sp<View> Window::findView(const string& id) {
    return _rootView->findView(id);
}




