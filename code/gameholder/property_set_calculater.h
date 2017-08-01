#ifndef property_set_calculater_h__
#define property_set_calculater_h__

class BattleUnit;

/*
 *	角色战斗属性数值计算器
 */
class PropertySetCalculater
{
public:
    PropertySetCalculater(BattleUnit* unit);
    ~PropertySetCalculater();

    // 属性汇总计算
    static Property CollectProperty(BattleUnit* pBattleUnit, const Property& srcProperty, PropertySet& srcPropSet, bool zeroSrc);
    static void CollectPropertySet(PropertySet& srcPropSet, BattleUnit* pBattleUnit);

    // 计算战斗力
    PropertySet&    GetLevelProp()  { return m_Level; }
    PropertySet&    GetTalentProp() { return m_Talent; }
    PropertySet&    GetEquipProp()  { return m_Equip; }
    PropertySet&    GetMapProp()    { return m_Map; }
    PropertySet&    GetBuffProp()   { return m_Buff; }
    PropertySet&    GetWingPro()    { return m_Wing; }

    PropertySet&    GetBaseProp()   { return m_BasePropSet; }
    PropertySet&    GetBuffAndMapProp() { return m_BuffAndMap; }

    PropertySet&    GetFinalProp() {return m_FinalPropSet;}

    void    CalcFinalProp();                // 计算最终所有属性

private:
    

    // 计算四围提供的属性
    void CalcFourAttr();

    static int32 GetEquipAnimationDamageMul(BattleUnit* pBattleUnit, PropertySet& srcPropSet);
    static int32 GetEquipAnimationAttackBlock(BattleUnit* pBattleUnit, PropertySet& srcPropSet);

private:
    BattleUnit* m_pBattleUnit;

    uint32      m_BaseHp;               // 角色的基础Hp(计算触发buff用)

    // 各系统的属性值
    PropertySet m_Level;                // 等级提供的属性
    PropertySet m_Equip;                // 装备提供的属性(包括宝石)
    PropertySet m_FourAttr;             // 力智体敏的属性
    PropertySet m_Talent;               // 天赋提供的属性(佣兵天赋、怪物品质)
    PropertySet m_Map;                  // 地图提供的属性
    PropertySet m_Buff;                 // buff提供的属性
    PropertySet m_Wing;                 // 翅膀提供的属性

    PropertySet m_BasePropSet;          // 角色基础属性(等级、装备、四围、天赋、翅膀)
    PropertySet m_BuffAndMap;
    PropertySet m_FinalPropSet;         // 最终的角色属性

};

#endif // property_set_calculater_h__
