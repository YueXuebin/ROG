#include "gameholder_pch.h"
#include "soul_dungeon_manager.h"
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


SoulDungeonManager::SoulDungeonManager(Player* player) :
m_player(player)
{
    Clear();
}

SoulDungeonManager::~SoulDungeonManager(void)
{

}

// ==================================================
// 清除数据
// ==================================================
void SoulDungeonManager::Clear()
{

}

// ==================================================
// 刷新数据
// ==================================================
void SoulDungeonManager::Refresh()
{
    //for(std::vector<SOUL_DUNGEON_TIMES_RECORDER>::iterator it = m_soul_dungeon_info.soul_dungeon_times_recorder.begin();
    //    it != m_soul_dungeon_info.soul_dungeon_times_recorder.end(); ++it)
    //{
    //    it->sys_times = 0;
    //    it->daily_pur_times = 0;
    //}

    SendSoulDungeonTimesList();
}

void SoulDungeonManager::FillSoulDungeonInfo(SOUL_DUNGEON_INFO& info) const
{
    info = m_soul_dungeon_info;
}

void SoulDungeonManager::LoadSoulDungeonInfo(const SOUL_DUNGEON_INFO& info)
{
    m_soul_dungeon_info = info;

    // 假设护魂副本的第一个副本未开启，则直接开启此副本
    //if(!m_player->IsPlayerDungeonOpen(FIRST_PROTECT_SOUL_DUNGEON_ID))
    //{
    //    m_player->SetPlayerDungeonFlag(FIRST_PROTECT_SOUL_DUNGEON_ID, DUNGEON_OPEN_FLAG);
    //}

}

// ==================================================
// 接收数据
// ==================================================
void SoulDungeonManager::OnRecv(uint32 msgID, void * data)
{
    //switch(msgID)
    //{
    //case MSG_RAIDS_SOUL_DUNGEON_INFO_REQ:
    //    {
    //        OnRaidsSoulDungeonInfoReq();
    //    }
    //    break;
    //case MSG_RAIDS_SOUL_DUNGEON_START_REQ:
    //    {
    //        OnRaidsSoulDungeonStartReq(data);
    //    }
    //    break;
    //case MSG_RAIDS_SOUL_DUNGEON_CMPL_REQ:
    //    {
    //        OnRaidsSoulDungeonCmplReq(data);
    //    }
    //    break;
    //case MSG_RAIDS_SOUL_DUNGEON_STOP_REQ:
    //    {
    //        OnRaidsSoulDungeonStopReq();
    //    }
    //    break;
    //case MSG_RAIDS_SOUL_DUNGEON_BUY_CD_REQ:
    //    {
    //        OnRaidsSoulDungeonBuyCDReq();
    //    }
    //    break;
    //case MSG_RAIDS_SOUL_DUNGEON_BUY_TIMES_REQ:
    //    {
    //        OnRaidsSoulDungeonBuyTimesReq(data);
    //    }
    //    break;
    //default:
    //    CnAssert(false);
    //    break;
    //}
    CnAssert(false);
}

// ==================================================
// 请求护魂副本信息
// ==================================================
void SoulDungeonManager::OnRaidsSoulDungeonInfoReq()
{
//    RAIDS_SOUL_DUNGEON_INFO_ACK sendMsg;
//    uint32 raids_time = 0;
//    int32  cd_rate = 0;
//    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
//
//    if(GetRaidsDungeonTimes() < 1)
//    {
//        goto _END;
//    }
//
//    sendMsg.raids_soul_dungeon_list = m_soul_dungeon_info.raids_dungeon_list;
//    sendMsg.raids_times = GetRaidsDungeonTimes();
//    sendMsg.total_raids_times = GetRaidsDungeonTotalTimes();
//
//    // 每次扫荡副本花费的时间
//    raids_time = 0;
//
//    // 扫荡秒CD比率
//    cd_rate = VipConfig::Instance()->GetVipValue( VIP_PRIVILEGE_24, m_player->GetVipLevel());
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
//    m_player->SendToGate(MSG_RAIDS_SOUL_DUNGEON_INFO_ACK, &sendMsg);
}

// ==================================================
// 请求护魂副本开始
// ==================================================
void SoulDungeonManager::OnRaidsSoulDungeonStartReq(void* data)
{
//    RAIDS_SOUL_DUNGEON_START_REQ* recvMsg = (RAIDS_SOUL_DUNGEON_START_REQ*) data;
//    std::vector<RAIDS_SOUL_DUNGEON_INFO>::iterator soulDungeonIt;
//    uint16  errcode     = ERR_SUCCEED;
//    uint32  raids_id    = 0;
//    int32   raids_num   = 0;
//    uint32  raids_time  = 0;
//    uint32  total_time  = 0;
//    int32   cd_rate     = 0;
//    uint32  now         = (uint32)Crown::SDNow().GetTimeValue();
//
//    // 检查参数的有效性
//    if(!recvMsg)
//    {
//        errcode = ERR_FAILED;
//        goto _END;
//    }
//
//    // check vip
//    if (VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_62 , m_player->GetVipLevel()) <= 0)
//    {
//        errcode = ERR_NO_ENOUGH_VIP;
//        goto _END;
//    }
//
//    // 如果扫荡列表大于0，则说明当前有正在扫荡的护魂副本
//    if(GetRaidsDungeonID() > 0)
//    {
//        errcode = ERR_RAIDSING_DUNGEON;
//        goto _END;
//    }
//
//    // 检查扫荡的列表
//    for(soulDungeonIt=recvMsg->raids_soul_dungeon_list.begin(); soulDungeonIt!=recvMsg->raids_soul_dungeon_list.end(); ++soulDungeonIt)
//    {
//        // 扫荡ID
//        raids_id = GetDungeonSubID(soulDungeonIt->dungeon_id);
//
//        // 检查副本类型
//        if( GetDungeonClass(soulDungeonIt->dungeon_id) != DUNGEON_CLASS_SOUL_ID)
//        {
//            errcode = ERR_FAILED;
//            goto _END;
//        }
//
//        // 检查扫荡ID是否在有效范围内
//        if(raids_id >= m_soul_dungeon_info.soul_dungeon_times_recorder.size())
//        {
//            errcode = ERR_FAILED;
//            goto _END;
//        }
//
//        // 检查扫荡的精魂副本ID是否有效
//        if(!IsValidRaidsID(raids_id))
//        {
//            errcode = ERR_FAILED;
//            goto _END;
//        }
//
//        // 检查要扫荡的护魂副本是否开启
//        if(!m_player->IsPlayerDungeonRadis(soulDungeonIt->dungeon_id))
//        {
//            errcode = ERR_NEED_CLEAR_DUNGEON;
//            goto _END;
//        }
//
//        // 检查扫荡次数有效性
//        if(soulDungeonIt->radis_times < 1)
//        {
//            errcode = ERR_FAILED;
//            goto _END;
//        }
//
//        // 检查扫荡次数是否足够
//        if(soulDungeonIt->radis_times > GetRaidsDungeonTimes(soulDungeonIt->dungeon_id))
//        {
//            errcode = ERR_FAILED;
//            goto _END;
//        }
//
//        raids_num += soulDungeonIt->radis_times;
//    }
//
//    // 每次扫荡副本花费的时间
//    raids_time = 0;
//
//    // 扫荡秒CD比率
//    cd_rate = VipConfig::Instance()->GetVipValue( VIP_PRIVILEGE_24, m_player->GetVipLevel());
//    if(cd_rate >= 0)
//    {
//        raids_time = uint32(raids_time * ((cd_rate+1)%2));
//    }
//
//    // 总时间
//    total_time = raids_time * raids_num;
//
//    // 设置扫荡的副本ID
//    m_soul_dungeon_info.raids_dungeon_list = recvMsg->raids_soul_dungeon_list;
//
//    // 设置扫荡次数
//    SetRaidsDungeonTimes(raids_num);
//
//    // 设置扫荡总次数
//    SetRaidsDungeonTotalTimes(raids_num);
//
//    // 设置扫荡的剩余时间
//    SetRaidsDungeonEndTime(now + raids_time);
//
//_END:
//    SendRaidsSoulDungeonStartAck(errcode, GetRaidsDungeonTimes(), 
//        GetRaidsDungeonTotalTimes(), raids_time, total_time);
}

// ==================================================
// 请求护魂副本结束
// ==================================================
void SoulDungeonManager::OnRaidsSoulDungeonCmplReq(void* data)
{
    const DungeonGainTemplate* dungeonTmpl = NULL;
    REWARD_TABLE rewardTable;
    uint16 errCode = ERR_FAILED;
    int32 num = 0;
    uint32 left_time = 0;
    uint32 raids_time = 0;
    uint32 time_diff = 0;
    uint32 total_time = 0;
    int32 total_times = 0;
    int32  cd_rate = 0;
    uint32 now = 0;

    if(!GetRaidsDungeonID() || (GetRaidsDungeonTimes() < 1))
    {
        goto _END;
    }

    // 检查时间是否
    now = (uint32)Crown::SDNow().GetTimeValue();
    if(GetRaidsDungeonEndTime() > now)
    {
        errCode = ERR_NO_REACH_TIME;
        left_time = GetRaidsDungeonEndTime() - now;
        goto _END;
    }

    // 计算时间差
    time_diff = now - GetRaidsDungeonEndTime();

    // 获得正在扫荡的副本模板
    dungeonTmpl = DungeonGainConfig::Instance()->GetDungeonGainTemplate(GetRaidsDungeonID());
    if(!dungeonTmpl)
    {
        goto _END;
    }

    // 每次扫荡副本花费的时间
    raids_time = 0;

    // 扫荡秒CD比率
    cd_rate = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_24, m_player->GetVipLevel());
    if(cd_rate >= 0)
    {
        raids_time =(uint32)(raids_time * ((cd_rate+1)%2));
    }

    // 把之前一次扫荡的时间差加上
    time_diff += raids_time;

    for(;(GetRaidsDungeonTimes() > 0) && (time_diff >= raids_time); )
    {
        // 减少一次扫荡次数
        DecRaidsDungeonTimes(GetRaidsDungeonID());

        // 扫荡完成任务
        //m_player->SendTaskEvent(TASK_EVENT_RADIS_DUNGEON, GetRaidsDungeonID(), 0, 0);

        // 活跃度
        m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_PROTECT_SOUL, 1, 0, 0);

        // 扫荡完成任务
        m_player->OnRaidsDungeonCmpl(GetRaidsDungeonID());

        // 最后剩得时间差
        time_diff -= raids_time;

        // 获得副本扫荡奖励
        rewardTable.Clear();
        rewardTable.reason = REWARD_REASON_RAIDS_SOUL_DUNGEON;
        rewardTable.reason_arg = GetRaidsDungeonTotalTimes() - GetRaidsDungeonTimes();
        num = 0;
        DungeonGainConfig::Instance()->GetDungeonGainReward(dungeonTmpl, 1, rewardTable.items);

        // 副本奖励
        //ItemManager::AddRewardTableWithEmail(m_player->GetCenterSvrID(), m_player->GetPlayerID(), rewardTable);
    }

    // 总时间
    total_time = raids_time * GetRaidsDungeonTimes();
    total_times = GetRaidsDungeonTotalTimes();

    // 如果还有剩余的扫荡次数，设置扫荡的剩余时间
    if(GetRaidsDungeonTimes() > 0)
    {
        SetRaidsDungeonEndTime(now + raids_time - time_diff);
        left_time = raids_time - time_diff;
    }
    // 否则重置扫荡ID和扫荡结束时间
    else
    {
        ClearRaidsDungeonList();
        SetRaidsDungeonTimes(0);
        SetRaidsDungeonTotalTimes(0);
        SetRaidsDungeonEndTime(0);
    }

    errCode = ERR_SUCCEED;

_END:
    SendRaidsSoulDungeonCmplAck(errCode, GetRaidsDungeonTimes(), 
        total_times, left_time, total_time);
}

// ==================================================
// 请求护魂副本停止
// ==================================================
void SoulDungeonManager::OnRaidsSoulDungeonStopReq()
{
    if(!GetRaidsDungeonID() || (GetRaidsDungeonTimes() < 1))
    {
        return;
    }

    // 清除扫荡副本的所有标记
    ClearRaidsDungeonList();
    SetRaidsDungeonTimes(0);
    SetRaidsDungeonTotalTimes(0);
    SetRaidsDungeonEndTime(0);

    // 通知副本扫荡停止
    SendRaidsSoulDungeonStopAck();
}

// ==================================================
// 护魂副本买CD请求
// ==================================================
void SoulDungeonManager::OnRaidsSoulDungeonBuyCDReq()
{
    const DungeonGainTemplate* dungeonTmpl = NULL;
    REWARD_TABLE rewardTable;
    uint16 errCode = ERR_FAILED;
    uint32 leftTime = 0;
    uint32 raids_time = 0;
    uint32 total_time = 0;
    uint32 buy_time = 0;
    int32  cd_rate = 0;
    int32  money_point = 0;
    uint32 now = 0;

    if(!GetRaidsDungeonID() || (GetRaidsDungeonTimes() < 1))
    {
        goto _ERR;
    }

    // 获得正在扫荡的副本模板
    dungeonTmpl = DungeonGainConfig::Instance()->GetDungeonGainTemplate(GetRaidsDungeonID());
    if(!dungeonTmpl)
    {
        goto _ERR;
    }

    // 每次扫荡副本花费的时间
    raids_time = 0;

    // 扫荡秒CD比率
    cd_rate = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_24 ,m_player->GetVipLevel());
    if(cd_rate >= 0)
    {
        raids_time = (uint32)(raids_time * ((cd_rate+1)%2));
    }

    //if(raids_time <= 0)
    //{
    //    m_player->GetItemManager()->AddItemByType(PROP_PROTECT_SOUL_SUB_TIME, 0, 0, 0);
    //    return;
    //}

    // 检查时间是否
    now = (uint32)Crown::SDNow().GetTimeValue();
    if(GetRaidsDungeonEndTime() > now)
    {
        leftTime = GetRaidsDungeonEndTime() - now;
    }

    total_time = ((GetRaidsDungeonTimes() - 1) * raids_time) + leftTime;

    // 循环处理扫荡副本
    //for(; (buy_time < total_time) && GameParamConfig::Instance()->GetRaidsDungeonBuyTime(); )
    //{
    //    buy_time += GameParamConfig::Instance()->GetRaidsDungeonBuyTime();
    //    money_point += GameParamConfig::Instance()->GetRaidsDungeonBuyCost();
    //}

    // 检查魔石数量
    if(m_player->GetMoneyPoint() < money_point)
    {
        errCode = ERR_NO_ENOUGH_MONEYPOINT;
        goto _ERR;
    }

    // 发送魔石交易订单
    TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_SUB_PROTECT_SOUL_TIME,
        PROP_PROTECT_SOUL_SUB_TIME, 0, buy_time, money_point);

    return;

_ERR:
    m_player->NotifyErrorCode(errCode);
}

// ==================================================
// 护魂副本购买次数请求
// ==================================================
void SoulDungeonManager::OnRaidsSoulDungeonBuyTimesReq(void* data)
{
//    RAIDS_SOUL_DUNGEON_BUY_TIMES_REQ* recvMsg = (RAIDS_SOUL_DUNGEON_BUY_TIMES_REQ*)data;
//    const DungeonGainTemplate* dungeonTmpl = NULL;
//    std::vector<RAIDS_SOUL_DUNGEON_INFO>::iterator soulDungeonIt;
//    REWARD_TABLE rewardTable;
//    std::string tradeTxt;
//    uint16 errCode = ERR_FAILED;
//    uint32 raids_id = 0;
//    uint32 raids_times = 0;
//    int32  money_point = 0;
//    int32 countnum = 0;
//    uint32 dungeonid = 0;
//
//    if(!recvMsg) 
//        return;
//
//    countnum = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_22, m_player->GetVipLevel());
//
//    // 检查扫荡的列表
//    for(soulDungeonIt=recvMsg->raids_soul_dungeon_list.begin(); 
//        soulDungeonIt != recvMsg->raids_soul_dungeon_list.end(); ++soulDungeonIt)
//    {
//        // 扫荡ID
//        raids_id = GetDungeonSubID(soulDungeonIt->dungeon_id);
//
//        // 检查副本类型
//        if( GetDungeonClass(soulDungeonIt->dungeon_id) != DUNGEON_CLASS_SOUL_ID)
//        {
//            errCode = ERR_FAILED;
//            goto _ERR;
//        }
//
//        // 检查扫荡ID是否在有效范围内
//        if(raids_id >= m_soul_dungeon_info.soul_dungeon_times_recorder.size())
//        {
//            errCode = ERR_FAILED;
//            goto _ERR;
//        }
//
//        // 检查要扫荡的护魂副本是否开启
//        if(!m_player->IsPlayerDungeonRadis(soulDungeonIt->dungeon_id))
//        {
//            errCode = ERR_NEED_CLEAR_DUNGEON;
//            goto _ERR;
//        }
//
//        // 检查扫荡次数
//        if(soulDungeonIt->radis_times < 1)
//        {
//            errCode = ERR_FAILED;
//            goto _ERR;
//        }
//
//        if(countnum < (soulDungeonIt->radis_times + GetDungeonPurTimes(soulDungeonIt->dungeon_id)))
//        {
//            errCode = ERR_VIP_OPERATION;
//            goto _ERR;
//        }
//
//        raids_times += soulDungeonIt->radis_times;
//
//        dungeonid = soulDungeonIt->dungeon_id;
//    }
//
//    // 获得每次购买的魔石价格
//    //money_point = PetConfig::Instance()->GetProSoulSpendMoney(dungeonid);
//    //money_point = money_point * raids_times;
//
//    // 检查魔石数量
//    if(m_player->GetMoneyPoint() < money_point)
//    {
//        errCode = ERR_NO_ENOUGH_MONEYPOINT;
//        goto _ERR;
//    }
//
//    write_SOUL_DUNGEON_RAIDS(recvMsg->raids_soul_dungeon_list, tradeTxt);
//
//    // 发送魔石交易订单
//    TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_PROTECT_SOUL_NUM,
//        PROP_PROTECT_SOUL_NUM, 0, 0, money_point, tradeTxt);
//
//    return;
//
//_ERR:
//    m_player->NotifyErrorCode(errCode);
}

// ==================================================
// 添加护魂副本的次数
// ==================================================
void SoulDungeonManager::AddProtectSoulNum(const std::string& txt)
{
    //std::vector<RAIDS_SOUL_DUNGEON_INFO> list;

    //read_SOUL_DUNGEON_RAIDS(list, txt);

    //std::vector<RAIDS_SOUL_DUNGEON_INFO>::iterator soulDungeonIt;
    //for(soulDungeonIt=list.begin(); 
    //    soulDungeonIt != list.end(); ++soulDungeonIt)
    //{
    //    AddDungeonPurTimes(soulDungeonIt->dungeon_id, soulDungeonIt->radis_times);
    //}

    //for(std::vector<RAIDS_SOUL_DUNGEON_INFO>::iterator it = list.begin();
    //    it != list.end(); ++it)
    //{
    //    AddProtectSoulTimes(it->dungeon_id, it->radis_times);
    //}

    //SendSoulDungeonTimesList();
}

// ==================================================
// 请求扫荡完成秒CD
// ==================================================
void SoulDungeonManager::OnProtectSoulSubTime(uint32 cd_time)
{
    const DungeonGainTemplate* dungeonTmpl = 0;
    REWARD_TABLE rewardTable;
    uint16 errCode = ERR_FAILED;
    int32 num = 0;
    uint32 left_time = 0;
    uint32 raids_time = 0;
    uint32 total_time = 0;
    int32  total_times = 0;
    int32  cd_rate = 0;
    uint32 now = 0;

    if(!GetRaidsDungeonID() || (GetRaidsDungeonTimes() < 1))
    {
        goto _END;
    }

    // 获得正在扫荡的副本模板
    dungeonTmpl = DungeonGainConfig::Instance()->GetDungeonGainTemplate(GetRaidsDungeonID());
    if(!dungeonTmpl)
    {
        goto _END;
    }

    // 每次扫荡副本花费的时间
    raids_time = 0;

    // 扫荡秒CD比率
    cd_rate = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_24 ,m_player->GetVipLevel());
    if(cd_rate >= 0)
    {
        raids_time = (uint32)(raids_time * ((cd_rate+1)%2));
    }

    // 循环处理扫荡副本
    for(; (GetRaidsDungeonTimes() > 0) && (total_time < cd_time); )
    {
        // 减少一次扫荡次数
        DecRaidsDungeonTimes(GetRaidsDungeonID());

        // 扫荡完成任务
        //m_player->SendTaskEvent(TASK_EVENT_RADIS_DUNGEON, GetRaidsDungeonID(), 0, 0);

        // 活跃度
        m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_PROTECT_SOUL, 1, 0, 0);

        // 扫荡完成任务
        m_player->OnRaidsDungeonCmpl(GetRaidsDungeonID());

        // 总时间
        total_time += raids_time;

        // 获得副本扫荡奖励
        rewardTable.Clear();
        rewardTable.reason = REWARD_REASON_RAIDS_SOUL_DUNGEON;
        rewardTable.reason_arg = GetRaidsDungeonTotalTimes() - GetRaidsDungeonTimes();
        num = 0;
        DungeonGainConfig::Instance()->GetDungeonGainReward(dungeonTmpl, 1, rewardTable.items);

        // 副本奖励
        //ItemManager::AddRewardTableWithEmail(m_player->GetCenterSvrID(), m_player->GetPlayerID(), rewardTable);
    }

    total_times = GetRaidsDungeonTotalTimes();

    // 如果还有剩余的扫荡次数，设置扫荡的剩余时间
    if(GetRaidsDungeonTimes() > 0)
    {
        SetRaidsDungeonEndTime(now + raids_time);
        left_time = raids_time;
    }
    // 否则重置扫荡ID和扫荡结束时间
    else
    {
        ClearRaidsDungeonList();
        SetRaidsDungeonTimes(0);
        SetRaidsDungeonTotalTimes(0);
        SetRaidsDungeonEndTime(0);
    }

    errCode = ERR_SUCCEED;

_END:
    SendRaidsSoulDungeonCmplAck(errCode, GetRaidsDungeonTimes(), total_times, left_time, total_time);
}

// ==================================================
// 检查是否为有效的ID
// ==================================================
bool SoulDungeonManager::IsValidRaidsID(uint32 id)
{
    //if((0 < id) && (id < SOUL_DUNGEON_MAX_NUM) && 
    //    (id < m_soul_dungeon_info.soul_dungeon_times_recorder.size()))
    //{
    //    return true;
    //}

    return false;
}

// ==================================================
// 获得要扫荡的副本的副本次数
// ==================================================
int32 SoulDungeonManager::GetRaidsDungeonTimes(uint32 dungeonID)
{
    //uint32 subId = GetDungeonSubID(dungeonID);

    //if(subId >= m_soul_dungeon_info.soul_dungeon_times_recorder.size())
    //{
    //    return 0;
    //}

    //// 系统次数
    //int32 sysTimes = GameParamConfig::Instance()->GetProtectSoulSysTimes();

    //if(sysTimes > m_soul_dungeon_info.soul_dungeon_times_recorder[subId].sys_times)
    //{
    //    sysTimes -= m_soul_dungeon_info.soul_dungeon_times_recorder[subId].sys_times;
    //}
    //else
    //{
    //    sysTimes = 0;
    //}

    //int32 totalTimes = sysTimes + m_soul_dungeon_info.soul_dungeon_times_recorder[subId].buy_times;

    //return totalTimes;
    return 0;
}

// ==================================================
// 获得扫荡副本ID
// ==================================================
uint32 SoulDungeonManager::GetRaidsDungeonID()
{
    //int32 num = 0;

    //if(m_soul_dungeon_info.raids_dungeon_total_times > m_soul_dungeon_info.raids_dungeon_times)
    //{
    //    num = m_soul_dungeon_info.raids_dungeon_total_times - m_soul_dungeon_info.raids_dungeon_times;
    //}

    //for(std::vector<RAIDS_SOUL_DUNGEON_INFO>::iterator 
    //    soulDungeonIt = m_soul_dungeon_info.raids_dungeon_list.begin();
    //    soulDungeonIt != m_soul_dungeon_info.raids_dungeon_list.end(); ++soulDungeonIt)
    //{
    //    if(num <= soulDungeonIt->radis_times)
    //    {
    //        return soulDungeonIt->dungeon_id;
    //    }
    //    else
    //    {
    //        num -= soulDungeonIt->radis_times;
    //    }
    //}

    return 0;
}

// 发送护魂副本次数列表
void SoulDungeonManager::SendSoulDungeonTimesList()
{
    //RAIDS_SOUL_DUNGEON_TIMES_LIST sendMsg;
    //sendMsg.soul_dungeon_times_recorder = m_soul_dungeon_info.soul_dungeon_times_recorder;
    //m_player->SendToGate(MSG_RAIDS_SOUL_DUNGEON_TIMES_NTF, &sendMsg);
}

// 清除副本扫荡列表
void SoulDungeonManager::ClearRaidsDungeonList()
{
    //m_soul_dungeon_info.raids_dungeon_list.clear();
}

// 获得扫荡副本的剩余次数
int32 SoulDungeonManager::GetRaidsDungeonTimes()
{
    //return m_soul_dungeon_info.raids_dungeon_times;
    return 0;
}

// 扫荡次数减1
void SoulDungeonManager::DecRaidsDungeonTimes(uint32 dungeonID)
{
    //SOUL_DUNGEON_TIMES_RECORDER* timesRecorder = NULL;

    //int32 sysTimes = 0;

    //uint32 subId = GetDungeonSubID(dungeonID);

    //if(m_soul_dungeon_info.raids_dungeon_times > 0)
    //    m_soul_dungeon_info.raids_dungeon_times--;

    //if(!IsValidRaidsID(subId))
    //{
    //    return;
    //}

    //// 系统次数
    //sysTimes = GameParamConfig::Instance()->GetProtectSoulSysTimes();

    //// 减少一次的次数
    //timesRecorder = &m_soul_dungeon_info.soul_dungeon_times_recorder[subId];

    //if(sysTimes > timesRecorder->sys_times)
    //{
    //    // 系统购买是增量
    //    ++timesRecorder->sys_times;
    //}
    //else if(timesRecorder->buy_times > 0)
    //{
    //    // 购买次数减量
    //    --timesRecorder->buy_times;
    //}
}

// 设置扫荡副本的剩余次数
void SoulDungeonManager::SetRaidsDungeonTimes(int32 val)
{
    //m_soul_dungeon_info.raids_dungeon_times = val;
}

// 获得扫荡副本总次数
int32 SoulDungeonManager::GetRaidsDungeonTotalTimes()
{
    //return m_soul_dungeon_info.raids_dungeon_total_times;
    return 0;
}

// 设置扫荡副本总次数
void SoulDungeonManager::SetRaidsDungeonTotalTimes(int32 val)
{
    //m_soul_dungeon_info.raids_dungeon_total_times = val;
}

// 获得扫荡副本的结束时间
uint32 SoulDungeonManager::GetRaidsDungeonEndTime()
{
    //return m_soul_dungeon_info.raids_dungeon_end_time;
    return 0;
}

// 设置扫荡副本的结束时间
void SoulDungeonManager::SetRaidsDungeonEndTime(uint32 val)
{
    //m_soul_dungeon_info.raids_dungeon_end_time = val;
}

// 副本已购买次数
int32 SoulDungeonManager::GetDungeonPurTimes(uint32 dungeon_id)
{
    //uint32 subId = GetDungeonSubID(dungeon_id);

    //if(subId >= m_soul_dungeon_info.soul_dungeon_times_recorder.size())
    //{
    //    return 0;
    //}

    //return m_soul_dungeon_info.soul_dungeon_times_recorder[subId].daily_pur_times;
    return 0;
}

// 副本购买次数增加
void SoulDungeonManager::AddDungeonPurTimes(uint32 dungeon_id, int times)
{
    //uint32 subId = GetDungeonSubID(dungeon_id);

    //if(subId >= m_soul_dungeon_info.soul_dungeon_times_recorder.size())
    //{
    //    return;
    //}

    //m_soul_dungeon_info.soul_dungeon_times_recorder[subId].daily_pur_times += times;
}

// 添加护魂副本次数
void SoulDungeonManager::AddProtectSoulTimes(uint32 dungeon_id, int32 times)
{
//    uint32 subId = GetDungeonSubID(dungeon_id);
//
//    if(times < 0)
//    {
//        CnAssert(false);
//        return;
//    }
//
//    if(subId >= m_soul_dungeon_info.soul_dungeon_times_recorder.size())
//    {
//        CnAssert(false);
//        return;
//    }
//
//    m_soul_dungeon_info.soul_dungeon_times_recorder[subId].buy_times += times;
}

// 发送扫荡副本应答
void SoulDungeonManager::SendRaidsSoulDungeonStartAck(uint16 errCode, int32 raids_times, 
                                                      int32 total_raids_times, uint32 left_time, uint32 total_time)
{
    //RAIDS_SOUL_DUNGEON_START_ACK sendMsg;
    //sendMsg.errcode = errCode;
    //sendMsg.raids_times = raids_times;
    //sendMsg.total_raids_times = total_raids_times;
    //sendMsg.left_time = left_time;
    //sendMsg.total_time = total_time;
    //m_player->SendToGate(MSG_RAIDS_SOUL_DUNGEON_START_ACK, &sendMsg);
}

// 发送扫荡副本完成
void SoulDungeonManager::SendRaidsSoulDungeonCmplAck(uint16 errCode, int32 raids_times, 
                                                     int32 total_raids_times, uint32 left_time,  uint32 total_time)
{
    //RAIDS_SOUL_DUNGEON_CMPL_ACK sendMsg;
    //sendMsg.errcode = errCode;
    //sendMsg.raids_times = raids_times;
    //sendMsg.total_raids_times = total_raids_times;
    //sendMsg.left_time = left_time;
    //sendMsg.total_time = total_time;
    //sendMsg.soul_dungeon_times_recorder = m_soul_dungeon_info.soul_dungeon_times_recorder;
    //m_player->SendToGate(MSG_RAIDS_SOUL_DUNGEON_CMPL_ACK, &sendMsg);
}

// 发送扫荡副本停止
void SoulDungeonManager::SendRaidsSoulDungeonStopAck()
{
    //NONE sendMsg;
    //m_player->SendToGate(MSG_RAIDS_SOUL_DUNGEON_STOP_ACK, &sendMsg);
}

bool SoulDungeonManager::CardBySoulDungeonTime(int32 dungeonID, int32 num )
{
    //RAIDS_SOUL_DUNGEON_BUY_TIMES_REQ recvMsg;

    //RAIDS_SOUL_DUNGEON_INFO infoMsg;
    //infoMsg.dungeon_id = dungeonID;
    //infoMsg.radis_times = num;
    //recvMsg.raids_soul_dungeon_list.push_back(infoMsg);


    //std::vector<RAIDS_SOUL_DUNGEON_INFO>::iterator soulDungeonIt;
    //std::string tradeTxt;
    //uint32 raids_id = 0;
    //int32 countnum = 0;

    //countnum = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_22, m_player->GetVipLevel());

    //// 检查扫荡的列表
    //for(soulDungeonIt=recvMsg.raids_soul_dungeon_list.begin(); 
    //    soulDungeonIt != recvMsg.raids_soul_dungeon_list.end(); ++soulDungeonIt)
    //{
    //    // 扫荡ID
    //    raids_id = GetDungeonSubID(soulDungeonIt->dungeon_id);

    //    // 检查副本类型
    //    if( GetDungeonClass(soulDungeonIt->dungeon_id) != DUNGEON_CLASS_SOUL_ID)
    //    {
    //        return false;
    //    }

    //    // 检查扫荡ID是否在有效范围内
    //    if(raids_id >= m_soul_dungeon_info.soul_dungeon_times_recorder.size())
    //    {
    //        return false;
    //    }

    //    // 检查要扫荡的护魂副本是否开启
    //    if(!m_player->IsPlayerDungeonRadis(soulDungeonIt->dungeon_id))
    //    {
    //        return false;
    //    }

    //    // 检查扫荡次数
    //    if(soulDungeonIt->radis_times < 1)
    //    {
    //        m_player->NotifyErrorCode(ERR_FAILED);
    //        return false;
    //    }

    //    if(countnum < (soulDungeonIt->radis_times + GetDungeonPurTimes(soulDungeonIt->dungeon_id)))
    //    {
    //        m_player->NotifyErrorCode(ERR_FAILED);
    //        return false;
    //    }
    //}

    //write_SOUL_DUNGEON_RAIDS(recvMsg.raids_soul_dungeon_list, tradeTxt);
    //AddProtectSoulNum(tradeTxt);
    return true;
}
