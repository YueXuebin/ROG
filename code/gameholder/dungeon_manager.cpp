#include "gameholder_pch.h"
#include "dungeon_manager.h"
#include "dungeon_set.h"
#include "dungeon.h"
#include "scene.h"
#include "server_config.h"
#include "center_handler.h"
#include "regular_manager.h"
#include "regular.h"
#include "config_reader.h"
#include "server_config.h"
#include "config_manager.h"

IMPLEMENT_SINGLETON(DungeonManager)

void  DungeonTemplate::GetRaidsMonsterList(std::map<uint32, int32>& monsterList)
{
    for(std::list<SceneTemplate>::iterator it = m_scene_list.begin(); it != m_scene_list.end(); ++it)
    {
        // 创建关卡的规则
        const SceneTemplate& sceneTemp = *it;
        Regular* pRegular = RegularManager::Instance()->CreateRegular(sceneTemp.design_file.c_str());
        if(pRegular)
        {
            pRegular->GetRaidsMonsterList(monsterList);      // 获取关卡中的怪物列表
            CnDelete pRegular;
        }
        else
            CnAssert(false);
    }
}

// ====================================================================================================

DungeonManager::DungeonManager()
{
    m_XmlDoc = NULL;
}

DungeonManager::~DungeonManager()
{
    for(DungeonSetMap::iterator iter = m_dungeonSets.begin(); iter != m_dungeonSets.end(); iter++)
    {
        CnDelete iter->second;
    }

    if(m_XmlDoc)
        CnDelete m_XmlDoc;
}

bool DungeonManager::Init()
{
    LoadXml();

    CreateDungeonSet();

    return true;
}

bool DungeonManager::CreateDungeonSet()
{
    DungeonSet* pDungeonSet = 0;

    for(DungeonInfoMap::iterator infoIt = m_dungeonInfos.begin(); infoIt != m_dungeonInfos.end(); ++infoIt)
    {
        if(!infoIt->second.m_dungeon_tmpl)
        {
            CnError("Can't create dungeon set %u\n", infoIt->second.m_dungeon_id);
            CnAssert(false);
            continue;
        }

        // 创建副本集
        pDungeonSet = CnNew DungeonSet(infoIt->second.m_dungeon_id, 
                    infoIt->second.m_dungeon_tmpl->m_dungeon_type, infoIt->second.m_max_channel);

        pDungeonSet->SetDungeonTemplate(infoIt->second.m_dungeon_tmpl);

        m_dungeonSets[infoIt->second.m_dungeon_id] = pDungeonSet;
    }

    // 创建城镇和野外类的永久地图
    for(DungeonSetMap::iterator dungeonSetIt = m_dungeonSets.begin(); dungeonSetIt != m_dungeonSets.end(); dungeonSetIt++)
    {
        DungeonSet* pDungeonSet = dungeonSetIt->second;
        if((pDungeonSet->GetType() == DUNGEON_TYPE_CITY) || (pDungeonSet->GetType() == DUNGEON_TYPE_WORLD))
        {
            for(uint32 ci=0; ci<pDungeonSet->GetChannelNumber(); ci++)
            {
                Dungeon* pDungeon = pDungeonSet->CreateDungeon(ci+1);
            }

            if(pDungeonSet->GetType() == DUNGEON_TYPE_CITY)
            {
                m_CityIDs.push_back(pDungeonSet->GetDungeonID());
            }
        }
    }

    return true;
}

bool DungeonManager::LoadXml()
{
    if(!LoadDungeonXML())
    {
        return false;
    }

    if(!LoadGameSvrXML())
    {
        return false;
    }

    return true;
}

bool DungeonManager::LoadDungeonXML()
{
    TiXmlDocument xmlDoc;

    //if (!xmlDoc.LoadFile(RES_SDB_PATH""DUNGEON_CONFIG_FILE))
    //{
    //   CnError("dungeon config failed filename=%s", RES_SDB_PATH""DUNGEON_CONFIG_FILE);
    //    return false;
    //}

    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", ConfigManager::Instance()->GetSdbPath().c_str(), DUNGEON_CONFIG_FILE);

    if (!xmlDoc.LoadFile(file_path))
    {
        CnError("dungeon config failed filename=%s", file_path);
        return false;
    }

    TiXmlElement* rooXml = xmlDoc.RootElement();

    for(TiXmlElement* xmlEle = rooXml->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if(strcmp(xmlEle->Value(), "dungeons") == 0)
        {
            if(!LoadDungeon(xmlEle))
            {
                return false;
            }
        }
    }

    return true;
}

bool DungeonManager::LoadGameSvrXML()
{
    TiXmlDocument xmlDoc;
    TiXmlElement* rooXml = 0;
    TiXmlElement* gamesvrXml = 0;
    uint32 myID = 0;
    uint32 serverID = 0;
    uint32 serverType = 0;

    if (!xmlDoc.LoadFile(RES_CONFIG_PATH""GAMESVR_CONFIG_FILE))
    {
        goto _ERR;
    }

    // 获得服务器自己的ID
    myID = ServerConfig::Instance()->GetMyID();

    // 根节点
    if(!(rooXml = xmlDoc.RootElement()))
    {
        goto _ERR;
    }

    // Gameserver节点
    if(!(gamesvrXml = rooXml->FirstChildElement("gameserver")))
    {
        goto _ERR;
    }

    for(; gamesvrXml; gamesvrXml = gamesvrXml->NextSiblingElement())
    {
        if(!LoadGameServer(gamesvrXml))
        {
            return false;
        }

        break;
    }

    CnInfo("%d dungeons on gameserver %d\n", m_dungeonInfos.size(), myID);

    return true;

_ERR:
    CnError("%s load failed\n", RES_CONFIG_PATH""GAMESVR_CONFIG_FILE);
    return false;
}

bool DungeonManager::LoadDungeon(TiXmlElement* xmlEle)
{
    DungeonTemplate dungeonTmpl;

    for(TiXmlElement* dungeonXml = xmlEle->FirstChildElement(); dungeonXml; dungeonXml = dungeonXml->NextSiblingElement())
    {
        if(strcmp(dungeonXml->Value(), "dungeon") != 0)
            continue;

        dungeonTmpl.Clear();

        ConfigReader::Read(dungeonXml, "id", dungeonTmpl.m_dungeon_id);
        ConfigReader::Read(dungeonXml, "type", dungeonTmpl.m_dungeon_type);

        // 检查id不能重复
        if(m_dungeonTmpls.find(dungeonTmpl.m_dungeon_id) != m_dungeonTmpls.end())
        {
            CnError("Duplicate dungeon id=%u\n", dungeonTmpl.m_dungeon_id);
            continue;
        }

        for(TiXmlElement* childXml = dungeonXml->FirstChildElement(); childXml; childXml = childXml->NextSiblingElement())
        {
            if(strcmp(childXml->Value(), "scenes") == 0)
            {
                LoadScene(childXml, dungeonTmpl);
            }
        }

        m_dungeonTmpls[dungeonTmpl.m_dungeon_id] = dungeonTmpl;
    }

    return true;
}

bool DungeonManager::LoadScene(TiXmlElement* xmlEle, DungeonTemplate& dungeonTmpl)
{
    uint32 scene_id = 0;
    SceneTemplate sceneTmpl;

    for(TiXmlElement* sceneXml = xmlEle->FirstChildElement(); sceneXml; sceneXml = sceneXml->NextSiblingElement())
    {
        sceneTmpl.Clear();

        if(strcmp(sceneXml->Value(), "scene") != 0)
        {
            continue;
        }

        // 读取场景属性信息
        sceneXml->QueryIntAttribute("close", &sceneTmpl.m_close);
        sceneXml->QueryUnsignedAttribute("id", &sceneTmpl.m_scene_id);
        sceneTmpl.design_file = sceneXml->Attribute("design");
        sceneTmpl.design_file = RES_DESIGN_PATH + sceneTmpl.design_file;

        sceneTmpl.m_map_file = sceneXml->Attribute("map");
        sceneTmpl.m_map_file = RES_MAP_PATH + sceneTmpl.m_map_file ;

        // 检测关卡文件和场景文件都存在
        if(!Crown::SDFileExists(sceneTmpl.design_file.c_str()))
        {
            CnError("design file %s not exist\n", sceneTmpl.design_file.c_str());
        }
        if(!Crown::SDFileExists(sceneTmpl.m_map_file.c_str()))
        {
            CnError("map file %s not exist\n", sceneTmpl.m_map_file.c_str());
        }

        for(TiXmlElement* childXml = sceneXml->FirstChildElement(); childXml; childXml = childXml->NextSiblingElement())
        {
            if(strcmp(childXml->Value(), "triggers") == 0)
            {
                bool ret = LoadTrigger(childXml, sceneTmpl.trigger_list);
                if(!ret)
                    CnError("load dungeonId=%d sceneId=%d trigger error\n", dungeonTmpl.m_dungeon_id, sceneTmpl.m_scene_id);
            }
        }

        dungeonTmpl.m_scene_list.push_back(sceneTmpl);
    }

    return true;
}

bool DungeonManager::LoadTrigger(TiXmlElement* xmlEle, std::list<Teleport>& triggerList)
{
    Teleport teleport;

    for(TiXmlElement* triggerXml = xmlEle->FirstChildElement(); triggerXml; triggerXml = triggerXml->NextSiblingElement())
    {
        teleport.Clear();

        if(strcmp(triggerXml->Value(), "trigger") != 0)
            continue;

        triggerXml->QueryUnsignedAttribute("source", &teleport.m_sourceTriggerID);
        triggerXml->QueryUnsignedAttribute("target_scene", &teleport.m_targetSceneID);
        triggerXml->QueryUnsignedAttribute("target_trigger", &teleport.m_targetTriggerID);

        if((teleport.m_sourceTriggerID != 0) && (teleport.m_targetSceneID != 0))
            triggerList.push_back(teleport);
        else
        {
            CnError("sourceTriggerID=%d targetSceneID=%d targetTriggerID=%d\n", teleport.m_sourceTriggerID, teleport.m_targetSceneID, teleport.m_targetTriggerID);
            CnAssert(false);
            return false;
        }
    }

    return true;
}

bool DungeonManager::LoadGameServer(TiXmlElement* xmlEle)
{
    DungeonTmplMap::iterator tmplIt;
    DungeonInfo dungeonInfo;

    for(TiXmlElement* dungeonEle = xmlEle->FirstChildElement(); dungeonEle; dungeonEle = dungeonEle->NextSiblingElement())
    {
        if(strcmp(dungeonEle->Value(), "dungeon") != 0)
            continue;

        dungeonInfo.Clear();

        // 地图ID
        if(!ConfigReader::Read(dungeonEle, "id", dungeonInfo.m_dungeon_id))
            continue;

        // 副本中最大频道数
        if(!ConfigReader::Read(dungeonEle, "channel_number", dungeonInfo.m_max_channel))
            continue;

        // 副本中最大人数
        if(!ConfigReader::Read(dungeonEle, "max_player", dungeonInfo.m_max_player))
            continue;

        // 查找副本模板
        tmplIt = m_dungeonTmpls.find(dungeonInfo.m_dungeon_id);
        if(tmplIt == m_dungeonTmpls.end())
        {
            CnError("Can't find dungeon template:%u\n", dungeonInfo.m_dungeon_id);
            continue;
        }

        // 设置副本的GameServerID
        dungeonInfo.m_gamesvr_id = ServerConfig::Instance()->GetMyID();

        // 复制副本的模板地址
        dungeonInfo.m_dungeon_tmpl = &tmplIt->second;

        // 保存副本信息
        if(m_dungeonInfos.find(dungeonInfo.m_dungeon_id) != m_dungeonInfos.end())
        {
            CnError("Duplicate dungeon info %u\n", dungeonInfo.m_dungeon_id);
            continue;
        }

        m_dungeonInfos[dungeonInfo.m_dungeon_id] = dungeonInfo;
    }

    return true;
}

Dungeon* DungeonManager::GetDungeon(uint32 dungeonID, uint32 channelID)
{
    if(m_dungeonSets.find(dungeonID) == m_dungeonSets.end())
        return NULL;

    DungeonSet* dungeonSet = m_dungeonSets[dungeonID];
    return dungeonSet->GetDungeon(channelID);
}

// ==================================================
// 进入副本
// ==================================================
Scene* DungeonManager::GetDefaultScene(uint32 dungeonID, uint32 channelID)
{
    Scene* scene = 0;

    CnAssert(channelID > 0 && channelID <= CHANNEL_ID_MAX);
    CnAssert(dungeonID > 0 && dungeonID <= DUNGEON_ID_MAX);

    DungeonTemplate* dungeonTmpl = GetDungeonTemplate(dungeonID);
    if(!dungeonTmpl)
    {
        CnAssert(false);
        return 0;
    }

    if(DUNGEON_TYPE_DUNGEON == dungeonTmpl->m_dungeon_type)
    {
        DungeonSet* pDungeonSet = GetDungeonSet(dungeonID);
        Dungeon* pEnterDungeon = pDungeonSet->GetDungeon(channelID);
        CnAssert(pEnterDungeon);                            // 要进入的副本不存在

        scene = pEnterDungeon->GetDefaultScene();
        pEnterDungeon->Update(Crown::SDGetTickCount());     // 使场景初始化
    }
    else if(DUNGEON_TYPE_CITY == dungeonTmpl->m_dungeon_type || DUNGEON_TYPE_WORLD == dungeonTmpl->m_dungeon_type || DUNGEON_TYPE_RECONNECTION == dungeonTmpl->m_dungeon_type)
    {   // 查找主城
        scene = GetScene(UNION_ID(channelID, dungeonID, 1));
    }

    CnAssert(scene);
    return scene;
}

// ==================================================
// 获得场景
// ==================================================
Scene*  DungeonManager::GetScene(uint32 sceneUnionID)
{
    uint32 channelID = CHANNEL_ID(sceneUnionID);
    uint32 dungeonID = DUNGEON_ID(sceneUnionID);
    uint32 sceneID = SCENE_ID(sceneUnionID);

    if(channelID==0 || dungeonID==0 || sceneID==0)
        return NULL;

    if(m_dungeonSets.find(dungeonID) == m_dungeonSets.end())
    {
        return NULL;
    }

    Dungeon* pDungeon = m_dungeonSets[dungeonID]->GetDungeon(channelID);
    if(pDungeon)
        return pDungeon->GetScene(sceneID);
    else
        return NULL;
}


void DungeonManager::StopAllRegular(uint32 dungeonID, bool success)
{
    DungeonSet* dungeon = GetDungeonSet(dungeonID);
    if(!dungeon)
    {
        return;
    }
    dungeon->StopAllRegular(success);
}

DungeonSet* DungeonManager::GetDungeonSet(uint32 dungeonId)
{
    if(m_dungeonSets.find(dungeonId) == m_dungeonSets.end())
        return NULL;

    return m_dungeonSets[dungeonId];
}

// ==================================================
// 获得模板信息
// ==================================================
DungeonTemplate* DungeonManager::GetDungeonTemplate(uint32 dungeonID)
{
    DungeonTmplMap::iterator it;
    it = m_dungeonTmpls.find(dungeonID);
    if(it != m_dungeonTmpls.end())
    {
        return &it->second;
    }
    return 0;
}

// ==================================================
// 获得副本信息
// ==================================================
DungeonInfo* DungeonManager::GetDungeonInfo(uint32 dungeonID)
{
    DungeonInfoMap::iterator it;
    it = m_dungeonInfos.find(dungeonID);
    if(it != m_dungeonInfos.end())
    {
        return &it->second;
    }
    return NULL;
}

// ==================================================
// 获得副本所在服务器id
// ==================================================
uint32 DungeonManager::GetServerIdByDungeonId( uint32 dungeonID )
{
    DungeonInfoMap::iterator it;
    it = m_dungeonInfos.find(dungeonID);
    if(it != m_dungeonInfos.end())
    {
        return it->second.m_gamesvr_id;
    }
    return 0;
}

Trigger* DungeonManager::GetTrigger(uint32 sceneUnionID, uint32 triggerID)
{
    Scene* pScene = GetScene(sceneUnionID);

    if (pScene)
    {
        return pScene->GetTriggerByID(triggerID);
    }

    return NULL;
}

void DungeonManager::Update(uint32 frame_count)
{
    for(DungeonSetMap::iterator iter = m_dungeonSets.begin(); iter != m_dungeonSets.end(); iter++)
    {
        DungeonSet* pDungeonSet = iter->second;

        if(pDungeonSet->GetDungeonNumber() > 0)
            iter->second->Update(frame_count);
    }
}

void DungeonManager::SendEvent(uint32 eventId, int32 arg1, int32 arg2, uint32 delayTime)
{
    for(DungeonSetMap::iterator iter = m_dungeonSets.begin(); iter != m_dungeonSets.end(); iter++)
    {
        DungeonSet* pDungeonSet = iter->second;
        pDungeonSet->SendEvent(eventId, arg1, arg2, delayTime);
    }
}

bool DungeonManager::HasDungeonId(uint32 dungeonId)
{
    DungeonTmplMap::iterator iter = m_dungeonTmpls.find(dungeonId);
    if(iter != m_dungeonTmpls.end())
        return true;

    return false;
}

bool DungeonManager::IsCity(uint32 dungeonID)
{
    for(std::vector<uint32>::iterator iter = m_CityIDs.begin(); iter != m_CityIDs.end(); iter++)
    {
        if(dungeonID == *iter)
            return true;
    }
    return false;
}

uint32 DungeonManager::GetPlayerNum()
{
    uint32 playerNum = 0;
    for(DungeonSetMap::iterator iter = m_dungeonSets.begin(); iter != m_dungeonSets.end(); iter++)
    {
        DungeonSet* pDungeonSet = iter->second;
        playerNum += pDungeonSet->GetPlayerNum();
    }
    return playerNum;
}

std::string DungeonManager::GetDesignFileName(uint32 dungeonId, uint32 sceneId)
{
    if(m_dungeonTmpls.find(dungeonId) == m_dungeonTmpls.end())
    {
        CnAssert(false);
        return "";
    }

    DungeonTemplate& dungeonTemplate = m_dungeonTmpls[dungeonId];
    for(std::list<SceneTemplate>::iterator iter = dungeonTemplate.m_scene_list.begin(); iter != dungeonTemplate.m_scene_list.end(); iter++)
    {
        if(iter->m_scene_id == sceneId)
        {
            return iter->design_file;
        }
    }

    CnAssert(false);
    return "";
}

// ==================================================
// 发送服务器副本列表通知(Game->Center)
// ==================================================
void DungeonManager::SendServerDungeonListNtf(uint32 sessionID)
{
    SERVER_DUNGEON_LIST_NTF sendMsg;
    SERVER_DUNGEON_INFO dungeonInfo;

    for(DungeonInfoMap::const_iterator dungeonIt = m_dungeonInfos.begin(); dungeonIt != m_dungeonInfos.end(); ++dungeonIt)
    {
        dungeonInfo.Clear();
        dungeonInfo.center_game_id = CalRegionID(ServerConfig::Instance()->GetCenterID(), ServerConfig::Instance()->GetMyID());
        dungeonInfo.dungeon_id = dungeonIt->second.m_dungeon_id;
        dungeonInfo.max_channel = dungeonIt->second.m_max_channel;
        dungeonInfo.max_player = dungeonIt->second.m_max_player;
        sendMsg.list.push_back(dungeonInfo);
    }

    CenterHandler::Instance()->SendBySessionID(sessionID, MSG_SERVER_DUNGEON_LIST_NTF, &sendMsg);
}

uint32 DungeonManager::RequestJumpDungeonId(uint32 dungeonID)
{
    uint32 errCode = 0;

    if(!HasDungeonId(dungeonID))
    {
        errCode = ERR_CHANGE_DUNGEON_NO_DUNGEON;    // 副本不存在
        return errCode;       
    }

    uint32 gameServerID = GetServerIdByDungeonId(dungeonID);
    if(gameServerID == 0)
    {
        errCode = ERR_CHANGE_DUNGEON_CLOSED;    //没有这个副本
        return errCode;
    }

    // 玩家是否可以跳副本判断
    // ...

    return ERR_SUCCEED;
}