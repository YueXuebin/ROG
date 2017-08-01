#include "commonlib_pch.h"
#include "config_reader.h"

#define CHECK_ATTRIBUTE()       \
    if(!node->Attribute(key))   \
    {                           \
        if(errCount)            \
            ++*errCount;        \
        CnWarn("Can't find '%s' attribute '%s' \n", node->Value(), key);    \
        return false;                                                       \
    }

bool ConfigReader::Read(const TiXmlElement* node, const char* key, int8& val, int32* errCount)
{
    if(!node || !key)
        return false;

    CHECK_ATTRIBUTE()

    val = (int8)atoi(node->Attribute(key));
    return true;
}

bool ConfigReader::Read(const TiXmlElement* node, const char* key, uint8& val, int32* errCount)
{
    if(!node || !key)
        return false;

    CHECK_ATTRIBUTE()

    val = (uint8)strtoul(node->Attribute(key), NULL, 10);
    return true;
}

bool ConfigReader::Read(const TiXmlElement* node, const char* key, int32& val, int32* errCount)
{
    if(!node || !key)
        return false;

    CHECK_ATTRIBUTE()

    val = (int32)atoi(node->Attribute(key));
    return true;
}

bool ConfigReader::Read(const TiXmlElement* node, const char* key, uint32& val, int32* errCount)
{
    if(!node || !key)
        return false;

    CHECK_ATTRIBUTE()

    val = (uint32)strtoul(node->Attribute(key), NULL, 10);
    return true;
}

bool ConfigReader::Read(const TiXmlElement* node, const char* key, std::string& val, int32* errCount)
{
    if(!node || !key)
        return false;

    CHECK_ATTRIBUTE()

    val = node->Attribute(key);
    return true;
}

Data3 ConfigReader::parseData3(const std::string& str)
{
    Data3 data3;
    data3.value1 = data3.value2 = data3.value3 = 0;

    if(str.length() == 0)
    {
        CnAssert(false);
        return data3;
    }

    std::vector<std::string> key_value = Crown::SDSplitStrings(str, '-');

    if(key_value.size() == 2)
    {
        data3.value1 = SDAtoi(key_value[0].c_str());
        data3.value2 = SDAtoi(key_value[1].c_str());
    }
    else if(key_value.size() == 3)
    {
        data3.value1 = SDAtoi(key_value[0].c_str());
        data3.value2 = SDAtoi(key_value[1].c_str());
        data3.value3 = SDAtoi(key_value[2].c_str());
    }
    else
        CnAssert(false);            // 只支持2个和3个数字

    return data3;
}

std::vector<Data3> ConfigReader::parseData3List(const std::string& str, char delim)
{
    std::vector<Data3> data3_list;

    std::vector<std::string> values = Crown::SDSplitStrings(str, delim);

    for(uint32 i=0; i<values.size(); ++i)
    {
        Data3 data3 = parseData3(values[i]);
        data3_list.push_back(data3);
    }
    return data3_list;
}

std::vector<int32> ConfigReader::parseIntList(const std::string& str, char delim)
{
    std::vector<int32> int_list;
    std::vector<std::string> values = Crown::SDSplitStrings(str, delim);
    for(uint32 i=0; i<values.size(); ++i)
    {
        int32 data = SDAtoi(values[i].c_str());
        int_list.push_back(data);
    }
    return int_list;
}

REWARD_ITEM ConfigReader::parseRewardItem(const Data3& data)
{
    REWARD_ITEM rewardItem;

    if(data.value3)
    {
        //rewardItem.type     = data.value1;
        rewardItem.id       = data.value1;
        rewardItem.num      = data.value2;
    }
    else
    {
        //rewardItem.type     = 0;
        rewardItem.id       = data.value1;
        rewardItem.num      = data.value2;
    }

    CnAssert(rewardItem.id);        // id和num不应该为0
    CnAssert(rewardItem.num);

    return rewardItem;
}

REWARD_ITEM ConfigReader::parseRewardItem(const std::string& str)
{
    return parseRewardItem(parseData3(str));
}

std::vector<REWARD_ITEM> ConfigReader::parseRewardItemList(const std::string& str, char delim)
{
    std::vector<REWARD_ITEM> rewardItemList;

    std::vector<Data3> data3_list = parseData3List(str, delim);
    for(uint32 i=0; i<data3_list.size(); i++)
    {
        REWARD_ITEM rewardItem = parseRewardItem(data3_list[i]);

        rewardItemList.push_back(rewardItem);
    }
    return rewardItemList;
}

bool ConfigReader::LoadRewardItem(TiXmlElement* itEle, REWARD_ITEM& reward_item)
{
    
    if(!itEle) return false;

    if(itEle->Attribute("type") == NULL)
    {
        CnAssert(false);
        return false;
    }

    return true;

    //reward_item.type = SDAtoi(itEle->Attribute("type"));

    //switch(reward_item.type)
    //{
    //case PROP_GAMEPOINT:
    //case PROP_SOULPOINT:
    //case PROP_EXP:
    //case PROP_MONEYPOINT:
    //    //case PROP_GUILDPOINT:
    //case PROP_HONORPOINT:
    //case PROP_BIND_MONEYPOINT:
    //case PROP_UNLOCK_MONSTER_DOOR_BOSS:
    //case PROP_TITLE:
    //    {
    //        reward_item.num = Crown::SDAtou(itEle->Attribute("num"));
    //    }
    //    break;
    //case PROP_ITEM:
    //    {
    //        reward_item.id = Crown::SDAtou(itEle->Attribute("item_id"));
    //        reward_item.num = Crown::SDAtou(itEle->Attribute("num"));
    //    }
    //    break;
    //    //     case PROP_CAREER_EQUIP:
    //    //         {
    //    //             reward_item.id = Crown::SDAtou(itEle->Attribute("career"));
    //    //             reward_item.num = Crown::SDAtou(itEle->Attribute("equip_id"));
    //    //         }
    //    //         break;
    //case PROP_RIDE_PET:
    //    {
    //        if(itEle->QueryUnsignedAttribute("ride_pet_id", &reward_item.id) == TIXML_NO_ATTRIBUTE)
    //        {
    //            CnWarn("have not ride_pet_id in xml");
    //            return false;
    //        }
    //    }
    //    break;
    //case  PROP_FOLLOW_PET:
    //    {
    //        if(itEle->QueryUnsignedAttribute("follow_pet_id", &reward_item.id) == TIXML_NO_ATTRIBUTE)
    //        {
    //            CnWarn("have not follow_pet_id in xml");
    //            return false;
    //        }
    //    }
    //    break; 
    //default:
    //    break;
    //}

    return true;
}
