#include "gateserver_pch.h"
#include "config_loader.h"
#include "net_manager.h"
#include "client_handler.h"
#include "game_handler.h"
#include "center_handler.h"
#include "player_manager.h"
#include "player.h"
#include "colorlog.h"


IMPLEMENT_SINGLETON(CenterHandler)

CenterHandler::CenterHandler()
{
    m_state = NET_STATE_INIT;
    m_NeedReconnect = false;
    m_closedtime = 0;
    memset(m_ip, 0, sizeof(m_ip));
    m_port = 0;
    m_clientID = 0;
}

CenterHandler::~CenterHandler()
{

}

bool CenterHandler::Init(const char* ip, uint32 port)
{
    CnInfo("connect to center %s:%d\n", ip, port);

    Crown::SDStrncpy(m_ip, ip, 20);
    m_port = port;
    Connect();
    return true;
}

void CenterHandler::Update()
{
    switch(m_state)
    {
    case NET_STATE_INIT:
    case NET_STATE_CREATE:
    case NET_STATE_RECONNECT:
    case NET_STATE_CONNECTED:
    case NET_STATE_CLOSED:
    default:
        break;
    }

    if(m_NeedReconnect && (Crown::SDGetTickCount() - m_closedtime) > 3000)
    {
        m_NeedReconnect = false;
        Connect();
    }
}

void CenterHandler::Connect()
{
    m_clientID = Net_Manager::Instance()->create_tcp_client(m_ip, m_port, &m_splitter_factory, (void*)(&RecvNetPacket), 10, true);
    m_state = NET_STATE_RECONNECT;
}

void CenterHandler::RecvNetPacket(uint32 eventType, uint32 sessionID, Net_Packet* packet, bool& delpacket)
{
    switch(eventType)
    {
    case TYPE_CONNECT:
        CenterHandler::Instance()->OnConnect(sessionID);
        break;
    case TYPE_CLOSE:
        CenterHandler::Instance()->OnClose(sessionID);
        break;
    case TYPE_EXCEPTION:
        CenterHandler::Instance()->OnError(sessionID);
        break;
    default:
        RecvMsg(sessionID, packet, delpacket);
        break;
    }
}

void CenterHandler::RecvMsg(uint32 sessionID, Net_Packet* packet, bool& delpacket)
{
    packet->read_uint16();
    uint16 timeStamp = packet->read_uint16();
    uint32 dataLen = packet->read_uint32();
    uint64 playerID = packet->read_uint64();
    uint32 msgID = packet->read_uint32();

    switch(msgID)
    {
    case MSG_SERVER_INFO_ACK:
    case MSG_SERVER_ACTIVE_ACK:
    case MSG_SERVER_LIST_ACK:
    case MSG_SERVER_ALL_READY:
    case MSG_LOGIN_GAME_ACK:
    case MSG_LOAD_PLAYER_ACK:
    case MSG_KICKOUT_GAME:
    case MSG_SET_PLAYER_ID_REQ:
    case MSG_SET_GAMESVR_ID_REQ:
        {
            DeFuncCMessageEncoder pfnDecode = CMessageEncoder::Instance()->FindDecodeFunc(msgID);
            if(NULL == pfnDecode)
                return;
            pfnDecode(playerID, sessionID, packet, (void*)(&OnRecv), USERDATA_TYPE_FUNC);
        }
        break;
    default:
        {
            // 角色广播ID
            if(playerID == BROADCAST_PLAYER_ID)
            {
                packet->movetobegin();
                PlayerManager::Instance()->SendToAll(packet);
            }
            else
            {
                Player* player = NULL;

                if(msgID == MSG_CREATE_PLAYER_ACK 
                    || msgID == MSG_IS_CREATE_PLAYER_NAME_ACK 
                    || msgID == MSG_ASK_PLAYER_ACK)
                    player = PlayerManager::Instance()->GetPlayerByLoginID(playerID);
                else
                    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);

                if(!player)
                    return;

                if(MSG_CHANGE_DUNGEON_ACK == msgID)
                {
                    if(Player::kStateChangeDungeon == player->GetState())
                    {
                        player->SetState(Player::kStateInGame);
                    }
                }

                packet->movetobegin();
                Net_Packet* sendPacket = new Net_Packet;
                sendPacket->cloneData(packet);

                // 加密
                if(player->GetState() >= Player::kStateSetPrivateKey)
                {    
                    player->CompressPacket(sendPacket);
                    player->EncryptPacket(sendPacket);
                }

                player->OnSendMessage(msgID);

                if(Net_Manager::Instance()->send_packet(player->GetSessionID(), sendPacket) != 0)
                {
                    delete sendPacket;
                }
            }
        }
        break;
    }
}

bool CenterHandler::Send(uint32 msgID, void* data, uint64 playerID)
{
    Net_Packet* packet = new Net_Packet;
    if(CMessageEncoder::Instance()->Encode(playerID, m_clientID, msgID, data, packet))
    {
        delete packet;
        return false;
    }

    if(Net_Manager::Instance()->send_packet(m_clientID, packet))
    {
        delete packet;
        return false;
    }

    return true;
}

bool CenterHandler::Send(Net_Packet* packet)
{
    if(Net_Manager::Instance()->send_packet(m_clientID, packet))
    {
        delete packet;
        return false;
    }
    return true;
}

void CenterHandler::OnConnect(uint32 sessionID)
{
    CnInfo("OnConnect to center %s:%d\n", m_ip, m_port);
    m_state = NET_STATE_CONNECTED;
    SendServerInfoReq();
}

void CenterHandler::OnClose(uint32 sessionID)
{
    ConfigLoader::Instance()->SetCenterID(0);
    m_clientID = 0;
    m_state = NET_STATE_CLOSED;
    m_closedtime = Crown::SDGetTickCount();
    m_NeedReconnect = true;
    PlayerManager::Instance()->CloseCenterServer();
}

void CenterHandler::OnError(uint32 sessionID)
{
    ConfigLoader::Instance()->SetCenterID(0);
    m_clientID = 0;
    m_state = NET_STATE_CLOSED;
    m_closedtime = Crown::SDGetTickCount();
    m_NeedReconnect = true;
    PlayerManager::Instance()->CloseCenterServer();
}

void CenterHandler::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata)
{
    if(NET_STATE_CONNECTED != CenterHandler::Instance()->GetState()) return;

    switch(msgID)
    {
    case MSG_SERVER_INFO_ACK:
        Instance()->RecvServerInfoAck(data);
        break;
    case MSG_SERVER_ACTIVE_ACK:
        Instance()->RecvServerActiveAck(data);
        break;
    case MSG_SERVER_LIST_ACK:
        Instance()->RecvServerListAck(data);
        break;
    case MSG_SERVER_ALL_READY:
        Instance()->RecvServerReadyAck(data);
        break;
    case MSG_LOGIN_GAME_ACK:
        Instance()->RecvLoginGameAck(playerID, data);
        break;
    case MSG_LOAD_PLAYER_ACK:
        Instance()->RecvLoadPlayerAck(playerID, data);
        break;
    case MSG_KICKOUT_GAME:
        Instance()->RecvKickoutGame(playerID, data);
        break;
    case MSG_SET_PLAYER_ID_REQ:
        Instance()->RecvSetPlayerIDReq(data);
        break;
    case MSG_SET_GAMESVR_ID_REQ:
        Instance()->RecvSetGameSvrIDReq(data);
        break;
    default:
        break;
    }
}

// ==================================================
// 发送角色离开游戏
// ==================================================
void CenterHandler::SendLogoutGame(uint64 loginID)
{
    LOGOUT_GAME sendMsg;
    sendMsg.exitcode;
    sendMsg.login_id_l = U64ID_LOW(loginID);
    sendMsg.login_id_h = U64ID_HIGH(loginID);
    Send(MSG_LOGOUT_GAME, &sendMsg);
}

// ==================================================
// 发送请求服务器信息
// ==================================================
void CenterHandler::SendServerInfoReq()
{
    CnDbg("SendServerInfoReq\n");
    SERVER_INFO_REQ sendMsg;
    sendMsg.server_type = SERVER_TYPE_GATE;
    sendMsg.server_id = ConfigLoader::Instance()->GetMyID();
    sendMsg.version = VERSION_FULL;
    sendMsg.center_id = ConfigLoader::Instance()->GetCenterID();
    Send(MSG_SERVER_INFO_REQ, &sendMsg);
}

// ==================================================
// 接收服务器信息
// ==================================================
void CenterHandler::RecvServerInfoAck(void* data)
{
    SERVER_INFO_ACK* recvMsg = (SERVER_INFO_ACK*)data;
    if(!recvMsg) return;

    CnDbg("RecvServerInfoAck centerId=%d listen %s:%d\n", recvMsg->center_id, recvMsg->server_ip.c_str(), recvMsg->server_port);

    if(recvMsg->errcode != ERR_SUCCEED)
    {
        if(recvMsg->errcode == ERR_VERSION_ERROR)
            CnError("version failed\n");
        else
            CnError("Get server info failed\n");
        return;
    }

    if(!ClientHandler::Instance()->Init(recvMsg->server_ip.c_str(), recvMsg->server_port))
    {
        CnFatal("Create client listener failed\n");
        return;
    }

    SendServerActiveReq();
}

// ==================================================
// 发送请求服务器激活
// ==================================================
void CenterHandler::SendServerActiveReq()
{
    CnDbg("SendServerActiveReq\n");

    CnAssert(ConfigLoader::Instance()->GetCenterID() == 0);     // 等Ack时才会设置CenterID

    SERVER_ACTIVE_REQ sendMsg;
    sendMsg.server_type = SERVER_TYPE_GATE;
    sendMsg.server_id = ConfigLoader::Instance()->GetMyID();
    sendMsg.center_id = ConfigLoader::Instance()->GetCenterID();
    Send(MSG_SERVER_ACTIVE_REQ, &sendMsg);
}

// ==================================================
// 接收服务器激活
// ==================================================
void CenterHandler::RecvServerActiveAck(void* data)
{
    SERVER_ACTIVE_ACK* recvMsg = (SERVER_ACTIVE_ACK*)data;
    if(!recvMsg) return;

    if(recvMsg->errcode != ERR_SUCCEED)
    {
        CnFatal("Active server failed\n");
        CnExit();
    }

    CnInfo("RecvServerActiveAck succeed\n");

    // 设置对应的中心服务器 ID
    CnAssert(ConfigLoader::Instance()->GetCenterID() == 0);
    ConfigLoader::Instance()->SetCenterID(recvMsg->center_id);
}

// ==================================================
// 接受服务器列表
// ==================================================
void CenterHandler::RecvServerListAck(void* data)
{
    SERVER_LIST_ACK* recvMsg = (SERVER_LIST_ACK*)data;
    if(!recvMsg) return;

    for(uint32 i=0; i<recvMsg->list.size(); ++i)
    {
        const SERVER_INFO& svrInfo = recvMsg->list[i];
        CnInfo("RecvServerListAck id=%d %s(%s):%d\n", svrInfo.server_id, svrInfo.server_ip.c_str(), svrInfo.server_outer_ip.c_str(), svrInfo.server_port);
        if(SERVER_TYPE_GAME != recvMsg->list[i].server_type)
            continue;

        // 如果Gate还没有连接过这个Game，则与之链接
        if(!GameHandler::Instance()->GetSessionID(recvMsg->list[i].server_id, recvMsg->list[i].center_id))
        {   // 用外网IP连接Game
            GameHandler::Instance()->Connect(recvMsg->list[i].server_id, recvMsg->list[i].center_id,recvMsg->list[i].server_outer_ip.c_str(), recvMsg->list[i].server_port);
        }
    }
}

// ==================================================
// 所有服务器准备完毕
// ==================================================
void CenterHandler::RecvServerReadyAck( void* data )
{
    CnInfo("ready\n");
}

void CenterHandler::RecvLoadPlayerAck(uint64 loginID, void* data)
{
    ERROR_CODE* pErrorCode = (ERROR_CODE*)data;
    Player* player = PlayerManager::Instance()->GetPlayerByLoginID(loginID);
    if(!player) return;

    CnAssert(player->GetState() == Player::kStateLoadReq);      // 只有请求加载角色，才会返回未检查到角色
    player->SetState(Player::kStateLoginOk);

    Net_Packet* packet = new Net_Packet;
    if(CMessageEncoder::Instance()->Encode(player->GetPlayerID(), player->GetSessionID(), MSG_LOAD_PLAYER_ACK, data, packet))
    {
        delete packet;
        CnAssert(false);
        return;
    }

    player->CompressPacket(packet);
    player->EncryptPacket(packet);

    if(Net_Manager::Instance()->send_packet(player->GetSessionID(), packet))
    {
        delete packet;
        CnAssert(false);
        return;
    }
}

// ==================================================
// 收到登陆游戏错误消息
// ==================================================
void CenterHandler::RecvLoginGameAck(uint64 loginID, void* data)
{
    LOGIN_GAME_ACK* recvMsg = (LOGIN_GAME_ACK*)data;

    Player* player = PlayerManager::Instance()->GetPlayerByLoginID(loginID);
    if(!player) return;

    switch(recvMsg->errcode)
    {
    case ERR_UNFOUND_PLAYER:   // 未检查到角色
        {
            CnAssert(player->GetState() == Player::kStateLoadReq);      // 只有请求加载角色，才会返回未检查到角色
            player->SetState(Player::kStateLoginOk);

            Net_Packet* packet = new Net_Packet;
            if(CMessageEncoder::Instance()->Encode(player->GetPlayerID(), player->GetSessionID(), MSG_LOGIN_GAME_ACK, data, packet))
            {
                delete packet;
                CnAssert(false);
                return;
            }

            player->CompressPacket(packet);
            player->EncryptPacket(packet);

            if(Net_Manager::Instance()->send_packet(player->GetSessionID(), packet))
            {
                delete packet;
                CnAssert(false);
                return;
            }
        }
        break;
    case ERR_SUCCEED:           // 登陆成功
        {
            CnAssert(player->GetState() == Player::kStateLoginReq);
            player->SetState(Player::kStateLoginOk);

            // 发送登陆成功通知
            Net_Packet* packet = new Net_Packet;
            if(CMessageEncoder::Instance()->Encode(player->GetPlayerID(), player->GetSessionID(), MSG_LOGIN_GAME_ACK, data, packet))
            {
                delete packet;
                CnAssert(false);
                return;
            }

            player->CompressPacket(packet);
            player->EncryptPacket(packet);

            if(Net_Manager::Instance()->send_packet(player->GetSessionID(), packet))
            {
                delete packet;
                CnAssert(false);
                return;
            }
        }
        break;
    case ERR_FAILED:            // 登陆失败
    case ERR_REPEAT_LOGIN:      // 角色重复登陆
    case ERR_VERIFY_LOGIN:      // 验证失败
    case ERR_OTHER_LOGIN:
    case ERR_ACCOUNT_INGAME:
    case ERR_SERVER_NO_READY:
    case ERR_PLAYER_NOT_EXIST_LOGIN:
    case ERR_ADD_ACCOUNT_LOGIN:
    case ERR_KUNLUN_EVENT_LOGIN:
    case ERR_LOAD_DB_LOGIN:
    case ERR_GAMESVR_CLOSE:             // GameServer不存在
    case ERR_ACTIVE_NOT_ACTIVE_LOGIN:   // 账号未用激活码激活
    case ERR_BAN_ACCOUNT_LOGIN:         // 账号被封了
    case ERR_VERSION_ERROR:             // 版本号不匹配
    case ERR_LACK_USERNAME:
    case ERR_LACK_QID:
    case ERR_LACK_SERVERID:
    case ERR_LACK_TIME:
    case ERR_LACK_SIGN:
    case ERR_LACK_ISADULT:
    case ERR_LACK_FCM:
    case ERR_MISMATCH_SIGN:
        {
            player->SetState(Player::kStatelogout);
            // 发送登陆失败通知
            Net_Packet* packet = new Net_Packet;
            if(CMessageEncoder::Instance()->Encode(player->GetPlayerID(), player->GetSessionID(), MSG_LOGIN_GAME_ACK, data, packet))
            {
                delete packet;
                CnAssert(false);
                return;
            }

            player->CompressPacket(packet);
            player->EncryptPacket(packet);

            if(Net_Manager::Instance()->send_packet(player->GetSessionID(), packet))
            {
                delete packet;
                CnAssert(false);
                return;
            }

            // 关闭网络
            Net_Manager::Instance()->delete_net(player->GetSessionID());
            // 删除角色
            PlayerManager::Instance()->DelSessionPlayer(player->GetSessionID());
        }
        break;
    default:
        CnAssert(false);
        break;
    }
}

// ==================================================
// 收到踢出游戏消息
// ==================================================
void CenterHandler::RecvKickoutGame(uint64 loginID, void* data)
{
    KICKOUT_GAME* recvMsg = (KICKOUT_GAME*)data;
    if(!recvMsg) return;

    Player* player = PlayerManager::Instance()->GetPlayerByLoginID(loginID);
    if(!player)
    {
        CnDbg("send center loginID %llu logout not in gate\n", loginID);
        CenterHandler::Instance()->SendLogoutGame(loginID);
        return;
    }

    CnAssert(player->GetLoginID() == loginID);

    // 关闭网络
    Net_Manager::Instance()->delete_net(player->GetSessionID());

    // 游戏服务器关闭时，暴力删除Center上的角色数据
    if(ERR_GAMESVR_CLOSE != recvMsg->errcode)
    {
        // 通知中心服务器角色离开
        CnDbg("send center player %llu logout by gameserver close\n", player->GetPlayerID());
        CenterHandler::Instance()->SendLogoutGame(player->GetLoginID());
    }

    // 删除角色
    PlayerManager::Instance()->DelSessionPlayer(player->GetSessionID());
}

// ==================================================
// 请求设置玩家ID
// ==================================================
void CenterHandler::RecvSetPlayerIDReq(void* data)
{
    SET_PLAYER_ID_REQ* recvMsg = (SET_PLAYER_ID_REQ*)data;
    if(!recvMsg) return;

    uint16 errCode = ERR_FAILED;
    uint64 loginID = 0;
    uint64 playerID = 0;

    loginID = U64ID(recvMsg->login_id_h, recvMsg->login_id_l);
    playerID = U64ID(recvMsg->player_id_h, recvMsg->player_id_l);

    Player* player = PlayerManager::Instance()->GetPlayerByLoginID(loginID);
    if(player)
    {
        errCode = ERR_SUCCEED;

        CnAssert(playerID);
        player->SetPlayerID(playerID);

        CnAssert(player->GetState() == Player::kStateLoadReq);
        player->SetState(Player::kStateLoadOk);

        char loginIdStr[30];
        Crown::SDUtoa64<30>(loginIdStr, loginID);
        char playerIdStr[30];
        Crown::SDUtoa64<30>(playerIdStr, playerID);
        CnInfo("player id = %s login\n", playerIdStr);
        SendSetPlayerIDAck(errCode, loginID, playerID);
    }
    else
    {
        CnDbg("send center player %llu logout by no loginID\n", playerID);
        SendLogoutGame(loginID);
    }
}

// ==================================================
// 回复设置玩家ID
// ==================================================
void CenterHandler::SendSetPlayerIDAck(uint16 errCode, uint64 loginID, uint64 playerID)
{
    SET_PLAYER_ID_ACK sendMsg;

    sendMsg.errcode = errCode;
    sendMsg.login_id_l = U64ID_LOW(loginID);
    sendMsg.login_id_h = U64ID_HIGH(loginID);
    Send(MSG_SET_PLAYER_ID_ACK, &sendMsg);
}

// ==================================================
// 请求设置游戏服务器ID
// ==================================================
void CenterHandler::RecvSetGameSvrIDReq(void* data)
{
    SET_GAMESVR_ID_REQ* recvMsg = (SET_GAMESVR_ID_REQ*)data;
    if(!recvMsg) return;

    uint16 errCode = ERR_FAILED;
    uint64 playerID = 0;

    playerID = ((uint64)recvMsg->player_id_h) << 32 | recvMsg->player_id_l;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(player)
    {
        errCode = ERR_SUCCEED;
        CnAssert(recvMsg->gamesvr_id);
        player->SetGameSvrID(recvMsg->gamesvr_id);
        if(player->GetState() != Player::kStateLoadOk && player->GetState() != Player::kStateInGame && player->GetState() != Player::kStateChangeDungeon)
        {
            // 由服务器驱动跳转副本时，player的state=kStateInGame
            CnAssert(false);
            CnError("RecvSetGameSvrIDReq state=%d\n", player->GetState());
        }
    }

    SendSetGameSvrIDAck(errCode, playerID);
}

// ==================================================
// 回复试着游戏服务器ID
// ==================================================
void CenterHandler::SendSetGameSvrIDAck(uint16 errCode, uint64 playerID)
{
    SET_GAMESVR_ID_ACK sendMsg;
    sendMsg.errcode = errCode;
    sendMsg.player_id_l = U64ID_LOW(playerID);
    sendMsg.player_id_h = U64ID_HIGH(playerID);
    Send(MSG_SET_GAMESVR_ID_ACK, &sendMsg);
}

