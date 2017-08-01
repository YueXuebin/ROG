#include "gameholder_pch.h"
#include "rog_regular_event_monster_dead.h"
#include "event_define.h"
#include "monster.h"
#include "regular_dungeon.h"
#include "scene.h"

CnImplementRTTI(ROGRegularEvent_MonsterDead, ROGRegularEvent);

ROGRegularEvent_MonsterDead::ROGRegularEvent_MonsterDead(RegularDungeon* pRegular) :
ROGRegularEvent(pRegular, ROG_REGULAR_EVENT_MONSTER_DEAD)
{
}

ROGRegularEvent_MonsterDead::~ROGRegularEvent_MonsterDead()
{

}

void ROGRegularEvent_MonsterDead::SetMonster(int32 monsterId, int32 count)
{
    SetParams(monsterId, count, 0);
}

void ROGRegularEvent_MonsterDead::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    ROGRegularEvent::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    if (GetActive())
        return;

    switch(eventId)
    {
    case BATTLE_EVENT_MONSTER_DEAD:
        {
            OnRecvCreatureDead(arg1);
        }
        break;
    case BATTLE_EVENT_BOSS_DEAD:
        {
            OnRecvCreatureDead(arg1);
        }
        break;
    }
}

void ROGRegularEvent_MonsterDead::OnRecvCreatureDead(uint32 objectID)
{
    BattleUnit* pBattleUnit = GetScene()->GetBattleUnit(objectID);
    if(!pBattleUnit)
    {
        CnAssert(false);
        return;
    }

    Monster* pMonster = CnDynamicCast(Monster, pBattleUnit);
    if(!pMonster)
    {
        CnAssert(false);
        return;
    }

    uint32 creatureId = pMonster->GetCreatureId();

    if(creatureId == GetMonsterId())
    {
        SetCurrentMonsterCount(GetCurrentMonsterCount() + 1);
        GetRegular()->SetConditionChanged();
    }

    if(GetCurrentMonsterCount() >= GetMonsterCount())
    {
        SetActive(true);
    }
}

