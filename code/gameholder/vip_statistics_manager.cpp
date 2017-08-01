#include "gameholder_pch.h"
#include "vip_statistics_manager.h"
#include "player_manager.h"
#include "event_define.h"


IMPLEMENT_SINGLETON(VipStatisticsManager)

VipStatisticsManager::VipStatisticsManager()
{

}

VipStatisticsManager::~VipStatisticsManager()
{

}

void VipStatisticsManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_VIP_STATISTICS_INFO:
        onRecvInfo(data);
        break;
    }
}

void VipStatisticsManager::onRecvInfo(void* data)
{
    VIP_STATISTICS_INFO* recvMsg = (VIP_STATISTICS_INFO*)data;

    m_vipInfo = *recvMsg;
    
    const PlayerManager::PlayerMap& playerMap = PlayerManager::Instance()->GetPlayers();
    for(PlayerManager::PlayerMap::const_iterator playerIt = playerMap.begin(); playerIt != playerMap.end(); ++playerIt)
    {
        playerIt->second->SendMyselfEvent(TASK_EVENT_VIP_STATISTICS_REFRESH, 0, 0);
    }
}

uint32 VipStatisticsManager::GetPlayerNumByVipLevel(int8 vipLevel)
{
    for(std::vector<VIP_STATISTICS_VIP_PLAYER_NUM>::iterator vipIt = m_vipInfo.vip_list.begin(); vipIt != m_vipInfo.vip_list.end(); ++vipIt)
    {
        if(vipIt->vip_level == vipLevel)
            return vipIt->player_num;
    }
    return 0;
}

void VipStatisticsManager::VipLevelChange(Player* pPlayer)
{
    if(!pPlayer)
        return;

    VIP_STATISTICS_PLAYER_VIP sendMsg;
    sendMsg.player_id.id_h = U64ID_HIGH(pPlayer->GetPlayerID());
    sendMsg.player_id.id_l = U64ID_LOW(pPlayer->GetPlayerID());
    sendMsg.vip_level = pPlayer->GetVipLevel();
    pPlayer->SendToCenter(MSG_VIP_STATISTICS_PLAYER_VIP, &sendMsg);     // 通知Center，VIP信息
}
