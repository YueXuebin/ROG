#ifndef regular_daily_002_h__
#define regular_daily_002_h__


#include "regular_daily_base.h"

// 日常2(保卫商人)
class RegularDaily_002 : public RegularDailyBase
{
public:
    RegularDaily_002(RegularDoc* pDoc);
    virtual ~RegularDaily_002();

    virtual void FirstUpdate();


};


#endif // regular_daily_002_h__
