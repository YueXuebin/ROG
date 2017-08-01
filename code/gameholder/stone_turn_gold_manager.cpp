#include "gameholder_pch.h"
#include "stone_turn_gold_manager.h"
#include "stone_turn_gold_config.h"
#include "player_manager.h"
#include "player.h"
#include "formula_manager.h"
#include "trade_manager.h"
#include "vip_config.h"
#include "gameplaytimes_notifier.h"
#include "guild_science_recorder.h"
#include "event_define.h"

// ==================================================
// 构造函数
// ==================================================
StoneTurnGoldManager::StoneTurnGoldManager(Player* player):
m_player(player)
{
    Clear();
}

// ==================================================
// 析构函数
// ==================================================
StoneTurnGoldManager::~StoneTurnGoldManager()
{
}

// ==================================================
// 清除数据
// ==================================================
void StoneTurnGoldManager::Clear()
{

}

// ==================================================
// 接收消息
// ==================================================
void StoneTurnGoldManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_STONE_TURN_GOLD_REQ:
        OnStoneTurnGoldReq(data);
        break;
    default:
        CnAssert(false);
        break;
    }
}

void StoneTurnGoldManager::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2)
{
    switch(eventId)
    {
    case EVENT_CHANGE_VIP:
        {
            UpdateStoneTurnGoldTimes();
        }
        break;
    default:
        break;
    }
}

// ==================================================
// 更新点石成金的次数
// ==================================================
void StoneTurnGoldManager::UpdateStoneTurnGoldTimes()
{
    int32 max_times = 0;
    int32 left_times = 0;

    max_times = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_13, m_player->GetVipLevel());

    if(max_times > m_player->GetStoneTurnGoldTimes())
    {
        left_times = max_times - m_player->GetStoneTurnGoldTimes();
    }
    else
    {
        left_times = 0;
    }

    m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_STONE_TURN_GOLD, left_times, max_times);
}


int32 StoneTurnGoldManager::GetStoneHowNum()
{
    int32 left_times = 0;
    int32 max_times = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_13, m_player->GetVipLevel());
    if(max_times > left_times)
        left_times = max_times - m_player->GetStoneTurnGoldTimes();
    else
        left_times = 0;

    return left_times;
}
// ==================================================
// 接收点石成金请求
// ==================================================
void StoneTurnGoldManager::OnStoneTurnGoldReq(void* data)
{
    STONE_TURN_GOLD* recmsg = (STONE_TURN_GOLD*)data;
    if(!recmsg)
        return;

    int32 money_point = 0;
    int32 game_point = 0;
    int32 totalTimes = 0;
    int32 times = 10; // 点击十次

    totalTimes = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_13, m_player->GetVipLevel());
    if(m_player->GetStoneTurnGoldTimes() >= totalTimes)
    {
        return;
    }

    if(recmsg->type == STONE_TURN_GOLD_NO_ONCE)//1次
    {
        money_point = StoneTurnGoldConfig::Instance()->GetCostMoneyPoint(m_player->GetStoneTurnGoldTimes()+1);
        game_point = FormulaManager::Instance()->CalcStoneTurnGold(m_player->GetLevel());

    }
    else if (recmsg->type == STONE_TURN_GOLD_ONCE)//10次
    {

        int32 surplus = totalTimes - m_player->GetStoneTurnGoldTimes();
        int32 leftnum = m_player->GetStoneTurnGoldTimes() + 1;
        
        if(surplus < times)
            times = surplus;

        for (int32 i = 0; i < times; i++)
        {
           money_point = money_point + StoneTurnGoldConfig::Instance()->GetCostMoneyPoint(leftnum);
           leftnum = leftnum + 1;
        }

        game_point = FormulaManager::Instance()->CalcStoneTurnGold(m_player->GetLevel());
        game_point = game_point * times;
    }

    if((money_point <= 0) || (game_point <= 0 ))
    {
        return;
    }

    if(money_point > m_player->GetMoneyPoint())
    {
        m_player->NotifyErrorCode(ERR_NO_ENOUGH_MONEYPOINT);
        return;
    }

    // 工会科技加成：点石成金加成 TODO:
    m_player->GetGuildScienceManager()->AddAdditionPromoteNum(GUILD_ADDITION_STONE_TURN_GOLD, game_point);

    TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_STONE_TURN_GAMEPOINT,
        PROP_GAMEPOINT, recmsg->type * 100 + times, game_point, money_point);

}
void StoneTurnGoldManager::OnStoneTurnGold(uint32 type)
{
    uint32 stype = (uint32)(type / 100);
    int32 count = type - STONE_TURN_GOLD_ONCE * 100;
    if(stype == STONE_TURN_GOLD_NO_ONCE)
    {
        m_player->IncStoneTurnGoldTimes();
        UpdateStoneTurnGoldTimes();
        m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_ALCHEMY, 1, 0, 0);
    }
    else if (stype == STONE_TURN_GOLD_ONCE)
    {
        for (int32 i = 0; i < count; i++)
        {
            m_player->IncStoneTurnGoldTimes();
        }
        UpdateStoneTurnGoldTimes();
        m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_ALCHEMY, 10, 0, 0);
    }
}
