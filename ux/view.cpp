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
        PROPERTY(View, padding)
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

static int s_viewCount = 0;

View::View() {
    s_viewCount++;
    //printf("View(). Count: %d!\n", s_viewCount);

}
View::~View() {
    s_viewCount--;
    //printf("~View(). Count: %d!\n", s_viewCount);
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
    if (key == "margin") {
        set_margin(v);
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
        set_padding(v);
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

size View::measure(const rect& rect) {
    // Default implementation only references the _width and _height props
    // Note that margins are not considered here.
    // TODO: Padding should be applied here!
    size s;
    s.height = _height.measurementVal().valuePx(rect.size.height);
    s.width = _width.measurementVal().valuePx(rect.size.width);
    return s;
}

struct insets View::getActualPadding(const rect& r) {
    struct insets pad = {0,0,0,0};
    if (_padding.isEmpty()) {
    }
    else if (_padding.isArray()) {
        auto& a = _padding.arrayRef();
        if (a.size() == 4) {
            pad.left = a.at(0).measurementVal().valuePx(r.size.width);
            pad.top = a.at(1).measurementVal().valuePx(r.size.height);
            pad.right = a.at(2).measurementVal().valuePx(r.size.width);
            pad.bottom = a.at(3).measurementVal().valuePx(r.size.height);
        } else if (a.size()==2) {
            pad.left =
            pad.right = a.at(0).measurementVal().valuePx(r.size.width);
            pad.top =
            pad.bottom = a.at(1).measurementVal().valuePx(r.size.height);
        } else {
            assert(0);
        }
    } else if (_padding.isNumeric()) {
        pad.left =
        pad.right =
        pad.top =
        pad.bottom = _padding.floatVal();
    } else {
        assert(false);
    }
    return pad;

}
struct insets View::getActualMargins(const rect& r) {
    struct insets margins = {0,0,0,0};
    if (_margin.isEmpty()) {
    }
    else if (_margin.isArray()) {
        auto& a = _margin.arrayRef();
        if (a.size() == 4) {
            margins.left = a.at(0).measurementVal().valuePx(r.size.width);
            margins.top = a.at(1).measurementVal().valuePx(r.size.height);
            margins.right = a.at(2).measurementVal().valuePx(r.size.width);
            margins.bottom = a.at(3).measurementVal().valuePx(r.size.height);
        } else if (a.size()==2) {
            margins.left =
            margins.right = a.at(0).measurementVal().valuePx(r.size.width);
            margins.top =
            margins.bottom = a.at(1).measurementVal().valuePx(r.size.height);
        } else {
            assert(0);
        }
    } else if (_margin.isNumeric()) {
        margins.left =
        margins.right =
        margins.top =
        margins.bottom = _margin.floatVal();
    } else {
        assert(false);
    }
    return margins;
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
    
    // Apply padding to given rect
    rect rect = r;
    rect.origin = {0,0};
    getActualPadding(r).applyToRect(rect);

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
                    size idealSize = subview->measure(subrect);
                    mainLengths[i] = idealSize.*main; //subrect.size.*main;
                }
            } else {
                mainLengths[i] = vb.floatVal();
            }
            // Add margins to the main length axis
            struct insets margins = subview->getActualMargins(rect);
            if (isRow) {
                mainLengths[i] += margins.left + margins.right;
            } else {
                mainLengths[i] += margins.top + margins.bottom;
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

            // The subview sizes were calculated with margins included. Now we unapply
            // them to get the correct final rect
            struct insets margins = subview->getActualMargins(rect);
            margins.applyToRect(subrect);

            // If X/Y props are set, apply them now
            applyPosition(subrect.origin.x, subview->_x, rect.size.width, subrect.size.width);
            applyPosition(subrect.origin.y, subview->_y, rect.size.height, subrect.size.height);

            subview->layout(subrect);
            rect.origin.*start += subrect.size.*main;
            rect.size.*main -= subrect.size.*main;
        }
        return;
    }
    
    // Default
    //rect.origin = {0,0};
    for (sp<View>& subview : _subviews) {
        struct rect subrect = rect;
        subrect.size = subview->measure(rect);
        struct insets margins = subview->getActualMargins(rect);
        subrect.origin.x += margins.left;
        subrect.origin.y += margins.top;
        // do not apply right/bottom margins as measure() result does not include margins
        applyPosition(subrect.origin.x, subview->_x, rect.size.width, subrect.size.width);
        applyPosition(subrect.origin.y, subview->_y, rect.size.height, subrect.size.height);
        subview->layout(subrect);
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

MOUSEFUNC View::set_onmouseenter(const MOUSEFUNC& f) {
    [(NSView_*)_nsview setWantMouseEnterExit: f!=nil];
    _onmouseenter = f; return _onmouseenter;
}
MOUSEFUNC View::set_onmouseexit(const MOUSEFUNC& f) {
    [(NSView_*)_nsview setWantMouseEnterExit: f!=nil];
    _onmouseexit = f; return _onmouseexit;
}
DRAGFUNC View::set_ondragenter(const DRAGFUNC &f) {
    [((NSView_*)_nsview) setWantDragDest:f!=nil];
    _ondragenter = f; return _ondragenter;
}
DRAGFUNC View::set_ondragdrop(const DRAGFUNC &f) {
    [((NSView_*)_nsview) setWantDragDest:f!=nil];
    _ondragdrop = f; return _ondragdrop;
}
val View::set_backgroundColor(const val& v) {
    _backgroundColor = v;
    [_nsview setNeedsDisplay: YES];
    return _backgroundColor;
}

val View::set_margin(const val& v) {
    _margin = v;
    [_nsview setNeedsLayout:YES];
    return _margin;
}

val View::set_padding(const val& v) {
    _padding = v;
    [_nsview setNeedsLayout:YES];
    return _padding;
}

