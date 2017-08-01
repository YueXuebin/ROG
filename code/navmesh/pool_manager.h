#ifndef pool_manager_h__
#define pool_manager_h__
class Edge;
class NewFunnel;
class Portal;
struct TriangleNode;
/*
 *	对象池管理器
 */
class PoolManager
{
public:
    PoolManager();
    ~PoolManager();

    Edge*   CreateEdge(const CoVec2& point1, const CoVec2& point2);
    void    DestoryEdge(Edge* pEdge);

    NewFunnel* CreateFunnel();                                                  //创建虎口路径类
    void DestoryFunnel(NewFunnel* pfunnel);

    TriangleNode* CreateTriangleNode(CoPolygon* polygon);                       //创建三角形节点
    void DestoryTriangleNode(TriangleNode* node);

    Portal* CreatePorTal(const CoVec2& left, const CoVec2& right);                                                     //虎口点类
    void DestoryPorTal(Portal* portal);

private:
    Crown::CSDObjectPool<Edge>  m_EdgePool;

    Crown::CSDObjectPool<NewFunnel>  m_NewFunnelPool;

    Crown::CSDObjectPool<TriangleNode>  m_TriangelNodePool;

    Crown::CSDObjectPool<Portal> m_PorTalPool;                                  //虎口点类
};


extern PoolManager g_PoolManager;


#endif // pool_manager_h__
