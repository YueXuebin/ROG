#include "gameholder_pch.h"
#include "player.h"
#include "special_activity_manager_gs.h"
#include "special_activity_config.h"
#include "activity_reward_config.h"
#include "item_manager.h"
#include "level_activity_manager.h"
#include "wing_manager.h"

SpecialActivityManager::SpecialActivityManager(Player* player) : m_player(player)
{
    m_upgrade_wing_status = -1;
    m_micro_client_status = -1;
}

SpecialActivityManager::~SpecialActivityManager()   
{

}

void SpecialActivityManager::OnRecv( uint32 msgID, void * data )
{
    switch (msgID)
    {
    case MSG_FIRST_TIME_RECHARGE_INFO_REQ:
        {
            OnFirstTimeRechargeInfoReq(data);
            break;
        }
    case MSG_FIRST_TIME_RECHARGE_ACCEPT_REWARD_REQ:
        {
            OnFirstTimeRechargeAcceptRewardReq(data);
            break;
        }
    case MSG_FIRST_TIME_RECHARGE_ACCEPT_REWARD_ACK:
        {
            OnFirstTimeRechargeAcceptRewardAck(data);
            break;
        }
    case MSG_SPECIAL_ACTIVITY_INFO_REQ:                 // 请求活动信息
        {
            OnSpecialActivityInfoReq(msgID, data);
            break;
        }
    case MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_REQ:
        {
            OnSpecialActivityAcceptRewardReq(msgID, data);
            break;
        }
    case MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK:
        {
            OnSpecialActivityAcceptRewardAck(msgID, data);
            break;
        }
    case MSG_SPECIAL_ACTIVITY_LOAD_INFO_ACK:
        {
            OnSpecialActivityLoadInfoAck(msgID, data);
            break;
        }
    case MSG_SPECIAL_ACTIVITY_SAVE_INFO_ACK:
        {
            OnSpecialActivitySaveInfoAck(msgID, data);
            break;
        }
    default:
        break;
    }
}

void SpecialActivityManager::SetAcceptedReward( int32& status, int32 index )
{
    status |= (1 << index);
}

bool SpecialActivityManager::IsAcceptedReward( int32 status, int32 index )
{
    return (status & (1 << index));
}

void SpecialActivityManager::OnFirstTimeRechargeInfoReq( void *data )
{
    m_player->SendToCenter(MSG_FIRST_TIME_RECHARGE_INFO_REQ, data);
}

void SpecialActivityManager::OnFirstTimeRechargeAcceptRewardReq( void *data )
{
    REWARD_TABLE rewardTable;

    int32 errcode = SpecialActivityConfig::Instance()->GetRewardByActivityIdAndIndex(SPECIAL_ACTIVITY_FIRST_TIME_RECHARGE, 0, rewardTable);
    if ( errcode != ERR_SUCCEED )
    {
        SendFirstTimeRechargeAcceptRewardAck(errcode);
        return;
    }

    errcode = SpecialActivityConfig::Instance()->CheckActivityTime(SPECIAL_ACTIVITY_FIRST_TIME_RECHARGE);
    if ( errcode != ERR_SUCCEED )
    {
        SendFirstTimeRechargeAcceptRewardAck(errcode);
        return;
    }

    //ItemManager* bag = m_player->GetItemManager();
    //if(!bag || !bag->CanAddRewardTable(rewardTable))
    //{
    //    errcode = ERR_NO_ENOUGH_BAGPOS;
    //    SendFirstTimeRechargeAcceptRewardAck(errcode);
    //    return;
    //}

    m_player->SendToCenter(MSG_FIRST_TIME_RECHARGE_ACCEPT_REWARD_REQ, data);
}

void SpecialActivityManager::OnFirstTimeRechargeAcceptRewardAck( void *data )
{
    FIRST_TIME_RECHARGE_ACCEPT_REWARD_ACK* ack = (FIRST_TIME_RECHARGE_ACCEPT_REWARD_ACK*) data;
    if (ack->errcode != ERR_SUCCEED)
    {
        SendFirstTimeRechargeAcceptRewardAck(ack->errcode);
        return;
    }

    REWARD_TABLE rewardTable;
    rewardTable.Clear();
    int32 errcode = SpecialActivityConfig::Instance()->GetRewardByActivityIdAndIndex(SPECIAL_ACTIVITY_FIRST_TIME_RECHARGE, 0, rewardTable);
    if ( errcode != ERR_SUCCEED )
    {
        SendFirstTimeRechargeAcceptRewardAck(errcode);
        return;
    }

    //ItemManager::AddRewardTableWithEmail(m_player->GetCenterSvrID(), m_player->GetPlayerID(), rewardTable);
    errcode = ERR_SUCCEED;
    SendFirstTimeRechargeAcceptRewardAck(errcode);
}

void SpecialActivityManager::SendFirstTimeRechargeAcceptRewardAck( uint32 errCode)
{
    FIRST_TIME_RECHARGE_ACCEPT_REWARD_ACK ack;
    ack.Clear();
    ack.errcode = errCode;
    m_player->SendToGate(MSG_FIRST_TIME_RECHARGE_ACCEPT_REWARD_ACK, &ack);
}

void SpecialActivityManager::OnSpecialActivityInfoReq( uint32 msgID, void *data )
{
    SPECIAL_ACTIVITY_INFO_REQ* req = (SPECIAL_ACTIVITY_INFO_REQ*) data;
    if (!req)
    {
        return;
    }

    switch (req->activity_id)
    {
    case SPECIAL_ACTIVITY_BATTLE_POWER_RANKING:
        {
            // 战力排行信息直接从gs获取,因为不需要领奖标记
            SendSpecialActivityInfoToPlayer(req->activity_id, 0, 0);
            break;
        }
    case SPECIAL_ACTIVITY_DAILY_RECHARGE:
        {
            m_player->SendToCenter(msgID, data);
            break;
        }
    case SPECIAL_ACTIVITY_PLAYER_LEVEL:
        {
            m_player->GetLevelActivityManager()->OnRecv(msgID, data);
            break;
        }
    case SPECIAL_ACTIVITY_UPGRADE_WING:
        {
            // 将当前玩家的翅膀阶数给gc,gc把状态直接给玩家
            req->param = m_player->GetWingManager()->GetWingLevel();
            m_player->SendToCenter(msgID, req);
            break;
        }
    case SPECIAL_ACTIVITY_MICRO_CLIENT:         // 请求微端奖励信息
        {
            // 从CenterServer获取当前的奖励状态
            SPECIAL_ACTIVITY_STORE_INFO info;            
            info.activity_id = req->activity_id;
            m_player->SendToCenter(MSG_SPECIAL_ACTIVITY_LOAD_INFO_REQ, &info);
            break;
        }
    default:
        break;
    }
}

void SpecialActivityManager::SendSpecialActivityInfoToPlayer( int32 activityId, int32 param, int32 status )
{
    SPECIAL_ACTIVITY_INFO_ACK ack;
    
    ack.activity_id = activityId;
    ack.param = param;

    // 获取活动时间
    if (SpecialActivityConfig::Instance()->GetActivityTime(ack.activity_id, ack.start_time, ack.end_time) != ERR_SUCCEED)
    {
        ack.start_time = 0;
        ack.end_time = 0;
    }

    if (SpecialActivityConfig::Instance()->GetInfoByActivityId(activityId, ack.info_list) != ERR_SUCCEED)
    {
        ack.info_list.Clear();
    }
    else
    {
        // 在活动期间内,更新奖励标记
        if (SpecialActivityConfig::Instance()->CheckActivityTime(ack.activity_id) == ERR_SUCCEED)
        {
            SpecialActivityConfig::Instance()->UpdateStatus(ack.info_list, param, status);
        }
    }

    if (SpecialActivityConfig::Instance()->GetRewardByActivityId(activityId, ack.reward_list) != ERR_SUCCEED)
    {
        ack.reward_list.Clear();
    }
    
    m_player->SendToGate(MSG_SPECIAL_ACTIVITY_INFO_ACK, &ack);
}

void SpecialActivityManager::SendSpecialActivityRewardToPlayer( int32 errCode, int32 activityId, int32 index, int32 param )
{
    SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK ack;
    ack.Clear();
    ack.errcode = errCode;
    ack.activity_id = activityId;
    ack.param = param;

    // 获取活动配置信息
    if (SpecialActivityConfig::Instance()->GetInfoByActivityId(ack.activity_id, ack.info_list) != ERR_SUCCEED)
    {
        ack.info_list.Clear();
        m_player->SendToGate(MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK, &ack);
        return;
    }

    // 越界检查
    if (index >= (int32)ack.info_list.infos.size())
    {
        return;
    }

    // 更新状态
    if(SPECIAL_ACTIVITY_UPGRADE_WING == ack.activity_id)
    {
        SpecialActivityConfig::Instance()->UpdateStatus(ack.info_list, ack.param, m_upgrade_wing_status);
    }
    else if(SPECIAL_ACTIVITY_MICRO_CLIENT == ack.activity_id)
    {
        SpecialActivityConfig::Instance()->UpdateStatus(ack.info_list, ack.param, m_micro_client_status);
    }
    else
    {
        CnAssert(false);
        return;
    }

    // 给奖励
    REWARD_TABLE rewardTable;

    int32 errcode = SpecialActivityConfig::Instance()->GetRewardByActivityIdAndIndex(activityId, index, rewardTable);
    if ( errcode != ERR_SUCCEED )
    {
        ack.errcode = errcode;
        m_player->SendToGate(MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK, &ack);
        return;
    }

    // 防沉迷
    REWARD_TABLE fcmRewardTable;
    fcmRewardTable.Clear();
    m_player->GetFcmRewardList(rewardTable.items, fcmRewardTable.items);
    fcmRewardTable.reason = rewardTable.reason;
    fcmRewardTable.reason_arg = rewardTable.reason_arg;
    //ItemManager::AddRewardTableWithEmail(m_player->GetCenterSvrID(), m_player->GetPlayerID(), rewardTable);

    m_player->SendToGate(MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK, &ack);
}

void SpecialActivityManager::OnSpecialActivityAcceptRewardReq( uint32 msgID, void *data )
{
    SPECIAL_ACTIVITY_ACCEPT_REWARD_REQ* req = (SPECIAL_ACTIVITY_ACCEPT_REWARD_REQ*) data;
    if (!req)
    {
        return;
    }

    if (SpecialActivityConfig::Instance()->CheckActivityTime(req->activity_id) != ERR_SUCCEED)
    {
        return;
    }

    switch (req->activity_id)
    {
    case SPECIAL_ACTIVITY_BATTLE_POWER_RANKING:
        {
            // 战力排行榜奖励,自动发,不需要领取
            break;
        }
    case SPECIAL_ACTIVITY_DAILY_RECHARGE:
        {
            // 转发请求到gc
            m_player->SendToCenter(msgID, data);
            break;
        }
    case SPECIAL_ACTIVITY_PLAYER_LEVEL:
        {
            m_player->GetLevelActivityManager()->OnRecv(msgID, data);
            break;
        }
    case SPECIAL_ACTIVITY_UPGRADE_WING:
        {
            OnUpgradeWingActivityAcceptRewardReq(req);
            break;
        }
    case SPECIAL_ACTIVITY_MICRO_CLIENT:
        {
            OnMicroClientActivityAcceptRewardReq(req);
            break;
        }
    default:
        break;
    }

}

void SpecialActivityManager::OnSpecialActivityAcceptRewardAck( uint32 msgID, void *data )
{
    SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK* ack = (SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK*) data;
    if (!ack)
    {
        return;
    }

    switch (ack->activity_id)
    {
    case SPECIAL_ACTIVITY_DAILY_RECHARGE:
        {
            if (ack->errcode == ERR_SUCCEED)
            {
                // 给奖励
                REWARD_TABLE rewardTable;
                rewardTable.Clear();
                int32 errcode = SpecialActivityConfig::Instance()->GetRewardByActivityIdAndIndex(ack->activity_id, ack->param, rewardTable);
                if ( errcode != ERR_SUCCEED )
                {
                    ack->errcode = errcode;
                    m_player->SendToGate(msgID, ack);
                    return;
                }

                // 防沉迷
                REWARD_TABLE fcmRewardTable;
                fcmRewardTable.Clear();
                m_player->GetFcmRewardList(rewardTable.items, fcmRewardTable.items);
                fcmRewardTable.reason = rewardTable.reason;
                fcmRewardTable.reason_arg = rewardTable.reason_arg;
                //ItemManager::AddRewardTableWithEmail(m_player->GetCenterSvrID(), m_player->GetPlayerID(), rewardTable);
            }

            m_player->SendToGate(msgID, ack);
            break;   
        }
    default:
        break;
    }

}

void SpecialActivityManager::SendSpecialActivitySaveInfoReq( int32 activityId, int32 status, int32 param )
{
    // 先让gc存档领奖标记,再通知gs给客户端发奖励
    SPECIAL_ACTIVITY_STORE_INFO info;
    info.Clear();
    info.activity_id = activityId;
    info.status = status;
    info.param = param; //将index传给gc,回来时根据param给奖励
    m_player->SendToCenter(MSG_SPECIAL_ACTIVITY_SAVE_INFO_REQ, &info);
}

// gc存档活动信息之后,gs需要给相应的奖励
void SpecialActivityManager::OnSpecialActivitySaveInfoAck( uint32 msgID, void *data )
{
    SPECIAL_ACTIVITY_STORE_INFO* ack = (SPECIAL_ACTIVITY_STORE_INFO*) data;
    if (!ack)
    {
        return;
    }

    switch (ack->activity_id)
    {
    case SPECIAL_ACTIVITY_BATTLE_POWER_RANKING:
        {
            break;
        }
    case SPECIAL_ACTIVITY_DAILY_RECHARGE:
        {
            break;
        }
    case SPECIAL_ACTIVITY_PLAYER_LEVEL:
        {
            break;
        }
    case SPECIAL_ACTIVITY_UPGRADE_WING:
        {
            SendSpecialActivityRewardToPlayer(ERR_SUCCEED, ack->activity_id, ack->param, m_player->GetWingManager()->GetWingLevel());
            break;
        }
    case SPECIAL_ACTIVITY_MICRO_CLIENT:
        {
            int microClientMark = 1;
            SendSpecialActivityRewardToPlayer(ERR_SUCCEED, ack->activity_id, ack->param, microClientMark);
            break;
        }
    default:
        break;
    }

}

void SpecialActivityManager::OnSpecialActivityLoadInfoReq( uint32 msgID, void *data )
{

}

void SpecialActivityManager::OnSpecialActivityLoadInfoAck( uint32 msgID, void *data )
{
    SPECIAL_ACTIVITY_STORE_INFO* ack = (SPECIAL_ACTIVITY_STORE_INFO*) data;

    switch (ack->activity_id)
    {
    case SPECIAL_ACTIVITY_BATTLE_POWER_RANKING:
        {
            break;
        }
    case SPECIAL_ACTIVITY_DAILY_RECHARGE:
        {
            break;
        }
    case SPECIAL_ACTIVITY_PLAYER_LEVEL:
        {
            break;
        }
    case SPECIAL_ACTIVITY_UPGRADE_WING:
        {
            m_upgrade_wing_status = ack->status;
            break;
        }
    case SPECIAL_ACTIVITY_MICRO_CLIENT:
        {
            m_micro_client_status = ack->status;
            int microClientMark = 1;
            SendSpecialActivityInfoToPlayer(ack->activity_id, microClientMark, m_micro_client_status);
            break;
        }
    default:
        break;
    }
}

void SpecialActivityManager::OnUpgradeWingActivityAcceptRewardReq( SPECIAL_ACTIVITY_ACCEPT_REWARD_REQ* req )
{
    if (m_upgrade_wing_status < 0)
    {
        // m_upgrade_wing_status小于0,表明未从gc同步状态
        return;
    }

    SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK ack;
    ack.Clear();
    ack.errcode = ERR_FAILED;
    ack.activity_id = req->activity_id;
    ack.param = m_player->GetWingManager()->GetWingLevel();

    // 获取活动配置信息
    if (SpecialActivityConfig::Instance()->GetInfoByActivityId(ack.activity_id, ack.info_list) != ERR_SUCCEED)
    {
        ack.info_list.Clear();
        m_player->SendToGate(MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK, &ack);
        return;
    }

    // 越界检查
    if (req->param >= (int32)ack.info_list.infos.size())
    {
        return;
    }

    // 更新状态
    SpecialActivityConfig::Instance()->UpdateStatus(ack.info_list, ack.param, m_upgrade_wing_status);

    // 检查领奖标记
    if (ack.info_list.infos[req->param].status != CAN_ACCEPT_REWARD)
    {
        m_player->SendToGate(MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK, &ack);
        return;
    }

    // 置领奖标记位
    SetAcceptedReward(m_upgrade_wing_status, req->param);
    ack.info_list.infos[req->param].status = ACCEPTED_REWARD;

    // 先存档领奖标记,再通知gs给客户端发奖励
    SendSpecialActivitySaveInfoReq(req->activity_id, m_upgrade_wing_status, req->param);
}

void SpecialActivityManager::OnMicroClientActivityAcceptRewardReq( SPECIAL_ACTIVITY_ACCEPT_REWARD_REQ* req )
{
    if (m_micro_client_status < 0)
    {
        // m_micro_client_status小于0,表明未从CenterSever同步状态
        CnDbg("player %s m_micro_client_status < 0\n", m_player->GetName().c_str());
        m_player->NotifyErrorCode(ERR_MICRO_CLIENT_ACTIVITY_NO_STATUS);
        return;
    }

    SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK ack;
    ack.Clear();
    ack.errcode = ERR_FAILED;
    ack.activity_id = req->activity_id;
    int32 microClientMark = 1;
    ack.param = microClientMark;

    // 获取活动配置信息
    if (SpecialActivityConfig::Instance()->GetInfoByActivityId(ack.activity_id, ack.info_list) != ERR_SUCCEED)
    {
        ack.info_list.Clear();
        m_player->SendToGate(MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK, &ack);
        return;
    }

    // 越界检查
    if (req->param >= (int32)ack.info_list.infos.size())
    {
        m_player->NotifyErrorCode(ERR_MICRO_CLIENT_ACTIVITY_NO_RANGE);
        return;
    }

    // 更新状态
    SpecialActivityConfig::Instance()->UpdateStatus(ack.info_list, ack.param, m_micro_client_status);

    // 检查领奖标记
    if (ack.info_list.infos[req->param].status != CAN_ACCEPT_REWARD)
    {
        m_player->SendToGate(MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK, &ack);
        return;
    }

    // 置领奖标记位
    SetAcceptedReward(m_micro_client_status, req->param);
    ack.info_list.infos[req->param].status = ACCEPTED_REWARD;

    // 先存档领奖标记,再通知gs给客户端发奖励
    SendSpecialActivitySaveInfoReq(req->activity_id, m_micro_client_status, req->param);
}
