
#ifndef COPOLYGON_H
#define	COPOLYGON_H

#include <iterator>

#include "sdu.h"
#include "CoMath.h"
#include "CoSegment.h"

/*
 *	只支持凸多边形
 */
class CoPolygon
{
    friend class CoWorld;

public:
    CoPolygon(CoPolygonType m_Type = COPOLYGON_TYPE_WALL);
    CoPolygon(const CoPolygon& polygon);
    virtual ~CoPolygon();

    class SegmentIterator
    {
        uint32 index;
        CoPolygon* m_Polygon;
    public:
        SegmentIterator(int32 index, CoPolygon* polygon) :index(index), m_Polygon(polygon) {}
        SegmentIterator(const SegmentIterator& rhs) : index(rhs.index), m_Polygon(rhs.m_Polygon) {}
        SegmentIterator& operator++()
        {
            ++index;
            return *this;
        }

        SegmentIterator operator++(int)
        {
            SegmentIterator tmp(index, m_Polygon);
            operator++();
            return tmp;
        }

        bool operator==(const SegmentIterator& rhs) const {return index == rhs.index;}
        bool operator!=(const SegmentIterator& rhs) const {return index != rhs.index;}


        CoSegment* operator*()
        {
            uint32 indexNext = index + 1 >= m_Polygon->m_Points.size() ? 0 : index + 1;
            return CnNew CoSegment(m_Polygon->m_Points[index], m_Polygon->m_Points[indexNext], m_Polygon);
        }
    };

    bool TestPoint(const CoVec2&) const;
    bool TestCircle(const CoVec2& center, real radius) const;
    bool TestPolygon(const CoPolygon& other) const;

    void append(const CoVec2& point);
    void set(const std::vector<CoVec2>& points);

    SegmentIterator SegmentBegin() { return SegmentIterator(0, this); }
    SegmentIterator SegmentEnd() { return SegmentIterator(m_Points.size(), this); }

    const std::vector<CoVec2>& Points() const { return m_Points; }

    const CoVec2& TopLeft() const { return m_TopLeft; }
    const CoVec2& BottomRight() const { return m_BottomRight; }
    CoVec2 Center() const;

    CoPolygonType Type() const {return m_Type;}

    static CoPolygon* Rect(CoVec2 topLeft, CoVec2 bottomRight, enum CoPolygonType = COPOLYGON_TYPE_WALL);

    void*  m_UserObject;                // 三角形节点

private:
    static void ProjectOnAxis(CoVec2 axis, const CoPolygon* polygon, real* min, real* max);      // 多边形在轴上的投影

private:
    std::vector<CoVec2> m_Points;		// 构成多边形的点

    CoVec2 m_TopLeft;					// 包围框
    CoVec2 m_BottomRight;

    CoPolygonType m_Type;

};

#endif	/* COPOLYGON_H */

