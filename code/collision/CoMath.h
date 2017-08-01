/*
*	物体系统数学库
*/
#ifndef CoMath_h__
#define CoMath_h__

#include <cmath>
#include <float.h>
#include <limits>
#include <iostream>
#include "sdtype.h"
#include "math2d.h"
#include "sdmath.h"

inline bool CoIsValid(real x)
{
    if (Crown::isnan(x))
        return false;

    real infinity = std::numeric_limits<real>::infinity();
    return -infinity < x && x < infinity;
}

inline real CoInvSqrt(real x)
{
    union
    {
        real x;
        int i;
    } convert;

    convert.x = x;
    real xhalf = 0.5f * x;
    convert.i = 0x5f3759df - (convert.i >> 1);
    x = convert.x;
    x = x * (1.5f - xhalf * x * x);
    return x;
}

#define	CoAtan2(y, x)	atan2f(y, x)

/// A 2D column vector.
struct CoVec2
{
    /// Default constructor does nothing (for performance).
    CoVec2() {}

    /// Construct using coordinates.
    CoVec2(int32 x, int32 y) : x((real)x), y((real)y) {}
    CoVec2(real x, real y) : x(x), y(y) {}
    CoVec2(const CoVec2& another) : x(another.x), y(another.y) {}

    /// Set this vector to all zeros.
    void SetZero() { x = 0.0f; y = 0.0f; }

    /// Set this vector to some specified coordinates.
    void Set(real x_, real y_) { x = x_; y = y_; }

    void SetAngle(real length, real angle) {x = length * cos(angle); y = length * sin(angle);}

    /// Negate this vector.
    CoVec2 operator -() const { CoVec2 v; v.Set(-x, -y); return v; }

    /// Read from and indexed element.
    real operator () (int i) const
    {
        return (&x)[i];
    }

    /// Write to an indexed element.
    real& operator () (int i)
    {
        return (&x)[i];
    }

    /// Add a vector to this vector.
    void operator += (const CoVec2& v)
    {
        x += v.x; y += v.y;
    }

    /// Subtract a vector from this vector.
    void operator -= (const CoVec2& v)
    {
        x -= v.x; y -= v.y;
    }

    /// Multiply this vector by a scalar.
    void operator *= (real a)
    {
        x *= a; y *= a;
    }

    /// Get the length of this vector (the norm).
    real Length() const
    {
        return sqrt(x * x + y * y);
    }

    /// Get the length squared. For performance, use this instead of
    /// PhyVec2::Length (if possible).
    real LengthSquared() const
    {
        return x * x + y * y;
    }

    /// Convert this vector into a unit vector. Returns the length.
    real Normalize()
    {
        real length = Length();
        if (length < FLT_EPSILON)
        {
            return 0.0f;
        }
        real invLength = 1.0f / length;
        x *= invLength;
        y *= invLength;

        return length;
    }

    /// Does this vector contain finite coordinates?
    bool IsValid() const
    {
        return CoIsValid(x) && CoIsValid(y);
    }
    // [-pi, pi)
    real GetAngle() const
    {
        return static_cast<real>(Crown::Angle(0.0, 0.0, x, y));
    }

    // Negate this point.
    void neg() 
    {
        x = - x;
        y = - y;
    }

    bool IsZero()
    {
        return x==0 && y==0;
    }

    real x, y;
};

/// Perform the dot product on two vectors.
inline real Vec2Dot(const CoVec2& a, const CoVec2& b)
{
    return a.x * b.x + a.y * b.y;
}

/// Perform the cross product on two vectors. In 2D this produces a scalar.
inline real Vec2Cross(const CoVec2& a, const CoVec2& b)
{
    return a.x * b.y - a.y * b.x;
}

/// Perform the cross product on a vector and a scalar. In 2D this produces a vector.
inline CoVec2 Vec2Cross(const CoVec2& a, real s)
{
    return CoVec2(s * a.y, -s * a.x);
}

/// Perform the cross product on a scalar and a vector. In 2D this produces
/// a vector.
inline CoVec2 Vec2Cross(real s, const CoVec2& a)
{
    return CoVec2(-s * a.y, s * a.x);
}

/// Add two vectors component-wise.
inline CoVec2 operator + (const CoVec2& a, const CoVec2& b)
{
    return CoVec2(a.x + b.x, a.y + b.y);
}

/// Subtract two vectors component-wise.
inline CoVec2 operator - (const CoVec2& a, const CoVec2& b)
{
    return CoVec2(a.x - b.x, a.y - b.y);
}

inline CoVec2 operator * (real s, const CoVec2& a)
{
    return CoVec2(s * a.x, s * a.y);
}

inline CoVec2 operator * (const CoVec2& p, real s)
{
    return CoVec2(p.x * s, p.y * s);
}

inline bool operator == (const CoVec2& a, const CoVec2& b)
{
    return a.x == b.x && a.y == b.y;
}

inline bool operator != (const CoVec2& a, const CoVec2& b)
{
    return !(a == b);
}

inline real Vec2Distance(const CoVec2& a, const CoVec2& b)
{
    CoVec2 c = a - b;
    return c.Length();
}

inline real Vec2DistanceSquared(const CoVec2& a, const CoVec2& b)
{
    CoVec2 c = a - b;
    return Vec2Dot(c, c);
}

//判断两个点是否相等
inline bool IsEqual(const CoVec2& pointA, const CoVec2& pointB)
{
    if((pointA.x == pointB.x) && (pointA.y == pointB.y))
        return true;
    return false;
}

// 点p到直线(l1, l2)的距离
real PointDistanceToLine(const CoVec2& p, const CoVec2& l1, const CoVec2& l2);
// 线段到点p的最近点
CoVec2 ClosestPointToSegment(const CoVec2& p, const CoVec2& a, const CoVec2& b);

inline std::ostream& operator <<(std::ostream& stream, const CoVec2& vec)
{
    return stream << "(" << vec.x << ", " << vec.y << ")";
}

#endif // CoMath_h__
