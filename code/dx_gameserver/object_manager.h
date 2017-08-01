#ifndef object_manager_h__
#define object_manager_h__

class Object;

class ObjectManager
{
public:

    static ObjectManager* Instance();

    ObjectManager();
    ~ObjectManager();

    void Init(IDirect3DDevice9* device);
    void Uninit();

    void RegisterObject(Object* pObject);
    void UnRegisterObject(Object* pObject);

    void Update();
    void Render();

    void OnLostDevice();
    void OnResetDevice();

    Object* GetRoot() {return m_Root;}

    IDirect3DDevice9* GetDevice() {return m_pDevice;}

    void OnLButtonDown(int32 px, int32 py);
    void OnLButtonUp(int32 px, int32 py);
    void OnMouseMove(int32 px, int32 py);


    void SendMessage(const std::string& message, Object* pObject);

private:
    std::vector<Object*>    m_Objects;
    
    Object*     m_Root;
    bool        m_bInit;

    IDirect3DDevice9*   m_pDevice;

};



#endif // object_manager_h__
