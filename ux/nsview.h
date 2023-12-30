//
//  libacorn
//

#include "../libacorn.h"

#ifdef __OBJC__


#define DECLARE_NSVIEW_TYPE(NSTYPE, CTYPE) \
    @interface NSTYPE##_ : NSTYPE { \
        @public CTYPE* _view; \
    } \
    @end


#endif
