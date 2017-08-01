#include "gameholder_pch.h"
#include "drop_object.h"
#include "scene.h"
#include "item_factory.h"



CnImplementRTTI(DropObject, SceneObject);


DropObject::DropObject(void)
:SceneObject(OBJ_DROP_OBJECT)
,m_pItemBase(NULL)
{
    m_PassTime = 0;
    m_LifeTime = 10000;
    SetRadius(0);       // 掉落物品，没有碰撞
}

DropObject::~DropObject(void)
{
    if (m_pItemBase)
    {
        ItemFactory::Instance()->DestoryItem(m_pItemBase);
    }
}

void DropObject::Init(const DropData& dropdata, ItemBase* pItemBase)
{
    CnAssert(pItemBase);

    m_FromPos = dropdata.from_pos;

    SetPosition(dropdata.pos);
    SetAngle(Crown::SDRandom(360));     // 方向随机

    SetLifeTime(dropdata.lifetime);
    m_OwnerID = dropdata.owner_id;

    m_pItemBase = pItemBase;
}

void DropObject::Update(uint32 frame_count)
{
    m_PassTime += g_ServerUpdateTime;
    if(m_PassTime >= m_LifeTime && (!isDestory()))
    {
        DestroyDropObject();
    }
}

void DropObject::DestroyDropObject()
{
    SetDestory();
    m_PassTime = 0;
}

ItemBase* DropObject::ExtractItem()
{
    ItemBase* ret = NULL;
    if(m_pItemBase)
    {
        ret = m_pItemBase;
    }
    return ret;
}

void DropObject::TransferItemOwnership()
{
    m_pItemBase = NULL;
}

