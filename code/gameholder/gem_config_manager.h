#ifndef gem_config_manager_h__
#define gem_config_manager_h__

#include "gem_config.h"

class GemConfigManager
{
public:
    GemConfigManager(void);
    virtual ~GemConfigManager(void);

    DECLARE_SINGLETON(GemConfigManager);

    bool LoadConfig(const char* path);

    const GEM_ROOT_GEM_STRUCT* GetGemConfigData(uint32 id);
    const GEM_ROOT_GEM_ITEM_STRUCT* GetGemItemConfigData(uint32 id, uint32 level);
    const int32 GetGemMaxLevel(uint32 id);
    const int32 GetExpByLevel(uint32 id, uint32 level);
    const int32 GetTotalExpByLevel(uint32 id, uint32 level);

private:
    const GEM_ROOT_STRUCT* m_GemList;
    std::map<uint32, const GEM_ROOT_GEM_STRUCT*> m_GemConfigMap; // <id, config data>
};

#endif // gem_config_manager_h__