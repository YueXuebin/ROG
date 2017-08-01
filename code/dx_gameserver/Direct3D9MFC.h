
// Direct3D9MFC.h : Direct3D9MFC 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CDirect3D9MFCApp:
// 有关此类的实现，请参阅 Direct3D9MFC.cpp
//

class CDirect3D9MFCApp : public CWinApp
{
public:
	CDirect3D9MFCApp();
    virtual ~CDirect3D9MFCApp();


// 重写
public:
	virtual BOOL InitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
    virtual int ExitInstance();
};

extern CDirect3D9MFCApp theApp;
