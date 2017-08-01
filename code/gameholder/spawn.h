#ifndef spawn_h__
#define spawn_h__

#include "sceneobject.h"
#include "path_data.h"
#include "regular_data.h"

class Monster;
class SpawnController;
class SpawnWave;

/*
 *	怪物出生点
 *  Spawn->(list)SpawnWave->(list)MonsterData
 */
class Spawn : public SceneObject
{
    CnDeclareRTTI
public:
    Spawn(int32 type);
    virtual ~Spawn();

    virtual void onEnterScene(Scene* pScene);
    virtual void onLeaveScene(Scene* pScene);

    virtual void Update(uint32 frame_count);
    virtual void OnDestory() {}

    void UpdateSpawn();

    void OnMonsterDead(Monster* pDeadMonster);
    void OnMonsterDestory(Monster* pDesMonster);

    int32 GetSpawnMonsterNumber();              // 出生的怪数量

    uint32 GetSpawnId() {return m_spawnId;}
    void SetSpawnId(uint32 id) {m_spawnId = id;}

    void SetLoop(bool loop) {m_loop = loop;}
    bool GetLoop() {return m_loop;}

    void SetNoCollisionObject(bool c) {m_NoCollisionObject = c;}
    bool GetNoCollisionObject() {return m_NoCollisionObject;}

    void SetSpawnRadius(uint32 max_radius, uint32 min_radius=0) {m_spawnMaxRadius = max_radius; m_spawnMinRadius = min_radius;}

    // 出怪控制器
    void AddWaveControl(const SpawnWaveControlData& waveControlData);
    void AddWaveControl(SpawnController* pController);

    // 波操作
    void AddWave(const SpawnWaveData& waveData);
    void AddWave(SpawnWave* pWave);
    SpawnWave* GetWave(int32 index);
    uint32 GetWaveNum() { return (uint32)m_waves.size(); }
    SpawnWave* GetCurrentWave();
    int32 GetCurrentWaveIndex() {return m_currentWaveIndex;}
    int32 GetCurrentWaveMonsterIndex() {return m_currentMonsterIndex;}
    int32 GetCurrentWaveMonsterNum();
    int32 GetCurrentWaveRemainNum();
    bool IsWaveMonsterEnd();        // 当前波的怪物是否出生完了
    bool IsLastWave();              // 当前波是否最后一波
    void PrevWave();
    void NextWave();
    void UpdateWave();

    bool IsSpawnEnd();              // 是否所有的怪都出完了

    // 刷怪操作(返回实际出了多少个怪)
    int32 SpawnMonster(int32 Number);     // 刷当前波的多个怪(-1为当前波所有怪)

    // 是否刷怪
    void SetSpawning(bool val){ m_IsSpawning = val; }
    bool GetSpawning() {return m_IsSpawning;}

    // 清除所有怪,并关闭循环
    void SetClearAllMosters();

private:
    void    SendSpawnAllDead();

private:
    uint32  m_spawnId;                          // 出生点ID
    uint32  m_spawnMaxRadius;                   // 刷怪范围外径
    uint32  m_spawnMinRadius;                   // 刷怪范围内径
    bool    m_loop;                             // 循环刷怪
    bool    m_NoCollisionObject;                // 无单位碰撞
    bool    m_RandomAngle;                      // 怪物随机方向

    bool    m_IsSpawning;                       // 是否正在出怪
    bool    m_bFirstUpdate;

    int32   m_currentMonsterIndex;              // 当前这波的第几个怪
    int32   m_currentWaveIndex;                 // 当前是第几波
    std::vector<SpawnWave*>      m_waves;            // 每波信息
    std::vector<Monster*>   m_spawndMonster;    // 出生的怪
    std::vector<SpawnController*>   m_SpawnControllers; // 出怪控制器
};

#endif // spawn_h__
