#ifndef rect_object_h__
#define rect_object_h__

#include "interact_object.h"

class RectObject : public InteractObject
{
public:
    RectObject();
    virtual ~RectObject();

    virtual void OnLostDevice();
    virtual void OnResetDevice();

    virtual void Render();
    virtual void Update();

    void SetWidth(uint32 w);
    void SetHeight(uint32 h);
    void SetColor(uint32 color);

private:
    bool Init();
    void Uninit();

protected:
    LPDIRECT3DVERTEXBUFFER9 m_pVB;
    CUSTOMVERTEX m_Vertices[6];

    RECT    m_Rect;

    bool    m_PointsDirty;
    bool    m_BufferDirty;
};



#endif // rect_object_h__
