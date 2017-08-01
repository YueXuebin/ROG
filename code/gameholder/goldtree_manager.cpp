#include "gameholder_pch.h"
#include "goldtree_manager.h"
#include "player.h"
#include "event_define.h"
#include "goldtree_config.h"
#include "vip_config.h"
#include "trade_manager.h"
#include "gameplaytimes_notifier.h"
#include "guild_science_recorder.h"

GoldtreeManager::GoldtreeManager(Player* player) :
m_player(player)
{
    Clear();
}

GoldtreeManager::~GoldtreeManager()
{

}

void GoldtreeManager::Clear()
{
    m_enterCDTime = 0;
    m_enterCount = 0;
    m_Dirty = false;
    m_gamePoint = 0;
    //m_remainingTime = 0;
    m_enterBuyCount = 0;
}

void GoldtreeManager::LoadGoldtreeInfo(GOLDTREE_INFO& info)
{
    m_enterCount = info.count;
    m_enterCDTime = info.enter_time;
    m_enterBuyCount = info.buy_count;
}

void GoldtreeManager::FillGoldtreeInfo(GOLDTREE_INFO& info)
{
    info.count = m_enterCount;
    info.enter_time = m_enterCDTime;
    info.buy_count = m_enterBuyCount;
}

void GoldtreeManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_GOLDTREE_INFO_REQ:
        onGoldtreeInfoReq();
        break;
    case MSG_GOLDTREE_CD_REQ:
        onGoldtreeCDReq();
        break;
    case MSG_GOLDTREE_BUY_REQ:
        //onGoldtreeBuyReq();
        break;
    default:
        break;
    }
}

void GoldtreeManager::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2)
{
    switch(eventId)
    {
    case EVENT_GOLDTREE_GAMESTART:
        {
            EnterGoldtree();
            Notify();
        }
        break;
    case EVENT_GOLDTREE_DAMAGE:
        {
            SubBossHP(arg1, arg2);
        }
        break;
    case EVENT_GOLDTREE_GAMEEND:
        {
            SendReward();
        }
        break;
    case EVENT_CHANGE_VIP:
        {
            onGoldtreeInfoReq();
        }
        break;
    default:
        break;
    }
}

void GoldtreeManager::CalDailyRefresh()
{
    SaveYesterdayData();

    m_enterCDTime = 0;
    m_enterCount = 0;
}

void GoldtreeManager::onGoldtreeInfoReq()
{
    GOLDTREE_INFO sendMsg;
    Crown::CSDDateTime nowTime = Crown::SDNow().GetTimeValue();

    if(VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_16 , m_player->GetVipLevel()))
        m_enterCDTime = 0;

    sendMsg.count = m_enterCount;
    sendMsg.buy_count = m_enterBuyCount;

    // 计算剩余时间
    if(m_enterCDTime > (uint32)nowTime.GetTimeValue())
    {
        sendMsg.enter_time = m_enterCDTime - (uint32)nowTime.GetTimeValue();
    }

    m_player->SendToGate(MSG_GOLDTREE_INFO_ACK, &sendMsg);

    SendGameNotify();
}



void GoldtreeManager::onGoldtreeCDReq()
{
    // 计算剩余时间
    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
    uint32 leftTime = 0;

    if(m_enterCDTime <= nowTime)
    {
        onGoldtreeInfoReq();
        return;
    }

    leftTime = m_enterCDTime - nowTime;

    // 计算所需魔石数
    int32 count = (leftTime / 60) + 1;      // 向上取整
    int32 money = count * GoldtreeConfig::Instance()->GetMoneyPoint();

    if(m_player->GetMoneyPoint() < money)
    {
        m_player->NotifyErrorCode(ERR_NO_ENOUGH_MONEYPOINT);
        return;
    }

    TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_GOLDTREE, PROP_GOLD_TREE_CD_TIMES, 0, 0 ,money);
}


void GoldtreeManager::OnGoldtreeCD()
{
    m_enterCDTime = 0;
    onGoldtreeInfoReq();
}

void GoldtreeManager::onGoldtreeBuyReq()
{
    uint16 err = ERR_FAILED;
    if(m_enterBuyCount >= (uint32)VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_14 , m_player->GetVipLevel()))
    {
        err = ERR_GOLDTREE_NO_ENOUGH_BUY_COUNT;
    }
    else
    {
        err = m_player->SubItemOne(REWARD_REASON_GOLDTREE_BUY_COUNT, PROP_ITEM, 1, GOLDTREE_TESSERA);
    }

    if(err == ERR_SUCCEED)
    {
        m_enterBuyCount++;
    }

    GOLDTREE_BUY_ACK sendMsg;
    sendMsg.err_code = err;
    sendMsg.enter_count = m_enterCount;
    sendMsg.buy_count = m_enterBuyCount;
    m_player->SendToGate(MSG_GOLDTREE_BUY_ACK, &sendMsg);

    SendGameNotify();
}

int32 GoldtreeManager::GetMaxEnterCount()
{
    return VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_14 , m_player->GetVipLevel()) + GoldtreeConfig::Instance()->GetGoldtreeCount();
}

void GoldtreeManager::SaveYesterdayData()
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

    //m_player->SaveYesterdayCount(OPEN_SYS_GOLD_TREE, count);
}

int16 GoldtreeManager::CanEnterGoldtree()
{
    ERROR_CODE sendMsg;
    sendMsg.Clear();

    // 次数用完
    int32 maxEnterCount = GetMaxEnterCount();
    if(m_enterCount >= maxEnterCount + m_enterBuyCount)
    {
        return ERR_GOLDTREE_NUMBER_END;
    }

    // 副本冷却中
    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
    if(nowTime < m_enterCDTime)
    {
        return ERR_GOLDTREE_CD;
    }

    return ERR_SUCCEED;
}

void GoldtreeManager::EnterGoldtree()
{
    m_enterCount++;
    // 进入冷却时间
    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
    m_enterCDTime = nowTime + GoldtreeConfig::Instance()->GetCDTime(m_enterCount) * 60;

    SendGameNotify();

    // 活跃度
    m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_GOLD_TREE, 1, 0, 0);
}

void GoldtreeManager::SubBossHP(int32 bossHP, int32 time)
{
    if(bossHP < 0)
    {
        return;
    }
    CnAssert(bossHP <= BASE_RATE_NUM);

    m_bossHP = bossHP;

    uint32 gamepoint = GoldtreeConfig::Instance()->GetGamePointByLevel(m_player->GetLevel());

    uint32 subHp = (BASE_RATE_NUM - bossHP) / 1000;
    //if(subHp == 0)
    //{
    //    m_gamePoint = gamepoint;
    //}
    //else
    //{
        m_gamePoint = subHp * gamepoint;
    //}

    // 工会科技加成：魔王金像加成
    int32 value = m_gamePoint;
    m_player->GetGuildScienceManager()->AddAdditionPromoteNum(GUILD_ADDITION_GOLD_TREE, value);
    m_gamePoint = value;

    //m_remainingTime = time;
    Notify();
}

void GoldtreeManager::SendReward()
{
    GOLDTREE_END_NTF sendMsg;

    sendMsg.num = m_bossHP;
    sendMsg.game_point = m_gamePoint;

    m_player->SendToGate(MSG_GOLDTREE_END_ACK, &sendMsg);
    m_player->AddGamePoint(m_gamePoint, REWARD_REASON_GOLDTREE);

    // 清除数据
    m_bossHP = 0;
    m_gamePoint = 0;
    //m_remainingTime = 0;
}

void GoldtreeManager::Notify()
{
    GOLDTREE_INFO_NTF sendMsg;
    sendMsg.boss_hp = m_bossHP;
    sendMsg.game_point = m_gamePoint;
    //sendMsg.remaining_time = m_remainingTime;

    m_player->SendToGate(MSG_GOLDTREE_INFO_NTF, &sendMsg);
}

void GoldtreeManager::SendGameNotify()
{
    int32 maxEnterCount = GetMaxEnterCount();
    int32 canEnterCount = maxEnterCount + m_enterBuyCount;

    canEnterCount -= (int32)m_enterCount;
    // 如果VIP到期，则有可能减出负数
    if (canEnterCount < 0)
        canEnterCount = 0;

    m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_GOLDTREE, canEnterCount, maxEnterCount);
}

