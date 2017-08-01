
// mfc_gameserver.h : main header file for the mfc_gameserver application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// Cmfc_gameserverApp:
// See mfc_gameserver.cpp for the implementation of this class
//

class Cmfc_gameserverApp : public CWinApp
{
public:
	Cmfc_gameserverApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
    virtual int ExitInstance();
    virtual BOOL OnIdle(LONG lCount);

private:
    ULONG_PTR m_gdiplusToken;
};

extern Cmfc_gameserverApp theApp;
