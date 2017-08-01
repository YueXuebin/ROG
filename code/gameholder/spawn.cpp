#include "gameholder_pch.h"
#include "spawn.h"
#include "scene.h"
#include "regular.h"
#include "monster.h"
#include "trigger.h"
#include "game_define.h"
#include "ai_controller.h"
#include "event_define.h"
#include "spawn_controller_factory.h"
#include "spawn_controller.h"
#include "performance_analysis.h"
#include "state_define.h"
#include "state_controller.h"
#include "spawn_wave.h"

CnImplementRTTI(Spawn, SceneObject);

Spawn::Spawn(int32 type):
SceneObject(OBJ_SPAWN)
{
    m_spawnId = 0;
    m_spawnMaxRadius = 0;
    m_spawnMinRadius = 0;

    m_currentMonsterIndex = 0;
    m_currentWaveIndex = 0;

    m_bFirstUpdate = true;
    m_IsSpawning = true;

    m_RandomAngle = false;
    m_NoCollisionObject = false;
}

Spawn::~Spawn()
{
    for(std::vector<SpawnWave*>::iterator iter = m_waves.begin(); iter != m_waves.end(); iter++)
    {
        CnDelete *iter;
    }
    m_waves.clear();

    for(std::vector<SpawnController*>::iterator iter2 = m_SpawnControllers.begin(); iter2 != m_SpawnControllers.end(); iter2++)
    {
        CnDelete *iter2;
    }
    m_SpawnControllers.clear();
}

int32 Spawn::GetSpawnMonsterNumber()
{
    return m_spawndMonster.size();
}

void Spawn::AddWaveControl(const SpawnWaveControlData& waveControlData)
{
    SpawnController* pController = CreateSpawnController(this, waveControlData);
    CnAssert(pController);
    if(pController)
    {
        AddWaveControl(pController);
    }
}

void Spawn::AddWaveControl(SpawnController* pController)
{
    CnAssert(pController);
    m_SpawnControllers.push_back(pController);
}

void Spawn::AddWave(const SpawnWaveData& waveData)
{
    SpawnWave* pWave = CnNew SpawnWave;

    pWave->delay = waveData.delay;

    for(uint32 i=0; i<waveData.MonsterDatas.size(); i++)
    {
        const MonsterData& monsterData = waveData.MonsterDatas[i];
        pWave->waveMonsters.push_back(monsterData);
    }

    AddWave(pWave);
}

void Spawn::AddWave( SpawnWave* pWave )
{
    m_waves.push_back(pWave);
}

SpawnWave* Spawn::GetWave(int32 index)
{
    if(index<0 || index>=(int32)m_waves.size())
    {
        return NULL;
    }
    return m_waves[index];
}

SpawnWave* Spawn::GetCurrentWave()
{
    return GetWave(m_currentWaveIndex);
}

int32 Spawn::GetCurrentWaveMonsterNum()
{
    SpawnWave* pWave = GetWave(m_currentWaveIndex);
    if(!pWave)
        return 0;

    return pWave->GetAllMonsterNum();
}

int32 Spawn::GetCurrentWaveRemainNum()
{
    SpawnWave* pWave = GetWave(m_currentWaveIndex);
    if(!pWave)
        return 0;

    return pWave->GetAllMonsterNum() - m_currentMonsterIndex;
}

bool Spawn::IsSpawnEnd()
{
    if(m_loop)
        return false;       // 循环刷怪，永不结束
    else
        return (IsLastWave() && IsWaveMonsterEnd());
}

bool Spawn::IsWaveMonsterEnd()
{
    SpawnWave* pWave = GetWave(m_currentWaveIndex);
    if(pWave)
    {
        return m_currentMonsterIndex >= (int32)pWave->GetAllMonsterNum();
    }
    return true;
}

bool Spawn::IsLastWave()
{
    return (m_currentWaveIndex+1) >= (int32)m_waves.size();
}

void Spawn::PrevWave()
{
    if(m_currentWaveIndex <= 0)
    {

    }
    else
    {
        m_currentMonsterIndex = 0;
        m_currentWaveIndex--;
    }
}

void Spawn::NextWave()
{
    if(m_currentWaveIndex < (int32)m_waves.size())
    {
        m_currentWaveIndex++;
    }

    if(m_currentWaveIndex >= (int32)m_waves.size())
    {
        if(m_loop)
        {
            m_currentWaveIndex = 0;         // 循环到第一波
            m_currentMonsterIndex = 0;
        }
    }
    else
    {
        m_currentMonsterIndex = 0;
    }
}

void Spawn::Update(uint32 frame_count)
{
    PERFORM_HELPER;
    SceneObject::Update(frame_count);

    // 出怪控制
    if(!m_IsSpawning)
        return;

    if(m_bFirstUpdate)
    {
        m_bFirstUpdate = false;
        if(m_SpawnControllers.size() == 0)
        {
            CnWarn("dungeon=%d scene=%d spawn=%d not control\n", GetScene()->GetDungeonID(), GetScene()->GetSceneID(), GetSpawnId());
        }

        for(std::vector<SpawnController*>::iterator iter = m_SpawnControllers.begin(); iter != m_SpawnControllers.end(); iter++)
        {
            SpawnController* pSpawnController = *iter;
            pSpawnController->FirstUpdate();
        }
        return;
    }

    for(std::vector<SpawnController*>::iterator iter = m_SpawnControllers.begin(); iter != m_SpawnControllers.end(); iter++)
    {
        SpawnController* pSpawnController = *iter;
        pSpawnController->Update();
    }

    UpdateSpawn();
}

void Spawn::UpdateSpawn()
{
    int32 spawnNum = 999999;        // 一次update的出怪数

    for(std::vector<SpawnController*>::iterator iter = m_SpawnControllers.begin(); iter != m_SpawnControllers.end(); iter++)
    {
        SpawnController* pSpawnController = *iter;
        spawnNum = pSpawnController->FiltSpawnNum(spawnNum);
    }

    if(spawnNum <= 0)
        return;

    int32 remain = spawnNum;
    do 
    {
        int32 num = SpawnMonster(remain);
        remain -= num;

        if(num == 0)
            NextWave();

    } while (remain > 0 && !IsSpawnEnd());

}

// Number == -1　把该波的怪全部刷出
int32 Spawn::SpawnMonster(int32 Number)
{
    if(m_currentWaveIndex >= (int32)m_waves.size() || m_currentWaveIndex < 0)
        return 0;

    if(Number == 0) return 0;

    SpawnWave* pWave = m_waves[m_currentWaveIndex];

    if(m_currentMonsterIndex >= (int32)pWave->GetAllMonsterNum())
    {
        return 0;
    }

    int32 spawnMonsterCount  = 0;       // 出怪计数
    int32 m_ThisMonsterIndex = m_currentMonsterIndex;

    const std::vector<MonsterData>& WaveSpawnList = pWave->waveMonsters;

    int32   spawnIndex = -1;

    for(std::vector<MonsterData>::const_iterator iter = WaveSpawnList.begin(); iter != WaveSpawnList.end(); iter++)
    {
        const MonsterData& waveSpawn = *iter;

        // 数量
        for(int32 i=0; i<waveSpawn.num; i++)
        {
            spawnIndex++;
            if(spawnIndex < m_ThisMonsterIndex)
                continue;
            if(Number > 0 && spawnIndex >= (m_ThisMonsterIndex+Number))
                break;

            m_currentMonsterIndex = spawnIndex+1;

            // 位置和角度
            CoVec2 spawnPos = GetScene()->FindValidPositionInRing(CoVec2(GetX(), GetY()), m_spawnMaxRadius, m_spawnMinRadius, PHYSICAL_RADIUS);
            int32 randomAngle = (int32)Crown::SDRandom(360);

            // 出怪事件
            if(m_currentMonsterIndex == 1)
            {   // 每波第一只怪时，发送波事件
                GetScene()->SendEvent(0, SCENE_EVENT_SPAWN_WAVE_START, m_currentWaveIndex, 0, 0);
            }

            // 出怪
            MonsterData newMonsterData(waveSpawn);
            newMonsterData.x = (int32)spawnPos.x;
            newMonsterData.y = (int32)spawnPos.y;
            if(m_RandomAngle)
                newMonsterData.angle = randomAngle;
            else
                newMonsterData.angle = GetAngle();
            Monster* pNewMonster = GetScene()->GetRegular()->AddMonster(newMonsterData);
            if(!pNewMonster)
            {
                CnAssert(false);
                continue;
            }

            // 设置怪的连接出身点
            pNewMonster->ConnectSpawn(this);

            // 设置碰撞
            pNewMonster->SetNoCollisionObject(m_NoCollisionObject);
            
            // 重置怪物的AI出身点
            pNewMonster->GetAIController()->SetBasePosition((int32)spawnPos.x, (int32)spawnPos.y);

            m_spawndMonster.push_back(pNewMonster);

            spawnMonsterCount++;
        }
    }

    return spawnMonsterCount;
}

void Spawn::onEnterScene(Scene * pScene)
{
    SceneObject::onEnterScene(pScene);
}

void Spawn::onLeaveScene(Scene * pScene)
{
    SceneObject::onLeaveScene(pScene);
}

void Spawn::OnMonsterDead( Monster* pDeadMonster )
{
    CnAssert(pDeadMonster);
    // 出生的怪死亡了
    bool exist = false;
    uint32 index = 0;
    for(;index<m_spawndMonster.size(); index++)
    {
        if(m_spawndMonster[index] == pDeadMonster)
        {
            exist = true;
            break;
        }
    }

    CnAssert(exist);        // 连接到该出生点的怪，一定在怪物列表中

    // 死亡的怪物从列表中去除
    m_spawndMonster.erase(m_spawndMonster.begin() + index);

    SendSpawnAllDead();
}

void Spawn::OnMonsterDestory( Monster* pDesMonster )
{
    CnAssert(pDesMonster);
    // 怪物销毁了
    bool exist = false;
    uint32 index = 0;
    for(;index<m_spawndMonster.size(); index++)
    {
        if(m_spawndMonster[index] == pDesMonster)
        {
            exist = true;
            break;
        }
    }
    
    // 怪物应该死亡时就不在列表中了，但是有可能不经过死亡就直接销毁
    if(exist)
    {
        // 消耗的怪物从列表中去除
        m_spawndMonster.erase(m_spawndMonster.begin() + index);

        SendSpawnAllDead();
    }

}

void Spawn::SetClearAllMosters()
{
    // 关闭循环
    m_loop = false;
    // 到最后一波的尾部
    m_currentWaveIndex = (int32)m_waves.size() - 1;
    m_currentMonsterIndex = (int32)GetWave(m_currentWaveIndex)->GetAllMonsterNum();
}

void Spawn::SendSpawnAllDead()
{
    // 怪全部销毁,抛事件
    if (m_spawndMonster.size() == 0)
    {
        GetScene()->SendEvent(0, SCENE_EVENT_SPAWN_WAVE_ALL_DEAD, m_currentWaveIndex, GetWaveNum(), 0);

        if(IsSpawnEnd())
        {
            GetScene()->SendEvent(0, SCENE_EVENT_SPAWN_ALL_DEAD, m_spawnId, 0, 0);
        }
    }
}



