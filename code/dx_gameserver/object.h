#ifndef object_h__
#define object_h__

#include "render_define.h"

class Object
{
public:
    Object();
    ~Object();

    virtual void OnLostDevice() = 0;
    virtual void OnResetDevice() = 0;

    virtual void Update();
    virtual void Render();

    bool IsVisible() {return m_Visible;}
    void SetVisible(bool v) {m_Visible = v;}

    void AddChild(Object* pChild);
    void RemoveChild(Object* pChild);
    bool HasChild(Object* pChild);


    void SetX(int32 x) {m_x = x;}
    int32 GetX() {return m_x;}
    void SetY(int32 y) {m_y = y;}
    int32 GetY() {return m_y;}
    void SetPosition(int32 x, int32 y) {m_x = x; m_y = y;}


    int32 GetGlobalX() {return m_GlobalX;}
    int32 GetGlobalY() {return m_GlobalY;}

    virtual void OnLButtonDown(int32 px, int32 py);
    virtual void OnLButtonUp(int32 px, int32 py);
    virtual void OnMouseMove(int32 px, int32 py);

    virtual void RecvMessage(const std::string& message, Object* pObject);


protected:
    IDirect3DDevice9*   m_pDevice;

    bool IsPosDirty() {return m_PosDirty;}

private:
    bool m_Visible;
    
    int32   m_x, m_y;
    int32   m_GlobalX, m_GlobalY;

    bool    m_PosDirty;

    Object* m_Parent;
    std::vector<Object*> m_Children;
};

#endif // object_h__
