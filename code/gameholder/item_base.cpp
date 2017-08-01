#include "gameholder_pch.h"
#include "item_base.h"
#include "item_config_manager.h"
#include "item_factory.h"

int32 ItemBase::s_ItemObjectNum = 0;

CnImplementRootRTTI(ItemBase);

ItemBase::ItemBase( uint32 id, uint32 itemType ) :
m_Id(id),
m_ItemType(itemType),
m_MaxStackNum(1),
m_Place(0),
m_Pos(0),
m_Num(1)
{
    CnAssert(ItemFactory::s_ObjectNum == s_ItemObjectNum);
    s_ItemObjectNum++;
}

ItemBase::~ItemBase()
{
    CnAssert(ItemFactory::s_ObjectNum == s_ItemObjectNum);
    s_ItemObjectNum--;
}

void ItemBase::LoadBase(const ITEM_BASE& info)
{
    CnAssert(GetId() == info.id);

    SetPlace(info.place);
    SetPos(info.pos);
    SetNum(Crown::SDMax(1, info.num));
}

void ItemBase::SaveBase(ITEM_BASE& info) const
{
    info.id = GetId();
    info.place = GetPlace();
    info.pos = GetPos();
    info.num = GetNum();
}

void ItemBase::SetNum( int32 val )
{
    val = Crown::SDRange(0, val, (int32)GetMaxStackNum());

    m_Num = val;
}
