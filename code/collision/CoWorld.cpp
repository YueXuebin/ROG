#include "CollisionPCH.h"
#include "CoWorld.h"
#include "CoDynamicObject.h"
#include "CoPointBullet.h"
#include "CoPolygon.h"

const int32 DEFAULT_MAP_LENGTH = 10000;

/////////////////////////////////////////////////////////////////////
// 检测对象计数用
const int32 CHECK_MAX = 100;
CoDynamicObject* CheckHolder[CHECK_MAX];
int32 HolderNum = 0;

void ClearCheck()
{
    HolderNum = 0;
}

void AddCheck(CoDynamicObject* pObject)
{    
    CheckHolder[HolderNum] = pObject;    
    if(HolderNum < (CHECK_MAX-1))
        HolderNum++;
}

bool HasCheck(CoDynamicObject* pObject)
{
    for(int32 i=0; i<HolderNum; i++)
    {
        if(CheckHolder[i] == pObject)
            return true;
    }
    return false;
}
/////////////////////////////////////////////////////////////////////


CoWorld::CoWorld(CoPolygon* worldBorder, uint32 cellLength) :
m_CellLength(cellLength)
{
    // 创建世界边界
    m_pBorder = worldBorder;
    if(!m_pBorder)
    {
        m_pBorder = CoPolygon::Rect( CoVec2(real(-DEFAULT_MAP_LENGTH), real(-DEFAULT_MAP_LENGTH)), CoVec2(real(DEFAULT_MAP_LENGTH), real(DEFAULT_MAP_LENGTH)) );
    }
    m_pBorder->m_Type = COPOLYGON_TYPE_WORLDBORDER;

    m_MinX = floor(m_pBorder->TopLeft().x) - cellLength;
    m_MinY = floor(m_pBorder->TopLeft().y) - cellLength;
    m_MaxX = ceil((m_pBorder->BottomRight().x) + cellLength);
    m_MaxY = ceil((m_pBorder->BottomRight().y) + cellLength);

    // 创建范围搜索器
    //m_pAreaSearcher = CnNew CoAreaSearcher(this);

    m_CellMinX = GetCellIndex(m_MinX);
    m_CellMinY = GetCellIndex(m_MinY);

    m_CellMaxX = GetCellIndex(m_MaxX);
    m_CellMaxY = GetCellIndex(m_MaxY);

    // 创建剖分格
    m_BigGrid = CnNew Cell[(m_CellMaxX - m_CellMinX + 1) * (m_CellMaxY - m_CellMinY + 1)];
    for(int32 iy=m_CellMinY; iy<=m_CellMaxY; iy++)
    {
        for(int32 ix=m_CellMinX; ix<=m_CellMaxX; ix++)
        {
            m_BigGrid[(iy-m_CellMinY)*(m_CellMaxX - m_CellMinX + 1)+(ix-m_CellMinX)].m_IndexX = ix;
            m_BigGrid[(iy-m_CellMinY)*(m_CellMaxX - m_CellMinX + 1)+(ix-m_CellMinX)].m_IndexY = iy;

            m_BigGrid[(iy-m_CellMinY)*(m_CellMaxX - m_CellMinX + 1)+(ix-m_CellMinX)].m_LeftTop.x = real(ix) * (int32)m_CellLength;
            m_BigGrid[(iy-m_CellMinY)*(m_CellMaxX - m_CellMinX + 1)+(ix-m_CellMinX)].m_LeftTop.y = real(iy) * (int32)m_CellLength;
            m_BigGrid[(iy-m_CellMinY)*(m_CellMaxX - m_CellMinX + 1)+(ix-m_CellMinX)].m_RightBottom.x = real(ix+1) * (int32)m_CellLength;
            m_BigGrid[(iy-m_CellMinY)*(m_CellMaxX - m_CellMinX + 1)+(ix-m_CellMinX)].m_RightBottom.y = real(iy+1) * (int32)m_CellLength;
        }
    }

    //AddPolygon(m_pBorder);
}

CoWorld::~CoWorld()
{
    CnAssert(m_Bullets.size() == 0);

    // 删除范围搜索器
    //CnDelete m_pAreaSearcher;

    // 删除三角
    for(std::vector<CoPolygon*>::iterator iter = m_Triangles.begin(); iter != m_Triangles.end(); iter++)
    {
        CnDelete *iter;
    }
    for(std::vector<CoPolygon*>::iterator iter = m_Navigations.begin(); iter != m_Navigations.end(); iter++)
    {
        CnDelete *iter;
    }

    // 删除动态物体
    for(int i = 0; i < (int)m_Bullets.size(); i++)
    {
        CnDelete m_Bullets[i];
    }
    m_Bullets.clear();

    // 删除地图剖分格
    CnDelete[] m_BigGrid;

    // 删除地图边界
    CnDelete m_pBorder;
} 

CoWorld::Cell* CoWorld::GetCellGridByCoord(real x, real y)
{
    if ((x < m_MinX || m_MaxX < x) || (y < m_MinY || m_MaxY < y))
        return NULL;

    return GetCellGrid(GetCellIndex(x), GetCellIndex(y));
}

CoWorld::Cell*	CoWorld::GetCellGridByCoordNoOut(real x, real y)
{
    CoVec2DInt pos = GetCellPosByCoordNoOut(x, y);

    return GetCellGrid(pos.X, pos.Y);    
}

CoVec2DInt CoWorld::GetCellPosByCoordNoOut(real x, real y)
{
    int32 cx = GetCellIndex(x);
    int32 cy = GetCellIndex(y);

    if (cx < m_CellMinX) cx = m_CellMinX;
    if (cx > m_CellMaxX) cx = m_CellMaxX;
    if (cy < m_CellMinY) cy = m_CellMinY;
    if (cy > m_CellMaxY) cy = m_CellMaxY;

    return CoVec2DInt(cx, cy);
}

CoWorld::Cell* CoWorld::GetCellGrid(int32 x, int32 y)
{
    CnVerify(m_CellMinX <= x && x <= m_CellMaxX && m_CellMinY <= y && y <= m_CellMaxY);
    int32 ix = x - m_CellMinX, iy = y - m_CellMinY;
    int idx = ix + iy * (m_CellMaxX - m_CellMinX + 1);
    int maxIdx = (m_CellMaxX - m_CellMinX + 1) * (m_CellMaxY - m_CellMinY + 1);
    CnAssert(idx >= 0 && idx < maxIdx);
    return &m_BigGrid[idx];
}

CoPointBullet* CoWorld::CreatePointBullet(CoBulletOwner* pOwner)
{
    CoPointBullet* pBullet = CnNew CoPointBullet(this, pOwner);

    m_Bullets.push_back(pBullet);

    return pBullet;
}

void CoWorld::DestoryPointBullet(CoPointBullet* pBullet)
{
    for(int i = 0; i < (int)m_Bullets.size(); i++)
    {
        if(pBullet == m_Bullets[i])
        {
            m_Bullets.erase(m_Bullets.begin()+i);
            CnDelete pBullet;
            return;
        }
    }
    CnVerify(false);
}

void CoWorld::AddObject(CoObject* pObject)
{
#ifdef _DEBUG
    for(int i = 0; i < (int)m_Objects.size(); i++)
    {
        CnVerify(m_Objects[i] != pObject);
    }

#endif
    m_Objects.push_back(pObject);

    pObject->onEnterWorld(this);
}

void CoWorld::RemoveObject(CoObject* pObject)
{
    pObject->onLeaveWorld(this);

    for(int i = 0; i < (int)m_Objects.size(); i++)
    {
        if(m_Objects[i] == pObject)
        {
            m_Objects.erase(m_Objects.begin()+i);
            return;
        }
    }
    CnVerify(false);
}

void CoWorld::Cell::DynObjectLeave( CoDynamicObject* pDynObject )
{
    for(int i = 0; i < (int)m_DynObjects.size(); i++)
    {
        if(m_DynObjects[i] == pDynObject)
        {
            m_DynObjects.erase(m_DynObjects.begin()+i);
            return;
        }
    }
    CnVerify(false);	
}

void CoWorld::Cell::DynObjectEnter( CoDynamicObject* pDynObject )
{
#ifdef _DEBUG
    for(int i = 0; i < (int)m_DynObjects.size(); i++)
    {
        CnVerify(m_DynObjects[i] != pDynObject);
    }

#endif
    m_DynObjects.push_back(pDynObject);
}

void CoWorld::Cell::CollObjectLeave( CoDynamicObject* pCollObject )
{
    for(int i = 0; i < (int)m_CollObjects.size(); i++)
    {
        if(m_CollObjects[i] == pCollObject)
        {
            m_CollObjects.erase(m_CollObjects.begin()+i);
            return;
        }
    }
    CnVerify(false);	
}

void CoWorld::Cell::CollObjectEnter( CoDynamicObject* pCollObject )
{
#ifdef _DEBUG
    for(int i = 0; i < (int)m_CollObjects.size(); i++)
    {
        CnVerify(m_CollObjects[i] != pCollObject);
    }

#endif
    m_CollObjects.push_back(pCollObject);
}


bool CoWorld::Cell::TestLineInCell(const CoVec2& p1, const CoVec2& p2) const
{
    bool isIntersect = false;
    real A1 = p1.y - p2.y;
    real B1 = p2.x - p1.x;
    real C1 = p1.x * p2.y - p2.x * p1.y;		// 线段[p1,p2]的直线方程

    real LineIntersectY = (-C1 - A1 * m_LeftTop.x) / B1;
    if(LineIntersectY >= m_LeftTop.y && LineIntersectY <= m_RightBottom.y
        && (p1.y-LineIntersectY)*(p2.y-LineIntersectY)<=0)		// 交点必须在线段中
        isIntersect = true;

    LineIntersectY = (-C1 - A1 * m_RightBottom.x) / B1;
    if(LineIntersectY >= m_LeftTop.y && LineIntersectY <= m_RightBottom.y
        && (p1.y-LineIntersectY)*(p2.y-LineIntersectY)<=0)
        isIntersect = true;

    real LineIntersectX = (-C1 - B1 * m_LeftTop.y) / A1;
    if(LineIntersectX >= m_LeftTop.x && LineIntersectX <= m_RightBottom.x
        && (p1.x-LineIntersectX)*(p2.x-LineIntersectX)<=0)
        isIntersect = true;

    LineIntersectX = (-C1 - B1 * m_RightBottom.y) / A1;
    if(LineIntersectX >= m_LeftTop.x && LineIntersectX <= m_RightBottom.x
        && (p1.x-LineIntersectX)*(p2.x-LineIntersectX)<=0)
        isIntersect = true;

    return isIntersect;
}

void CoWorld::AddTriangle(CoPolygon* triangle)
{
    m_Triangles.push_back(triangle);

    PutTriangleToCells(triangle);
}

void CoWorld::AddNavigation(CoPolygon* navigate)
{
    m_Navigations.push_back(navigate);

    PutNavigationTriangleToCells(navigate);
}

void CoWorld::MakeBlockCell()
{
    // 遍历所有剖分格
    for(int32 iy=m_CellMinY; iy<=m_CellMaxY; iy++)
    {
        for(int32 ix=m_CellMinX; ix<=m_CellMaxX; ix++)
        {
            Cell* pTestCell = GetCellGrid(ix, iy);
            // 剖分格与导航网格碰撞检测
            bool isIn = false;
            for(std::vector<CoPolygon*>::iterator iter=m_Navigations.begin(); iter != m_Navigations.end(); iter++)
            {
                CoPolygon* pTriangle = *iter;
                if(TestCellWithTriangle(pTestCell, pTriangle))
                {
                    isIn = true;
                    break;
                }
            }

            pTestCell->SetState( isIn ? CELL_NORMAL : CELL_BLOCK);

        }		
    }

}

void CoWorld::PutTriangleToCells(CoPolygon* triangle)
{
    CoPolygon* pTriangle = triangle;
    CnVerify(pTriangle->Points().size() == 3);		// 必须是三角型
    if(pTriangle->Points().size() != 3)
        return;
    // 计算三角aabb所占据的剖分格
    Cell* pLTCell = GetCellGridByCoord(pTriangle->TopLeft().x, pTriangle->TopLeft().y);
    Cell* pRBCell = GetCellGridByCoord(pTriangle->BottomRight().x, pTriangle->BottomRight().y);

    if(!pLTCell || !pRBCell)
        return;

    CoVec2	tmpVec2;

    // 遍历该三角aabb所占据的剖分格
    for(int32 cy=pLTCell->m_IndexY; cy<=pRBCell->m_IndexY; cy++)
    {
        for(int32 cx=pLTCell->m_IndexX; cx<=pRBCell->m_IndexX; cx++)
        {
            Cell* pTestCell = GetCellGrid(cx, cy);
            CnVerify(pTestCell->m_IndexX == cx && pTestCell->m_IndexY == cy);
            if(TestCellWithTriangle(pTestCell, pTriangle))
                pTestCell->m_Triangles.push_back(pTriangle);
        }
    }

}
void CoWorld::PutNavigationTriangleToCells(CoPolygon* triangle)
 {
     CoPolygon* pTriangle = triangle;
     CnVerify(pTriangle->Points().size() == 3);     // 必须是三角型
     if(pTriangle->Points().size() != 3)
         return;
     // 计算三角aabb所占据的剖分格
     Cell* pLTCell = GetCellGridByCoord(pTriangle->TopLeft().x, pTriangle->TopLeft().y);
     Cell* pRBCell = GetCellGridByCoord(pTriangle->BottomRight().x, pTriangle->BottomRight().y);

     if(!pLTCell || !pRBCell)
         return;

     CoVec2	tmpVec2;

     // 遍历该三角aabb所占据的剖分格
     for(int32 cy=pLTCell->m_IndexY; cy<=pRBCell->m_IndexY; cy++)
     {
         for(int32 cx=pLTCell->m_IndexX; cx<=pRBCell->m_IndexX; cx++)
         {
             Cell* pTestCell = GetCellGrid(cx, cy);
             CnVerify(pTestCell->m_IndexX == cx && pTestCell->m_IndexY == cy);
             if(TestCellWithTriangle(pTestCell, pTriangle))
                 pTestCell->m_Navi_Triangles.push_back(pTriangle);
         }
     }
 }
bool CoWorld::TestCellWithTriangle(CoWorld::Cell* pTestCell, CoPolygon* pTriangle)
{
    CoVec2	tmpVec2;
    // 三角的aabb和Cell的aabb要重叠
    if( pTriangle->TopLeft().x >= pTestCell->m_RightBottom.x
        || pTriangle->BottomRight().x <= pTestCell->m_LeftTop.x
        || pTriangle->TopLeft().y >= pTestCell->m_RightBottom.y
        || pTriangle->BottomRight().y <= pTestCell->m_LeftTop.y)
        return false;

    // 三角型有顶点在cell中
    if( pTestCell->TestPointInCell(pTriangle->Points()[0])
        || pTestCell->TestPointInCell(pTriangle->Points()[1])
        || pTestCell->TestPointInCell(pTriangle->Points()[2]) )
    {
        return true;
    }

    // cell的顶点在三角型中
    tmpVec2.x = pTestCell->m_LeftTop.x;
    tmpVec2.y = pTestCell->m_LeftTop.y;
    if( pTriangle->TestPoint(tmpVec2) )
    {
        return true;
    }

    tmpVec2.x = pTestCell->m_LeftTop.x;
    tmpVec2.y = pTestCell->m_RightBottom.y;
    if( pTriangle->TestPoint(tmpVec2) )
    {
        return true;
    }

    tmpVec2.x = pTestCell->m_RightBottom.x;
    tmpVec2.y = pTestCell->m_RightBottom.y;
    if( pTriangle->TestPoint(tmpVec2) )
    {
        return true;
    }

    tmpVec2.x = pTestCell->m_RightBottom.x;
    tmpVec2.y = pTestCell->m_LeftTop.y;
    if( pTriangle->TestPoint(tmpVec2) )
    {
        return true;
    }

    // 三角形的边是否与AABB的边相交
    if( pTestCell->TestLineInCell(pTriangle->Points()[0], pTriangle->Points()[1])
        || pTestCell->TestLineInCell(pTriangle->Points()[1], pTriangle->Points()[2])
        || pTestCell->TestLineInCell(pTriangle->Points()[2], pTriangle->Points()[0]) )
    {
        return true;
    }

    return false;
}

bool CoWorld::TestOutBorder(const CoVec2& center, real radius)
{
    // 判断是否出界
    if( center.x < (m_pBorder->TopLeft().x - radius) 
        || center.x > (m_pBorder->BottomRight().x + radius)
        || center.y < (m_pBorder->TopLeft().y - radius)
        || center.y > (m_pBorder->BottomRight().y + radius) )
        return true;
    return false;
}

bool CoWorld::TestPolygonWithTriangles(const CoPolygon* pPolygon)
{
    real left = pPolygon->TopLeft().x;
    real right = pPolygon->BottomRight().x;
    real top = pPolygon->TopLeft().y;
    real bottom = pPolygon->BottomRight().y;

    // 判断多边形的每个顶点都没有出边界
    const std::vector<CoVec2>& points = pPolygon->Points();
    for(uint32 i=0; i<points.size(); i++)
    {
        if(TestOutBorder(points[i], 0))
            return true;
    }

    // 查找多边型的AABB对应的Cell
    Cell* pLTCell = GetCellGridByCoord(left, top);
    Cell* pRBCell = GetCellGridByCoord(right, bottom);
    if(!pLTCell || !pRBCell)
        return true;		// 超出世界范围

    // 遍历该圆aabb所占据的剖分格
    for(int32 cy=pLTCell->m_IndexY; cy<=pRBCell->m_IndexY; cy++)
    {
        for(int32 cx=pLTCell->m_IndexX; cx<=pRBCell->m_IndexX; cx++)
        {
            Cell* pTestCell = GetCellGrid(cx, cy);
            // 遍历三角
            for(size_t ti=0; ti<pTestCell->m_Triangles.size(); ti++)
            {
                CoPolygon* pTriangle = pTestCell->m_Triangles[ti];

                if(pTriangle->TestPolygon(*pPolygon))
                    return true;
            }
        }
    }
    return false;
}

bool CoWorld::TestCircleWithCell(const CoVec2& center, real radius)
{
    real left = center.x - radius;
    real right = center.x + radius;
    real top = center.y - radius;
    real bottom = center.y + radius;

    if(TestOutBorder(center, radius))
        return true;
    
    Cell* pLTCell = GetCellGridByCoord(left, top);
    Cell* pRBCell = GetCellGridByCoord(right, bottom);
    if(!pLTCell || !pRBCell)
        return true;		// 超出世界范围

    // 遍历该圆aabb所占据的剖分格
    for(int32 cy=pLTCell->m_IndexY; cy<=pRBCell->m_IndexY; cy++)
    {
        for(int32 cx=pLTCell->m_IndexX; cx<=pRBCell->m_IndexX; cx++)
        {
            Cell* pTestCell = GetCellGrid(cx, cy);
            if(pTestCell->GetState() == CELL_BLOCK)
                return true;
        }
    }
    return false;
}

bool CoWorld::TestCircleWithTriangles( const CoVec2& center, real radius )
{
    real left = center.x - radius;
    real right = center.x + radius;
    real top = center.y - radius;
    real bottom = center.y + radius;
    
    if(TestOutBorder(center, radius))
        return true;

    Cell* pLTCell = GetCellGridByCoord(left, top);
    Cell* pRBCell = GetCellGridByCoord(right, bottom);
    if(!pLTCell || !pRBCell)
        return true;		// 超出世界范围

    // 遍历该圆aabb所占据的剖分格
    for(int32 cy=pLTCell->m_IndexY; cy<=pRBCell->m_IndexY; cy++)
    {
        for(int32 cx=pLTCell->m_IndexX; cx<=pRBCell->m_IndexX; cx++)
        {
            Cell* pTestCell = GetCellGrid(cx, cy);
            // 遍历三角
            for(size_t ti=0; ti<pTestCell->m_Triangles.size(); ti++)
            {
                CoPolygon* pPolygon = pTestCell->m_Triangles[ti];
                if( pPolygon->TestCircle(center, radius) )
                    return true;
            }
        }
    }
    return false;
}

bool CoWorld::TestCircleWithObjects( const CoVec2& center, real radius,  CoDynamicObject** exclude, int32 excludeLen)
{
    real left = center.x - radius;
    real right = center.x + radius;
    real top = center.y - radius;
    real bottom = center.y + radius;

    Cell* pLTCell = GetCellGridByCoord(left, top);
    Cell* pRBCell = GetCellGridByCoord(right, bottom);

    if(!pLTCell || !pRBCell)
        return false;		// 超出世界范围

    // 遍历该圆aabb所占据的剖分格
    for(int32 cy=pLTCell->m_IndexY; cy<=pRBCell->m_IndexY; cy++)
    {
        for(int32 cx=pLTCell->m_IndexX; cx<=pRBCell->m_IndexX; cx++)
        {
            Cell* pTestCell = GetCellGrid(cx, cy);
            // 遍历对象
            for(size_t oi=0; oi<pTestCell->m_DynObjects.size(); oi++)
            {
                CoDynamicObject* pObject = pTestCell->m_DynObjects[oi];
                if( pObject->CollidedWith(center, radius) )
                {
                    // 是否被排除
                    if(exclude)
                    {
                        bool isExclude = false;
                        for(int32 ei=0; ei<excludeLen; ei++)
                        {
                            if(pObject == exclude[ei])
                            {
                                isExclude = true;
                                break;
                            }
                        }
                        if(isExclude)
                            continue;
                    }

                    return true;
                }
            }

        }
    }
    return false;
}

CoVec2 CoWorld::CircleFind(const CoVec2& center, real maxDistance, real radius)
{
    if(!TestCircleWithTriangles(center, radius))
    {
        return center;  // center本身就是非碰撞点
    }

    int32 step = 5;
    if(radius > 1)
    {
        int32 s = int32(maxDistance / radius);
        if(s < step)
            step = s;
    }
    
    real stepLen = maxDistance / step;

    // 步长间隔，八方向寻找
    bool find = false;
    CoVec2  DesPos;

    for(int32 si=0; si<step; si++)
    {
        real checkLen = stepLen * (si+1);
        real angle;
        for(int32 ai=0; ai<8; ai++)
        {
            angle = real(ai * M_PI / 4);
            DesPos.x = checkLen * cos(angle) + center.x;
            DesPos.y = checkLen * sin(angle) + center.y;

            if(!TestCircleWithTriangles(DesPos, radius))
            {
                find = true;
                break;
            }
        }
        if(find)
            break;
    }
    if(find)
        return DesPos;
    else
        return center;
}

CoVec2 CoWorld::RayTraceFind(const CoVec2& startPos, int32 orientAngle, real maxDistance, real radius)
{
    real orientRadian = ANGLE_TO_RADIAN(orientAngle);

    CoVec2 vertical;
    vertical.SetAngle(radius, ANGLE_TO_RADIAN(orientRadian+90));

    std::vector<CoVec2> testPoints;
    testPoints.push_back(CoVec2(0,0));
    testPoints.push_back(CoVec2(0,0));
    testPoints.push_back(CoVec2(0,0));
    testPoints.push_back(CoVec2(0,0));

    //   p1-----------------p2
    //   |                  |
    // start               des
    //   |                  |
    //   p4-----------------p3
    CoVec2& p1 = testPoints[0];
    CoVec2& p2 = testPoints[1];
    CoVec2& p3 = testPoints[2];
    CoVec2& p4 = testPoints[3];

    // 直接可达
    CoVec2 dir;
    dir.SetAngle(maxDistance, orientRadian);
    CoVec2 desPos = startPos + dir;

    p1 = startPos + vertical;
    p2 = desPos + vertical;
    p3 = desPos - vertical;
    p4 = startPos - vertical;

    CoPolygon testPolygon;
    testPolygon.set(testPoints);

    if(!TestPolygonWithTriangles(&testPolygon))
        return desPos;      // 目标点直接可达

    // 二分法查找最近的非碰撞点
    real bisectDis = maxDistance/2;
    real testDis = bisectDis;

    CoVec2 testDir, lastTestDir(0, 0);
    testDir.SetAngle(testDis, orientRadian);
    desPos = startPos + testDir;

    int32 maxSearch = 10;        // n次二分后，还是无法找到非碰撞点
    CoVec2 nearestDesPos = startPos;
    do
    {
        p1 = startPos + vertical;
        p2 = desPos + vertical;
        p3 = desPos - vertical;
        p4 = startPos - vertical;
        testPolygon.set(testPoints);
        bool isCollide = TestPolygonWithTriangles(&testPolygon);

        if(isCollide)
        {
            testDis -= bisectDis/2;
        }
        else
        {
            testDis += bisectDis/2;
            nearestDesPos = desPos;
        }
        bisectDis /= 2;

        lastTestDir = testDir;

        testDir.SetAngle(testDis, orientRadian);
        desPos = startPos + testDir;

        maxSearch--;
    }
    while( maxSearch >= 0 && Vec2DistanceSquared(testDir, lastTestDir) > 2500 );

    return nearestDesPos;



}

CoVec2 CoWorld::RayTraceReverseFind(const CoVec2& startPos, int32 orientAngle, real maxDistance, real radius)
{
    real orientRadian = ANGLE_TO_RADIAN(orientAngle);

    CoVec2 dir;
    dir.SetAngle(maxDistance, orientRadian);

    CoVec2 desPos = startPos + dir;
    if(!TestCircleWithTriangles(desPos, radius))
        return desPos;      // 目标点直接可达

    // 二分法查找最近的非碰撞点
    real bisectDis = maxDistance/2;
    real testDis = bisectDis;

    CoVec2 testDir, lastTestDir(0, 0);
    testDir.SetAngle(testDis, orientRadian);
    desPos = startPos + testDir;

    int32 maxSearch = 5;        // 5次二分后，还是无法找到非碰撞点
    CoVec2 nearestDesPos = startPos;
    do
    {
        bool isCollide = TestCircleWithTriangles(desPos, radius);

        if(isCollide)
            testDis -= bisectDis/2;
        else
        {
            testDis += bisectDis/2;
            nearestDesPos = desPos;
        }
        bisectDis /= 2;

        lastTestDir = testDir;

        testDir.SetAngle(testDis, orientRadian);
        desPos = startPos + testDir;

        maxSearch--;
    }
    while( maxSearch >= 0 && Vec2DistanceSquared(testDir, lastTestDir) > 2500 );

    return nearestDesPos;
}

int32 CoWorld::FindObjectsWithObject(CoDynamicObject* pObject, CoDynamicObject** outFind, int32 outLen)
{
    return FindObjectsWithCircle(pObject->GetPosition(), pObject->GetRadius(), outFind, outLen);
}

int32 CoWorld::FindObjectsWithCircle(const CoVec2& center, real radius,  CoDynamicObject** outFind, int32 outLen)
{
    real left = center.x - radius;
    real right = center.x + radius;
    real top = center.y - radius;
    real bottom = center.y + radius;

    left = Crown::SDMax(left, GetMinX());
    right = Crown::SDMin(right, GetMaxX());
    top = Crown::SDMax(top, GetMinY());
    bottom = Crown::SDMin(bottom, GetMaxY());

    Cell* pLTCell = GetCellGridByCoord(left, top);
    Cell* pRBCell = GetCellGridByCoord(right, bottom);

    if(!pLTCell || !pRBCell)
        return 0;		// 超出世界范围

    int32 findCount = 0;

    ClearCheck();

    // 遍历该圆aabb所占据的剖分格
    for(int32 cy=pLTCell->m_IndexY; cy<=pRBCell->m_IndexY; cy++)
    {
        for(int32 cx=pLTCell->m_IndexX; cx<=pRBCell->m_IndexX; cx++)
        {
            Cell* pTestCell = GetCellGrid(cx, cy);
            // 遍历对象
            for(size_t oi=0; oi<pTestCell->m_CollObjects.size(); oi++)
            {
                CoDynamicObject* pObject = pTestCell->m_CollObjects[oi];

                if(HasCheck(pObject)) continue;
                AddCheck(pObject);

                if( pObject->CollidedWith(center, radius) )
                {
                    if(findCount < outLen)
                    {
                        outFind[findCount] = pObject;
                        findCount++;
                    }
                    else
                        return findCount;       // outFind不够了
                }
            }

        }
    }
    return findCount;
}

int32 CoWorld::FindObjectsWithFan( const CoVec2& center, real radius, real angleCenter, real angleSeperation, CoDynamicObject** outFind, int32 outLen )
{
    real left = center.x - radius;
    real right = center.x + radius;
    real top = center.y - radius;
    real bottom = center.y + radius;

    left = Crown::SDMax(left, GetMinX());
    right = Crown::SDMin(right, GetMaxX());
    top = Crown::SDMax(top, GetMinY());
    bottom = Crown::SDMin(bottom, GetMaxY());

    Cell* pLTCell = GetCellGridByCoord(left, top);
    Cell* pRBCell = GetCellGridByCoord(right, bottom);

    if(!pLTCell || !pRBCell)
        return 0;		// 超出世界范围

    int32 findCount = 0;
    ClearCheck();

    // 遍历该圆aabb所占据的剖分格
    for(int32 cy=pLTCell->m_IndexY; cy<=pRBCell->m_IndexY; cy++)
    {
        for(int32 cx=pLTCell->m_IndexX; cx<=pRBCell->m_IndexX; cx++)
        {
            Cell* pTestCell = GetCellGrid(cx, cy);
            // 遍历对象
            for(size_t oi=0; oi<pTestCell->m_CollObjects.size(); oi++)
            {
                CoDynamicObject* pObject = pTestCell->m_CollObjects[oi];

                if(HasCheck(pObject)) continue;
                AddCheck(pObject);

                // 对象是否在圆内
                if( pObject->CollidedWith(center, radius) )
                {
                    // 对象是否在角度范围内
                    CoVec2 dir(pObject->GetPosX() - center.x, pObject->GetPosY() - center.y);
                    bool isIn = false;
                    if(dir.LengthSquared() <= (pObject->GetRadius() * pObject->GetRadius()) )
                        isIn = true;
                    else
                    {
                        real a = asin(pObject->GetRadius() / dir.Length());
                        real angleDiff = fabs(dir.GetAngle() - angleCenter);
                        if(angleDiff > M_PI)
                            angleDiff = (real)M_PI*2 - angleDiff;

                        if(angleDiff <= (angleSeperation + a))
                            isIn = true;
                    }

                    if(isIn)
                    {
                        if(findCount < outLen)
                        {
                            outFind[findCount] = pObject;
                            findCount++;
                        }
                        else
                            return findCount;       // outFind不够了
                    }
                }
            }
        }
    }
    return findCount;
}

int32 CoWorld::FindObjectsWithRect( const CoVec2& center, real length, real width, real angle, CoDynamicObject** outFind, int32 outLen )
{
    // 计算矩形的包围圆
    real radius = sqrt(length * length + width * width);

    real left = center.x - radius;
    real right = center.x + radius;
    real top = center.y - radius;
    real bottom = center.y + radius;

    left = Crown::SDMax(left, GetMinX());
    right = Crown::SDMin(right, GetMaxX());
    top = Crown::SDMax(top, GetMinY());
    bottom = Crown::SDMin(bottom, GetMaxY());

    Cell* pLTCell = GetCellGridByCoord(left, top);
    Cell* pRBCell = GetCellGridByCoord(right, bottom);

    if(!pLTCell || !pRBCell)
        return 0;		// 超出世界范围

    int32 findCount = 0;
    ClearCheck();

    // 遍历该圆aabb所占据的剖分格
    for(int32 cy=pLTCell->m_IndexY; cy<=pRBCell->m_IndexY; cy++)
    {
        for(int32 cx=pLTCell->m_IndexX; cx<=pRBCell->m_IndexX; cx++)
        {
            Cell* pTestCell = GetCellGrid(cx, cy);
            // 遍历对象
            for(size_t oi=0; oi<pTestCell->m_CollObjects.size(); oi++)
            {
                CoDynamicObject* pObject = pTestCell->m_CollObjects[oi];

                if(HasCheck(pObject)) continue;
                AddCheck(pObject);

                // 对象是否在圆内
                if( pObject->CollidedWith(center, radius) )
                {
                    // 对象是否在矩形范围内
                    CoVec2 dir(pObject->GetPosX() - center.x, pObject->GetPosY() - center.y);
                    real dirl = fabs(sin(dir.GetAngle() - angle) * dir.Length());
                    real dird = fabs(cos(dir.GetAngle() - angle) * dir.Length());

                    real A = tan(angle);
                    real B = -1.0f;
                    real C = -1.0f * tan(angle) * center.x + center.y;

                    real d = fabs(A * pObject->GetPosX() + B * pObject->GetPosY() + C) / sqrt(A*A+B*B);
                    real l = sqrt( (pObject->GetPosition() - center).LengthSquared() - d*d );
                    
                    if( d <= (width+dird) && l <= (length+dirl) )
                    {
                        if(findCount < outLen)
                        {
                            outFind[findCount] = pObject;
                            findCount++;
                        }
                        else
                            return findCount;       // outFind不够了
                    }
                }
            }
        }
    }
    return findCount;
}
