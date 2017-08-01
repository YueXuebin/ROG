#include "CollisionPCH.h"
#include "CoAreaSearcher.h"
#include "CoWorld.h"
#include "CoDynamicObject.h"

#include <set>

CoAreaSearcher::CoAreaSearcher(CoWorld* pWorld)
: m_pWorld(pWorld)
{
}

CoAreaSearcher::~CoAreaSearcher()
{

}
/*
void CoAreaSearcher::SearchInRoundness(
	std::vector<CoDynamicObject*>& container, real centreX, real centreY, real radius) const
{
#ifdef A_FASTER_ALGORITHM
#else
	SearchInRoundnessSlow(container, centreX, centreY, radius);
#endif
}

void CoAreaSearcher::SearchInRoundnessSlow(
	std::vector<CoDynamicObject*>& container, real centreX, real centreY, real radius) const
{
	std::set<CoDynamicObject*> visited;

	// Traverse from top left cell to bottom right.
	CoWorld::Cell* pLTCell = m_pWorld->GetCellGridByCoordNoOut((centreX - radius), (centreY - radius));
	CoWorld::Cell* pRBCell = m_pWorld->GetCellGridByCoordNoOut((centreX + radius), (centreY + radius));

	for(int i = pLTCell->m_IndexY; i <= pRBCell->m_IndexY; ++i)
	{
		for(int j = pLTCell->m_IndexX; j <= pRBCell->m_IndexX; ++j)
		{
			CoWorld::Cell* cell = m_pWorld->GetCellGrid(j, i);
            const std::vector<CoDynamicObject*>& objects = cell->GetCollObjects();
			for(std::vector<CoDynamicObject*>::const_iterator itor = objects.begin();
					itor != objects.end(); ++itor)
			{
				CoDynamicObject* obj = *itor;
				if (visited.find(obj) != visited.end())
					continue;
				visited.insert(obj);

				double distX = obj->GetPosX() - centreX;
                double distY = obj->GetPosY() - centreY;
				double radiusTotal = radius + obj->GetRadius();

				if ((distX * distX + distY * distY) < (radiusTotal * radiusTotal))
					container.push_back(obj);
			}
		}
	}

    //DBG("Got %u", container.size());
}

void CoAreaSearcher::SearchInAngle(std::vector<CoDynamicObject *> &container,
	real centreX, real centreY, real angleCenter, real angleSeperation, real range)
{
    CnVerify(0 < angleSeperation && angleSeperation < M_PI);

	std::vector<CoDynamicObject *> nearbyObjects;
	SearchInRoundness(nearbyObjects, centreX, centreY, range);

	for(std::vector<CoDynamicObject*>::const_iterator itor = nearbyObjects.begin();
			itor != nearbyObjects.end(); ++itor)
	{
		CoDynamicObject* obj = *itor;

		real angleToObj = Crown::Angle(centreX, centreY, obj->GetPosX(), obj->GetPosY());

        // No comment, just feel free to ask me. --Logan
        real a = Crown::NormalizeAngle(angleToObj - angleCenter);
        a = std::abs(a);
        if (a > M_PI)
            a = M_PI * 2 - a;

        if (a < angleSeperation)
			container.push_back(obj);
	}
}

*/