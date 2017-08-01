#ifndef item_config_manager_h__
#define item_config_manager_h__
#include "equip_config.h"
#include "consume_config.h"
#include "material_config.h"
#include "stone_config.h"
#include "equip_attribute_config.h"
#include "equip_quality_attribute_config.h"

#include "item_equip.h"
#include "item_stone.h"
#include "equip_quality_up_config.h"
#include "equip_wash_up_config.h"
#include "equip_star_up_config.h"
#include "equip_enchantment_config.h"

typedef std::map<uint32, const EQUIP_ROOT_TEMPLATE_STRUCT*> TpltEquipMap;
typedef std::map<uint32, const CONSUME_ROOT_TEMPLATE_STRUCT*> TpltConsumeMap;
typedef std::map<uint32, const MATERIAL_ROOT_TEMPLATE_STRUCT*> TpltMaterialMap;
typedef std::map<uint32, const STONE_ROOT_TEMPLATE_STRUCT*> TpltStoneMap;
typedef std::map<uint32, const EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT*> TpltEquipAttributeMap;
typedef std::map<int32, TpltEquipAttributeMap>  TpltEquipAttributeGradeMap;
typedef std::map<int32, TpltEquipAttributeMap>  TpltEquipAttributePropMap;
typedef std::map<uint32, std::vector<uint32> > TpltEquipQualityAttributePropMap;
typedef std::map<uint32, std::vector<uint32> > TpltEquipQualityAttributeClassMap;       // "> >"中间要有空格,gcc要求
typedef std::map<std::string, const EQUIP_QUALITY_UP_ROOT_HEAD_STRUCT* > TpltEquipQualityUpMap;
typedef std::map<uint32, const EQUIP_WASH_UP_ROOT_HEAD_STRUCT* > TpltEquipWashUpMap;
typedef std::map<uint32, const EQUIP_STAR_UP_ROOT_HEAD_STRUCT*> TpltEquipStarUpMap;
typedef std::map<uint32, const EQUIP_ENCHANTMENT_ROOT_FORMULA_STRUCT*> TpltEquipEnchantMap;

/*
 *	物品相关的配置
 */
class ItemConfigManager
{
    ItemConfigManager();
    ~ItemConfigManager();

    DECLARE_SINGLETON(ItemConfigManager)

public:
    bool LoadConfig(const char* path);

    const EQUIP_ROOT_TEMPLATE_STRUCT* FindEquipTpltById(uint32 id) const;
    const CONSUME_ROOT_TEMPLATE_STRUCT* FindConsumeTpltById(uint32 id) const;
    const MATERIAL_ROOT_TEMPLATE_STRUCT* FindMaterialTpltById(uint32 id) const;
    const STONE_ROOT_TEMPLATE_STRUCT* FindStoneTpltById(uint32 id) const;
    const EQUIP_QUALITY_UP_ROOT_HEAD_STRUCT* FindQualityUpData(uint32 quality) const;           // 查询装备升品消耗'
    const EQUIP_WASH_UP_ROOT_HEAD_STRUCT* FindWashStone() const;                                // 根据id查询洗练石信息
    const EQUIP_STAR_UP_ROOT_HEAD_STRUCT* FindStarUpData(uint32 star) const;                    // 根据星级查询升星配置
    const EQUIP_ENCHANTMENT_ROOT_FORMULA_STRUCT* FindEnchantData(uint32 attribute_id) const;    // 根据条目id查询附魔配置

    const uint32 GetEquipStarUpMax() const { return m_EquipStarUpMax; } // 装备升星最大星级

    const TpltEquipAttributePropMap* GetEquipAttributePropMap() const {return &m_EquipAttributePropMap;}

    const EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT* FindEquipAttributeTpltById(uint32 id) const;
    const TpltEquipAttributeMap* FindEquipAttributeMapByGrade(uint32 grade) const;
    const TpltEquipAttributeMap* FindEquipAttributeMapByProp(uint32 prop) const;

    const std::vector<uint32>* FindCanPropIdListByEquipClass(uint32 equip_class) const;

    static bool CheckEquipTypeWithPos(int32 type, int32 part);
    static bool CheckEquipSwapType(int32 type);
    static bool CheckEquipSwapPart(int32 part1, int32 part2);

    static int32 FindEquipPartByType(uint32 type, const EquipGrid& grid);
    static uint8 GetEquipAnimation(const EquipGrid& grid);

    uint32 GetItemTypeById(uint32 id) const;
    uint32 IsItemId(uint32 id) const;

    void FindEquipAttributeProp(int32 type, std::vector<const TpltEquipAttributeMap*>& Vector);

    static bool IsWeapon(uint32 equipSubType);       // 是武器
    static bool IsArmor(uint32 equipSubType);        // 是防具
    static bool IsJewelry(uint32 equipSubType);      // 是饰品

protected:
    void InsertToGradeMap(const EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT* pStruct);
    void InsertToPropMap(const EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT* pStruct);

private:
    TpltEquipMap    m_EquipMap;
    TpltConsumeMap  m_ConsumeMap;
    TpltMaterialMap m_MaterialMap;
    TpltStoneMap    m_StoneMap;

    TpltEquipAttributeMap       m_EquipAttributeMap;                // 所有的随机属性条目<id, 条目>
    TpltEquipAttributeGradeMap  m_EquipAttributeGradeMap;           // 各等阶的随机条目<等阶, <id, 该等阶的条目>>
    TpltEquipAttributePropMap   m_EquipAttributePropMap;            // 同属性的随机条目<属性id, <id, 该属性的条目>>
    TpltEquipQualityAttributePropMap    m_EquipQualityAttributePropMap;     // 属性对应的可出装备类型<属性id, 装备类型列表>
    TpltEquipQualityAttributeClassMap   m_EquipQualityAttributeClassMap;    // 装备类型对应的可出属性列表<装备类, 属性id列表>
    TpltEquipQualityUpMap       m_EquipQualityUpMap;                // 装备升品列表 <'升品石-品质',升品石列表> 比如： <'1',<列表>>
    TpltEquipWashUpMap          m_EquipWashUpMap;                   // 装备洗练列表 <洗练石id, 条目>
    TpltEquipStarUpMap          m_EquipStarUpMap;                   // 装备升星列表 <星级, 条目>
    TpltEquipEnchantMap          m_EquipEnchantMap;                  // 装备附魔列表 <附魔公式id, 条目>

    uint32  m_EquipStarUpMax;   // 装备升星最大星级
};

#endif // item_config_manager_h__
