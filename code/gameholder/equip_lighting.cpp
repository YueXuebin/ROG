#include "gameholder_pch.h"
#include "equip_lighting.h"
#include "config_reader.h"

IMPLEMENT_SINGLETON(EquipLightingConfig)

EquipLightingConfig::EquipLightingConfig(void)
{
}

EquipLightingConfig::~EquipLightingConfig(void)
{
}
// 分数比较函数
bool SortConfigRules(const EquipLightingItem& left, const EquipLightingItem& right) 
{
    return (left.level > right.level);
}

bool EquipLightingConfig::LoadConfig( const char* path )
{
    TiXmlDocument xmlDoc;

    if(!xmlDoc.LoadFile(path))
    {
        CnError("Load equip_lighting file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();

    if(!xmlRoot)
        return false;

    EquipLightingItem item;
    std::string  strvalue = "";
    for(TiXmlElement* careerEle = xmlRoot->FirstChildElement(); careerEle; careerEle = careerEle->NextSiblingElement())
    {
        if(strcmp(careerEle->Value(), "star") == 0 || strcmp(careerEle->Value(), "quality") == 0 || strcmp(careerEle->Value(), "jewel") == 0)
        {
            for(TiXmlElement* starEle = careerEle->FirstChildElement(); starEle; starEle = starEle->NextSiblingElement())
            {
                item.Clear();
                item.level = SDAtoi(starEle->Attribute("level"));
                strvalue = starEle->Attribute("addpro");
                LoadProList(strvalue, &item.addProlist);
                if(strcmp(careerEle->Value(), "jewel") == 0)
                {
                    ConfigReader::Read(starEle, "num", item.num);
                    jewellist.push_back(item);
                }
                else if(strcmp(careerEle->Value(), "star") == 0)
                {
                    starlist.push_back(item);
                }
                else if(strcmp(careerEle->Value(), "quality") == 0)
                {
                    qualitylist.push_back(item);
                }
            }
        }
    }
    std::sort(starlist.begin(), starlist.end(), SortConfigRules);
    std::sort(qualitylist.begin(), qualitylist.end(), SortConfigRules);
    std::sort(jewellist.begin(), jewellist.end(), SortConfigRules);
    return true;
}

const EquipLightingItem* EquipLightingConfig::GetEquipStarLisghtingConfig( int32 starNum )
{
    for(StarList::iterator item = starlist.begin(); item != starlist.end(); item++)
    {
        if(starNum >= item->level)
        {
            return &(*item);
        }
    }
    return NULL;
}

const EquipLightingItem* EquipLightingConfig::GetEquipQuelityLisghtingConfig( int32 level )
{
    for(QualityList::iterator item = qualitylist.begin(); item != qualitylist.end(); item++)
    {
        if(level >= item->level)
        {
            return &(*item);
        }
    }
    return NULL;
}

const EquipLightingItem* EquipLightingConfig::GetEquipJewelLisghtingConfig( int32 jewelLevel )
{
    for(JewelList::iterator item = jewellist.begin(); item != jewellist.end(); item++)
    {
        if(jewelLevel >= item->level)
        {
            return &(*item);
        }
    }
    return NULL;
}

//========================================================================
//解析属性列表
//========================================================================
void EquipLightingConfig::LoadProList(std::string base_value, PropertyList* list)
{
    std::vector<std::string> values = Crown::SDSplitStrings(base_value, ' ');
    for(uint32 i=0; i<values.size(); ++i)
    {
        std::vector<std::string> key_value = Crown::SDSplitStrings(values[i], '-');

        Property curproperty;
        curproperty.Clear();
        if(key_value.size() == 2)
        {
            curproperty.type = SDAtoi(key_value[0].c_str());
            curproperty.value= SDAtoi(key_value[1].c_str());
        }
        list->push_back(curproperty);
    }
}
