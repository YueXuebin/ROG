#include "gameholder_pch.h"
#include "gate_handler.h"
#include "gameworld.h"
#include "server_config.h"
#include "player_manager.h"
#include "scene.h"
#include "dungeon.h"
#include "dungeon_set.h"
#include "config_manager.h"
#include "regular.h"
#include "pet_manager.h"


IMPLEMENT_SINGLETON(GateHandler)

GateHandler::GateHandler()
{
    m_gateServer = NULL;

    for(uint32 i=0; gClientMsgIDSet[i]; ++i)
    {
        m_clientMsgID.insert(gClientMsgIDSet[i]);
    }
}

GateHandler::~GateHandler()
{
    if(m_gateServer)
    {
        CnDelete m_gateServer;
    }
}

bool GateHandler::Init(const char* ip, uint32 port)
{
    m_gateServer = CreateTransferServerProxy(ip, port, &RecvMsg, 0);
    if(!m_gateServer)
    {
        return false;
    }

    m_ListenIp = ip;
    m_ListenPort = port;

    return true;
}

void GateHandler::CloseAll()
{
    if(!m_gateServer)
    {
        CnAssert(false);
        return;
    }

    for(std::map<uint32, uint32>::iterator iter = m_sessionMap.begin(); iter != m_sessionMap.end(); iter++)
    {
        uint32 region_id = iter->first;
        uint32 session_id = iter->second;
        uint32 gateId = CalGateIDByRegionID(region_id);
        uint32 centerId = CalCenterIDByRegionID(region_id);
        CnDbg("close gate cid=%d gid=%d\n", centerId, gateId);
        m_gateServer->Close(session_id);
        UnregisterServer(session_id);
    }
}

void GateHandler::Close(uint32 closeCenterId)
{
    if(!m_gateServer)
    {
        CnAssert(false);
        return;
    }

    CnAssert(closeCenterId != 0);

    bool hasMore = true;

    while (hasMore)
    {
        hasMore = false;
        std::map<uint32, uint32>::iterator iter = m_sessionMap.begin();
        for(; iter != m_sessionMap.end(); iter++)
        {
            uint32 region_id = iter->first;
            uint32 gateId = CalGateIDByRegionID(region_id);
            uint32 centerId = CalCenterIDByRegionID(region_id);
            if(centerId != closeCenterId)
                continue;
            CnDbg("close gate cid=%d gid=%d\n", centerId, gateId);
            break;
        }

        if(iter != m_sessionMap.end())
        {
            uint32 session_id = iter->second;
            m_gateServer->Close(session_id);
            UnregisterServer(session_id);
            hasMore = true;
        }
    }

    
}

void GateHandler::RecvMsg(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata)
{
    switch(msgID)
    {
    case MSG_NET_ACCEPT:
        GateHandler::Instance()->OnAccept(sessionID, data);
        break;
    case MSG_NET_CONNECT:
        GateHandler::Instance()->OnConnect(sessionID, data);
        break;
    case MSG_NET_CLOSE:
        GateHandler::Instance()->OnClose(sessionID, data);
        break;
    case MSG_NET_ERROR:
        GateHandler::Instance()->OnError(sessionID, data);
        break;
    default:
        GateHandler::Instance()->OnRecv(playerID, sessionID, msgID, data, userdata);
        break;
    }
}

void GateHandler::OnAccept(uint32 sessionID, void* data)
{
    CnInfo("accept gate sessionid=%d\n", sessionID);
    SendServerGateIDReq(sessionID);
}

void GateHandler::OnConnect(uint32 sessionID, void* data)
{
}

void GateHandler::OnClose(uint32 sessionID, void* data)
{
    UnregisterServer(sessionID);
}

void GateHandler::OnError(uint32 sessionID, void* data)
{
    UnregisterServer(sessionID);
}

void GateHandler::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata)
{
    switch(msgID)
    {
    case MSG_SERVER_GATEID_ACK:
        RecvServerGateIDAck(sessionID, data);
        break;
    default:
        {
            if(IsValidClientMsgID(msgID))
            {
                Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
                if(!player) return;
                player->OnRecv(msgID, data);
            }
        }
        break;
    }
}

void GateHandler::UnregisterServer(uint32 sessionID)
{
    for(std::map<uint32, uint32>::iterator iter = m_sessionMap.begin(); iter != m_sessionMap.end(); iter++)
    {
        if(iter->second == sessionID)
        {
            uint32 region_id = iter->first;
            uint32 gate_id = CalGateIDByRegionID(region_id);
            uint32 center_id = CalCenterIDByRegionID(region_id);

            CnInfo("unregister gate cid=%d gid=%d\n", center_id, gate_id);

            m_sessionMap.erase(iter);
            break;
        }
    }
}

void GateHandler::SendServerGateIDReq(uint32 sessionID)
{
    NONE sendMsg;
    TransferSend(sessionID, MSG_SERVER_GATEID_REQ, &sendMsg);
}

void GateHandler::RecvServerGateIDAck(uint32 sessionID, void* data)
{
    SERVER_GATEID_ACK* recvMsg = (SERVER_GATEID_ACK*)data;

    if(!recvMsg) return;

    uint32 region_id = CalRegionID(recvMsg->center_id, recvMsg->gate_id);

    if(m_sessionMap.find(region_id) == m_sessionMap.end())
    {
        m_sessionMap[region_id] = sessionID;
        CnInfo("accept gateserver cid=%d gid=%d\n", recvMsg->center_id, recvMsg->gate_id);
    }
    else
    {
        CnAssert(m_sessionMap[region_id] == sessionID);        // 相同的centerID和gateID
    }
}

void GateHandler::SendChangeDungeonAck(uint16 errCode, Player* player, Scene* scene)
{
    CHANGE_DUNGEON_ACK sendMsg;
    if(!player) return;
    sendMsg.errcode = errCode;
    sendMsg.server_id = ServerConfig::Instance()->GetMyID();
    sendMsg.gameserver_type = ServerConfig::Instance()->GetMyType();
    sendMsg.player_object_id = player->GetId();
    sendMsg.soldier_object_id = player->GetPetMgr()->GetSoldierObjectID();
    sendMsg.union_id = scene->GetUnionID();
    sendMsg.x = player->GetX();
    sendMsg.y = player->GetY();
    sendMsg.angle = player->GetAngle();
    sendMsg.now.time = (uint32)Crown::SDNow().GetTimeValue();
    sendMsg.language = ConfigManager::Instance()->GetLanguage();
    sendMsg.timezone = Crown::SDGetTimeZone();
    player->SendToGate(MSG_CHANGE_DUNGEON_ACK, &sendMsg);
}

bool GateHandler::SendByGateID(uint32 gateID, uint32 msgID, void* data, uint64 playerID)
{
    std::map<uint32, uint32>::iterator it = m_sessionMap.find(gateID);

    if(it == m_sessionMap.end())
        return false;

    if(TransferSend(it->second, msgID, data, playerID))
    {
        return false;
    }

    return true;
}

bool GateHandler::SendByCenterGateID(uint32 centerID, uint32 gateID, uint32 msgID, void* data, uint64 playerID )
{
    CnAssert(false);            // 暂无此函数需求
    std::map<uint32, uint32>::iterator it;
    uint32 id = 0;

    id = CalRegionID(centerID, gateID);

    it = m_sessionMap.find(id);
    if(it == m_sessionMap.end())
        return false;

    if(TransferSend(it->second, msgID, data, playerID))
    {
        return false;
    }

    return true;
}

void GateHandler::SendToAll(uint32 msgID, void* data, uint64 playerID)
{
    for(std::map<uint32, uint32>::iterator it = m_sessionMap.begin(); it != m_sessionMap.end(); ++it)
    {
        TransferSend(it->second, msgID, data, playerID);
    }
}

bool GateHandler::IsValidClientMsgID(uint32 msgID)
{
    if(m_clientMsgID.find(msgID) != m_clientMsgID.end())
    {
        return true;
    }
    return false;
}
