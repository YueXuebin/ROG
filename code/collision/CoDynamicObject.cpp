#include "CollisionPCH.h"
#include "CoDynamicObject.h"
#include "CoWorld.h"
#include "BoundingBoxInt.h"

const int32 NOTIFY_APPEAR_RANGE = 2;
const int32 NOTIFY_DISAPPEAR_RANGE = NOTIFY_APPEAR_RANGE + 1;

CoDynamicObject::CoDynamicObject(real x, real y, real radius, CoObjectOwner* pOwner) :
	CoObject(pOwner),
	m_PosX(x),
	m_PosY(y),
	m_Radius(radius)
{
    CnAssert(m_Radius > 0);
	//CnVerify(pOwnerWorld);
	UpdatePos();
}

CoDynamicObject::~CoDynamicObject()
{
    LeaveCell();

#if 0
    for(int y = m_pOwnerWorld->GetCellMinY(); y <= m_pOwnerWorld->GetCellMaxY(); y++)
    {
        for(int x = m_pOwnerWorld->GetCellMinX(); x <= m_pOwnerWorld->GetCellMaxX(); x++)
        {
            // This object should be cleared.
            CnVerify(!m_pOwnerWorld->GetCellGrid(x, y)->GetDynObjects().has(this));
        }
    }
#endif

}

void CoDynamicObject::onEnterWorld(CoWorld* pWorld)
{
    CoObject::onEnterWorld(pWorld);
}

void CoDynamicObject::onLeaveWorld(CoWorld* pWorld)
{
    LeaveCell();
    CoObject::onLeaveWorld(pWorld);
}

void CoDynamicObject::EnterCell(const CoVec2DInt& cellPos)
{
    m_pOwnerWorld->GetCellGrid(cellPos.X, cellPos.Y)->DynObjectEnter(this);
    EnterCollCell();
}

void CoDynamicObject::EnterCollCell()
{
    BoundingBoxInt bbox = GetBBox(CoVec2(m_PosX, m_PosY));
    for (int y = bbox.Pos().Y, dy = 0; dy < bbox.Size().Y; y++, dy++)
    {
        for (int x = bbox.Pos().X, dx = 0; dx < bbox.Size().X; x++, dx++)
        {
            m_pOwnerWorld->GetCellGrid(x, y)->CollObjectEnter(this);
        }
    }
}

void CoDynamicObject::LeaveCell()
{
    if(m_pOwnerWorld)
    {
        CnAssert(m_CellPosInited);
        m_pOwnerWorld->GetCellGrid(m_CellPos.X, m_CellPos.Y)->DynObjectLeave(this);
        LeaveCollCell();
    }
}

void CoDynamicObject::LeaveCollCell()
{
    for (int y = m_BBox.Pos().Y, dy = 0; dy < m_BBox.Size().Y; y++, dy++)
    {
        for (int x = m_BBox.Pos().X, dx = 0; dx < m_BBox.Size().X; x++, dx++)
        {
            m_pOwnerWorld->GetCellGrid(x, y)->CollObjectLeave(this);
        }
    }
}


void CoDynamicObject::SetPosition(const CoVec2& pos)
{
	SetPosition(pos.x, pos.y);
}

void CoDynamicObject::SetPosition( real PosX, real PosY )
{
	m_PosX = PosX;
	m_PosY = PosY;

	UpdatePos();
}

void CoDynamicObject::SetRadius( real Radius )
{
	m_Radius = Radius;

	UpdatePos();
}

void CoDynamicObject::SetAngle(real angle)
{
	m_Angle = angle;
}

void CoDynamicObject::UpdatePos()
{
    if(!m_pOwnerWorld)
        return;

    // obj新占的格
    CoVec2DInt cellPos = m_pOwnerWorld->GetCellPosByCoordNoOut(m_PosX, m_PosY);

    if (m_CellPosInited)
    {
        LeaveCell();
    }

    EnterCell(cellPos);
    NotifyBoundBoxUpdated(cellPos);

    m_CellPos = cellPos;
    UpdateBBox();

    m_CellPosInited = true;
}

BoundingBoxInt CoDynamicObject::GetBBox(const CoVec2& pos)
{
    CoVec2DInt posLL = m_pOwnerWorld->GetCellPosByCoordNoOut(pos.x - m_Radius, pos.y - m_Radius);
    CoVec2DInt posUR = m_pOwnerWorld->GetCellPosByCoordNoOut(pos.x + m_Radius, pos.y + m_Radius);
    CoVec2DInt size(posUR.X - posLL.X + 1, posUR.Y - posLL.Y + 1);
    return BoundingBoxInt(posLL, size);
}

void CoDynamicObject::UpdateBBox()
{
    m_BBox = GetBBox(CoVec2(m_PosX, m_PosY));
}

void CoDynamicObject::NotifyBoundBoxUpdated(const CoVec2DInt& cellPos)
{
    BoundingBoxInt worldBB(m_pOwnerWorld->GetCellMinX(), m_pOwnerWorld->GetCellMinY(),
        m_pOwnerWorld->GetCellMaxX(), m_pOwnerWorld->GetCellMaxY());

    // appear notify
    {
        BoundingBoxInt oldBB;
        if (m_CellPosInited)
        {
            oldBB = BoundingBoxInt(m_CellPos, CoVec2DInt(0, 0));
            oldBB.Inflate(CoVec2DInt(NOTIFY_APPEAR_RANGE, NOTIFY_APPEAR_RANGE));
            oldBB.Intersect(worldBB);
        }

        BoundingBoxInt newBB(cellPos, CoVec2DInt(0, 0));
        newBB.Inflate(CoVec2DInt(NOTIFY_APPEAR_RANGE, NOTIFY_APPEAR_RANGE));
        newBB.Intersect(worldBB);

        for (int y = newBB.Pos().Y, dy = 0; dy < newBB.Size().Y; y++, dy++)
        {
            for (int x = newBB.Pos().X, dx = 0; dx < newBB.Size().X; x++, dx++)
            {
                CoVec2DInt pos(x, y);
                if (oldBB.Contains(pos)) continue;
                CoWorld::Cell* cell = m_pOwnerWorld->GetCellGrid(x, y);
                //const DynamicObjectArray& objects = cell->GetDynObjects();
                //for (std::vector<CoDynamicObject*>::iterator itor = objects.begin(); itor != objects.end(); ++itor)
                //{
                //    CoDynamicObject* obj = *itor;
                //    if (obj == this) continue;  // filter self out
                //    auto p1 = obj->GetOwner();
                //    if (!p1) continue;
                //    auto p2 = GetOwner();
                //    if (!p2) continue;
                //    p1->OnObjectAppeared(p2);
                //    p2->OnObjectAppeared(p1);
                //}
            }
        }
    }
    // disappear notify
    {
        if (!m_CellPosInited) return;

        BoundingBoxInt oldBB(m_CellPos, CoVec2DInt(0, 0));
        oldBB.Inflate(CoVec2DInt(NOTIFY_DISAPPEAR_RANGE, NOTIFY_DISAPPEAR_RANGE));
        oldBB.Intersect(worldBB);

        BoundingBoxInt newBB(cellPos, CoVec2DInt(0, 0));
        newBB.Inflate(CoVec2DInt(NOTIFY_DISAPPEAR_RANGE, NOTIFY_DISAPPEAR_RANGE));
        newBB.Intersect(worldBB);

        for (int y = oldBB.Pos().Y, dy = 0; dy < oldBB.Size().Y; y++, dy++)
        {
            for (int x = oldBB.Pos().X, dx = 0; dx < oldBB.Size().X; x++, dx++)
            {
                CoVec2DInt pos(x, y);
                if (newBB.Contains(pos)) continue;
                CoWorld::Cell* cell = m_pOwnerWorld->GetCellGrid(x, y);
                const std::vector<CoDynamicObject*>& objects = cell->GetDynObjects();
                /*for (auto itor = objects.begin();
                    itor != objects.end(); ++itor)
                {
                    CoDynamicObject* obj = *itor;
                    if (obj == this) continue;
                    auto p1 = obj->GetOwner();
                    if (!p1) continue;
                    auto p2 = GetOwner();
                    if (!p2) continue;
                    p1->OnObjectDisappeared(p2);
                    p2->OnObjectDisappeared(p1);
                }*/
            }
        }
    }
}

real CoDynamicObject::GetAngleIntersectionWithDirection(const CoDynamicObject& obj) const
{
	return GetAngleIntersectionWithDirection(obj.GetPosition());
}

real CoDynamicObject::GetAngleIntersectionWithDirection(const CoVec2& position) const
{
	real angleToObj = AngleTo(position);
	return Crown::NormalizeAngle(angleToObj - GetAngle());
}

bool CoDynamicObject::CollidedWith(const CoVec2& center, real radius) const
{
    if(m_Radius <= 0 || radius <= 0)
        return false;       // 半径为0,则无碰撞

	real centerOffset = (GetPosition() - center).Length();
	return (centerOffset - GetRadius() - radius) < 0;
}

bool CoDynamicObject::CollidedWith(const CoDynamicObject& obj) const
{
	if (GetOwner() && !GetOwner()->MayCollidedWith(*obj.GetOwner()))
		return false;

	if (this == &obj)
		return false;

	real centerOffset = (GetPosition() - obj.GetPosition()).Length();
	return (centerOffset - GetRadius() - obj.GetRadius()) < 0;
}

bool CoDynamicObject::CollidedWith(const CoSegment& segment) const
{
	if (GetOwner() && !GetOwner()->MayCollidedWith(segment))
		return false;

	// Inspired by http://www.csie.ntnu.edu.tw/~u91029/PointLinePlane.html
	CoVec2 pos = GetPosition();
	CoVec2 segVector = segment.Vector();

	CoVec2 startToThis = pos - segment.start;
	if (Vec2Dot(startToThis, segVector) < 0)
		return startToThis.Length() < GetRadius();

	CoVec2 endToThis = pos - segment.end;
	if (Vec2Dot(endToThis, -segVector) < 0)
		return endToThis.Length() < GetRadius();

	real area = fabs(Vec2Cross(startToThis, endToThis));
	real distance = area / segVector.Length();

	return distance < GetRadius();
}

real CoDynamicObject::DistanceFrom(const CoDynamicObject& obj) const
{
	real centerOffset = (GetPosition() - obj.GetPosition()).Length();
	real distance = centerOffset - GetRadius() - obj.GetRadius();
	return distance > 0 ? distance : 0;
}

real CoDynamicObject::AngleTo(const CoDynamicObject& obj) const
{
	return AngleTo(obj.GetPosition());
}

real CoDynamicObject::AngleTo(const CoVec2& pos) const
{
    return Crown::Angle(GetPosX(), GetPosY(), pos.x, pos.y);
}

void CoDynamicObject::NotifyNeighbours(int32 msgId, void *pData, const std::set<CoObjectOwner*>& ignores)
{
    BoundingBoxInt worldBB(m_pOwnerWorld->GetCellMinX(), m_pOwnerWorld->GetCellMinY(),
        m_pOwnerWorld->GetCellMaxX(), m_pOwnerWorld->GetCellMaxY());

    BoundingBoxInt bbox(m_CellPos, CoVec2DInt(0, 0));
    bbox.Inflate(CoVec2DInt(NOTIFY_DISAPPEAR_RANGE, NOTIFY_DISAPPEAR_RANGE));
    bbox.Intersect(worldBB);

    for (int y = bbox.Pos().Y, dy = 0; dy < bbox.Size().Y; y++, dy++)
    {
        for (int x = bbox.Pos().X, dx = 0; dx < bbox.Size().X; x++, dx++)
        {
            CoWorld::Cell* cell = m_pOwnerWorld->GetCellGrid(x, y);
            const std::vector<CoDynamicObject*>& objects = cell->GetDynObjects();
            /*for (auto itor = objects.begin();
                itor != objects.end(); ++itor)
            {
                CoDynamicObject* obj = *itor;
                CoObjectOwner* owner = obj->GetOwner();
                if (!owner) continue;
                owner->OnNotify(msgId, pData);
            }*/
        }
    }
}

CoDynamicObject* CoDynamicObject::GetFirstCollided() const
{
    for (int y = m_BBox.Pos().Y, dy = 0; dy < m_BBox.Size().Y; y++, dy++)
    {
        for (int x = m_BBox.Pos().X, dx = 0; dx < m_BBox.Size().X; x++, dx++)
        {
            CoWorld::Cell* cell = m_pOwnerWorld->GetCellGrid(x, y);
            const std::vector<CoDynamicObject*>& objects = cell->GetCollObjects();
           /* for (auto itor = objects.begin();
                itor != objects.end(); ++itor)
            {
                if (CollidedWith(**itor))
                    return *itor;
            }*/
        }
    }

	return nullptr;
}

int32 CoDynamicObject::GetAllCollided(std::set<CoDynamicObject *>* container) const
{
	int32 before = container->size();
    for (int y = m_BBox.Pos().Y, dy = 0; dy < m_BBox.Size().Y; y++, dy++)
    {
        for (int x = m_BBox.Pos().X, dx = 0; dx < m_BBox.Size().X; x++, dx++)
        {
	        CoWorld::Cell* cell = m_pOwnerWorld->GetCellGrid(x, y);
	        const std::vector<CoDynamicObject*>& objects = cell->GetCollObjects();
	        //for (auto itor = objects.begin();
		       // itor != objects.end(); ++itor)
	        //{
         //       // FIXME: 发现空指针的来源.
         //       CoDynamicObject* p = *itor;
         //       if (p && CollidedWith(*p))
			      //  container->insert(*itor);
	        //}
        }
    }
	return container->size() - before;
}
/*
bool CoDynamicObject::CanMoveTo(real nextX, real nextY) const
{
    BT_PROFILE_FUNC;

    if (Crown::isnan(nextX) || Crown::isnan(nextY))
        return false;

    std::set<CoDynamicObject *> objsCollided;
	std::set<CoSegment *> segmentsTested;
	std::set<CoPolygon *> polygonTested;

	GetAllCollided(&objsCollided);

	CoDynamicObject testObj(m_pOwnerWorld, nextX, nextY, GetRadius(), nullptr);
    const BoundingBoxInt& bbox = testObj.m_BBox;
    for (int y = bbox.Pos().Y, dy = 0; dy < bbox.Size().Y; y++, dy++)
    {
        for (int x = bbox.Pos().X, dx = 0; dx < bbox.Size().X; x++, dx++)
        {
	        CoWorld::Cell* cell = m_pOwnerWorld->GetCellGrid(x, y);
	        // 物理对象的碰撞检测
	        //auto objects = cell->GetCollObjects();
	        //for (auto itor = objects.begin(); itor != objects.end(); ++itor)
	        //{
		       // CoDynamicObject* obj = *itor;
		       // if (obj != this && !objsCollided.has(obj) && testObj.CollidedWith(*obj))
		       // {
			      //  return false;		// 和新的其他角色碰撞
		       // }
		       // objsCollided.insert(obj);
	        //}
            // 世界边缘的碰撞检测
            ///*
	        auto segments = cell->GetSegments();
	        for(auto itor = segments.begin(); itor != segments.end(); ++itor)
	        {
		        Crown::shared_ptr<CoSegment> segment = *itor;
		        if (segmentsTested.has(segment.get()))
			        continue;
		        segmentsTested.insert(segment.get());

		        if (testObj.CollidedWith(*segment))
		        {
			        return false;
		        }

                // 正如里面FIXME所提到的,这一段会有性能问题.
                // 注释之后,快速移动的物体可能会穿过墙壁之类的地方.
                // FIXME: 请通过连续碰撞判断来修补.
        //				// If the owner does not care the type of segment,
        //				// ignore the whole polygon.
        //				if (GetOwner() && !GetOwner()->MayCollidedWith(*segment.get()))
        //					continue;

        //				CoPolygon* polygon = segment->owner;
        //				if (!polygonTested.has(polygon))
        //				{
        //					polygonTested.insert(polygon);

        //					// FIXME: May be performance issue.
        //					bool covered = polygon->TestPoint(testObj.Position());
        //					// The testObj should inside world (COPOLOGON_TYPE_WORLDBORDER),
        //					// out of wall (COPOLOGON_TYPE_WALL) and hole (COPOLOGON_TYPE_HOLE).
        //					if (((polygon->Type() == COPOLYGON_TYPE_WORLDBORDER) && !covered)
        //							|| ((polygon->Type() == COPOLYGON_TYPE_WALL) && covered)
        //							|| ((polygon->Type() == COPOLYGON_TYPE_HOLE) && covered))
        //					{
        //						return false;
        //					}
        //				}
	        } // for segment
            //*
        }
    }

	return true;
}
*/
/*
bool CoDynamicObject::IterateCollision(std::function<bool(CoDynamicObject*)> objCallback, std::function<bool(CoSegment*)> segCallback)
{
	CnHashSet<CoDynamicObject *> objsCollided;
	CnHashSet<CoSegment *> segmentsTested;

    for (int y = m_BBox.Pos().Y, dy = 0; dy < m_BBox.Size().Y; y++, dy++)
    {
        for (int x = m_BBox.Pos().X, dx = 0; dx < m_BBox.Size().X; x++, dx++)
        {
	        CoWorld::Cell* cell = m_pOwnerWorld->GetCellGrid(x, y);
            // 与其他物理对象碰撞检测
	        auto objects = cell->GetCollObjects();
	        for (auto itor = objects.begin(); itor != objects.end(); ++itor)
	        {
		        CoDynamicObject* obj = *itor;
		        if (obj != this && !objsCollided.has(obj) && CollidedWith(*obj) && objCallback(obj))
		        {
                    return true;
		        }
		        objsCollided.insert(obj);
	        }
            // 与线段碰撞检测
	        auto segments = cell->GetSegments();
	        for (auto itor = segments.begin(); itor != segments.end(); ++itor)
	        {
		        Crown::shared_ptr<CoSegment>& segment = *itor;
		        if (!segmentsTested.has(segment.get()) && CollidedWith(*segment) && segCallback(segment.get()))
		        {
                    return true;
		        }
		        segmentsTested.insert(segment.get());
	        }
        }
    }

    return false;
}
*/