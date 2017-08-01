#include "centerserver_pch.h"
#include "offline_event_manager.h"
#include "offline_event_job.h"
#include "player.h"
#include "player_manager.h"

IMPLEMENT_SINGLETON(OfflineEventManager)

OfflineEventManager::OfflineEventManager()
{

}

OfflineEventManager::~OfflineEventManager()
{

}

void OfflineEventManager::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_OFFLINE_EVENT_SEND:
        OnOfflineEventSend(data, playerID);
        break;
    }
}

void OfflineEventManager::SendOfflineEvent(uint64 playerID, uint32 eventID, const std::string& eventInfo)
{
    Player* pPlayer = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(pPlayer)
    {
        OFFLINE_EVENT_INFO evenInfo;
        evenInfo.event_id = eventID;
        evenInfo.info_str = eventInfo;
        OFFLINE_EVENT_LIST sendMsg;
        sendMsg.event_list.push_back(evenInfo);
        pPlayer->SendToGame(MSG_OFFLINE_EVENT_NTF, &sendMsg);
        return;
    }

    if(!OfflineEventJob::Instance()->SaveOfflineEvent(playerID, eventID, eventInfo))
    {
        CnError("save offline event err!");
    }
}

void OfflineEventManager::SendOfflineEvent(const std::string& account_name, const std::string& server_name, uint32 eventID, const std::string& eventInfo)
{
    Player* pPlayer = PlayerManager::Instance()->GetPlayerByAccount(account_name, server_name);
    if(pPlayer)
    {
        OFFLINE_EVENT_INFO evenInfo;
        evenInfo.event_id = eventID;
        evenInfo.info_str = eventInfo;
        OFFLINE_EVENT_LIST sendMsg;
        sendMsg.event_list.push_back(evenInfo);
        pPlayer->SendToGame(MSG_OFFLINE_EVENT_NTF, &sendMsg);
        return;
    }

    if(!OfflineEventJob::Instance()->SaveOfflineEvent(account_name, server_name, eventID, eventInfo))
    {
        CnError("save offline event err!");
    }
}

void OfflineEventManager::OnPlayerChangeDungeon(Player* pPlayer)
{
    std::vector<offlineEventData> offlineEventList;
    offlineEventList.clear();

    OFFLINE_EVENT_LIST sendMsg;
    OFFLINE_EVENT_INFO eventInfo;

   // OfflineEventJob::Instance()->LoadOfflineEvent(pPlayer->GetPlayerID(), offlineEventList);

    for(std::vector<offlineEventData>::iterator eventIt = offlineEventList.begin(); eventIt != offlineEventList.end(); ++eventIt)
    {
        eventInfo.Clear();
        eventInfo.event_id = eventIt->event_id;
        eventInfo.info_str = eventIt->eventInfo;
        sendMsg.event_list.push_back(eventInfo);
    }

    pPlayer->SendToGame(MSG_OFFLINE_EVENT_NTF, &sendMsg);

 //   OfflineEventJob::Instance()->DelOfflineEvent(pPlayer->GetPlayerID());
}

void OfflineEventManager::OnOfflineEventSend(void* data, uint64 playerID)
{
    OFFLINE_EVENT_INFO* recvMsg = (OFFLINE_EVENT_INFO*)data;

    SendOfflineEvent(playerID, recvMsg->event_id, recvMsg->info_str);
}
