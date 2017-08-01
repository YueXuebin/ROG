#include "gameholder_pch.h"
#include "player_door_reward_config.h"
#include "item_config_manager.h"

IMPLEMENT_SINGLETON(PlayerDoorRewardConfig)


PlayerDoorRewardConfig::PlayerDoorRewardConfig()
{

}

PlayerDoorRewardConfig::~PlayerDoorRewardConfig()
{

}

bool PlayerDoorRewardConfig::LoadConfig( const char* path )
{
    PlayerDoorRewardTemplate rewardTmpl;
    TiXmlDocument xmlDoc;

    std::vector<std::string> gainItems;
    std::vector<std::string> rewardItems;
    REWARD_ITEM gainItem;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load player door reward file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* Ele = xmlRoot->FirstChildElement(); Ele; Ele = Ele->NextSiblingElement())
    {
        rewardTmpl.Clear();

        rewardTmpl.rank = SDAtoi(Ele->Attribute("rank"));
        rewardTmpl.honor_point = SDAtoi(Ele->Attribute("honorpoint"));
        rewardTmpl.game_point = SDAtoi(Ele->Attribute("gamepoint"));
        rewardTmpl.soul_point = SDAtoi(Ele->Attribute("soulpoint"));

        gainItems.clear();
        gainItems = Crown::SDSplitStrings(Ele->Attribute("items"), ' ');
        for(std::vector<std::string>::iterator gainIt = gainItems.begin(); gainIt != gainItems.end(); ++gainIt)
        {
            rewardItems.clear();
            rewardItems = Crown::SDSplitStrings(*gainIt, '-');

            gainItem.Clear();

            if (rewardItems.size() == 3)
            {
                //gainItem.type = SDAtoi(rewardItems[0].c_str());
                gainItem.id = Crown::SDAtou(rewardItems[1].c_str());
                gainItem.num = SDAtoi(rewardItems[2].c_str());

                switch(gainItem.id)
                {
                case PROP_ITEM:      // 奖励物品
                    {
                        //if(!ItemConfig::Instance()->GetItemTmpl(gainItem.id))
                        //{
                        //    CnWarn("Can't find item:%u for player door reward rank:%u gain item\n", gainItem.id, rewardTmpl.rank);
                        //}
                        //else
                        //{
                        //    rewardTmpl.items.push_back(gainItem);
                        //}
                    }
                    break;
                default:
                    CnWarn("Unknow reward id:%u player door reward rank:%u gain item\n", gainItem.id, rewardTmpl.rank);
                    break;
                }
            }
            else
            {
                CnWarn("Unknow player door reward rank:%u gain item\n", rewardTmpl.rank);
            }
        }

        if(m_playerDoorRewards.find(rewardTmpl.rank) == m_playerDoorRewards.end())
        {
            m_playerDoorRewards[rewardTmpl.rank] = rewardTmpl;
        }
        else
        {
            CnWarn("Duplicate player door rank: %u\n", rewardTmpl.rank);
        }
    }

    return true;
}

const PlayerDoorRewardTemplate*  PlayerDoorRewardConfig::GetPlayerDoorRewardTemplate(uint32 rank)
{
    if( m_playerDoorRewards.find(rank) == m_playerDoorRewards.end() )
        return NULL;        // 该rank不存在

    return &m_playerDoorRewards[rank];
}

uint32 PlayerDoorRewardConfig::GetHonorPointByRank(uint32 rank)
{
    if( m_playerDoorRewards.find(rank) == m_playerDoorRewards.end() )
        return NULL;        // 该rank不存在

    return m_playerDoorRewards[rank].honor_point;
}

uint32 PlayerDoorRewardConfig::GetGamePointByRank(uint32 rank)
{
    if( m_playerDoorRewards.find(rank) == m_playerDoorRewards.end() )
        return NULL;        // 该rank不存在

    return m_playerDoorRewards[rank].game_point;
}

uint32 PlayerDoorRewardConfig::GetSoulPointByRank(uint32 rank)
{
    if( m_playerDoorRewards.find(rank) == m_playerDoorRewards.end() )
        return NULL;        // 该rank不存在

    return m_playerDoorRewards[rank].soul_point;
}
