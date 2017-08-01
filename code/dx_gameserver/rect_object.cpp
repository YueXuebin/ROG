#include "stdafx.h"
#include "rect_object.h"


RectObject::RectObject()
{
    m_pVB = NULL;
    m_BufferDirty = true;
    Init();
}

RectObject::~RectObject()
{
    Uninit();
}

void RectObject::OnLostDevice()
{
    if( m_pVB != NULL )
    {
        m_pVB->Release();
        m_pVB = NULL;
    }
}

void RectObject::OnResetDevice()
{
    if(m_pVB == NULL)
    {
        if( FAILED( m_pDevice->CreateVertexBuffer( 6 * sizeof( CUSTOMVERTEX ),
            0, D3DFVF_CUSTOMVERTEX,
            D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
        {

        }
    }
}

bool RectObject::Init()
{
    m_Rect.left = 0;
    m_Rect.right = 100;
    m_Rect.top = 0;
    m_Rect.bottom = 50;

    for(int32 i=0; i<6; i++)
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

void RectObject::Uninit()
{
    OnLostDevice();
}

void RectObject::Update()
{
    InteractObject::Update();

    if(IsPosDirty() || m_PointsDirty)
    {
        int32 top = m_Rect.top + GetGlobalY();
        int32 bottom = m_Rect.bottom + GetGlobalY();
        int32 left = m_Rect.left + GetGlobalX();
        int32 right = m_Rect.right + GetGlobalX();

        m_Vertices[2].x = left;
        m_Vertices[2].y = top;
        m_Vertices[1].x = left;
        m_Vertices[1].y = bottom;
        m_Vertices[0].x = right;
        m_Vertices[0].y = top;

        m_Vertices[5].x = left;
        m_Vertices[5].y = bottom;
        m_Vertices[4].x = right;
        m_Vertices[4].y = bottom;
        m_Vertices[3].x = right;
        m_Vertices[3].y = top;

        m_BufferDirty = true;
        m_PointsDirty = false;
    }
}

void RectObject::Render()
{
    InteractObject::Render();

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
    m_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
}

void RectObject::SetWidth(uint32 w)
{
    m_Rect.left = 0;
    m_Rect.right = w;
    m_PointsDirty = true;
}

void RectObject::SetHeight(uint32 h)
{
    m_Rect.top = 0;
    m_Rect.bottom = h;
    m_PointsDirty = true;
}

void RectObject::SetColor(uint32 color)
{
    for(int32 i=0; i<6; i++)
    {
        m_Vertices[i].color = color;
    }
    m_BufferDirty = true;
}
