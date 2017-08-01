#include "gameholder_pch.h"
#include "regular_daily_base.h"

CnImplementRTTI(RegularDailyBase, RegularROGDungeon);

RegularDailyBase::RegularDailyBase(RegularDoc *pDoc) :
RegularROGDungeon(pDoc)
{
    
}

RegularDailyBase::~RegularDailyBase()
{
    
}

void RegularDailyBase::OneSecondUpdate(int32 deltatime)
{
    RegularROGDungeon::OneSecondUpdate(deltatime);
}

void RegularDailyBase::Update(uint32 curr_tick)
{
    RegularROGDungeon::Update(curr_tick);
}

void RegularDailyBase::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    RegularROGDungeon::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);
}