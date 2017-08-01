#include "gameholder_pch.h"
#include "regular_yijie_base.h"
#include "game_util.h"
#include "event_define.h"
#include "scene.h"
#include "player.h"

CnImplementRTTI(RegularYiJieBase, RegularROGDungeon);

RegularYiJieBase::RegularYiJieBase(RegularDoc *pDoc) :
RegularROGDungeon(pDoc)
{

}

RegularYiJieBase::~RegularYiJieBase()
{

}

void RegularYiJieBase::OneSecondUpdate(int32 deltatime)
{
    RegularROGDungeon::OneSecondUpdate(deltatime);
}

void RegularYiJieBase::Update(uint32 curr_tick)
{
    RegularROGDungeon::Update(curr_tick);

}

void RegularYiJieBase::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    RegularROGDungeon::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    switch(eventId)
    {
        case BATTLE_EVENT_PLAYER_DEAD:
            {
                OnBattleEventPlayerKill(arg2, arg1);
            }
            break;
        case BATTLE_EVENT_MONSTER_DEAD:
            {
                OnBattleEventMonsterDead((uint32)arg1, (uint32)arg2);
            }
            break;
        default:
            break;
    }
}

void RegularYiJieBase::OnBattleEventPlayerKill( uint32 attackerID, uint32 defencerID )
{
    Player* attackPlayer = 0;
    //玩家死亡
    attackPlayer = GetScene()->GetPlayerByObjectId(defencerID);
    if(!attackPlayer)
        return;

    if (GetLifeCount() > 0)     // 有可用命数
    {
        NONE none;
        attackPlayer->SendToGate(MSG_PLAYER_DEAD_CAN_RELIVE, &none);
    }
}

void RegularYiJieBase::OnBattleEventMonsterDead( uint32 monsterID, uint32 killerID )
{

}
