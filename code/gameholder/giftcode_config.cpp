#include "gameholder_pch.h"
#include "giftcode_config.h"
#include "config_reader.h"


IMPLEMENT_SINGLETON(GiftCodeConfig)

GiftCodeConfig::GiftCodeConfig()
{

}

GiftCodeConfig::~GiftCodeConfig()
{

}


bool GiftCodeConfig::LoadConfig(const char *fileName)
{
    TiXmlDocument xmlDoc;
    REWARD_ITEM item;

    if(!xmlDoc.LoadFile(fileName))
    {
        CnError("Load file: %s failed\n", fileName);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    for (TiXmlElement *pElement = xmlRoot->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
    {
        if (Crown::SDStrcmp(pElement->Value(), "reward") == 0)
        {
            for (TiXmlElement *pGiftsCodeElem = pElement->FirstChildElement(); pGiftsCodeElem; pGiftsCodeElem = pGiftsCodeElem->NextSiblingElement())
            {
                if (Crown::SDStrcmp(pGiftsCodeElem->Value(), "gift_code") == 0)
                {
                    uint32 giftsCodeType;
                    ConfigReader::Read(pGiftsCodeElem, "type", giftsCodeType);
                    for (TiXmlElement *pRewardElem = pGiftsCodeElem->FirstChildElement(); pRewardElem; pRewardElem = pRewardElem->NextSiblingElement())
                    {
                        item.Clear();
                        ConfigReader::Read(pRewardElem, "item_id", item.id);
                        ConfigReader::Read(pRewardElem, "num", item.num);
                        //ConfigReader::Read(pRewardElem, "type", item.type);
                        m_rewards[giftsCodeType].push_back(item);
                    }
                }
            }
        }
        else if (Crown::SDStrcmp(pElement->Value(), "max_gift_code_type") == 0)
        {
            ConfigReader::Read(pElement, "value", m_maxGiftCodeType);
        }
    }

    return true;
}

bool GiftCodeConfig::GetRewards(uint32 type, std::vector<REWARD_ITEM>& rewards)
{
    RewardMap::iterator iter = m_rewards.find(type);
    if (iter == m_rewards.end())
        return false;

    rewards = iter->second;
    return true;
}
