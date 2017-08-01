
// Direct3D9MFCView.cpp : CDirect3D9MFCView 类的实现
//

#include "stdafx.h"
#include "Direct3D9MFC.h"

#include "Direct3D9MFCDoc.h"
#include "Direct3D9MFCView.h"

#include "profile_canvas.h"
#include "object_manager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDirect3D9MFCView

IMPLEMENT_DYNCREATE(CDirect3D9MFCView, CView)

BEGIN_MESSAGE_MAP(CDirect3D9MFCView, CView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CDirect3D9MFCView 构造/析构

CDirect3D9MFCView::CDirect3D9MFCView() : _device( 0 ), _teapot( 0 )
{
	ZeroMemory( &_d3dpp, sizeof( _d3dpp ) );

}

CDirect3D9MFCView::~CDirect3D9MFCView()
{
    delete m_ProfileCanvas;
    
    ObjectManager::Instance()->Uninit();

	if( _device )
		_device->Release( );

	if( _teapot )
		_teapot->Release( );
}

BOOL CDirect3D9MFCView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CDirect3D9MFCView 绘制

void CDirect3D9MFCView::OnDraw(CDC* /*pDC*/)
{
	CDirect3D9MFCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CDirect3D9MFCView 诊断

#ifdef _DEBUG
void CDirect3D9MFCView::AssertValid() const
{
	CView::AssertValid();
}

void CDirect3D9MFCView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDirect3D9MFCDoc* CDirect3D9MFCView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDirect3D9MFCDoc)));
	return (CDirect3D9MFCDoc*)m_pDocument;
}

HRESULT CDirect3D9MFCView::InitD3D( HWND hWnd, int width, int height, bool windowed, D3DDEVTYPE deviceType )
{
	HRESULT hr = 0;

	IDirect3D9 *d3d9 = 0;
	d3d9 = Direct3DCreate9( D3D_SDK_VERSION );
	if( !d3d9 )
	{
		::MessageBox( 0, _T( "InitD3D( ) - Failed!" ), 0, 0 );
		return E_FAIL;
	}

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps( D3DADAPTER_DEFAULT, deviceType, &caps );

	int vp = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	_d3dpp.BackBufferWidth				= width;
	_d3dpp.BackBufferHeight				= height;
	_d3dpp.BackBufferFormat				= D3DFMT_A8R8G8B8;
	_d3dpp.BackBufferCount				= 1;
	_d3dpp.MultiSampleType				= D3DMULTISAMPLE_NONE;
	_d3dpp.MultiSampleQuality			= 0;
	_d3dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	_d3dpp.Windowed						= windowed;
	_d3dpp.hDeviceWindow				= hWnd;
	_d3dpp.EnableAutoDepthStencil		= true;
	_d3dpp.AutoDepthStencilFormat		= D3DFMT_D24S8;
	_d3dpp.Flags						= 0;
	_d3dpp.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
	_d3dpp.PresentationInterval			= D3DPRESENT_INTERVAL_DEFAULT; //D3DPRESENT_INTERVAL_IMMEDIATE; // D3DPRESENT_INTERVAL_DEFAULT 垂直同步

	hr = d3d9->CreateDevice( D3DADAPTER_DEFAULT, deviceType, hWnd, vp,
							 &_d3dpp, &_device );
	if( FAILED( hr ) )
	{
		_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		hr = d3d9->CreateDevice( D3DADAPTER_DEFAULT, deviceType, hWnd, vp,
								 &_d3dpp, &_device );
		if( FAILED( hr ) )
		{
			d3d9->Release( );
			::MessageBox( 0, _T("CreateDevice( ) - Failed!"), 0, 0 );
			return hr;
		}
	}

	d3d9->Release( );

	return S_OK;
}

HRESULT CDirect3D9MFCView::Setup( int width, int height )
{
	if( _device )
	{
		D3DXMATRIX v;
		D3DXVECTOR3 pos	  ( 0.0f, 0.0f, -50.0f );
		D3DXVECTOR3 target( 0.0f, 0.0f,  0.0f );
		D3DXVECTOR3 up	  ( 0.0f, 1.0f,  0.0f );
		D3DXMatrixLookAtLH( &v, &pos, &target, &up );
		_device->SetTransform( D3DTS_VIEW, &v );

		if( !_teapot )
        {
			D3DXCreateTeapot( _device, &_teapot, 0 );
        }

        ObjectManager::Instance()->Init(_device);
        ObjectManager::Instance()->OnResetDevice();

		
		_device->SetRenderState( D3DRS_LIGHTING, false );

		D3DVIEWPORT9 vp = { 0, 0, width, height, 0.0f, 1.0f };
		_device->SetViewport( &vp );

		D3DXMATRIX p;
		D3DXMatrixPerspectiveFovLH( &p, D3DX_PI * 0.25f, (float)width / (float)height,
									1.0f, 1000.0f );
		_device->SetTransform( D3DTS_PROJECTION, &p );
	}

	return S_OK;
}

HRESULT CDirect3D9MFCView::Cleanup()
{
    ObjectManager::Instance()->OnLostDevice();
	return S_OK;
}

HRESULT CDirect3D9MFCView::Update( float timeDelta )
{
	if( _device )
	{
		static float angle = 0.0f;
		D3DXMATRIX yRotationMatrix;
		D3DXMatrixRotationY( &yRotationMatrix, angle );

		_device->SetTransform( D3DTS_WORLD, &yRotationMatrix );

		angle += timeDelta;

		if( angle > D3DX_PI * 2.0f )
			angle = 0.0f;
	}

	return S_OK;
}

HRESULT CDirect3D9MFCView::Render()
{
    ObjectManager::Instance()->Update();

	if( _device )
	{
		_device->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );

		_device->BeginScene( );

        // 渲染茶壶
        _device->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
        _teapot->DrawSubset( 0 );

        // 渲染
        _device->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
        ObjectManager::Instance()->Render();

		_device->EndScene( );

		_device->Present( 0, 0, 0, 0 );
	}

	return S_OK;
}

#endif //_DEBUG


void CDirect3D9MFCView::SetFPSText(const std::string& txt)
{
    m_ProfileCanvas->SetFPSText(txt);
}

// CDirect3D9MFCView 消息处理程序

void CDirect3D9MFCView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CRect rect;
	GetClientRect( &rect );

	HRESULT hr = InitD3D( GetSafeHwnd( ), rect.right, rect.bottom,
						  true, D3DDEVTYPE_HAL );
	if( FAILED( hr ) )
	{
		::MessageBox( 0, _T("InitD3D( ) - Failed!"), 0, 0 );
		::PostQuitMessage( 0 );
	}

	hr = Setup( rect.right, rect.bottom );
	if( FAILED( hr ) )
	{
		::MessageBox( 0, _T("Setup( ) - Failed!"), 0, 0 );
		::PostQuitMessage( 0 );
	}

    m_ProfileCanvas = new ProfileCanvas;
    ObjectManager::Instance()->GetRoot()->AddChild(m_ProfileCanvas);
}

void CDirect3D9MFCView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if( _device )
	{
		HRESULT hr = 0;

		_d3dpp.BackBufferWidth	= cx;
		_d3dpp.BackBufferHeight = cy;

		hr = Cleanup( );
		if( FAILED( hr ) )
		{
			::MessageBox( 0, _T("Cleanup( ) - Failed!"), 0, 0 );
			PostQuitMessage( 0 );
		}

		hr = _device->Reset( &_d3dpp );
		if( FAILED( hr ) )
		{
			::MessageBox( 0, _T("Reset( ) - Failed!"), 0, 0 );
			PostQuitMessage( 0 );
		}

		hr = Setup( cx, cy );
		if( FAILED( hr ) )
		{
			::MessageBox( 0, _T("Setup( ) - Failed!"), 0, 0 );
			PostQuitMessage( 0 );
		}
	}
}

BOOL CDirect3D9MFCView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}


void CDirect3D9MFCView::OnLButtonDown(UINT nFlags, CPoint point)
{
    ObjectManager::Instance()->OnLButtonDown(point.x, point.y);

    CView::OnLButtonDown(nFlags, point);
}

void CDirect3D9MFCView::OnLButtonUp(UINT nFlags, CPoint point)
{
    ObjectManager::Instance()->OnLButtonUp(point.x, point.y);

    CView::OnLButtonUp(nFlags, point);
}

void CDirect3D9MFCView::OnMouseMove(UINT nFlags, CPoint point)
{
    ObjectManager::Instance()->OnMouseMove(point.x, point.y);

    CView::OnMouseMove(nFlags, point);
}
