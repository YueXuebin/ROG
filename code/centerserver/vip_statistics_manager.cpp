#include "centerserver_pch.h"
#include "vip_statistics_manager.h"
#include "special_activity_job.h"
#include "game_handler.h"
#include "festival_manager.h"

IMPLEMENT_SINGLETON(VipStatisticsManager)

VipStatisticsManager::VipStatisticsManager()
{
    m_PlayerVipMap.clear();
    m_VipAutoGrowth.Clear();
    m_ConfigInfo.Clear();
    m_minute = 0;
    m_minuteTotal = 1;
}

VipStatisticsManager::~VipStatisticsManager()
{

}

void VipStatisticsManager::SetConfig(const std::string& configJson)
{
    VIP_STATISTICS_CONFIG info;
    if(!JsonToStruct(info, configJson))
        return;

    m_ConfigInfo = info;

    SendVipPlayerNum();
}

void VipStatisticsManager::Init()
{
    SpecialActivityJob::Instance()->GetVipStatisticsInfo(m_VipAutoGrowth, m_PlayerVipMap);

    // 初始化数据
    for(int32 i = 1; i <= 10; ++i)
    {
        std::vector<VIP_STATISTICS_AUTO_GROWTH>::iterator autoIt = m_VipAutoGrowth.auto_list.begin();
        for(; autoIt != m_VipAutoGrowth.auto_list.end(); ++ autoIt)
        {
            if(autoIt->vip_level == i)
                break;
        }

        if(autoIt == m_VipAutoGrowth.auto_list.end())
        {
            VIP_STATISTICS_AUTO_GROWTH autoInfo;
            autoInfo.vip_level = i;
            autoInfo.auto_value = 0;
            m_VipAutoGrowth.auto_list.push_back(autoInfo);
        }
    }
}

void VipStatisticsManager::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_VIP_STATISTICS_PLAYER_VIP:
        onPlayerVipInfo(data);
        break;
    }
}

void VipStatisticsManager::Update()
{
    m_minute += g_ServerUpdateTime;
    if(m_minute < 60000)
        return;

    m_minute = 0;
    ++m_minuteTotal;

    const TASK_FESTIVAL_TEMPLATE* festivalTmp = FestivalManager::Instance()->GetFestivalTemplate(FESTIVAL_VIP_STATISTICS_ID);
    if(!festivalTmp)
    {
        return;     // 无'全服VIP福利'活动
    }

    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
    if(nowTime < festivalTmp->begin_time || nowTime > festivalTmp->end_time)    // 不在活动时间内
        return;

    bool isSend = false;
    for(std::vector<VIP_STATISTICS_DATA>::iterator vipIt = m_ConfigInfo.vip_list.begin(); vipIt != m_ConfigInfo.vip_list.end(); ++vipIt)
    {
        if(vipIt->cycles_minute == 0)
            continue;

        if((m_minuteTotal % vipIt->cycles_minute) == 0)     // 每过给定的分钟数做一次自动增长
        {
            // 每一个小于这个vip等级的人数都应自增长
            for(std::vector<VIP_STATISTICS_AUTO_GROWTH>::iterator autoIt = m_VipAutoGrowth.auto_list.begin(); autoIt != m_VipAutoGrowth.auto_list.end(); ++autoIt)
            {
                if((uint32)autoIt->vip_level > vipIt->level)        // vip等级大于模板等级不增长
                    continue;

                if(GetPlayerNumByVipLevel(autoIt->vip_level) >= vipIt->max_value)   // 玩家数(包括自增长)超过最大增长值
                    continue;

                autoIt->auto_value += vipIt->auto_value;
                isSend = true;
            }
        }
    }

    if(isSend)
        SendVipPlayerNum();
}

void VipStatisticsManager::SaveVipStatistics()
{
    SpecialActivityJob::Instance()->SaveVipStatisticsInfo(m_VipAutoGrowth);
}

VIP_STATISTICS_INFO VipStatisticsManager::GetVipInfo()
{
    VIP_STATISTICS_VIP_PLAYER_NUM vipPlayerNum;
    VIP_STATISTICS_INFO vipInfo;
 
    for(int32 i = 1; i <= 10; ++i)
    {
        vipPlayerNum.vip_level = i;
        vipPlayerNum.player_num = 0;
        vipInfo.vip_list.push_back(vipPlayerNum);
    }

    for(std::map<uint64, int8>::iterator playerIt = m_PlayerVipMap.begin(); playerIt != m_PlayerVipMap.end(); ++playerIt)
    {
        std::vector<VIP_STATISTICS_VIP_PLAYER_NUM>::iterator vipIt = vipInfo.vip_list.begin();
        for(; vipIt != vipInfo.vip_list.end(); ++vipIt)
        {
            if(playerIt->second >= vipIt->vip_level)
            {
                ++vipIt->player_num;
            }
        }
    }

    for(std::vector<VIP_STATISTICS_AUTO_GROWTH>::iterator autoIt = m_VipAutoGrowth.auto_list.begin(); autoIt != m_VipAutoGrowth.auto_list.end(); ++autoIt)
    {
        std::vector<VIP_STATISTICS_VIP_PLAYER_NUM>::iterator vipIt = vipInfo.vip_list.begin();
        for(; vipIt != vipInfo.vip_list.end(); ++vipIt)
        {
            if(autoIt->vip_level == vipIt->vip_level)
            {
                vipIt->player_num += autoIt->auto_value;
                break;
            }
        }
    }
    return vipInfo;
}

void VipStatisticsManager::SendVipPlayerNum()
{
    VIP_STATISTICS_INFO sendMsg = GetVipInfo();

    GameHandler::Instance()->SendToAll(MSG_VIP_STATISTICS_INFO, &sendMsg);
}

void VipStatisticsManager::onPlayerVipInfo(void* data)
{
    VIP_STATISTICS_PLAYER_VIP* recvMsg = (VIP_STATISTICS_PLAYER_VIP*)data;

    CnAssert(0 <= recvMsg->vip_level && recvMsg->vip_level <= 10);
    
    uint64 playerID = U64ID(recvMsg->player_id.id_h, recvMsg->player_id.id_l);
    std::map<uint64, int8>::iterator playerIt = m_PlayerVipMap.find(playerID);
    if(playerIt == m_PlayerVipMap.end())
    {
        if(recvMsg->vip_level == 0 || recvMsg->vip_level > 10)
            return;

        m_PlayerVipMap[playerID] = recvMsg->vip_level;
    }
    else
    {
        if(recvMsg->vip_level == 0)
            m_PlayerVipMap.erase(playerIt);
        else if(recvMsg->vip_level != playerIt->second)
            playerIt->second = recvMsg->vip_level;
        else
            return;
    }

    SendVipPlayerNum();
}

uint32 VipStatisticsManager::GetPlayerNumByVipLevel(int8 vipLevel)
{
    VIP_STATISTICS_INFO vipInfo = GetVipInfo();
    for(std::vector<VIP_STATISTICS_VIP_PLAYER_NUM>::iterator vipIt = vipInfo.vip_list.begin(); vipIt != vipInfo.vip_list.end(); ++vipIt)
    {
        if(vipIt->vip_level == vipLevel)
            return vipIt->player_num;
    }

    return 0;
}
