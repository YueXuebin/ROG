#include "gameholder_pch.h"
#include "treasury_manager.h"
#include "player.h"
#include "treasury_config.h"
#include "event_define.h"
#include "vip_config.h"
#include "gameplaytimes_notifier.h"
#include "trade_manager.h"
#include "guild_science_recorder.h"

TreasuryManager::TreasuryManager(Player* player) :
m_player(player)
{
    Clear();
}

TreasuryManager::~TreasuryManager()
{

}

void TreasuryManager::Clear()
{
    m_enterCDTime = 0;
    m_enterCount = 0;
    m_Dirty = false;
    m_gamePoint = 0;
    m_killMonsterCout = 0;
    m_enterBuyCount = 0;
}

void TreasuryManager::LoadTreasuryInfo(TREASURY_INFO& info)
{
    m_enterCount = info.count;
    m_enterCDTime = info.enter_time;
    m_enterBuyCount = info.buy_count;
}

void TreasuryManager::FillTreasuryInfo(TREASURY_INFO& info)
{
    info.count = m_enterCount;
    info.enter_time = m_enterCDTime;
    info.buy_count = m_enterBuyCount;
}

void TreasuryManager::Update()
{
    if(m_Dirty)
    {
        Notify();
        m_Dirty = false;
    }
}

void TreasuryManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_TREASURY_INFO_REQ:
        onTreasuryInfoReq();
        break;
    case MSG_TREASURY_CD_REQ:
        onTreasuryCDReq();
        break;
    case MSG_TREASURY_BUY_REQ:
        onTreasuryBuyCount();
        break;
    default:
        break;
    }
}

void TreasuryManager::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    switch(eventId)
    {
    case EVENT_TREASURY_KILL_MONSTER:
        {
            AddKillCount();
        }
        break;
    case EVENT_TREASURY_GAMESTART:
        {
            Notify();
        }
        break;
    case EVENT_TREASURY_GAMEEND:
        {
            SendReward();
        }
        break;
    case EVENT_CHANGE_VIP:
        {
            onTreasuryInfoReq();
        }
        break;
    }
}

void TreasuryManager::CalDailyRefresh()
{
    SaveYesterdayData();

    m_enterCDTime = 0;
    m_enterCount = 0;
}

void TreasuryManager::SaveYesterdayData()
{
    // 是否保存最大次数
    //bool isSaveMaxCount = m_player->GetGainbackResourceManager()->IsSaveMaxCount();
    //int32 count = 0;

    //if(isSaveMaxCount)
    //    count = GetMaxEnterCount();
    //else
    //    count = GetMaxEnterCount() - m_enterCount;

    //if(count < 0)
    //    count = 0;

    //m_player->SaveYesterdayCount(OPEN_SYS_TREASURY, count);
}

void TreasuryManager::onTreasuryInfoReq()
{
    TREASURY_INFO sendMsg;
    Crown::CSDDateTime nowTime = Crown::SDNow();

    // 根据VIP等级，检查是否可秒CD
    if(VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_17 , m_player->GetVipLevel()))
        m_enterCDTime = 0;

    sendMsg.count = m_enterCount;
    // 计算剩余时间
    if(m_enterCDTime > (uint32)nowTime.GetTimeValue())
    {
        sendMsg.enter_time = m_enterCDTime - (uint32)nowTime.GetTimeValue();
    }
    sendMsg.buy_count = m_enterBuyCount;
    m_player->SendToGate(MSG_TREASURY_INFO_ACK, &sendMsg);

    SendGameNotify();
}

void TreasuryManager::onTreasuryCDReq()
{
    // 计算剩余时间
    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
    uint32 leftTime = 0;

    if(m_enterCDTime <= nowTime)
    {
        onTreasuryInfoReq();
        return;
    }

    leftTime = m_enterCDTime - nowTime;

    // 计算所需魔石数
    int32 count = (leftTime / 60) + 1;           // 向上取整
    int32 money = count * TreasuryConfig::Instance()->GetMoneyPoint();

    if(m_player->GetMoneyPoint() < money)
    {
        m_player->NotifyErrorCode(ERR_NO_ENOUGH_MONEYPOINT);
        return;
    }

    TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_THREASURY, PROP_TREASURY_CD_TIMES, 0, 0 ,money);
}

void TreasuryManager::onTreasuryCD()
{
    m_enterCDTime = 0;
    onTreasuryInfoReq();
}

void TreasuryManager::onTreasuryBuyCount()
{
    uint16 err = ERR_FAILED;
    //if(m_enterBuyCount >= VipConfig::Instance()->GetTreasuryCount(m_player->GetVipLevel()))
    if(m_enterBuyCount >= (uint32)VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_15 , m_player->GetVipLevel()))
    {
        err = ERR_TREASURY_NO_ENOUGH_BUY_COUNT;
    }
    else
    {
        err = m_player->SubItemOne(REWARD_REASON_TREASURY_BUY_COUNT, PROP_ITEM, 1, GOLDTREE_TESSERA);
    }

    if(err == ERR_SUCCEED)
    {
        m_enterBuyCount++;
    }

    TREASURY_BUY_ACK sendMsg;
    sendMsg.err_code = err;
    sendMsg.enter_count =  m_enterCount;
    sendMsg.buy_count = m_enterBuyCount;
    m_player->SendToGate(MSG_TREASURY_BUY_ACK, &sendMsg);
    SendGameNotify();
}

int32 TreasuryManager::GetMaxEnterCount()
{
    return VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_15 , m_player->GetVipLevel()) + TreasuryConfig::Instance()->GetTreasuryCount();
}

void TreasuryManager::AddKillCount()
{
    int32 num = 0;

    m_killMonsterCout++;

    num = TreasuryConfig::Instance()->GetReward(m_player->GetLevel());
    num = m_player->GetFcmGainValue(num);

    // 工会科技加成：藏宝库加成 TODO:
    m_player->GetGuildScienceManager()->AddAdditionPromoteNum(GUILD_ADDITION_TREASURY, num);

    m_gamePoint += num;

    m_Dirty = true;
}

int16 TreasuryManager::CanEnterTreasury()
{
    ERROR_CODE sendMsg;
    sendMsg.Clear();

    // 次数用完
    int32 maxEnterCount = GetMaxEnterCount();
    if(m_enterCount >= maxEnterCount + m_enterBuyCount)
    {
        return ERR_TREASURY_NUMBER_END;
    }

    // 副本冷却中
    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
    if(nowTime < m_enterCDTime)
    {
        return ERR_TREASURY_CD;
    }

    SendGameNotify();

    return ERR_SUCCEED;
}

void TreasuryManager::EnterTreasury()
{
    m_enterCount++;
    // 进入的冷却时间
    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
    m_enterCDTime = nowTime + TreasuryConfig::Instance()->GetCDTime(m_enterCount) * 60;

    SendGameNotify();
}

void TreasuryManager::SendReward()
{
    TREASURY_END_NTF sendMsg;
    sendMsg.Clear();

    // 杀怪的数量
    sendMsg.num = GetKillMonsterCount();

    // 获得金币
    sendMsg.game_point = m_gamePoint;

    m_player->SendToGate(MSG_TREASURY_END_ACK, &sendMsg);

    // 添加金币
    m_player->AddGamePoint(m_gamePoint, REWARD_REASON_TREASURY);

    // 清除数据
    m_killMonsterCout = 0;
    m_gamePoint = 0;
    //m_remainingTime = 0;

    // 活跃度
    m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_TREASURY_DUNGEON, 1, 0, 0);
}

void TreasuryManager::Notify()
{
    TREASURY_INFO_NTF sendMsg;
    sendMsg.Clear();
    sendMsg.monster_num = m_killMonsterCout;
    sendMsg.game_point = m_gamePoint;

    m_player->SendToGate(MSG_TREASURY_INFO_NTF, &sendMsg);
}

void TreasuryManager::SendGameNotify()
{
    int32 maxEnterCount = GetMaxEnterCount();
    int32 canEnterCount = maxEnterCount + m_enterBuyCount;

    canEnterCount -= (int32)m_enterCount;
    // 如果VIP到期，则有可能减出负数
    if (canEnterCount < 0)
        canEnterCount = 0;

    m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_TREASURE, canEnterCount, maxEnterCount);
}


