#ifndef design_config_manager_h__
#define design_config_manager_h__

#include "trigger_config.h"

/*
 *	πÿø®œ‡πÿ≈‰÷√
 */
class DesignConfigManager
{
public:
    DesignConfigManager();
    virtual ~DesignConfigManager();

    DECLARE_SINGLETON(DesignConfigManager)

    bool LoadConfig(const char* path);

    const TRIGGER_ROOT_TRIGGER_STRUCT* GetTriggerConfigData(uint32 triggerID);

private:

    std::map<uint32, const TRIGGER_ROOT_TRIGGER_STRUCT*>    m_TriggerConfigMap;       // <trigger id, trigger config data>

};

#endif // design_config_manager_h__
