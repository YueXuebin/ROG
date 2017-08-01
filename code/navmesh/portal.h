#ifndef portal_h__
#define portal_h__


class Portal
{
    friend class Crown::CSDObjectPool<Portal>;
    Portal(const CoVec2& left, const CoVec2& right);
    ~Portal();

public:
    const CoVec2& GetLeft() {return m_left;}
    const CoVec2& GetRight() {return m_right;}

private:
    CoVec2 m_left;                  //上虎口点
    CoVec2 m_right;                 //右虎口点
};


#endif // portal_h__