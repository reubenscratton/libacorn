//
//  libacorn
//

#include "../libacorn.h"
#include "nsview.h"
extern float g_backingScaleFactor;

#ifdef __OBJC__

DECLARE_NSVIEW_TYPE(NSView, View);

@implementation NSView_
#include "nsview.objc.inc"
@end

#endif


template<>
void js_init<View>(qjs::Context::Module& m) {
    m.class_<View>("View")
        .constructor<>()
        .fun<&View::addView>("addView")
        .fun<&View::findView>("findView")
        PROPERTY(View, onmouseenter)
        PROPERTY(View, onmouseexit)
        PROPERTY(View, ondragenter)
        PROPERTY(View, ondragdrop)
        PROPERTY(View, backgroundColor)
        PROPERTY(View, margin)
    ;
}



View* View::createImpl(const val& props) {
    string viewClassName = props.get("class").stringVal();
    if (!viewClassName.length()) {
        if (props.hasVal("text")) {
            viewClassName = "TextView";
        } else if (props.hasVal("image")) {
            viewClassName = "ImageView";
        }
    }
    View* v = nullptr;
    if (viewClassName.length()) {
        v = (View*)Object::createByName(viewClassName);
    } else {
        v = new View();
    }
    v->createNSView();
    assert(v->_nsview);
    v->applyProps(props);
    return v;
}


View::View() {
}
View::~View() {
}
void View::createNSView() {
    _nsview = [[NSView_ alloc] init];
    ((NSView_*)_nsview)->_view = this;
}
void View::onGravityChanged() {
    // no-op
}
bool View::applyProp(const string& key, val& v) {
    if (key == "id") {
        _id = v.stringVal();
        return true;
    }
    if (key == "backgroundColor") {
        set_backgroundColor(v);
        return true;
    }
    if (key == "views") {
        auto& a = v.arrayRef();
        for (val& vs : a) {
            sp<View> view = View::create<View>(vs);
            addView(view);
        }
        return true;
    }
    if (key == "layout") {
        _layout = v.stringVal();
        return true;
    }
    if (key == "x") {
        _x = v;
        return true;
    }
    if (key == "y") {
        _y = v;
        return true;
    }
    if (key == "width") {
        _width = v;
        return true;
    }
    if (key == "height") {
        _height = v;
        return true;
    }
    if (key == "flex-basis") {
        _flexBasis = v;
        return true;
    };
    if (key == "flex-grow") {
        _flexGrow = v.floatVal();
        return true;
    };
    if (key == "flex-shrink") {
        _flexShrink = v.floatVal();
        return true;
    };
    if (key == "gravity") {
        auto getGravity = [=](const val& v) -> float {
            if (v.isString()) {
                auto& g = v.stringRef();
                if (g=="left" || g=="top") return 0;
                if (g=="center" || g=="centre") return 0.5f;
                if (g=="right" || g=="bottom") return 1.0f;
            }
            else if (v.isNumeric()) {
                return v.floatVal();
            }
            log_error("Bad gravity val: %s", v.debugString());
            return 0;
        };
        if (v.isArray()) {
            auto& a = v.arrayRef();
            assert(a.size() == 2);
            _gravityH = getGravity(a[0]);
            _gravityV = getGravity(a[1]);
        }
        else if (v.isString() || v.isNumeric()) {
            _gravityH = getGravity(v);
        }
        else {
            log_error("Bad gravity val: %s", v.debugString());
            return true;
        }
        onGravityChanged();
        return true;
    }
    if (key == "padding") {
        if (v.isNumeric()) {
            _padding[0] = _padding[1] = _padding[2] = _padding[3] = v;
        }
        else if (v.isArray()) {
            auto& a = v.arrayRef();
            if (a.size() == 4) {
                _padding[0] = a[0];
                _padding[1] = a[1];
                _padding[2] = a[2];
                _padding[3] = a[3];
            } else if (a.size() == 2) {
                _padding[0] = _padding[2] = a[0].floatVal();
                _padding[1] = _padding[3] = a[1].floatVal();
            } else {
                throw "Invalid padding declaration";
            }
        } else {
            throw "Invalid padding declaration";
        }
        return true;
    }
    if (key == "border") {
        if (!v.isArray()) {
            throw "Invalid border declaration";
        }
        auto& a = v.arrayRef();
        if (a.size() != 3) {
            throw "Invalid border declaration";
        }
        _borderColor = a[0];
        _borderWidth = a[1];
        _borderStyle = a[2];
        return true;
    }
    return false;
}

void applyPosition(float& existing, val& pos, float parentLength, float ownLength) {
    if (!pos.isEmpty()) {
        if (pos.isString()) {
            if (pos == "center") {
                existing += (parentLength - ownLength) / 2;
            }
        }
        if (pos.isNumeric()) {
            existing += pos.measurementVal().valuePx(parentLength);
        }
    }

}

void View::measure(rect& rect) {
    auto c = rect.size;
    //if (_superview->_layout != "row") { // if is row element, fill height by default
    //if (!_height.isEmpty()) {
        rect.size.height = _height.measurementVal().valuePx(rect.size.height);
    //}
    //if (_superview->_layout != "column") {
    //if (!_width.isEmpty()) {
        rect.size.width = _width.measurementVal().valuePx(rect.size.width);
    //}
    applyPosition(rect.origin.x, _x, c.width, rect.size.width);
    applyPosition(rect.origin.y, _y, c.height, rect.size.height);
}


void View::layout(rect& r) {
    if (!_isRoot) {
        // convert rect from hw pixels (px) to logical pixels
        NSRect nsr = NSMakeRect(r.origin.x/g_backingScaleFactor,
                                r.origin.y/g_backingScaleFactor,
                                r.size.width/g_backingScaleFactor,
                                r.size.height/g_backingScaleFactor);
        [_nsview setFrame:nsr];
    }
    if (_subviews.size() == 0) {
        return;
    }
    
    // Inset the given rect by our padding
    rect rect = r;
    rect.origin.x = _padding[0].measurementVal().valuePx(r.size.width);
    rect.origin.y = _padding[1].measurementVal().valuePx(r.size.height);
    rect.size.width -=  _padding[0].measurementVal().valuePx(r.size.width) +
                        _padding[2].measurementVal().valuePx(r.size.width);
    rect.size.height -= _padding[1].measurementVal().valuePx(r.size.height) +
                        _padding[3].measurementVal().valuePx(r.size.height);
    
    // Flexbox
    bool isRow = (_layout == "row");
    bool isCol = (_layout == "column");
    if (isRow || isCol) {
        val View::*pmainprop = isRow ? &View::_width : &View::_height;
        float size::*main = isRow ? &size::width : &size::height;
        float size::*cross = isRow ? &size::height : &size::width;
        float point::*start = isRow ? &point::x : &point::y;
        float basis = 0;
        float shrinkTotal = 0;
        float growTotal = 0;
        float* mainLengths = (float*)alloca(sizeof(float)*_subviews.size());
        // flex-basis: if unspecified or 'auto' then the corresponding main
        // length prop (width or height) is used. If this length prop is
        // unspecified then the max-content size is used.
        int i = 0;
        for (sp<View>& subview : _subviews) {
            // Calculate the flex basis
            val& vb = subview->_flexBasis;
            if (vb.isEmpty() || vb == "auto") {
                // If subview has a set main size we use that, otherwise ask
                // the subview how big it'd *like* to be, given a fixed
                // cross-size and infinite main size.
                val& mainprop = subview.get()->*pmainprop;
                if (!mainprop.isEmpty()) {
                    mainLengths[i] = mainprop.measurementVal().valuePx(rect.size.*main);
                }
                else {
                    struct rect subrect = rect;
                    subrect.size.*main = 2000000000;
                    subview->measure(subrect);
                    mainLengths[i] = subrect.size.*main;
                }
            } else {
                mainLengths[i] = vb.floatVal();
            }
            basis += mainLengths[i];
            i++;
            shrinkTotal += subview->_flexShrink;
            growTotal += subview->_flexGrow;
        }
        float diff = rect.size.*main - basis;
        i=0;
        for (sp<View>& subview : _subviews) {
            struct rect subrect = rect;
            subrect.size.*main = mainLengths[i++];
            if (diff >= 0  && growTotal>0) {
                subrect.size.*main += (subview->_flexGrow / growTotal) * diff;
            } else if (diff < 0 && shrinkTotal>0) {
                subrect.size.*main += (subview->_flexShrink / shrinkTotal) * diff;
            }
            subrect.size.*cross = rect.size.*cross;
            subview->layout(subrect);
            rect.origin.*start += subrect.size.*main;
            rect.size.*main -= subrect.size.*main;
        }
        return;
    }
    
    // Default
    //rect.origin = {0,0};
    for (sp<View>& subview : _subviews) {
        subview->measure(rect);
        subview->layout(rect);
    }
    
}

void View::addView(sp<View> view) {
    assert(view->_superview == nullptr);
    view->_superview = this;
    _subviews.push_back(view);
    [_nsview addSubview:view->_nsview];
}

sp<View> View::findView(const string& id) {
    for (sp<View>& subview : _subviews) {
        if (id == subview->_id) {
            return subview;
        }
    }
    for (sp<View>& subview : _subviews) {
        auto r = subview->findView(id);
        if (r) {
            return r;
        }
    }
    return nullptr;
}

MOUSEFUNC View::get_onmouseenter()  {
    return _onmouseenter;
}
MOUSEFUNC View::set_onmouseenter(const MOUSEFUNC& f) {
    [(NSView_*)_nsview setWantMouseEnterExit: f!=nil];
    _onmouseenter = f; return _onmouseenter;
}
MOUSEFUNC View::get_onmouseexit()  {
    return _onmouseexit;
}
MOUSEFUNC View::set_onmouseexit(const MOUSEFUNC& f) {
    [(NSView_*)_nsview setWantMouseEnterExit: f!=nil];
    _onmouseexit = f; return _onmouseexit;
}

DRAGFUNC View::get_ondragenter()  {
    return _ondragenter;
}
DRAGFUNC View::set_ondragenter(const DRAGFUNC &f) {
    [((NSView_*)_nsview) setWantDragDest:f!=nil];
    _ondragenter = f; return _ondragenter;
}
DRAGFUNC View::get_ondragdrop()  {
    return _ondragdrop;
}
DRAGFUNC View::set_ondragdrop(const DRAGFUNC &f) {
    [((NSView_*)_nsview) setWantDragDest:f!=nil];
    _ondragdrop = f; return _ondragdrop;
}

val View::get_backgroundColor() {
    return _backgroundColor;
}
val View::set_backgroundColor(const val& v) {
    _backgroundColor = v;
    [_nsview setNeedsDisplay: YES];
    return _backgroundColor;
}

val View::get_margin() {
    return _margin;
}
val View::set_margin(const val& v) {
    _margin = v;
    [_nsview setNeedsLayout:YES];
    return _margin;
}
