#include "gameholder_pch.h"
#include "player.h"
#include "level_activity_manager.h"
#include "special_activity_config.h"
#include "activity_reward_config.h"
#include "item_manager.h"

LevelActivityManager::LevelActivityManager(Player* player) : m_player(player)
{
    m_acceptFlag = 0;
    m_activityId = SPECIAL_ACTIVITY_PLAYER_LEVEL;
}

LevelActivityManager::~LevelActivityManager()
{

}

void LevelActivityManager::LoadLevelActivityInfo( LEVEL_ACTIVITY_INFO& levelActivityInfo )
{
    m_acceptFlag = levelActivityInfo.accept_flag;
}

void LevelActivityManager::FillLevelActivityInfo( LEVEL_ACTIVITY_INFO& levelActivityInfo )
{
    levelActivityInfo.accept_flag = m_acceptFlag;
}

void LevelActivityManager::OnRecv( uint32 msgID, void * data )
{
    switch (msgID)
    {
    case MSG_SPECIAL_ACTIVITY_INFO_REQ:
        {
            OnLevelActivityInfoReq(data);
            break;
        }
    case MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_REQ:
        {
            OnLevelActivityAcceptRewardReq(data);
            break;
        }
    default:
        break;
    }

}

void LevelActivityManager::OnLevelActivityInfoReq( void *data )
{
    SPECIAL_ACTIVITY_INFO_REQ* req = (SPECIAL_ACTIVITY_INFO_REQ*) data;
    if ((!req) || (req->activity_id != m_activityId))
    {
        return;
    }

    SPECIAL_ACTIVITY_INFO_ACK ack;
    ack.Clear();
    ack.activity_id = req->activity_id;

    // 获取活动时间
    if (SpecialActivityConfig::Instance()->GetActivityTime(ack.activity_id, ack.start_time, ack.end_time) != ERR_SUCCEED)
    {
        ack.start_time = 0;
        ack.end_time = 0;
    }

    if (SpecialActivityConfig::Instance()->GetInfoByActivityId(ack.activity_id, ack.info_list) != ERR_SUCCEED)
    {
        ack.info_list.Clear();
    }
    else
    {
        // 在活动期间内,更新奖励标记
        if (SpecialActivityConfig::Instance()->CheckActivityTime(ack.activity_id) == ERR_SUCCEED)
        {
            UpdateFlag(ack.info_list, m_player->GetLevel(), m_acceptFlag);
        }
    }

    if (SpecialActivityConfig::Instance()->GetRewardByActivityId(ack.activity_id, ack.reward_list) != ERR_SUCCEED)
    {
        ack.reward_list.Clear();
    }

    m_player->SendToGate(MSG_SPECIAL_ACTIVITY_INFO_ACK, &ack);
}

void LevelActivityManager::OnLevelActivityAcceptRewardReq( void *data )
{
    SPECIAL_ACTIVITY_ACCEPT_REWARD_REQ* req = (SPECIAL_ACTIVITY_ACCEPT_REWARD_REQ*) data;
    if ((!req) || (req->activity_id != m_activityId))
        return;

    // 奖励信息
    const SpecialActivityInfo* info = SpecialActivityConfig::Instance()->GetActivityInfoByIdAndParam1(m_activityId, req->param);
    if (!info)
    {
        SendAcceptRewardAck(ERR_FAILED, info->param1);
        return;
    }

    // 检查等级
    if (m_player->GetLevel() < info->param2)
    {
        SendAcceptRewardAck(ERR_NO_ENOUGH_PLAYER_LEVEL, info->param1);
        return;
    }

    // 是否已领
    if (IsIndexAccept(info->param1))
    {
        SendAcceptRewardAck(ERR_FAILED, info->param1);
        return;
    }

    // 检查奖励能否放入背包
    const REWARD_TABLE* rewardTable = ActivityRewardConfig::Instance()->GetReward(info->rewardId);

    // 防沉迷
    REWARD_TABLE fcmReward;
    m_player->GetFcmRewardList(rewardTable->items, fcmReward.items);
    fcmReward.reason = rewardTable->reason;
    fcmReward.reason_arg = rewardTable->reason_arg;

    //ItemManager* bag = m_player->GetItemManager();
    //if(!bag || !bag->CanAddRewardTable(fcmReward))
    //{
    //    SendAcceptRewardAck(ERR_NO_ENOUGH_BAGPOS, info->param1);
    //    return;
    //}

    // 设置领奖标记
    SetIndexAcceptFlag(info->param1);
    SaveLevelActivity(info->param2);
}

void LevelActivityManager::UpdateFlag( SPECIAL_ACTIVITY_INFO_LIST& infoList, int32 level, int32 flag )
{
    // 获取活动配置信息成功,再设置状态
    for (uint32 i = 0; i < infoList.infos.size(); i++)
    {
        // 等级不足,直接跳出,因为param2递增
        if (level < infoList.infos[i].param2)
        {
            break;
        }

        // 位标记0:未领奖  1:已领奖
        infoList.infos[i].status = NO_REWARD;
        if (flag & (1 << i))
        {
            infoList.infos[i].status = ACCEPTED_REWARD;
            continue;
        }

        // 未领奖且等级足够
        infoList.infos[i].status = CAN_ACCEPT_REWARD;
    }
}

void LevelActivityManager::SaveLevelActivity( uint32 level )
{
    SAVE_LEVEL_ACTIVITY_INFO_REQ req;
    req.Clear();
    req.level_activity_info.accept_flag = m_acceptFlag;
    req.level = level;      // 如果填非0,存档完成后需给相应的等级奖励

    m_player->SendToCenter(MSG_SAVE_LEVEL_ACTIVITY_INFO_REQ, &req);
}

void LevelActivityManager::CBSaveLevelActivity( void* data )
{
    SAVE_LEVEL_ACTIVITY_INFO_ACK* recvAck = (SAVE_LEVEL_ACTIVITY_INFO_ACK*) data;
    // level非0,需给等级奖励
    if (recvAck->level > 0)
    {
        // 给奖励
        const SpecialActivityInfo* info = SpecialActivityConfig::Instance()->GetActivityInfoByIdAndParam2(m_activityId, recvAck->level);
        if (info)
        {
            const REWARD_TABLE* rewardTable = ActivityRewardConfig::Instance()->GetReward(info->rewardId);

            // 防沉迷
            REWARD_TABLE fcmReward;
            m_player->GetFcmRewardList(rewardTable->items, fcmReward.items);
            fcmReward.reason = rewardTable->reason;
            fcmReward.reason_arg = rewardTable->reason_arg;

            //ItemManager::AddRewardTableWithEmail(m_player->GetCenterSvrID(), m_player->GetPlayerID(), fcmReward);
        }

        SendAcceptRewardAck(ERR_SUCCEED, info->param1);
    }
}

void LevelActivityManager::SendAcceptRewardAck( uint32 errCode, int32 index )
{
    SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK ack;
    ack.Clear();
    ack.errcode = errCode;
    ack.activity_id = m_activityId;
    ack.param = index;

    if (SpecialActivityConfig::Instance()->GetInfoByActivityId(ack.activity_id, ack.info_list) != ERR_SUCCEED)
    {
        ack.info_list.Clear();
    }
    else
    {
        UpdateFlag(ack.info_list, m_player->GetLevel(), m_acceptFlag);
    }

    m_player->SendToGate(MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK, &ack);
}

void LevelActivityManager::SetLevelAcceptFlag( uint32 level )
{
    const SpecialActivityInfo* info = SpecialActivityConfig::Instance()->GetActivityInfoByIdAndParam2(m_activityId, level);
    uint32 mask = 1 << info->param1;
    m_acceptFlag |= mask;
}

bool LevelActivityManager::IsLevelAccept( uint32 level )
{
    const SpecialActivityInfo* info = SpecialActivityConfig::Instance()->GetActivityInfoByIdAndParam2(m_activityId, level);
    uint32 mask = 1 << info->param1;
    return m_acceptFlag & mask;
}

void LevelActivityManager::SetIndexAcceptFlag( uint32 index )
{    
    uint32 mask = 1 << index;
    m_acceptFlag |= mask;
}

bool LevelActivityManager::IsIndexAccept( uint32 index )
{
    uint32 mask = 1 << index;
    return m_acceptFlag & mask;
}

