#include "gameholder_pch.h"
#include "vip_reward_config.h"
#include "item_config_manager.h"
#include "item_base.h"
#include "player.h"
#include "game_util.h"
#include "item_manager.h"

IMPLEMENT_SINGLETON(VipRewardConfig)

VipRewardConfig::VipRewardConfig()
{
    Clear();
}

VipRewardConfig::~VipRewardConfig()
{
    
}

void VipRewardConfig::Clear()
{
    m_vip_reward_map.clear();
}

bool VipRewardConfig::LoadConfig(const char* path)
{
    TiXmlDocument   xmlDoc;
    VipReward       vipReward;
    REWARD_ITEM     rewardItem;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load item file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    // 读取物品信息
    for(TiXmlElement* rewardEle = xmlRoot->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
    {
        vipReward.Clear();
        vipReward.vip = Crown::SDAtou(rewardEle->Attribute("id"));

        for(TiXmlElement* itEle = rewardEle->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
        {
            rewardItem.Clear();

            if(!LoadRewardItem(itEle, rewardItem))
            {
                continue;
            }

            vipReward.items.push_back(rewardItem);
        }

        if(m_vip_reward_map.find(vipReward.vip) == m_vip_reward_map.end())
        {
            m_vip_reward_map[vipReward.vip] = vipReward;
        }
        else
        {
            CnWarn("Duplicate vip reward:%u\n", vipReward.vip);
        }
    }

    return true;
}

const VipReward* VipRewardConfig::GetVipReward(uint32 vip)
{
    std::map<uint32, VipReward>::iterator rewardIt;
    rewardIt = m_vip_reward_map.find(vip);

    if(rewardIt != m_vip_reward_map.end())
    {
        return &(rewardIt->second);
    }

    return nullptr;
}

