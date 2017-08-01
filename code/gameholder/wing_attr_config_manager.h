#ifndef wing_attr_config_manager_h__
#define wing_attr_config_manager_h__

#include "wing_attr_config.h"

class WingAttrConfigManager
{
public:
    WingAttrConfigManager(void);
    virtual ~WingAttrConfigManager(void);

    DECLARE_SINGLETON(WingAttrConfigManager);

    bool LoadConfig(const char* path);


    const WING_ATTR_ROOT_WING_STRUCT*      FindWing(uint32 id) const;                    // 查找翅膀
    const WING_ATTR_ROOT_WING_ITEM_STRUCT* FindWingItem(uint32 id, uint32 level) const;  // 通过翅膀id和等级，查询翅膀属性


};

#endif // wing_attr_config_manager_h__