#pragma once

#ifndef SEGMENT_H
#define SEGMENT_H

#include "CoMath.h"
#include "CoCostants.h"

class CoPolygon;

struct CoSegment
{
	enum Side
	{
		LEFT,
		RIGHT
	};

	enum Axis
	{
		NONE, X, Y
	};

	CoVec2 start, end;
	CoPolygonType type;
	CoPolygon* owner;
	enum Axis parallelWith;

	CoSegment(CoVec2 start, CoVec2 end, CoPolygon* owner);
	CoSegment(CoVec2 start, CoVec2 end, CoPolygonType type = COPOLYGON_TYPE_WALL);

	enum Side SideOf(CoVec2 point) const;

	CoVec2 Vector() const;

	real LengthSquared() const { return Vector().LengthSquared(); }
	real Length() const { return Vector().Length(); }

	real ProjectionX(real y) const;
	real ProjectionY(real x) const;

	Axis GetParallel(CoVec2 start, CoVec2 end) const;
private:
	void SetSlope();
	real m_Slope;
};

#endif // SEGMENT_H
