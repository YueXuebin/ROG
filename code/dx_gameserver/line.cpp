#include "stdafx.h"
#include "line.h"


Line::Line()
{
    m_pVB = NULL;
    m_BufferDirty = true;
    Init();
}

Line::~Line()
{
    Uninit();
}

bool Line::Init()
{
    for(int32 i=0; i<POINT_NUM; i++)
    {
        m_Points[i].x = m_Points[i].y = 0;
    }

    for(int32 i=0; i<LINE_LENGTH; i++)
    {
        m_Vertices[i].x = 0;
        m_Vertices[i].y = 0;
        m_Vertices[i].z = 0.5f;
        m_Vertices[i].rhw = 1.0f;
        m_Vertices[i].color = 0xffffffff;
    }

    m_PointsDirty = true;

    OnResetDevice();

    return true;
}

void Line::Uninit()
{
    OnLostDevice();
}

void Line::Update()
{
    Object::Update();
    
    if(IsPosDirty() || m_PointsDirty)
    {
        for(int32 i=0; i<(POINT_NUM-1); i++)
        {
            m_Vertices[i*2].x = m_Points[i].x + GetGlobalX();
            m_Vertices[i*2].y = m_Points[i].y + GetGlobalY();
            m_Vertices[i*2+1].x = m_Points[i+1].x + GetGlobalX();
            m_Vertices[i*2+1].y = m_Points[i+1].y + GetGlobalY();
        }
        m_BufferDirty = true;
        m_PointsDirty = false;
    }
}

void Line::Render()
{
    Object::Render();

    if(m_BufferDirty)
    {
        VOID* pVertices;
        if( SUCCEEDED( m_pVB->Lock( 0, sizeof( m_Vertices ), ( void** )&pVertices, 0 ) ) )
        {
            memcpy( pVertices, m_Vertices, sizeof( m_Vertices ) );
            m_pVB->Unlock();
        }
        m_BufferDirty = false;
    }

    m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof( CUSTOMVERTEX ) );
    m_pDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
    m_pDevice->DrawPrimitive( D3DPT_LINELIST, 0, LINE_LENGTH/2 );
}

void Line::SetColor(DWORD color)
{
    for(int32 i=0; i<LINE_LENGTH; i++)
    {
        m_Vertices[i].color = color;
    }
    m_BufferDirty = true;
}

void Line::SetPoints(POINT* pPoint, int32 len)
{
    if(len <= 1)
        return;

    if(len > POINT_NUM)
        len = POINT_NUM;

    for(int32 i=0; i<len; i++)
    {
        m_Points[i].x = pPoint[i].x;
        m_Points[i].y = pPoint[i].y;
    }

    for(int32 i=len; i<POINT_NUM; i++)
    {
        m_Points[i].x = pPoint[len-1].x;
        m_Points[i].y = pPoint[len-1].y;
    }

    m_PointsDirty = true;
}

void Line::SetPoint(const POINT& p, int32 index)
{
    if(index > POINT_NUM || index < 0)
        return;

    m_Points[index] = p;
    m_PointsDirty = true;
}

void Line::OnLostDevice()
{
    if( m_pVB != NULL )
    {
        m_pVB->Release();
        m_pVB = NULL;
    }
}

void Line::OnResetDevice()
{
    if(m_pVB == NULL)
    {
        if( FAILED( m_pDevice->CreateVertexBuffer( LINE_LENGTH * sizeof( CUSTOMVERTEX ),
            0, D3DFVF_CUSTOMVERTEX,
            D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
        {

        }
    }
}



