#include "CollisionPCH.h"
#include "CoSegment.h"
#include "CoPolygon.h"

#include <exception>

CoSegment::CoSegment(CoVec2 start, CoVec2 end, CoPolygon* owner) :
	start(start), end(end), type(owner->Type()), owner(owner),
	parallelWith(GetParallel(start, end))
{
	SetSlope();
}

CoSegment::CoSegment(CoVec2 start, CoVec2 end, CoPolygonType type) :
	start(start), end(end), type(type), owner(nullptr),
	parallelWith(GetParallel(start, end))
{
	SetSlope();
}

CoVec2 CoSegment::Vector() const
{
	return CoVec2(end.x - start.x, end.y - start.y);
}

void CoSegment::SetSlope()
{
	if (parallelWith == Y)
		return; // (end.x - start.x) equals 0, m_Slope is useless.

	m_Slope = (end.y - start.y) / (end.x - start.x);
}

CoSegment::Axis CoSegment::GetParallel(CoVec2 start, CoVec2 end) const
{
	if (std::abs(start.x - end.x) < FLT_EPSILON)
		return Y;

	if (std::abs(start.y - end.y) < FLT_EPSILON)
		return X;

	return NONE;
}

CoSegment::Side CoSegment::SideOf(CoVec2 point) const
{
	float x = point.x - start.x;
	float y = point.y - start.y;

	if ((Vector().x * y - Vector().y * x) < 0)
		return LEFT;
	return RIGHT;
}

real CoSegment::ProjectionX(real y) const
{
	if (parallelWith == X)
		throw std::exception();

	if (parallelWith == Y)
		return start.x;

	return (y - start.y) / m_Slope + start.x;
}

real CoSegment::ProjectionY(real x) const
{
	if (parallelWith == Y)
		throw std::exception();

	if (parallelWith == X)
		return start.y;

	return (x - start.x) * m_Slope + start.y;
}

