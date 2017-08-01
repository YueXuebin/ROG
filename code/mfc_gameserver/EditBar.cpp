// EditBar.cpp : implementation file
//

#include "stdafx.h"
#include "mfc_gameserver.h"
#include "EditBar.h"


// CEditBar

IMPLEMENT_DYNAMIC(CEditBar, CToolBar)

#define COMBOBOX_WIDTH 100
#define COMBOBOX_HEIGHT 100

static UINT BASED_CODE EditButtons[] =
{
    ID_EDIT_BAR_CHANNEL,
    ID_EDIT_BAR_DUNGEON,
    ID_EDIT_BAR_SCENE,
    IDW_EDIT_BAR_TRIANGLE1,
    IDW_EDIT_BAR_TRIANGLE2,
    IDW_EDIT_BAR_TRIANGLE3,
    IDW_EDIT_BAR_SCALE_SUB,
    IDW_EDIT_BAR_SCALE_ADD,
    IDW_EDIT_BAR_CELL_INFO,
    IDW_EDIT_BAR_OBJECT,
    IDW_EDIT_BAR_CREATURE,
    IDW_EDIT_BAR_GRID,
};

CEditBar::CEditBar()
{

}

CEditBar::~CEditBar()
{
}

BOOL CEditBar::Init(CWnd* pParentWnd)
{
    DWORD barStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_SIZE_DYNAMIC;

    if (!Create(pParentWnd, barStyle, IDW_EDIT_BAR))
    {
        return FALSE;
    }

    SetWindowText("edit");

    LoadBitmap(IDB_EDIT_BAR);

    if(!SetButtons(EditButtons, sizeof(EditButtons)/sizeof(UINT)))
        return FALSE;

    DWORD editStyle = WS_VISIBLE|WS_CHILD|WS_BORDER|ES_NUMBER;
    CRect rect(-COMBOBOX_WIDTH, -COMBOBOX_HEIGHT, 0, 0);
    // 频道
    if (!m_ChannnelBox.Create(editStyle, rect, this, IDW_EDIT_BAR_CHANNEL))
        return FALSE;

    SetButtonInfo(0, IDW_EDIT_BAR_CHANNEL, TBBS_SEPARATOR, COMBOBOX_WIDTH);

    if (m_ChannnelBox.m_hWnd != NULL)
    {
        CRect rect;
        GetItemRect(0, rect);

        m_ChannnelBox.SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOCOPYBITS);
        m_ChannnelBox.ShowWindow(SW_SHOW);
        
    }
    // 副本
    if(!m_DungeonBox.Create(editStyle, rect, this, IDW_EDIT_BAR_DUNGEON))
        return FALSE;

    SetButtonInfo(1, IDW_EDIT_BAR_DUNGEON, TBBS_SEPARATOR, COMBOBOX_WIDTH);

    if (m_DungeonBox.m_hWnd != NULL)
    {
        CRect rect;
        GetItemRect(1, rect);

        m_DungeonBox.SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOCOPYBITS);
        m_DungeonBox.ShowWindow(SW_SHOW);
    }
    // 场景
    if(!m_SceneBox.Create(editStyle, rect, this, IDW_EDIT_BAR_SCENE))
        return FALSE;

    SetButtonInfo(2, IDW_EDIT_BAR_SCENE, TBBS_SEPARATOR, COMBOBOX_WIDTH);

    if (m_SceneBox.m_hWnd != NULL)
    {
        CRect rect;
        GetItemRect(2, rect);

        m_SceneBox.SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOCOPYBITS);
        m_SceneBox.ShowWindow(SW_SHOW);
    }



    return TRUE;
}


BEGIN_MESSAGE_MAP(CEditBar, CToolBar)
    ON_WM_CREATE()
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()



// CEditBar message handlers



int CEditBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CToolBar::OnCreate(lpCreateStruct) == -1)
        return -1;

     

    return 0;
}

void CEditBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CToolBar::OnShowWindow(bShow, nStatus);

}
