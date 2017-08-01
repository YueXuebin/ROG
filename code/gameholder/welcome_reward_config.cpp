#include "gameholder_pch.h"
#include "welcome_reward_config.h"
#include "config_reader.h"


IMPLEMENT_SINGLETON(WelcomeRewardConfig)

WelcomeRewardConfig::WelcomeRewardConfig(void)
{
}

WelcomeRewardConfig::~WelcomeRewardConfig(void)
{
}

REWARD_TABLE& WelcomeRewardConfig::GetRewadTable()
{
    return rewardTable;
}

bool WelcomeRewardConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    rewardTable.Clear();

    REWARD_ITEM rewardItem;

    rewardTable.reason = REWARD_REASON_WELCOME_REWARD;

    for(TiXmlElement* itEle = xmlRoot->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
    {
       // const char* vv = itEle->GetText();
        if(!ConfigReader::LoadRewardItem(itEle, rewardItem))
        {
            continue;
        }

        rewardTable.items.push_back(rewardItem);
    }

    return true;
}
