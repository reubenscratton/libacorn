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
@implementation NSWindow_
- (id)init {
    self = [self initWithContentRect:NSMakeRect(100,100,400,300)
                                       styleMask: NSWindowStyleMaskTitled |
                                                NSWindowStyleMaskClosable |
                                          NSWindowStyleMaskMiniaturizable |
                                               NSWindowStyleMaskResizable
                                         backing:NSBackingStoreBuffered
                                           defer:NO];
    self.delegate = self;
    return self;
}
- (NSSize)windowWillResize:(NSWindow *)sender
                    toSize:(NSSize)frameSize {
    return frameSize;
}
- (void)windowDidResize:(NSNotification *)notification {
}
@end

Window* Window::create(const val& props) {
    auto w = new Window();
    w->applyProps(props);
    return w;
}

Window::Window() : ScriptObj() {
    _rootView = View::create({});
    _rootView->_isRoot = true;
    _wnd = [NSWindow_ new];
    g_backingScaleFactor = _wnd.backingScaleFactor;
    g_systemFontSize = [NSFont preferredFontForTextStyle:NSFontTextStyleBody options:@{}].pointSize;
    _wnd.contentView = _rootView->_nsview;
}


bool Window::applyProp(const string& key, val& v) {
    
    if (key == "width") {
        CGSize size = _wnd.contentView.frame.size;
        size.width = v.intVal() / g_backingScaleFactor; // acorn vals are hw px, convert it to iOS/macOS logical pixels
        [_wnd setContentSize:size];
        return true;
    }
    if (key == "height") {
        CGSize size = _wnd.contentView.frame.size;
        size.height = v.intVal() / g_backingScaleFactor;
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
