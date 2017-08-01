#include "gameholder_pch.h"
#include "open_system_manager.h"

OpenSystemManager::OpenSystemManager(Player* player)
{

}

OpenSystemManager::~OpenSystemManager()
{

}

void OpenSystemManager::LoadOpenSystemInfo(const OPEN_SYSTEM_INFO& info)
{
    m_open_system_info = info;
}

void OpenSystemManager::FillOpenSystemInfo(OPEN_SYSTEM_INFO& info) const
{
    info = m_open_system_info;
}

void OpenSystemManager::SetOpenSystemInfo(const std::vector<uint32>& info)
{
    m_open_system_info.open_system_data = info;
}
