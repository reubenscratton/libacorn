//
//  libacorn
//

// Common NSView-based code that cannot be shared by inheritance. This file is supposed
// to be #included for NSView-derived types.

- (BOOL)autoresizesSubviews {
    return NO;
}

- (BOOL)isFlipped {
    return YES;
}

- (void)layout {
    if (_view->_isRoot) {
        rect r = {0,0,(float)self.frame.size.width * g_backingScaleFactor,(float)self.frame.size.height * g_backingScaleFactor};
        _view->layout(r);
    }
}

- (void)mouseEntered:(NSEvent *)event {
    if (_view->_onmouseenter) {
        _view->_onmouseenter();
    }
}

- (void)mouseExited:(NSEvent *)event {
    if (_view->_onmouseexit) {
        _view->_onmouseexit();
    }
}

- (void)drawRect:(NSRect)rect {
    
    if (!_view->_backgroundColor.isEmpty()) {
        color c = _view->_backgroundColor.toColor();
        CGContextRef ctx = [NSGraphicsContext currentContext].CGContext;
        CGContextSetFillColorWithColor(ctx, [NSColor colorWithRed:c.r green:c.g blue:c.b alpha:c.a].CGColor);
        CGContextFillRect(ctx, rect);
    }
    
    [super drawRect:rect];
    
    if (!_view->_borderWidth.isEmpty()) {
        CGContextRef ctx = [NSGraphicsContext currentContext].CGContext;
        CGContextSetStrokeColorSpace(ctx, CGColorSpaceCreateDeviceRGB());
        float width = _view->_borderWidth.measurementVal().valuePt(0);
        CGContextSetLineWidth(ctx, width);
        if (_view->_borderStyle == "solid") {
            CGContextSetLineDash(ctx, 0, nullptr, 0);
        }
        else if (_view->_borderStyle == "dashed") {
            CGFloat dash[] = {width*2, width};
            CGContextSetLineDash(ctx, 0, dash, 2);
        } else {
            assert(0);
        }
        color col = _view->_borderColor.toColor();
        CGFloat rgba[] = {(CGFloat)col.r, (CGFloat)col.g, (CGFloat)col.b, (CGFloat)col.a};
        CGContextSetStrokeColor(ctx, rgba);
        CGContextStrokeRect(ctx, [self bounds]);
    }
}


- (void)setWantMouseEnterExit:(bool)want {
    if (want) {
        if (self.trackingAreas.count == 0) {
            [self addTrackingArea:[[NSTrackingArea alloc] initWithRect:self.bounds options:NSTrackingMouseEnteredAndExited|NSTrackingActiveAlways|NSTrackingInVisibleRect owner:self userInfo:nil]];
        }
    } else {
        if (self.trackingAreas.count > 0) {
            [self removeTrackingArea:self.trackingAreas[0]];
        }
    }
}



// Drag-drop

- (void)setWantDragDest:(BOOL)want {
    [self registerForDraggedTypes:@[NSPasteboardTypeFileURL]];

}
- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender {
    //View* view = (__bridge View*)objc_getAssociatedObject(self, kAcornView);
    if (_view->_ondragenter) {
        vector<string> files(sender.numberOfValidItemsForDrop);
        auto pfiles = &files;
        [sender enumerateDraggingItemsWithOptions:0 forView:self classes:@[NSURL.class] searchOptions:@{NSPasteboardURLReadingFileURLsOnlyKey: [NSNumber numberWithBool:YES]} usingBlock:^(NSDraggingItem* item, NSInteger idx, BOOL* stop) {
            NSURL* url = item.item;
            (*pfiles)[idx] = ns2str(url.path);
        }];
        if (_view->_ondragenter(files) > 0) {
            return NSDragOperationCopy;
        }
    }
    return NSDragOperationNone;
}
- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender {
    //View* view = (__bridge View*)objc_getAssociatedObject(self, kAcornView);
    if (!_view->_ondragdrop) {
        return NO;
    }
    NSArray* objs = [sender.draggingPasteboard readObjectsForClasses: @[NSURL.class] options: nil];
    vector<string> files(objs.count);
    int i = 0;
    for (NSURL* url : objs) {
        files[i++] = ns2str(url.path);
    }
    return _view->_ondragdrop(files);
}



/*template <typename T>
const char* kAcornView = "acornview";
void swizzle(Class clazz, SEL sel, IMP hook, T& real) {
    Method method = class_getInstanceMethod(clazz, sel);
    assert(method);
    // When swizzling a class method, use class_getClassMethod(class, sel) instead
    real = (typeof(real))method_getImplementation(method);
    class_replaceMethod(clazz, sel, hook, method_getTypeEncoding(method));
}
View* view = (__bridge View*)objc_getAssociatedObject(self, kAcornView);

static void (*real_drawRect)(id,SEL,NSRect);
static void hook_drawRect(id nsview, SEL sel, NSRect rect) {
 }
 
 if (!real_drawRect) {
     Class clazz = [_nsview class];
     swizzle(clazz, @selector(drawRect:), (IMP)hook_drawRect, real_drawRect);
     swizzle(clazz, @selector(autoresizesSubviews), (IMP)hook_autoresizesSubviews, real_autoresizesSubviews);
     swizzle(clazz, @selector(isFlipped), (IMP)hook_isFlipped, real_isFlipped);
     swizzle(clazz, @selector(layout), (IMP)hook_layout, real_layout);
     
     //Protocol* pro = objc_getProtocol("NSDraggingDestination");
     //class_addProtocol(clazz, pro);
 }
 objc_setAssociatedObject(_nsview, "acornview", (__bridge id)this, OBJC_ASSOCIATION_ASSIGN);

*/
