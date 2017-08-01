#include "gameholder_pch.h"
#include "zombie_config.h"
#include "game_util.h"
#include "item_base.h"
#include "config_reader.h"

IMPLEMENT_SINGLETON(ZombieConfig)

ZombieConfig::ZombieConfig()
{

}

ZombieConfig::~ZombieConfig()
{

}

bool ZombieConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;
    REWARD_ITEM item;

    if(!xmlDoc.LoadFile(path))
    {
        CnError("Load dungeon_gain file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    ZombieDungeonInfo info;
    uint32 dungeonId = 0;

    for (TiXmlElement *pElement = xmlRoot->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
    {
        if (strcmp(pElement->Value(), "dungeons") == 0)
        {
            for (TiXmlElement *pRewardElem = pElement->FirstChildElement(); pRewardElem; pRewardElem = pRewardElem->NextSiblingElement())
            {
                info.Clear();
                ConfigReader::Read(pRewardElem, "dungeonid", dungeonId);
                ConfigReader::Read(pRewardElem, "pre_id", info.pre_dungeon_id);
                m_zombieDungeons.insert(std::make_pair(dungeonId, info));
            }
        }
        else if (strcmp(pElement->Value(), "rewards") == 0)
        {
            for (TiXmlElement *pItemElem = pElement->FirstChildElement(); pItemElem; pItemElem = pItemElem->NextSiblingElement())
            {
                item.Clear();
                if (!LoadRewardItem(pItemElem, item))
                    continue;

                m_chestRewards.push_back(item);
            }
        }
        else if (strcmp(pElement->Value(), "consts") == 0)
        {
            for (TiXmlElement *pConstsElem = pElement->FirstChildElement(); pConstsElem; pConstsElem = pConstsElem->NextSiblingElement())
            {
                if (strcmp(pConstsElem->Value(), "multiple_seconds") == 0)
                {
                    ConfigReader::Read(pConstsElem, "value", m_multipleSeconds);
                }
                else if (strcmp(pConstsElem->Value(), "max_cumulative_seconds") == 0)
                {
                    ConfigReader::Read(pConstsElem, "value", m_maxCumulativeSeconds);
                }
                else if (strcmp(pConstsElem->Value(), "need_live_seconds") == 0)
                {
                    ConfigReader::Read(pConstsElem, "value", m_needLiveSeconds);
                }
                else
                {
                    CnError("zombie.xml %s not read", pConstsElem->Value());
                }
            }
        }
        else
        {
            CnError("zombie.xml %s not read", pElement->Value());
        }

    }

    return true;
}

const ZombieDungeonInfo* ZombieConfig::GetZombieDungeonInfo(uint32 dungeonId)
{
    ZombieDungeonMap::iterator iter = m_zombieDungeons.find(dungeonId);
    if (iter != m_zombieDungeons.end())
        return &iter->second;

    return NULL;
}
