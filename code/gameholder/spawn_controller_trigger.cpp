#include "gameholder_pch.h"
#include "spawn_controller_trigger.h"
#include "config_reader.h"
#include "trigger.h"
#include "spawn.h"
#include "scene.h"

SpawnController_Trigger::SpawnController_Trigger( Spawn* spawn ) :
SpawnController(spawn, SPAWN_CONTROL_TRIGGER)
{
    m_TriggerId = 0;
    m_Active = false;
}

SpawnController_Trigger::~SpawnController_Trigger()
{

}

bool SpawnController_Trigger::Init( const SpawnWaveControlData& waveControlData )
{
    ConfigReader::Read(waveControlData.waveControlXML, "trigger_id", m_TriggerId);
    CnAssert(m_TriggerId > 0);

    return true;
}

void SpawnController_Trigger::FirstUpdate()
{

}

void SpawnController_Trigger::Update()
{
    Trigger* pTrigger = m_pSpawn->GetScene()->GetTriggerByID(m_TriggerId);
    if (pTrigger)
    {
        if (pTrigger->IsPlayerIn())
        {
            m_Active = true;
        }
    }
}

int32 SpawnController_Trigger::FiltSpawnNum( int32 num )
{
   if (m_Active)
   {
        return num;
   }
   return 0;
}
