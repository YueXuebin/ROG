#include "gameholder_pch.h"
#include "package_config.h"

IMPLEMENT_SINGLETON(PackageConfig)

PackageConfig::PackageConfig(void)
{
}

PackageConfig::~PackageConfig(void)
{
}

// 分数比较函数
bool SortConfigPackageItem(const TREASURE_PACKAGE_ITEM& left, const TREASURE_PACKAGE_ITEM& right) 
{
    return (left.item_probability > right.item_probability);
}


bool PackageConfig::LoadConfig( const char* path )
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, FASHIONS_TREASURE_PACKAGE_FILE);
    if(!LoadTreasurePackage(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    return true;
}

const TREASURE_PACKAGE_INFO* PackageConfig::GetTreasurePackage( int32 id )
{
    if(m_treasurePackage.find(id) == m_treasurePackage.end())
    {
        return NULL;
    }
    return &m_treasurePackage[id];
}


bool PackageConfig::LoadTreasurePackage( const char* path )
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
    int total = 0;
    int num = 0;
    m_treasurePackage.clear();

    std::string  strvalue = "";
    std::vector<std::string> key_value;
    for(TiXmlElement* tableEle = xmlRoot->FirstChildElement(); tableEle; tableEle = tableEle->NextSiblingElement())
    {

        TREASURE_PACKAGE_INFO package;
        package.Clear();
        total = 0;
        package.package_id = SDAtoi(tableEle->Attribute("package_id"));
        package.type_limit = SDAtoi(tableEle->Attribute("type_limit"));
        package.extract = 0;
        package.times = 0;

        for (TiXmlElement* treaEle = tableEle->FirstChildElement(); treaEle; treaEle = treaEle->NextSiblingElement())
        {
            TREASURE_PACKAGE_ITEM item;
            item.Clear();

            item.count_max = SDAtoi(treaEle->Attribute("count_max"));
            item.count_min = SDAtoi(treaEle->Attribute("count_min"));
            item.item_id = SDAtoi(treaEle->Attribute("item_id"));
            item.item_probability = SDAtoi(treaEle->Attribute("item_probability"));
            item.notice = SDAtoi(treaEle->Attribute("notice"));

            item.extract = SDAtoi(treaEle->Attribute("extract"));
            item.wrap = SDAtoi(treaEle->Attribute("wrap"));
            item.times = SDAtoi(treaEle->Attribute("times"));

            strvalue = treaEle->Attribute("per_cd");
            item.per_cd = GetTimeUint(strvalue);
            strvalue = treaEle->Attribute("ser_cd");
            item.ser_cd = GetTimeUint(strvalue);

            if(item.extract == 1)
            {
                package.extract = item.extract;
                package.times = item.times;
            }
            total += item.item_probability;
            package.item_list.push_back(item);
        }
        package.total_probability = total; 

        num = 0;
        std::sort(package.item_list.begin(), package.item_list.end(), SortConfigPackageItem);
        for (std::vector<TREASURE_PACKAGE_ITEM>::iterator it = package.item_list.begin(); it != package.item_list.end(); it++)
        {
            num += it->item_probability;
            it->all_probability = (uint32)(((float)num / total) * BASE_RATE_NUM);
        }
        m_treasurePackage[package.package_id] = package;
    }
    return true;
}


uint32 PackageConfig::GetTimeUint(const std::string& base_value )
{
    std::vector<std::string> values = Crown::SDSplitStrings(base_value, ':');
    if(values.size() == 4)
    {
        return SDAtoi(values[0].c_str())*24*3600 + SDAtoi(values[1].c_str())*3600 + SDAtoi(values[2].c_str())*60 + SDAtoi(values[3].c_str());
    }
    else if(values.size() == 3)
    {
        return SDAtoi(values[0].c_str())*3600 + SDAtoi(values[1].c_str())*60 + SDAtoi(values[2].c_str());
    }
    else if(values.size() == 2)
    {
        return SDAtoi(values[0].c_str())*60 + SDAtoi(values[1].c_str());
    }
    else if(values.size() == 1)
    {
        return SDAtoi(values[0].c_str());
    }
    else
    {
        return 0;
    }
}