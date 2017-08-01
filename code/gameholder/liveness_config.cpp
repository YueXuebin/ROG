#include "gameholder_pch.h"
#include "liveness_config.h"

IMPLEMENT_SINGLETON(LivenessConfig)

LivenessConfig::LivenessConfig()
{
    m_target_map.clear();
    m_reward_map.clear();
}

LivenessConfig::~LivenessConfig()
{
    
}

bool LivenessConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if (!xmlRoot) 
        return false;

    for (TiXmlElement* listEle = xmlRoot->FirstChildElement(); listEle; listEle = listEle->NextSiblingElement())
    {
        const char* nodeName = listEle->Value();
        if (Crown::SDStrcmp(nodeName, "target_list") == 0)
        {
            LivenessTarget livenessTarget;
            for (TiXmlElement* ele = listEle->FirstChildElement(); ele; ele = ele->NextSiblingElement())
            {
                livenessTarget.Clear();
                ele->QueryIntAttribute("target", &livenessTarget.target);
                ele->QueryIntAttribute("count", &livenessTarget.count);
                ele->QueryIntAttribute("liveness", &livenessTarget.liveness);
                ele->QueryIntAttribute("system_id", &livenessTarget.sysId);

                m_target_map.insert(std::make_pair(livenessTarget.target,livenessTarget));
            }
        }
        else if (Crown::SDStrcmp(nodeName, "reward_list") == 0)
        {
            LivenessReward livenessReward;
            for (TiXmlElement* ele = listEle->FirstChildElement(); ele; ele = ele->NextSiblingElement())
            {
                livenessReward.Clear();
                ele->QueryIntAttribute("index", &livenessReward.index);
                ele->QueryIntAttribute("liveness", &livenessReward.liveness);
                ele->QueryIntAttribute("common_reward", &livenessReward.common_reward);

                m_reward_map.insert(std::make_pair(livenessReward.index, livenessReward));
            }
        }
        else
        {
            CnError("Load file: %s , unknow node %s\n", path, nodeName);
            return false;
        }
    }

    return true;
}

const LivenessTarget* LivenessConfig::GetTarget(int32 target)
{
    LivenessTargetConfigMap::const_iterator it = m_target_map.find(target);
    if (it != m_target_map.end())
    {
        return &(it->second);
    }
    return NULL;
}

int32 LivenessConfig::GetRewardID( int32 index )
{
    LivenessRewardConfigMap::const_iterator it = m_reward_map.find(index);
    if (it != m_reward_map.end())
    {
        return it->second.common_reward;
    }

    return 0;
}

const LivenessTargetConfigMap* LivenessConfig::GetTargetMap()
{
    return &m_target_map;
}

const LivenessRewardConfigMap* LivenessConfig::GetRewardMap()
{
    return &m_reward_map;
}

