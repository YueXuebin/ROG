#ifndef regular_daily_001_h__
#define regular_daily_001_h__


#include "regular_daily_base.h"

// 日常1(生存战)
class RegularDaily_001 : public RegularDailyBase
{
public:
    RegularDaily_001(RegularDoc* pDoc);
    virtual ~RegularDaily_001();

    virtual void FirstUpdate();


};


#endif // regular_daily_001_h__
