/*
 *	‰÷»æÕº≤„
 */
#ifndef Layer_h__
#define Layer_h__

class Canvas;

class Layer
{
public:
    Layer(Canvas* canvas);
    virtual ~Layer();

    void render(Gdiplus::Graphics* graph);
    void UpdateSize();

    void RenderDirty() {m_RenderDirty = true;}

    void ClearBmp();

    virtual bool OnLButtonDown(int32 px, int32 py) { return false; }

    void ToRenderPos(POINT& p);
    void ToScenePos(POINT& p);

protected:
    virtual void OnRender(Gdiplus::Graphics* graph) = 0;

    void DrawText(Gdiplus::Graphics* graph, const WCHAR* wstr, int x, int y, Gdiplus::Color *color);

protected:
    Canvas*     m_Canvas;
    Scene*      m_pScene;
    CoWorld*    m_pCoWorld;

    static Gdiplus::Color   m_CellColor;
    static Gdiplus::Color   m_FontColor;
    static Gdiplus::Color   m_WallColor;
    static Gdiplus::Color   m_NavigateColor;
    static Gdiplus::Color   m_BoundingBoxColor;
    static Gdiplus::Color   m_GridColor;

    static Gdiplus::Color   m_DefaultColor;
    static Gdiplus::Color   m_PlayerColor;
    static Gdiplus::Color   m_MonsterColor;
    static Gdiplus::Color   m_BossColor;
    static Gdiplus::Color   m_NpcColor;
    static Gdiplus::Color   m_PetColor;
    static Gdiplus::Color   m_TriggerColor;
    static Gdiplus::Color   m_TriggerActiveColor;

    static Gdiplus::Color   m_RedColor;
    static Gdiplus::Color   m_GreenColor;
    static Gdiplus::Color   m_BlueColor;
    static Gdiplus::Color   m_GreyColor;

    bool        m_RenderDirty;
    Gdiplus::Bitmap* m_pLayerBmp;

};

#endif // Layer_h__
