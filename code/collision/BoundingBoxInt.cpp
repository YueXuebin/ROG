#include "CollisionPCH.h"
#include "BoundingBoxInt.h"

BoundingBoxInt::BoundingBoxInt(const CoVec2DInt& size)
    : m_size(size)
{
    CheckValid();
}

BoundingBoxInt::BoundingBoxInt(const CoVec2DInt& pos, const CoVec2DInt& size)
    : m_pos(pos),
    m_size(size)
{
    CheckValid();
}

BoundingBoxInt::BoundingBoxInt(int minX, int minY, int maxX, int maxY)
    : m_pos(minX, minY),
    m_size(maxX - minX + 1, maxY - minY + 1)
{
    CheckValid();
}

BoundingBoxInt::BoundingBoxInt(const BoundingBoxInt& bb)
    : m_pos(bb.m_pos),
    m_size(bb.m_size)
{
    CheckValid();
}

void BoundingBoxInt::operator =(const BoundingBoxInt& rhs)
{
    m_pos = rhs.m_pos;
    m_size = rhs.m_size;
}

void BoundingBoxInt::Inflate(const CoVec2DInt& size)
{
    CnAssert(size.IsValidSize());
    m_pos -= size;
    CoVec2DInt size2(2 * size.X + 1, 2 * size.Y + 1);
    m_size += size2;
    CheckValid();
}

void BoundingBoxInt::Translate(const CoVec2DInt& offset)
{
    m_pos += offset;
}

void BoundingBoxInt::Intersect(const BoundingBoxInt& rhs)
{
    CoVec2DInt pos(Crown::SDMax(rhs.m_pos.X, m_pos.X),
        Crown::SDMax(rhs.m_pos.Y, m_pos.Y));
    CoVec2DInt ur = m_pos;  // upper-right pos
    ur += m_size;
    CoVec2DInt ur2 = rhs.m_pos;
    ur2 += rhs.m_size;
    CoVec2DInt posUR(Crown::SDMin(ur.X, ur2.X),
        Crown::SDMin(ur.Y, ur2.Y));
    posUR -= pos;   // got size
    if (posUR.X <= 0 || posUR.Y <= 0)
    {
        m_size = CoVec2DInt(0, 0);
    }
    else
    {
        m_pos = pos;
        m_size = posUR;
    }
    CheckValid();
}

bool BoundingBoxInt::Contains(const CoVec2DInt& pos) const
{
    CoVec2DInt pos0(pos);
    pos0 -= m_pos;
    return pos0.IsInside(m_size);
}
