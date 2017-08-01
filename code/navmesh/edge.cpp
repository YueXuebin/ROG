#include "navmesh_pch.h"
#include "edge.h"
#include "math2d.h"


Edge::Edge(const CoVec2& point1, const CoVec2& point2)
{
    bool b_swap = false;
    if(point1.y > point2.y)
        b_swap = true;
    else if (point1.y == point2.y)
    {
        if(point1.x == point2.x)
            CnDbg("repeat point");

        if(point1.x > point2.x)
            b_swap = true;
    }

    if(b_swap)
    {
        m_point2 = point1;
        m_point1 = point2;
    }
    else
    {
        m_point1 = point1;
        m_point2 = point2;
    }
}

Edge::~Edge(void)
{
}

bool Edge::HavePoint(const CoVec2& point)
{
    return (IsEqual(m_point1, point) || IsEqual(m_point2, point));
}

real Edge::GetDistanceSquared()
{
    return Crown::DistanceSquared(m_point1.x , m_point1.y, m_point2.x , m_point2.y);
}
