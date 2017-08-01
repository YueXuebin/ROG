#include "CollisionPCH.h"
#include "CoPolygon.h"

CoPolygon::CoPolygon(CoPolygonType type) :
m_TopLeft(0, 0), 
m_BottomRight(0, 0), 
m_Type(type),
m_UserObject(NULL)
{
}

CoPolygon::CoPolygon(const CoPolygon& polygon) :
m_Points(polygon.m_Points),
m_TopLeft(polygon.m_TopLeft),
m_BottomRight(polygon.m_BottomRight),
m_Type(polygon.m_Type),
m_UserObject(polygon.m_UserObject)
{

}

CoPolygon::~CoPolygon()
{
}

void CoPolygon::set(const std::vector<CoVec2>& points)
{
    m_Points.clear();

    for(uint32 i=0; i<points.size(); i++)
    {
        append(points[i]);
    }
}

void CoPolygon::append(const CoVec2& point)
{
    m_Points.push_back(point);

    if(m_Points.size() == 1)
    {
        m_TopLeft.x = m_TopLeft.y = (real)99999999;		
        m_BottomRight.x = m_BottomRight.y = (real)-999999999;
    }

    m_TopLeft.x = Crown::SDMin(m_TopLeft.x, point.x);
    m_TopLeft.y = Crown::SDMin(m_TopLeft.y, point.y);
    m_BottomRight.x = Crown::SDMax(m_BottomRight.x, point.x);
    m_BottomRight.y = Crown::SDMax(m_BottomRight.y, point.y);
}

bool CoPolygon::TestPoint(const CoVec2& point) const
{
    double sum = 0;
    for (uint32 i = 0; i < m_Points.size(); i++)
    {
        CoVec2 vec0 = m_Points[i] - point;
        uint32 next = i + 1 >= m_Points.size() ? 0 : i + 1;
        CoVec2 vec1 = m_Points[next] - point;
        sum += Crown::NormalizeAngle(vec1.GetAngle() - vec0.GetAngle());
    }
    return fabs(sum) > M_PI;
}

bool CoPolygon::TestCircle(const CoVec2& center, real radius) const
{
    if( TestPoint(center) )		// 圆心在多边形内
        return true;

    // 圆心到每条边的距离
    for(uint32 i = 0; i < m_Points.size(); i++)
    {
        CoVec2 closestP;		// 圆心到边的最近点
        if(i==0)
            closestP = ClosestPointToSegment(center, m_Points[m_Points.size()-1], m_Points[i]);
        else
            closestP = ClosestPointToSegment(center, m_Points[i-1], m_Points[i]);

        real r2 = radius*radius;
        if(r2 < 1) r2 = 1;
        if( Vec2DistanceSquared(closestP, center) <= r2)		// 圆与边相交
            return true;
    }

    return false;
}

bool CoPolygon::TestPolygon(const CoPolygon& other) const
{
    // 分离轴测试
    const CoPolygon* polygonA = this;
    const CoPolygon* polygonB = &other;

    uint32 pointCountA = polygonA->m_Points.size();
    uint32 pointCountB = polygonB->m_Points.size();

    uint32 lineCountA = pointCountA;
    uint32 lineCountB = pointCountB;

    for(uint32 i=0; i<(lineCountA+lineCountB); i++)
    {
        // 计算分离轴
        CoVec2 p1;
        CoVec2 p2;
        if(i < lineCountA)
        {
            if(i == (lineCountA-1))
            {
                p1 = polygonA->m_Points[i];
                p2 = polygonA->m_Points[0];
            }
            else
            {
                p1 = polygonA->m_Points[i];
                p2 = polygonA->m_Points[i+1];
            }
        }
        else
        {
            if(i == (lineCountA+lineCountB-1))
            {
                p1 = polygonB->m_Points[i-lineCountA];
                p2 = polygonB->m_Points[0];
            }
            else
            {
                p1 = polygonB->m_Points[i-lineCountA];
                p2 = polygonB->m_Points[i-lineCountA+1];
            }
        }
        CoVec2 edge(p1-p2);
        CoVec2 axis;
        axis.SetAngle(1, edge.GetAngle() + real(M_PI_2));

        // 计算A和B分别在分离轴的投影
        real minA = 0;
        real maxA = 0;
        real minB = 0;
        real maxB = 0;

        ProjectOnAxis(axis, polygonA, &minA, &maxA);
        ProjectOnAxis(axis, polygonB, &minB, &maxB);
        
        // 计算投影是否相交
        real intervalDistance;
        if (minA < minB)
            intervalDistance = minB - maxA;
        else
            intervalDistance = minA - maxB;

        if(intervalDistance > 0)
            return false;                   // 此轴投影不相交
    }

    return true;                // 所有分离轴上的投影都相交
}

void CoPolygon::ProjectOnAxis(CoVec2 axis, const CoPolygon* polygon, real* min, real* max)
{
    real dotProduct = Vec2Dot(axis, polygon->m_Points[0]);
    *min = dotProduct;
    *max = dotProduct;
    for(uint32 i=0; i <polygon->m_Points.size(); i++)
    {
        dotProduct = Vec2Dot(polygon->m_Points[i], axis);
        if(dotProduct < *min)
            *min = dotProduct;
        else if(dotProduct > *max)
            *max = dotProduct;
    }
}

CoPolygon* CoPolygon::Rect(CoVec2 topLeft, CoVec2 bottomRight,  enum CoPolygonType type)
{
    CoPolygon* polygon(CnNew CoPolygon(type));
    polygon->append(topLeft);
    polygon->append(CoVec2(bottomRight.x, topLeft.y));
    polygon->append(bottomRight);
    polygon->append(CoVec2(topLeft.x, bottomRight.y));

    return polygon;
}

CoVec2 CoPolygon::Center() const
{
    return CoVec2((m_TopLeft.x + m_BottomRight.x)/2, (m_TopLeft.y + m_BottomRight.y)/2);
}
