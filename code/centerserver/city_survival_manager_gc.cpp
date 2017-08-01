#include "centerserver_pch.h"
#include "city_survival_manager_gc.h"
#include "game_handler.h"
#include "player.h"

IMPLEMENT_SINGLETON(CitySurvivalManager)

CitySurvivalManager::CitySurvivalManager()
{
}

CitySurvivalManager::~CitySurvivalManager()
{

}

//gs->cs
void CitySurvivalManager::OnGSRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_CITY_SURVIVAL_REALLY_START_NTF:
    case MSG_CITY_SURVIVAL_REALLY_END_NTF:
    case MSG_CITY_SURVIVAL_INFO_NTF:
        {
            uint32 serverId = GameHandler::Instance()->GetRegionIDBySessionID(sessionID);

            // 同步到其他服务器上
            GameHandler::Instance()->SendToAllBut(serverId, msgID, data);
            break;
        }
    default:
        break;
    }
}

//ts->cs
void CitySurvivalManager::OnTSRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_CITY_SURVIVAL_START_NTF: 
        {
            GameHandler::Instance()->SendToAll(msgID, data);
            break;
        }
    case MSG_CITY_SURVIVAL_END_NTF:
        {
            GameHandler::Instance()->SendToAll(msgID, data);
            break;
        }
    default:
        break;
    }
}



