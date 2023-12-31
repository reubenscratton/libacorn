//
//  libacorn
//
#include "../libacorn.h"


class ImageView : public View {
public:
    virtual size measure(const rect& rect) override;
    ImageView();
    
protected:
    
#ifdef __OBJC__
    virtual void createNSView() override;
#endif
    virtual bool applyProp(const string& key, val& v) override;
};

