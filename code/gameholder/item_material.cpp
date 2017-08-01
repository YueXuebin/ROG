#include "gameholder_pch.h"
#include "item_material.h"
#include "item_config_manager.h"

CnImplementRTTI(ItemMaterial, ItemBase);

ItemMaterial::ItemMaterial( uint32 id ) : ItemBase(id, ITEM_TYPE_MATERIAL),
m_SubType(0)
{

}

bool ItemMaterial::Init()
{
    const MATERIAL_ROOT_TEMPLATE_STRUCT * pTplt = ItemConfigManager::Instance()->FindMaterialTpltById(GetId());
    if (!pTplt)
    {
        CnWarn("ItemMaterial::Init not found template by id=%u!\n", GetId());
        return false;
    }

    m_MaxStackNum = pTplt->max_stack;

    m_SubType = pTplt->sub_type;

    return true;
}

void ItemMaterial::LoadItem( const void* pInfo )
{
    CnAssert(pInfo);
    const ITEM_MATERIAL* pMaterial = static_cast<const ITEM_MATERIAL*>(pInfo);
    CnAssert(pMaterial);

    LoadBase(pMaterial->base);
}

void ItemMaterial::SaveItem( void* pInfo ) const
{
    CnAssert(pInfo);
    ITEM_MATERIAL* pMaterial = static_cast<ITEM_MATERIAL*>(pInfo);
    CnAssert(pMaterial);

    SaveBase(pMaterial->base);
}