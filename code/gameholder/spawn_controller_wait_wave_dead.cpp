#include "gameholder_pch.h"
#include "spawn_controller_wait_wave_dead.h"
#include "spawn.h"


SpawnController_WaitWaveDead::SpawnController_WaitWaveDead( Spawn* spawn ) :
SpawnController(spawn, SPAWN_CONTROL_WAIT_WAVE_DEAD)
{

}

SpawnController_WaitWaveDead::~SpawnController_WaitWaveDead()
{

}

bool SpawnController_WaitWaveDead::Init( const SpawnWaveControlData& waveControlData )
{
    return true;
}

void SpawnController_WaitWaveDead::FirstUpdate()
{
    m_IsWaitDead = true;
}

void SpawnController_WaitWaveDead::Update()
{
    int32 monsterNum = m_pSpawn->GetSpawnMonsterNumber();
    if(monsterNum > 0)
    {
        m_IsWaitDead = true;
    }
    else
        m_IsWaitDead = false;
}

int32 SpawnController_WaitWaveDead::FiltSpawnNum( int32 num )
{
    if(m_IsWaitDead)
    {
        int32 canNum = m_pSpawn->GetCurrentWaveRemainNum();
        num = Crown::SDMin(num, canNum);
    }
    else
    {
        int32 canNum = m_pSpawn->GetCurrentWaveMonsterNum();
        num = Crown::SDMin(num, canNum);
    }
    return num;
}

