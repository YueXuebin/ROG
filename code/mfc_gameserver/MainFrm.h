
// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include "EditBar.h"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
    void DockControlBarLeftOf(CToolBar* Bar,CToolBar* LeftOf);

    void SetStatusText(const CString& text);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CToolBar            m_wndToolBar;       // 标准工具栏
    CEditBar            m_wndEditBar;       // 编辑工具栏

	CStatusBar          m_wndStatusBar;     // 状态栏

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnChannel();
    afx_msg void OnDungeon();
    afx_msg void OnScene();
    afx_msg void OnUpdateDungeonUI(CCmdUI* pCmdUI);
    afx_msg void OnTriangle(UINT nID);
    afx_msg void OnUpdateTriangle(CCmdUI* pCmdUI);
    afx_msg void OnScaleSub();
    afx_msg void OnScaleAdd();
    afx_msg void OnCellInfo();
    afx_msg void OnObject();
    afx_msg void OnCreature();
    afx_msg void OnGrid();
    
	DECLARE_MESSAGE_MAP()

};


