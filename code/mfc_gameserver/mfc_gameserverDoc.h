
// mfc_gameserverDoc.h : interface of the Cmfc_gameserverDoc class
//


#pragma once


class Cmfc_gameserverDoc : public CDocument
{
protected: // create from serialization only
	Cmfc_gameserverDoc();
	DECLARE_DYNCREATE(Cmfc_gameserverDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~Cmfc_gameserverDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


