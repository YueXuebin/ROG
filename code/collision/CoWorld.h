/*
*	物理世界
*/
#ifndef CoWorld_h__
#define CoWorld_h__

#include "CoPolygon.h"
#include <IterationUtils.h>
#include "CoVec2DInt.h"

// 剖分格属性
enum CellState
{
    CELL_NORMAL,
    CELL_BLOCK
};

class CoObjectOwner;
class CoObject;
class CoDynamicObject;
class CoBullet;
class CoBulletOwner;
class CoPointBullet;
class CoAreaSearcher;
class CoPolygon;
struct CoSegment;
struct CoVec2;

class CoWorld
{
public:
    typedef CnVector<CoDynamicObject*> DynamicObjectArray;

    // 剖分格定义
    typedef struct Cell
    {
        friend class CoWorld;

        void DynObjectLeave(CoDynamicObject* pDynObject);
        void DynObjectEnter(CoDynamicObject* pDynObject);

        void CollObjectLeave(CoDynamicObject* pCollObject);
        void CollObjectEnter(CoDynamicObject* pCollObject);

        enum CellState GetState() const {return m_State;}
        void SetState(enum CellState state) {m_State = state;}

        const DynamicObjectArray& GetDynObjects() {return m_DynObjects;}
        const DynamicObjectArray& GetCollObjects() { return m_CollObjects; }

        bool TestPointInCell(const CoVec2& p) const {return p.x>=m_LeftTop.x && p.x<=m_RightBottom.x && p.y>=m_LeftTop.y && p.y<=m_RightBottom.y;}
        bool TestLineInCell(const CoVec2& p1, const CoVec2& p2) const;

        uint32 GetTrianglesCount() const { return m_Triangles.size(); }

        Cell() : m_State(CELL_NORMAL) {}

        std::vector<CoPolygon*> GetNaviTriangleList() {return m_Navi_Triangles;}  //得到格子里面的导航网格三角形

        int32 m_IndexX, m_IndexY;		//  该剖分格的下标
        CoVec2	m_LeftTop, m_RightBottom;		// 剖分格的aabb
    private:		
        enum CellState m_State;		// 剖分格状态
        DynamicObjectArray  m_DynObjects;               // 剖分格中的动态对象
        DynamicObjectArray  m_CollObjects;              // 用来检测碰撞的对象
        std::vector<CoPolygon*> m_Triangles;            // 剖分格中的碰撞三角
        std::vector<CoPolygon*> m_Navi_Triangles;       // 剖分格中的导航三角
    } Cell;


    CoWorld(CoPolygon* worldBorder, uint32 cellLength);
    ~CoWorld();

    // 创建子弹对象
    CoPointBullet*	CreatePointBullet(CoBulletOwner* pOwner);			// 射线型子弹
    void			DestoryPointBullet(CoPointBullet* pBullet);

    // 物理对象
    void    AddObject(CoObject* pObject);
    void    RemoveObject(CoObject* pObject);

    // 剖分格属性
    uint32	GetCellLength() const { return m_CellLength; }
    real	GetMinX() const { return m_MinX; }
    real	GetMinY() const { return m_MinY; }
    real	GetMaxX() const { return m_MaxX; }
    real	GetMaxY() const { return m_MaxY; }
    int32	GetCellMinX() const { return m_CellMinX; }
    int32	GetCellMinY() const { return m_CellMinY; }
    int32	GetCellMaxX() const { return m_CellMaxX; }
    int32	GetCellMaxY() const { return m_CellMaxY; }

    Cell*	GetCellGrid(int32 x, int32 y);

    uint32  GetObjectCount() {return m_Objects.size();}

    // The cast should 2.9 -> 2, 3.1 -> 3, -0.1 -> -1, -1.2 -> -2
    int32	GetCellIndex(real axisValue) const { return static_cast<int32>(std::floor(axisValue / GetCellLength())); }

    /*
    * Get CellGrid by given coordination. Return nullptr if one of x, y is invalid.
    */
    Cell* 	GetCellGridByCoord(real x, real y);
    // 世界坐标转为剖分格坐标，如果出界会转换为边界上的格坐标
    Cell*	GetCellGridByCoordNoOut(real x, real y);
    CoVec2DInt GetCellPosByCoordNoOut(real x, real y);

    void AddTriangle(CoPolygon*);
    void AddNavigation(CoPolygon*);
    void MakeBlockCell();       // 根据导航网格，判断阻挡的Cell

    bool TestOutBorder(const CoVec2& center, real radius);
    bool TestCircleWithCell(const CoVec2& center, real radius);
    bool TestCircleWithTriangles(const CoVec2& center, real radius);
    bool TestCircleWithObjects(const CoVec2& center, real radius, CoDynamicObject** exclude, int32 excludeLen);
    bool TestPolygonWithTriangles(const CoPolygon* pPolygon);
    bool TestCellWithTriangle(Cell* pTestCell, CoPolygon* pTriangle);

    // 寻找某方向上最远的一个非碰撞点(射线起始点, 方向(角度), 最大距离, 碰撞半径)
    CoVec2  RayTraceReverseFind(const CoVec2& startPos, int32 orientAngle, real maxDistance, real radius);
    // 寻找某方向上最近的一个非碰撞点(射线起始点，方向(角度), 最大距离, 碰撞半径)
    CoVec2  RayTraceFind(const CoVec2& startPos, int32 orientAngle, real maxDistance, real radius);
    // 寻找某圆形区域类离中心最近的一个碰撞点(中心，最大距离，碰撞半径)
    CoVec2  CircleFind(const CoVec2& center, real maxDistance, real radius);

    int32 FindObjectsWithObject(CoDynamicObject* pObject, CoDynamicObject** outFind, int32 outLen);
    int32 FindObjectsWithCircle(const CoVec2& center, real radius, CoDynamicObject** outFind, int32 outLen);
    // center扇形的圆心，radius扇形的半径，angleCenter扇形方向，angleSeperation扇形角度
    int32 FindObjectsWithFan(const CoVec2& center, real radius, real angleCenter, real angleSeperation, CoDynamicObject** outFind, int32 outLen);
    // center矩形中心，length矩形的x方向半长，width矩形的y方向半长，angle矩形的角度
    int32 FindObjectsWithRect(const CoVec2& center, real length, real width, real angle, CoDynamicObject** outFind, int32 outLen);

    const std::vector<CoPolygon*> GetTriangles() {return m_Triangles;}
    const std::vector<CoPolygon*> GetNavigations() {return m_Navigations;}

private:
    void	PutSegmentToCells(CoSegment* segment);
    void	PutTriangleToCells(CoPolygon* triangle);
    void	PutNavigationTriangleToCells(CoPolygon* triangle);              //放入导航网格三角形
private:
    CoPolygon*  m_pBorder;        // 地图边界

    Cell*		m_BigGrid;		// 剖分格数组

    real		m_MinX, m_MinY;
    real		m_MaxX, m_MaxY;			// 地图大小
    int32		m_CellLength;			// 剖分大小
    int32		m_CellMinX, m_CellMinY;
    int32		m_CellMaxX, m_CellMaxY;		//  剖分格数

    DynamicObjectArray	m_DynObjects;		// 动态物体

    std::vector<CoPolygon*> m_Triangles;    // 碰撞三角
    std::vector<CoPolygon*> m_Navigations;  // 导航网格

    typedef std::vector<CoBullet*> BulletArray;
    BulletArray	m_Bullets;		// 子弹对象

    typedef std::vector<CoObject*>  ObjectArray;
    ObjectArray m_Objects;      // 物理对象

    CoAreaSearcher* m_pAreaSearcher; // 范围搜索代理
};

#endif // CoWorld_h__
