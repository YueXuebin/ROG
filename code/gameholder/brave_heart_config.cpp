#include "gameholder_pch.h"
#include "brave_heart_config.h"
#include "game_util.h"

IMPLEMENT_SINGLETON(BraveHeartConfig)


BraveHeartConfig::BraveHeartConfig()
{

}

BraveHeartConfig::~BraveHeartConfig()
{

}

bool BraveHeartConfig::LoadAttrRedeems(const std::string& attrStr, RedeemTemplate& redeem)
{
    std::vector<std::string> attrStrTable = Crown::SDSplitStrings(attrStr, ' ');

    if(attrStrTable.size() <= 1)
        return false;

    for (std::vector<std::string>::iterator stringAttr = attrStrTable.begin(); stringAttr != attrStrTable.end(); ++stringAttr)
    {
        std::vector<std::string> numStrTable = Crown::SDSplitStrings(*stringAttr, '-');

        AttrTemplate attr;
        attr.Clear();
        attr.attr_value = SDAtoi(numStrTable[1].c_str());
        attr.heart_point = SDAtoi(numStrTable[2].c_str());

        redeem.attrs.push_back(attr);
        redeem.id = SDAtoi(numStrTable[0].c_str());
    }

    return true;
}

bool BraveHeartConfig::LoadConfig( const char* path )
{
    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load brave heart file: %s failed\n", path);
        return false;
    }
    
    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot)
    {
        return false;
    }

    for(TiXmlElement* tableEle = xmlRoot->FirstChildElement(); tableEle; tableEle = tableEle->NextSiblingElement())
    {
        if(strcmp(tableEle->Value(), "tiers") == 0)
        {
            TierTemplate tier;
            for(TiXmlElement* tierEle = tableEle->FirstChildElement(); tierEle; tierEle = tierEle->NextSiblingElement()) // tier
            {
                tier.Clear();

                tier.id = SDAtoi(tierEle->Attribute("id"));
                StageTemplate stage;
                for(TiXmlElement* stageEle = tierEle->FirstChildElement(); stageEle; stageEle = stageEle->NextSiblingElement()) // stage
                {
                    stage.Clear();

                    stage.id = SDAtoi(stageEle->Attribute("id"));
                    DiffTemplate diff;
                    for(TiXmlElement* diffEle = stageEle->FirstChildElement(); diffEle; diffEle = diffEle->NextSiblingElement()) // diff
                    {
                        diff.Clear();

                        diff.id = SDAtoi(diffEle->Attribute("id"));
                        diff.dungeon_id = SDAtoi(diffEle->Attribute("dungeon_id"));
                        diff.boss_id = SDAtoi(diffEle->Attribute("boss_id"));
                        diff.heart_point = SDAtoi(diffEle->Attribute("heart_point"));

                        stage.diffs.push_back(diff);
                    }
                    // attrs
                    std::string attr0 = stageEle->Attribute("attr0");
                    RedeemTemplate redeem0;
                    LoadAttrRedeems(attr0, redeem0);
                    stage.redeems.push_back(redeem0);

                    std::string attr1 = stageEle->Attribute("attr1");
                    RedeemTemplate redeem1;
                    LoadAttrRedeems(attr1, redeem1);
                    stage.redeems.push_back(redeem1);

                    std::string attr2 = stageEle->Attribute("attr2");
                    RedeemTemplate redeem2;
                    LoadAttrRedeems(attr2, redeem2);
                    stage.redeems.push_back(redeem2);

                    std::string attr3 = stageEle->Attribute("attr3");
                    RedeemTemplate redeem3;
                    LoadAttrRedeems(attr3, redeem3);
                    stage.redeems.push_back(redeem3);

                    std::string attr4 = stageEle->Attribute("attr4");
                    RedeemTemplate redeem4;
                    LoadAttrRedeems(attr4, redeem4);
                    stage.redeems.push_back(redeem4);

                    std::string attr5 = stageEle->Attribute("attr5");
                    RedeemTemplate redeem5;
                    LoadAttrRedeems(attr5, redeem5);
                    stage.redeems.push_back(redeem5);

                    std::string attr6 = stageEle->Attribute("attr6");
                    RedeemTemplate redeem6;
                    LoadAttrRedeems(attr6, redeem6);
                    stage.redeems.push_back(redeem6);

                    std::string attr7 = stageEle->Attribute("attr7");
                    RedeemTemplate redeem7;
                    LoadAttrRedeems(attr7, redeem7);
                    stage.redeems.push_back(redeem7);

                    std::string attr8 = stageEle->Attribute("attr8");
                    RedeemTemplate redeem8;
                    LoadAttrRedeems(attr8, redeem8);
                    stage.redeems.push_back(redeem8);

                    tier.stages.push_back(stage);
                }

                m_tierList.push_back(tier);
            }
        }
        else if(strcmp(tableEle->Value(), "rewards") == 0)
        {
            TierRewardTemplate tierReward;
            for(TiXmlElement* tierEle = tableEle->FirstChildElement(); tierEle; tierEle = tierEle->NextSiblingElement()) // tier
            {
                tierReward.Clear();

                tierReward.id = SDAtoi(tierEle->Attribute("id"));
                RewardTemplate reward;
                for(TiXmlElement* rewardEle = tierEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement()) // reward
                {
                    reward.Clear();

                    reward.heart_point = SDAtoi(rewardEle->Attribute("heart_point"));
                    std::string gains = rewardEle->Attribute("gains");
                    REWARD_TABLE rewardTable;
                    if(!LoadRewardTable(gains, rewardTable))
                    {
                        CnWarn("Unknow rewards: tier:%u heart_point:%u gains\n", tierReward.id, reward.heart_point);
                        return false;
                    }
                    reward.gain_items = rewardTable.items;

                    tierReward.rewardList.push_back(reward);
                }

                m_rewardList.push_back(tierReward);
            }
        }
        else if(strcmp(tableEle->Value(), "count") == 0)
        {
            for(TiXmlElement* countEle = tableEle->FirstChildElement(); countEle; countEle = countEle->NextSiblingElement())
            {
                CountInfoTemplate countInfo;
                countInfo.num = SDAtoi(countEle->Attribute("num"));
                countInfo.money = SDAtoi(countEle->Attribute("money"));
                countInfo.card = SDAtoi(countEle->Attribute("card"));
                m_countList.push_back(countInfo);
            }
        }
    }

    return true;
}

int32 BraveHeartConfig::GetDungeonId(int32 mTierId, int32 mStageId, int32 mDiff)
{
    for (TierList::iterator tier = m_tierList.begin(); tier != m_tierList.end(); tier++)
    {
        if (tier->id == mTierId)
        {
            for (std::vector<StageTemplate>::iterator stage = tier->stages.begin(); stage != tier->stages.end(); stage++)
            {
                if (stage->id == mStageId)
                {
                    for (std::vector<DiffTemplate>::iterator diff = stage->diffs.begin(); diff != stage->diffs.end(); diff++)
                    {
                        if (diff->id == mDiff)
                        {
                            return diff->dungeon_id;
                        }
                    }
                }
            }
        }
    }

    return 0;
}

int32 BraveHeartConfig::GetHeartPoint(int32 mTierId, int32 mStageId, int32 mDiff)
{
    for (TierList::iterator tier = m_tierList.begin(); tier != m_tierList.end(); tier++)
    {
        if (tier->id == mTierId)
        {
            for (std::vector<StageTemplate>::iterator stage = tier->stages.begin(); stage != tier->stages.end(); stage++)
            {
                if (stage->id == mStageId)
                {
                    for (std::vector<DiffTemplate>::iterator diff = stage->diffs.begin(); diff != stage->diffs.end(); diff++)
                    {
                        if (diff->id == mDiff)
                        {
                            return diff->heart_point;
                        }
                    }
                }
            }
        }
    }

    return 0;
}

int32 BraveHeartConfig::GetRedeemAttrValue(int32 mTierId, int32 mStageId, int32 mAttrId, int32 mHeartPointCost)
{
    for (TierList::iterator tier = m_tierList.begin(); tier != m_tierList.end(); tier++)
    {
        if (tier->id == mTierId)
        {
            for (std::vector<StageTemplate>::iterator stage = tier->stages.begin(); stage != tier->stages.end(); stage++)
            {
                if (stage->id == mStageId)
                {
                    for (std::vector<RedeemTemplate>::iterator redeem = stage->redeems.begin(); redeem != stage->redeems.end(); redeem++)
                    {
                        if (redeem->id == mAttrId)
                        {
                            for (std::vector<AttrTemplate>::iterator attr = redeem->attrs.begin(); attr != redeem->attrs.end(); attr++)
                            {
                                if (attr->heart_point == mHeartPointCost)
                                    return attr->attr_value;
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}

void BraveHeartConfig::GetRewardTable(int32 mTierId, int32 mHeartPointSum, REWARD_TABLE& rewardTable)
{
    for (TierRewardList::iterator tier = m_rewardList.begin(); tier != m_rewardList.end(); tier++)
    {
        if (tier->id == mTierId)
        {
            for (std::vector<RewardTemplate>::iterator reward = tier->rewardList.begin(); reward != tier->rewardList.end(); reward++)
            {
                if (mHeartPointSum >= reward->heart_point) // fill reward table
                {
                    for (std::vector<REWARD_ITEM>::iterator item = reward->gain_items.begin(); item != reward->gain_items.end(); item++)
                    rewardTable.items.push_back(*item);
                }
            }
        }
    }
}

const CountInfoTemplate* BraveHeartConfig::GetCountInfo( int32 num )
{
    if(m_countList.size() < 1)
        return NULL;

    std::vector<CountInfoTemplate>::iterator it = m_countList.begin();
    for (it; it != m_countList.end(); it++)
    {
        if(it->num == num)
            return &(*it);
    }

    it = m_countList.end() - 1;
    return &(*it);
}
