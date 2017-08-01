#include "gameholder_pch.h"
#include "item_factory.h"

int32 ItemFactory::s_ObjectNum = 0;

IMPLEMENT_SINGLETON(ItemFactory)


ItemFactory::ItemFactory()
{

}

ItemFactory::~ItemFactory()
{
    if (0 != s_ObjectNum)
    {
        CnAssert(false);        // 池对象没有完全回收
    }
}

ItemBase* ItemFactory::CreateItem( uint32 id, uint32 itemType )
{
    switch (itemType)
    {
    case ITEM_TYPE_EQUIP:
        return CreateEquip(id);
    case ITEM_TYPE_CONSUME:
        return CreateConsume(id);
    case ITEM_TYPE_MATERIAL:
        return CreateMaterial(id);
    case ITEM_TYPE_STONE:
        return CreateStone(id);
    default:
        CnError("CreateItem unknown item type=%u!\n", itemType);
        break;
    }

    return NULL;
}

ItemEquip* ItemFactory::CreateEquip( uint32 id )
{
    ItemEquip* pEquip = ItemFactory::Instance()->m_EquipPool.Alloc(id);
    if (!pEquip)
    {
        CnAssert(false);
        return NULL;
    }

    ItemFactory::Instance()->s_ObjectNum++;
    CnAssert(ItemFactory::Instance()->s_ObjectNum == ItemBase::s_ItemObjectNum);

    if (!pEquip->Init())
    {
        DestoryItem(pEquip);
        return NULL;
    }

    return pEquip;
}

ItemConsume* ItemFactory::CreateConsume( uint32 id )
{
    ItemConsume* pConsume = ItemFactory::Instance()->m_ConsumePool.Alloc(id);
    if (!pConsume)
    {
        CnAssert(false);
        return NULL;
    }

    ItemFactory::Instance()->s_ObjectNum++;
    CnAssert(ItemFactory::Instance()->s_ObjectNum == ItemBase::s_ItemObjectNum);

    if (!pConsume->Init())
    {
        DestoryItem(pConsume);
        return NULL;
    }

    return pConsume;
}

ItemMaterial* ItemFactory::CreateMaterial( uint32 id )
{
    ItemMaterial* pMaterial = ItemFactory::Instance()->m_MaterialPool.Alloc(id);
    if (!pMaterial)
    {
        CnAssert(false);
        return NULL;
    }

    ItemFactory::Instance()->s_ObjectNum++;
    CnAssert(ItemFactory::Instance()->s_ObjectNum == ItemBase::s_ItemObjectNum);

    if (!pMaterial->Init())
    {
        DestoryItem(pMaterial);
        return NULL;
    }

    return pMaterial;
}

ItemStone* ItemFactory::CreateStone(uint32 id)
{
    ItemStone* pStone = ItemFactory::Instance()->m_StonePool.Alloc(id);
    if(!pStone)
    {
        CnAssert(false);
        return NULL;
    }
    
    ItemFactory::Instance()->s_ObjectNum++;
    CnAssert(ItemFactory::Instance()->s_ObjectNum == ItemBase::s_ItemObjectNum);

    if(!pStone->Init())
    {
        DestoryItem(pStone);
        return NULL;
    }

    return pStone;
}

void ItemFactory::DestoryItem( ItemBase * pItem )
{
    if (!pItem)
    {
        CnAssert(false);
    }

    if (pItem->IsKindOf(&ItemEquip::ms_RTTI))
    {
        ItemFactory::Instance()->m_EquipPool.Free(CnDynamicCast(ItemEquip, pItem));
    }
    else if (pItem->IsKindOf(&ItemConsume::ms_RTTI))
    {
        ItemFactory::Instance()->m_ConsumePool.Free(CnDynamicCast(ItemConsume, pItem));
    }
    else if (pItem->IsKindOf(&ItemMaterial::ms_RTTI))
    {
        ItemFactory::Instance()->m_MaterialPool.Free(CnDynamicCast(ItemMaterial, pItem));
    }
    else if(pItem->IsKindOf(&ItemStone::ms_RTTI))
    {
        ItemFactory::Instance()->m_StonePool.Free(CnDynamicCast(ItemStone, pItem));
    }
    else
    {
        CnAssert(false);
        return;
    }

    ItemFactory::Instance()->s_ObjectNum--;
    CnAssert(ItemFactory::Instance()->s_ObjectNum == ItemBase::s_ItemObjectNum);
}