#include "stdafx.h"
#include "button.h"
#include "text_obj.h"
#include "object_manager.h"

Button::Button()
{
    m_ButtonState = BUTTON_NORMAL;

    m_Text = new TextObj;
    AddChild(m_Text);
}

Button::~Button()
{
    delete m_Text;
}

void Button::SetText(const std::string& text)
{
    m_Text->SetText(text);
}

void Button::OnLButtonDown(int32 px, int32 py)
{
    RectObject::OnLButtonDown(px, py);

    int32 top = m_Rect.top + GetGlobalY();
    int32 bottom = m_Rect.bottom + GetGlobalY();
    int32 left = m_Rect.left + GetGlobalX();
    int32 right = m_Rect.right + GetGlobalX();

    if(px >= left && px <= right && py >= top && py <= bottom)
    {
        SetState(BUTTON_DOWN);
        ObjectManager::Instance()->SendMessage("button down", this);
    }
}

void Button::OnLButtonUp(int32 px, int32 py)
{
    RectObject::OnLButtonUp(px, py);

    int32 top = m_Rect.top + GetGlobalY();
    int32 bottom = m_Rect.bottom + GetGlobalY();
    int32 left = m_Rect.left + GetGlobalX();
    int32 right = m_Rect.right + GetGlobalX();

    if(px >= left && px <= right && py >= top && py <= bottom)
    {
        SetState(BUTTON_HOVER);
    }
    else
    {
        SetState(BUTTON_NORMAL);
    }

}

void Button::OnMouseMove(int32 px, int32 py)
{
    RectObject::OnMouseMove(px, py);

    int32 top = m_Rect.top + GetGlobalY();
    int32 bottom = m_Rect.bottom + GetGlobalY();
    int32 left = m_Rect.left + GetGlobalX();
    int32 right = m_Rect.right + GetGlobalX();

    if(m_ButtonState != BUTTON_DOWN)
    {
        if(px >= left && px <= right && py >= top && py <= bottom)
        {
            SetState(BUTTON_HOVER);
        }
        else
        {
            SetState(BUTTON_NORMAL);
        }
    }

}

void Button::SetState(uint32 state)
{
    m_ButtonState = state;
    switch(m_ButtonState)
    {
    case BUTTON_NORMAL:
        SetColor(0xffaaaaaa);
        break;
    case BUTTON_HOVER:
        SetColor(0xffffff00);
        break;
    case BUTTON_DOWN:
        SetColor(0xff00ffff);
        break;

        
    }
}

