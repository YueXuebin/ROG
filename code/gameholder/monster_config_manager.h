#ifndef monster_config_manager_h__
#define monster_config_manager_h__

#include "monster_attribute_norm_config.h"
#include "monster_attribute_revise_config.h"
#include "monster_quality_modifier_config.h"

/*
 *	怪物表管理器
 */
class MonsterConfigManager
{
    MonsterConfigManager();
    ~MonsterConfigManager();
    DECLARE_SINGLETON(MonsterConfigManager)
public:
    bool LoadConfig(const char* path);
    const MONSTER_ATTRIBUTE_NORM_ROOT_TEMPLATE_STRUCT* GetMonsterAttributeNormByLevel(int32 level) const;
    const MONSTER_ATTRIBUTE_REVISE_ROOT_MONSTER_STRUCT* GetMonsterAttributeReviseById(int32 id) const;
    const MONSTER_QUALITY_MODIFIER_ROOT_MODIFIER_STRUCT* GetMonsterQualityModifierById(uint32 modifierId) const;

private:
    std::map<int32, const MONSTER_ATTRIBUTE_NORM_ROOT_TEMPLATE_STRUCT*> m_NormMap;              // <怪物等级，怪物等级标准属性>
    std::map<int32, const MONSTER_ATTRIBUTE_REVISE_ROOT_MONSTER_STRUCT*> m_ReviseMap;           // <怪物id, 各怪物的属性修正>
    std::map<uint32, const MONSTER_QUALITY_MODIFIER_ROOT_MODIFIER_STRUCT*>  m_ModifierMap;      // <属性id, 品质属性数据>
};

#endif // monster_config_manager_h__
