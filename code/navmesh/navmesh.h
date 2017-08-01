/*
 *	导航网格接口
 */
#ifndef navmesh_h__
#define navmesh_h__

class CoPolygon;

class NavMesh
{
    friend NavMesh* CreateNavMesh();
    friend void DestroyNavMesh(NavMesh* pNavMesh);

protected:
    NavMesh(const std::vector<CoPolygon*>& triangles) {}
    virtual ~NavMesh() {}
public:
    /*
     *	start   起点
     *  end     终点
     *  pPath   存放结果的数组指针
     *  PathLen 存放结果的数组长度
     *  返回：是否存在可达路径
     */
    virtual bool FindPath(const CoVec2& start, const CoVec2& end, std::vector<CoVec2>& pPath) = 0;
};


NavMesh* CreateNavMesh(const std::vector<CoPolygon*>& triangles, CoWorld* pCoWorld);
void DestroyNavMesh(NavMesh* pNavMesh);



#endif // navmesh_h__
