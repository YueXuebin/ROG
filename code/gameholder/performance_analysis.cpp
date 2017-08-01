#include "gameholder_pch.h"
#include "performance_analysis.h"



PerformanceHelper::PerformanceHelper(const std::string& name) :
m_name(name)
{
    PerformanceManager::Instance()->Begin(m_name);
}

PerformanceHelper::~PerformanceHelper()
{
    PerformanceManager::Instance()->End(m_name);
}
