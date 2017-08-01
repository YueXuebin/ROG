#include "gameholder_pch.h"
#include "skill_config_manager.h"
#include "game_util.h"


IMPLEMENT_SINGLETON(SkillConfigManager)

SkillConfigManager::SkillConfigManager()
{
    SKILL_ENTRY::CreateInstance();
    BUFF_ENTRY::CreateInstance();
    SKILL_LEVELUP_ENTRY::CreateInstance();
    RUNE_ENTRY::CreateInstance();
}

SkillConfigManager::~SkillConfigManager()
{
    SKILL_ENTRY::DestroyInstance();
    BUFF_ENTRY::DestroyInstance();
    SKILL_LEVELUP_ENTRY::DestroyInstance();
    RUNE_ENTRY::DestroyInstance();
}

bool SkillConfigManager::LoadConfig(const char* path)
{
    if(SKILL_ENTRY::Instance()->LoadConfig(path))
    {
        for(std::vector<SKILL_ROOT_SKILL_STRUCT>::const_iterator iter = SKILL_ENTRY::Instance()->GetStruct().skill_list.begin();
            iter != SKILL_ENTRY::Instance()->GetStruct().skill_list.end(); iter++)
        {
            m_SkillConfigMap[iter->id] = &(*iter);
        }
    }

    if(BUFF_ENTRY::Instance()->LoadConfig(path))
    {
        for(std::vector<BUFF_ROOT_BUFF_STRUCT>::const_iterator iter = BUFF_ENTRY::Instance()->GetStruct().buff_list.begin();
            iter != BUFF_ENTRY::Instance()->GetStruct().buff_list.end(); iter++)
        {
            m_BuffConfigMap[iter->id] = &(*iter);
        }
    }

    if(SKILL_LEVELUP_ENTRY::Instance()->LoadConfig(path))
    {
        for(std::vector<SKILL_LEVELUP_ROOT_SKILL_VALUE_STRUCT>::const_iterator iter = SKILL_LEVELUP_ENTRY::Instance()->GetStruct().skill_value_list.begin();
            iter != SKILL_LEVELUP_ENTRY::Instance()->GetStruct().skill_value_list.end(); iter++)
        {
            m_SkillLevelUpConfigMap[iter->level] = &(*iter);
        }
    }

    if(RUNE_ENTRY::Instance()->LoadConfig(path))
    {
        for(std::vector<RUNE_ROOT_RUNE_STRUCT>::const_iterator iter = RUNE_ENTRY::Instance()->GetStruct().rune_list.begin();
            iter != RUNE_ENTRY::Instance()->GetStruct().rune_list.end(); iter++)
        {
            m_RuneConfigMap[iter->id] = &(*iter);
        }
    }

    return true;
}

const SKILL_ROOT_SKILL_STRUCT* SkillConfigManager::GetSkillConfigData(uint32 skillID)
{
    std::map<uint32, const SKILL_ROOT_SKILL_STRUCT*>::const_iterator iter = m_SkillConfigMap.find(skillID);
    if(iter != m_SkillConfigMap.end())
        return iter->second;
    else
    {
        CnWarn("skill %d config data not exist\n", skillID);
        return NULL;
    }
}

const BUFF_ROOT_BUFF_STRUCT* SkillConfigManager::GetBuffConfigData(uint32 buffID)
{
    std::map<uint32, const BUFF_ROOT_BUFF_STRUCT*>::const_iterator iter = m_BuffConfigMap.find(buffID);
    if(iter != m_BuffConfigMap.end())
        return iter->second;
    else
    {
        CnWarn("buff %d config data not exist\n", buffID);
        return NULL;
    }
}

const RUNE_ROOT_RUNE_STRUCT* SkillConfigManager::GetRuneConfigData(uint32 runeID)
{
    std::map<uint32, const RUNE_ROOT_RUNE_STRUCT*>::const_iterator iter = m_RuneConfigMap.find(runeID);
    if(iter != m_RuneConfigMap.end())
        return iter->second;
    else
    {
        CnWarn("rune %d config data not exist\n", runeID);
        return NULL;
    }
}

int32 SkillConfigManager::GetSkillMinLevel()
{
    if (m_SkillLevelUpConfigMap.empty())
    {
        CnAssert(false);
        return 0;
    }

    return m_SkillLevelUpConfigMap.begin()->first;
}

int32 SkillConfigManager::GetSkillMaxLevel()
{
    if (m_SkillLevelUpConfigMap.empty())
    {
        CnAssert(false);
        return 0;
    }

    return m_SkillLevelUpConfigMap.rbegin()->first;
}

uint32 SkillConfigManager::GetSkillGrowByLevelQuality(uint32 level, uint32 quality, int32* result_exp)
{
    std::map<uint32, const SKILL_LEVELUP_ROOT_SKILL_VALUE_STRUCT*>::const_iterator iter = m_SkillLevelUpConfigMap.find(level);
    if(iter == m_SkillLevelUpConfigMap.end())
    {
        CnAssert(false);
        return 0;
    }
    const SKILL_LEVELUP_ROOT_SKILL_VALUE_STRUCT* pLevelUpData = iter->second;
    CnAssert(pLevelUpData);

    if(quality >= MAX_EQUIP_QUALITY)
    {
        CnAssert(false);
        return 0;
    }

    int32 grow = 0;
    if(result_exp)
        *result_exp = 0;
    
    switch(quality)
    {
    case QUALITY_WHITE:
        grow = pLevelUpData->white;
        if(result_exp)
            *result_exp = pLevelUpData->white_exp;
        break;
    case QUALITY_GREEN:
        grow = pLevelUpData->green;
        if(result_exp)
            *result_exp = pLevelUpData->green_exp;
        break;
    case QUALITY_BLUE:
        grow = pLevelUpData->blue;
        if(result_exp)
            *result_exp = pLevelUpData->blue_exp;
        break;
    case QUALITY_PURPLE:
        grow = pLevelUpData->purple;
        if(result_exp)
            *result_exp = pLevelUpData->purple_exp;
        break;
    case QUALITY_GOLD:
        grow = pLevelUpData->gold;
        if(result_exp)
            *result_exp = pLevelUpData->gold_exp;
        break;
    default:
        CnAssert(false);
        break;
    }

    return grow;
}

int32 SkillConfigManager::GetSkillExpByLevelQuality(int32 level, uint32 quality)
{
    if(quality >= MAX_EQUIP_QUALITY)
    {
        CnAssert(false);
        return 0;
    }

    std::map<uint32, const SKILL_LEVELUP_ROOT_SKILL_VALUE_STRUCT*>::const_iterator iter = m_SkillLevelUpConfigMap.find(level);
    if(iter == m_SkillLevelUpConfigMap.end())
    {
        CnAssert(false);
        return 0;
    }

    const SKILL_LEVELUP_ROOT_SKILL_VALUE_STRUCT* pLevelUpData = iter->second;
    CnAssert(pLevelUpData);

    switch(quality)
    {
    case QUALITY_WHITE:
        return pLevelUpData->white_exp;
    case QUALITY_GREEN:
        return pLevelUpData->green_exp;
    case QUALITY_BLUE:
        return pLevelUpData->blue_exp;
    case QUALITY_PURPLE:
        return pLevelUpData->purple_exp;
    case QUALITY_GOLD:
        return pLevelUpData->gold_exp;
    default:
        CnAssert(false);
        break;
    }

    return 0;
}
