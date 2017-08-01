
// Direct3D9MFCDoc.cpp : CDirect3D9MFCDoc 类的实现
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


// CDirect3D9MFCDoc 构造/析构

CDirect3D9MFCDoc::CDirect3D9MFCDoc()
{
	// TODO: 在此添加一次性构造代码

}

CDirect3D9MFCDoc::~CDirect3D9MFCDoc()
{
}

BOOL CDirect3D9MFCDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CDirect3D9MFCDoc 序列化

void CDirect3D9MFCDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CDirect3D9MFCDoc 诊断

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


// CDirect3D9MFCDoc 命令
