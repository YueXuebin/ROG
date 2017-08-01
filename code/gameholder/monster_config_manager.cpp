#include "gameholder_pch.h"
#include "monster_config_manager.h"

IMPLEMENT_SINGLETON(MonsterConfigManager)

MonsterConfigManager::MonsterConfigManager()
{
    MONSTER_ATTRIBUTE_NORM_ENTRY::CreateInstance();
    MONSTER_ATTRIBUTE_REVISE_ENTRY::CreateInstance();
    MONSTER_QUALITY_MODIFIER_ENTRY::CreateInstance();
}

MonsterConfigManager::~MonsterConfigManager()
{
    MONSTER_ATTRIBUTE_NORM_ENTRY::DestroyInstance();
    MONSTER_ATTRIBUTE_REVISE_ENTRY::DestroyInstance();
    MONSTER_QUALITY_MODIFIER_ENTRY::DestroyInstance();
}

bool MonsterConfigManager::LoadConfig( const char* path )
{
    if (MONSTER_ATTRIBUTE_NORM_ENTRY::Instance()->LoadConfig(path))
    {
        m_NormMap.clear();
        const MONSTER_ATTRIBUTE_NORM_ROOT_STRUCT & RootStruct = MONSTER_ATTRIBUTE_NORM_ENTRY::Instance()->GetStruct();
        std::vector<MONSTER_ATTRIBUTE_NORM_ROOT_TEMPLATE_STRUCT>::const_iterator cIter = RootStruct.template_list.begin();
        for (; cIter != RootStruct.template_list.end(); ++cIter)
        {
            if (m_NormMap.end() != m_NormMap.find(cIter->level))
            {
                CnWarn("MONSTER_ATTRIBUTE_NORM duplicate level = %d!\n", cIter->level);
                continue;
            }

            m_NormMap.insert(std::make_pair(cIter->level, &(*cIter)));
        }
    }
    else
    {
        return false;
    }

    if (MONSTER_ATTRIBUTE_REVISE_ENTRY::Instance()->LoadConfig(path))
    {
        m_ReviseMap.clear();
        const MONSTER_ATTRIBUTE_REVISE_ROOT_STRUCT& RootStruct = MONSTER_ATTRIBUTE_REVISE_ENTRY::Instance()->GetStruct();
        std::vector<MONSTER_ATTRIBUTE_REVISE_ROOT_MONSTER_STRUCT>::const_iterator cIter = RootStruct.monster_list.begin();
        for (; cIter != RootStruct.monster_list.end(); ++cIter)
        {
            if (m_ReviseMap.end() != m_ReviseMap.find(cIter->id))
            {
                CnWarn("MONSTER_ATTRIBUTE_REVISE duplicate id = %d!\n", cIter->id);
                continue;
            }

            m_ReviseMap.insert(std::make_pair(cIter->id, &(*cIter)));
        }
    }
    else
    {
        return false;
    }

    if(MONSTER_QUALITY_MODIFIER_ENTRY::Instance()->LoadConfig(path))
    {
        const MONSTER_QUALITY_MODIFIER_ROOT_STRUCT& RootStruct = MONSTER_QUALITY_MODIFIER_ENTRY::Instance()->GetStruct();
        std::vector<MONSTER_QUALITY_MODIFIER_ROOT_MODIFIER_STRUCT>::const_iterator cIter = RootStruct.modifier_list.begin();
        for(; cIter != RootStruct.modifier_list.end(); ++cIter)
        {
            if(m_ModifierMap.end() != m_ModifierMap.find(cIter->id))
            {
                CnWarn("MONSTER_QUALITY_MODIFIER duplicate id = %d!\n", cIter->id);
                continue;
            }

            m_ModifierMap.insert(std::make_pair(cIter->id, &(*cIter)));
        }
    }
    else
    {
        return false;
    }

    return true;
}

const MONSTER_ATTRIBUTE_NORM_ROOT_TEMPLATE_STRUCT * MonsterConfigManager::GetMonsterAttributeNormByLevel( int32 level ) const
{
    std::map<int32, const MONSTER_ATTRIBUTE_NORM_ROOT_TEMPLATE_STRUCT *>::const_iterator cIter = m_NormMap.find(level);
    if (m_NormMap.end() == cIter)
    {
        return NULL;
    }

    return cIter->second;
}

const MONSTER_ATTRIBUTE_REVISE_ROOT_MONSTER_STRUCT * MonsterConfigManager::GetMonsterAttributeReviseById( int32 id ) const
{
    std::map<int32, const MONSTER_ATTRIBUTE_REVISE_ROOT_MONSTER_STRUCT *>::const_iterator cIter = m_ReviseMap.find(id);
    if (m_ReviseMap.end() == cIter)
    {
        return NULL;
    }

    return cIter->second;
}

const MONSTER_QUALITY_MODIFIER_ROOT_MODIFIER_STRUCT* MonsterConfigManager::GetMonsterQualityModifierById(uint32 modifierId) const
{
    std::map<uint32, const MONSTER_QUALITY_MODIFIER_ROOT_MODIFIER_STRUCT*>::const_iterator cIter = m_ModifierMap.find(modifierId);
    if(m_ModifierMap.end() == cIter)
    {
        return NULL;
    }
    return cIter->second;
}