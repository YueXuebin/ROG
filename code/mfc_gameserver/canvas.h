/*
 *	��������
 */
#ifndef canvas_h__
#define canvas_h__

class Cmfc_gameserverView;
class Layer;
class SceneLayer;
class SceneInfoLayer;
class ObjectLayer;
class GridLayer;
class Scene;

class Canvas
{
public:
    Canvas(Cmfc_gameserverView* view);
    ~Canvas();
    
    void render(Gdiplus::Graphics* graph);

    int32   GetWidth() {return m_Width;}
    int32   GetHeight() {return m_Height;}
    int32   GetRenderWidth() {return int32(m_Width * m_ImageScale);}
    int32   GetRenderHeight() {return int32(m_Height * m_ImageScale);}
    
    float   GetScale() {return m_RenderScale;}
    void    SetScale(float s);
    
    Scene*  GetDrawScene();

    uint32  GetChannelID() {return m_ChannelId;}
    uint32  GetDungeonID() {return m_DungeonId;}
    uint32  GetSceneID() {return m_SceneId;}
    void    SetDungeon(int32 channelID, int32 dungeonID, int32 sceneID);
    
    void    ShowWall(bool show);
    bool    IsShowWall();

    void    ShowNav(bool show);
    bool    IsShowNav();

    void    ShowCell(bool show);
    bool    IsShowCell();

    void    ShowObject(bool show);
    bool    IsShowObject();

    void    ShowCreature(bool show);
    bool    IsShowCreature();

    void    ShowGrid(bool show);
    bool    IsShowGrid();

    void    SwitchCellInfo();

    void    OnLButtonDown(int32 px, int32 py);
    void    OnMouseMove(int32 px, int32 py);

private:
    void CalcFPS();
    void RenderFPS(Gdiplus::Graphics* graph);

    void RenderInfo();

    void UpdateSize();


private:
    int32       m_Width;
    int32       m_Height;
    float       m_ImageScale;           // ����
    float       m_RenderScale;          //

    int32       m_rx;                   // canvas����ʼxy
    int32       m_ry;
    int32       m_MouseX;               // �����ָ��λ��
    int32       m_MouseY;

    uint32      m_ChannelId;
    uint32      m_DungeonId;
    uint32      m_SceneId;
    Scene*      m_pScene;

    // ͼ��
    SceneLayer*             m_SceneLayer;       // ������
    SceneInfoLayer*         m_SceneInfoLayer;   // ������Ϣ��
    ObjectLayer*            m_ObjectLayer;      // �����
    GridLayer*              m_GridLayer;        // �����
    std::vector<Layer*>     m_Layers;

    Cmfc_gameserverView*     m_pView;

    // fps
    DWORD   m_LastTickCount;
    DWORD   m_LastSecondTickCount;
    DWORD   m_FPS;
    DWORD   m_FPSCounter;
};


#endif // canvas_h__
