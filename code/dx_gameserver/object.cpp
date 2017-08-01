#include "stdafx.h"
#include "object.h"
#include "object_manager.h"

Object::Object():
m_Visible(true)
{
    ObjectManager::Instance()->RegisterObject(this);
    m_pDevice = ObjectManager::Instance()->GetDevice();

    m_x = m_y = 0;
    m_GlobalX = m_GlobalY = 0;

    m_Parent = NULL;
}

Object::~Object()
{
    while(m_Children.size() > 0)
    {
        delete m_Children[0];
    }
    m_Children.clear();

    if(m_Parent)
    {
        m_Parent->RemoveChild(this);
    }
    ObjectManager::Instance()->UnRegisterObject(this);
}

void Object::Update()
{
    int32 m_LastGlobalX = m_GlobalX;
    int32 m_LastGlobalY = m_GlobalY;
    if(m_Parent)
    {
        m_GlobalX = m_Parent->m_GlobalX + m_x;
        m_GlobalY = m_Parent->m_GlobalY + m_y;
    }
    
    m_PosDirty = (m_LastGlobalX != m_GlobalX || m_LastGlobalY != m_GlobalY);

    for(std::vector<Object*>::iterator iter = m_Children.begin(); iter != m_Children.end(); iter++)
    {
        Object* pObject = *iter;
        pObject->Update();
    }
}

void Object::Render()
{
    for(std::vector<Object*>::iterator iter = m_Children.begin(); iter != m_Children.end(); iter++)
    {
        Object* pObject = *iter;
        if(pObject->IsVisible())
            pObject->Render();
    }
}

void Object::AddChild(Object* pChild)
{
    if(HasChild(pChild))
        return;

    if(pChild->m_Parent)
    {
        pChild->m_Parent->RemoveChild(pChild);
    }

    ASSERT(pChild->m_Parent == NULL);
    pChild->m_Parent = this;
    m_Children.push_back(pChild);
}

void Object::RemoveChild(Object* pChild)
{
    for(std::vector<Object*>::iterator iter = m_Children.begin(); iter != m_Children.end(); iter++)
    {
        Object* pObject = *iter;
        if(pObject == pChild)
        {
            ASSERT(pChild->m_Parent == this);
            pChild->m_Parent = NULL;
            m_Children.erase(iter);
            return;
        }
    }
}

bool Object::HasChild(Object* pChild)
{
    for(std::vector<Object*>::iterator iter = m_Children.begin(); iter != m_Children.end(); iter++)
    {
        Object* pObject = *iter;
        if(pObject == pChild)
        {
            return true;
        }
    }

    return false;
}

void Object::OnLButtonDown(int32 px, int32 py)
{
    for(std::vector<Object*>::iterator iter = m_Children.begin(); iter != m_Children.end(); iter++)
    {
        Object* pObject = *iter;
        pObject->OnLButtonDown(px, py);
    }
}

void Object::OnLButtonUp(int32 px, int32 py)
{
    for(std::vector<Object*>::iterator iter = m_Children.begin(); iter != m_Children.end(); iter++)
    {
        Object* pObject = *iter;
        pObject->OnLButtonUp(px, py);
    }
}

void Object::OnMouseMove(int32 px, int32 py)
{
    for(std::vector<Object*>::iterator iter = m_Children.begin(); iter != m_Children.end(); iter++)
    {
        Object* pObject = *iter;
        pObject->OnMouseMove(px, py);
    }
}

void Object::RecvMessage(const std::string& message, Object* pObject)
{
    for(std::vector<Object*>::iterator iter = m_Children.begin(); iter != m_Children.end(); iter++)
    {
        Object* pObject = *iter;
        pObject->RecvMessage(message, pObject);
    }
}
