#include "gameholder_pch.h"
#include "performance_manager.h"

IMPLEMENT_SINGLETON(PerformanceManager)

PerformanceManager::PerformanceManager()
{
    m_current = NULL;
    m_time_index = 0;
    m_stop = false;
}

PerformanceManager::~PerformanceManager()
{
    while(m_NodeMap.size() > 0)
    {
        std::map<std::string, PerformanceNode*>::iterator iter = m_NodeMap.begin();
        CnDelete iter->second;
    }
}

void PerformanceManager::Init()
{
    m_root = CnNew PerformanceNode("root", NULL);
    m_frame = CnNew PerformanceNode("frame", NULL);
}

void PerformanceManager::Begin(const std::string& name)
{
    if(m_stop)
        return;

    if(!m_current)
        m_current = m_root;

    PerformanceNode* pThis = m_current->GetChildByName(name);
    if(!pThis)
        pThis = CnNew PerformanceNode(name, m_current);

    m_current = pThis;
    m_current->Start();

}

void PerformanceManager::End(const std::string& name)
{
    if(m_stop)
        return;

    CnAssert(m_current->GetName() == name);
    m_current->End();
    m_current = m_current->GetParent();
}

void PerformanceManager::Update()
{
    if(m_stop)
        return;

    m_frame->End();

    CnAssert(m_current == m_root);
    m_root->Update();
    m_frame->Update();

    m_time_index++;
    if(m_time_index >= RECORD_MAX)
        m_time_index = 0;

    m_frame->Start();
}

void PerformanceManager::RegisterNode(PerformanceNode* pNode)
{
    std::map<std::string, PerformanceNode*>::iterator iter = m_NodeMap.find(pNode->GetFullName());
    CnAssert(iter == m_NodeMap.end());
    m_NodeMap[pNode->GetFullName()] = pNode;
}

void PerformanceManager::UnRegisterNode(PerformanceNode* pNode)
{
    std::map<std::string, PerformanceNode*>::iterator iter = m_NodeMap.find(pNode->GetFullName());
    if(iter != m_NodeMap.end())
    {
        m_NodeMap.erase(iter);
    }
    else
    {
        CnAssert(false);
    }
}

void PerformanceManager::SetStop(bool stop)
{
    m_stop = stop;
}

