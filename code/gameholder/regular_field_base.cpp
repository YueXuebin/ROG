#include "gameholder_pch.h"
#include "regular_field_base.h"
#include "game_util.h"
#include "battle_unit.h"
#include "pet.h"
#include "event_define.h"
#include "scene.h"
#include "player.h"
#include "pet_manager.h"



CnImplementRTTI(RegularFieldBase, RegularBuilder);

RegularFieldBase::RegularFieldBase(RegularDoc* pDoc) : 
RegularBuilder(pDoc)
{
    m_PlayerGroupCounter = 10;
}

RegularFieldBase::~RegularFieldBase()
{

}

void RegularFieldBase::OnCreatureEnter(BattleUnit* creature)
{
    RegularBuilder::OnCreatureEnter(creature);

    // 野外PVP,free for all
    if(m_PlayerGroupCounter < 10)
        m_PlayerGroupCounter = 10;
    m_PlayerGroupCounter++;

    // 野外地图的组规则
    uint32 CreatureType = creature->GetObjectType();
    switch(CreatureType)
    {
    case OBJ_PLAYER:
        creature->SetGroup(m_PlayerGroupCounter);
        break;
    case OBJ_OFFLINE_PLAYER:
        creature->SetGroup(FRIEND_GROUP);
        break;
    case OBJ_PET:
        if(((Pet*)creature)->GetMaster())
            creature->SetGroup( ((Pet*)creature)->GetMaster()->GetGroup() );
        break;
    case OBJ_MONSTER:
        creature->SetGroup(ENEMY_GROUP);
        break;
    case OBJ_BOSS:
        creature->SetGroup(ENEMY_GROUP);
        break;
    default:
        CnAssert(false);
        break;
    }
}

void RegularFieldBase::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    RegularBuilder::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

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

void RegularFieldBase::OnBattleEventPlayerKill( uint32 attackerID, uint32 defencerID )
{
    Player* attackPlayer = 0;
    //玩家死亡
    attackPlayer = GetScene()->GetPlayerByObjectId(defencerID);
    if(!attackPlayer)
        return;

    NONE none;
    attackPlayer->SendToGate(MSG_PLAYER_DEAD_CAN_RELIVE, &none);
}

void RegularFieldBase::OnPlayerResurgenceReq(int32 arg1)
{
    Player* player = m_pScene->FindPlayer(arg1);
    if(!player || !player->IsDead())
        return;

    Resurgence((BattleUnit *)player, 0, 0);  // 原地复活
    Resurgence((BattleUnit *)player->GetPetMgr()->GetSoldier(), 0, 0); // 复活佣兵
}

