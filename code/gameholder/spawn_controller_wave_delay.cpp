#include "gameholder_pch.h"
#include "spawn_controller_wave_delay.h"
#include "config_reader.h"
#include "spawn.h"


SpawnController_WaveDelay::SpawnController_WaveDelay( Spawn* spawn ) :
SpawnController(spawn, SPAWN_CONTROL_WAVE_DELAY)
{
    m_DelayTime = 0;
    m_DelayTimer = 0;

    m_InDelayState = false;
    m_DelayedWaveIndex = -1;
}

SpawnController_WaveDelay::~SpawnController_WaveDelay()
{

}

bool SpawnController_WaveDelay::Init( const SpawnWaveControlData& waveControlData )
{
    ConfigReader::Read(waveControlData.waveControlXML, "delay_time", m_DelayTime);
    m_DelayTime *= 1000;

    return true;
}

void SpawnController_WaveDelay::FirstUpdate()
{
    
}

void SpawnController_WaveDelay::Update()
{
    if(m_InDelayState)
    {
        m_DelayTimer -= g_ServerUpdateTime;
        if(m_DelayTimer <= 0)
            LeaveDelay();
    }
    else
    {
    }
}

int32 SpawnController_WaveDelay::FiltSpawnNum( int32 num )
{
    if(m_InDelayState)
        return 0;
    
    int32 remainNum = m_pSpawn->GetCurrentWaveRemainNum();
    if(num > 0 && remainNum ==0 && m_DelayedWaveIndex != m_pSpawn->GetCurrentWaveIndex())
    {   // 要开始出下一波怪了
        EnterDelay();
        num = 0;
    }
    
    if(m_DelayedWaveIndex != m_pSpawn->GetCurrentWaveIndex())
        num = Crown::SDMin(remainNum, num);
    
    return num;
}

void SpawnController_WaveDelay::EnterDelay()
{
    m_InDelayState = true;
    m_DelayTimer = m_DelayTime;
}

void SpawnController_WaveDelay::LeaveDelay()
{
    m_InDelayState = false;
    m_DelayTimer = 0;

    m_DelayedWaveIndex = m_pSpawn->GetCurrentWaveIndex();
    if(m_DelayedWaveIndex >= (int32)m_pSpawn->GetWaveNum())
        m_DelayedWaveIndex = -1;
}


