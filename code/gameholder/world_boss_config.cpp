#include "gameholder_pch.h"
#include "world_boss_config.h"
#include "game_util.h"

IMPLEMENT_SINGLETON(WorldBossConfig)

WorldBossConfig::WorldBossConfig()
{
    m_mapWorldBossReward.clear();
}

WorldBossConfig::~WorldBossConfig()
{

}

bool WorldBossConfig::LoadWorldBossConfig(const char* path)
{
    REWARD_TABLE rewardTable;
    REWARD_ITEM rewardItem;
    WorldBossReward bossReward;
    WorldBossRewardListByType rewardListByType;
    uint32 dungeon_id = 0;

    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load item file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot)
        return false;

    for(TiXmlElement* worldBossEle = xmlRoot->FirstChildElement(); worldBossEle; worldBossEle = worldBossEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(worldBossEle->Value(), "dungeon") == 0)
        {
            rewardListByType.clear();
            worldBossEle->QueryUnsignedAttribute("dungeon_id", &dungeon_id);
            for(TiXmlElement* dungeonEle = worldBossEle->FirstChildElement(); dungeonEle; dungeonEle = dungeonEle->NextSiblingElement())
            {
                bossReward.clear();
                if(Crown::SDStrcmp(dungeonEle->Value(), "win_rewards") == 0)
                {
                    for(TiXmlElement* rewardsEle = dungeonEle->FirstChildElement(); rewardsEle; rewardsEle = rewardsEle->NextSiblingElement())
                    {
                        rewardTable.Clear();
                        rewardsEle->QueryUnsignedAttribute("rank", &bossReward.rank);
                        for(TiXmlElement* rewardEle = rewardsEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
                        {
                            rewardItem.Clear();
                            if(!LoadRewardItem(rewardEle, rewardItem))
                            {
                                CnFatal("Fatal: world boss reward fatal! rank: %d\n", bossReward.rank);
                                continue;
                            }

                            rewardTable.reason = REWARD_REASON_WORLD_BOSS;
                            rewardTable.items.push_back(rewardItem);
                        }
                        bossReward.rewardTable = rewardTable;
                        rewardListByType.WinRewardList[bossReward.rank] = bossReward;
                    }
                }
                else if(Crown::SDStrcmp(dungeonEle->Value(), "failed_rewards") == 0)
                {
                    for(TiXmlElement* rewardsEle = dungeonEle->FirstChildElement(); rewardsEle; rewardsEle = rewardsEle->NextSiblingElement())
                    {
                        rewardTable.Clear();
                        rewardsEle->QueryUnsignedAttribute("rank", &bossReward.rank);
                        for(TiXmlElement* rewardEle = rewardsEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
                        {
                            rewardItem.Clear();
                            if(!LoadRewardItem(rewardEle, rewardItem))
                            {
                                CnFatal("Fatal: world boss reward fatal! rank: %d\n", bossReward.rank);
                                continue;
                            }

                            rewardTable.reason = REWARD_REASON_WORLD_BOSS;
                            rewardTable.items.push_back(rewardItem);
                        }
                        bossReward.rewardTable = rewardTable;
                        rewardListByType.FaildRewardList[bossReward.rank] = bossReward;
                    }
                }
            }
            m_mapWorldBossReward[dungeon_id] = rewardListByType;
        }
    }

    return true;
}

const REWARD_TABLE* WorldBossConfig::GetWorldBossReward(uint32 dungeon_id, uint32 rank, bool Win)
{
    WorldBossRewardList* rewardList;
    std::map<uint32, WorldBossRewardListByType>::iterator worldBossRewardList = m_mapWorldBossReward.find(dungeon_id);
    if(worldBossRewardList == m_mapWorldBossReward.end())
    {
        CnFatal("Fatal: world boss map id: %d is not exist!\n", dungeon_id);
        return NULL;
    }

    if(Win)
        rewardList = &worldBossRewardList->second.WinRewardList;
    else
        rewardList = &worldBossRewardList->second.FaildRewardList;

    WorldBossRewardList::iterator worldBossReward = rewardList->find(rank);
    if(worldBossReward == rewardList->end())
    {
        return &((*rewardList)[0].rewardTable);
    }
    else
    {
        return &(worldBossReward->second.rewardTable);
    }

    return NULL;
}

