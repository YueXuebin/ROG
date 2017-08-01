
// Direct3D9MFCView.h : CDirect3D9MFCView ��Ľӿ�
//


#pragma once

class ProfileCanvas;
class CDirect3D9MFCDoc;

class CDirect3D9MFCView : public CView
{
protected: // �������л�����
	CDirect3D9MFCView();
	DECLARE_DYNCREATE(CDirect3D9MFCView)

// ����
public:
	CDirect3D9MFCDoc* GetDocument() const;

    void SetFPSText(const std::string& txt);

private:
	HRESULT InitD3D( HWND hWnd, int width, int height,
					 bool windowed, D3DDEVTYPE deviceType );
	HRESULT Setup( int width, int height );
	HRESULT Cleanup( );

// ����
public:
	HRESULT	Update( float timeDelta );
	HRESULT Render( );

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// ʵ��
public:
	virtual ~CDirect3D9MFCView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

private:
	IDirect3DDevice9		*_device;
	D3DPRESENT_PARAMETERS	 _d3dpp;

	ID3DXMesh				*_teapot;

    ProfileCanvas*          m_ProfileCanvas;


public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // Direct3D9MFCView.cpp �еĵ��԰汾
inline CDirect3D9MFCDoc* CDirect3D9MFCView::GetDocument() const
   { return reinterpret_cast<CDirect3D9MFCDoc*>(m_pDocument); }
#endif

