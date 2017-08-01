#include "gameholder_pch.h"
#include "gem_config_manager.h"

IMPLEMENT_SINGLETON(GemConfigManager)

GemConfigManager::GemConfigManager(void)
{
    GEM_ENTRY::CreateInstance();
}

GemConfigManager::~GemConfigManager(void)
{
    GEM_ENTRY::DestroyInstance();
}

bool GemConfigManager::LoadConfig(const char* path)
{
    if (GEM_ENTRY::Instance()->LoadConfig(path))
    {
        m_GemList = &GEM_ENTRY::Instance()->GetStruct();

        for(std::vector<GEM_ROOT_GEM_STRUCT>::const_iterator iter = m_GemList->gem_list.begin();
            iter != m_GemList->gem_list.end(); iter++)
        {
            m_GemConfigMap[iter->id] = &(*iter);
        }
    }
    return true;
}

const GEM_ROOT_GEM_STRUCT* GemConfigManager::GetGemConfigData(uint32 stoneId)
{
    std::map<uint32, const GEM_ROOT_GEM_STRUCT*>::const_iterator iter = m_GemConfigMap.find(stoneId);
    if (iter != m_GemConfigMap.end())
        return iter->second;
    else
    {
        CnWarn("gem %d config data not exist\n", stoneId);
        return NULL;
    }
}

const int32 GemConfigManager::GetGemMaxLevel(uint32 stoneId)
{
    const GEM_ROOT_GEM_STRUCT* gemConfig = GetGemConfigData(stoneId);
    int32 maxLevel = 1;
    if (gemConfig)
    {
        for(std::vector<GEM_ROOT_GEM_ITEM_STRUCT>::const_iterator iter = gemConfig->item_list.begin();
            iter != gemConfig->item_list.end(); iter++)
        {
            if (iter->level > maxLevel)
            {
                maxLevel = iter->level;
            }
        }
    }
    return maxLevel;
}

const GEM_ROOT_GEM_ITEM_STRUCT* GemConfigManager::GetGemItemConfigData(uint32 stoneId, uint32 level)
{
    const GEM_ROOT_GEM_STRUCT* gemConfig = GetGemConfigData(stoneId);
    if (gemConfig)
    {
        for(std::vector<GEM_ROOT_GEM_ITEM_STRUCT>::const_iterator iter = gemConfig->item_list.begin();
            iter != gemConfig->item_list.end(); iter++)
        {
            if (iter->level == level)
            {
                return &(*iter);
            }
        }

    }
    return NULL;
}

const int32 GemConfigManager::GetExpByLevel(uint32 stoneId, uint32 level)
{
    const GEM_ROOT_GEM_ITEM_STRUCT* item = GetGemItemConfigData(stoneId, level);

    if (!item)
    {
        CnAssert(false);
        return 0;
    }

    return item->exp;
}

const int32 GemConfigManager::GetTotalExpByLevel(uint32 stoneId, uint32 level)
{
    const GEM_ROOT_GEM_STRUCT* gemConfig = GetGemConfigData(stoneId);
    int32 exp = 0;
    if (gemConfig)
    {
        for(std::vector<GEM_ROOT_GEM_ITEM_STRUCT>::const_iterator iter = gemConfig->item_list.begin();
            iter != gemConfig->item_list.end(); iter++)
        {
            if (iter->level >= (int32)level)
            {
                exp += iter->exp;
            }
        }
    }
    return exp;
}