/*
*	有些对象Id管理器,负责Id的分配回收等
*/
#ifndef IdManager_h__
#define IdManager_h__


class IdManager
{
    static IdManager*	m_Instance;
public:
    IdManager();
    ~IdManager();

    static IdManager* Instance() {return m_Instance;}

    uint32	GenerateId(uint32 objType);
    void	ReleaseId(uint32 id);

    uint32	GetPlayerCount() const {return m_PlayerCount;}
    uint32	GetMonsterCount() const {return m_MonsterCount;}
    uint32  GetBossCount() const {return m_BossCount;}
    uint32	GetNpcCount() const {return m_NpcCount;}
    uint32  GetPetCount() const {return m_PetCount;}
    uint32  GetOfflinePlayerCount() const {return m_OfflinePlayerCount;}
    uint32	GetTriggerCount() const {return m_TriggerCount;}
    uint32  GetCheckpointCount() const {return m_CheckpointCount;}
    uint32  GetSpawnCount() const {return m_SpawnCount;}
    uint32  GetPortCount() const {return m_PortCount;}
    uint32  GetTouchCount() const {return m_TouchCount;}
    uint32  GetDropObjectCount() const {return m_DropObjectCount;}

private:
    uint32 _GenerateId(uint32 objType);

private:
    uint32	m_SceneIdBase;
    uint32	m_SceneCount;

    uint32	m_PlayerIdBase;
    uint32	m_PlayerCount;

    uint32	m_MonsterIdBase;
    uint32	m_MonsterCount;

    uint32  m_BossIdBase;
    uint32  m_BossCount;

    uint32	m_NpcIdBase;
    uint32	m_NpcCount;

    uint32  m_PetIdBase;
    uint32  m_PetCount;

    uint32  m_OfflinePlayerIdBase;
    uint32  m_OfflinePlayerCount;

    uint32  m_TowerIdBase;
    uint32  m_TowerCount;

    uint32	m_TriggerIdBase;
    uint32	m_TriggerCount;

    uint32  m_CheckpointIdBase;
    uint32  m_CheckpointCount;

    uint32  m_SpawnIdBase;
    uint32  m_SpawnCount;

    uint32  m_PortIdBase;
    uint32  m_PortCount;

    uint32  m_TouchIdBase;
    uint32  m_TouchCount;

    uint32  m_DropObjectIdBase;
    uint32  m_DropObjectCount;
};



#endif // IdManager_h__
