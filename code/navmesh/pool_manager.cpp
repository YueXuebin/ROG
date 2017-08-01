#include "navmesh_pch.h"
#include "pool_manager.h"
#include "edge.h"
#include "newfunnel.h"
#include "triangle_node.h"
#include "portal.h"
PoolManager g_PoolManager;

PoolManager::PoolManager() :
m_EdgePool(100),
m_NewFunnelPool(100),
m_TriangelNodePool(100),
m_PorTalPool(100)
{

}

PoolManager::~PoolManager()
{

}


Edge* PoolManager::CreateEdge(const CoVec2& point1, const CoVec2& point2)
{
    return m_EdgePool.Alloc(point1, point2);
}

void PoolManager::DestoryEdge( Edge* pEdge )
{
    m_EdgePool.Free(pEdge);
}

NewFunnel* PoolManager::CreateFunnel()
{
    return m_NewFunnelPool.Alloc();
}

void PoolManager::DestoryFunnel(NewFunnel* pfunnel)
{
    m_NewFunnelPool.Free(pfunnel);
}

TriangleNode* PoolManager::CreateTriangleNode(CoPolygon* polygon)
{
    return m_TriangelNodePool.Alloc(polygon);
}

void PoolManager::DestoryTriangleNode(TriangleNode* node)
{
    m_TriangelNodePool.Free(node);
}

Portal* PoolManager::CreatePorTal(const CoVec2& left, const CoVec2& right)
{
   return m_PorTalPool.Alloc(left, right);
}

void PoolManager::DestoryPorTal(Portal* portal)
{
    m_PorTalPool.Free(portal);
}
