#include "gameholder_pch.h"
#include "vip_gift_config.h"
#include "player.h"
#include "item_manager.h"

IMPLEMENT_SINGLETON(VipGiftConfig)
VipGiftConfig::VipGiftConfig(void)
{
}

VipGiftConfig::~VipGiftConfig(void)
{
}

void VipGiftConfig::Clear()
{

}

bool VipGiftConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;
    int32 level = 0;
    int32 vigor = 0;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load vipgift file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    for(TiXmlElement* xmlEle = xmlRoot->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        uint32 viplevel = 0;
        xmlEle->QueryUnsignedAttribute("vip_level", &viplevel);
        std::string base_value = xmlEle->Attribute("rewards");

        REWARD_TABLE rewardTable;
        rewardTable.reason = REWARD_REASON_VIP_GIFT;
        std::vector<std::string> values = Crown::SDSplitStrings(base_value, ' ');
        for(uint32 i=0; i<values.size(); ++i)
        {
            std::vector<std::string> key_value = Crown::SDSplitStrings(values[i], '-');

            REWARD_ITEM rewarditem;
            rewarditem.Clear();
            if(key_value.size() == 3)
            {
                //rewarditem.type = SDAtoi(key_value[0].c_str());
                rewarditem.id = SDAtoi(key_value[1].c_str());
                rewarditem.num = SDAtoi(key_value[2].c_str());
            }
            rewardTable.items.push_back(rewarditem);
        }

        if(rewardTable.items.size() <=0)
            CnWarn("have no rewarditem in level%u \n", viplevel);

        if(viplevel != 0)
        {
            if(vip_git_list.find(viplevel) == vip_git_list.end())
            {
                vip_git_list[viplevel] = rewardTable;
            }
            else
            {
                CnWarn("Duplicate viplevel: %u\n", viplevel);
            }
        }
    }
    return true;
}
int32 VipGiftConfig::GetRewardItemList(int32 level)
{
    std::map<uint32, REWARD_TABLE>::iterator listiter;
    listiter = vip_git_list.find(level);
    if(listiter != vip_git_list.end())
        return listiter->second.items.size();

    return 0;
}
bool VipGiftConfig::AddVipGift(Player* player, int32 level)
{
    std::map<uint32, REWARD_TABLE>::iterator listiter;
    listiter = vip_git_list.find(level);
    if(listiter != vip_git_list.end())
    {
        //ItemManager::AddRewardTableWithEmail(player->GetCenterSvrID(), player->GetPlayerID(), listiter->second);
        return true;
    }

    return false;
}