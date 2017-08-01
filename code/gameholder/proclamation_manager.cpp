#include "gameholder_pch.h"
#include "proclamation_manager.h"
#include "center_handler.h"
#include "player.h"
#include "gate_handler.h"

IMPLEMENT_SINGLETON(ProclamationManager)

ProclamationManager::ProclamationManager(void)
{
}

ProclamationManager::~ProclamationManager(void)
{
}

void ProclamationManager::OnServerReady()
{
    // 加载配置信息
    NONE proclMsg;
    CenterHandler::Instance()->SendToMasterCenter(MSG_GLOBAL_SETTING_PROCLAMATION_REQ, &proclMsg);        // call -> MSG_WORLD_BOSS_INFO_LOAD
}

void ProclamationManager::OnRecv( uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_GLOBAL_SETTING_PROCLAMATION_ACK:
        {
            // 更新配置信息
            GLOBAL_PROCLAMATION_BAG* recvMsg = (GLOBAL_PROCLAMATION_BAG*)data;
            LoadProclamationInfo(*recvMsg);
        }
        break;
    default:
        break;
    }

}

void ProclamationManager::LoadProclamationInfo(GLOBAL_PROCLAMATION_BAG& info)
{
    m_proclamationList = info;
}

void ProclamationManager::OnPlayerProclamationReq(Player* player, void* data)
{
    if(!player)
    {
        CnWarn("player proclamation req error\n");
        return;
    }

    GLOBAL_PROCLAMATION_BAG recvMsg ;
    recvMsg.chat_list = m_proclamationList.chat_list;

    GateHandler::Instance()->SendByGateID(player->GetGateSvrID(), MSG_GLOBAL_SETTING_PROCLAMATION_NTF, &recvMsg, player->GetPlayerID());
}