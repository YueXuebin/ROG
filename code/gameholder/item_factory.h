#ifndef item_factory_h__
#define item_factory_h__

#include "item_base.h"
#include "item_equip.h"
#include "item_consume.h"
#include "item_material.h"
#include "item_stone.h"

class ItemFactory
{
    ItemFactory();
    ~ItemFactory();
    DECLARE_SINGLETON(ItemFactory)
public:
    static ItemBase* CreateItem(uint32 id, uint32 itemType);
    static ItemEquip* CreateEquip(uint32 id);
    static ItemConsume* CreateConsume(uint32 id);
    static ItemMaterial* CreateMaterial(uint32 id);
    static ItemStone* CreateStone(uint32 id);

    static void DestoryItem(ItemBase* pItem);

public:
    static int32                        s_ObjectNum;        // 对象计数(校验用)

private:
    Crown::CSDObjectPool<ItemEquip>	    m_EquipPool;
    Crown::CSDObjectPool<ItemConsume>   m_ConsumePool;
    Crown::CSDObjectPool<ItemMaterial>  m_MaterialPool;
    Crown::CSDObjectPool<ItemStone>     m_StonePool;

    
};

#endif
