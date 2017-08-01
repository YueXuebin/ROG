#pragma once


// CEditBar

class CEditBar : public CToolBar
{
	DECLARE_DYNAMIC(CEditBar)

public:
	CEditBar();
	virtual ~CEditBar();

    BOOL Init(CWnd* pParentWnd);

    CEdit       m_ChannnelBox;
    CEdit       m_DungeonBox;
    CEdit       m_SceneBox;

protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};


