#include "gameholder_pch.h"
#include "tower_defence_manager.h"
#include "dungeon_manager.h"

IMPLEMENT_SINGLETON(TowerDefenceManager)

TowerDefenceManager::TowerDefenceManager()
{
    Clear();
}

TowerDefenceManager::~TowerDefenceManager()
{

}

void TowerDefenceManager::Clear()
{
    m_IsOpen = false;
    m_currTime = 0;
}

bool TowerDefenceManager::CanEnter()
{
    return IsOpen();
}

void TowerDefenceManager::Update()
{
    m_currTime += g_ServerUpdateTime;
}

void TowerDefenceManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_TD_START_NTF:
        OnActivityStart(data);
        break;
    case MSG_TD_END_NTF:
        OnActivityEnd();
        break;
    default:
        break;
    }
}

void TowerDefenceManager::OnActivityStart(void* data)
{
    TD_START_NTF* recvMsg = (TD_START_NTF*)data;
    m_IsOpen = true;
    m_currTime = recvMsg->current_time;
}

void TowerDefenceManager::OnActivityEnd()
{
    m_IsOpen = false;
    m_currTime = 0;
    // 关闭所有修罗场
    //DungeonManager::Instance()->StopAllRegular(TD_MAP, false);
}
