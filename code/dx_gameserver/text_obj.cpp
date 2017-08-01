#include "stdafx.h"
#include "text_obj.h"


TextObj::TextObj()
{
    m_pFont = NULL;
    m_Color = 0xffffffff;
    OnResetDevice();
}

TextObj::~TextObj()
{
    OnLostDevice();
}

void TextObj::OnLostDevice()
{
    if( m_pFont )
    {
        m_pFont->OnLostDevice();
        m_pFont = NULL;
    }
}

void TextObj::OnResetDevice()
{
    if(!m_pFont)
    {
        D3DXCreateFont( m_pDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
           "Arial", &m_pFont );
    }
    else
    {
        m_pFont->OnResetDevice();
    }
}

void TextObj::Update()
{
    Object::Update();

    if(IsPosDirty())
    {
        m_Rect.left = GetGlobalX() + 0;
        m_Rect.right = GetGlobalX() + 200;
        m_Rect.top = GetGlobalY() + 0;
        m_Rect.bottom = GetGlobalY() + 50;
    }
    
}

void TextObj::Render()
{
    Object::Render();

    if(!m_pFont)
        return;

    m_pFont->DrawTextA(NULL, m_Text.c_str(), -1, &m_Rect, DT_NOCLIP, m_Color);

}

void TextObj::SetText(const std::string& text)
{
    m_Text = text;
}
