#include "gameholder_pch.h"
#include "contrast_manager.h"
#include "offline_player_manager.h"
#include "event_define.h"
#include "player.h"



ContrastManager::ContrastManager(Player* player) :
m_Player(player)
{
    m_ContrastPlayerName.clear();
}

ContrastManager::~ContrastManager()
{

}

void ContrastManager::OnRecv( uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_CONTRAST_REQ:
        {   // 请求玩家战斗力对比
            GAME_CONTRAST_REQ* req = (GAME_CONTRAST_REQ*)data;
            const OFFLINE_PLAYER_INFO* offline_player_info = OfflinePlayerManager::Instance()->GetOfflinePlayerRecordByName(req->roleName);
            if(offline_player_info)
                SendContrastPlayerInfo(offline_player_info);
            else
            {
                m_ContrastPlayerName = req->roleName;
                OfflinePlayerManager::Instance()->AskCSOfflinePlayerInfoByName(req->roleName, 0);
            }
        }
        break;
    }
}

void ContrastManager::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 )
{
    switch(eventId)
    {
    case SYSTEM_EVENT_LOAD_OFFLINE_PLAYER:
        {
            if(m_ContrastPlayerName.length() > 0)
            {
                const OFFLINE_PLAYER_INFO* offline_player_info = OfflinePlayerManager::Instance()->GetOfflinePlayerRecordByName(m_ContrastPlayerName);
                if(offline_player_info)
                    SendContrastPlayerInfo(offline_player_info);
            }
        }
    }
}

void ContrastManager::SendContrastPlayerInfo(const OFFLINE_PLAYER_INFO* otherPlayerInfo)
{
    m_ContrastPlayerName.clear();

    GAME_CONTRASTS ack;
    m_Player->FillOffilePlayerInfo(ack.me);
    ack.other = *otherPlayerInfo;

    m_Player->SendToGate(MSG_CONTRAST_ACK, &ack);
}