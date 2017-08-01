
// Direct3D9MFC.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "Direct3D9MFC.h"
#include "MainFrm.h"
#include <MMSystem.h>

#include "Direct3D9MFCDoc.h"
#include "Direct3D9MFCView.h"

#include "gameserver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDirect3D9MFCApp

BEGIN_MESSAGE_MAP(CDirect3D9MFCApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CDirect3D9MFCApp::OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CDirect3D9MFCApp ����

CDirect3D9MFCApp::CDirect3D9MFCApp()
{

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

CDirect3D9MFCApp::~CDirect3D9MFCApp()
{


}

// Ψһ��һ�� CDirect3D9MFCApp ����

CDirect3D9MFCApp theApp;


// CDirect3D9MFCApp ��ʼ��

BOOL CDirect3D9MFCApp::InitInstance()
{
    bool ret = g_GameServer.Init();
    _ASSERT(ret);

	CWinApp::InitInstance();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)

	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDirect3D9MFCDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CDirect3D9MFCView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// ������׼������DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);


	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand ֮����
	return TRUE;
}

int CDirect3D9MFCApp::ExitInstance()
{
    g_GameServer.Uninit();

    return CWinApp::ExitInstance();
}


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CDirect3D9MFCApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CDirect3D9MFCApp ��Ϣ�������

BOOL CDirect3D9MFCApp::OnIdle(LONG lCount)
{
	CWinApp::OnIdle(lCount);

    CWnd *mainFrame = AfxGetMainWnd( );
    CDirect3D9MFCView *cView = (CDirect3D9MFCView *)mainFrame->GetWindow( GW_CHILD );

    g_GameServer.Tick(cView);

	static float lastTime = (float)timeGetTime( );

	float currentTime = (float)timeGetTime( );

	float deltaTime = (currentTime - lastTime) * 0.001f;

	lastTime = currentTime;

	cView->Update( deltaTime );
	cView->Render( );

	return TRUE;
}
