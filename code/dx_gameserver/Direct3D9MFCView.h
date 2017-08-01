
// Direct3D9MFCView.h : CDirect3D9MFCView 类的接口
//


#pragma once

class ProfileCanvas;
class CDirect3D9MFCDoc;

class CDirect3D9MFCView : public CView
{
protected: // 仅从序列化创建
	CDirect3D9MFCView();
	DECLARE_DYNCREATE(CDirect3D9MFCView)

// 属性
public:
	CDirect3D9MFCDoc* GetDocument() const;

    void SetFPSText(const std::string& txt);

private:
	HRESULT InitD3D( HWND hWnd, int width, int height,
					 bool windowed, D3DDEVTYPE deviceType );
	HRESULT Setup( int width, int height );
	HRESULT Cleanup( );

// 操作
public:
	HRESULT	Update( float timeDelta );
	HRESULT Render( );

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CDirect3D9MFCView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
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

#ifndef _DEBUG  // Direct3D9MFCView.cpp 中的调试版本
inline CDirect3D9MFCDoc* CDirect3D9MFCView::GetDocument() const
   { return reinterpret_cast<CDirect3D9MFCDoc*>(m_pDocument); }
#endif

