#ifndef triangle_node_h__
#define triangle_node_h__


struct TriangleNode
{
    TriangleNode(CoPolygon* polygon);

    int32 GetF_Value() {return m_g + m_h;}

    //是否在关闭类表里面
    void SetCloseState(bool close) {b_close = close;}

    bool GetCloseState() {return b_close;}

    int32 Get_G() {return m_g;}
    int32 Get_H() {return m_h;}

    real Get_X() {return m_x;}
    real Get_Y() {return m_y;}

    void Set_G(int32 value) {m_g = value;}
    void Set_H(int32 value) { m_h = value;}

    void InitNode(CoPolygon* polygon);                          //初始化结构
    bool ContaiansPoint(const CoVec2& curpoint);                //当前三角形是否包含该点
    void Reset();                                               //重置 g,h值
    void NeighborReset();                                       //重置临边三角角形
    int32 getPointIndexOffset(const CoVec2& p, int32 offset = 0);//
    const CoVec2& pointCCW(const CoVec2& curpoint);              //得到三角形的右开口
    const CoVec2& pointCW(const CoVec2& curpoint);                //得到三角形的左开口

    void SetPolygonOfTriangleNode(TriangleNode* node);           //设置polygon的三角形  triangelNode => CoPolygon的

    CoPolygon* GetTriangle();

    CoPolygon* m_triangles;           //当前的三角形
    int32  m_g; //a*算法里面g 实际走了多长距离
    int32  m_h; // 当前点到，目标点的距离

    real m_x;      //三角形x,y的中心点
    real m_y;

    bool b_close;   //是否在关闭列表里面

    TriangleNode* m_parentNode;
    std::vector<TriangleNode*> m_neighbors; //相邻的三角形
};


#endif // triangle_node_h__