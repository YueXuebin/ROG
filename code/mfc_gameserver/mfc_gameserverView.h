
// mfc_gameserverView.h : interface of the Cmfc_gameserverView class
//


#pragma once

class Canvas;
class Cmfc_gameserverDoc;

class Cmfc_gameserverView : public CScrollView
{
protected: // create from serialization only
	Cmfc_gameserverView();
	DECLARE_DYNCREATE(Cmfc_gameserverView)

// Attributes
public:
	Cmfc_gameserverDoc* GetDocument() const;
    Canvas* GetCanvas() {return m_Canvas;}
// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll);

    void    Render();

    void    UpdateSize();
protected:

    Canvas*  m_Canvas;
    uint32  m_HeartBeat;

// Implementation
public:
	virtual ~Cmfc_gameserverView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
    virtual void OnInitialUpdate();
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // debug version in mfc_gameserverView.cpp
inline Cmfc_gameserverDoc* Cmfc_gameserverView::GetDocument() const
   { return reinterpret_cast<Cmfc_gameserverDoc*>(m_pDocument); }
#endif

