//
//
//


class TextView : public View {
public:
    virtual size measure(const rect& r) override;
    virtual void layout(rect& r) override;
    TextView();
    
protected:
    
    virtual void onGravityChanged() override;
#ifdef __OBJC__
    virtual void createNSView() override;
#endif
    
    virtual bool applyProp(const string& key, val& v) override;
};

