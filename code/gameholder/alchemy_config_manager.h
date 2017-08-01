/*
*   加载领地相关配置
*/
#ifndef alchemy_config_manager_h__
#define alchemy_config_manager_h__

#include "alchemy_config.h"

class AlchemyConfigManager
{
public:
    AlchemyConfigManager(void);
    ~AlchemyConfigManager(void);

    DECLARE_SINGLETON(AlchemyConfigManager)

    bool LoadConfig(const char* path);
    const ALCHEMY_ROOT_ALCHEMY_STRUCT* FindManorMakeDstItem(uint32 id) const;                       //根据公式id查找领地制生成的物品信息
};

#endif // ALCHEMY_config_manager_h__
