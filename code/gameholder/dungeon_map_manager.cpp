#include "gameholder_pch.h"
#include "dungeon_map_manager.h"

#include "dungeon_type.h"
#include "player.h"
#include "config_reader.h"
#include "config_manager.h"

IMPLEMENT_SINGLETON(DungeonMapManager)

DungeonMapManager::DungeonMapManager()
{
    m_XmlDoc = NULL;
}

DungeonMapManager::~DungeonMapManager()
{
    if (m_XmlDoc)
    {
        CnDelete m_XmlDoc;
        m_XmlDoc = NULL;
    }
}

bool DungeonMapManager::Init()
{
    if (LoadDungeonXML())
    {
        return true;
    }

    return false;
}

bool DungeonMapManager::AskTriggerJumpUnionID(Player* player, uint32 triggerID, uint32& targetUnionID, uint32& targetTriggerID)
{
    // 当前位置
    int32 union_map_id = player->GetUnionMapID();
    uint32 source_dungeonid = DUNGEON_ID(union_map_id);
    uint32 source_sceneid = SCENE_ID(union_map_id);
    uint32 source_triggerid = triggerID;

    // 触发跳转位置
    const MapTriggerTemplate* trigger_template = GetTrigger(source_dungeonid, source_sceneid, source_triggerid);
    if (trigger_template)
    {
        uint32 target_dungeonid = trigger_template->m_targetDungeonID;
        uint32 target_sceneid = trigger_template->m_targetSceneID;
        uint32 target_triggerid = trigger_template->m_targetTriggerID;

        targetUnionID = UNION_ID(0, target_dungeonid, target_sceneid);
        targetTriggerID = target_triggerid;
        return true;
    }

    return false;
}

const MapTriggerTemplate* DungeonMapManager::GetTrigger(uint32 dungeonID, uint32 sceneID, uint32 triggerID)
{
    MapTriggerTemplate trigger_template;
    trigger_template.Clear();

    MapDungeonTemplateMap::const_iterator dungeon_iter = m_dungeonTemplateMap.find(dungeonID);
    if (dungeon_iter != m_dungeonTemplateMap.end())
    {
        // dungeon
        const MapDungeonTemplate& dungeon_tmpl = dungeon_iter->second;

        std::list<MapSceneTemplate>::const_iterator scene_iter = dungeon_tmpl.m_scene_list.begin();
        while (scene_iter != dungeon_tmpl.m_scene_list.end())
        {
            // scene
            if (scene_iter->m_scene_id == sceneID)
            {
                std::list<MapTriggerTemplate>::const_iterator trigger_iter = scene_iter->trigger_list.begin();
                while (trigger_iter != scene_iter->trigger_list.end())
                {
                    // trigger
                    if (trigger_iter->m_sourceTriggerID == triggerID)
                    {
                        return &(*trigger_iter);
                    }
                    ++trigger_iter;
                }
            }
            ++scene_iter;
        }
    }

    return NULL;
}

bool DungeonMapManager::LoadDungeonXML()
{
    TiXmlDocument xmlDoc;

    //if (!xmlDoc.LoadFile(RES_SDB_PATH""DUNGEON_CONFIG_FILE))
    //{
    //    CnError("dungeon config failed filename=%s", RES_SDB_PATH""DUNGEON_CONFIG_FILE);
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

bool DungeonMapManager::LoadDungeon(TiXmlElement* xmlEle)
{
    MapDungeonTemplate dungeonTmpl;

    for(TiXmlElement* dungeonXml = xmlEle->FirstChildElement(); dungeonXml; dungeonXml = dungeonXml->NextSiblingElement())
    {
        if(strcmp(dungeonXml->Value(), "dungeon") != 0)
            continue;

        dungeonTmpl.Clear();

        ConfigReader::Read(dungeonXml, "id", dungeonTmpl.m_dungeon_id);

        // 检查id不能重复
        if(m_dungeonTemplateMap.find(dungeonTmpl.m_dungeon_id) != m_dungeonTemplateMap.end())
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

        m_dungeonTemplateMap[dungeonTmpl.m_dungeon_id] = dungeonTmpl;
    }

    return true;
}

bool DungeonMapManager::LoadScene(TiXmlElement* xmlEle, MapDungeonTemplate& dungeonTmpl)
{
    uint32 scene_id = 0;
    MapSceneTemplate sceneTmpl;

    for(TiXmlElement* sceneXml = xmlEle->FirstChildElement(); sceneXml; sceneXml = sceneXml->NextSiblingElement())
    {
        sceneTmpl.Clear();

        if(strcmp(sceneXml->Value(), "scene") != 0)
        {
            continue;
        }

        // 读取场景属性信息
        sceneXml->QueryUnsignedAttribute("id", &sceneTmpl.m_scene_id);

        for(TiXmlElement* childXml = sceneXml->FirstChildElement(); childXml; childXml = childXml->NextSiblingElement())
        {
            if(strcmp(childXml->Value(), "triggers") == 0)
            {
                LoadTrigger(childXml, sceneTmpl.trigger_list);
            }
        }

        dungeonTmpl.m_scene_list.push_back(sceneTmpl);
    }

    return true;
}

bool DungeonMapManager::LoadTrigger(TiXmlElement* xmlEle, std::list<MapTriggerTemplate>& triggerList)
{
    MapTriggerTemplate teleport;

    for(TiXmlElement* triggerXml = xmlEle->FirstChildElement(); triggerXml; triggerXml = triggerXml->NextSiblingElement())
    {
        teleport.Clear();

        if(strcmp(triggerXml->Value(), "trigger") != 0)
            continue;

        triggerXml->QueryUnsignedAttribute("source", &teleport.m_sourceTriggerID);
        triggerXml->QueryUnsignedAttribute("target_dungeon", &teleport.m_targetDungeonID);
        triggerXml->QueryUnsignedAttribute("target_scene", &teleport.m_targetSceneID);
        triggerXml->QueryUnsignedAttribute("target_trigger", &teleport.m_targetTriggerID);

        if((teleport.m_sourceTriggerID != 0) && (teleport.m_targetSceneID != 0))
            triggerList.push_back(teleport);
        else
            CnAssert(false);
    }

    return true;
}

