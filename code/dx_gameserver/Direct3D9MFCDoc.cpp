
// Direct3D9MFCDoc.cpp : CDirect3D9MFCDoc ���ʵ��
//

#include "stdafx.h"
#include "Direct3D9MFC.h"

#include "Direct3D9MFCDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDirect3D9MFCDoc

IMPLEMENT_DYNCREATE(CDirect3D9MFCDoc, CDocument)

BEGIN_MESSAGE_MAP(CDirect3D9MFCDoc, CDocument)
END_MESSAGE_MAP()


// CDirect3D9MFCDoc ����/����

CDirect3D9MFCDoc::CDirect3D9MFCDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CDirect3D9MFCDoc::~CDirect3D9MFCDoc()
{
}

BOOL CDirect3D9MFCDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CDirect3D9MFCDoc ���л�

void CDirect3D9MFCDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CDirect3D9MFCDoc ���

#ifdef _DEBUG
void CDirect3D9MFCDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDirect3D9MFCDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CDirect3D9MFCDoc ����
