#include "gameholder_pch.h"
#include "design_config_manager.h"

IMPLEMENT_SINGLETON(DesignConfigManager)

DesignConfigManager::DesignConfigManager()
{
    TRIGGER_ENTRY::CreateInstance();
}

DesignConfigManager::~DesignConfigManager()
{
    TRIGGER_ENTRY::DestroyInstance();
}

bool DesignConfigManager::LoadConfig(const char* path)
{
    if(TRIGGER_ENTRY::Instance()->LoadConfig(path))
    {
        for(std::vector<TRIGGER_ROOT_TRIGGER_STRUCT>::const_iterator iter = TRIGGER_ENTRY::Instance()->GetStruct().trigger_list.begin();
            iter != TRIGGER_ENTRY::Instance()->GetStruct().trigger_list.end(); iter++)
        {
            m_TriggerConfigMap[iter->show_id] = &(*iter);
        }
    }

    return true;
}

const TRIGGER_ROOT_TRIGGER_STRUCT* DesignConfigManager::GetTriggerConfigData(uint32 showID)
{
    std::map<uint32, const TRIGGER_ROOT_TRIGGER_STRUCT*>::iterator iter = m_TriggerConfigMap.find(showID);
    if(iter != m_TriggerConfigMap.end())
        return iter->second;
    else
    {
        CnWarn("trigger %d config data not exist\n", showID);
        return NULL;
    }
}