#ifndef newfunnel_h__
#define newfunnel_h__


class Portal;

// 拐角路径类
class NewFunnel
{
    friend class Crown::CSDObjectPool<NewFunnel>;

    NewFunnel(void);
    ~NewFunnel(void);
public:
    void PushPortal(CoVec2* p1 , CoVec2* p2 = NULL);

    void GetPathPoint(std::vector<CoVec2>& pathsVec);       // 拐角点法 找出最近的拐点

private:
    bool GetVequal(const CoVec2& pointA, const CoVec2& pointB);

    real GetTriarea2(const CoVec2& a , const CoVec2& b, const CoVec2& c);

private:
    std::vector<Portal*> m_portalsList;                     //存放虎口点
};


#endif // newfunnel_h__