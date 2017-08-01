#ifndef line_h__
#define line_h__



#include "object.h"




const int32 POINT_NUM = 200;                        // n个点
const int32 LINE_LENGTH = (POINT_NUM-1)*2;          // n-1条线段

class Line : public Object
{
public:
    Line();
    virtual ~Line();

    void SetColor(DWORD color);
    void SetPoints(POINT* pPoint, int32 len);
    void SetPoint(const POINT& p, int32 index);

protected:
    virtual void OnLostDevice();
    virtual void OnResetDevice();
    virtual void Update();
    virtual void Render();

private:
    bool Init();
    void Uninit();

private:
    LPDIRECT3DVERTEXBUFFER9 m_pVB; // Buffer to hold Vertices

    CUSTOMVERTEX m_Vertices[LINE_LENGTH];

    POINT   m_Points[POINT_NUM];
    bool    m_PointsDirty;

    bool    m_BufferDirty;

};


#endif // line_h__
