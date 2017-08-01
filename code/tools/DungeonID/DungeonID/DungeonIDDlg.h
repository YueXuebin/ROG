
// DungeonIDDlg.h : header file
//

#pragma once


// CDungeonIDDlg dialog
class CDungeonIDDlg : public CDialog
{
// Construction
public:
	CDungeonIDDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DUNGEONID_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
    HICON m_hIcon;
    INT32 m_updateCount;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnEnUpdateEdit1();
    afx_msg void OnEnUpdateEdit2();
    afx_msg void OnEnUpdateEdit3();
    afx_msg void OnEnUpdateEdit4();

    void UpdateDungeonIDTxt();
    void UpdateUnionIDTxt();
};
