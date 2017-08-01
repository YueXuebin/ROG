#include "gameholder_pch.h"
#include "performance_node.h"
#include "performance_manager.h"

PerformanceNode::PerformanceNode(const std::string& name, PerformanceNode* parent) :
m_name(name),
m_parent(parent)
{
    start_time = end_time = delta_time = 0;
    m_reEntry = reEntry = 0;
    memset(delta_times, 0, sizeof(delta_times));

    if(m_parent)
    {
        m_parent->AddChild(this);
        m_fullname = m_parent->GetFullName()+"  "+m_name;
    }
    else
        m_fullname = m_name;

    PerformanceManager::Instance()->RegisterNode(this);
}

PerformanceNode::~PerformanceNode()
{
    PerformanceManager::Instance()->UnRegisterNode(this);
}

void PerformanceNode::AddChild(PerformanceNode* pChild)
{
    m_Children.push_back(pChild);
}

void PerformanceNode::Start()
{
    start_time = Crown::SDGetTickCount();
    reEntry++;
}

void PerformanceNode::End()
{
    end_time = Crown::SDGetTickCount();
    delta_time += end_time - start_time;
    start_time = end_time; 
}

void PerformanceNode::Update()
{
    delta_times[PerformanceManager::Instance()->GetTimeIndex()] = delta_time;

    delta_time = 0;
    m_reEntry = reEntry;
    reEntry = 0;

    for(std::vector<PerformanceNode*>::iterator iter = m_Children.begin(); iter != m_Children.end(); iter++)
    {
        PerformanceNode* pNode = *iter;
        pNode->Update();
    }
}

PerformanceNode* PerformanceNode::GetChildByName(const std::string& name)
{
    for(std::vector<PerformanceNode*>::iterator iter = m_Children.begin(); iter != m_Children.end(); iter++)
    {
        PerformanceNode* pNode = *iter;
        if(pNode->GetName() == name)
            return pNode;
    }
    return NULL;
}

PerformanceNode* PerformanceNode::GetChildByIndex(uint32 index)
{
    if(index >= m_Children.size())
        return NULL;

    return m_Children[index];
}