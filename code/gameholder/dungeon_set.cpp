#include "gameholder_pch.h"
#include "center_handler.h"
#include "dungeon_set.h"
#include "dungeon.h"
#include "regular_manager.h"
#include "dungeon_manager.h"
#include "scene.h"
#include "battle_unit_connecter.h"
#include "channel_manager.h"
#include "default_channel_manager.h"
#include "city_channel_manager.h"
#include "team_channel_manager.h"
#include "td_channel_manager.h"
#include "ashura_channel_manager.h"
#include "world_boss_channel_manager.h"
#include "city_fight_channel_manager.h"
#include "faction_pk_channel_manager.h"
#include "crusade_boss_channel_manager.h"
#include "twilight_war_channel_manager.h"
#include "city_survival_channel_manager.h"
#include "capture_flag_channel_manager.h"
#include "performance_analysis.h"
#include "multiserver_battle_channel_manager.h"
#include "world_map_channel_manager.h"
#include "chaos_faction_channel_manager.h"
#include "cross_team_channel_manager.h"

DungeonSet::DungeonSet(uint32 dungeonID, uint32 type, uint32 channel_number):
m_SceneXml(NULL),
m_dungeonID(dungeonID),
m_type(type),
m_channel_number(channel_number)
{
    m_pBattleUnitConnecter = CnNew BattleUnitConnecter(this);
    m_pChannelManager = CreateChannelManager(dungeonID, type);
}

DungeonSet::~DungeonSet()
{
    CnDelete m_pChannelManager;
    m_pChannelManager = NULL;

    CnDelete m_pBattleUnitConnecter;        // 先解除 BattleUnit 联系
    m_pBattleUnitConnecter = NULL;

    for(DungeonMap::iterator iter = m_Dungeons.begin(); iter != m_Dungeons.end(); iter++)
    {
        CnDelete iter->second;
    }
}

ChannelManager* DungeonSet::CreateChannelManager(uint32 dungeonID, uint32 type)
{
    if(type == DUNGEON_TYPE_CITY || type == DUNGEON_TYPE_WORLD)
    {
        // 如果以后有进一步需求，则增加void field channel manager
        //if (dungeonID == DUNGEON_VOID_FIELD_ID)
        //    return CnNew VoidFieldChannelManager(this);
        //else
            return CnNew CityChannelManager(this);
    }
    else
    {
        if(dungeonID == DUNGEON_PVP_MAP)
        {
            return CnNew MultiserverBattleChannelManager(this);
        }
    }

    return CnNew DefaultChannelManager(this);
}

void DungeonSet::Update(uint32 frame_count)
{
    PERFORM_HELPER;

    // 战斗单位连接器
    m_pBattleUnitConnecter->Update();

    // 副本更新
    for(DungeonMap::iterator iter = m_Dungeons.begin(); iter != m_Dungeons.end(); iter++)
    {
        iter->second->Update(frame_count);
    }

    // 频道管理器
    m_pChannelManager->Update();

    // 释放无人副本
    for(DungeonMap::iterator dIter = m_Dungeons.begin(); dIter != m_Dungeons.end();)
    {
        Dungeon* pDungeon = dIter->second;
        if(pDungeon->NeedDestory())
        {
            CnAssert(pDungeon->GetPlayerNum() == 0);        // 确认释放的副本没有玩家存在
            DungeonMap::iterator delIter = dIter;
            dIter++;

            m_Dungeons.erase(delIter);
            m_pChannelManager->UnregisterChannel(pDungeon->GetChannelID());
            CnDelete pDungeon;
        }
        else
            dIter++;
    }
}

Dungeon* DungeonSet::CreateDungeon(uint32 channelID)
{
    // 确认频道合法
    CnAssert(checkValidChannelID(channelID));

    m_pChannelManager->RegisterChannel(channelID);

    Dungeon* pNewDungeon = CnNew Dungeon(this, channelID);       // 创建该频道的副本
    if(!pNewDungeon->Init())
    {
        CnDelete pNewDungeon;
        return NULL;
    }

    pNewDungeon->Update(Crown::SDGetTickCount());   // 使场景初始化

    // 放入副本
    m_Dungeons[channelID] = pNewDungeon;

    return pNewDungeon;
}

Dungeon* DungeonSet::GetDungeon(uint32 channelID)
{
    if(m_Dungeons.find(channelID) == m_Dungeons.end())
    {
        return NULL;
    }
    return m_Dungeons[channelID];
}

void DungeonSet::StopAllRegular(bool success)
{
    for(DungeonMap::iterator dungeonIt = m_Dungeons.begin(); dungeonIt != m_Dungeons.end(); ++dungeonIt)
    {
        dungeonIt->second->StopAllRegular(success);
    }
}

uint32 DungeonSet::findValidChannelID()
{
    uint32 id=1;        // 0 保留
    for(; id<CHANNEL_ID_MAX; id++)
    {
        if(m_Dungeons.find(id) == m_Dungeons.end())
            break;
    }

    return id;
}

bool DungeonSet::checkValidChannelID(uint32 channelID)
{
    return m_Dungeons.find(channelID) == m_Dungeons.end();
}

uint32 DungeonSet::GetPlayerNum()
{
    uint32 playerNum = 0;
    for(DungeonMap::iterator iter = m_Dungeons.begin(); iter != m_Dungeons.end(); iter++)
    {
        playerNum += iter->second->GetPlayerNum();
    }
    return playerNum;
}

std::vector<Player*> DungeonSet::GetPlayerMap()
{
    std::vector<Player*> playerMap;
    playerMap.clear();
    for(DungeonMap::iterator it = m_Dungeons.begin(); it != m_Dungeons.end(); ++it)
    {
        std::vector<Player*> dungeonList = it->second->GetPlayerMap();
        playerMap.insert(playerMap.end(), dungeonList.begin(), dungeonList.end());
    }

    return playerMap;
}


//关卡文件校验
bool DungeonSet::CheckMap()
{
    TiXmlHandle sceneXML = GetXml();
    for(TiXmlElement* pScene = sceneXML.FirstChild("scene").ToElement(); pScene; pScene = pScene->NextSiblingElement())
    {
        int32 scene_id = 0;
        pScene->Attribute("id", &scene_id);
        // 获得该副本的地图和关卡
        std::string mapFileName = pScene->Attribute("map");
        std::string designFileName = pScene->Attribute("design");
        int32 close = 0;
        pScene->QueryIntAttribute("close",  &close);

        //关卡文件对比
        std::string filename = RES_DESIGN_PATH;
        filename += designFileName;
        if (RegularManager::Instance()->GetRegularDoc(filename) == NULL)
        {
            CnFatal("%s does not exist!!\n",designFileName.c_str());
            return false;
        }
    }
    return true;
}

void DungeonSet::SendEvent(uint32 eventId, int32 arg1, int32 arg2, uint32 delayTime)
{
    for(DungeonMap::iterator iter = m_Dungeons.begin(); iter != m_Dungeons.end(); iter++)
    {
        iter->second->SendEvent(eventId, arg1, arg2, delayTime);
    }
}

uint32 DungeonSet::AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID)
{
    return m_pChannelManager->AskEnterUnionID(player, sceneID, channelID);
}

