#include "gameholder_pch.h"
#include "guide_manager.h"
#include "player.h"

GuideManager::GuideManager(Player* player) :
m_player(player)
{

}

GuideManager::~GuideManager()
{

}

void GuideManager::LoadGuideInfo(const GUIDE_INFO& info)
{
    m_guide_info = info;
}

void GuideManager::FillGuideInfo(GUIDE_INFO& info) const
{
    info = m_guide_info;
}

void GuideManager::SetGuideInfo(const std::vector<uint32>& guide_info)
{
    m_guide_info.guide_data = guide_info;
}