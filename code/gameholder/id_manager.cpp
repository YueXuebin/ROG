#include "gameholder_pch.h"
#include "id_manager.h"
#include "id_pool_manager.h"


const uint32 MASK_SHIFT = 24;
const uint32 MASK_ID_BITS =	(1<<MASK_SHIFT)-1;

enum IdMask 
{
    SCENE_ID_MASK,
    PLAYER_ID_MASK,
    MONSTER_ID_MASK,
    BOSS_ID_MASK,
    NPC_ID_MASK,
    PET_ID_MASK,
    OFFLINE_PLAYER_ID_MASK,
    TOWER_ID_MASK,
    TRIGGER_ID_MASK,
    CHECKPOINT_ID_MASK,
    SPAWN_ID_MASK,
    PORT_ID_MASK,
    TOUCH_ID_MASK,
    DROPOBJECT_ID_MASK,

    ID_MASK_COUNT           // 最大256
};

// IdMask 和 ObjectType 的元素一一对应。
COMPILE_ASSERT((int32)ID_MASK_COUNT == (int32)OBJ_SUM);

IdManager*	IdManager::m_Instance = NULL;
IdManager	g_IdManager;

CNTHREAD_LOCAL bool g_MainThread = false;

IdManager::IdManager()
{
    CnVerify(!m_Instance);
    m_Instance = this;

    g_MainThread = true;

    // Zero, 0, is preserved for nullptr.
    m_SceneIdBase = m_PlayerIdBase = m_MonsterIdBase = m_BossIdBase = m_NpcIdBase = m_PetIdBase = m_OfflinePlayerIdBase = m_TowerIdBase = m_TriggerIdBase = m_CheckpointIdBase = m_SpawnIdBase = m_PortIdBase = m_TouchIdBase = m_DropObjectIdBase = 1;
    m_SceneCount = m_PlayerCount = m_MonsterCount = m_BossCount = m_NpcCount = m_PetCount = m_OfflinePlayerCount = m_TowerCount = m_TriggerCount = m_CheckpointCount = m_SpawnCount = m_PortCount = m_TouchCount = m_DropObjectCount = 0;
}

IdManager::~IdManager()
{
    m_Instance = NULL;
}

uint32 IdManager::GenerateId(uint32 objType)
{
    uint32 newId = _GenerateId(objType);
    while( !IdPoolManager::Instance()->RegisterID(newId) )     // 确保ID可用
    {
        newId = _GenerateId(objType);
    }
    // 计数
    switch(objType)
    {
    case OBJ_SCENE:
        m_SceneCount++;
        break;
    case OBJ_PLAYER:
        m_PlayerCount++;
        break;
    case OBJ_MONSTER:
        m_MonsterCount++;
        break;
    case OBJ_BOSS:
        m_BossCount++;
        break;
    //case OBJ_NPC:
    //    m_NpcCount++;
    //    break;
    case OBJ_PET:
        m_PetCount++;
        break;
    case OBJ_OFFLINE_PLAYER:
        m_OfflinePlayerCount++;
        break;
    case OBJ_TOWER:
        m_TowerCount++;
        break;
    case OBJ_TRIGGER:
        m_TriggerCount++;
        break;
    case OBJ_CHECKPOINT:
        m_CheckpointCount++;
        break;
    case OBJ_SPAWN:
        m_SpawnCount++;
        break;
    case OBJ_PORT:
        m_PortCount++;
        break;
    case OBJ_TOUCH:
        m_TouchCount++;
        break;
    case OBJ_DROP_OBJECT:
        m_DropObjectCount++;
        break;
    default:
        CnVerify(false);
        break;
    }


    return newId;
}

uint32 IdManager::_GenerateId(uint32 objType)
{
    CnAssert(g_MainThread);

    uint32 id = 0xffffffff;
    uint32 mask = 0;
    switch(objType)
    {
    case OBJ_SCENE:
        {
            mask = SCENE_ID_MASK << MASK_SHIFT;

            id = m_SceneIdBase + mask;
            m_SceneIdBase++;
            if(m_SceneIdBase > MASK_ID_BITS)
                m_SceneIdBase = 1;
        }
        break;
    case OBJ_PLAYER:
        {
            mask = PLAYER_ID_MASK << MASK_SHIFT;

            id = m_PlayerIdBase + mask;
            m_PlayerIdBase++;
            if(m_PlayerIdBase > MASK_ID_BITS)
                m_PlayerIdBase = 1;
        }
        break;
    case OBJ_MONSTER:
        {
            mask = MONSTER_ID_MASK << MASK_SHIFT;

            id = m_MonsterIdBase + mask;
            m_MonsterIdBase++;
            if(m_MonsterIdBase > MASK_ID_BITS)
                m_MonsterIdBase = 1;
        }
        break;
    case OBJ_BOSS:
        {
            mask = BOSS_ID_MASK << MASK_SHIFT;

            id = m_BossIdBase + mask;
            m_BossIdBase++;
            if ( m_BossIdBase > MASK_ID_BITS)
                m_BossIdBase = 1;
        }
        break;
    //case OBJ_NPC:
    //    {
    //        mask = NPC_ID_MASK << MASK_SHIFT;

    //        id = m_NpcIdBase + mask;
    //        m_NpcIdBase++;
    //        if(m_NpcIdBase > MASK_ID_BITS)
    //            m_NpcIdBase = 1;
    //    }
    //    break;
    case OBJ_PET:
        {
            mask = PET_ID_MASK << MASK_SHIFT;

            id = m_PetIdBase + mask;
            m_PetIdBase++;
            if(m_PetIdBase > MASK_ID_BITS)
                m_PetIdBase = 1;
        }
        break;
    case OBJ_OFFLINE_PLAYER:
        {
            mask = OFFLINE_PLAYER_ID_MASK << MASK_SHIFT;

            id = m_OfflinePlayerIdBase + mask;
            m_OfflinePlayerIdBase++;
            if(m_OfflinePlayerIdBase > MASK_ID_BITS)
                m_OfflinePlayerIdBase = 1;
        }
        break;
    case OBJ_TOWER:
        {
            mask = TOWER_ID_MASK << MASK_SHIFT;

            id = m_TowerIdBase + mask;
            m_TowerIdBase++;
            if(m_TowerIdBase > MASK_ID_BITS)
                m_TowerIdBase = 1;
        }
        break;
    case OBJ_TRIGGER:
        {
            mask = TRIGGER_ID_MASK << MASK_SHIFT;

            id = m_TriggerIdBase + mask;
            m_TriggerIdBase++;
            if(m_TriggerIdBase > MASK_ID_BITS)
                m_TriggerIdBase = 1;
        }
        break;
    case OBJ_CHECKPOINT:
        {
            mask = CHECKPOINT_ID_MASK << MASK_SHIFT;

            id = m_CheckpointIdBase + mask;
            m_CheckpointIdBase++;
            if (m_CheckpointIdBase > MASK_ID_BITS)
                m_CheckpointIdBase = 1;
        }
        break;
    case OBJ_SPAWN:
        {
            mask = SPAWN_ID_MASK << MASK_SHIFT;

            id = m_SpawnIdBase + mask;
            m_SpawnIdBase++;
            if (m_SpawnIdBase > MASK_ID_BITS)
                m_SpawnIdBase = 1;
        }
        break;
    case OBJ_PORT:
        {
            mask = PORT_ID_MASK << MASK_SHIFT;

            id = m_PortIdBase + mask;
            m_PortIdBase++;
            if (m_PortIdBase > MASK_ID_BITS)
                m_PortIdBase = 1;
        }
        break;
    case OBJ_TOUCH:
        {
            mask = TOUCH_ID_MASK << MASK_SHIFT;

            id = m_TouchIdBase + mask;
            m_TouchIdBase++;
            if(m_TouchIdBase > MASK_ID_BITS)
                m_TouchIdBase = 1;
        }
        break;
    case OBJ_DROP_OBJECT:
        {
            mask = DROPOBJECT_ID_MASK << MASK_SHIFT;

            id = m_DropObjectIdBase + mask;
            m_DropObjectIdBase++;
            if(m_DropObjectIdBase > MASK_ID_BITS)
                m_DropObjectIdBase = 1;
        }
        break;
    default:
        CnVerify(false);
        break;
    }

    return id;
}

void IdManager::ReleaseId(uint32 id)
{
    CnAssert(g_MainThread);

    bool ret = IdPoolManager::Instance()->UnregisterID(id);
    if(ret)
    {
#ifdef DEBUG
        IdPoolManager::Instance()->CleanZero();
#else
        if(id % 1000 == 0)
            IdPoolManager::Instance()->CleanZero();        // 间隔一段时间再做清理
#endif
    }
    else
        CnAssert(ret);          // 确保正确注销id


    uint32 mask = id >> MASK_SHIFT;
    switch(mask)
    {
    case SCENE_ID_MASK:
        CnAssert(m_SceneCount > 0);
        m_SceneCount--;
        break;
    case PLAYER_ID_MASK:
        CnAssert(m_PlayerCount > 0);
        m_PlayerCount--;
        break;
    case MONSTER_ID_MASK:
        CnAssert(m_MonsterCount > 0);
        m_MonsterCount--;
        break;
    case BOSS_ID_MASK:
        CnAssert(m_BossCount > 0);
        m_BossCount--;
        break;
    case NPC_ID_MASK:
        CnAssert(m_NpcCount > 0);
        m_NpcCount--;
        break;
    case PET_ID_MASK:
        CnAssert(m_PetCount > 0);
        m_PetCount--;
        break;
    case OFFLINE_PLAYER_ID_MASK:
        CnAssert(m_OfflinePlayerCount > 0);
        m_OfflinePlayerCount--;
        break;
    case TOWER_ID_MASK:
        CnAssert(m_TowerCount > 0);
        m_TowerCount--;
        break;
    case TRIGGER_ID_MASK:
        CnAssert(m_TriggerCount > 0);
        m_TriggerCount--;
        break;
    case CHECKPOINT_ID_MASK:
        CnAssert(m_CheckpointCount > 0);
        m_CheckpointCount--;
        break;
    case SPAWN_ID_MASK:
        CnAssert(m_SpawnCount > 0);
        m_SpawnCount--;
        break;
    case PORT_ID_MASK:
        CnAssert(m_PortCount > 0);
        m_PortCount--;
        break;
    case TOUCH_ID_MASK:
        CnAssert(m_TouchCount > 0);
        m_TouchCount--;
        break;
    case DROPOBJECT_ID_MASK:
        CnAssert(m_DropObjectCount > 0);
        m_DropObjectCount--;
        break;
    default:
        CnAssert(false);
        break;
    }
}

