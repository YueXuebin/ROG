#include "gameholder_pch.h"
#include "zombie_manager.h"
#include "message_encoder.h"
#include "zombie_config.h"
#include "dungeon_manager.h"
#include "event_define.h"
#include "vip_config.h"
#include "player.h"
#include "gameplaytimes_notifier.h"

ZombieManager::ZombieManager(Player *player):
m_player(player),
m_enterCount(0),
m_cumulativeLiveTime(0)
{
    
}

ZombieManager::~ZombieManager()
{

}

void ZombieManager::LoadZombieInfo(const ZOMBIE_INFO &info)
{
    m_enterCount = info.enter_count;

    AddLiveSeconds(info.cumulative_total_time);

    std::vector<unsigned int>::const_iterator iter = info.finish_zombie_dungeons.begin();
    for (; iter != info.finish_zombie_dungeons.end(); ++iter)
        m_finishDungeons.insert(*iter);
}

void ZombieManager::FillZombieInfo(ZOMBIE_INFO &info)
{
    info.enter_count = m_enterCount;
    info.cumulative_total_time = m_cumulativeLiveTime;

    std::set<uint32>::iterator iter = m_finishDungeons.begin();
    for (; iter != m_finishDungeons.end(); ++iter)
        info.finish_zombie_dungeons.push_back(*iter);
}

void ZombieManager::OnRecv(uint32 msgId, void* data)
{
    switch (msgId)
    {
    case MSG_ZOMBIE_WAR_INFO_REQ:
        OnZombieWarInfoReq();
        break;
    case MSG_ZOMBIE_OPEN_CHEST_REQ:
        OnReceiveReward();
        break;
    default:
        break;
    }
}

void ZombieManager::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    switch (eventId)
    {
    case EVENT_CHANGE_VIP:
        OnZombieWarInfoReq();
        break;
    default:
        break;
    }
}

void ZombieManager::OnZombieWarInfoReq()
{
    if (!m_player) return;

    int32 vipEnterCount = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_55, m_player->GetVipLevel());

    ZOMBIE_WAR_INFO sendMsg;
    ZOMBIE_DUNGEON_STATUS dungeonStatus;
    if (vipEnterCount < 0)
    {
        sendMsg.last_enter_count = 0;
    }
    else
    {
        sendMsg.last_enter_count = (uint32)vipEnterCount > m_enterCount ? (uint32)vipEnterCount - m_enterCount : 0;
    }
    
    sendMsg.live_time = m_cumulativeLiveTime;
    
    const ZombieConfig::ZombieDungeonMap& zombieMap = ZombieConfig::Instance()->GetDungeonInfoMap();
    for (ZombieConfig::ZombieDungeonMap::const_iterator iter = zombieMap.begin(); iter != zombieMap.end(); ++iter)
    {
        dungeonStatus.Clear();
        dungeonStatus.dungeon_id = iter->first;
        dungeonStatus.lock_status = CORPSESURVIVAL_UNLOCK;

        //前置副本没有完成不能解锁
        if (iter->second.pre_dungeon_id != 0)
        {
            if (!IsFinish(iter->second.pre_dungeon_id)) 
            {
                dungeonStatus.lock_status = CORPSESURVIVAL_LOCK;
            }
        }

        sendMsg.zombie_info_list.push_back(dungeonStatus);
    }
    m_player->SendToGate(MSG_ZOMBIE_WAR_INFO_ACK, &sendMsg);

    NotifyNum();
    //SendGameNotify();
}

void ZombieManager::OnReceiveReward()
{
    if (!m_player)
        return;

    if (m_cumulativeLiveTime < ZombieConfig::Instance()->GetMaxCumulativeSeconds())
        return;

    REWARD_TABLE rewardTable;
    rewardTable.Clear();
    rewardTable.items = ZombieConfig::Instance()->GetChestRewards();
    m_player->TakeInRewardTable(&rewardTable);
    
    //m_reward_num = m_reward_num + 1;
    m_player->SendTaskEvent(TASK_EVENT_ZOMBIE_REWARD, 1, 0);

    CleanLiveTime();

    OnZombieWarInfoReq();
}

uint16 ZombieManager::CanChangeToZombie(uint32 dungeonId)
{
    if (m_enterCount >= GetMaxEnterCount())
    {
        return ERR_ENTER_ZOMBIE_OUT_LIMIT;
    }

    const ZombieDungeonInfo *zombieInfo = ZombieConfig::Instance()->GetZombieDungeonInfo(dungeonId);
    if (!zombieInfo)
    {
        return ERR_NONE_ZOMBIE_DUNGEON;
    }

    if (zombieInfo->pre_dungeon_id != 0)
    {
        if (!IsFinish(zombieInfo->pre_dungeon_id))
            return ERR_PRE_ZOMBIE_NO_FINISH;
    }

    return ERR_SUCCEED;
}

void ZombieManager::AddLiveSeconds(uint32 sec)
{
    uint32 maxLiveSeconds = ZombieConfig::Instance()->GetMaxCumulativeSeconds();
    if (m_cumulativeLiveTime + sec > maxLiveSeconds)
        m_cumulativeLiveTime = maxLiveSeconds;
    else
        m_cumulativeLiveTime += sec;
}


uint32 ZombieManager::GetMaxEnterCount()
{
    CnAssert(m_player);
    if (m_player)
        return VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_55, m_player->GetVipLevel());

    return 0;
}

void ZombieManager::EndZombieWar()
{
    uint32 maxEnterCount = GetMaxEnterCount();
    CnAssert(m_enterCount < maxEnterCount);
    if (m_enterCount < maxEnterCount)
        ++m_enterCount;

    NotifyNum();
}

void ZombieManager::EnterZombieWar()
{
    m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_ZOMBIE_LIVE, 1, 0, 0);
}

void ZombieManager::NotifyNum()
{
    int32 levetime = 0;
    if( GetMaxEnterCount() >  m_enterCount)
        levetime = GetMaxEnterCount() - m_enterCount;
    m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_ZOMBOE, levetime, GetMaxEnterCount());
}

void ZombieManager::CalDailyRefresh()
{
    m_enterCount = 0;
}

bool ZombieManager::IsFinish(uint32 dungeonId)
{
    std::set<uint32>::iterator iter = m_finishDungeons.find(dungeonId);
    if (iter != m_finishDungeons.end())
        return true;    
    
    return false;
}

void ZombieManager::FinishDungeon(uint32 dungeonId)
{
    m_finishDungeons.insert(dungeonId);
}