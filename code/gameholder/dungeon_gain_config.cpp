#include "gameholder_pch.h"
#include "dungeon_gain_config.h"
#include "item_config_manager.h"
#include "vip_config.h"
#include "dungeon_score_config.h"
#include "game_util.h"

IMPLEMENT_SINGLETON(DungeonGainConfig)


DungeonGainConfig::DungeonGainConfig()
{

}

DungeonGainConfig::~DungeonGainConfig()
{

}

bool DungeonGainConfig::LoadConfig(const char* path)
{
    REWARD_ITEM rewardItem;
    DungeonGainTemplate dungeonGainTmpl;
    std::string gainItems;
    std::vector<std::string> rewardItems;
    TiXmlDocument xmlDoc;

    if(!xmlDoc.LoadFile(path))
    {
        CnError("Load dungeon_gain file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* dungeon_gainEle = xmlRoot->FirstChildElement(); dungeon_gainEle; dungeon_gainEle = dungeon_gainEle->NextSiblingElement())
    {
        rewardItem.Clear();
        dungeonGainTmpl.Clear();
        dungeonGainTmpl.id = SDAtoi(dungeon_gainEle->Attribute("id"));
        dungeonGainTmpl.name = dungeon_gainEle->Attribute("name");
        dungeonGainTmpl.exp = SDAtoi(dungeon_gainEle->Attribute("exp"));
        dungeonGainTmpl.soul = SDAtoi(dungeon_gainEle->Attribute("soul"));
        dungeonGainTmpl.game_point = SDAtoi(dungeon_gainEle->Attribute("gamepoint"));

        gainItems.clear();
        gainItems = dungeon_gainEle->Attribute("gain");

        REWARD_TABLE rewardTable;
        if(!LoadRewardTable(gainItems, rewardTable))
        {
            CnWarn("Unknow dungeonID:%u gain item\n", dungeonGainTmpl.id);
            return false;
        }
        dungeonGainTmpl.gain_items = rewardTable.items;

        if(m_dungeon_gains.find(dungeonGainTmpl.id) == m_dungeon_gains.end())
        {
            m_dungeon_gains[dungeonGainTmpl.id] = dungeonGainTmpl;
        }
        else
        {
            CnWarn("Duplicate dungeon_gain id: %u\n", dungeonGainTmpl.id);
        }
    }

    return true;
}

const DungeonGainTemplate*  DungeonGainConfig::GetDungeonGainTemplate(uint32 dungeon_Id)
{
    if( m_dungeon_gains.find(dungeon_Id) == m_dungeon_gains.end() )
        return NULL;        // 该Id不存在

    return &m_dungeon_gains[dungeon_Id];
}

void DungeonGainConfig::GetDungeonGainReward(const DungeonGainTemplate* gainTmpl, int32 num, std::vector<REWARD_ITEM>& rewards)
{
    std::vector<REWARD_ITEM> gainReward;
    uint32 randNum = 0;

    if(!gainTmpl) return;

    // 赋值装备
    gainReward = gainTmpl->gain_items;

    // 计算给予的物品
    for(int32 i=0; (i < num) && (gainReward.size() > 0); ++i)
    {
        randNum = Crown::SDRandom(gainReward.size());
        rewards.push_back(gainReward[randNum]);
        gainReward.erase(gainReward.begin() + randNum);
    }
}

void DungeonGainConfig::GetDungeonGainRewardEx( const DungeonGainTemplate* gainTmpl, int32 num, std::vector<REWARD_ITEM>& rewards, const std::vector<REWARD_ITEM>& excludedRewards )
{
    std::vector<REWARD_ITEM> gainReward;
    uint32 randNum = 0;

    if(!gainTmpl) return;

    // 将排除列表中的奖励排除
    if ((excludedRewards.size() >0) && (gainTmpl->gain_items.size() > excludedRewards.size()))
    {
        for (uint32 i = 0; i < gainTmpl->gain_items.size(); i++)
        {
            //for (uint32 j = 0; j < excludedRewards.size(); j++)
            //{
            //    if ((gainTmpl->gain_items[i].flag != excludedRewards[j].flag) ||
            //        (gainTmpl->gain_items[i].type != excludedRewards[j].type) ||
            //        (gainTmpl->gain_items[i].id != excludedRewards[j].id) ||
            //        (gainTmpl->gain_items[i].num != excludedRewards[j].num))
            //    {
            //        gainReward.push_back(gainTmpl->gain_items[i]);
            //    }
            //}
        }
    }
    else
    {
        // 排除列表为空或奖励数量都不足以排除的,就不排除了
        gainReward = gainTmpl->gain_items;
    }

    // 计算给予的物品
    for(int32 i=0; (i < num) && (gainReward.size() > 0); ++i)
    {
        randNum = Crown::SDRandom(gainReward.size());
        rewards.push_back(gainReward[randNum]);
        gainReward.erase(gainReward.begin() + randNum);
    }
}

void DungeonGainConfig::GetRewardTable(const DungeonGainTemplate* gainTmpl, int32 num, REWARD_TABLE& rewardTable)
{
    std::vector<REWARD_ITEM> gainReward;
    REWARD_ITEM rewardItem;
    uint32 randNum = 0;

    if(!gainTmpl) return;

    // 赋值装备
    gainReward = gainTmpl->gain_items;

    // 奖励经验
    if(gainTmpl->exp > 0)
    {
        rewardItem.Clear();
        rewardItem.id = PROP_EXP;
        rewardItem.num = gainTmpl->exp;
        rewardTable.items.push_back(rewardItem);
    }

    // 奖励游戏币
    if(gainTmpl->game_point > 0)
    {
        rewardItem.Clear();
        rewardItem.id = PROP_GAMEPOINT;
        rewardItem.num = gainTmpl->game_point;
        rewardTable.items.push_back(rewardItem);
    }

    // 奖励精魂
    if(gainTmpl->soul > 0)
    {
        rewardItem.Clear();
        rewardItem.id = PROP_SOULPOINT;
        rewardItem.num = gainTmpl->soul;
        rewardTable.items.push_back(rewardItem);
    }

    // 计算给予的物品
    for(int32 i=0; (i < num) && (gainReward.size() > 0); ++i)
    {
        randNum = (uint32)rand();
        randNum = randNum % gainReward.size();
        rewardTable.items.push_back(gainReward[randNum]);
        gainReward.erase(gainReward.begin() + randNum);
    }

}

void DungeonGainConfig::GetRewardTableIncludeExtra( const DungeonGainTemplate* gainTmpl, int32 num, uint32 score, uint32 vipLevel, REWARD_TABLE& rewardTable )
{
    if(!gainTmpl) 
        return;

    real extraExpPercent = 0;
    real extraSoulPercent = 0;
    real extraGamePointPercent = 0;
    // 获取额外奖励百分比
    GetExtraRewardPercent(score, vipLevel, extraExpPercent, extraSoulPercent, extraGamePointPercent);

    REWARD_ITEM rewardItem;

    // 奖励经验
    if(gainTmpl->exp > 0)
    {
        rewardItem.Clear();
        rewardItem.id = PROP_EXP;
        rewardItem.num = (int32)(gainTmpl->exp * (1 + extraExpPercent)*num);
        rewardTable.items.push_back(rewardItem);
    }

    // 奖励游戏币
    if(gainTmpl->game_point > 0)
    {
        rewardItem.Clear();
        rewardItem.id = PROP_GAMEPOINT;
        rewardItem.num = (int32)(gainTmpl->game_point * (1 + extraGamePointPercent)*num);
        rewardTable.items.push_back(rewardItem);
    }

    // 奖励精魂
    if(gainTmpl->soul > 0)
    {
        rewardItem.Clear();
        rewardItem.id = PROP_SOULPOINT;
        rewardItem.num = (int32)(gainTmpl->soul * (1 + extraSoulPercent)*num);
        rewardTable.items.push_back(rewardItem);
    }

    // 计算给予的物品
    uint32 randNum = 0;
    std::vector<REWARD_ITEM> gainReward = gainTmpl->gain_items;
    for(int32 i=0; (i < num) && (gainReward.size() > 0); ++i)
    {
        randNum = Crown::SDRandom(gainReward.size());
        rewardTable.items.push_back(gainReward[randNum]);
        gainReward.erase(gainReward.begin() + randNum);
    }

}


void DungeonGainConfig::GetExtraRewardPercent( uint32 score, uint32 vipLevel, real& extraExpPercent, real& extraSoulPercent, real& extraGamePointPercent )
{
    // 副本评价额外奖励
    real scoreExtraExpPercent = 0;
    real scoreExtraSoulPercent = 0;
    real scoreExtraGamePointPercent = 0;
    //real scoreExtraRewardPercent = DungeonScoreConfig::Instance()->GetExtraRewardPercentByScore(score) / 100.0f;
    //if (scoreExtraRewardPercent > 0)
    //{
    //    scoreExtraExpPercent = scoreExtraRewardPercent;
    //    scoreExtraSoulPercent = scoreExtraRewardPercent;
    //    scoreExtraGamePointPercent = scoreExtraRewardPercent;
    //}

    // vip额外奖励
    real vipExtraExpPercent = 0;
    real vipExtraSoulPercent = 0;
    real vipExtraGamePointPercent = 0;


    // 增益效果额外奖励(双倍卡等)
    real otherExtraExpPercent = 0;
    real otherExtraSoulPercent = 0;
    real otherExtraGamePointPercent = 0;

    extraExpPercent = scoreExtraExpPercent + scoreExtraExpPercent + scoreExtraExpPercent;
    extraSoulPercent = scoreExtraSoulPercent + vipExtraSoulPercent + otherExtraSoulPercent;
    extraGamePointPercent = scoreExtraGamePointPercent + vipExtraGamePointPercent + otherExtraGamePointPercent;
}