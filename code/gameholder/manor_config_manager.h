/*
*   加载领地相关配置
*/
#ifndef territory_config_manager_h__
#define territory_config_manager_h__

#include "territory_make_config.h"

class TerritoryConfigManager
{
public:
    TerritoryConfigManager(void);
    ~TerritoryConfigManager(void);

    DECLARE_SINGLETON(TerritoryConfigManager)

    bool LoadConfig(const char* path);

    const std::vector<TERRITORY_MAKE_ROOT_TERRITORY_MAKE_ITEM_STRUCT>* FindManorMakeItem(uint32 id) const;       //根据公式id查找领地制作需要消耗的物品列表
    const TERRITORY_MAKE_ROOT_TERRITORY_MAKE_STRUCT* FindManorMakeDstItem(uint32 id) const;                       //根据公式id查找领地制生成的物品信息
};

#endif // territory_config_manager_h__
