#include "centerserver_pch.h"
#include "gate_handler.h"
#include "game_handler.h"
#include "centerserver.h"
#include "af_transfer_server.h"
#include "player_manager.h"
#include "player.h"
#include "player_net_handler.h"
#include "server_monitor.h"
#include "config_loader.h"


IMPLEMENT_SINGLETON(GateHandler)

GateHandler::GateHandler()
{
    m_gateServer = NULL;
}

GateHandler::~GateHandler()
{
    if(m_gateServer)
        CnDelete m_gateServer;
}

bool GateHandler::Init(const char* ip, uint32 port)
{
    m_gateServer = (AFTransferServerProxy*)CreateTransferServerProxy(ip, port, &RecvMsg, 0);
    if(!m_gateServer)
    {
        return false;
    }
    return true;
}

void GateHandler::RegistRegionID(uint32 centGateID, uint32 sessionID)
{
    CnAssert(m_centGateID2snID.find(centGateID) == m_centGateID2snID.end());
    m_centGateID2snID[centGateID] = sessionID;
    CnInfo("register gateserver cid=%d gid=%d\n", CalCenterIDByRegionID(centGateID), CalGateIDByRegionID(centGateID));
}

void GateHandler::UnregistRegionID(uint32 sessionID)
{
    for(std::map<uint32, uint32>::iterator it = m_centGateID2snID.begin(); it != m_centGateID2snID.end(); ++it)
    {
        if(it->second != sessionID)
            continue;
        uint32 centGateID = it->first;
        uint32 centerID = CalCenterIDByRegionID(centGateID);
        uint32 gateID = CalGateIDByRegionID(centGateID);

        CnInfo("unregister gateserver cid=%d gid=%d\n", centerID, gateID);
        CnAssert(centerID);
        CnAssert(gateID);

        CenterServer::Instance()->UnactiveServer(SERVER_TYPE_GATE, gateID, centerID);

        m_centGateID2snID.erase(it);
        break;
    }
}

bool GateHandler::IsConnectGate(uint32 serverID)
{
    return m_centGateID2snID.find(serverID) != m_centGateID2snID.end();
}

uint32 GateHandler::GetRegionIDBySessionID(uint32 sessionID)
{
    for(std::map<uint32, uint32>::iterator it = m_centGateID2snID.begin(); it != m_centGateID2snID.end(); it++)
    {
        if(it->second == sessionID)
            return it->first;
    }
    return 0;
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

void GateHandler::SendByID(uint32 centGateID, uint32 msgID, void* data, uint64 playerID)
{
    std::map<uint32, uint32>::iterator iter = m_centGateID2snID.find(centGateID);
    if(iter != m_centGateID2snID.end())
        TransferSend(iter->second, msgID, data, playerID);
    else
    {
        // 发送给已经断开的Gate
    }
}

void GateHandler::SendToAll(uint32 msgID, void* data, uint64 playerID)
{
    for(std::map<uint32, uint32>::iterator it = m_centGateID2snID.begin(); it != m_centGateID2snID.end(); ++it)
    {
        TransferSend(it->second, msgID, data, playerID);
    }
}

void GateHandler::OnAccept(uint32 sessionID, void* data)
{
    CnDbg("Accept from gateserver\n");
}

void GateHandler::OnConnect(uint32 sessionID, void* data)
{
}

void GateHandler::OnClose(uint32 sessionID, void* data)
{
    uint32 centGateID = GetRegionIDBySessionID(sessionID);
    if(centGateID)
    {
        PlayerManager::Instance()->OnCloseGateServer(centGateID);
        ServerMonitor::Instance()->OnGateServerClose(centGateID);
    }
    UnregistRegionID(sessionID);
}

void GateHandler::OnError(uint32 sessionID, void* data)
{
    uint32 centGateID = GetRegionIDBySessionID(sessionID);
    if(centGateID)
    {
        PlayerManager::Instance()->OnCloseGateServer(centGateID);
        ServerMonitor::Instance()->OnGateServerClose(centGateID);
    }
    UnregistRegionID(sessionID);
}

void GateHandler::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata)
{
    switch(msgID)
    {
    case MSG_SERVER_INFO_REQ:
        CenterServer::Instance()->RecvServerInfoReq(sessionID, data);
        break;
    case MSG_SERVER_ACTIVE_REQ:
        CenterServer::Instance()->RecvServerActiveReq(sessionID, data);
        break;
    case MSG_LOGIN_GAME_REQ:        // 登陆请求
        CenterServer::Instance()->RecvLoginGameReq(playerID, sessionID, data);
        break;
    case MSG_CREATE_PLAYER_REQ:     // 创建角色
        CenterServer::Instance()->RecvCreatePlayerReq(playerID, sessionID, data);
        break;
    case MSG_IS_CREATE_PLAYER_NAME_REQ:     // 角色名是否可用
        CenterServer::Instance()->RecvIsCreatePlayerReq(playerID, sessionID, data);
        break;
    case MSG_ASK_PLAYER_REQ:        // 查询角色信息
        CenterServer::Instance()->RecvAskPlayerInfoReq(playerID, sessionID, data);
        break;
    case MSG_LOAD_PLAYER_REQ:       // 加载角色请求
        CenterServer::Instance()->RecvLoadPlayerReq(playerID, sessionID, data);
        break;
    case MSG_SET_PLAYER_ID_ACK:
        RecvSetPlayerIDAck(sessionID, data);
        break;
    case MSG_SET_GAMESVR_ID_ACK:
        RecvSetGameSvrIDAck(sessionID, data);
        break;
    case MSG_LOGOUT_GAME:
        RecvLogoutGame(sessionID, data);
        break;
    default:
        break;
    }
}

void GateHandler::SendSetPlayerIDReq(uint32 serverID, uint64 loginID, uint64 playerID)
{
    SET_PLAYER_ID_REQ sendMsg;
    sendMsg.login_id_l = U64ID_LOW(loginID);
    sendMsg.login_id_h = U64ID_HIGH(loginID);
    sendMsg.player_id_l = U64ID_LOW(playerID);
    sendMsg.player_id_h = U64ID_HIGH(playerID);
    SendByID(serverID, MSG_SET_PLAYER_ID_REQ, &sendMsg);
}

void GateHandler::SendSetGameSvrIDReq(uint32 serverID, uint64 playerID, uint32 gameSvrID)
{
    SET_GAMESVR_ID_REQ sendMsg;
    sendMsg.player_id_l = U64ID_LOW(playerID);
    sendMsg.player_id_h = U64ID_HIGH(playerID);
    sendMsg.gamesvr_id = gameSvrID;
    SendByID(serverID, MSG_SET_GAMESVR_ID_REQ, &sendMsg);
}

// ==================================================
// 发送角色踢出游戏的请求
// ==================================================
void GateHandler::SendKickOutGame(uint32 gateSvrID, uint64 loginID, uint16 errCode)
{
    KICKOUT_GAME sendMsg;
    sendMsg.errcode = errCode;
    SendByID(gateSvrID, MSG_KICKOUT_GAME, &sendMsg, loginID);
}

void GateHandler::SendLoginGameAck(uint16 errCode, uint32 centGateID, uint64 loginID)
{
    if(errCode != ERR_SUCCEED)
    {
        CnDbg("SendLoginGameAck err=%d\n", errCode);
    }

    LOGIN_GAME_ACK sendMsg;
    sendMsg.errcode = errCode;
    SendByID(centGateID, MSG_LOGIN_GAME_ACK, &sendMsg, loginID);
}

void GateHandler::SendLoadPlayerAck(uint16 errCode, uint32 serverID, uint64 loginID)
{
    if(errCode != ERR_SUCCEED)
    {
        CnDbg("SendLoadPlayerAck err=%d\n", errCode);
    }
    ERROR_CODE ack;
    ack.errcode = errCode;
    SendByID(serverID, MSG_LOAD_PLAYER_ACK, &ack, loginID);                         // 此协议使用loginID
}

void GateHandler::SendCreatePlayerAck(uint16 errCode, uint32 serverID, uint64 loginID)
{
    ERROR_CODE sendMsg;
    sendMsg.errcode = errCode;
    SendByID(serverID, MSG_CREATE_PLAYER_ACK, &sendMsg, loginID);                   // 此协议使用loginID
}

void GateHandler::SendIsCreatePlayerNameAck(uint16 errCode, uint32 serverID, uint64 loginID)
{
    ERROR_CODE sendMsg;
    sendMsg.errcode = errCode;
    SendByID(serverID, MSG_IS_CREATE_PLAYER_NAME_ACK, &sendMsg, loginID);           // 此协议使用loginID
}

void GateHandler::SendAskPlayerAck(ASK_PLAYER_LIST& playerList, uint32 serverID, uint64 loginID)
{
    SendByID(serverID, MSG_ASK_PLAYER_ACK, &playerList, loginID);                   // 此协议使用loginID
}

void GateHandler::SendChangeDungeonAck(uint16 errCode, uint32 serverID, uint64 playerID)
{
    CHANGE_DUNGEON_ACK sendMsg;
    sendMsg.errcode = errCode;          // 失败原因
    sendMsg.server_id = 0;
    sendMsg.gameserver_type = 0;
    sendMsg.player_object_id = 0;
    sendMsg.union_id = 0;
    sendMsg.x = 0;
    sendMsg.y = 0;
    sendMsg.now.time = (uint32)Crown::SDNow().GetTimeValue();
    SendByID(serverID, MSG_CHANGE_DUNGEON_ACK, &sendMsg, playerID);
}

void GateHandler::RecvSetPlayerIDAck(uint32 sessionID, void* data)
{
    SET_PLAYER_ID_ACK* recvMsg = (SET_PLAYER_ID_ACK*)data;
    Player* player = nullptr;
    uint64 loginID = 0;

    if(!recvMsg) return;

    loginID = U64ID(recvMsg->login_id_h, recvMsg->login_id_l);

    player = PlayerManager::Instance()->GetPlayerByLoginID(loginID);
    if(!player)
    {
        return;
    }

    // 更新好友、公会的在线状态
    player->UpdateSocialState();

    player->GetNetHandler()->RecvSetPlayerIDAck(recvMsg);
}

// ==================================================
// 收到角色的离线请求
// ==================================================
void GateHandler::RecvLogoutGame( uint32 sessionID, void* data )
{
    LOGOUT_GAME* recvMsg = (LOGOUT_GAME*) data;
    uint64 loginID = 0;
    Player* player = NULL;

    if(!recvMsg) return;

    loginID = U64ID(recvMsg->login_id_h, recvMsg->login_id_l);

    player = PlayerManager::Instance()->GetPlayerByLoginID(loginID);
    if(!player)
    {
        CnDbg("gate logout player loginID=%llu not exist\n", loginID);      // 当客户端之链接，不创建角色就断线
        return;
    }

    PlayerManager::Instance()->LogoutGame(player);
}

void GateHandler::RecvSetGameSvrIDAck(uint32 sessionID, void* data)
{
    SET_GAMESVR_ID_ACK* recvMsg = (SET_GAMESVR_ID_ACK*)data;
    if(!recvMsg)
        return;
    
    uint64 playerID = 0;
    playerID = U64ID(recvMsg->player_id_h, recvMsg->player_id_l);

    Player* player = nullptr;
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        CnAssert(false);
        return;
    }
    
    player->GetNetHandler()->RecvSetGameSvrIDAck(recvMsg);
}
