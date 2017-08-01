#include "GridWorldPCH.h"
#include "GridObject.h"
#include "GridWorld.h"


GridObject::GridObject(GirdObjectOwner* owner) :
m_GridPosX(0),
m_GridPosY(0),
m_pGridWorld(NULL),
m_Next(NULL),
m_Owner(owner)
{

}

GridObject::~GridObject()
{

}

void GridObject::SetPosition(real x, real y)
{
    m_x = x;
    m_y = y;

    if(m_pGridWorld)
    {
        // 计算对应的网格位置
        uint32 cellLength = m_pGridWorld->GetCellLength();

        int32 cellPosX = (m_x/cellLength) > 0 ? int32(m_x/cellLength) : int32(m_x/cellLength)-1;
        int32 cellPosY = (m_y/cellLength) > 0 ? int32(m_y/cellLength) : int32(m_y/cellLength)-1;
        
        if(m_GridPosX != cellPosX || m_GridPosY != cellPosY)
        {
            CnAssert(m_pGridWorld);
            m_pGridWorld->OnMoveGridObject(this, m_GridPosX, m_GridPosY, cellPosX, cellPosY);

            m_GridPosX = cellPosX;
            m_GridPosY = cellPosY;
        }
    }
}

void GridObject::OnEnterGridWorld(GridWorld* world)
{
    CnAssert(!m_pGridWorld && world);
    m_pGridWorld = world;

    // 计算对应的网格位置
    uint32 cellLength = m_pGridWorld->GetCellLength();

    m_GridPosX = (m_x / cellLength) > 0 ? int32(m_x / cellLength) : int32(m_x / cellLength)-1;
    m_GridPosY = (m_y / cellLength) > 0 ? int32(m_y / cellLength) : int32(m_y / cellLength)-1;
}

void GridObject::OnLeaveGridWorld(GridWorld* world)
{
    CnAssert(m_pGridWorld && m_pGridWorld == world);
    m_pGridWorld = NULL;

    m_GridPosX = m_GridPosY = 0;
}



