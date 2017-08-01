
// DungeonIDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DungeonID.h"
#include "DungeonIDDlg.h"
#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDungeonIDDlg dialog

// 场景id分段(0保留)
// |--------频道id 11bit ---------|------- 副本id 13bit -----|------- 场景id 8bit -------|

#define CHANNEL_ID_MAX 0x7ff        // 最大频道id 2047
#define DUNGEON_ID_MAX 0x1fff       // 最大副本id 8191
#define SCENE_ID_MAX 0xff           // 最大场景id 255


#define UNION_ID(channel_id, dungeon_id, scene_id) ( ((channel_id & CHANNEL_ID_MAX)<<21) | ((dungeon_id & DUNGEON_ID_MAX)<<8) | (scene_id & SCENE_ID_MAX) )
#define CHANNEL_ID(union_id) ( union_id >> 21 )
#define DUNGEON_ID(union_id) ( (union_id >> 8) & DUNGEON_ID_MAX )
#define SCENE_ID(union_id) ( union_id & SCENE_ID_MAX )


CDungeonIDDlg::CDungeonIDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDungeonIDDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDungeonIDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDungeonIDDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_EN_UPDATE(IDC_EDIT1, &CDungeonIDDlg::OnEnUpdateEdit1)
    ON_EN_UPDATE(IDC_EDIT2, &CDungeonIDDlg::OnEnUpdateEdit2)
    ON_EN_UPDATE(IDC_EDIT3, &CDungeonIDDlg::OnEnUpdateEdit3)
    ON_EN_UPDATE(IDC_EDIT4, &CDungeonIDDlg::OnEnUpdateEdit4)
END_MESSAGE_MAP()


// CDungeonIDDlg message handlers

BOOL CDungeonIDDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    SetDlgItemText(IDC_EDIT1, "0");
    SetDlgItemText(IDC_EDIT2, "0");
    SetDlgItemText(IDC_EDIT3, "0");
    SetDlgItemText(IDC_EDIT4, "0000000000");

    m_updateCount = 0;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDungeonIDDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDungeonIDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDungeonIDDlg::OnEnUpdateEdit1()
{
    UpdateDungeonIDTxt();
}

void CDungeonIDDlg::OnEnUpdateEdit2()
{
    UpdateDungeonIDTxt();
}

void CDungeonIDDlg::OnEnUpdateEdit3()
{
    UpdateDungeonIDTxt();
}

void CDungeonIDDlg::OnEnUpdateEdit4()
{
    UpdateUnionIDTxt();
}


void CDungeonIDDlg::UpdateDungeonIDTxt()
{
    CString channelTxt;
    CString dungeonTxt;
    CString sceneTxt;
    CString unionTxt;

    if(m_updateCount == 0)
    {
        GetDlgItemText(IDC_EDIT1, channelTxt);
        GetDlgItemText(IDC_EDIT2, dungeonTxt);
        GetDlgItemText(IDC_EDIT3, sceneTxt);

        if(channelTxt == "")
        {
            channelTxt="0";
            ++m_updateCount;
            SetDlgItemText(IDC_EDIT1, "0");
        }

        if(dungeonTxt == "")
        {
            dungeonTxt="0";
            ++m_updateCount;
            SetDlgItemText(IDC_EDIT2, "0");
        }

        if(sceneTxt == "")
        {
            sceneTxt="0";
            ++m_updateCount;
            SetDlgItemText(IDC_EDIT3, "0");
        }

        unsigned int channelID = atoi(channelTxt);
        unsigned int dungeonID = atoi(dungeonTxt);
        unsigned int sceneID = atoi(sceneTxt);

        unsigned int unionID = UNION_ID(channelID, dungeonID, sceneID);
        unionTxt.Format("%010u", unionID);
        ++m_updateCount;
        SetDlgItemText(IDC_EDIT4, unionTxt);
    }
    else
    {
        --m_updateCount;
    }
}

void CDungeonIDDlg::UpdateUnionIDTxt()
{
    CString channelTxt;
    CString dungeonTxt;
    CString sceneTxt;
    CString unionTxt;

    unsigned int channelID = 0;
    unsigned int dungeonID = 0;
    unsigned int sceneID = 0;
    unsigned int unionID = 0;

    if(m_updateCount == 0)
    {
        GetDlgItemText(IDC_EDIT4, unionTxt);
        if(unionTxt == "") unionTxt="0";

        unionID = atoi(unionTxt);

        channelTxt.Format("%u", CHANNEL_ID(unionID));
        dungeonTxt.Format("%u", DUNGEON_ID(unionID));
        sceneTxt.Format("%u", SCENE_ID(unionID));

        m_updateCount += 3;
        SetDlgItemText(IDC_EDIT1, channelTxt);
        SetDlgItemText(IDC_EDIT2, dungeonTxt);
        SetDlgItemText(IDC_EDIT3, sceneTxt);
    }
    else
    {
        --m_updateCount;
    }
}
