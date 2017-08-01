#include "gameholder_pch.h"
#include "monster_config_manager.h"
#include "point_monster.h"
#include "scene.h"
#include "spawn.h"
#include "spawn_wave.h"
#include "spawn_controller_speed.h"
#include "spawn_controller_max_num.h"
#include "spawn_controller_factory.h"
#include "point_barrel.h"
#include "event_define.h"


CnImplementRTTI(PointMonster, PointBase);

PointMonster::PointMonster(RegularBuilder* pRogRegular, uint32 CheckPointID) :
PointBase(pRogRegular, CheckPointID, POINT_TYPE_MONSTER),
m_pSpawn(NULL)
{
    m_DelayTime = 0;
    m_StartSpawn = false;

    m_TriggerID = -1;

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
    m_pMaxNumController->SetMaxNum(20);
    m_pSpawn->AddWaveControl(m_pMaxNumController);

    // 设置一波怪物数据
    MonsterData md;
    md.creatureType = 1;
    md.creatureId = 1001;
    md.level = Crown::SDMax(m_pRogRegular->GetMapLevel(), 1);            // 怪物等级=地图等级
    md.num = 0;

    SpawnWaveData wavaData;
    wavaData.MonsterDatas.push_back(md);
    m_pSpawn->AddWave(wavaData);
}

PointMonster::~PointMonster()
{

}

bool PointMonster::Build()
{
    if (m_EventId != 0)
    {
        return true;
    }

    if(m_TriggerID <= 0)
    {
        m_StartSpawn = true;
        if(m_DelayTime <= 0)
            m_pSpawn->SetSpawning(true);
    }
    return true;
}

void PointMonster::Update(uint32 curr_tick)
{
    if(m_StartSpawn && !m_pSpawn->GetSpawning())
    {
        m_DelayTime -= g_ServerUpdateTime;
        if(m_DelayTime <= 0)
            m_pSpawn->SetSpawning(true);
    }
}

void PointMonster::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    PointBase::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    if(eventId == TRIGGER_REGULAR_EVENT)
    {   // 玩家触发触发器(trigger_id)
        int32 triggerID = arg1;
        if(triggerID == GetTriggerID() && !m_StartSpawn)
        {
            m_StartSpawn = true;
            if(m_DelayTime <= 0 && !m_pSpawn->GetSpawning())
            {
                m_pSpawn->SetSpawning(true);
            }
        }
    }
}

void PointMonster::SetMax(uint32 max)
{
    m_pMaxNumController->SetMaxNum(max);
}

void PointMonster::SetRadius(int32 max_radius, int32 min_radius)
{
    m_pSpawn->SetSpawnRadius(max_radius, min_radius);
}

void PointMonster::SetLoop(bool loop)
{
    m_pSpawn->SetLoop(loop);
}

void PointMonster::SetCantMove(bool val)
{
    MonsterData& monsterData = m_pSpawn->GetWave(0)->waveMonsters[0];
    monsterData.cantMove = val;
}

void PointMonster::SetAttackable(bool val)
{
    MonsterData& monsterData = m_pSpawn->GetWave(0)->waveMonsters[0];
    monsterData.attackable = val;
}

void PointMonster::SetMonster(uint32 monsterID, int32 num)
{
    MonsterData& monsterData = m_pSpawn->GetWave(0)->waveMonsters[0];
    monsterData.creatureId = monsterID;
    monsterData.num = num;

    m_MonsterId = monsterID;
}

void PointMonster::SetMovie(const std::string& movie_name, uint32 movie_time, bool attack_able)
{
    MonsterData& monsterData = m_pSpawn->GetWave(0)->waveMonsters[0];
    monsterData.state = STATE_MOVIE;
    monsterData.state_arg = movie_time;
    monsterData.movie_name = movie_name;
    monsterData.movie_attackable = attack_able;
}

void PointMonster::SetLevel(uint32 level)
{
    MonsterData& monsterData = m_pSpawn->GetWave(0)->waveMonsters[0];
    monsterData.level = level;
}

MonsterData* PointMonster::GetMonsterData()
{
    return &m_pSpawn->GetWave(0)->waveMonsters[0];
}

void PointMonster::SetDefaultMovie()
{
    MonsterData& monsterData = m_pSpawn->GetWave(0)->waveMonsters[0];

    const MONSTER_ATTRIBUTE_REVISE_ROOT_MONSTER_STRUCT* monsterStruct = MonsterConfigManager::Instance()->GetMonsterAttributeReviseById(monsterData.creatureId);
    if(!monsterStruct)
    {
        CnAssert(false);
        return;
    }

    if(monsterStruct->movie_name.length()>0 && monsterStruct->movie_time>0)
        SetMovie(monsterStruct->movie_name, monsterStruct->movie_time);
    else
        CnWarn("monster %d has no movie\n", monsterData.creatureId);
}

void PointMonster::SetDelayTime(int32 time)
{
    m_DelayTime = time;
}

void PointMonster::SetSpawnSpeed(int32 speed)
{
    m_pSpeedController->SetSpeedTime(speed);
}

void PointMonster::SetAI(const std::string& aiName)
{
    MonsterData& monsterData = m_pSpawn->GetWave(0)->waveMonsters[0];
    monsterData.ai = aiName;
}

void PointMonster::SetGroup(uint32 group)
{
    MonsterData& monsterData = m_pSpawn->GetWave(0)->waveMonsters[0];
    monsterData.group = group;
}

void PointMonster::SetTalk(bool canTalk)
{
    MonsterData& monsterData = m_pSpawn->GetWave(0)->waveMonsters[0];
    if(canTalk)
        monsterData.showflag = monsterData.showflag | SHOW_FLAG_NEED_TALK;
    else
        monsterData.showflag = monsterData.showflag & (~SHOW_FLAG_NEED_TALK);
}

void PointMonster::SetAlwaysShow(bool alwaysShow)
{
    MonsterData& monsterData = m_pSpawn->GetWave(0)->waveMonsters[0];
    monsterData.isAppearNotifyAll = alwaysShow;
}

// 通过触发器触发出怪
void PointMonster::SetTriggerID(int32 TriggerID)
{
    m_TriggerID = TriggerID;
}

void PointMonster::SetAIGroup(uint32 AIGroupId)
{
    MonsterData& monsterData = m_pSpawn->GetWave(0)->waveMonsters[0];
    monsterData.ai_group = AIGroupId;
}

void PointMonster::SetQuality(uint32 quality)
{
    MonsterData& monsterData = m_pSpawn->GetWave(0)->waveMonsters[0];
    monsterData.quality = quality;
}

void PointMonster::OnEventTrigger()
{
    if(m_TriggerID <= 0)
    {
        m_StartSpawn = true;
        if(m_DelayTime <= 0)
            m_pSpawn->SetSpawning(true);
    }
}