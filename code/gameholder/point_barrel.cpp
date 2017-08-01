#include "gameholder_pch.h"
#include "point_barrel.h"
#include "spawn.h"
#include "spawn_wave.h"
#include "spawn_controller_speed.h"
#include "spawn_controller_max_num.h"
#include "spawn_controller_factory.h"
#include "scene.h"
#include "event_define.h"



CnImplementRTTI(PointBarrel, PointBase);

PointBarrel::PointBarrel(RegularBuilder* pRogRegular, uint32 CheckPointID) :
PointBase(pRogRegular, CheckPointID, POINT_TYPE_BARREL),
m_pSpawn(NULL)
{
    // 创建一个怪物出生点
    SpawnData spawnData;
    spawnData.x = (int32)m_position.x;
    spawnData.y = (int32)m_position.y;
    spawnData.angle = m_angle;
    spawnData.spawnId = m_pRogRegular->GenerateValidTriggerID();
    spawnData.loop = false;

    m_pSpawn = m_pRogRegular->CreateSpawn(spawnData);
    m_pSpawn->SetSpawning(false);
    m_pRogRegular->GetScene()->AddSpawn(m_pSpawn);

    // 添加出怪控制器
    SpawnWaveControlData controlData;

    // 出怪速度控制
    controlData.type = SPAWN_CONTROL_SPEED;
    m_pSpeedController = (SpawnController_Speed*)CreateSpawnController(m_pSpawn, controlData);
    m_pSpeedController->SetSpeedTime(1);
    m_pSpawn->AddWaveControl(m_pSpeedController);

    // 出怪最大数控制
    controlData.type = SPAWN_CONTROL_MAX_NUM;
    m_pMaxNumController = (SpawnController_MaxNum*)CreateSpawnController(m_pSpawn, controlData);
    m_pMaxNumController->SetMaxNum(10);
    m_pSpawn->AddWaveControl(m_pMaxNumController);

    // 添加出怪波
    MonsterData md;
    md.creatureType = 1;
    md.creatureId = 3999;       // 木桶为3999
    md.cantMove = true;
    md.cantBuff = true;
    md.cantHeal = true;
    md.cantTaunt = true;
    md.cantPush = true;
    md.num = 1;

    SpawnWaveData wavaData;
    wavaData.MonsterDatas.push_back(md);

    m_pSpawn->AddWave(wavaData);

}

PointBarrel::~PointBarrel()
{

}

bool PointBarrel::Build()
{
    if(m_EventId == 0)
    {
        m_pSpawn->SetSpawning(true);
    }
    return true;
}

void PointBarrel::SetRadius(int32 max_radius, int32 min_radius)
{
    m_pSpawn->SetSpawnRadius(max_radius, min_radius);
}

void PointBarrel::SetLoop(bool loop)
{
    m_pSpawn->SetLoop(loop);
}

void PointBarrel::SetNumber(int32 num)
{
    m_pSpawn->GetWave(0)->waveMonsters[0].num = num;
    m_pMaxNumController->SetMaxNum(num);
}

void PointBarrel::OnEventTrigger()
{
    m_pSpawn->SetSpawning(true);
}