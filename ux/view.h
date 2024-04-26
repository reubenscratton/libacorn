//
//  libacorn
//

class JSObj : public acorn::Object {
public:
    JSObj() : Object() {
    }
    
    void applyProps(const val& props) {
        if (props.type == val::type::MAP) {
            for (auto it=props.mapBegin() ; it!=props.mapEnd() ; it++) {
                applyProp(it->first, it->second);
            }
        }
    }
protected:
    virtual bool applyProp(const string& key, val& v) {
        log_warn("Ignored applyProp: %s", key.c_str());
        return false;
    }
    
};


/*
 Every view has these properties:
 
 width, height - the actual size of the view. Can be set explicitly but doing so
                 won't override min/max/flex constraints if they are set.

 min-width, min-height - limits for view size.
 max-width, max-height
 
 content-size     - the size of the view's content *as rendered*.
 content-size-max - the size the view's content *would be*, given infinite space
 
 Image has:
   - the intrinsic/actual image size.
   - the size the image is rendered at (depends on content-scale)

 Text has:
   - the size it would be, given infinite space
   - the size it would be, given finite width but infinite height.
   - the size it actually is. Will include ellipsis if text truncated.
 
 When measure() is called it is given a rect that cannot be exceeded. The view
 can choose to make its bounds exceed its frame, i.e. make itself scrollable.
 Or it can clip. Or it can ellipsize. Or it can shrink its content. 
 
 
 content-width, content-height - the intrinsic size of the views content (i.e.
       excluding padding). Read-only, cos it's up to the view to define it
       if it needs to. For leaf views it will (generally)
       be the size of the view's content (e.g. text or canvas rendering).
       For containers it defaults to the union of all child views. If an explicit
       height or width is set, the corresponding content measurement defaults
       to that minus relevant padding.
 
 layout - for containers this determines how subviews are arranged. If unspecified
       then child views are positioned by code, or they position themselves.
       Can be set to 'row' or 'column' in which case more properties
       (on child views) apply:


 flex-grow, flex-shrink - the standard CSS properties that control
 a view's flexibility along the main axis (within the min/max limits).
  basis defaults to 'auto'.
 
 flex-wrap - unlike CSS this is an integer that specifies the max number of lines
 in the cross axis. Defaults to 1, i.e. a single line. 0 means no limit.
 
 
 
 
 */

struct point {
    float x,y;
};
struct size {
    float width,height;
};
struct rect {
    point origin;
    size size;
};
struct insets {
    float left,top,right,bottom;
    inline void applyToRect(rect& r) {
        r.origin.x += left;
        r.origin.y += top;
        r.size.width -= left+right;
        r.size.height -= top+bottom;
    }
};
typedef std::function<void(const val& data)> EVENTFUNC;
typedef std::function<void()> MOUSEFUNC;
typedef std::function<int(vector<string>)> DRAGFUNC;


// C++ class that wraps the Obj-C class and hides as much platform detail as possible
class View : public JSObj {
public:
    template <class T>
    static sp<T> create(const val& props) {
        T* v = (T*)createImpl(props);
        return sp<T>(v);
    }
    static View* createImpl(const val& props);

    // Returns the size this view would like to be, given the constraining rect
    virtual size measure(const rect& rect);
    
    virtual void layout(rect& rect);
    
    bool _isRoot = false;
protected:
    virtual bool applyProp(const string& key, val& v) override;
    
    virtual void onGravityChanged();
    virtual struct insets getActualMargins(const rect& r);
    virtual struct insets getActualPadding(const rect& r);
#ifdef __OBJC__
    virtual void createNSView();
    NSView* _nsview;
#else
    void* _nsview;
#endif
    
public:
    // Props
    val _x;
    val _y;
    val _width;
    val _height;
    val _widthMin;
    val _heightMin;
    val _widthMax;
    val _heightMax;
    val _flexBasis;
    float _flexGrow = 0;
    float _flexShrink = 0;
    string _layout;
    float _gravityH = 0;
    float _gravityV = 0;
    val _borderColor;
    val _borderWidth;
    val _borderStyle;

    // Read-only props?
    size _contentSize = {0, 0};


    DECLPROPJ(onmouseenter, MOUSEFUNC);
    DECLPROPJ(onmouseexit, MOUSEFUNC);
    DECLPROPJ(ondragenter, DRAGFUNC);
    DECLPROPJ(ondragdrop, DRAGFUNC);
    DECLPROPJ(backgroundColor, val);
    DECLPROPJ(margin, val);
    DECLPROPJ(padding, val);


    string _id;
    View* _superview = nullptr;
    vector<sp<View>> _subviews;
    View();
    ~View();
    
    // Methods
    void addView(sp<View> view);
    sp<View> findView(const string& id);
    
private:
    friend class Window;
};

