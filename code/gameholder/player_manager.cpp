#include "gameholder_pch.h"
#include "player_manager.h"
#include "dungeon_manager.h"
#include "scene.h"
#include "gate_handler.h"
#include "center_handler.h"
#include "server_config.h"
#include "team_relationship.h"


IMPLEMENT_SINGLETON(PlayerManager)

PlayerManager::PlayerManager()
{
    m_SecondTimer = 0;
}

PlayerManager::~PlayerManager()
{
    CnAssert(m_players.size() == m_loginIDMap.size());
    CnAssert(m_players.size() == m_objectIDMap.size());

    for(PlayerMap::iterator iter = m_players.begin(); iter != m_players.end(); iter++)
    {
        Player* delPlayer = iter->second;
        if(delPlayer->GetScene())
            delPlayer->GetScene()->DelPlayer(delPlayer->GetId());
        CnDelete iter->second;
    }
}

Player* PlayerManager::GetPlayerByObjID(uint32 objectID)
{
    std::map<uint32, Player*>::iterator it;
    if((it = m_objectIDMap.find(objectID)) == m_objectIDMap.end())
    {
        return NULL;
    }
    return it->second;
}

Player* PlayerManager::GetPlayerByPlayerID(uint64 playerID)
{
    std::map<uint64, Player*>::iterator it;
    if((it = m_players.find(playerID)) == m_players.end())
    {
        return NULL;
    }
    return it->second;
}

Player* PlayerManager::GetPlayerByLoginID(uint64 loginID)
{
    std::map<uint64, Player*>::iterator it;
    if((it = m_loginIDMap.find(loginID)) == m_loginIDMap.end())
    {
        return NULL;
    }
    return it->second;
}

Player* PlayerManager::GetPlayerByName(const std::string& name)
{
    std::map<uint64, Player*>::iterator it;

    for(it = m_players.begin(); it != m_players.end(); ++it)
    {
        Player* player = it->second;
        if(player && (player->GetName() == name) && (player->GetNetState() == Player::kStateInGame))        // 下线流程中的玩家排除掉
        {
            return it->second;
        }
    }

    return NULL;
}

uint32 PlayerManager::GetPlayerNumInGame()
{
    uint32 playerNum = 0;
    for(PlayerMap::iterator iter = m_players.begin(); iter != m_players.end(); iter++)
    {
        Player* player = iter->second;
        if(player->GetNetState() == Player::kStateInGame || player->GetNetState() == Player::kStateJoinTeam)
            playerNum++;
    }
    return playerNum;
}

uint32 PlayerManager::GetPlayerNumByCenterID(uint32 centerId)
{
    uint32 playerNum = 0;
    for(PlayerMap::iterator iter = m_players.begin(); iter != m_players.end(); iter++)
    {
        Player* player = iter->second;
        if(player->GetCenterSvrID() == centerId)
            playerNum++;
    }
    return playerNum;
}

bool PlayerManager::AddPlayer(Player* player)
{
    if(m_players.find(player->GetPlayerID()) != m_players.end())
    {
        CnFatal("add same player id %llu\n", player->GetPlayerID());
        CnAssert(false);
        return false;
    }

    CnAssert(m_players.find(player->GetPlayerID()) == m_players.end());
    m_players[player->GetPlayerID()] = player;
    CnAssert(m_loginIDMap.find(player->GetLoginID()) == m_loginIDMap.end());
    m_loginIDMap[player->GetLoginID()] = player;
    CnAssert(m_objectIDMap.find(player->GetId()) == m_objectIDMap.end());
    m_objectIDMap[player->GetId()] = player;

    CnAssert(m_players.size() == m_loginIDMap.size());
    CnAssert(m_players.size() == m_objectIDMap.size());

    return true;
}

void PlayerManager::DelPlayer(uint64 playerID)
{
    std::map<uint64, Player*>::iterator it;
    if((it = m_players.find(playerID)) == m_players.end())
    {
        CnAssert(false);    // 删除一个不存在的玩家
        return;
    }

    Player* delPlayer = it->second;

    CnAssert(m_loginIDMap.find(delPlayer->GetLoginID()) != m_loginIDMap.end());
    m_loginIDMap.erase(delPlayer->GetLoginID());
    CnAssert(m_objectIDMap.find(delPlayer->GetId()) != m_objectIDMap.end());
    m_objectIDMap.erase(delPlayer->GetId());
    CnAssert(m_players.find(playerID) != m_players.end());

    m_players.erase(playerID);

    CnAssert(m_players.size() == m_loginIDMap.size());
    CnAssert(m_players.size() == m_objectIDMap.size());

    TeamRelationShip::Instance()->OnDelPlayer(delPlayer);

    CnDelete delPlayer;
}

void PlayerManager::OnCloseCenterServer(uint32 centerID)
{
    std::list<uint64> list;
    Scene* pScene = 0;

    for(std::map<uint64, Player*>::iterator it=m_players.begin(); it != m_players.end(); ++it)
    {
        Player* player = it->second;

        if(player->GetCenterSvrID() != centerID)
        {
            continue;
        }

        pScene = DungeonManager::Instance()->GetScene(player->GetUnionMapID());
        if(pScene && player->GetScene())
        {
            pScene->DelPlayer(player->GetId());
        }

        list.push_back(it->first);
        CnWarn("del player %s on close center\n", player->GetName().c_str());       // 在此删除玩家信息,应该是center主动断开了,玩家信息未正常回存
    }

    for(std::list<uint64>::iterator it=list.begin(); it != list.end(); ++it)
    {
        DelPlayer(*it);
    }
}

void PlayerManager::ReloadTask()
{
    for(std::map<uint64, Player*>::iterator it = m_players.begin(); it != m_players.end(); ++it)
    {
        if (it->second->GetNetState() == Player::kStateInGame)
        {
            it->second->ReloadTask();
        }
    }
}

void PlayerManager::SendToAll(uint32 msgId, void* msg)
{
    for(std::map<uint64, Player*>::iterator it = m_players.begin(); it != m_players.end(); ++it)
    {
        if (it->second->GetNetState() == Player::kStateInGame)
        {
            it->second->SendToGate(msgId, msg);
        }
    }
}

void PlayerManager::AddMailToAll(MAIL_INFO* mail)
{
    std::map<uint64, Player*>::iterator it;

    if(!mail) return;

    for(it = m_players.begin(); it != m_players.end(); ++it)
    {
        if(it->second->GetNetState() == Player::kStateInGame)
        {
            it->second->AddMail(mail);
        }
    }
}

// 删除过期邮件
void PlayerManager::DelExpiredMail(uint32 mail_id_h)
{
    std::map<uint64, Player*>::iterator it;

    for(it = m_players.begin(); it != m_players.end(); ++it)
    {
        if(it->second->GetNetState() == Player::kStateInGame)
        {
            it->second->DelExpiredMail(mail_id_h);
        }
    }
}

void PlayerManager::KickOutPlayer(uint16 errCode, uint64 playerID)
{
    Player* player = GetPlayerByPlayerID(playerID);
    if(!player)
    {
        CnAssert(false);
        return;
    }

    KICKOUT_GAME kickout;
    kickout.errcode = errCode;

    CenterHandler::Instance()->SendByPlayer(player, MSG_GS_KICKOUT_GAME, &kickout, playerID);
}

void PlayerManager::KickOutPlayerByCenterID(uint16 errCode, uint32 CenterID)
{
    for(std::map<uint64, Player*>::iterator it = m_players.begin(); it != m_players.end(); ++it)
    {
        Player* player = it->second;
        if(player->GetCenterSvrID() != CenterID)
            continue;

        KICKOUT_GAME kickout;
        kickout.errcode = errCode;

        CenterHandler::Instance()->SendByPlayer(player, MSG_GS_KICKOUT_GAME, &kickout, it->first);
    }
}

void PlayerManager::Update()
{
    m_SecondTimer += g_ServerUpdateTime;
    if(m_SecondTimer < 1000)
        return;
    m_SecondTimer = 0;

    // 每秒一次，自动回存检测
    uint32 AutoSavePeriod = ServerConfig::Instance()->GetAutoSavePeriod() * 1000;
    for(PlayerMap::iterator iter = m_players.begin(); iter != m_players.end(); iter++)
    {
        Player* player = iter->second;

        if(player->GetNetState() != Player::kStateInGame)
            continue;

        if(player->m_auto_save_time >= AutoSavePeriod)
        {
            player->m_auto_save_time = 0;
            // 该玩家数据回存CenterServer
            CenterHandler::Instance()->AutoSavePlayerInfoReq(player);
        }
    }
}

void PlayerManager::SyncFrame(uint32 frameCounter)
{
    SYNC_FRAME_NTF syncNtf;
    syncNtf.frame_count = frameCounter;

    for(PlayerMap::iterator iter = m_players.begin(); iter != m_players.end(); iter++)
    {
        Player* player = iter->second;

        if(player->GetNetState() != Player::kStateInGame)
            continue;

        player->SendToGate(MSG_SYNC_FRAME_NTF, &syncNtf);
    }
    //CnDbg("frame %d\n", frameCounter);
}

void PlayerManager::GmtUpdate( uint32 type )
{
    std::map<uint64, Player*>::iterator it;

    for(it = m_players.begin(); it != m_players.end(); ++it)
    {
        if(it->second->GetNetState() == Player::kStateInGame)
        {
            it->second->GmtUpdate(type);
        }
    }
}

