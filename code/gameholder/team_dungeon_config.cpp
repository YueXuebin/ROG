#include "gameholder_pch.h"
#include "team_dungeon_config.h"
#include "player.h"

IMPLEMENT_SINGLETON(TeamDungeonConfig)

TeamDungeonConfig::TeamDungeonConfig()
{
    Clear();
}

TeamDungeonConfig::~TeamDungeonConfig()
{
    
}

void TeamDungeonConfig::Clear()
{
    m_team_dungeon_map.clear();
}

// ==================================================
// 加载配置文件
// ==================================================
bool TeamDungeonConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;
    TeamDungeonTmp teamDungeon;
    teamDungeon.Clear();

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load team_dungeon file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    // 服务器配置信息
    for(TiXmlElement* xmlEle = xmlRoot->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if(strcmp(xmlEle->Value(), "team_dungeon") == 0)
        {
            teamDungeon.Clear();
            teamDungeon.Id = SDAtoi(xmlEle->Attribute("id"));
            teamDungeon.level = SDAtoi(xmlEle->Attribute("level"));
            teamDungeon.type = SDAtoi(xmlEle->Attribute("type"));
            teamDungeon.physicalStrength = SDAtoi(xmlEle->Attribute("physical_strength"));
            teamDungeon.dungeonId = SDAtoi(xmlEle->Attribute("dungeon_id"));

            if(m_team_dungeon_map.find(teamDungeon.Id) == m_team_dungeon_map.end())
            {
                m_team_dungeon_map[teamDungeon.Id] = teamDungeon;
            }
            else
            {
                CnWarn("Duplicate team dungeon id: %u\n", teamDungeon.Id);
            }
        }
    }

    return true;
}

const TeamDungeonTmp* TeamDungeonConfig::GetTeamDungeonInfoByDungeonID(uint32 dungeonId)
{
    std::map<int32, TeamDungeonTmp>::iterator teamIt = m_team_dungeon_map.begin();

    for(; teamIt != m_team_dungeon_map.end(); ++teamIt)
    {
        if(teamIt->second.dungeonId == dungeonId)
        {
            return &teamIt->second;
        }
    }

    return NULL;
}
