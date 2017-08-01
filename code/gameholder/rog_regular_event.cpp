#include "gameholder_pch.h"
#include "rog_regular_event.h"

CnImplementRTTI(ROGRegularEvent, RegularEvent);

ROGRegularEvent::ROGRegularEvent(RegularDungeon* pRegular, int32 type) :
RegularEvent((Regular*)pRegular, type)
{
    m_param1 = 0;
    m_param2 = 0;
    m_param3 = 0;
}

ROGRegularEvent::~ROGRegularEvent()
{

}

RegularDungeon* ROGRegularEvent::GetRegular()
{
    return (RegularDungeon*) m_pRegular;
}

DUNGEON_CONDITION ROGRegularEvent::GetCondition(Player* player)
{
    CnAssert(player);

    DUNGEON_CONDITION condition;

    condition.active = GetActiveForPlayer(player);
    condition.type = GetType();
    condition.param1 = GetParam1();
    condition.param2 = GetParam2();
    condition.param3 = GetParam3();

    return condition;
}