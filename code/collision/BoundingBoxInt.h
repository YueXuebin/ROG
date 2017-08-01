#ifndef BoundingBoxInt_h__
#define BoundingBoxInt_h__



#include "CoVec2DInt.h"

class BoundingBoxInt
{
public:
    BoundingBoxInt() {}
    BoundingBoxInt(const CoVec2DInt& size);
    BoundingBoxInt(const CoVec2DInt& pos, const CoVec2DInt& size);
    BoundingBoxInt(const BoundingBoxInt& bb);
    BoundingBoxInt(int minX, int minY, int maxX, int maxY);
    void operator =(const BoundingBoxInt& rhs);
    bool operator ==(const BoundingBoxInt& rhs)
    { return m_pos == rhs.m_pos && m_size == rhs.m_size; }

    const CoVec2DInt& Pos() const { return m_pos; }
    const CoVec2DInt& Size() const { return m_size; }

    void Inflate(const CoVec2DInt& size);
    void Translate(const CoVec2DInt& offset);
    void Intersect(const BoundingBoxInt& rhs);
    bool Contains(const CoVec2DInt& pos) const;

private:
    CoVec2DInt m_pos;   // the inside point with minimum x and y
    CoVec2DInt m_size;

    void CheckValid() const { CnAssert(m_size.IsValidSize()); }
};

#endif // BoundingBoxInt_h__