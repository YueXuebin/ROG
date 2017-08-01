#include "stdafx.h"
#include "object_manager.h"
#include "object.h"
#include "dummy.h"

ObjectManager g_Instance;

ObjectManager* ObjectManager::Instance()
{
    return &g_Instance;
}


ObjectManager::ObjectManager()
{
    m_bInit = false;
}

ObjectManager::~ObjectManager()
{

}

void ObjectManager::Init(IDirect3DDevice9 *device)
{
    if(m_bInit)
    {
        ASSERT(m_pDevice == device);
        return;
    }

    m_pDevice = device;

    m_Root = new Dummy;

    m_bInit = true;
}

void ObjectManager::Uninit()
{
    delete m_Root;
}

void ObjectManager::RegisterObject(Object* pObject)
{
#ifdef DEBUG
    for(std::vector<Object*>::iterator iter = m_Objects.begin(); iter != m_Objects.end(); iter++)
    {
        if((*iter) == pObject)
        {
            ASSERT(false);
        }
    }
#endif

    m_Objects.push_back(pObject);
}

void ObjectManager::UnRegisterObject(Object* pObject)
{
    for(std::vector<Object*>::iterator iter = m_Objects.begin(); iter != m_Objects.end(); iter++)
    {
        if((*iter) == pObject)
        {
            m_Objects.erase(iter);
            return;
        }
    }
}

void ObjectManager::OnLostDevice()
{
    for(std::vector<Object*>::iterator iter = m_Objects.begin(); iter != m_Objects.end(); iter++)
    {
        (*iter)->OnLostDevice();
    }
}

void ObjectManager::OnResetDevice()
{
    for(std::vector<Object*>::iterator iter = m_Objects.begin(); iter != m_Objects.end(); iter++)
    {
        (*iter)->OnResetDevice();
    }
}

void ObjectManager::Update()
{
    m_Root->Update();
}

void ObjectManager::Render()
{
    m_Root->Render();
}

void ObjectManager::OnLButtonDown(int32 px, int32 py)
{
    m_Root->OnLButtonDown(px, py);
}

void ObjectManager::OnLButtonUp(int32 px, int32 py)
{
    m_Root->OnLButtonUp(px, py);
}

void ObjectManager::OnMouseMove(int32 px, int32 py)
{
    m_Root->OnMouseMove(px, py);
}

void ObjectManager::SendMessage(const std::string& message, Object* pObject)
{
    for(std::vector<Object*>::iterator iter = m_Objects.begin(); iter != m_Objects.end(); iter++)
    {
        (*iter)->RecvMessage(message, pObject);
    }
}



