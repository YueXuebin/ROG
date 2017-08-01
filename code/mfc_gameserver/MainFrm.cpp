
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "mfc_gameserver.h"

#include "MainFrm.h"

#include "mfc_gameserverView.h"
#include "canvas.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_EN_CHANGE(IDW_EDIT_BAR_CHANNEL, OnChannel)
    ON_EN_CHANGE(IDW_EDIT_BAR_DUNGEON, OnDungeon)
    ON_EN_CHANGE(IDW_EDIT_BAR_SCENE, OnScene)
    ON_UPDATE_COMMAND_UI_RANGE(IDW_EDIT_BAR_CHANNEL, IDW_EDIT_BAR_SCENE, OnUpdateDungeonUI)
    ON_COMMAND_RANGE(IDW_EDIT_BAR_TRIANGLE1, IDW_EDIT_BAR_TRIANGLE3, OnTriangle)
    ON_UPDATE_COMMAND_UI_RANGE(IDW_EDIT_BAR_TRIANGLE1, IDW_EDIT_BAR_TRIANGLE3, OnUpdateTriangle)
    ON_COMMAND(IDW_EDIT_BAR_SCALE_SUB, OnScaleSub)
    ON_COMMAND(IDW_EDIT_BAR_SCALE_ADD, OnScaleAdd)
    ON_COMMAND(IDW_EDIT_BAR_CELL_INFO, OnCellInfo)
    ON_COMMAND(IDW_EDIT_BAR_OBJECT, OnObject)
    ON_COMMAND(IDW_EDIT_BAR_CREATURE, OnCreature)
    ON_COMMAND(IDW_EDIT_BAR_GRID, OnGrid)
    
    
    
    
END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    // TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // 标准工具栏
    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
        !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;      // fail to create
    }

    // 编辑工具栏
    if (!m_wndEditBar.Init(this))
    {
        TRACE0("Failed to create editbar\n");
        return -1;      // fail to create
    }

    // 状态栏
    if (!m_wndStatusBar.Create(this))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }
    m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

    UINT nID, nStyle;
    int cxWidth;

    m_wndStatusBar.GetPaneInfo( 0, nID, nStyle, cxWidth);
    m_wndStatusBar.SetPaneInfo( 0, nID, SBPS_STRETCH|SBPS_NORMAL, cxWidth);

    // 停靠设置
    m_wndEditBar.EnableDocking(CBRS_ALIGN_ANY);
    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);

    DockControlBar(&m_wndToolBar);
    DockControlBarLeftOf(&m_wndEditBar, &m_wndToolBar);

    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CFrameWnd::PreCreateWindow(cs) )
        return FALSE;
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG

void CMainFrame::DockControlBarLeftOf( CToolBar* Bar,CToolBar* LeftOf )
{
    CRect rect;
    DWORD dw;
    UINT n;

    // get MFC to adjust the dimensions of all docked ToolBars
    // so that GetWindowRect will be accurate
    RecalcLayout();
    LeftOf->GetWindowRect(&rect);
    rect.OffsetRect(1,0);
    dw=LeftOf->GetBarStyle();
    n = 0;
    n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
    n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
    n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
    n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;

    // When we take the default parameters on rect, DockControlBar will dock
    // each Toolbar on a seperate line.  By calculating a rectangle, we in effect
    // are simulating a Toolbar being dragged to that location and docked.
    DockControlBar(Bar,n,&rect);
}

void CMainFrame::OnChannel()
{
    Cmfc_gameserverView* view = (Cmfc_gameserverView*)GetActiveView();
    Canvas* pCanvas = view->GetCanvas();

    CString str;
    m_wndEditBar.m_ChannnelBox.GetWindowText(str);
    int channelID = atoi(str);
    if(channelID != pCanvas->GetChannelID())
        pCanvas->SetDungeon(channelID, pCanvas->GetDungeonID(), pCanvas->GetSceneID());
}

void CMainFrame::OnDungeon()
{
    Cmfc_gameserverView* view = (Cmfc_gameserverView*)GetActiveView();
    Canvas* pCanvas = view->GetCanvas();

    CString str;

    m_wndEditBar.m_DungeonBox.GetWindowText(str);
    int dungeonID = atoi(str);
    if(dungeonID != pCanvas->GetDungeonID())
        pCanvas->SetDungeon(pCanvas->GetChannelID(), dungeonID, pCanvas->GetSceneID());

}

void CMainFrame::OnScene()
{
    Cmfc_gameserverView* view = (Cmfc_gameserverView*)GetActiveView();
    Canvas* pCanvas = view->GetCanvas();

    CString str;
    m_wndEditBar.m_SceneBox.GetWindowText(str);
    int sceneID = atoi(str);

    if(sceneID != pCanvas->GetSceneID())
        pCanvas->SetDungeon(pCanvas->GetChannelID(), pCanvas->GetDungeonID(), sceneID);
}

void CMainFrame::OnTriangle(UINT nID)
{
    Cmfc_gameserverView* view = (Cmfc_gameserverView*)GetActiveView();
    Canvas* pCanvas = view->GetCanvas();

    if(nID == IDW_EDIT_BAR_TRIANGLE1)
    {
        pCanvas->ShowWall(!pCanvas->IsShowWall());
    }
    else if(nID == IDW_EDIT_BAR_TRIANGLE2)
    {
        pCanvas->ShowNav(!pCanvas->IsShowNav());
    }
    else if(nID == IDW_EDIT_BAR_TRIANGLE3)
    {
        pCanvas->ShowCell(!pCanvas->IsShowCell());
    }

}

void CMainFrame::OnUpdateTriangle(CCmdUI* pCmdUI)
{

}

void CMainFrame::OnUpdateDungeonUI( CCmdUI* pCmdUI )
{
    Cmfc_gameserverView* view = (Cmfc_gameserverView*)GetActiveView();
    Canvas* pCanvas = view->GetCanvas();

    CString str;
    if(pCmdUI->m_nID == IDW_EDIT_BAR_CHANNEL)
    {
        str.Format("%d", pCanvas->GetChannelID());
        m_wndEditBar.m_ChannnelBox.SetWindowText(str);
    }
    else if(pCmdUI->m_nID == IDW_EDIT_BAR_DUNGEON)
    {
        str.Format("%d", pCanvas->GetDungeonID());
        m_wndEditBar.m_DungeonBox.SetWindowText(str);
    }
    else if(pCmdUI->m_nID == IDW_EDIT_BAR_SCENE)
    {
        str.Format("%d", pCanvas->GetSceneID());
        m_wndEditBar.m_SceneBox.SetWindowText(str);
    }
}

void CMainFrame::OnScaleSub()
{
    Cmfc_gameserverView* view = (Cmfc_gameserverView*)GetActiveView();
    Canvas* pCanvas = view->GetCanvas();

    pCanvas->SetScale(pCanvas->GetScale() - 0.01f);
}

void CMainFrame::OnScaleAdd()
{
    Cmfc_gameserverView* view = (Cmfc_gameserverView*)GetActiveView();
    Canvas* pCanvas = view->GetCanvas();

    pCanvas->SetScale(pCanvas->GetScale() + 0.01f);
}

void CMainFrame::OnCellInfo()
{
    Cmfc_gameserverView* view = (Cmfc_gameserverView*)GetActiveView();
    Canvas* pCanvas = view->GetCanvas();

    pCanvas->SwitchCellInfo();
}

void CMainFrame::OnObject()
{
    Cmfc_gameserverView* view = (Cmfc_gameserverView*)GetActiveView();
    Canvas* pCanvas = view->GetCanvas();

    pCanvas->ShowObject(!pCanvas->IsShowObject());
}

void CMainFrame::OnCreature()
{
    Cmfc_gameserverView* view = (Cmfc_gameserverView*)GetActiveView();
    Canvas* pCanvas = view->GetCanvas();
    
    pCanvas->ShowCreature(!pCanvas->IsShowCreature());
}

void CMainFrame::OnGrid()
{
    Cmfc_gameserverView* view = (Cmfc_gameserverView*)GetActiveView();
    Canvas* pCanvas = view->GetCanvas();

    pCanvas->ShowGrid(!pCanvas->IsShowGrid());
}

void CMainFrame::SetStatusText( const CString& text )
{
    m_wndStatusBar.SetPaneText(0, text);
}



// CMainFrame message handlers
