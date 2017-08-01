#include "commonlib_pch.h"
#include "activity_reward_config.h"
#include "config_reader.h"
//#include "game_util.h"
//#include "player.h"
//#include "item_manager.h"
//#include "equip_manager.h"
//#include "pet_manager.h"
//#include "ride_manager.h"

IMPLEMENT_SINGLETON(ActivityRewardConfig)
ActivityRewardConfig::ActivityRewardConfig()
{
    Clear();
}

ActivityRewardConfig::~ActivityRewardConfig()
{

}

void ActivityRewardConfig::Clear()
{
    m_activity_reward_map.clear();
}

bool ActivityRewardConfig::LoadConfig( const char* path )
{
    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    REWARD_TABLE rewardTable;
    REWARD_ITEM rewardItem;

    // 读取信息
    for(TiXmlElement* xmlEle = xmlRoot->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if(strcmp(xmlEle->Value(), "activity_reward") == 0)
        {
            for(TiXmlElement* rewardEle = xmlEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
            {
                rewardTable.Clear();
                for(TiXmlElement* itEle = rewardEle->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
                {
                    rewardItem.Clear();
                    if(!ConfigReader::LoadRewardItem(itEle, rewardItem))
                    {
                        continue;
                    }

                    rewardTable.items.push_back(rewardItem);
                }

                rewardTable.reason = Crown::SDAtou(rewardEle->Attribute("reason"));

                uint32 rewardId = Crown::SDAtou(rewardEle->Attribute("id"));
                m_activity_reward_map.insert(std::make_pair(rewardId, rewardTable));
            }
        }
    }

    return true;
}

const REWARD_TABLE* ActivityRewardConfig::GetReward( uint32 rewardId )
{
    ActivityRewardMap::iterator iter = m_activity_reward_map.find(rewardId);
    if (iter != m_activity_reward_map.end())
    {
        return &(iter->second);
    }

    return nullptr;
}

//bool ActivityRewardConfig::CanGiveReward( Player* player, uint32 rewardId)
//{
//    if (!player)
//        return false;
//
//    const ActivityReward* reward = GetReward(rewardId);
//    if (!reward)
//        return false;
//
//    REWARD_ITEM item;
//    REWARD_TABLE table;
//    table.Clear();
//
//    for (ActivityReward::const_iterator rewardIt = reward->begin(); 
//        rewardIt != reward->end(); ++rewardIt)
//    {
//        switch(rewardIt->type)
//        {
//        case PROP_ITEM:
//            {
//                item.Clear();
//                item.id = rewardIt->id;
//                item.num = rewardIt->num;
//                table.items.push_back(item);
//                break;
//            }
//        default:
//            break;
//        }
//    }
//
//    ItemManager* itemMgr = player->GetBag();
//    if(!itemMgr->CanAddRewardTable(table))
//        return false;
//
//    return true;
//}
//
//bool ActivityRewardConfig::GiveReward( Player* player, uint32 rewardId, uint32 rewardReason )
//{
//    if (!player)
//        return false;
//
//    const ActivityReward* reward = GetReward(rewardId);
//    if (!reward)
//        return false;
//
//    // 防沉迷
//    REWARD_TABLE fcmReward;
//    player->GetFcmRewardList(*reward, fcmReward.items, fcmReward.count);
//    fcmReward.reason = rewardReason;
//
//    ItemManager::AddRewardTable(player->GetPlayerID(), fcmReward);
//    return true;
//}
