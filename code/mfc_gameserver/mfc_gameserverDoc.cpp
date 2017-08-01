
// mfc_gameserverDoc.cpp : implementation of the Cmfc_gameserverDoc class
//

#include "stdafx.h"
#include "mfc_gameserver.h"

#include "mfc_gameserverDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cmfc_gameserverDoc

IMPLEMENT_DYNCREATE(Cmfc_gameserverDoc, CDocument)

BEGIN_MESSAGE_MAP(Cmfc_gameserverDoc, CDocument)
END_MESSAGE_MAP()


// Cmfc_gameserverDoc construction/destruction

Cmfc_gameserverDoc::Cmfc_gameserverDoc()
{
	// TODO: add one-time construction code here

}

Cmfc_gameserverDoc::~Cmfc_gameserverDoc()
{
}

BOOL Cmfc_gameserverDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// Cmfc_gameserverDoc serialization

void Cmfc_gameserverDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// Cmfc_gameserverDoc diagnostics

#ifdef _DEBUG
void Cmfc_gameserverDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void Cmfc_gameserverDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// Cmfc_gameserverDoc commands
