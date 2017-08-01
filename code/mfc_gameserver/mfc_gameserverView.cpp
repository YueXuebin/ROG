
// mfc_gameserverView.cpp : implementation of the Cmfc_gameserverView class
//

#include "stdafx.h"
#include "mfc_gameserver.h"

#include "mfc_gameserverDoc.h"
#include "mfc_gameserverView.h"

#include "canvas.h"
#include "gameserver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cmfc_gameserverView

IMPLEMENT_DYNCREATE(Cmfc_gameserverView, CScrollView)

BEGIN_MESSAGE_MAP(Cmfc_gameserverView, CScrollView)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// Cmfc_gameserverView construction/destruction

Cmfc_gameserverView::Cmfc_gameserverView()
{
	m_Canvas = new Canvas(this);
    m_HeartBeat = 0;
}

Cmfc_gameserverView::~Cmfc_gameserverView()
{
    delete m_Canvas;
    m_Canvas = NULL;
}

BOOL Cmfc_gameserverView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

// Cmfc_gameserverView drawing

void Cmfc_gameserverView::OnDraw(CDC* /*pDC*/)
{
	Cmfc_gameserverDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// Cmfc_gameserverView diagnostics

#ifdef _DEBUG
void Cmfc_gameserverView::AssertValid() const
{
	CScrollView::AssertValid();
}

void Cmfc_gameserverView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

Cmfc_gameserverDoc* Cmfc_gameserverView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cmfc_gameserverDoc)));
	return (Cmfc_gameserverDoc*)m_pDocument;
}
#endif //_DEBUG


// Cmfc_gameserverView message handlers
void Cmfc_gameserverView::Render()
{
    CPaintDC dc(this); // device context for painting

    if(!g_GameServer.IsInit())
        return;

    if(m_HeartBeat == GameHolder::Instance()->GetHeartBeat())
        return;
    m_HeartBeat = GameHolder::Instance()->GetHeartBeat();

    CRect rect;
    GetClientRect(&rect);

    Gdiplus::Rect rc(rect.left, rect.top, rect.right, rect.bottom);

    if(true)
    {   // gdi++ 双缓冲
        Gdiplus::Graphics graphics(dc.m_hDC);

        Gdiplus::Bitmap bmp(rect.Width(), rect.Height());
        Gdiplus::Graphics* graph = Gdiplus::Graphics::FromImage(&bmp);
        if(!graph)
            return;

        Gdiplus::Status retStatus;

        // clear buffer
        Gdiplus::SolidBrush brush(Gdiplus::Color(210, 210, 210));
        retStatus = graph->FillRectangle(&brush, rc);
        _ASSERT(retStatus == Gdiplus::Ok);

        // render
        m_Canvas->render(graph);

        delete graph;

        // render to screen
        retStatus = graphics.DrawImage(&bmp, 0, 0);
        _ASSERT(retStatus == Gdiplus::Ok);
    }
    else
    {   // Gdi 双缓冲
        CDC MemDC; //首先定义一个显示设备对象
        CBitmap MemBitmap;//定义一个位图对象

        MemDC.CreateCompatibleDC(NULL); 

        //下面建立一个与屏幕显示兼容的位图，至于位图的大小嘛，可以用窗口的大小
        MemBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());  //将位图选入到内存显示设备中

        //只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上
        CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);
        //先用背景色将位图清除干净,这里我用的是白色作为背景,你也可以用自己应该用的颜色
        MemDC.FillSolidRect(0,0,rect.Width(),rect.Height(),RGB(255,255,0));

        Gdiplus::Graphics graph(MemDC);
        m_Canvas->render(&graph);

        //将内存中的图拷贝到屏幕上进行显示
        dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);
        //绘图完成后的清理  
        MemBitmap.DeleteObject();
        MemDC.DeleteDC();
    }

}

void Cmfc_gameserverView::UpdateSize()
{
    SIZE sizeTotal;
    sizeTotal.cx = m_Canvas->GetRenderWidth();
    sizeTotal.cy = m_Canvas->GetRenderHeight();
    SetScrollSizes(MM_TEXT, sizeTotal);
}

void Cmfc_gameserverView::OnInitialUpdate()
{
    CScrollView::OnInitialUpdate();

    UpdateSize();
}

// 同CScrollView实现，只修改了一行
BOOL Cmfc_gameserverView::OnScrollBy( CSize sizeScroll, BOOL bDoScroll )
{
    int xOrig, x;
    int yOrig, y;

    // don't scroll if there is no valid scroll range (ie. no scroll bar)
    CScrollBar* pBar;
    DWORD dwStyle = GetStyle();
    pBar = GetScrollBarCtrl(SB_VERT);
    if ((pBar != NULL && !pBar->IsWindowEnabled()) ||
        (pBar == NULL && !(dwStyle & WS_VSCROLL)))
    {
        // vertical scroll bar not enabled
        sizeScroll.cy = 0;
    }
    pBar = GetScrollBarCtrl(SB_HORZ);
    if ((pBar != NULL && !pBar->IsWindowEnabled()) ||
        (pBar == NULL && !(dwStyle & WS_HSCROLL)))
    {
        // horizontal scroll bar not enabled
        sizeScroll.cx = 0;
    }

    // adjust current x position
    xOrig = x = GetScrollPos(SB_HORZ);
    int xMax = GetScrollLimit(SB_HORZ);
    x += sizeScroll.cx;
    if (x < 0)
        x = 0;
    else if (x > xMax)
        x = xMax;

    // adjust current y position
    yOrig = y = GetScrollPos(SB_VERT);
    int yMax = GetScrollLimit(SB_VERT);
    y += sizeScroll.cy;
    if (y < 0)
        y = 0;
    else if (y > yMax)
        y = yMax;

    // did anything change?
    if (x == xOrig && y == yOrig)
        return FALSE;

    if (bDoScroll)
    {
        // do scroll and update scroll positions
        //ScrollWindow(-(x-xOrig), -(y-yOrig));
        if (x != xOrig)
            SetScrollPos(SB_HORZ, x);
        if (y != yOrig)
            SetScrollPos(SB_VERT, y);

        Invalidate();
    }
    return TRUE;
}

void Cmfc_gameserverView::OnPaint()
{
    //CPaintDC dc(this); // device context for painting

    Render();
}

BOOL Cmfc_gameserverView::OnEraseBkgnd(CDC* pDC)
{
    //return CScrollView::OnEraseBkgnd(pDC);
    return TRUE;
}

void Cmfc_gameserverView::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_Canvas->OnLButtonDown(point.x, point.y);

    CScrollView::OnLButtonDown(nFlags, point);
}

void Cmfc_gameserverView::OnMouseMove(UINT nFlags, CPoint point)
{
    m_Canvas->OnMouseMove(point.x, point.y);

    CScrollView::OnMouseMove(nFlags, point);
}