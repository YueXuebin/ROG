#include "gameholder_pch.h"
#include "shorcut_config.h"

IMPLEMENT_SINGLETON(ShorcutConfig)

ShorcutConfig::ShorcutConfig(void)
{
}

ShorcutConfig::~ShorcutConfig(void)
{
}

bool ShorcutConfig::LoadConfig( const char* path )
{
    char file_path[SD_MAX_PATH];

    SDSprintf(file_path, "%s%s", path, SHORCUT_CONFIG_FILE);
    if(!LoadShorcutConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    return true;
}


//=====================================================
//读取新的化身配置
//=====================================================
bool ShorcutConfig::LoadShorcutConfig(const char* path)
{
    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load npc file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    std::string  strvalue = "";
    REWARD_TABLE citem;
    for(TiXmlElement* tableEle = xmlRoot->FirstChildElement(); tableEle; tableEle = tableEle->NextSiblingElement())
    {
        if(strcmp(tableEle->Value(), "item") == 0)
        {
            citem.Clear();
            citem.reason_arg = SDAtoi(tableEle->Attribute("type"));

            strvalue = tableEle->Attribute("reward");
            LoadFashionItemList(&citem.items, strvalue);
            m_rewardList.push_back(citem);
        }
    }

    return true;
}


//========================================================================
//解析升级物品
//========================================================================
void ShorcutConfig::LoadFashionItemList(std::vector<REWARD_ITEM>* list, const std::string& base_value)
{
    std::vector<std::string> values = Crown::SDSplitStrings(base_value, ' ');
    for(uint32 i=0; i<values.size(); ++i)
    {
        std::vector<std::string> key_value = Crown::SDSplitStrings(values[i], '-');
        REWARD_ITEM curitem;
        curitem.Clear();
        if(key_value.size() == 3)
        {
            //curitem.type = SDAtoi(key_value[0].c_str());
            curitem.id = SDAtoi(key_value[1].c_str());
            curitem.num = SDAtoi(key_value[2].c_str());
        }
        else if(key_value.size() == 2)
        {
            //curitem.type = SDAtoi(key_value[0].c_str());
            curitem.id = SDAtoi(key_value[0].c_str());
            curitem.num = SDAtoi(key_value[1].c_str());
        }
        list->push_back(curitem);
    }
}


//========================================================
//获取奖励信息
//========================================================
const REWARD_TABLE* ShorcutConfig::GetRewardList( int32 type )
{
    for (ShorcutConfigList::iterator item = m_rewardList.begin(); item != m_rewardList.end(); item++)
    {
        if(item->reason_arg == type)
            return &(*item);
    }
    return NULL;
}