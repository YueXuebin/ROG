#include "gameholder_pch.h"
#include "chaos_faction_config.h"
#include "game_util.h"
#include "property.h"

IMPLEMENT_SINGLETON(ChaosFactionConfig)

ChaosFactionConfig::ChaosFactionConfig()
{
    m_player_max = 0;
    m_poluoKillNum = 0;
    m_skillLevel = 0;
    m_battlePower = 0;
    m_coefficient = 0;
    m_playerPropMap.clear();
    m_scorePlayerDead = 0;
    m_scoreDamageTower = 0;
    m_scorePoluoDead = 0;
    m_winRewards.clear();
    m_loseRewards.clear();
    m_chaosBattlepowerMap.clear();
}

ChaosFactionConfig::~ChaosFactionConfig()
{

}

bool ChaosFactionConfig::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];

    SDSprintf(file_path, "%s%s", path, CHAOS_FACTION_FILE);
    if(!LoadChaosFactionConfig(file_path))
        return false;

    return true;
}

bool ChaosFactionConfig::LoadChaosFactionConfig(const char* path)
{
    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load chaos faction file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    for(TiXmlElement* rootEle = xmlRoot->FirstChildElement(); rootEle; rootEle = rootEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(rootEle->Value(), "setting") == 0)
        {
            rootEle->QueryUnsignedAttribute("player_max", &m_player_max);
        }
        else if(Crown::SDStrcmp(rootEle->Value(), "base") == 0)
        {
            rootEle->QueryUnsignedAttribute("poluo_kill_num", &m_poluoKillNum);
            rootEle->QueryUnsignedAttribute("skill_level", &m_skillLevel);
            rootEle->QueryFloatAttribute("battle_power", &m_battlePower);
            rootEle->QueryFloatAttribute("battlepower_coefficient", &m_coefficient);
        }
        else if(Crown::SDStrcmp(rootEle->Value(), "battlepowers") == 0)
        {
            LoadBattlePower(rootEle);
        }
        else if(Crown::SDStrcmp(rootEle->Value(), "careers") == 0)
        {
            if(!LoadCareers(rootEle))
                return false;
        }
        else if(Crown::SDStrcmp(rootEle->Value(), "score") == 0)
        {
            rootEle->QueryFloatAttribute("player_dead", &m_scorePlayerDead);
            rootEle->QueryFloatAttribute("damage_tower", &m_scoreDamageTower);
            rootEle->QueryFloatAttribute("poluo_dead", &m_scorePoluoDead);
        }
        else if(Crown::SDStrcmp(rootEle->Value(), "bosss") == 0)
        {
            ChaosBossTmp bossTmp;
            for(TiXmlElement* bosssEle = rootEle->FirstChildElement(); bosssEle; bosssEle = bosssEle->NextSiblingElement())
            {
                if(Crown::SDStrcmp(bosssEle->Value(), "boss") == 0)
                {
                    bossTmp.Clear();
                    bosssEle->QueryIntAttribute("group", &bossTmp.group);
                    bosssEle->QueryUnsignedAttribute("spring_id", &bossTmp.spring_id);
                    bosssEle->QueryUnsignedAttribute("perimeter_id", &bossTmp.perimeter_id);
                    bosssEle->QueryUnsignedAttribute("inner_id", &bossTmp.inner_id);
                    bosssEle->QueryUnsignedAttribute("crystal_id", &bossTmp.crystal_id);
                    bosssEle->QueryUnsignedAttribute("poluo_id", &bossTmp.poluo_id);
                    bossTmp.ai = bosssEle->Attribute("ai");

                    const char* pathStr = bosssEle->Attribute("path");
                    bossTmp.pathData = ParsePath(pathStr);
                    m_bossMap[bossTmp.group] = bossTmp;
                }
            }
        }
        else if(Crown::SDStrcmp(rootEle->Value(), "rewards") == 0)
        {
            ChaosReward rewardInfo;
            for(TiXmlElement* rewardsEle = rootEle->FirstChildElement(); rewardsEle; rewardsEle = rewardsEle->NextSiblingElement())
            {
                if(Crown::SDStrcmp(rewardsEle->Value(), "win") == 0)
                {
                    for(TiXmlElement* winEle = rewardsEle->FirstChildElement(); winEle; winEle = winEle->NextSiblingElement())
                    {
                        if(Crown::SDStrcmp(winEle->Value(), "reward") == 0)
                        {
                            rewardInfo.Clear();
                            winEle->QueryUnsignedAttribute("rank", &rewardInfo.rank);
                            winEle->QueryIntAttribute("integral_base", &rewardInfo.integral_base);
                            winEle->QueryIntAttribute("integral_begin", &rewardInfo.integral_begin);
                            winEle->QueryIntAttribute("integral_end", &rewardInfo.integral_end);
                            winEle->QueryIntAttribute("alliance_point", &rewardInfo.alliance_point);
                            m_winRewards[rewardInfo.rank] = rewardInfo;
                        }
                    }
                }
                else if(Crown::SDStrcmp(rewardsEle->Value(), "lose") == 0)
                {
                    for(TiXmlElement* loseEle = rewardsEle->FirstChildElement(); loseEle; loseEle = loseEle->NextSiblingElement())
                    {
                        if(Crown::SDStrcmp(loseEle->Value(), "reward") == 0)
                        {
                            rewardInfo.Clear();
                            loseEle->QueryUnsignedAttribute("rank", &rewardInfo.rank);
                            loseEle->QueryIntAttribute("integral_base", &rewardInfo.integral_base);
                            loseEle->QueryIntAttribute("integral_begin", &rewardInfo.integral_begin);
                            loseEle->QueryIntAttribute("integral_end", &rewardInfo.integral_end);
                            loseEle->QueryIntAttribute("alliance_point", &rewardInfo.alliance_point);
                            m_loseRewards[rewardInfo.rank] = rewardInfo;
                        }
                    }
                }
            }
        }
        else if(Crown::SDStrcmp(rootEle->Value(), "pet_skill") == 0)
        {
            for(TiXmlElement* petSkillEle = rootEle->FirstChildElement(); petSkillEle; petSkillEle = petSkillEle->NextSiblingElement())
            {
                if(Crown::SDStrcmp(petSkillEle->Value(), "pet") == 0)
                {
                    uint32 petid = 0;
                    std::string petPropStr = "";
                    petSkillEle->QueryUnsignedAttribute("id", &petid);
                    petPropStr = petSkillEle->Attribute("skill_prop");
                    m_followPetPropMap[petid] = GetPropListByString(petPropStr);
                }
            }
        }
    }

    return true;
}

bool ChaosFactionConfig::LoadBattlePower(TiXmlElement* rootEle)
{
    for(TiXmlElement* battlepowersEle = rootEle->FirstChildElement(); battlepowersEle; battlepowersEle = battlepowersEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(battlepowersEle->Value(), "prop") == 0)
        {
            uint32 propType = 0;
            real battle_power = 0;
            battlepowersEle->QueryUnsignedAttribute("type", &propType);
            battlepowersEle->QueryFloatAttribute("coefficient", &battle_power);

            if(m_chaosBattlepowerMap.find(propType) != m_chaosBattlepowerMap.end())
            {
                CnError("chaos battlepower prop type:%d is exist!\n", propType);
                return false;
            }

            m_chaosBattlepowerMap[propType] = battle_power;
        }
    }

    return true;
}

bool ChaosFactionConfig::LoadCareers(TiXmlElement* rootEle)
{
    ChaosPlayerTmp chaosPlayerTmp;
    ChaosProp prop;

    for(TiXmlElement* careersEle = rootEle->FirstChildElement(); careersEle; careersEle = careersEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(careersEle->Value(), "career") == 0)
        {
            chaosPlayerTmp.Clear();
            careersEle->QueryIntAttribute("id", &chaosPlayerTmp.career);
            careersEle->QueryIntAttribute("level", &chaosPlayerTmp.level);
            careersEle->QueryFloatAttribute("value_total", &chaosPlayerTmp.valueTotal);

            for(TiXmlElement* careerEle = careersEle->FirstChildElement(); careerEle; careerEle = careerEle->NextSiblingElement())
            {
                if(Crown::SDStrcmp(careerEle->Value(), "prop") == 0)
                {
                    prop.Clear();
                    careerEle->QueryIntAttribute("type", &prop.type);
                    careerEle->QueryFloatAttribute("value", &prop.value);
                    chaosPlayerTmp.propList.push_back(prop);
                }
            }

            CnAssert(chaosPlayerTmp.valueTotal > 0);

            m_playerPropMap[chaosPlayerTmp.career] = chaosPlayerTmp;
        }
    }

    return true;
}

const ChaosPlayerTmp* ChaosFactionConfig::GetPlayerProp(int8 career)
{
    std::map<int8, ChaosPlayerTmp>::iterator playerIt = m_playerPropMap.find(career);
    if(playerIt == m_playerPropMap.end())
        return NULL;

    return &playerIt->second;
}

real ChaosFactionConfig::GetPropByBattlepower(uint32 propType, real battlepower)
{
    ChaosBattlepowerMap::iterator it = m_chaosBattlepowerMap.find(propType);
    if(it == m_chaosBattlepowerMap.end())
        return 0;

    if(it->second == 0)
        return 0;

    return battlepower / (real)it->second;
}

const ChaosBossTmp* ChaosFactionConfig::GetBossTmp(int8 groupType)
{
    std::map<int8, ChaosBossTmp>::iterator bossIt = m_bossMap.find(groupType);
    if(bossIt == m_bossMap.end())
        return NULL;

    return &bossIt->second;
}

const ChaosReward* ChaosFactionConfig::GetWinRewardTmpl(int32 rank)
{
    std::map<uint32, ChaosReward>::iterator rewardIt = m_winRewards.find(rank);
    if(rewardIt == m_winRewards.end())
    {
        CnError("chaos reward is not exit!win rank:%d", rank);
        return NULL;
    }

    return &rewardIt->second;
}

const ChaosReward* ChaosFactionConfig::GetLoseRewardTmpl(int32 rank)
{
    std::map<uint32, ChaosReward>::iterator rewardIt = m_loseRewards.find(rank);
    if(rewardIt == m_loseRewards.end())
    {
        CnError("chaos reward is not exit!lose rank:%d", rank);
        return NULL;
    }

    return &rewardIt->second;
}

const PropertyList* ChaosFactionConfig::GetFollowPetProp(uint32 followPetID)
{
    std::map<uint32, PropertyList>::iterator it = m_followPetPropMap.find(followPetID);
    if(it == m_followPetPropMap.end())
    {
        CnError("In chaos faction, Follow pet prop is not exist! pet id:%d\n", followPetID);
        return 0;
    }

    return &it->second;
}


