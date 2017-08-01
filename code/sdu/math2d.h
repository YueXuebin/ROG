#pragma once

#ifndef MATH2D_H
#define MATH2D_H

#include <cmath>

namespace Crown
{

#define RADIAN_TO_ANGLE(x)  int32((x)*180.f/M_PI)
#define ANGLE_TO_RADIAN(x)  real((x)*M_PI/180.f)

inline real DistanceSquared(real x1, real y1, real x2, real y2)
{
	real dx = x2 - x1;
	real dy = y2 - y1;
	return dx * dx + dy * dy;
}

/*
	Return the distance of the given point in 2D.
*/
inline real Distance(real x1, real y1, real x2, real y2)
{
	real dx = x2 - x1;
	real dy = y2 - y1;
	return sqrt(dx * dx + dy * dy);
}

/*
 * Normalize angle to [-pi, pi).
 */
inline real NormalizeAngle(real angle, real centre = 0)
{
    // The result will be [centre - pi, centre + pi).
    const real TWO_PI = real(M_PI) * 2;
    return angle - TWO_PI * floor((angle + real(M_PI) - centre) / TWO_PI);
}

/*
	Return the angle of the given vector in 2D.
	The value of angle will be in [-pi, pi).
*/
inline real Angle(real startX, real startY, real endX, real endY)
{
	real dx = endX - startX;
	real dy = endY - startY;
    return atan2(dy, dx);
}

} // namespace

#endif // MATH2D_H
