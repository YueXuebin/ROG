#include "gameholder_pch.h"
#include "plot_dungeon_manager.h"
#include "player.h"
#include "vip_config.h"
#include "game_util.h"
#include "dungeon_gain_config.h"
#include "item_manager.h"
#include "trade_manager.h"
#include "event_define.h"
#include "json_helper.h"
#include "player_dungeon_manager.h"
#include "dungeon_data.h"
#include "team_manager.h"


PlotDungeonManager::PlotDungeonManager(Player* player) :
m_player(player)
{

}

PlotDungeonManager::~PlotDungeonManager(void)
{
    
}

void PlotDungeonManager::FillPlotDungeonInfo(PLOT_DUNGEON_INFO& info) const
{
    info = m_plot_dungeon_info;
}

void PlotDungeonManager::LoadPlotDungeonInfo(const PLOT_DUNGEON_INFO& info)
{
    m_plot_dungeon_info = info;
}

// 接收数据
void PlotDungeonManager::OnRecv(uint32 msgID, void * data)
{
    //switch(msgID)
    //{
    //case MSG_RAIDS_DUNGEON_INFO_REQ:
    //    {
    //        OnRaidsDungeonInfoReq(data);
    //    }
    //    break;
    //case MSG_RAIDS_DUNGEON_START_REQ:
    //    {
    //        OnRaidsDungeonStartReq(data);
    //    }
    //    break;
    //case MSG_RAIDS_DUNGEON_CMPL_REQ:
    //    {
    //        OnRaidsDungeonCmplReq(data);
    //    }
    //    break;
    //case MSG_RAIDS_DUNGEON_STOP_REQ:
    //    {
    //        OnRaidsDungeonStopReq();
    //    }
    //    break;
    //case MSG_RAIDS_DUNGEON_BUY_CD_REQ:
    //    {
    //        OnRaidsDungeonBuyCDReq();
    //    }
    //    break;
    //case MSG_READY_ENTER_DUNGEON_REQ:
    //    {
    //        OnReadyEnterDungeonReq(data);
    //    }
    //    break;
    //default:
    //    CnAssert(false);
    //    break;
    //}
}
//
//// 请求扫荡完成秒CD
//void PlotDungeonManager::OnRaidsDungeonSubTime(uint32 cd_time)
//{
//    const DungeonGainTemplate* dungeonTmpl = 0;
//    REWARD_TABLE rewardTable;
//    uint16 errCode = ERR_FAILED;
//    int32 num = 0;
//    uint32 score = 0;
//    uint32 left_time = 0;
//    uint32 raids_time = 0;
//    uint32 total_time = 0;
//    int32  total_times = 0;
//    int32  cd_rate = 0;
//    uint32 now = 0;
//
//    if(!GetRaidsDungeonID() || (GetRaidsDungeonTimes() < 1))
//    {
//        goto _END;
//    }
//
//    // 获得正在扫荡的副本模板
//    dungeonTmpl = DungeonGainConfig::Instance()->GetDungeonGainTemplate(GetRaidsDungeonID());
//    if(!dungeonTmpl)
//    {
//        goto _END;
//    }
//
//    // 每次扫荡副本花费的时间
//    raids_time = 0;
//
//    // 扫荡秒CD比率
//    cd_rate = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_20 ,m_player->GetVipLevel());
//    if(cd_rate >= 0)
//    {
//        raids_time = (uint32)(raids_time * ((cd_rate+1)%2));
//    }
//
//    // 循环处理扫荡副本
//    for(; (GetRaidsDungeonTimes() > 0) && (total_time < cd_time); )
//    {
//        // 减少一次扫荡次数
//        DecRaidsDungeonTimes();
//
//        // 扫荡完成任务
//        //m_player->SendTaskEvent(TASK_EVENT_RADIS_DUNGEON, GetRaidsDungeonID(), 0, 0);
//
//        // 活跃度
//        m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_FINISH_PLOT_DUNGEON, 1, 0, 0);
//
//        // 扫荡完成任务
//        m_player->OnRaidsDungeonCmpl(GetRaidsDungeonID());
//
//        // 总时间
//        total_time += raids_time;
//
//        // 获得副本扫荡奖励
//        rewardTable.Clear();
//        rewardTable.reason = REWARD_REASON_RAIDS_DUNGEON;
//        rewardTable.reason_arg = GetRaidsDungeonTotalTimes() - GetRaidsDungeonTimes();
//        num = 0;
//        score = m_player->GetDungeonScoreFlag(GetRaidsDungeonID());
//        DungeonGainConfig::Instance()->GetRewardTableIncludeExtra(dungeonTmpl, num, score, m_player->GetVipLevel(), rewardTable);
//
//        // 副本奖励
//        ItemManager::AddRewardTableWithEmail(m_player->GetCenterSvrID(), m_player->GetPlayerID(), rewardTable);
//    }
//
//    total_times = GetRaidsDungeonTotalTimes();
//
//    // 如果还有剩余的扫荡次数，设置扫荡的剩余时间
//    if(GetRaidsDungeonTimes() > 0)
//    {
//        SetRaidsDungeonEndTime(now + raids_time);
//        left_time = raids_time;
//    }
//    // 否则重置扫荡ID和扫荡结束时间
//    else
//    {
//        SetRaidsDungeonID(0);
//        SetRaidsDungeonTimes(0);
//        SetRaidsDungeonTotalTimes(0);
//        SetRaidsDungeonEndTime(0);
//        SetRaidsDungeonBuyVigor(0);
//    }
//
//    errCode = ERR_SUCCEED;
//
//_END:
//    SendRaidsDungeonCmplAck(errCode, GetRaidsDungeonTimes(), total_times, left_time, total_time);
//}
//
//// 请求扫荡副本信息
//void PlotDungeonManager::OnRaidsDungeonInfoReq(void* data)
//{
//    RAIDS_DUNGEON_INFO_ACK sendMsg;
//    uint32 raids_time = 0;
//    int32  cd_rate = 0;
//    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
//
//    if(GetRaidsDungeonTimes() < 1)
//    {
//        goto _END;
//    }
//
//    sendMsg.dungeon_id = GetRaidsDungeonID();
//    sendMsg.raids_times = GetRaidsDungeonTimes();
//    sendMsg.total_raids_times = GetRaidsDungeonTotalTimes();
//
//    // 每次扫荡副本花费的时间
//    raids_time = 0;
//
//    // 扫荡秒CD比率
//    cd_rate = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_20 ,m_player->GetVipLevel());
//    if(cd_rate >= 0)
//    {
//        raids_time = (uint32)(raids_time * ((cd_rate+1)%2));
//    }
//
//    if(GetRaidsDungeonEndTime() > now)
//    {
//        sendMsg.left_time = GetRaidsDungeonEndTime() - now;
//    }
//
//    sendMsg.total_time = ((GetRaidsDungeonTimes() - 1) * raids_time) + sendMsg.left_time;
//
//_END:
//    m_player->SendToGate(MSG_RAIDS_DUNGEON_INFO_ACK, &sendMsg);
//}
//
//// 处理扫荡副本
//void PlotDungeonManager::OnRaidsDungeonStartReq(void* data)
//{
//    RAIDS_DUNGEON_START_REQ* recvMsg = (RAIDS_DUNGEON_START_REQ*) data;
//    uint16 errcode = ERR_SUCCEED;
//    int32 raids_vigor = 0;
//    int32 cost_vigor = 0;
//    int32 cost_buy_vigor = 0;
//    uint32 raids_time = 0;
//    uint32 total_time = 0;
//    int32  cd_rate = 0;
//    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
//    uint32 dungeonClass = 0;
//
//    // 检查参数的有效性
//    if(!recvMsg || (recvMsg->times < 1))
//    {
//        errcode = ERR_FAILED;
//        goto _END;
//    }
//
//    // 检查副本类型
//    dungeonClass = GetDungeonClass(recvMsg->dungeon_id);
//    if(dungeonClass != DUNGEON_CLASS_PLOT_ID)
//    {
//        errcode = ERR_FAILED;
//        goto _END;
//    }
//
//    // 检查要扫荡的剧情副本是否开启
//    if(!m_player->IsPlayerDungeonRadis(recvMsg->dungeon_id))
//    {
//        errcode = ERR_FAILED;
//        goto _END;
//    }
//
//    // 检查是否已在扫荡
//    if(GetRaidsDungeonID() > 0)
//    {
//        errcode = ERR_RAIDSING_DUNGEON;
//        goto _END;
//    }
//
//    // 每次扫荡副本消耗的体力
//    raids_vigor = 0;
//
//    // 每次扫荡副本花费的时间
//    raids_time = 0;
//
//    // 扫荡秒CD比率
//    cd_rate = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_20, m_player->GetVipLevel());
//    if(cd_rate >= 0)
//    {
//        raids_time = uint32(raids_time * ((cd_rate+1)%2));
//    }
//
//    // 总共消耗的体力值
//    cost_vigor = recvMsg->times * raids_vigor;
//
//    // 判断体力是否足够
//    if((raids_vigor < 0) || (cost_vigor < 0) || (m_player->GetVigor() < cost_vigor))
//    {
//        errcode = ERR_NO_ENOUGH_VIGOR;
//        goto _END;
//    }
//
//    // 总时间
//    total_time = raids_time * recvMsg->times;
//
//    // 扫荡扣除体力
//    cost_buy_vigor = m_player->SubVigor(cost_vigor);
//
//    // 设置扫荡的副本ID
//    SetRaidsDungeonID(recvMsg->dungeon_id);
//
//    // 设置扫荡次数
//    SetRaidsDungeonTimes(recvMsg->times);
//
//    // 设置扫荡总次数
//    SetRaidsDungeonTotalTimes(recvMsg->times);
//
//    // 设置扫荡的剩余时间
//    SetRaidsDungeonEndTime(now + raids_time);
//
//    // 设置扫荡话费的购买体力
//    SetRaidsDungeonBuyVigor(cost_buy_vigor);
//
//_END:
//    SendRaidsDungeonStartAck(errcode, GetRaidsDungeonTimes(), GetRaidsDungeonTotalTimes(), raids_time, total_time);
//}
//
//// 请求完成扫荡副本
//void PlotDungeonManager::OnRaidsDungeonCmplReq(void* data)
//{
//    const DungeonGainTemplate* dungeonTmpl = NULL;
//    REWARD_TABLE rewardTable;
//    uint16 errCode = ERR_FAILED;
//    int32 num = 0;
//    uint32 score = 0;
//    uint32 left_time = 0;
//    uint32 raids_time = 0;
//    uint32 time_diff = 0;
//    uint32 total_time = 0;
//    int32 total_times = 0;
//    int32  cd_rate = 0;
//    uint32 now = 0;
//
//    if(!GetRaidsDungeonID() || (GetRaidsDungeonTimes() < 1))
//    {
//        goto _END;
//    }
//
//    // 检查时间是否
//    now = (uint32)Crown::SDNow().GetTimeValue();
//    if(GetRaidsDungeonEndTime() > now)
//    {
//        errCode = ERR_NO_REACH_TIME;
//        left_time = GetRaidsDungeonEndTime() - now;
//        goto _END;
//    }
//
//    // 计算时间差
//    time_diff = now - GetRaidsDungeonEndTime();
//
//    // 获得正在扫荡的副本模板
//    dungeonTmpl = DungeonGainConfig::Instance()->GetDungeonGainTemplate(GetRaidsDungeonID());
//    if(!dungeonTmpl)
//    {
//        goto _END;
//    }
//
//    // 每次扫荡副本花费的时间
//    raids_time = 0;
//
//    // 扫荡秒CD比率
//    cd_rate = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_20 ,m_player->GetVipLevel());
//    if(cd_rate >= 0)
//    {
//        raids_time =(uint32)(raids_time * ((cd_rate+1)%2));
//    }
//
//    // 把之前一次扫荡的时间差加上
//    time_diff += raids_time;
//
//    for(;(GetRaidsDungeonTimes() > 0) && (time_diff >= raids_time); )
//    {
//        // 减少一次扫荡次数
//        DecRaidsDungeonTimes();
//
//        // 扫荡完成任务
//        //m_player->SendTaskEvent(TASK_EVENT_RADIS_DUNGEON, GetRaidsDungeonID(), 0, 0);
//
//        // 活跃度
//        m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_FINISH_PLOT_DUNGEON, 1, 0, 0);
//
//        // 扫荡完成任务
//        m_player->OnRaidsDungeonCmpl(GetRaidsDungeonID());
//
//        // 最后剩得时间差
//        time_diff -= raids_time;
//
//        // 获得副本扫荡奖励
//        rewardTable.Clear();
//        rewardTable.reason = REWARD_REASON_RAIDS_DUNGEON;
//        rewardTable.reason_arg = GetRaidsDungeonTotalTimes() - GetRaidsDungeonTimes();
//        num = 0;
//        score = m_player->GetDungeonScoreFlag(GetRaidsDungeonID());
//        DungeonGainConfig::Instance()->GetRewardTableIncludeExtra(dungeonTmpl, num, score, m_player->GetVipLevel(), rewardTable);
//
//        // 副本奖励
//        ItemManager::AddRewardTableWithEmail(m_player->GetCenterSvrID(), m_player->GetPlayerID(), rewardTable);
//    }
//
//    // 总时间
//    total_time = raids_time * GetRaidsDungeonTimes();
//    total_times = GetRaidsDungeonTotalTimes();
//
//    // 如果还有剩余的扫荡次数，设置扫荡的剩余时间
//    if(GetRaidsDungeonTimes() > 0)
//    {
//        SetRaidsDungeonEndTime(now + raids_time - time_diff);
//        left_time = raids_time - time_diff;
//    }
//    // 否则重置扫荡ID和扫荡结束时间
//    else
//    {
//        SetRaidsDungeonID(0);
//        SetRaidsDungeonTimes(0);
//        SetRaidsDungeonTotalTimes(0);
//        SetRaidsDungeonEndTime(0);
//        SetRaidsDungeonBuyVigor(0);
//    }
//
//    errCode = ERR_SUCCEED;
//
//_END:
//    SendRaidsDungeonCmplAck(errCode, GetRaidsDungeonTimes(), total_times, left_time, total_time);
//}
//
//// 请求扫荡副本结束
//void PlotDungeonManager::OnRaidsDungeonStopReq()
//{
//    int32 raids_vigor = 0;
//    int32 back_vigor = 0;
//    int32 back_sys_vigor = 0;
//    int32 back_buy_vigor = 0;
//
//    if(!GetRaidsDungeonID() || (GetRaidsDungeonTimes() < 1))
//    {
//        return;
//    }
//
//    // 每次扫荡副本消耗的体力
//    raids_vigor = 0;
//
//    // 总共消耗的体力值
//    back_vigor = GetRaidsDungeonTimes() * raids_vigor;
//
//    // 判断体力是否足够
//    if((raids_vigor < 0) || (back_vigor < 0))
//    {
//        back_vigor = 0;
//    }
//
//    // 判断恢复的体力值
//    if(back_vigor > GetRaidsDungeonBuyVigor())
//    {
//        back_sys_vigor = back_vigor - GetRaidsDungeonBuyVigor();
//        back_buy_vigor = GetRaidsDungeonBuyVigor();
//    }
//    else
//    {
//        back_buy_vigor = back_vigor;
//    }
//
//    // 返还体力
//    m_player->SubSysVigor(back_sys_vigor);
//    m_player->AddBuyVigor(back_buy_vigor);
//
//    // 清除扫荡副本的所有标记
//    SetRaidsDungeonID(0);
//    SetRaidsDungeonTimes(0);
//    SetRaidsDungeonTotalTimes(0);
//    SetRaidsDungeonEndTime(0);
//    SetRaidsDungeonBuyVigor(0);
//
//    // 通知副本扫荡停止
//    SendRaidsDungeonStopAck();
//}
//
//// 请求购买扫荡副本减CD
//void PlotDungeonManager::OnRaidsDungeonBuyCDReq()
//{
//    const DungeonGainTemplate* dungeonTmpl = NULL;
//    REWARD_TABLE rewardTable;
//    uint16 errCode = ERR_FAILED;
//    uint32 leftTime = 0;
//    uint32 raids_time = 0;
//    uint32 total_time = 0;
//    uint32 buy_time = 0;
//    int32  cd_rate = 0;
//    int32  money_point = 0;
//    uint32 now = 0;
//
//    if(!GetRaidsDungeonID() || (GetRaidsDungeonTimes() < 1))
//    {
//        goto _ERR;
//    }
//
//    // 获得正在扫荡的副本模板
//    dungeonTmpl = DungeonGainConfig::Instance()->GetDungeonGainTemplate(GetRaidsDungeonID());
//    if(!dungeonTmpl)
//    {
//        goto _ERR;
//    }
//
//    // 每次扫荡副本花费的时间
//    raids_time = 0;
//
//    // 扫荡秒CD比率
//    cd_rate = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_20 ,m_player->GetVipLevel());
//    if(cd_rate >= 0)
//    {
//        raids_time = (uint32)(raids_time * ((cd_rate+1)%2));
//    }
//
//    // 检查时间是否
//    now = (uint32)Crown::SDNow().GetTimeValue();
//    if(GetRaidsDungeonEndTime() > now)
//    {
//        leftTime = GetRaidsDungeonEndTime() - now;
//    }
//
//    total_time = ((GetRaidsDungeonTimes() - 1) * raids_time) + leftTime;
//
//    // 循环处理扫荡副本
//    //for(; buy_time < total_time; )
//    //{
//    //    buy_time += GameParamConfig::Instance()->GetRaidsDungeonBuyTime();
//    //    money_point += GameParamConfig::Instance()->GetRaidsDungeonBuyCost();
//    //}
//
//    // 检查魔石数量
//    if(m_player->GetMoneyPoint() < money_point)
//    {
//        errCode = ERR_NO_ENOUGH_MONEYPOINT;
//        goto _ERR;
//    }
//
//    // 发送魔石交易订单
//    //TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_SUB_DUNGEON_RAIDS_TIME, PROP_SUB_DUNGEON_RAIDS_TIME, 0, total_time, money_point);
//
//    return;
//
//_ERR:
//    m_player->NotifyErrorCode(errCode);
//}
//
//// 获得扫荡副本ID
//uint32 PlotDungeonManager::GetRaidsDungeonID()
//{
//    //return m_plot_dungeon_info.raids_dungeon_id;
//    return 0;
//}
//
//// 设置扫荡副本ID
//void PlotDungeonManager::SetRaidsDungeonID(uint32 val)
//{
//    //m_plot_dungeon_info.raids_dungeon_id = val;
//}
//
//// 获得扫荡副本的剩余次数
//int32 PlotDungeonManager::GetRaidsDungeonTimes()
//{
//    //return m_plot_dungeon_info.raids_dungeon_times;
//    return 0;
//}
//
//// 扫荡次数减1
//void PlotDungeonManager::DecRaidsDungeonTimes()
//{
//    //if(m_plot_dungeon_info.raids_dungeon_times > 0)
//    //    --m_plot_dungeon_info.raids_dungeon_times;
//}
//
//// 设置扫荡副本的剩余次数
//void PlotDungeonManager::SetRaidsDungeonTimes(int32 val)
//{
//    //m_plot_dungeon_info.raids_dungeon_times = val;
//}
//
//// 获得扫荡副本总次数
//int32 PlotDungeonManager::GetRaidsDungeonTotalTimes()
//{
//    //return m_plot_dungeon_info.raids_dungeon_total_times;
//    return 0;
//}
//
//// 设置扫荡副本总次数
//void PlotDungeonManager::SetRaidsDungeonTotalTimes(int32 val)
//{
//    //m_plot_dungeon_info.raids_dungeon_total_times = val;
//}
//
//// 获得扫荡副本的结束时间
//uint32 PlotDungeonManager::GetRaidsDungeonEndTime()
//{
//    //return m_plot_dungeon_info.raids_dungeon_end_time;
//    return 0;
//}
//
//// 设置扫荡副本的结束时间
//void PlotDungeonManager::SetRaidsDungeonEndTime(uint32 val)
//{
//    //m_plot_dungeon_info.raids_dungeon_end_time = val;
//}
//
//// 获得扫荡副本时花费的购买体力（因为体力值由两部分组成，系统赠送和购买，所以在返还时需要注意）
//int32 PlotDungeonManager::GetRaidsDungeonBuyVigor()
//{
//    //return m_plot_dungeon_info.raids_dungeon_buy_vigor;
//    return 0;
//}
//
//// 设置扫荡副本时花费的购买体力
//void PlotDungeonManager::SetRaidsDungeonBuyVigor(int32 val)
//{
//    //m_plot_dungeon_info.raids_dungeon_buy_vigor = val;
//}
//
//// 发送扫荡副本应答
//void PlotDungeonManager::SendRaidsDungeonStartAck(uint16 errCode, int32 raids_times, 
//                                                    int32 total_raids_times, uint32 left_time, uint32 total_time)
//{
//    RAIDS_DUNGEON_START_ACK sendMsg;
//    sendMsg.errcode = errCode;
//    sendMsg.raids_times = raids_times;
//    sendMsg.total_raids_times = total_raids_times;
//    sendMsg.left_time = left_time;
//    sendMsg.total_time = total_time;
//    m_player->SendToGate(MSG_RAIDS_DUNGEON_START_ACK, &sendMsg);
//}
//
//// 发送扫荡副本应答
//void PlotDungeonManager::SendRaidsDungeonCmplAck(uint16 errCode, int32 raids_times, 
//                                                   int32 total_raids_times, uint32 left_time, uint32 total_time)
//{
//    RAIDS_DUNGEON_CMPL_ACK sendMsg;
//    sendMsg.errcode = errCode;
//    sendMsg.raids_times = raids_times;
//    sendMsg.total_raids_times = total_raids_times;
//    sendMsg.left_time = left_time;
//    sendMsg.total_time = total_time;
//    m_player->SendToGate(MSG_RAIDS_DUNGEON_CMPL_ACK, &sendMsg);
//}
//
//// 发送扫荡副本停止
//void PlotDungeonManager::SendRaidsDungeonStopAck()
//{
//    NONE sendMsg;
//    m_player->SendToGate(MSG_RAIDS_DUNGEON_STOP_ACK, &sendMsg);
//}

void PlotDungeonManager::SetDungeonFlag( uint32 dungeonID, uint32 flag )
{
 /*   bool newFlag = true;
    for (std::vector<UNIT_DUNGEON_INFO>::iterator it = m_plot_dungeon_info.open_dungeon.begin(); it != m_plot_dungeon_info.open_dungeon.end(); it++ )
    {
        if(it->dungeon_id == dungeonID)
        {
            newFlag = false;
            break;
        }
    }

    if(newFlag)
    {
        UNIT_DUNGEON_INFO dungeon;
        dungeon.dungeon_id = dungeonID;
        dungeon.dungeon_score = 0;
        m_plot_dungeon_info.open_dungeon.push_back(dungeon);
    }*/
}

bool PlotDungeonManager::GetDungeonFlag( uint32 dungeonID )
{
    //for (std::vector<UNIT_DUNGEON_INFO>::iterator it = m_plot_dungeon_info.open_dungeon.begin(); it != m_plot_dungeon_info.open_dungeon.end(); it++ )
    //{
    //    if(it->dungeon_id == dungeonID)
    //    {
    //        return true;
    //        break;
    //    }
    //}
    //return false;
    return false;
}

uint32 PlotDungeonManager::GetDungeonScoreFlag( uint32 dungeonID )
{
    //for (std::vector<UNIT_DUNGEON_INFO>::iterator it = m_plot_dungeon_info.open_dungeon.begin(); it != m_plot_dungeon_info.open_dungeon.end(); it++ )
    //{
    //    if(it->dungeon_id == dungeonID)
    //    {
    //        return it->dungeon_score;
    //        break;
    //    }
    //}
    return 0;
}

void PlotDungeonManager::SetDungeonScoreFlag( uint32 dungeonID, uint32 score )
{
    //for (std::vector<UNIT_DUNGEON_INFO>::iterator it = m_plot_dungeon_info.open_dungeon.begin(); it != m_plot_dungeon_info.open_dungeon.end(); it++ )
    //{
    //    if(it->dungeon_id == dungeonID)
    //    {
    //        it->dungeon_score = score;
    //        break;
    //    }
    //}
}

//void PlotDungeonManager::OnReadyEnterDungeonReq( void* data )
//{
//    ENTER_DUNGEON_REQ* recvMsg = (ENTER_DUNGEON_REQ*) data;
//
//    // 检查参数的有效性
//    if(!recvMsg)
//    {
//        return;
//    }
//
//    if(TeamManager::Instance()->OnEnterDungeonByTeam(m_player, recvMsg))
//    {
//        // 已经进入队友的副本了
//        return;
//    }
//    else
//    {
//        //m_player->SendToGate(MSG_READY_ENTER_DUNGEON_NTF, recvMsg);
//    }
//
//    
//
//}
