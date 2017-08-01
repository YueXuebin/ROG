#include "gameholder_pch.h"
#include "id_pool_manager.h"
#include "id_pool_segment.h"

IMPLEMENT_SINGLETON(IdPoolManager)

IdPoolManager::IdPoolManager()
{
    m_SegmentPool = CnNew Crown::CSDObjectPool<IdPoolSegment>(1000, 1000);      // 对象池
    m_OctetPool = CnNew Crown::CSDObjectPool<IdPoolOctet>(1000, 1000);
}

IdPoolManager::~IdPoolManager()
{
    DestoryIdPoolSegment(m_RootSegment);

    CnDelete m_SegmentPool;
    CnDelete m_OctetPool;
}

bool IdPoolManager::Init()
{
    m_RootSegment = CreateIdPoolSegment(SEGMENT_32);
    return true;
}

bool IdPoolManager::RegisterID(uint32 id)
{
    return m_RootSegment->RegisterID(id);
}

bool IdPoolManager::UnregisterID(uint32 id)
{
    return m_RootSegment->UnregisterID(id);
}

void IdPoolManager::CleanZero()
{
    m_RootSegment->CleanZero();
}
