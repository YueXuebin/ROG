#include "gameholder_pch.h"
#include "player.h"
#include "liveness_manager.h"
#include "liveness_config.h"
#include "common_reward_config.h"
#include "event_define.h"
#include "gameplaytimes_notifier.h"

LivenessManager::LivenessManager( Player* player ):m_player(player)
{
    Clear();
}

LivenessManager::~LivenessManager()
{

}

void LivenessManager::Clear()
{
    m_cur_liveness = 0;
    m_liveness_target_map.clear();
    m_liveness_reward_map.clear();

    LIVENESS_TARGET_INFO targetInfo;
    const LivenessTargetConfigMap* targetMap = LivenessConfig::Instance()->GetTargetMap();
    LivenessTargetConfigMap::const_iterator targetIter = targetMap->begin();
    for (; targetIter != targetMap->end(); targetIter++)
    {
        targetInfo.Clear();
        targetInfo.target = targetIter->second.target;
        targetInfo.need_count = targetIter->second.count;
        targetInfo.cur_count = 0;
        targetInfo.finish = 0;
        targetInfo.liveness = targetIter->second.liveness;

        m_liveness_target_map.insert(std::make_pair(targetInfo.target, targetInfo));
    }

    const LivenessRewardConfigMap* rewardMap = LivenessConfig::Instance()->GetRewardMap();
    LivenessRewardConfigMap::const_iterator rewardIter = rewardMap->begin();
    for (; rewardIter != rewardMap->end(); rewardIter++)
    {
        m_liveness_reward_map.insert(std::make_pair(rewardIter->second.index, NO_REWARD));
    }

}

void LivenessManager::LoadLivenessInfo( const LIVENESS_INFO& liveness_info )
{
    LivenessTargetStatus::iterator targetToIter;
    std::vector<LIVENESS_TARGET_INFO>::const_iterator targetFromIter = liveness_info.info_list.begin();
    for ( ; targetFromIter != liveness_info.info_list.end(); targetFromIter++)
    {
        targetToIter = m_liveness_target_map.find(targetFromIter->target);
        if (targetToIter != m_liveness_target_map.end())
        {
            // 只有当前次数,和完成标记存档,其余都是通过配置表获取
            targetToIter->second.cur_count = targetFromIter->cur_count;
            targetToIter->second.finish = targetFromIter->finish;
        }
    }

    for (uint32 i = 0; i < liveness_info.reward_list.size(); i++)
    {
        m_liveness_reward_map[i+1] = liveness_info.reward_list[i];
    }
}

void LivenessManager::FillLivenessInfo( LIVENESS_INFO& liveness_info )
{
    liveness_info.cur_liveness = m_cur_liveness;

    LivenessTargetStatus::iterator targetIter = m_liveness_target_map.begin();
    for ( ; targetIter != m_liveness_target_map.end();  targetIter++)
    {
        liveness_info.info_list.push_back(targetIter->second);
    }

    LivenessRewardStatus::iterator rewardIter = m_liveness_reward_map.begin();
    for ( ; rewardIter != m_liveness_reward_map.end();  rewardIter++)
    {
        liveness_info.reward_list.push_back(rewardIter->second);
    }
}

void LivenessManager::CalDailyRefresh()
{
    Clear();

    // 通知客户端可完成目标数量
    m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_LIVENESS, m_liveness_target_map.size());
}

void LivenessManager::OnRecv( uint32 msgID, void * data )
{
    switch (msgID)
    {
    case MSG_LIVENESS_INFO_REQ:
        {
            OnLivenessInfoReq(data);
            break;
        }
    case MSG_LIVENESS_ACCEPT_REWARD_REQ:
        {
            OnLivenessAcceptRewardReq(data);
            break;
        }
    default:
        break;
    }
}

void LivenessManager::OnLivenessInfoReq( void *data )
{
    // 先更新到最新状态
    Update();
    
    SendLivenessInfoAck();
}

void LivenessManager::OnLivenessAcceptRewardReq( void *data )
{
    LIVENESS_ACCEPT_REWARD_REQ* req = (LIVENESS_ACCEPT_REWARD_REQ*) data;
    if (!req)
        return;

    LivenessRewardStatus::iterator iter = m_liveness_reward_map.find(req->index);
    if (iter == m_liveness_reward_map.end())
    {
        SendLivenessAcceptRewardAck(ERR_FAILED, req->index);
        return;
    }

    // 先更新到最新状态
    Update();
    
    if (iter->second != CAN_ACCEPT_REWARD)
    {
        SendLivenessAcceptRewardAck(ERR_FAILED, req->index);
        return;
    }

    // 置标记
    iter->second = ACCEPTED_REWARD;

    // 给奖励
    uint32 commonRewardId = LivenessConfig::Instance()->GetRewardID(req->index);
    const REWARD_TABLE* rewardTable = CommonRewardConfig::Instance()->GetReward(commonRewardId);
    m_player->TakeInRewardTable(rewardTable);

    // 回执
    SendLivenessAcceptRewardAck(ERR_SUCCEED, req->index);

    // 领完奖励,再更新一下,因为要发目标个数和奖励标记-_-!....
    Update();

    m_player->SendPlaySystemLog(PLAYSYSTEM_LIVENESS, req->index);
}

void LivenessManager::Update()
{
    int32 unFinishNum = 0;
    int32 cur_liveness = 0;
    bool hasReward = false;

    // 计算当前活跃度
    LivenessTargetStatus::iterator targetIter = m_liveness_target_map.begin();
    for ( ; targetIter != m_liveness_target_map.end();  targetIter++)
    {
        const LivenessTarget *plt = LivenessConfig::Instance()->GetTarget(targetIter->second.target);
        if (!plt)
            continue;

        if (targetIter->second.finish)
        {
            cur_liveness += targetIter->second.liveness;
        }
        //else if (OpenSysConfig::Instance()->IsOpenSys(plt->sysId, m_player, false) == ERR_SUCCEED)
        //{
        //    unFinishNum++;
        //}
    }

    // 设置当前活跃度
    m_cur_liveness = cur_liveness;

    // 根据活跃度设置奖励标记
    const LivenessRewardConfigMap* rewardMap = LivenessConfig::Instance()->GetRewardMap();
    LivenessRewardConfigMap::const_iterator rewardIter = rewardMap->begin();
    for (; rewardIter != rewardMap->end(); rewardIter++)
    {
        if ((m_cur_liveness >= rewardIter->second.liveness) &&
            m_liveness_reward_map[rewardIter->second.index] != ACCEPTED_REWARD)
        {
            m_liveness_reward_map[rewardIter->second.index] = CAN_ACCEPT_REWARD;

            hasReward = true;
        }
    }

    // 通知客户端可完成目标数量
    m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_LIVENESS, unFinishNum);

    // 通知客户端是否有奖励可领
    m_player->GetNotifyManager()->RewardNotify(NOTIFY_REWARD_LIVENESS, hasReward);
}

void LivenessManager::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 )
{
    // 检查功能开启
    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_LIVENESS, m_player) != ERR_SUCCEED)
    //{
    //    return;
    //}

    // event与对应的targt相差3000
    OnUpdateTarget(eventId%LIVENESS_EVENT_BEGIN, arg1);
}

void LivenessManager::OnUpdateTarget( int32 target, int32 num )
{
    //CnDbg("LivenessManager::OnUpdateTarget %s target:%d, num:%d\n", m_player->GetName().c_str(), target, num);
    if (num < 1)
        return;

    LivenessTargetStatus::iterator iter = m_liveness_target_map.find(target);
    if (iter == m_liveness_target_map.end())
        return;

    // 已完成
    if (iter->second.finish)
        return;

    iter->second.cur_count += num;
    if (iter->second.cur_count >= iter->second.need_count)
    {
        iter->second.cur_count = iter->second.need_count;
        iter->second.finish = 1;
    }

    //CnDbg("LivenessManager::OnUpdateTarget %s cur:%d, need:%d\n", m_player->GetName().c_str(), iter->second.cur_count, iter->second.need_count);

    Update();
    SendLivenessInfoAck();
}

void LivenessManager::SendLivenessInfoAck()
{
    LIVENESS_INFO info;
    info.Clear();
    FillLivenessInfo(info);

    m_player->SendToGate(MSG_LIVENESS_INFO_ACK, &info);
}

void LivenessManager::SendLivenessAcceptRewardAck( int32 errCode, int32 index )
{
    LIVENESS_ACCEPT_REWARD_ACK ack;
    ack.Clear();
    ack.errcode = errCode;
    ack.index = index;

    m_player->SendToGate(MSG_LIVENESS_ACCEPT_REWARD_ACK, &ack);
}
