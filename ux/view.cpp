//
//  libacorn
//

#include "../libacorn.h"

extern float g_backingScaleFactor;


@implementation _NSView

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

@end


View* View::create(const val& props) {
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

class DbgView : public View {
public:
    virtual void layout(rect& rect) override {
        View::layout(rect);
    }

};
DECLARE_DYNCREATE(DbgView);

View::View() {
}
void View::createNSView() {
    _NSView* nsview = [[_NSView alloc] init];
    nsview->_view = this;
    _nsview = nsview;
}
void View::onGravityChanged() {
    // no-op
}
bool View::applyProp(const string& key, val& v) {
    if (key == "backgroundColor") {
        color cv = v.toColor();
        [_nsview setWantsLayer: YES];
        _nsview.layer.backgroundColor = [NSColor colorWithRed:cv.r green:cv.g blue:cv.b alpha:cv.a].CGColor;
        return true;
    }
    if (key == "views") {
        auto& a = v.arrayRef();
        for (val& vs : a) {
            View* view = View::create(vs);
            view->_superview = this;
            _subviews.push_back(view);
            [_nsview addSubview:view->_nsview];
        }
        return true;
    }
    if (key == "layout") {
        _layout = v.stringVal();
        return true;
    }
    if (key == "width") {
        _width = v.floatVal();
        return true;
    }
    if (key == "height") {
        _height = v.floatVal();
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
            _padding.left = _padding.right = _padding.top = _padding.bottom = v.floatVal();
        }
        else if (v.isArray()) {
            auto& a = v.arrayRef();
            if (a.size() == 4) {
                _padding.left = a[0].floatVal();
                _padding.top = a[1].floatVal();
                _padding.right = a[2].floatVal();
                _padding.bottom = a[3].floatVal();
            } else if (a.size() == 2) {
                _padding.left = _padding.right = a[0].floatVal();
                _padding.top = _padding.bottom = a[1].floatVal();
            } else {
                throw "Invalid padding declaration";
            }
        } else {
            throw "Invalid padding declaration";
        }
        return true;
    }
    return false;
}

void View::measure(rect& rect) {
    if (_superview->_layout != "row") { // if is row element, fill height by default
        rect.size.height = (_height.isEmpty())?0:_height.floatVal();
    }
    if (_superview->_layout != "column") {
        rect.size.width = (_width.isEmpty())?0:_width.floatVal();
    }
}

extern float g_backingScaleFactor;

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
    rect.origin.x += _padding.left;
    rect.origin.y += _padding.top;
    rect.size.width -= _padding.left + _padding.right;
    rect.size.height -= _padding.top + _padding.bottom;
    
    // Flexbox
    bool isRow = (_layout == "row");
    bool isCol = (_layout == "column");
    if (isRow || isCol) {
        val View::*mainprop = isRow ? &View::_width : &View::_height;
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
        for (View* subview : _subviews) {
            // Calculate the flex basis
            val& vb = subview->_flexBasis;
            if (vb.isEmpty() || vb == "auto") {
                // Ask subview how big it'd *like* to be, given a fixed
                // cross-size and infinite main size.
                struct rect subrect = rect;
                subrect.size.*main = 2000000000;
                subview->measure(subrect);
                mainLengths[i] = subrect.size.*main;
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
        for (View* subview : _subviews) {
            struct rect subrect = rect;
            subrect.size.*main = mainLengths[i++];
            if (diff >= 0) {
                subrect.size.*main += (subview->_flexGrow / growTotal) * diff;
            } else {
                subrect.size.*main += (subview->_flexShrink / shrinkTotal) * diff;
            }
            subrect.size.*cross = rect.size.*cross;
            subview->layout(subrect);
            rect.origin.*start += subrect.size.*main;
            rect.size.*main -= subrect.size.*main;
        }
    }
}
