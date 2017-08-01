#include "CollisionPCH.h"
#include "CoMath.h"

const CoVec2 CoVec2_zero(0.0f, 0.0f);


real PointDistanceToLine(const CoVec2& p, const CoVec2& l1, const CoVec2& l2)
{
	real A = l1.y - l2.y;
	real B = l2.x - l1.x;
	real C = l1.x * l2.y - l2.x * l1.y;

	real d = std::abs(A*p.x+B*p.y+C)/std::sqrt(A*A+B*B);
	return d;
}

CoVec2 ClosestPointToSegment(const CoVec2& p, const CoVec2& a, const CoVec2& b)
{
	CoVec2 ab = b - a;
	real t = Vec2Dot((p - a), ab)/ Vec2Dot(ab, ab);
	if(t<0.0f) t=0.0f;
	if(t>1.0f) t=1.0f;

	return a+t*ab;
}