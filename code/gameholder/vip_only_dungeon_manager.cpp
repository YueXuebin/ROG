#include "gameholder_pch.h"
#include "vip_only_dungeon_manager.h"
#include "player.h"
#include "dungeon_manager.h"
#include "vip_only_dungeon_config.h"
#include "trade_manager.h"
#include "player_dungeon_manager.h"
#include "event_define.h"
#include "gameplaytimes_notifier.h"

VIPOnlyDungeonManager::VIPOnlyDungeonManager(Player *player) : m_player(player)
{
}

VIPOnlyDungeonManager::~VIPOnlyDungeonManager()
{
}

void VIPOnlyDungeonManager::ChangeDungeon(bool isFree)
{
    if (isFree)
    {
        m_vipOnlyDungeonInfo.used_free_count += 1;
        CnDbg("VIP dungeon free count=%d\n", m_vipOnlyDungeonInfo.used_free_count);
    }
    else
    {
        m_vipOnlyDungeonInfo.used_pay_count += 1;
        CnDbg("VIP dungeon pay count=%d\n", m_vipOnlyDungeonInfo.used_pay_count);
    }

    PushVIPOnlyDungeonInfo(ERR_SUCCEED);

    CHANGE_DUNGEON_REQ msg;
    msg.dungeon_id = VIPOnlyDungeonConfig::Instance()->GetDungeonID();
    m_player->OnRecv(MSG_CHANGE_DUNGEON_REQ, &msg);
}

void VIPOnlyDungeonManager::LoadVIPOnlyDungeonInfo(VIP_ONLY_DUNGEON_INFO &info)
{
    m_vipOnlyDungeonInfo = info;

    //今天零点
    Crown::CSDDateTime date = Crown::SDNow();
    date.SetTime(0, 0, 0);

    //隔天重新计算
    if (m_vipOnlyDungeonInfo.record_time < date.GetTimeValue())
    {
        m_vipOnlyDungeonInfo.record_time = (uint32)date.GetTimeValue();
        m_vipOnlyDungeonInfo.used_free_count = 0;
        m_vipOnlyDungeonInfo.used_pay_count = 0;
        //m_vipOnlyDungeonInfo.Clear();
    }

    //推送更新消息
    PushVIPOnlyDungeonInfo(ERR_SUCCEED);
}

void VIPOnlyDungeonManager::FillVIPOnlyDungeonInfo(VIP_ONLY_DUNGEON_INFO &info)
{
    info = m_vipOnlyDungeonInfo;
}

void VIPOnlyDungeonManager::CalDailyRefresh()
{
    //隔天重新计算
    m_vipOnlyDungeonInfo.record_time = (uint32)Crown::SDNow().GetTimeValue();
    m_vipOnlyDungeonInfo.used_free_count = 0;
    m_vipOnlyDungeonInfo.used_pay_count = 0;
    
    //m_vipOnlyDungeonInfo.Clear();

    //推送更新消息
    PushVIPOnlyDungeonInfo(ERR_SUCCEED);
}

void VIPOnlyDungeonManager::CalGamePlayTimes()
{
    int32 vipLevel = m_player->GetVipLevel();
    const VIP_ONLY_DUNGEON_CFG *cfg = VIPOnlyDungeonConfig::Instance()->GetVIPConfig(vipLevel);
    if (cfg)
    {
        int32 totalCount = cfg->max_free_count + cfg->max_pay_count;
        int32 usedCount = m_vipOnlyDungeonInfo.used_free_count + m_vipOnlyDungeonInfo.used_pay_count;
        int32 leftCount = totalCount - usedCount;
        m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_VIP_ONLY_DUNGEON, leftCount, totalCount);
    }
}

void VIPOnlyDungeonManager::OnRecv(uint32 msgID, void *data)
{
    switch (msgID)
    {
    case MSG_VIP_ONLY_DUNGEON_INFO_REQ:
        {
            PushVIPOnlyDungeonInfo(ERR_SUCCEED);
        }
        break;
    case MSG_VIP_ONLY_DUNGEON_ENTER_REQ:
        {
            OnMsgVIPOnlyDungeonEnterReq(msgID, data);
        }
        break;
    }
}

void VIPOnlyDungeonManager::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2)
{
    switch (eventId)
    {
    case EVENT_VIP_ONLY_DUNGEON_END:
        {
            OnVIPOnlyDungeonEnd(arg1 != 0);
        }
        break;
    }
}

void VIPOnlyDungeonManager::OnMsgVIPOnlyDungeonEnterReq(uint32 msgID, void *data)
{
    uint32 dungeonID = VIPOnlyDungeonConfig::Instance()->GetDungeonID();

    //必须在主城且副本ID存在
    uint32 dungeonClass = GetDungeonClass(DUNGEON_ID(m_player->GetUnionMapID()));
    if (dungeonClass != DUNGEON_TYPE_CITY || dungeonID == 0)
    {
        return;
    }

    //副本是否可以进入
    PlayerDungeonManager *playerDungeonMgr = m_player->GetPlayerDungeonManager();
    uint16 errCode = playerDungeonMgr->CanChangeToDungeon(m_player, dungeonID);
    if (errCode != ERR_SUCCEED)
    {
        PushVIPOnlyDungeonInfo(ERR_VIP_ONLY_DUNGEON_FAIL);
        return;
    }

    int32 vipLevel = m_player->GetVipLevel();
    const VIP_ONLY_DUNGEON_CFG *cfg = VIPOnlyDungeonConfig::Instance()->GetVIPConfig(vipLevel);
    if (!cfg)
    {
        PushVIPOnlyDungeonInfo(ERR_VIP_ONLY_DUNGEON_FAIL);
        return;
    }

    //还有免费次数可用，首先使用免费次数
    if (m_vipOnlyDungeonInfo.used_free_count < cfg->max_free_count)
    {
        ChangeDungeon(true);
        return;
    }

    int32 totalCount = cfg->max_free_count + cfg->max_pay_count;
    int32 usedCount = m_vipOnlyDungeonInfo.used_free_count + m_vipOnlyDungeonInfo.used_pay_count;

    //今日次数已用尽
    if (usedCount >= totalCount)
    {
        PushVIPOnlyDungeonInfo(ERR_VIP_ONLY_DUNGEON_COUNT_LACK);
        return;
    }

    //魔石不足
    int32 cost = int32(floor(cfg->money_point_cost * cfg->discount / 100.0f));
    if (m_player->GetMoneyPoint() < cost)
    {
        PushVIPOnlyDungeonInfo(ERR_VIP_ONLY_DUNGEON_MONEY_POINT_LACK);
        return;
    }

    TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_VIP_ONLY_DUNGEON, PROP_VIP_ONLY_DUNGEON, 0, 0, cost);
}

void VIPOnlyDungeonManager::OnVIPOnlyDungeonEnd(bool isWin)
{
    int32 val = VIPOnlyDungeonConfig::Instance()->GetSoulValue(isWin);

    VIP_ONLY_DUNGEON_END_ACK ack;
    ack.is_win = isWin;
    ack.soul_value = val;
    m_player->SendToGate(MSG_VIP_ONLY_DUNGEON_END_ACK, &ack);

    m_player->AddSoulPoint(val, REWARD_REASON_VIP_ONLY_DUNGEON);
}

void VIPOnlyDungeonManager::PushVIPOnlyDungeonInfo(uint16 err)
{
    //同步客户端数据
    VIP_ONLY_DUNGEON_INFO_ACK ack;

    ack.errcode = err;

    if (ack.errcode == ERR_SUCCEED)
    {
        ack.used_free_count = m_vipOnlyDungeonInfo.used_free_count;
        ack.used_pay_count = m_vipOnlyDungeonInfo.used_pay_count;
    }

    m_player->SendToGate(MSG_VIP_ONLY_DUNGEON_INFO_ACK, &ack);
}