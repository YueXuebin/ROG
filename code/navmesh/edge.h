#ifndef edge_h__
#define edge_h__

class Edge
{
    friend class Crown::CSDObjectPool<Edge>;
    
    Edge(const CoVec2& point1, const CoVec2& point2);
    ~Edge(void);
public:

    bool HavePoint(const CoVec2& point);

    real GetDistanceSquared();  // 求出半径
private:
    CoVec2 m_point1;
    CoVec2 m_point2;
};

#endif // edge_h__
