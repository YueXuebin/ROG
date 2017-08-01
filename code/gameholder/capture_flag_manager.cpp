#include "gameholder_pch.h"
#include "capture_flag_manager.h"
#include "dungeon_manager.h"
#include "dungeon.h"

IMPLEMENT_SINGLETON(CaptureFlagManager)

CaptureFlagManager::CaptureFlagManager()
{
    Clear();
}

CaptureFlagManager::~CaptureFlagManager()
{

}

void CaptureFlagManager::Clear()
{
    m_IsOpen = false;
    m_currTime = 0;
}

bool CaptureFlagManager::CanEnter()
{
    return IsOpen();
}

void CaptureFlagManager::Update(uint32 curr_tick)
{
    m_currTime += g_ServerUpdateTime;
}

void CaptureFlagManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_CAPTURE_FLAG_START_NTF:
        OnActivityStart(data);
        break;
    case MSG_CAPTURE_FLAG_END_NTF:
        OnActivityEnd();
        break;
    default:
        break;
    }
}

void CaptureFlagManager::OnActivityStart(void* data)
{
    CAPTURE_FLAG_START_NTF* recvMsg = (CAPTURE_FLAG_START_NTF*)data;
    m_IsOpen = true;
    m_currTime = recvMsg->current_time;
}

void CaptureFlagManager::OnActivityEnd()
{
    m_IsOpen = false;
    m_currTime = 0;
    // ¹Ø±ÕËùÓÐ¶áÆì
    //DungeonManager::Instance()->StopAllRegular(CAPTURE_FLAG_MAP, false);
}
