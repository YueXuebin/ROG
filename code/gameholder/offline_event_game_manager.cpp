#include "gameholder_pch.h"
#include "offline_event_game_manager.h"
#include "player.h"
#include "player_manager.h"
#include "scene.h"
#include "center_handler.h"

IMPLEMENT_SINGLETON(OfflineEventGameManager)

OfflineEventGameManager::OfflineEventGameManager()
{

}

OfflineEventGameManager::~OfflineEventGameManager()
{

}

void OfflineEventGameManager::OnRecv(uint64 playerID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_OFFLINE_EVENT_NTF:
        onOfflineEventNtf(playerID, data);
        break;
    }
}

void OfflineEventGameManager::sendOfflineEvent(uint64 playerID, uint32 centerID, uint32 eventID, std::string& eventInfo)
{
    Player* pPlayer = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(pPlayer)
    {
        Scene* scene = 0;
        scene = pPlayer->GetScene();
        if(scene)
        {
            //scene->SendEvent(pPlayer->GetId(), eventID, 0, 0, 0, eventInfo);
        }
        else
            CnError("offline event scene is not exist! player_id:%llu, event_id:%d, event_info:%s.\n", pPlayer->GetPlayerID(), eventID, eventInfo.c_str());
    }
    else
    {
        OFFLINE_EVENT_INFO sendMsg;
        sendMsg.event_id = eventID;
        sendMsg.info_str = eventInfo;
        CenterHandler::Instance()->SendByCenterID(centerID, MSG_OFFLINE_EVENT_SEND, &sendMsg, playerID);
    }
}

void OfflineEventGameManager::onOfflineEventNtf(uint64 playerID, void* data)
{
    OFFLINE_EVENT_LIST* recvMsg = (OFFLINE_EVENT_LIST*)data;
    Player* pPlayer = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!pPlayer)
    {
        CnError("offline event lose! playerID:%d");
        return;
    }

    for(std::vector<OFFLINE_EVENT_INFO>::iterator eventIt = recvMsg->event_list.begin(); eventIt != recvMsg->event_list.end(); ++eventIt)
    {
        Scene* scene = 0;
        scene = pPlayer->GetScene();
        if(scene)
        {
            //scene->SendEvent(pPlayer->GetId(), eventIt->event_id, 0, 0, 0, eventIt->info_str);
        }
        else
            CnError("offline event scene is not exist! player_id:%llu, event_id:%d, event_info:%s.\n", pPlayer->GetPlayerID(), eventIt->event_id, eventIt->info_str.c_str());
    }
}
