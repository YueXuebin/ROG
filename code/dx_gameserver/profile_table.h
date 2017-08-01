#ifndef profile_table_h__
#define profile_table_h__


#include "object.h"

class Line;
class TextObj;


class ProfileTable : public Object
{
public:
    ProfileTable();
    ~ProfileTable();

    virtual void OnLostDevice() {}
    virtual void OnResetDevice() {}

    virtual void Update();

    void SetPerformanceNode(PerformanceNode* pNode);

    float GetScale() {return m_scale;}
    void SetScale(float s) {m_scale = s;}

private:
    Line*                   m_LineTable;
    Line*                   m_LineProfile;
    TextObj*                m_FunctionName;

    int32                   m_height;
    float                   m_scale;

    PerformanceNode*        m_pPerformanceNode;
};


#endif // profile_table_h__
