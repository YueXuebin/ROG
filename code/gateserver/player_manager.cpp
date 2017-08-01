#include "gateserver_pch.h"
#include "player_manager.h"
#include "gateserver.h"
#include "net_manager.h"
#include "player.h"

IMPLEMENT_SINGLETON(PlayerManager)

// ==================================================
// 构造
// ==================================================
PlayerManager::PlayerManager()
{

}

// ==================================================
// 析构
// ==================================================
PlayerManager::~PlayerManager()
{
    for(std::map<uint32, Player*>::iterator iter = m_players.begin(); iter != m_players.end(); iter++)
    {
        CnDelete iter->second;
    }
}

void PlayerManager::Update()
{
    for(std::map<uint32, Player*>::iterator iter = m_players.begin(); iter != m_players.end(); iter++)
    {
        iter->second->Update();
    }


    static int32 frameCount = 0;
    frameCount++;

#ifdef WIN
    const uint32 timer = g_ServerUpdateFrequency * 5;
#else
    const uint32 timer = g_ServerUpdateFrequency * 5 * 60;
#endif
    if(frameCount < timer)
        return;
    frameCount = 0;

    CnAssert(m_players.size() == m_loginIDMap.size());
    CnDbg("playernum = %d\n", m_players.size());
}

// ==================================================
// 添加角色
// ==================================================
bool PlayerManager::AddSessionPlayer(uint32 sessionID, uint32 addrIP, uint16 addrPort)
{
    Player* player = 0;
    uint32 publicKey = (uint32)rand() | ((uint32)rand() << 16);

    if(m_loginIDMap.find(sessionID) != m_loginIDMap.end())
    {
        return false;
    }

    player = CnNew Player();
    player->Clear();

    // 添加玩家
    player->SetState(Player::kStateInit);
    player->SetEncryptPublicKey(publicKey);
    player->SetSessionID(sessionID);
    player->SetLoginID(GateServer::Instance()->GenLoginUUID());
    player->SetLoginTime((uint32)time(0));
    player->SetAddrIP(addrIP);
    player->SetAddrPort(addrPort);

    m_players[sessionID] = player;
    m_loginIDMap[player->GetLoginID()] = m_players[sessionID];

    CnInfo("add session player %d ip=%s port=%d\n", m_loginIDMap.size(), Crown::GetIpStr(addrIP).c_str(), addrPort);

    return true;
}

// ==================================================
// 删除角色
// ==================================================
void PlayerManager::DelSessionPlayer(uint32 sessionID)
{
    std::map<uint32, Player*>::iterator it = m_players.find(sessionID);
    if(it == m_players.end())
    {
        return;
    }

    m_loginIDMap.erase(it->second->GetLoginID());
    CnDelete it->second;
    it->second = NULL;
    m_players.erase(it);
}

uint32 PlayerManager::GetPlayerNumByGameServerIDAndState(uint32 gameServerID, uint32 state)
{
    uint32 count = 0;
    for(std::map<uint32, Player*>::iterator iter = m_players.begin(); iter != m_players.end(); iter++)
    {
        Player* player = iter->second;
        if(player->GetGameSvrID() == gameServerID && player->GetState() == state)
            count++;
    }
    return count;
}

// ==================================================
// 获得玩家的登陆ID
// ==================================================
uint64 PlayerManager::GetLoginID(uint32 sessionID)
{
    std::map<uint32, Player*>::iterator it = m_players.find(sessionID);
    if(it != m_players.end())
    {
        return it->second->GetLoginID();
    }
    return 0;
}

// ==================================================
// 获得玩家的游戏ID
// ==================================================
uint64 PlayerManager::GetPlayerID(uint32 sessionID)
{
    std::map<uint32, Player*>::iterator it = m_players.find(sessionID);
    if(it != m_players.end())
    {
        return it->second->GetPlayerID();
    }
    return 0;
}


// ==================================================
// 通过登陆ID获得会话ID
// ==================================================
uint32 PlayerManager::GetSnIDByLoginID(uint64 loginID)
{
    for(std::map<uint32, Player*>::iterator it = m_players.begin(); it != m_players.end(); ++it)
    {
        if(it->second->GetLoginID() == loginID)
        {
            return it->second->GetSessionID();
        }
    }
    return 0;
}

// ==================================================
// 通过角色ID获得会话ID
// ==================================================
uint32 PlayerManager::GetSnIDByPlayerID(uint64 playerID)
{
    for(std::map<uint32, Player*>::iterator it = m_players.begin(); it != m_players.end(); ++it)
    {
        if(it->second->GetPlayerID() == playerID)
        {
            return it->second->GetSessionID();
        }
    }
    return 0;
}

// ==================================================
// 获得玩家的游戏状态
// ==================================================
uint32 PlayerManager::GetState(uint32 sessionID)
{
    std::map<uint32, Player*>::iterator it = m_players.find(sessionID);
    if(it != m_players.end())
    {
        return it->second->GetState();
    }
    return 0;
}

// ==================================================
// 获得玩家
// ==================================================
Player* PlayerManager::GetPlayer(uint32 sessionID)
{
    std::map<uint32, Player*>::iterator it = m_players.find(sessionID);
    if(it != m_players.end())
    {
        return it->second;
    }
    return 0;
}

// ==================================================
// 获得玩家
// ==================================================
Player* PlayerManager::GetPlayerByLoginID(uint64 loginID)
{
    std::map<uint64, Player*>::iterator it = m_loginIDMap.find(loginID);
    if(it != m_loginIDMap.end())
    {
        return it->second;
    }
    return 0;
}

// ==================================================
// 获得玩家
// ==================================================
Player* PlayerManager::GetPlayerByPlayerID(uint64 playerID)
{
    // TEMP !!!
    for(std::map<uint32, Player*>::iterator it = m_players.begin(); it != m_players.end(); ++it)
    {
        if(playerID != it->second->GetPlayerID()) continue;
        return it->second;
    }
    return 0;
}

// ==================================================
// 关闭中心服务器
// ==================================================
void PlayerManager::CloseCenterServer()
{
    std::list<uint32> list;

    for(std::map<uint32, Player*>::iterator it=m_players.begin(); it != m_players.end(); ++it)
    {
        Net_Manager::Instance()->delete_net(it->first);
        list.push_back(it->first);
    }

    for(std::list<uint32>::iterator it=list.begin(); it != list.end(); ++it)
    {
        DelSessionPlayer(*it);
    }
}

// ==================================================
// 发给所有的好友
// ==================================================
void PlayerManager::SendToAll(Net_Packet* packet)
{
    Net_Packet* sendPacket = 0;
    Encrypt* encrypt = 0;

    for(std::map<uint32, Player*>::iterator playerIt = m_players.begin(); playerIt != m_players.end(); ++playerIt)
    {
        Player* player = playerIt->second;
        if(player->GetState() != Player::kStateInGame) 
            continue;

        sendPacket = new Net_Packet;
        sendPacket->cloneData(packet);

        
        player->CompressPacket(sendPacket);
        // 加密
        player->EncryptPacket(sendPacket);

        player->OnSendMessage(0);

        if(Net_Manager::Instance()->send_packet(player->GetSessionID(), sendPacket) != 0)
        {
            delete sendPacket;
        }
    }
}

void PlayerManager::SendToAll(uint32 msgID, void* data)
{
    Net_Packet* sendPacket = 0;
    Encrypt* encrypt = 0;

    for(std::map<uint32, Player*>::iterator playerIt = m_players.begin(); playerIt != m_players.end(); ++playerIt)
    {
        Player* player = playerIt->second;
        if(player->GetState() != Player::kStateInGame) 
            continue;

        sendPacket = new Net_Packet;
    
        if(CMessageEncoder::Instance()->Encode(player->GetPlayerID(), player->GetSessionID(), msgID, data, sendPacket))
        {
            delete sendPacket;
            continue;
        }

        player->CompressPacket(sendPacket);
        // 加密
        player->EncryptPacket(sendPacket);

        player->OnSendMessage(0);

        if(Net_Manager::Instance()->send_packet(player->GetSessionID(), sendPacket) != 0)
        {
            delete sendPacket;
        }
    }

}


