#include "gameholder_pch.h"
#include "item_consume.h"
#include "item_config_manager.h"

CnImplementRTTI(ItemConsume, ItemBase);

ItemConsume::ItemConsume( uint32 id ) : ItemBase(id, ITEM_TYPE_CONSUME)
, m_SubType(0)
, m_CDType(0)
, m_CDTime(0)
, m_ContinuedTime(0)
, m_Level(0)
{

}

bool ItemConsume::Init()
{
    const CONSUME_ROOT_TEMPLATE_STRUCT * pTplt = ItemConfigManager::Instance()->FindConsumeTpltById(GetId());
    if (!pTplt)
    {
        CnWarn("ItemConsume::Init not found template by id=%u!\n", GetId());
        return false;
    }

    m_MaxStackNum = pTplt->max_stack;

    m_SubType = pTplt->sub_type;
    m_CDType = pTplt->cd_type;
    m_CDTime = pTplt->cd_time;
    m_ContinuedTime = pTplt->continued_time;
    m_Level = pTplt->lv;

    if (!pTplt->effect.empty())
    {
        m_EffectData = ConfigReader::parseData3List(pTplt->effect);
    }

    return true;
}

void ItemConsume::LoadItem( const void* pInfo )
{
    CnAssert(pInfo);
    const ITEM_CONSUME* pConsume = static_cast<const ITEM_CONSUME*>(pInfo);
    CnAssert(pConsume);

    LoadBase(pConsume->base);
}

void ItemConsume::SaveItem( void* pInfo ) const
{
    CnAssert(pInfo);
    ITEM_CONSUME* pConsume = static_cast<ITEM_CONSUME*>(pInfo);
    CnAssert(pConsume);

    SaveBase(pConsume->base);
}
