#include "centerserver_pch.h"
#include "server_info_handler.h"
#include "af_transfer_server.h"
#include "player_manager.h"
#include "player.h"
#include "dungeon_manager.h"

IMPLEMENT_SINGLETON(ServerInfoHandler)

ServerInfoHandler::ServerInfoHandler()
{
    m_InfoServer = 0;
}

ServerInfoHandler::~ServerInfoHandler()
{
    if (m_InfoServer)
        CnDelete m_InfoServer;
}

bool ServerInfoHandler::Init(const char* ip, uint32 port)
{
    m_InfoServer = (AFTransferServerProxy*)CreateTransferServerProxy(ip, port, &RecvMsg, 0);
    if(!m_InfoServer)
    {
        return false;
    }
    return true;
}

void ServerInfoHandler::RecvMsg(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata)
{
    switch(msgID)
    {
    case MSG_NET_ACCEPT:
        ServerInfoHandler::Instance()->OnAccept(sessionID, data);
        break;
    case MSG_NET_CONNECT:
        ServerInfoHandler::Instance()->OnConnect(sessionID, data);
        break;
    case MSG_NET_CLOSE:
        ServerInfoHandler::Instance()->OnClose(sessionID, data);
        break;
    case MSG_NET_ERROR:
        ServerInfoHandler::Instance()->OnError(sessionID, data);
        break;
    default:
        ServerInfoHandler::Instance()->OnRecv(playerID, sessionID, msgID, data, userdata);
        break;
    }
}

void ServerInfoHandler::OnAccept(uint32 sessionID, void* data)
{

}

void ServerInfoHandler::OnConnect(uint32 sessionID, void* data)
{

}

void ServerInfoHandler::OnClose(uint32 sessionID, void* data)
{

}

void ServerInfoHandler::OnError(uint32 sessionID, void* data)
{

}

void ServerInfoHandler::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata)
{
    switch(msgID)
    {
    case MSG_MONITOR_INFO_REQ:
        {
            // 返回监控信息
            MONITOR_INFO_REQ* Info_Center = (MONITOR_INFO_REQ*) data;
            //CnInfo("Command received: %d \n",Info_Center->test);
            MONITOR_INFO_ACK PlayerName;
            //返回在线玩家数

            //返回玩家信息列表
            std::map<std::string, Player*> p = PlayerManager::Instance()->GetAccountMap();
            for (std::map<std::string, Player*>::iterator it = p.begin();it != p.end(); ++it)
            {
                MONITOR_PLAYER_INFO pi;                
                pi.name= it->first;
                pi.acount = it->second->GetAccount();
                pi.dungeon_id = it->second->GetUnionMapID();
                PlayerName.player_list.push_back(pi);
            }
            
            //返回玩家等级信息
            m_InfoServer->Send(sessionID, MSG_MONITOR_INFO_ACK, &PlayerName);
        }
        break;
    case MSG_MONITOR_DUNGEON_INFO_REQ:
        {
            OnDoungenReq(sessionID);
        }
        break;
    default:
        break;
    }
}

void ServerInfoHandler::OnDoungenReq( uint32 sessionID )
{
    MONITOR_DUNGEON_INFO_ACK dungeonInfo;

    // 将主城信息放入sameCity中
    //DungeonManager::Instance()->GetDungeonPlayerCount(dungeonInfo.dungeon_list);

    m_InfoServer->Send(sessionID, MSG_MONITOR_DUNGEON_INFO_ACK, &dungeonInfo);
}
