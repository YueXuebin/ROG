#include "gameholder_pch.h"
#include "battle_state_manager.h"
#include "event_define.h"
#include "game_define.h"
#include "ride_manager.h"
#include "player.h"
#include "state_define.h"


BattleStateManager::BattleStateManager(Player* player) : 
m_player(player)
{
    m_InBattle = false;
}

BattleStateManager::~BattleStateManager()
{

}

void BattleStateManager::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 )
{
    switch(eventId)
    {
    case BATTLE_EVENT_ONHIT:   // 被攻击进入战斗状态
        EnterBattleState();
        break;
    }
}
// 使用技能进入战斗状态
void BattleStateManager::OnUseSkill(uint32 skill_id)
{
    if(skill_id == BUFF_RIDE_REST)
        return;

    EnterBattleState();

}

void BattleStateManager::Update()
{
    if(m_InBattle)
    {
        m_InBattleTimer += g_ServerUpdateTime;
        if(m_InBattleTimer >= g_BattleStateTime)
        {
            LeaveBattleState();
        }
    }
}

void BattleStateManager::EnterBattleState()
{
    m_InBattleTimer = 0;
    if(m_InBattle)
        return;

    m_InBattle = true;
    OnEnterBattleState();
}

void BattleStateManager::LeaveBattleState()
{
    if(!m_InBattle)
        return;

    m_InBattle = false;
    OnLeaveBattleState();
}

void BattleStateManager::OnEnterBattleState()
{
    // 进入战斗状态通知
    BATTLE_STATE_NTF ntf;
    ntf.battle_state = true;
    m_player->SendToGate(MSG_BATTLE_STATE_NTF, &ntf);
}

void BattleStateManager::OnLeaveBattleState()
{
    BATTLE_STATE_NTF ntf;
    ntf.battle_state = false;
    m_player->SendToGate(MSG_BATTLE_STATE_NTF, &ntf);
}

void BattleStateManager::OnStateChange(uint32 oldState, uint32 newState)
{
    if(IsServerState(newState))
    {
        // 下坐骑
        RideManager* pRideManager = m_player->GetRideManager();
        if(pRideManager->GetRidePetID() != 0)
        {
            pRideManager->RequestDownRide();
        }
    }
}
