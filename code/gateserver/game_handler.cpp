#include "gateserver_pch.h"
#include "game_handler.h"
#include "config_loader.h"
#include "net_manager.h"
#include "player_manager.h"
#include "player.h"

IMPLEMENT_SINGLETON(GameHandler)

GameHandler::GameHandler()
{
}

GameHandler::~GameHandler()
{
    for(GameServerInfoMap::iterator iter = m_GameServerInfos.begin(); iter != m_GameServerInfos.end(); iter++)
    {
        CnDelete iter->second;
    }
}

void GameHandler::Update()
{
    uint32 now = Crown::SDGetTickCount();
    for(GameServerInfoMap::iterator iter = m_GameServerInfos.begin(); iter != m_GameServerInfos.end(); iter++)
    {
        GameServerInfo* pInfo = iter->second;
        if(pInfo->needReconnect && (now - pInfo->closedtime) > 3000 )
        {
            Reconnect(pInfo);
        }
    }
}

bool GameHandler::Connect(uint32 serverID,uint32 centerID, const char* ip, uint32 port)
{
    CnInfo("connecting gameserver id=%d %s:%d\n", serverID, ip, port);

    CnAssert(m_GameServerInfos.find(serverID) == m_GameServerInfos.end());


    GameServerInfo* pInfo = CnNew GameServerInfo;
    pInfo->serverID = serverID;
    pInfo->ip = ip;
    pInfo->port = port;
    pInfo->closedtime = 0;
    pInfo->centerID = centerID;

    Reconnect(pInfo);

    return true;
}

void GameHandler::Reconnect(GameServerInfo* pInfo)
{
    pInfo->sessionID = Net_Manager::Instance()->create_tcp_client(pInfo->ip.c_str(), pInfo->port, &m_splitterFactory, (void*)(&RecvNetPacket), 10, true);
    pInfo->state = NET_STATE_RECONNECT;
    pInfo->needReconnect = false;

    CnAssert(!m_GameServerInfos[pInfo->sessionID]);
    m_GameServerInfos[pInfo->sessionID] = pInfo;
}

uint32 GameHandler::GetSessionID(uint32 gameSvrID,uint32 centerID)
{
    for(GameServerInfoMap::iterator it = m_GameServerInfos.begin(); it != m_GameServerInfos.end(); ++it)
    {
        if(gameSvrID == it->second->serverID && centerID == it->second->centerID)
        {
            return it->second->sessionID;
        }
    }
    return 0;
}

void GameHandler::RecvNetPacket(uint32 eventType, uint32 sessionID, Net_Packet* packet, bool& delpacket)
{
    switch(eventType)
    {
    case TYPE_CONNECT:
        GameHandler::Instance()->OnConnect(sessionID);
        break;
    case TYPE_CLOSE:
        GameHandler::Instance()->OnClose(sessionID);
        break;
    case TYPE_EXCEPTION:
        GameHandler::Instance()->OnError(sessionID);
        break;
    default:
        GameHandler::Instance()->RecvMsg(sessionID, packet, delpacket);
        break;
    }
}

void GameHandler::RecvMsg(uint32 sessionID, Net_Packet* packet, bool& delpacket)
{
    packet->read_uint16();
    uint16 timeStamp = packet->read_uint16();
    uint32 dataLen = packet->read_uint32();
    uint64 playerID = packet->read_uint64();
    uint32 msgID = packet->read_uint32();

    switch(msgID)
    {
    case MSG_SERVER_GATEID_REQ:
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
                Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
                if(!player)
                {
                    return;
                }

                if(MSG_CHANGE_DUNGEON_ACK == msgID)
                {
                    if(Player::kStateChangeDungeon == player->GetState())
                    {
                        player->SetState(Player::kStateInGame);
                    }
                }
                else if(MSG_CHANGE_PLAYER_NAME_ACK == msgID)
                {
                    if(Player::kStateChangePlayerName == player->GetState())
                    {
                        player->SetState(Player::kStateInGame);
                    }
                }
                else if(msgID == MSG_PLAYER_INFO)
                {
                    CnAssert(player->GetState() == Player::kStateLoadOk);
                    CnInfo("player id = %llu send info\n", player->GetPlayerID());
                    player->SetState(Player::kStateInGame);         // 通知客户端PlayerInfo,才算是正式进入游戏
                }

                packet->movetobegin();
                Net_Packet* sendPacket = new Net_Packet;
                sendPacket->cloneData(packet);

                // 加密
                if(player->GetState() >= Player::kStateSetPrivateKey)
                {
                    player->CompressPacket(sendPacket);     // 先压缩
                    player->EncryptPacket(sendPacket);      // 再加密
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

bool GameHandler::Send(uint32 sessionID, uint32 msgID, void* data, uint64 playerID)
{
    if(TransferSend(sessionID, msgID, data, playerID))
    {
        return false;
    }

    return true;
}

void GameHandler::OnConnect(uint32 sessionID)
{
    m_GameServerInfos[sessionID]->state = NET_STATE_CONNECTED;
    CnInfo("connected to gameserver id=%d %s:%d\n", m_GameServerInfos[sessionID]->serverID, m_GameServerInfos[sessionID]->ip.c_str(), m_GameServerInfos[sessionID]->port);
}

void GameHandler::OnClose(uint32 sessionID)
{
    OnDisconnectGame(sessionID);
}

void GameHandler::OnError(uint32 sessionID)
{
    OnDisconnectGame(sessionID);
}

void GameHandler::OnDisconnectGame(uint32 sessionID)
{
    GameServerInfo* pServerInfo = m_GameServerInfos[sessionID];
    CnAssert(pServerInfo);

    uint32 playerInGameNum = PlayerManager::Instance()->GetPlayerNumByGameServerIDAndState(pServerInfo->serverID, Player::kStateInGame);
    CnAssert(playerInGameNum == 0);         // Game安全退出,在游戏中的玩家数=0

    pServerInfo->state = NET_STATE_CLOSED;
    pServerInfo->needReconnect = true;
    pServerInfo->closedtime = Crown::SDGetTickCount();

    CnInfo("disconnect gameserver id=%d\n", pServerInfo->serverID);

    m_GameServerInfos.erase(sessionID);

    CnDelete pServerInfo;
}

void GameHandler::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata)
{
    switch(msgID)
    {
    case MSG_SERVER_GATEID_REQ:
        Instance()->RecvServerGateIDReq(sessionID, data);
        break;
    default:
        break;
    }
}

void GameHandler::RecvServerGateIDReq(uint32 sessionID, void* data)
{
    NONE* recvMsg = (NONE*)data;
    if(!recvMsg) return;

    SendServerGateIDAck(sessionID);
}

void GameHandler::SendServerGateIDAck(uint32 sessionID)
{
    SERVER_GATEID_ACK sendMsg;

    sendMsg.center_id = ConfigLoader::Instance()->GetCenterID();
    sendMsg.gate_id = ConfigLoader::Instance()->GetMyID();

    TransferSend(sessionID, MSG_SERVER_GATEID_ACK, &sendMsg);
}


