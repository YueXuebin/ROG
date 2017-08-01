/*
*	导航网格实现类
*/
#ifndef navmesh_imp_h__
#define navmesh_imp_h__

#include "navmesh.h"


struct TriangleNode;
class Edge;

bool SortOpenList(TriangleNode* node1, TriangleNode* node2);

class NavMeshImp : public NavMesh
{
public:
    NavMeshImp(const std::vector<CoPolygon*>& triangles, CoWorld* pCoWorld);
    virtual ~NavMeshImp();

    virtual bool FindPath(const CoVec2& start, const CoVec2& end, std::vector<CoVec2>& pPath);

    void Init(const std::vector<CoPolygon*>& triangles);

private:
    void GetNeighborsTriangles(TriangleNode* polygon);

    //判断两个三角形是否有公共边
    bool IsCommonEdge(CoPolygon* triangle1 , CoPolygon*triangle2);

    bool JudgePointInTriangle(CoPolygon* triangle , const CoVec2& pointP); //判断点是否在三角形里面

    //从点里面找出属的三角形
    TriangleNode* GetTriangeFromPoint(const CoVec2& start);

    //查找开放表里面是否有这个节点
    bool bFindOpenList(TriangleNode* node) ;

    void AddCloseList(TriangleNode* curNode);

    //得到当前三角和邻居三角形之间的距离
    real GetDistance(TriangleNode* triangle, TriangleNode* nexttriangle);

    //利用拐点算法，遍历三角形列表，求出最近路径
    void ChannelToPortals(CoVec2& startpoint, CoVec2& endpoint, std::vector<TriangleNode*> triangelist, std::vector<CoVec2>& pPath);

    //添加到开器表
    void AddOpenList(TriangleNode* node);

    //移除最小的三角形类表
    void SortRemoveTriangle();

    void UpdateOpenList(TriangleNode* curNode);

    bool IsEqualsTriangle(CoPolygon* triangle1 , CoPolygon*triangle2);

    bool IsFindCloseList(TriangleNode* triangelNode);

    bool FindTriangleList(const CoVec2& start, const CoVec2& end, std::vector<TriangleNode*>& trilist);         // A*查找最近的三角形带

    int32 GetNotCommonVerTexNum(TriangleNode* triangle1, TriangleNode* triangle2);                             //两个三角形是否有公共顶点的序列号

    const CoVec2& GetNotCommonVerTex(TriangleNode* triangle1, TriangleNode* triangle2);                       //查找两个三角形公共顶点

    Edge* GetCommonEdge(CoPolygon* triangle1 , CoPolygon*triangle2);                                    //得到二个三角形的公共边

    void Destory();
private:
    std::vector<TriangleNode*> m_AStartPolygonList; 

    std::vector<TriangleNode*> m_OpenList;   // open表
    std::vector<TriangleNode*> m_CloseList;  // close表

    CoWorld* pWorld;
};

#endif // navmesh_imp_h__
