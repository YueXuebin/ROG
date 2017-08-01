#ifndef item_equip_h__
#define item_equip_h__
#include "rtti.h"
#include "item_base.h"
#include "property.h"
#include "equip_attribute_config.h"

class ItemStone;

class ItemFactory;

/*
 *	装备
 *  不可叠加
 */
class ItemEquip : public ItemBase
{
    CnDeclareRTTI
public:
    ItemEquip( uint32 id);
    virtual ~ItemEquip();

public:

    bool Init();

    uint32 GetPartType() const { return m_PartType; }
    uint32 GetSubType() const { return m_SubType; }
    uint32 GetGrade() const { return m_Grade; }
    int32  GetEqpLevel() const { return m_EqpLevel; }

public:
    virtual void LoadItem(const void* pInfo);
    virtual void SaveItem(void* pInfo) const;

public:
    uint32 GetQuality() const { return m_Quality; }
    uint32 GetStar() const { return m_Star; }
    void SetQuality( uint32 val);
    void SetStar( uint32 val );

    const std::vector<uint32>& GetQualityAttributeList() const { return m_QualityAttributeList; }
    void SetQualityAttributeList(const std::vector<uint32>& attribute);
    void SetQualityAttribute(const uint32 index, const uint32 attributeId);
    void AddQualityAttribute(const uint32 attributeId);

    const std::vector<uint32>& GetWashUpAttributeList() const { return m_WashUpAttributeList; }
    void ClearWashUpAttributeList() { m_WashUpAttributeList.clear(); }
    void SetWashUpAttributeList(const std::vector<uint32>& attributes) { m_WashUpAttributeList = attributes; }
    const std::vector<uint32>& GetWashUpLockList() const { return m_WashUpLockList; }
    void ClearWashUpLockList() { m_WashUpLockList.clear(); }
    void SetWashUpLockList(const std::vector<uint32>& locks) { m_WashUpLockList = locks; }

    const std::vector<uint32>& GetEnchantAttributeList() const { return m_EnchantAttributeList; }
    void ClearEnchantAttributeList();
    void SetEnchantAttributeList(const std::vector<uint32>& attribute);
    void AddEnchantAttribute(const uint32 attributeId);

    const std::vector<uint32>& GetGemHoleList() const { return m_GemHoleList; }
    uint32 GetGemHoleType(uint32 pos) const { return m_GemHoleList.size() >= pos ? m_GemHoleList[pos] : 0; }
    void SetGemHoleList(const std::vector<uint32>& holes);
    void AddGemHole(uint32 holeType);
    void SetGemHole(uint32 pos, uint32 holeType);

    const std::vector<ITEM_STONE>& GetGemStoneList() const { return m_GemStoneList; }
    void SetGemStoneList(const std::vector<ITEM_STONE>& gems);
    ITEM_STONE* GetGemStone(uint32 pos);
    bool HasGemStone(uint32 pos);
    void ClearGemStoneList();
    
    const PropertySet& GetPropertySet() const;

private:
    void CalcEquipProp() const;                                         // 装备内部如果改变了就自行计算一次，外部调用直接取
    void CalcEquipPropImpl(PropertySet& pPropSet, uint32 star) const;   // 装备内部如果改变了就自行计算一次，外部调用直接取

private:
    uint32                      m_PartType;             // 位置类型(装备位置)
    uint32                      m_SubType;              // 装备类型(1头盔 2盔甲 3肩甲 4手套 5鞋子 6项链 7戒指 8匕首 9单手剑 10单手锤 11双手剑 12长杖 13弓 14盾)
    uint32                      m_Grade;                // 阶级
    int32                       m_EqpLevel;             // 
    int32                       m_Star;                 // 星级

protected:
    uint32                      m_Quality;              // 品质

    std::vector<uint32>         m_QualityAttributeList; // 品质属性id
    std::vector<uint32>         m_WashUpAttributeList;  // 洗练后未使用的品质属性
    std::vector<uint32>         m_WashUpLockList;       // 洗练属性加锁

    std::vector<uint32>         m_EnchantAttributeList; // 附魔属性id

    std::vector<uint32>         m_GemHoleList;          // 宝石孔
    std::vector<ITEM_STONE>     m_GemStoneList;         // 镶嵌的宝石

    mutable bool                        m_PropSetDirty;
    mutable PropertySet                 m_PropSet;              // 装备总属性(属性汇总后)

private:
    mutable PropertySet                 m_NextStarPropSet;      // 装备下一星级总属性(属性汇总后)
};

typedef std::vector<ItemEquip* > EquipGrid;

#endif