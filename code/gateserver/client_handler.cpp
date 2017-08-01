#include "gateserver_pch.h"
#include "client_handler.h"
#include "net_manager.h"
#include "game_handler.h"
#include "center_handler.h"
#include "player_manager.h"
#include "player.h"

IMPLEMENT_SINGLETON(ClientHandler)

ClientHandler::ClientHandler()
{
    m_serverID = 0;
}

ClientHandler::~ClientHandler()
{

}

bool ClientHandler::Init(const char* ip, uint32 port)
{
    m_serverID = Net_Manager::Instance()->create_tcp_server(ip, port, &m_splitterFactory, (void*)&RecvNetPacket, false);
    return true;
}

void ClientHandler::RecvNetPacket(uint32 eventType, uint32 sessionID, Net_Packet* packet, bool& delpacket)
{
    switch(eventType)
    {
    case TYPE_ACCEPT:
        ClientHandler::Instance()->OnAccept(sessionID, packet->remote_addr.get_addr(), packet->remote_addr.get_port());
        break;
    case TYPE_CLOSE:
        ClientHandler::Instance()->OnClose(sessionID, packet->remote_addr.get_addr(), packet->remote_addr.get_port());
        break;
    case TYPE_EXCEPTION:
        ClientHandler::Instance()->OnError(sessionID, packet->remote_addr.get_addr(), packet->remote_addr.get_port());
        break;
    default:
        RecvMsg(sessionID, packet, delpacket);
        break;
    }
}

void ClientHandler::RecvMsg(uint32 sessionID, Net_Packet* packet, bool& delpacket)
{
    Player* player = 0;
    uint16 markcode = packet->read_uint16();        // DG
    uint8 timeStamp = packet->read_uint8();         // 时间戳
    uint8 counter = packet->read_uint8();           // 计数器
    uint32 dataLen = packet->read_uint32();         // 数据包长(整包)
    Bookmark bmPlayerID = packet->getcurpos();
    uint64 playerId = packet->read_uint64();        // PlayerID
    Bookmark bmMsgID = packet->getcurpos();
    uint32 msgId = packet->read_uint32();           // msgID

    player = PlayerManager::Instance()->GetPlayer(sessionID);
    if(!player)
        return;

    //等于0时不检查计数
    if (!ClientHandler::Instance()->CheckMsgCount(sessionID, counter))
    {
        CnWarn("player %llu message %d counter error\n", player->GetPlayerID(), msgId);
        return;
    }

    switch(msgId)
    {
    case MSG_ENCRYPT_CONNECT_REQ:
        {
            // 检查状态
            if(player->GetState() > Player::kStateInit)
                return;

            // 发送加密Token
            player->SendEncryptPublicKeyNtf();
        }
        break;
    case MSG_ENCRYPT_PRIVATE_KEY_NTF:
        {
            // 检查状态
            if(player->GetState() > Player::kStateInit)
                return;

            DeFuncCMessageEncoder pfnDecode = CMessageEncoder::Instance()->FindDecodeFunc(msgId);
            if(NULL == pfnDecode)
                return;
            pfnDecode(0, sessionID, packet, (void*)(&OnRecv), USERDATA_TYPE_FUNC);
        }
        break;
    case MSG_LOGIN_GAME_REQ:
        {
            // 只有在初始状态的玩家才会发登陆请求消息
            if(player->GetState() != Player::kStateSetPrivateKey)
            {
                CnError("Client login game failed\n");
                return;
            }

            player->DecryptPacket(packet);

            DeFuncCMessageEncoder pfnDecode = CMessageEncoder::Instance()->FindDecodeFunc(msgId);
            if(NULL == pfnDecode)
                return;
            pfnDecode(playerId, sessionID, packet, (void*)(&OnRecv), USERDATA_TYPE_FUNC);
        }
        break;
    case MSG_IS_CREATE_PLAYER_NAME_REQ:
    case MSG_CREATE_PLAYER_REQ:
    case MSG_ASK_PLAYER_REQ:
        {
            if(player->GetState() != Player::kStateLoginOk)
            {
                if(msgId == MSG_IS_CREATE_PLAYER_NAME_REQ)
                    CnError("Client ask player name failed\n");
                else if(msgId == MSG_CREATE_PLAYER_REQ)
                    CnError("Client create player failed\n");
                else if(msgId == MSG_ASK_PLAYER_REQ)
                    CnError("Client ask player info failed\n");
                return;
            }

            // 解码
            player->DecryptPacket(packet);

            uint64 loginID = PlayerManager::Instance()->GetLoginID(sessionID);

            packet->write_uint64(loginID, bmPlayerID);
            packet->movetobegin();

            Net_Packet* sendPacket = new Net_Packet;
            sendPacket->cloneData(packet);

            CenterHandler::Instance()->Send(sendPacket);
        }
        break;
    case MSG_LOAD_PLAYER_REQ:
        {
            // 只有登陆成功后的玩家才能够，请求加载角色
            if(player->GetState() != Player::kStateLoginOk)
            {                
                CnError("Client load player failed\n");
                return;
            }

            player->DecryptPacket(packet);

            DeFuncCMessageEncoder pfnDecode = CMessageEncoder::Instance()->FindDecodeFunc(msgId);
            if(NULL == pfnDecode)
                return;
            pfnDecode(playerId, sessionID, packet, (void*)(&OnRecv), USERDATA_TYPE_FUNC);
        }
        break;
    case MSG_ECHO_REQ:
        {
            // 玩家进入游戏后，才响应ping
            if(player->GetState() != Player::kStateInGame)
                return;

            player->DecryptPacket(packet);      // 解密

            DeFuncCMessageEncoder pfnDecode = CMessageEncoder::Instance()->FindDecodeFunc(msgId);
            if(NULL == pfnDecode)
                return;
            pfnDecode(0, sessionID, packet, (void*)(&OnRecv), USERDATA_TYPE_FUNC);

            packet->write_uint32(MSG_ECHO_ACK, bmMsgID);        // 修改msgID，ECHO的reg,ack数据结构完全一致
            packet->movetobegin();

            Net_Packet* sendPacket = new Net_Packet;
            sendPacket->cloneData(packet);

            if (!player->CompressPacket(sendPacket))
                return;

            player->EncryptPacket(sendPacket);
            Net_Manager::Instance()->send_packet(sessionID ,sendPacket);
        }
        break;
    default:
        {
            if(player->GetState() != Player::kStateInGame)
            {
                // TODO 返回错误消息提示
                CnDbg("player %llu not in kStateInGame msg=%d\n", player->GetPlayerID(), msgId);
                return;
            }

            // 解码
            player->DecryptPacket(packet);

            if(MSG_CHANGE_DUNGEON_REQ == msgId)
            {
                player->SetState(Player::kStateChangeDungeon);
            }
            else if(MSG_CHANGE_PLAYER_NAME_REQ == msgId)
            {
                player->SetState(Player::kStateChangePlayerName);
            }

            // 注意，客户端发来的playerID是不可信的，也是无用的，需要在Gate上进行复写
            uint64 playerID = player->GetPlayerID();
            packet->write_uint64(playerID, bmPlayerID);

            Net_Packet* sendPacket = new Net_Packet;
            sendPacket->cloneData(packet);

            uint32 gameSNID = GameHandler::Instance()->GetSessionID(CalGameIDByRegionID(player->GetGameSvrID()),CalCenterIDByRegionID(player->GetGameSvrID()));
            if(!gameSNID)
            {
                CnDbg("player %llu no session msgId=%d\n", player->GetPlayerID(), msgId);
                delete sendPacket;
                return;
            }

            player->OnRecvMessage(msgId);

            if(Net_Manager::Instance()->send_packet(gameSNID, sendPacket) != 0)
            {
                delete sendPacket;
            }
        }
        break;
    }
}

void ClientHandler::OnAccept(uint32 sessionID, uint32 addrIP, uint16 addrPort)
{
    // 超过连接数上限了
    if(PlayerManager::Instance()->GetPlayerNum() >= 1500)
    {
        CnDbg("exceed max player num\n");
        Net_Manager::Instance()->delete_net(sessionID);
        return;
    }

    // 如果添加出错，则关闭网络
    if(!PlayerManager::Instance()->AddSessionPlayer(sessionID, addrIP, addrPort))
    {
        CnDbg("add session player error\n");
        Net_Manager::Instance()->delete_net(sessionID);
        return;
    }
}

void ClientHandler::OnClose(uint32 sessionID, uint32 addrIP, uint16 addrPort)
{
    Player* player = PlayerManager::Instance()->GetPlayer(sessionID);
    if(!player) return;
    CnDbg("send center player %llu logout by client close\n", player->GetPlayerID());
    CenterHandler::Instance()->SendLogoutGame(player->GetLoginID());
    PlayerManager::Instance()->DelSessionPlayer(sessionID);
    ClientHandler::Instance()->RemoveCount(sessionID);
}

void ClientHandler::OnError(uint32 sessionID, uint32 addrIP, uint16 addrPort)
{
    Player* player = PlayerManager::Instance()->GetPlayer(sessionID);
    if(!player) return;
    CnDbg("send center player %llu logout by client error\n", player->GetPlayerID());
    CenterHandler::Instance()->SendLogoutGame(player->GetLoginID());
    PlayerManager::Instance()->DelSessionPlayer(sessionID);
    ClientHandler::Instance()->RemoveCount(sessionID);
}

void ClientHandler::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata)
{
    switch(msgID)
    {
    case MSG_LOGIN_GAME_REQ:
        {
            ClientHandler::Instance()->RecvLoginGameReq(sessionID, data);
        }
        break;
    case MSG_LOAD_PLAYER_REQ:
        {
            ClientHandler::Instance()->RecvLoadPlayerReq(sessionID, data);
        }
        break;
    case MSG_ECHO_REQ:
        {
            TEST_ECHO* recvMsg = (TEST_ECHO*)data;
            //CnInfo("echo time:%d\n", recvMsg->time);
        }
        break;
    case MSG_ENCRYPT_PRIVATE_KEY_NTF:
        {
            ClientHandler::Instance()->RecvEncryptPrivateKeyNtf(sessionID, data);
        }
        break;
    default:
        break;
    }
}

void ClientHandler::RecvLoadPlayerReq(uint32 sessionID, void* data)
{
    LOAD_PLAYER_REQ* recvMsg = (LOAD_PLAYER_REQ*)data;
    if(!recvMsg) return;

    Player* player = PlayerManager::Instance()->GetPlayer(sessionID);
    if(!player || (Player::kStateLoginOk != player->GetState()))
    {   // 状态不对
        return;
    }
    
    CnAssert(player->GetState() == Player::kStateLoginOk);
    player->SetState(Player::kStateLoadReq);

    CenterHandler::Instance()->Send(MSG_LOAD_PLAYER_REQ, recvMsg, player->GetLoginID());
}

void ClientHandler::RecvLoginGameReq(uint32 sessionID, void* data)
{
    LOGIN_GAME_REQ* recvMsg = (LOGIN_GAME_REQ*)data;
    if(!recvMsg) return;

    Player* player = PlayerManager::Instance()->GetPlayer(sessionID);
    if(!player || (Player::kStateSetPrivateKey != player->GetState()))
    {   // 状态不对
        return;
    }

    CnAssert(player->GetState() == Player::kStateSetPrivateKey);
    player->SetState(Player::kStateLoginReq);

    recvMsg->addr_ip = player->GetAddrIP();         // 客户端IP
    recvMsg->addr_port = player->GetAddrPort();     // 客户端端口号
    if(recvMsg->net_compress)
        player->EnableCompress();                   // 给该客户端，发送压缩网络包

    CenterHandler::Instance()->Send(MSG_LOGIN_GAME_REQ, recvMsg, player->GetLoginID());
}

void ClientHandler::RecvEncryptPrivateKeyNtf(uint32 sessionID, void* data)
{
    ENCRYPT_TOKEN* recvMsg = (ENCRYPT_TOKEN*)data;
    uint32 publicKey = 0;

    if(!recvMsg) return;

    Player* player = PlayerManager::Instance()->GetPlayer(sessionID);
    if(!player || (Player::kStateInit != player->GetState()))
    {
        return;
    }

    publicKey = player->GetEncryptPublicKey();
    recvMsg->key = publicKey ^ recvMsg->key;

    CnAssert(player->GetState() == Player::kStateInit);
    player->SetState(Player::kStateSetPrivateKey);

    player->SetEncryptPrivateKey(recvMsg->key);
}

bool ClientHandler::CheckMsgCount(uint32 sessionId, uint8 count)
{
    // 只取低7位
    uint8 newCount = (m_MsgCounterMap[sessionId] + 1) & 0x7F;
    count = count & 0x7F;

    if (newCount != count && count != 0) 
        return false;
    m_MsgCounterMap[sessionId] = newCount;
    return true;
}

void ClientHandler::RemoveCount(uint32 sessionId)
{
    m_MsgCounterMap.erase(sessionId);
}