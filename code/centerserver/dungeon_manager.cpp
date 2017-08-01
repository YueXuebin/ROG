#include "centerserver_pch.h"
#include "dungeon_manager.h"
#include "config_reader.h"
#include "game_handler.h"
#include "player.h"
#include "guild_manager.h"
#include "channel_manager.h"
#include "config_loader.h"
#include "dongyi_log.h"

IMPLEMENT_SINGLETON(DungeonManager)

DungeonManager::DungeonManager()
{
}

DungeonManager::~DungeonManager()
{

}

bool DungeonManager::Init()
{
    if(!LoadDungeonConfig())
    {
        return false;
    }

    return true;
}

bool DungeonManager::LoadDungeonConfig()
{
    DungeonTmpl dungeonTmpl;

    // 加载副本配置文件
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", ConfigLoader::Instance()->GetSdbPath().c_str(), DUNGEON_CONFIG_FILE);

    TiXmlDocument xmlDoc = TiXmlDocument(file_path);
    if(!xmlDoc.LoadFile())
    {
        CRITICAL("dungeon config failed filename=%s\n", file_path);
        return false;
    }

    TiXmlElement* rootXml = xmlDoc.RootElement();
    if(!rootXml)
    {
        return false;
    }

    // 副本数据
    TiXmlElement* dungeonsXml = rootXml->FirstChild("dungeons")->ToElement();
    if(!dungeonsXml)
    {
        return false;
    }

    for(TiXmlElement* xmlEle = dungeonsXml->FirstChild("dungeon")->ToElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        dungeonTmpl.Clear();

        ConfigReader::Read(xmlEle, "id", dungeonTmpl.m_dungeon_id);
        ConfigReader::Read(xmlEle, "type", dungeonTmpl.m_dungeon_type);

        CnAssert(   dungeonTmpl.m_dungeon_type == DUNGEON_TYPE_CITY || 
                    dungeonTmpl.m_dungeon_type == DUNGEON_TYPE_DUNGEON ||
                    dungeonTmpl.m_dungeon_type == DUNGEON_TYPE_WORLD || 
                    dungeonTmpl.m_dungeon_type == DUNGEON_TYPE_RECONNECTION);

        if(m_dungeonTmplMap.find(dungeonTmpl.m_dungeon_id) == m_dungeonTmplMap.end())
        {
            m_dungeonTmplMap[dungeonTmpl.m_dungeon_id] = dungeonTmpl;
        }
        else
        {
            CnError("Duplicate dungeon template id:%u", dungeonTmpl.m_dungeon_id);
        }
    }

    return true;
}

void DungeonManager::RecvServerDungeonListNtf(uint32 sessionID, void* data)
{
    SERVER_DUNGEON_LIST_NTF* recvMsg = (SERVER_DUNGEON_LIST_NTF*)data;
    if(!recvMsg)
        return;

    uint32 GameServerID = 0;
    bool isCrossGame = false;
    for(std::vector<SERVER_DUNGEON_INFO>::const_iterator dungeonIt = recvMsg->list.begin(); dungeonIt != recvMsg->list.end(); ++dungeonIt)
    {
        if(GameServerID == 0)
        {
            ServerConfig* pConfig = ConfigLoader::Instance()->GetServerConfig(SERVER_TYPE_GAME, CalGameIDByRegionID(dungeonIt->center_game_id));
            if(!pConfig)
            {
                CnAssert(false);
                return;
            }
            GameServerID = pConfig->m_id;
            isCrossGame = !pConfig->m_is_config;            // 判断是否为跨服Game
        }
        else
        {
            CnAssert(GameServerID == CalGameIDByRegionID(dungeonIt->center_game_id));
        }

        // 添加副本列表
        DungeonInfo dungeonInfo;
        dungeonInfo.Clear();        
        dungeonInfo.m_cent_game_id = dungeonIt->center_game_id;
        dungeonInfo.m_dungeon_id = dungeonIt->dungeon_id;
        dungeonInfo.m_max_channel = dungeonIt->max_channel;
        dungeonInfo.m_max_player = dungeonIt->max_player;


        if(isCrossGame)
        {
            DungeonInfoMap& crossDungeonInfoMap = m_dungeonInfoMapCross[dungeonIt->center_game_id];

            dungeonInfo.m_gamesvr_type = GAMESVR_TYPE_CROSS;

            if(crossDungeonInfoMap.find(dungeonInfo.m_dungeon_id) == crossDungeonInfoMap.end())
            {
                crossDungeonInfoMap[dungeonInfo.m_dungeon_id] = dungeonInfo;
            }
            else
            {
                CnError("Duplicate dungeon info cross gameserverID:%u, dungeonID:%u\n", CalGameIDByRegionID(dungeonInfo.m_cent_game_id), dungeonInfo.m_dungeon_id);
            }

            // 跨服Game才加入到列表中
            std::vector<uint32>& centGameIDList = m_dungeonInGameMapCross[dungeonIt->dungeon_id];
#ifdef DEBUG
            // 检查Game上的副本Id不能重复
            for(uint32 i=0; i<centGameIDList.size(); i++)
            {
                if(centGameIDList[i] == dungeonIt->center_game_id)
                {
                    CnAssert(false);
                }
            }
#endif
            centGameIDList.push_back(dungeonIt->center_game_id);
            
        }
        else
        {
            DungeonInfoMap& dungeonInfoMap = m_dungeonInfoMap[dungeonIt->center_game_id];

            dungeonInfo.m_gamesvr_type = GAMESVR_TYPE_INDIE;

            if(dungeonInfoMap.find(dungeonInfo.m_dungeon_id) == dungeonInfoMap.end())
            {
                dungeonInfoMap[dungeonInfo.m_dungeon_id] = dungeonInfo;
            }
            else
            {
                CnError("Duplicate dungeon info gameserverID:%u, dungeonID:%u\n", CalGameIDByRegionID(dungeonInfo.m_cent_game_id), dungeonInfo.m_dungeon_id);
            }

            // 非跨服Game才加入到列表中
            std::vector<uint32>& centGameIDList = m_dungeonInGameMap[dungeonIt->dungeon_id];
#ifdef DEBUG
            // 检查Game上的副本Id不能重复
            for(uint32 i=0; i<centGameIDList.size(); i++)
            {
                if(centGameIDList[i] == dungeonIt->center_game_id)
                {
                    CnAssert(false);
                }
            }
#endif
            centGameIDList.push_back(dungeonIt->center_game_id);
        }
    }
}

uint32 DungeonManager::RequestJumpDungeonId(uint32 dungeonID, uint32 centGameID, Player* player)
{
    uint32 errCode = 0;

    const std::vector<uint32>* gameList = GetGameListByDungeon(dungeonID, false);
    if(!gameList)
    {
        const std::vector<uint32>* gameList = GetGameListByDungeon(dungeonID, true);
        if(!gameList)
        {
            errCode = ERR_CHANGE_DUNGEON_NO_DUNGEON;    // 副本不存在
            return errCode;
        }
    }

    // 玩家是否可以跳副本判断
    // ...

    return ERR_SUCCEED;
}

uint32 DungeonManager::GetServerIdByUnionId( uint32 unionID )
{
    uint32 dungeonId = DUNGEON_ID(unionID);
    return GetServerIdByDungeonId(dungeonId);
}

uint32 DungeonManager::GetServerIdByDungeonId(uint32 dungeonID)
{
    CnAssert(false);        // 废弃

    return 0;
}

uint32 DungeonManager::GetDungeonType(uint32 dungeonID)
{
    DungeonTmplMap::iterator iter = m_dungeonTmplMap.find(dungeonID);
    if(iter == m_dungeonTmplMap.end()) 
    {
        CnAssert(false);
        return DUNGEON_TYPE_NONE;
    }
    return iter->second.m_dungeon_type;
}

DungeonInfo* DungeonManager::GetDungeonInfoWithServerID(uint32 dungeonID, uint32 centGameID, bool justCrossGame)
{
    DungeonInfoMap* dungeonInfoMap = NULL;

    std::map<uint32, DungeonInfoMap>::iterator iter = m_dungeonInfoMap.find(centGameID);
    if(!justCrossGame && iter != m_dungeonInfoMap.end())
    {
        dungeonInfoMap = &iter->second;
    }
    else
    {
        std::map<uint32, DungeonInfoMap>::iterator crossiter = m_dungeonInfoMapCross.find(centGameID);
        if(crossiter != m_dungeonInfoMapCross.end())
        {
            dungeonInfoMap = &crossiter->second;
        }
        else
        {
        }
    }

    if(!dungeonInfoMap)
        return NULL;

    DungeonInfoMap::iterator dIter = dungeonInfoMap->find(dungeonID);
    if(dIter == dungeonInfoMap->end())
        return NULL;

    DungeonInfo* pDungeonInfo = &dIter->second;
    CnAssert(pDungeonInfo->m_cent_game_id == centGameID);
    CnAssert(pDungeonInfo->m_dungeon_id == dungeonID);

    return pDungeonInfo;
}

bool DungeonManager::HasDungeonId(uint32 dungeonID)
{
    DungeonInGameMap::iterator iter = m_dungeonInGameMap.find(dungeonID);
    if(iter == m_dungeonInGameMap.end())
        return NULL;
    
    std::vector<uint32>& gameList = iter->second;
    return gameList.size() > 0;
}

bool DungeonManager::HasDungeonIdOnGame(uint32 dungeonID, uint32 centGameID, bool justCrossGame)
{
    return GetDungeonInfoWithServerID(dungeonID, centGameID, justCrossGame) != NULL;
}

bool DungeonManager::HasUnionId(uint32 unionId)
{
    return HasDungeonId(DUNGEON_ID(unionId));
}

void DungeonManager::IncreasePlayer(uint32 dungeonId, Player* player)
{
    DungeonInfo* pDungeonInfo = GetDungeonInfoWithServerID(dungeonId, player->GetCentGameID(), false);

    if(!pDungeonInfo)
    {
        CnAssert(false);
        return;
    }

    CnAssert(pDungeonInfo->m_player_count >= 0);
    pDungeonInfo->m_player_count++;

    CnAssert(!player->m_InDungeonCheck);
    player->m_InDungeonCheck = true;
    player->m_InGameServerType = pDungeonInfo->m_gamesvr_type;

    CnAssert(m_CheckPlayers.find(player) == m_CheckPlayers.end());
    m_CheckPlayers[player] = player;

    uint32 checkSize = m_CheckPlayers.size();
    uint32 num = GetPlayerNum();
    CnAssert(checkSize == num);

    //CnDbg("IncreasePlayer=%d %s %d %d\n", dungeonId, player->GetName().c_str(), m_CheckPlayers.size(), GetPlayerNum());
    // 玩家进入副本 log
    DoingGameLog::Instance()->WriteCenterLog(LOG_DUNGEON, player->GetPlayerID(), WriterToLogByDungeon( dungeonId, DUNGEON_STATE_INTO));
}

void DungeonManager::DecreasePlayer(uint32 dungeonId, Player* player)
{
    DungeonInfo* pDungeonInfo = GetDungeonInfoWithServerID(dungeonId, player->GetCentGameID(), false);
    if(!pDungeonInfo)
    {
        CnAssert(false);
        return;
    }

    if(pDungeonInfo->m_player_count <= 0)
    {
        CnWarn("Decrease Player %s dungeonId=%d\n", player->GetName().c_str(), dungeonId);
        CnAssert(false);
    }

    pDungeonInfo->m_player_count--;

    CnAssert(player->m_InDungeonCheck);
    player->m_InDungeonCheck = false;

    std::map<Player*, Player*>::iterator iter = m_CheckPlayers.find(player);
    CnAssert(iter != m_CheckPlayers.end());
    m_CheckPlayers.erase(iter);

    uint32 checkSize = m_CheckPlayers.size();
    uint32 num = GetPlayerNum();
    CnAssert(checkSize == num);

    //CnDbg("DecreasePlayer=%d %s %d %d\n", dungeonId, player->GetName().c_str(), m_CheckPlayers.size(), GetPlayerNum());

    //玩家离开副本log
    DoingGameLog::Instance()->WriteCenterLog(LOG_DUNGEON, player->GetPlayerID(), WriterToLogByDungeon(dungeonId, DUNGEON_STATE_LEVE));
}

uint32 DungeonManager::GetPlayerNum()
{
    uint32 playerNum = 0;
    // 每个Game上的每个副本里人数
    for(std::map<uint32, DungeonInfoMap>::iterator iter = m_dungeonInfoMap.begin(); iter != m_dungeonInfoMap.end(); iter++)
    {
        DungeonInfoMap& dungeonInfoMap = iter->second;
        for(DungeonInfoMap::iterator dIter = dungeonInfoMap.begin(); dIter != dungeonInfoMap.end(); dIter++)
        {
            playerNum += dIter->second.m_player_count;
        }
    }

    for(std::map<uint32, DungeonInfoMap>::iterator iter = m_dungeonInfoMapCross.begin(); iter != m_dungeonInfoMapCross.end(); iter++)
    {
        DungeonInfoMap& dungeonInfoMap = iter->second;
        for(DungeonInfoMap::iterator dIter = dungeonInfoMap.begin(); dIter != dungeonInfoMap.end(); dIter++)
        {
            playerNum += dIter->second.m_player_count;
        }
    }
    return playerNum;
}

void DungeonManager::OnCloseGameServer(uint32 centGameID)
{
    CnAssert(CalCenterIDByRegionID(centGameID));
    CnAssert(CalGameIDByRegionID(centGameID));

    std::map<uint32, DungeonInfoMap>::iterator iter = m_dungeonInfoMap.find(centGameID);
    if(iter != m_dungeonInfoMap.end())
    {
        DungeonInfoMap& dungeonInfoMap = iter->second;
        for(DungeonInfoMap::iterator dIter = dungeonInfoMap.begin(); dIter != dungeonInfoMap.end(); dIter++)
        {
            CnAssert(dIter->second.m_cent_game_id == centGameID);
            CnAssert(dIter->second.m_player_count == 0);        // 安全退出的GS,人数必然为0
        }
        m_dungeonInfoMap.erase(iter);
    }

    iter = m_dungeonInfoMapCross.find(centGameID);
    if(iter != m_dungeonInfoMapCross.end())
    {
        DungeonInfoMap& dungeonInfoMap = iter->second;
        for(DungeonInfoMap::iterator dIter = dungeonInfoMap.begin(); dIter != dungeonInfoMap.end(); dIter++)
        {
            CnAssert(dIter->second.m_cent_game_id == centGameID);
            CnAssert(dIter->second.m_player_count == 0);        // 安全退出的GS,人数必然为0
        }
        m_dungeonInfoMapCross.erase(iter);
    }

    for(DungeonInGameMap::iterator iter = m_dungeonInGameMap.begin(); iter != m_dungeonInGameMap.end(); iter++)
    {
        std::vector<uint32>& GameIDs = iter->second;
        for(std::vector<uint32>::iterator gIter = GameIDs.begin(); gIter != GameIDs.end(); gIter++)
        {
            if(centGameID == *gIter)
            {
                GameIDs.erase(gIter);
                break;
            }
        }
    }

    for(DungeonInGameMap::iterator iter = m_dungeonInGameMapCross.begin(); iter != m_dungeonInGameMapCross.end(); iter++)
    {
        std::vector<uint32>& GameIDs = iter->second;
        for(std::vector<uint32>::iterator gIter = GameIDs.begin(); gIter != GameIDs.end(); gIter++)
        {
            if(centGameID == *gIter)
            {
                GameIDs.erase(gIter);
                break;
            }
        }
    }
}

const std::vector<uint32>* DungeonManager::GetGameListByDungeon(uint32 dungeonID, bool isCross)
{
    if(isCross)
    {
        DungeonInGameMap::iterator iter = m_dungeonInGameMapCross.find(dungeonID);
        if(iter != m_dungeonInGameMapCross.end())
            return &iter->second;
        else
            return NULL;
    }
    else
    {
        DungeonInGameMap::iterator iter = m_dungeonInGameMap.find(dungeonID);
        if(iter != m_dungeonInGameMap.end())
            return &iter->second;
        else
            return NULL;
    }
}
