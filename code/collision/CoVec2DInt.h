#ifndef CoVec2DInt_h__
#define CoVec2DInt_h__

#include "Assertion.h"

struct CoVec2DInt
{
    CoVec2DInt() : X(0), Y(0) {}
    CoVec2DInt(int x, int y) : X(x), Y(y) {}
    CoVec2DInt(const CoVec2DInt& vec) : X(vec.X), Y(vec.Y) {}
    void operator =(const CoVec2DInt& rhs) { X = rhs.X; Y = rhs.Y; }

    bool operator ==(const CoVec2DInt& rhs) { return X == rhs.X && Y == rhs.Y; }
    void operator +=(const CoVec2DInt& rhs) { X += rhs.X; Y += rhs.Y; }
    void operator -=(const CoVec2DInt& rhs) { X -= rhs.X; Y -= rhs.Y; }
    void operator *=(int scale) { X *= scale; Y *= scale; }
    bool IsValidSize() const { return X >= 0 && Y >= 0; }
    bool IsInside(const CoVec2DInt& size) const
    {
        CnAssert(size.IsValidSize());
        return X >= 0 && Y >= 0 &&
            X < size.X && Y < size.Y;
    }

    int X, Y;
};

#endif // CoVec2DInt_h__