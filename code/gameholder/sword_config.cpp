#include "gameholder_pch.h"
#include "sword_config.h"
#include "config_reader.h"
#include "game_util.h"

IMPLEMENT_SINGLETON(SwordConfig);

SwordConfig::SwordConfig(void)
{
}

SwordConfig::~SwordConfig(void)
{
}

bool SwordConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;
    if(!xmlDoc.LoadFile(path))
    {
        CnError("Load sword file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    for(TiXmlElement* element = xmlRoot->FirstChildElement(); element; element = element->NextSiblingElement())
    {
        if (Crown::SDStrcmp(element->Value(), "sword_levelups") == 0)
        {
            if(!LoadSwordEquip(element))
                return false;
        }
    }

    return true;
}

const SwordEquipTmpl* SwordConfig::GetSwordTmpl(uint32 swordID)
{
    std::map<uint32, SwordEquipTmpl>::iterator it = m_sword.find(swordID);
    if(it == m_sword.end())
        return NULL;

    return &it->second;
}

const SwordLevelTmpl* SwordConfig::GetLevelTmpl(uint32 swordID, uint32 level)
{
    const SwordEquipTmpl* swordTmpl = GetSwordTmpl(swordID);
    if(!swordTmpl)
        return NULL;

    std::map<uint32, SwordLevelTmpl>::const_iterator levelTmpl = swordTmpl->level_list.find(level);
    if(levelTmpl == swordTmpl->level_list.end())
        return NULL;

    return &levelTmpl->second;
}

const SwordStarTmpl* SwordConfig::GetStarTmpl(uint32 swordID, uint32 level, uint32 star)
{
    const SwordLevelTmpl* levelTmpl = GetLevelTmpl(swordID, level);
    if(!levelTmpl)
        return NULL;

    std::map<uint32, SwordStarTmpl>::const_iterator starTmpl = levelTmpl->star_list.find(star);
    if(starTmpl == levelTmpl->star_list.end())
        return NULL;

    return &starTmpl->second;
}

int32 SwordConfig::GetMaxLevel(uint32 swordID)
{
    const SwordEquipTmpl* swordTmpl = GetSwordTmpl(swordID);
    if(!swordTmpl)
        return 0;

    return swordTmpl->level_list.rbegin()->first;
}

int32 SwordConfig::GetMaxStarTmpl(uint32 swordID, uint32 level)
{
    const SwordLevelTmpl* levelTmpl = GetLevelTmpl(swordID, level);
    if(!levelTmpl)
        return 0;

    return levelTmpl->star_list.rbegin()->first;
}

//加载基本信息
bool SwordConfig::LoadSwordEquip(TiXmlElement* swordEquipEle)
{
    SwordEquipTmpl  equipTmpl;
    SwordLevelTmpl  levelTmpl;
    SwordStarTmpl   starTmpl;

    std::string valueStr = "";

    for(TiXmlElement* swordEle = swordEquipEle->FirstChildElement(); swordEle; swordEle = swordEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(swordEle->Value(), "sword") == 0)
        {
            equipTmpl.Clear();
            ConfigReader::Read(swordEle, "id", equipTmpl.id);
            ConfigReader::Read(swordEle, "item_id", equipTmpl.item_id);

            for (TiXmlElement* levelEle = swordEle->FirstChildElement(); levelEle; levelEle = levelEle->NextSiblingElement())
            {
                if(Crown::SDStrcmp(levelEle->Value(), "level") == 0)
                {
                    levelTmpl.Clear();
                    ConfigReader::Read(levelEle, "id", levelTmpl.level);

                    valueStr = "";
                    ConfigReader::Read(levelEle, "need_item", valueStr);
                    if(!LoadRewardTable(valueStr, levelTmpl.need_items))
                    {
                        CnError("Load sword config error! sword_id:%d, level_id:%d\n", equipTmpl.id, levelTmpl.level);
                        return false;
                    }

                    for (TiXmlElement* starEle = levelEle->FirstChildElement(); starEle; starEle = starEle->NextSiblingElement())
                    {
                        if(Crown::SDStrcmp(starEle->Value(), "star") == 0)
                        {
                            starTmpl.Clear();
                            ConfigReader::Read(starEle, "id", starTmpl.star);

                            valueStr = "";
                            ConfigReader::Read(starEle, "need_item", valueStr);
                            if(!LoadRewardTable(valueStr, starTmpl.need_items))
                            {
                                CnError("Load sword config error! sword_id:%d, level_id:%d, star_id:%d\n", equipTmpl.id, levelTmpl.level, starTmpl.star);
                                return false;
                            }

                            valueStr = "";
                            ConfigReader::Read(starEle, "prop_list", valueStr);
                            starTmpl.level_propety = GetPropListByString(valueStr);

                            levelTmpl.star_list[starTmpl.star] = starTmpl;
                        }
                    }

                    equipTmpl.level_list[levelTmpl.level] = levelTmpl;
                }
                else if (Crown::SDStrcmp(levelEle->Value(), "lock") == 0)
                {
                    int32 lockID = 0;
                    int32 moneypoint = 0;
                    ConfigReader::Read(levelEle, "id", lockID);
                    ConfigReader::Read(levelEle, "moneypoint", moneypoint);

                    equipTmpl.lock_list[lockID] = moneypoint;
                }
            }

            m_sword[equipTmpl.id] = equipTmpl;
        }
    }

    return true;
}
