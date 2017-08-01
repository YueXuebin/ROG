#include "gameholder_pch.h"
#include "regular_maoxian_base.h"
#include "game_util.h"
#include "event_define.h"
#include "scene.h"
#include "player.h"
#include "pet_manager.h"

CnImplementRTTI(RegularMaoXianBase, RegularROGDungeon);

RegularMaoXianBase::RegularMaoXianBase(RegularDoc *pDoc) :
RegularROGDungeon(pDoc)
{
    SetLifeCount(3);
}

RegularMaoXianBase::~RegularMaoXianBase()
{
}

void RegularMaoXianBase::OneSecondUpdate(int32 deltatime)
{
    RegularROGDungeon::OneSecondUpdate(deltatime);
}

void RegularMaoXianBase::Update(uint32 curr_tick)
{
    RegularROGDungeon::Update(curr_tick);
}

void RegularMaoXianBase::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    RegularROGDungeon::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    switch(eventId)
    {
        case BATTLE_EVENT_PLAYER_DEAD:
            {
                OnBattleEventPlayerKill(arg2, arg1);
            }
            break;
        case EVENT_PLAYER_RESURGENCE_REQ:       // 接收复活请求
            {
                OnPlayerResurgenceReq(arg1);
            }
            break;
        default:
            break;
    }
}

void RegularMaoXianBase::OnBattleEventPlayerKill( uint32 attackerID, uint32 defencerID )
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

void RegularMaoXianBase::OnPlayerResurgenceReq(int32 arg1)
{
    Player* player = m_pScene->FindPlayer(arg1);
    if(!player || !player->IsDead())
        return;

    if (GetLifeCount() <= 0)
        return;

    SubLifeCount(1);                    // 减去一条命
    Resurgence((BattleUnit *)player, 0, 0);  // 原地复活
    Resurgence((BattleUnit *)player->GetPetMgr()->GetSoldier(), 0, 0); // 复活佣兵
}
