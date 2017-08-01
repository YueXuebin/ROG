#include "gameholder_pch.h"
#include "spawn_controller_speed.h"
#include "config_reader.h"


SpawnController_Speed::SpawnController_Speed( Spawn* spawn ) :
SpawnController(spawn, SPAWN_CONTROL_SPEED)
{
    m_SpeedTime = 1000;
    m_UpdateTime = 0;
}

SpawnController_Speed::~SpawnController_Speed()
{

}

bool SpawnController_Speed::Init( const SpawnWaveControlData& waveControlData )
{
    ConfigReader::Read(waveControlData.waveControlXML, "speed_time", m_SpeedTime);

    return true;
}

void SpawnController_Speed::FirstUpdate()
{
    m_UpdateTime = m_SpeedTime;
}

void SpawnController_Speed::Update()
{
    m_UpdateTime += g_ServerUpdateTime;
}

int32 SpawnController_Speed::FiltSpawnNum( int32 num )
{
    if(m_SpeedTime == 0)
        return num;

    if(m_UpdateTime < m_SpeedTime)
        return 0;

    int32 canNum = m_UpdateTime / m_SpeedTime;
    m_UpdateTime = m_UpdateTime % m_SpeedTime;

    return Crown::SDMin(canNum, num);
}


