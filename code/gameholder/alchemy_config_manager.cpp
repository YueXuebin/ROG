#include "gameholder_pch.h"
#include "alchemy_config_manager.h"
#include "alchemy_config.h"

IMPLEMENT_SINGLETON(AlchemyConfigManager)

AlchemyConfigManager::AlchemyConfigManager(void)
{
    ALCHEMY_ENTRY::CreateInstance();
}

AlchemyConfigManager::~AlchemyConfigManager(void)
{
    ALCHEMY_ENTRY::DestroyInstance();
}

bool AlchemyConfigManager::LoadConfig(const char* path)
{
    // ¼ÓÔØ²Ö¿âÖÆ×÷ÅäÖÃ
    if (ALCHEMY_ENTRY::Instance()->LoadConfig(path))
    {
        return true;
        /*
        m_EquipQualityUpMap.clear();

        const EQUIP_QUALITY_UP_ROOT_STRUCT& RootStruct = EQUIP_QUALITY_UP_ENTRY::Instance()->GetStruct();

        std::vector<EQUIP_QUALITY_UP_ROOT_HEAD_STRUCT>::const_iterator cIter = RootStruct.head_list.begin();

        for (; cIter != RootStruct.head_list.end(); ++cIter)
        {

            if (m_EquipQualityUpMap.end() != m_EquipQualityUpMap.find(key))
            {
                CnWarn("EQUIP_ATTRIBUTE duplicate id = %d!\n", cIter->id);
                continue;
            }

            m_EquipQualityUpMap[key] = &(*cIter); 
        }*/
    }
    else
    {
        return false;
    }
}


const ALCHEMY_ROOT_ALCHEMY_STRUCT* AlchemyConfigManager::FindManorMakeDstItem(uint32 id) const
{
    const ALCHEMY_ROOT_STRUCT& RootStruct = ALCHEMY_ENTRY::Instance()->GetStruct();

    for (std::vector<ALCHEMY_ROOT_ALCHEMY_STRUCT>::const_iterator cIter = RootStruct.alchemy_list.begin(); cIter != RootStruct.alchemy_list.end(); ++cIter)
    {

        if (cIter->id == id)
        {
            return &(*cIter);
        }
    }

    return NULL;
}
