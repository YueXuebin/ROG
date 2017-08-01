#include "CollisionPCH.h"
#include "CoObject.h"
#include "CoDynamicObject.h"
#include "CoWorld.h"
#include <limits>

CoObjectOwner::CoObjectOwner():
	m_pCoDynObject(nullptr)
{

}

CoDynamicObject *CoObjectOwner::CreateCoObject(real x, real y, real radius)
{
	m_pCoDynObject = CnNew CoDynamicObject(x, y, radius, this);

    return m_pCoDynObject;
}


CoObject::CoObject(CoObjectOwner* pOwner) :
	m_pOwnerWorld(NULL),
	m_pOwner(pOwner),
    m_CellPosInited(false),
    m_layer(0)
{

}

CoObject::~CoObject()
{

}

bool CoObject::CollisionTestWithCell()
{
	const CoWorld::Cell* pCellGrid =
        m_pOwnerWorld->GetCellGrid(m_CellPos.X, m_CellPos.Y);
	if (pCellGrid->GetState() == CELL_BLOCK)
		return true;

	return false;
}

void CoObject::onEnterWorld(CoWorld* pWorld)
{
    m_CellPosInited = false;
    m_pOwnerWorld = pWorld;

    UpdatePos();
}

void CoObject::onLeaveWorld(CoWorld* pWorld)
{
    m_pOwnerWorld = NULL;

}
