
// Direct3D9MFCDoc.h : CDirect3D9MFCDoc ��Ľӿ�
//


#pragma once


class CDirect3D9MFCDoc : public CDocument
{
protected: // �������л�����
	CDirect3D9MFCDoc();
	DECLARE_DYNCREATE(CDirect3D9MFCDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CDirect3D9MFCDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


