
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
    {   // gdi++ ˫����
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
    {   // Gdi ˫����
        CDC MemDC; //���ȶ���һ����ʾ�豸����
        CBitmap MemBitmap;//����һ��λͼ����

        MemDC.CreateCompatibleDC(NULL); 

        //���潨��һ������Ļ��ʾ���ݵ�λͼ������λͼ�Ĵ�С������ô��ڵĴ�С
        MemBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());  //��λͼѡ�뵽�ڴ���ʾ�豸��

        //ֻ��ѡ����λͼ���ڴ���ʾ�豸���еط���ͼ������ָ����λͼ��
        CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);
        //���ñ���ɫ��λͼ����ɾ�,�������õ��ǰ�ɫ��Ϊ����,��Ҳ�������Լ�Ӧ���õ���ɫ
        MemDC.FillSolidRect(0,0,rect.Width(),rect.Height(),RGB(255,255,0));

        Gdiplus::Graphics graph(MemDC);
        m_Canvas->render(&graph);

        //���ڴ��е�ͼ��������Ļ�Ͻ�����ʾ
        dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);
        //��ͼ��ɺ������  
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

// ͬCScrollViewʵ�֣�ֻ�޸���һ��
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