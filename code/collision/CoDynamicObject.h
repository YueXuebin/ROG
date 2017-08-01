#pragma once

/*
 *	可移动对象
 */

#include "CoObject.h"
#include "BoundingBoxInt.h"

struct CoSegment;

class CoDynamicObject : public CoObject
{
	friend class CoWorld;

public:
	CoDynamicObject(real x, real y, real radius, CoObjectOwner* pOwner = NULL);
	virtual ~CoDynamicObject();

	void	SetPosition(const CoVec2 &pos);
	void	SetPosition(real PosX, real PosY);
	real	GetPosX() const {return m_PosX;}
	real	GetPosY() const {return m_PosY;}
	CoVec2	GetPosition() const { return CoVec2(m_PosX, m_PosY); }
    BoundingBoxInt GetBoundingBox() {return m_BBox;}
        
	void	SetAngle(real angle);
	real	GetAngle() const {return m_Angle;}
	void	SetRadius(real Radius);
	real	GetRadius() const {return m_Radius;}
	bool	CollidedWith(const CoDynamicObject& obj) const;
	bool	CollidedWith(const CoSegment& segment) const;
	bool	CollidedWith(const CoVec2& center, real radius) const;
	real	GetAngleIntersectionWithDirection(const CoDynamicObject& obj) const;
	real	GetAngleIntersectionWithDirection(const CoVec2& position) const;

	real	DistanceFrom(const CoDynamicObject& obj) const;
	real	AngleTo(const CoDynamicObject& obj) const;
	real	AngleTo(const CoVec2& pos) const;

    void    NotifyNeighbours(int32 msgId, void* pData, const std::set<CoObjectOwner*>& ignores = std::set<CoObjectOwner*>());

	// Return the first object found which collided with this.
	CoDynamicObject* GetFirstCollided() const;

	// Return objects found.
    int32 GetAllCollided(std::set<CoDynamicObject *>* container) const;

	bool	CanMoveTo(real nextX, real nextY) const;


protected:
	virtual void UpdatePos();
    virtual void NotifyBoundBoxUpdated(const CoVec2DInt& cellPos);

    void    onEnterWorld(CoWorld* pWorld);
    void    onLeaveWorld(CoWorld* pWorld);

private:
	real	m_PosX, m_PosY;
    BoundingBoxInt m_BBox;  // Used for collision detection.
	real	m_Radius, m_Angle;

    void EnterCell(const CoVec2DInt& cellPos);
    void EnterCollCell();

    void LeaveCell();
    void LeaveCollCell();

    void UpdateBBox();
    BoundingBoxInt GetBBox(const CoVec2& pos);
};
