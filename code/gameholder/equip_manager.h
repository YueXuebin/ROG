#ifndef equip_manager_h__
#define equip_manager_h__
#include "item_equip.h"

class Creature;
class ItemManager;

class EquipManager
{
public:
    friend class ItemManager;

    ItemManager* GetItemMgr();

public:
    EquipManager(Creature* pCreature, uint8 place);
    ~EquipManager();

    const EquipGrid& GetEquipGrid() const {return m_EquipGrid;}
    EquipGrid& GetEquipGrid() {return m_EquipGrid;}

    void Clear();

    void OnRecv(uint32 msgID, void* data);

    void LoadEquipInfo(const EQUIP_INFO& info);                     // 加载装备信息
    void FillEquipInfo(EQUIP_INFO& info) const;                     // 填充装备信息

    bool IsEmptyGrid( int32 pos);

    int32 GetEquipAnimation() const;

    void CalcAllEquipProp();                                            // 计算装备属性

    ItemEquip* GetEquipByPos(int32 pos);

    bool IsValidGrid(int32 pos) const;

    void SetPropertyDirty() {m_PropertyDirty = true;}

    static void SetEquipRandom(ItemEquip* pEquip);  // 随机品质属性条目和宝石孔等
    static std::vector<uint32> RandomQualityUpAttributes(ItemEquip* pEquip, uint32 propNum, const std::vector<uint32>* excludeAttributes); // 随机属性条目

private:
    uint16 OptInstall( ItemBase* pSrc,  const int32 dstPos );
    uint16 OptUninstall( int32 srcPos, int32 dstPos = -1 );
    uint16 OptSwap( uint32 srcPlace, int32 srcPos, uint32 dstPlace, int32 dstPos );

    void AddToGrid(ItemEquip * pEquip, bool bSwap = false);

    void UpdateEquipInfoNtf();

private:
    const uint8     m_Place;

private:
    Creature*       m_pCreature;                              // 角色指针

    EquipGrid       m_EquipGrid;                            // 装备列表

    bool            m_PropertyDirty;                        // 属性发生改变
};

#endif
