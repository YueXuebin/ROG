
// Direct3D9MFC.h : Direct3D9MFC Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CDirect3D9MFCApp:
// �йش����ʵ�֣������ Direct3D9MFC.cpp
//

class CDirect3D9MFCApp : public CWinApp
{
public:
	CDirect3D9MFCApp();
    virtual ~CDirect3D9MFCApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
    virtual int ExitInstance();
};

extern CDirect3D9MFCApp theApp;
