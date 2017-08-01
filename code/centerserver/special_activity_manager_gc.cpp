#include "centerserver_pch.h"
#include "special_activity_manager_gc.h"
#include "special_activity_config.h"
#include "activity_reward_config.h"
#include "special_activity_job.h"
#include "ranking_loader_job.h"
#include "mail_manager.h"
#include "pay_job.h"
#include "player.h"
#include "player_manager.h"
#include "festival_manager.h"
#include "mail_title_config.h"

IMPLEMENT_SINGLETON(SpecialActivityManager)

SpecialActivityManager::SpecialActivityManager()
{

}

SpecialActivityManager::~SpecialActivityManager()
{

}

int32 SpecialActivityManager::GetDaysBetweenDates(time_t t1, time_t t2)
{
    Crown::CSDDateTime localDay1 = Crown::CSDDateTime(t1);
    Crown::CSDDateTime localDay2 = Crown::CSDDateTime(t2);

    if (t1 == 0)
        localDay1.SetDate(1970, 1, 2);
    if (t2 == 0)
        localDay2.SetDate(1970, 1, 2);

    localDay1.SetTime(0, 0, 0);
    localDay2.SetTime(0, 0, 0);

    int64 diffDay = localDay1.DiffDayNoDst(localDay2);
    return (int32)diffDay;
}

void SpecialActivityManager::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_RANKING_SAVED_NTF:
        {
            OnBattlePowerRankingRefresh();
            break;
        }
    case MSG_SPECIAL_ACTIVITY_BATTLE_POWER_ACTIVITY_ACK:
        {
            OnGetBattlePowerActivityInfoAck(data);
            break;
        }
    case MSG_SPECIAL_ACTIVITY_BATTLE_POWER_RANKING_ACK:
        {
            OnGetBattlePowerRankingAck(data);
            break;
        }
    case MSG_FIRST_TIME_RECHARGE_INFO_REQ:
        {
            OnFirstTimeRechargeInfoReq(playerID, data);
            break;
        }
    case MSG_FIRST_TIME_RECHARGE_ACCEPT_REWARD_REQ:
        {
            OnFirstTimeRechargeAcceptRewardReq(playerID, data);
            break;
        }
    case MSG_SPECIAL_ACTIVITY_INFO_REQ:
        {
            OnSpecialActivityInfoReq(playerID, data);
            break;
        }
    case MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_REQ:
        {
            OnSpecialActivityAcceptRewardReq(playerID, data);
            break;
        }
    case MSG_SPECIAL_ACTIVITY_SAVE_INFO_REQ:
        {
            SendSpecialActivitySaveInfoReq(playerID, data);
            break;
        }
    case MSG_SPECIAL_ACTIVITY_LOAD_INFO_REQ:
        {
            OnSpecialActivityLoadInfoReq(playerID, data);
            break;
        }
    default:
        break;
    }
}

//--------------------------------------------SpecialActivity------------------------------------------------------------

void SpecialActivityManager::OnSpecialActivityInfoReq( uint64 playerID, void *data )
{
    SPECIAL_ACTIVITY_INFO_REQ* req = (SPECIAL_ACTIVITY_INFO_REQ*) data;
    if (!req)
        return;

    switch (req->activity_id)
    {
    case SPECIAL_ACTIVITY_DAILY_RECHARGE:
        {
            OnDailyRechargeInfoReq(playerID, data);
            break;
        }
    case SPECIAL_ACTIVITY_UPGRADE_WING:
        {
            OnUpgradeWingInfoReq(playerID, data);
            break;
        }
    default:
        break;
    }

}

void SpecialActivityManager::OnSpecialActivityAcceptRewardReq( uint64 playerID, void *data )
{
    SPECIAL_ACTIVITY_ACCEPT_REWARD_REQ* req = (SPECIAL_ACTIVITY_ACCEPT_REWARD_REQ*) data;
    if (!req)
        return;

    switch (req->activity_id)
    {
    case SPECIAL_ACTIVITY_DAILY_RECHARGE:
        {
            OnDailyRechargeAcceptRewardReq(playerID, data);
            break;
        }
    default:
        break;
    }
}

void SpecialActivityManager::SendSpecialActivitySaveInfoReq( uint64 playerID, void *data )
{
    SPECIAL_ACTIVITY_STORE_INFO* req = (SPECIAL_ACTIVITY_STORE_INFO*) data;
    if (!req)
        return;

    switch (req->activity_id)
    {
    case SPECIAL_ACTIVITY_UPGRADE_WING:
        {
            if (SpecialActivityJob::Instance()->SaveUpgradeWingActivityInfo(playerID, req->status))
            {
                Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
                if (!player || (player->GetState() < Player::kStateInGame))
                {
                    return;
                }

                player->SendToGame(MSG_SPECIAL_ACTIVITY_SAVE_INFO_ACK, req);
            }
            break;
        }
    case SPECIAL_ACTIVITY_MICRO_CLIENT:
        {
            if (SpecialActivityJob::Instance()->SaveMicroClientActivityInfo(playerID, req->status))
            {
                Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
                if (!player || (player->GetState() < Player::kStateInGame))
                {
                    if(player)
                        CnWarn("player %s req micro client reward but not in kStateInGame\n", player->GetName().c_str());
                    return;
                }

                player->SendToGame(MSG_SPECIAL_ACTIVITY_SAVE_INFO_ACK, req);
            }
            break;
        }
    default:
        break;
    }
}

void SpecialActivityManager::OnSpecialActivityLoadInfoReq( uint64 playerID, void *data )
{
    SPECIAL_ACTIVITY_STORE_INFO* req = (SPECIAL_ACTIVITY_STORE_INFO*) data;
    if (!req)
        return;

    switch (req->activity_id)
    {
    case SPECIAL_ACTIVITY_MICRO_CLIENT:
        {
            GetMicroClientActivityInfo(playerID);
            break;
        }
    default:
        break;
    }
}

void SpecialActivityManager::SendSpecialActivityLoadInfoAck( uint64 playerID, int32 activityId, int32 status, int32 param )
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if (!player || (player->GetState() < Player::kStateInGame))
    {
        CnWarn("Special Activity Info %d Load, But player not in game\n", activityId);
        return;
    }

    SPECIAL_ACTIVITY_STORE_INFO info;    
    info.activity_id = activityId;
    info.status = status;
    info.param = param;
    player->SendToGame(MSG_SPECIAL_ACTIVITY_LOAD_INFO_ACK, &info);
}

//--------------------------------------------BattlePowerRanking------------------------------------------------------------

void SpecialActivityManager::OnBattlePowerRankingRefresh()
{
    // 活动时间检查
    if (SpecialActivityConfig::Instance()->CheckActivityTime(SPECIAL_ACTIVITY_BATTLE_POWER_RANKING) != ERR_SUCCEED)
    {
        return;
    }

    // 异步获取上次战力排行给奖时间
    SpecialActivityJob::Instance()->GetBattlePowerActivityInfo(MSG_SPECIAL_ACTIVITY_BATTLE_POWER_ACTIVITY_REQ);
}

void SpecialActivityManager::OnGetBattlePowerActivityInfoAck( void* data )
{
    SPECIAL_ACTIVITY_BATTLE_POWER_ACTIVITY_INFO* info = (SPECIAL_ACTIVITY_BATTLE_POWER_ACTIVITY_INFO*) data;
    if (!info)
        return;

    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
    if ((info->time > 0) && (GetDaysBetweenDates(nowTime, info->time) <= 0))
        return;

    // 存档发奖时间
    info->time = nowTime;
    if (SpecialActivityJob::Instance()->SaveBattlePowerActivityInfo(info))
    {
        GiveBattlePowerRankingReward();
    }
}

void SpecialActivityManager::GiveBattlePowerRankingReward()
{
    const SpecialActivity* activity = SpecialActivityConfig::Instance()->GetActivity(SPECIAL_ACTIVITY_BATTLE_POWER_RANKING);
    if (!activity)
        return;

    // 异步获取排行,在CBGetBattlePowerRanking中给奖励
    RankingLoaderJob::Instance()->GetBattlePowerRanking(activity->param, MSG_SPECIAL_ACTIVITY_BATTLE_POWER_RANKING_REQ);
}

void SpecialActivityManager::OnGetBattlePowerRankingAck( void* data )
{
    SPECIAL_ACTIVITY_BATTLE_POWER_RANKING_ACK* ack  = (SPECIAL_ACTIVITY_BATTLE_POWER_RANKING_ACK*) data;

    if (!ack)
        return;

    if (ack->ranking_list.empty())
        return;

    // 战力排行奖励
    //const TASK_FESTIVAL_TEMPLATE* festivalTmpl = FestivalManager::Instance()->GetFestivalTemplate(SPECIAL_ACTIVITY_BATTLE_POWER_RANKING);

    //const SpecialActivity* activity = SpecialActivityConfig::Instance()->GetActivity(SPECIAL_ACTIVITY_BATTLE_POWER_RANKING);
    //if (!activity)
    //    return;

    //SpecialActivityInfoList::const_iterator iter = activity->infoList.begin();
    //for (; iter != activity->infoList.end(); iter++)
    //{
    //   //param1:小排名 param2:大排名
    //   CnAssert(iter->param1 <= iter->param2);
    //   for (int32 i = iter->param1; i <= iter->param2; i++)
    //   {
    //       // 排名人数不足
    //       if (i > (int32)ack->ranking_list.size())
    //           return;

    //       SPECIAL_ACTIVITY_RANKING_INFO* info = &(ack->ranking_list[i-1]);
    //       if (info)
    //       {
    //           const REWARD_TABLE* rewardTable = ActivityRewardConfig::Instance()->GetReward(iter->rewardId);
    //           if (rewardTable)
    //           {
    //               // 发送邮件
    //               MailManager::Instance()->SendMessageMail(0, info->name, rewardTable->items, MAIL_MESSAGE_TYPE_BATTLE_POWER_RANKING, 1, Crown::ToString(i).c_str());
    //               CnInfo("Send Battle Power Ranking Succeed, player=%s , rewardId=%d !\n", info->name.c_str(), iter->rewardId);
    //           }
    //       }
    //   }
    //}
}

//--------------------------------------------FirstTimeRecharge------------------------------------------------------------

void SpecialActivityManager::GetFirstTimeRechargeActivityInfo( uint64 playerID, std::string type )
{
    // 获取首冲奖励状态标记
    SpecialActivityJob::Instance()->GetFirstTimeRechargeActivityInfo(playerID, type);
}

void SpecialActivityManager::CBGetFirstTimeRechargeActivityInfo( uint64 playerID, std::string type, int32 status )
{
    if (type == "info")
    {
        CBOnFirstTimeRechargeInfoReq(playerID, status);
    }
    else if (type == "accept_reward")
    {
        CBOnFirstTimeRechargeAcceptRewardReq(playerID, status);
    }
}

void SpecialActivityManager::OnFirstTimeRechargeInfoReq( uint64 playerID, void* data )
{
    if (SpecialActivityConfig::Instance()->CheckActivityTime(SPECIAL_ACTIVITY_FIRST_TIME_RECHARGE) != ERR_SUCCEED)
    {
        SendFirstTimeRechargeInfoAck(playerID, 0);
        return;
    }

    GetFirstTimeRechargeActivityInfo(playerID, "info");
}

void SpecialActivityManager::CBOnFirstTimeRechargeInfoReq( uint64 playerID, int32 status )
{
    // 可领奖,或已领奖,直接发送回执
    if ((status == CAN_ACCEPT_REWARD) || (status == ACCEPTED_REWARD))
    {
        SendFirstTimeRechargeInfoAck(playerID, status);
        return;
    }

    // 当前无奖励,则获取当前充值总额
    PayJob::Instance()->GetTotalRechargeByPlayerId(playerID);
}

void SpecialActivityManager::CBGetTotalRechargeByPlayerId( uint64 playerID, uint32 moneyPoint )
{
    // 已有充值金额
    if (moneyPoint > 0)
    {
        if (SpecialActivityJob::Instance()->SaveFirstTimeRechargeActivityInfo(playerID, CAN_ACCEPT_REWARD))
        {
            SendFirstTimeRechargeInfoAck(playerID, CAN_ACCEPT_REWARD);
            return;
        }
    }

    SendFirstTimeRechargeInfoAck(playerID, NO_REWARD);
}

void SpecialActivityManager::SendFirstTimeRechargeInfoAck( uint64 playerID, int32 status )
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if (!player || (player->GetState() < Player::kStateInGame))
        return;

    FIRST_TIME_RECHARGE_INFO info;
    info.Clear();
    info.status = status;
    if (SpecialActivityConfig::Instance()->GetRewardByActivityIdAndIndex(SPECIAL_ACTIVITY_FIRST_TIME_RECHARGE, 0, info.rewards) != ERR_SUCCEED)
    {
        info.rewards.Clear();
    }

    if (SpecialActivityConfig::Instance()->GetActivityTime(SPECIAL_ACTIVITY_FIRST_TIME_RECHARGE, info.start_time, info.end_time) != ERR_SUCCEED)
    {
        info.start_time = 0;
        info.end_time = 0;
    }

    player->SendToGate(MSG_FIRST_TIME_RECHARGE_INFO_ACK, &info);
}

void SpecialActivityManager::OnFirstTimeRechargeAcceptRewardReq( uint64 playerID, void* data )
{
    GetFirstTimeRechargeActivityInfo(playerID, "accept_reward");
}

void SpecialActivityManager::CBOnFirstTimeRechargeAcceptRewardReq( uint64 playerID, int32 status )
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if (!player || (player->GetState() < Player::kStateInGame))
    {
        return;
    }

    FIRST_TIME_RECHARGE_ACCEPT_REWARD_ACK ack;
    ack.Clear();
    ack.errcode = ERR_FAILED;

    if (status == CAN_ACCEPT_REWARD)
    {
        // 可领奖时,先把标记位置成已领奖,再通知gs给奖
        if (SpecialActivityJob::Instance()->SaveFirstTimeRechargeActivityInfo(playerID, ACCEPTED_REWARD))
        {
            ack.errcode = ERR_SUCCEED;
        }
    }

    player->SendToGame(MSG_FIRST_TIME_RECHARGE_ACCEPT_REWARD_ACK, &ack);
}

//--------------------------------------------DailyRecharge------------------------------------------------------------

void SpecialActivityManager::GetDailyRechargeActivityInfo( uint64 playerID, void* data, std::string type )
{
    // 获取今天之前的日常充值活动信息
    // 如有未领的奖励,通过邮件发奖,同时将老的充值信息删除
    SpecialActivityJob::Instance()->GetOldDailyRechargeActivityInfo(playerID);

    // 获取今天的日常充值活动信息
    int32 param = -1;
    if (type == "accept_reward")
    {
        SPECIAL_ACTIVITY_ACCEPT_REWARD_REQ* req = (SPECIAL_ACTIVITY_ACCEPT_REWARD_REQ*)data;
        param = req->param;
    }
    SpecialActivityJob::Instance()->GetCurDailyRechargeActivityInfo(playerID, param, type);
}

void SpecialActivityManager::CBGetOldDailyRechargeActivityInfo( uint64 playerID, DailyRechargeActivityInfoList& oldInfoList, int32 time)
{
    if (oldInfoList.empty())
        return;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if (!player)
    {
        return;
    }

    // 先将老的充值活动信息删除,再给奖励
    if (SpecialActivityJob::Instance()->DeleteOldDailyRechargeActivityInfo(playerID, time))
    {
        SPECIAL_ACTIVITY_INFO_LIST infoList;
        infoList.Clear();
        if (SpecialActivityConfig::Instance()->GetInfoByActivityId(SPECIAL_ACTIVITY_DAILY_RECHARGE, infoList) != ERR_SUCCEED)
        {
            return;
        }

        // 给奖励
        REWARD_TABLE rewardTable;
        DailyRechargeActivityInfoList::iterator iter = oldInfoList.begin();
        for (; iter != oldInfoList.end(); iter++)
        {
            rewardTable.Clear();

            // 获取活动配置信息成功,再设置状态
            for (uint32 i = 0; i < infoList.infos.size(); i++)
            {
                // 位标记0:未领奖  1:已领奖
                if (iter->status & (1 << i))
                    continue;

                // 充值的钱不够,直接跳出,因为param2递增
                if (iter->money_point < (int32)infoList.infos[i].param2)
                    break;

                SpecialActivityConfig::Instance()->GetRewardByActivityIdAndIndex(SPECIAL_ACTIVITY_DAILY_RECHARGE, i, rewardTable);
            }

            if (rewardTable.items.size() > 0)
            {
                // 每一天的奖励发送一封邮件
                //MailManager::Instance()->SendMail(0, player->GetName(), MailTitleConfig::Instance()->GetMailTitle(MAIL_TITLE_ID_DAILY_RECHARGE_ACTIVITY), "", MAIL_TYPE_PLAYER_SYS, rewardTable.items);
                MailManager::SendMessageMail(0, player->GetName(), rewardTable.items, MAIL_TITLE_ID_DAILY_RECHARGE_ACTIVITY, 0);
            }
        }
    }
}

void SpecialActivityManager::CBGetCurDailyRechargeActivityInfo( uint64 playerID, int32 param, std::string type, int32 money, int32 status, uint32 dtime )
{
    if (type == "info")
    {
        CBOnDailyRechargeInfoReq(playerID, money, status, dtime);
    }
    else if (type == "accept_reward")
    {
        CBOnDailyRechargeAcceptRewardReq(playerID, param, money, status, dtime);
    }
}

void SpecialActivityManager::OnDailyRechargeInfoReq( uint64 playerID, void* data )
{
    GetDailyRechargeActivityInfo(playerID, data, "info");
}

void SpecialActivityManager::CBOnDailyRechargeInfoReq( uint64 playerID, int32 money, int32 status, uint32 dtime )
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if (!player || (player->GetState() < Player::kStateInGame))
    {
        return;
    }

    SPECIAL_ACTIVITY_INFO_ACK ack;
    ack.Clear();
    ack.activity_id = SPECIAL_ACTIVITY_DAILY_RECHARGE;
    ack.param = money;

    // 获取活动时间
    if (SpecialActivityConfig::Instance()->GetActivityTime(ack.activity_id, ack.start_time, ack.end_time) != ERR_SUCCEED)
    {
        ack.start_time = 0;
        ack.end_time = 0;
    }

    // 获取活动配置信息
    if (SpecialActivityConfig::Instance()->GetInfoByActivityId(ack.activity_id, ack.info_list) != ERR_SUCCEED)
    {
        ack.info_list.Clear();
    }
    else
    {
        // 在活动期间内,更新奖励标记
        if (SpecialActivityConfig::Instance()->CheckActivityTime(ack.activity_id) == ERR_SUCCEED)
        {
            SpecialActivityConfig::Instance()->UpdateStatus(ack.info_list, money, status);
        }
    }

    // 获取奖励列表
    if (SpecialActivityConfig::Instance()->GetRewardByActivityId(ack.activity_id, ack.reward_list) != ERR_SUCCEED)
    {
        ack.reward_list.Clear();
    }

    player->SendToGate(MSG_SPECIAL_ACTIVITY_INFO_ACK, &ack);
}

void SpecialActivityManager::OnDailyRechargeAcceptRewardReq( uint64 playerID, void* data )
{
    GetDailyRechargeActivityInfo(playerID, data, "accept_reward");
}

void SpecialActivityManager::CBOnDailyRechargeAcceptRewardReq( uint64 playerID, int32 param, int32 money, int32 status, uint32 dtime )
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if (!player || (player->GetState() < Player::kStateInGame))
    {
        return;
    }

    if (param < 0)
    {
        return;
    }

    SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK ack;
    ack.Clear();
    ack.errcode = ERR_FAILED;
    ack.activity_id = SPECIAL_ACTIVITY_DAILY_RECHARGE;
    ack.param = param;

    // 获取活动配置信息
    if (SpecialActivityConfig::Instance()->GetInfoByActivityId(ack.activity_id, ack.info_list) != ERR_SUCCEED)
    {
        ack.info_list.Clear();
        player->SendToGate(MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK, &ack);
        return;
    }

    // 越界检查
    if (param >= (int32)ack.info_list.infos.size())
    {
        player->SendToGate(MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK, &ack);
        return;
    }

    // 充值数量检查
    if (money < ack.info_list.infos[param].param2)
    {
        player->SendToGate(MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK, &ack);
        return;
    }

    // 更新奖励状态
    SpecialActivityConfig::Instance()->UpdateStatus(ack.info_list, money, status);

    // 奖励状态检查
    if (ack.info_list.infos[param].status != CAN_ACCEPT_REWARD)
    {
        player->SendToGate(MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK, &ack);
        return;
    }

    // 置领奖标记位
    ack.info_list.infos[param].status = ACCEPTED_REWARD;
    status |= (1 << param);
    // 先存档领奖标记,再通知gs给客户端发奖励
    if (SpecialActivityJob::Instance()->SaveDailyRechargeActivityInfo(playerID, status, dtime))
    {
        // 只有成功才发送应答到gs;如果失败,直接把失败回执给玩家
        ack.errcode = ERR_SUCCEED;
        player->SendToGame(MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_ACK, &ack);
        return;
    }
}

//--------------------------------------------UpgradeWingActivity------------------------------------------------------------

void SpecialActivityManager::GetUpgradeWingActivityInfo( uint64 playerID, std::string type, int32 param )
{
    SpecialActivityJob::Instance()->GetUpgradeWingActivityInfo(playerID, type, param);
}

void SpecialActivityManager::CBGetUpgradeWingActivityInfo( uint64 playerID, std::string type, int32 param, int32 status )
{
    if (type == "info")
    {
        CBOnUpgradeWingInfoReq(playerID, param, status);
    }
}

void SpecialActivityManager::OnUpgradeWingInfoReq( uint64 playerID, void* data )
{
    SPECIAL_ACTIVITY_INFO_REQ* req = (SPECIAL_ACTIVITY_INFO_REQ*) data;
    GetUpgradeWingActivityInfo(playerID, "info", req->param);
}


// param为当前玩家翅膀阶数
void SpecialActivityManager::CBOnUpgradeWingInfoReq( uint64 playerID, int32 param, int32 status )
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if (!player || (player->GetState() < Player::kStateInGame))
    {
        return;
    }

    SPECIAL_ACTIVITY_INFO_ACK ack;
    ack.Clear();
    ack.activity_id = SPECIAL_ACTIVITY_UPGRADE_WING;
    ack.param = param;

    // 获取活动时间
    if (SpecialActivityConfig::Instance()->GetActivityTime(ack.activity_id, ack.start_time, ack.end_time) != ERR_SUCCEED)
    {
        ack.start_time = 0;
        ack.end_time = 0;
    }

    // 获取活动配置信息
    if (SpecialActivityConfig::Instance()->GetInfoByActivityId(ack.activity_id, ack.info_list) != ERR_SUCCEED)
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

    // 获取奖励列表
    if (SpecialActivityConfig::Instance()->GetRewardByActivityId(ack.activity_id, ack.reward_list) != ERR_SUCCEED)
    {
        ack.reward_list.Clear();
    }

    // 直接发给玩家
    player->SendToGate(MSG_SPECIAL_ACTIVITY_INFO_ACK, &ack);

    // 发给gs,做status缓存,领奖时就不需要再想gc获取status了
    SendSpecialActivityLoadInfoAck(playerID, SPECIAL_ACTIVITY_UPGRADE_WING, status, ERR_SUCCEED);
}

//--------------------------------------------MicroClientActivity------------------------------------------------------------

void SpecialActivityManager::GetMicroClientActivityInfo( uint64 playerID )
{
    SpecialActivityJob::Instance()->GetMicroClientActivityInfo(playerID);
}

void SpecialActivityManager::CBGetMicroClientActivityInfo( uint64 playerID, int32 status )
{
    SendSpecialActivityLoadInfoAck(playerID, SPECIAL_ACTIVITY_MICRO_CLIENT, status, ERR_SUCCEED);
}
