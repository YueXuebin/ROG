#include "stdafx.h"
#include "gameserver.h"
#include "canvas.h"
#include "scene_layer.h"
#include "scene_info_layer.h"
#include "object_layer.h"
#include "grid_layer.h"
#include "mfc_gameserverView.h"
#include "MainFrm.h"

Canvas::Canvas(Cmfc_gameserverView* view)
{
    m_LastTickCount = 0;
    m_LastSecondTickCount = 0;
    m_FPS = 0;
    m_FPSCounter = 0;

    m_pView = view;

    m_Width = m_Height = 700;
    m_ImageScale = 1.0f;
    m_RenderScale = 0.09f;
    m_rx = m_ry = 0;

    m_ChannelId = 1;
    m_DungeonId = CITY_MAIN_CITY_MAP;         // 初始副本id
    m_SceneId = 1;
    m_pScene = NULL;

    m_SceneLayer = new SceneLayer(this);            // 场景图层
    m_SceneInfoLayer = new SceneInfoLayer(this);    // 场景信息层
    m_GridLayer = new GridLayer(this);              // 网格场景管理层
    m_ObjectLayer = new ObjectLayer(this);          // 对象层

    m_Layers.push_back(m_SceneLayer);
    m_Layers.push_back(m_SceneInfoLayer);
    m_Layers.push_back(m_GridLayer);
    m_Layers.push_back(m_ObjectLayer);
}

Canvas::~Canvas()
{
    for(uint32 i=0; i<m_Layers.size(); i++)
    {
        delete m_Layers[i];
    }
    m_Layers.clear();

    m_SceneLayer = NULL;

}

void Canvas::render( Gdiplus::Graphics* graph )
{
    Gdiplus::Status retStatus;

    CalcFPS();      // 计算帧率

    Gdiplus::Bitmap canvasBmp(GetWidth(), GetHeight());
    Gdiplus::Graphics* canvasGraph = Gdiplus::Graphics::FromImage(&canvasBmp);
    if(!canvasGraph)
        return;

    // 计算画布的位置
    CRect windowRect;
    m_pView->GetClientRect(&windowRect);
    CRect canvasRect(0, 0, GetWidth(), GetHeight());

    
    // clear buffer
    Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255));
    retStatus = canvasGraph->FillRectangle(&brush, Gdiplus::Rect(0, 0, GetWidth(), GetHeight()));
    _ASSERT(retStatus == Gdiplus::Ok);

    // 渲染各图层
    Scene* pScene = GetDrawScene();
    if(pScene)
    {
        if(m_pScene != pScene)
        {
            m_pScene = pScene;
            UpdateSize();
        }

        for(uint32 i=0; i<m_Layers.size(); i++)
        {
            m_Layers[i]->render(canvasGraph);
        }
    }

    delete canvasGraph;


    // 渲染画布到离屏buffer上
    m_rx = 0;
    if(windowRect.Width() > GetRenderWidth())
        m_rx = (windowRect.Width() - GetRenderWidth())/2;
    else
    {
        // 计算滚动条偏移
        int32 maxPos = m_pView->GetScrollLimit(SB_HORZ);
        int32 curPos = m_pView->GetScrollPos(SB_HORZ);

        m_rx = int32( (float)curPos/maxPos * -(GetRenderWidth() - windowRect.Width()) );
    }

    m_ry = 0;
    if(windowRect.Height() > GetRenderHeight())
        m_ry = (windowRect.Height() - GetRenderHeight())/2;
    else
    {
        // 计算滚动条偏移
        int32 maxPos = m_pView->GetScrollLimit(SB_VERT);
        int32 curPos = m_pView->GetScrollPos(SB_VERT);

        m_ry = int32( (float)curPos/maxPos * -(GetRenderHeight() - windowRect.Height()) );
    }

    Gdiplus::Rect rc(m_rx, m_ry, GetRenderWidth(), GetRenderHeight());
    retStatus = graph->DrawImage(&canvasBmp, rc);
    _ASSERT(retStatus == Gdiplus::Ok);

    // 渲染帧率等信息
    RenderFPS(graph);
    // 显示状态信息
    RenderInfo();
}

// 计算帧率
void Canvas::CalcFPS()
{
    // 计算帧率
    if(m_LastTickCount == 0)
    {
        m_LastTickCount = GetTickCount();
        m_LastSecondTickCount = m_LastTickCount;
        return;
    }

    DWORD curTickCount = GetTickCount();
    if((curTickCount - m_LastSecondTickCount) >= 1000)
    {
        m_FPS = m_FPSCounter;
        m_FPSCounter = 0;
        m_LastSecondTickCount = curTickCount;
    }
    else
    {
        m_FPSCounter++;
    }

    m_LastTickCount = curTickCount;
}
// 渲染帧率等信息
void Canvas::RenderFPS( Gdiplus::Graphics* graph )
{
    Gdiplus::SolidBrush  brush(Gdiplus::Color(255, 0, 0, 0));
    Gdiplus::FontFamily  fontFamily(L"Times New Roman");
    Gdiplus::Font        font(&fontFamily, 14, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::PointF      pointF(0.0f, 0.0f);

    WCHAR wstr[50];
    swprintf_s(wstr, 50, L"FPS:%d Frame:%d", m_FPS, g_FrameCount);

    graph->DrawString(wstr, -1, &font, pointF, &brush);
}

void Canvas::RenderInfo()
{
    CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd();

    if(!m_pScene)
    {
        pFrame->SetStatusText("");
        return;
    }

    CString info;
    // pawn info
    Scene* pScene = GetDrawScene();
    if(!pScene)
        return;

    info.Format("Player:%d  Monster:%d  Boss:%d  NPC:%d  PET:%d  Trigger:%d  Select:%d (%d, %d)",
        pScene->GetPlayers().size(),
        pScene->GetMonsters().size(),
        pScene->GetBosses().size(),
        pScene->GetNpcs().size(),
        pScene->GetPets().size(),
        pScene->GetTriggers().size(),
        m_ObjectLayer->GetSelectObjectID(),
        m_MouseX,
        m_MouseY
        );

    pFrame->SetStatusText(info);
}

Scene*  Canvas::GetDrawScene()
{
    return DungeonManager::Instance()->GetScene(UNION_ID(m_ChannelId, m_DungeonId, m_SceneId));
}

void Canvas::UpdateSize()
{
    CoWorld* pCoWorld = m_pScene->GetCoWorld();

    int32 x = (pCoWorld->GetCellMaxX() - pCoWorld->GetCellMinX() + 1) * pCoWorld->GetCellLength();
    int32 y = (pCoWorld->GetCellMaxY() - pCoWorld->GetCellMinY() + 1) * pCoWorld->GetCellLength();
    
    x *= GetScale();
    y *= GetScale();

    m_Width = x;
    m_Height = y;

    for(uint32 i=0; i<m_Layers.size(); i++)
    {
        m_Layers[i]->UpdateSize();
    }

    m_pView->UpdateSize();
}

void Canvas::SetDungeon( int32 channelID, int32 dungeonID, int32 sceneID )
{
    m_ChannelId = channelID;
    m_DungeonId = dungeonID;
    m_SceneId = sceneID;

    m_SceneLayer->RenderDirty();
}

void Canvas::ShowWall(bool show)
{
    m_SceneLayer->ShowWall(show);
}

bool Canvas::IsShowWall()
{
    return m_SceneLayer->IsShowWall();
}

void Canvas::ShowNav(bool show)
{
    m_SceneLayer->ShowNav(show);
}

bool Canvas::IsShowNav()
{
    return m_SceneLayer->IsShowNav();
}

void Canvas::ShowCell(bool show)
{
    m_SceneLayer->ShowCell(show);
}

bool Canvas::IsShowCell()
{
    return m_SceneLayer->IsShowCell();
}

void Canvas::SetScale( float s )
{
    if(s < 0.01f)
        s = 0.01f;

    m_RenderScale = s;

    UpdateSize();
}

void Canvas::SwitchCellInfo()
{
    m_SceneInfoLayer->SwitchCellInfo();
}

void Canvas::ShowObject( bool show )
{
    m_ObjectLayer->ShowObject(show);
}

bool Canvas::IsShowObject()
{
    return m_ObjectLayer->IsShowObject();
}

void Canvas::ShowCreature( bool show )
{
    m_ObjectLayer->ShowCreature(show);
}

bool Canvas::IsShowCreature()
{
    return m_ObjectLayer->IsShowCreature();
}

void Canvas::ShowGrid(bool show)
{
    m_GridLayer->ShowGrid(show);
}

bool Canvas::IsShowGrid()
{
    return m_GridLayer->IsShowGrid();
}

void Canvas::OnLButtonDown(int32 px, int32 py)
{
    int32 cx = px - m_rx;
    int32 cy = py - m_ry;
    if(cx > m_Width || cx < 0 || cy > m_Height|| cy < 0)
        return;     // 点击到画布外

    //cx /= m_RenderScale;
    //cy /= m_RenderScale;
    
    for(uint32 i=0; i<m_Layers.size(); i++)
    {
        if(m_Layers[i]->OnLButtonDown(cx, cy))
            break;
    }

}

void Canvas::OnMouseMove(int32 px, int32 py)
{
    m_MouseX = px - m_rx;
    m_MouseY = py - m_ry;
    
    if(m_Layers.size()>0)
    {
        POINT p;
        p.x = m_MouseX;
        p.y = m_MouseY;
        m_Layers[0]->ToScenePos(p);
        m_MouseX = p.x;
        m_MouseY = p.y;
    }
}
