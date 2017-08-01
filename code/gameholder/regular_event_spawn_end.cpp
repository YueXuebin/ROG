#include "gameholder_pch.h"
#include "game_util.h"
#include "regular.h"
#include "regular_event_spawn_end.h"
#include "regular_define.h"
#include "scene.h"
#include "spawn.h"
#include "event_define.h"

RegularEvent_SpawnEnd::RegularEvent_SpawnEnd(Regular* pRegular) : 
RegularEvent(pRegular, REGULAR_EVENT_SPAWN_END)
{
    m_spawnID = 0;
}

RegularEvent_SpawnEnd::~RegularEvent_SpawnEnd()
{

}

void RegularEvent_SpawnEnd::ParseEventXml(TiXmlElement* EventElement)
{
    EventElement->QueryUnsignedAttribute("spawn_id", &m_spawnID);
    if (!m_pRegular->GetScene()->GetSpawn(m_spawnID))
        CnFatal("spawnID %d not exist\n", m_spawnID);
}

void RegularEvent_SpawnEnd::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, const std::string& arg3)
{
    switch(eventId)
    {
    case SCENE_EVENT_SPAWN_ALL_DEAD:
        {
            OnRecvSpawnAllDead(arg1);
        }
        break;
    default:
        break;
    }
}

void RegularEvent_SpawnEnd::OnRecvSpawnAllDead(uint32 spawnID)
{
    if(!m_pRegular || !m_pRegular->GetScene())
    {
        CnAssert(false);
        return;
    }

    if(GetActive())
        return;

    // 不是当前Spawn
    if(spawnID != m_spawnID)
        return;

    SetActive(true);
}

