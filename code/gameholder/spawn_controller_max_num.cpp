#include "gameholder_pch.h"
#include "spawn_controller_max_num.h"
#include "config_reader.h"
#include "spawn.h"


SpawnController_MaxNum::SpawnController_MaxNum( Spawn* spawn ) :
SpawnController(spawn, SPAWN_CONTROL_MAX_NUM)
{
    m_MaxNum = 0;
}

SpawnController_MaxNum::~SpawnController_MaxNum()
{

}

bool SpawnController_MaxNum::Init( const SpawnWaveControlData& waveControlData )
{
    ConfigReader::Read(waveControlData.waveControlXML, "max_num", m_MaxNum);

    return true;
}

void SpawnController_MaxNum::FirstUpdate()
{

}

void SpawnController_MaxNum::Update()
{

}

int32 SpawnController_MaxNum::FiltSpawnNum( int32 num )
{
    int32 canNum = Crown::SDMax(m_MaxNum - m_pSpawn->GetSpawnMonsterNumber(), 0); 
    
    return Crown::SDMin(canNum, num);
}


