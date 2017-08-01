#include "gameholder_pch.h"
#include "spawn_controller_factory.h"
#include "spawn.h"
#include "spawn_controller_speed.h"
#include "spawn_controller_trigger.h"
#include "spawn_controller_max_num.h"
#include "spawn_controller_wait_wave_dead.h"
#include "spawn_controller_wave_delay.h"


SpawnController* CreateSpawnController( Spawn* pSpawn, const SpawnWaveControlData& waveControlData )
{
    SpawnController* pController = NULL;
    switch(waveControlData.type)
    {
    case SPAWN_CONTROL_SPEED:
        pController = CnNew SpawnController_Speed(pSpawn);
        break;
    case SPAWN_CONTROL_WAIT_WAVE_DEAD:
        pController = CnNew SpawnController_WaitWaveDead(pSpawn);
        break;
    case SPAWN_CONTROL_MAX_NUM:
        pController = CnNew SpawnController_MaxNum(pSpawn);
        break;
    case SPAWN_CONTROL_WAVE_DELAY:
        pController = CnNew SpawnController_WaveDelay(pSpawn);
        break;
    case SPAWN_CONTROL_TRIGGER:
        pController = CnNew SpawnController_Trigger(pSpawn);
        break;
    default:
        CnAssert(false);
        break;
    }

    if(pController)
    {
        pController->Init(waveControlData);
        return pController;
    }
    else
    {
        CnAssert(false);
        return NULL;
    }
}
