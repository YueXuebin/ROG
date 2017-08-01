#ifndef regular_daily_base_h__
#define regular_daily_base_h__


#include "regular_rog_dungeon.h"


/*
 *	日常地图规则基类
 */
class RegularDailyBase : public RegularROGDungeon
{
    CnDeclareRTTI
public:
    RegularDailyBase(RegularDoc *pDoc);
    virtual ~RegularDailyBase();


    virtual void OneSecondUpdate(int32 deltatime);

    virtual void Update(uint32 curr_tick);
    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

private:
    
};


#endif // regular_daily_base_h__
