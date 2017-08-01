#include "gameholder_pch.h"
#include "scene.h"
#include "gameholder.h"
#include "player.h"
#include "monster.h"
#include "boss.h"
#include "npc.h"
#include "pet.h"
#include "offline_player.h"
#include "trigger.h"
#include "checkpoint.h"
#include "spawn.h"
#include "regular_manager.h"
#include "regular.h"
#include "gate_handler.h"
#include "vip_config.h"
#include "dungeon_gain_config.h"
#include "dungeon.h"
#include "dungeon_set.h"
#include "scene_notifier.h"
#include "event_dispatcher.h"
#include "battle_recorder.h"
#include "babel_manager.h"
#include "event_define.h"
#include "battle_unit_connecter.h"
#include "touch.h"
#include "drop_object.h"
#include "game_util.h"
#include "performance_analysis.h"
#include "scene_notifier.h"



Scene::Scene(Dungeon* pParent,
             uint32 sceneId,
             const std::list<Teleport>& triggers, 
             CoWorld* pCoWorld,
             b2World* pB2World,
             const std::string& designFileName,
             bool bClose) :
m_HeartBeat(0),
m_SceneTime(0)
{
    m_pDungeon = pParent;
    m_scene_id = sceneId;
    m_bClose = bClose;
    m_InUnitUpdate = false;

    m_dungeon_id = m_pDungeon->GetDungeonID();
    m_channel_id = m_pDungeon->GetChannelID();
    //CnDbg("new scene dungeon=%d scene=%d channel=%d\n", m_dungeon_id, sceneId, m_channel_id);

    // 创建触发器列表
    m_TeleportList = triggers;

    m_EventDispatcher = CnNew EventDispatcher(this);

    // 创建物理世界
    m_pCoWorld = pCoWorld;
    CnAssert(m_pCoWorld);

    // 创建网格场景
    uint32 gridLength = 75;
    int32 gridMinX = RealPosToCellPos(m_pCoWorld->GetMinX(), gridLength);
    int32 gridMaxX = RealPosToCellPos(m_pCoWorld->GetMaxX(), gridLength);
    int32 gridMinY = RealPosToCellPos(m_pCoWorld->GetMinY(), gridLength);
    int32 gridMaxY = RealPosToCellPos(m_pCoWorld->GetMaxY(), gridLength);
    m_pGridWorld = CnNew GridWorld(gridLength, gridMinX, gridMaxX, gridMinY, gridMaxY);

    // 创建box2d物理世界
    m_pB2World = pB2World;
    m_pB2World->SetContactListener(this);

    // 创建导航网格
    m_pNavMesh = CreateNavMesh(m_pCoWorld->GetNavigations(), m_pCoWorld);

    // 创建游戏规则
    m_pRegular = RegularManager::Instance()->CreateRegular(designFileName.c_str());
    CnAssert(m_pRegular);
    m_pRegular->SetScene(this);

    // 创建通知器
    m_useAOI = IsEnableAoi();
    m_pNotifier = CreateNotifier(this, m_useAOI);

    m_EventDispatcher->Init();

    m_CanUseAI = true;

}

Scene::~Scene()
{
    m_EventDispatcher->UnInit();

    // 删除所有非玩家生物
    for(BattleUnitMap::iterator iter = m_BattleUnits.begin(); iter != m_BattleUnits.end(); iter++)
    {
        BattleUnit* pBattleUnit = iter->second;
        if(pBattleUnit->IsKindOf(&Player::ms_RTTI))
            continue;

        pBattleUnit->SetDestory();
    }

    for(BattleUnitMap::iterator iter = m_BattleUnits.begin(); iter != m_BattleUnits.end(); )
    {
        BattleUnit* pBattleUnit = iter->second;
        iter++;

        if(pBattleUnit->IsKindOf(&Player::ms_RTTI))
            continue;

        _UpdateDelObject(pBattleUnit);
        CnDelete pBattleUnit;
    }
    CnAssert(m_BattleUnits.size() == m_Players.size());

    // 删除所有非生物对象
    for(SceneObjectMap::iterator iter = m_SceneObjects.begin(); iter != m_SceneObjects.end(); )
    {
        SceneObject* pSceneObject = iter->second;
        iter++;

        pSceneObject->SetDestory();

        _UpdateDelObject(pSceneObject);
        CnDelete pSceneObject;
    }
    CnAssert(m_SceneObjects.size() == 0);

    CnDelete m_pNotifier;

    CnDelete m_pRegular;

    DestroyNavMesh(m_pNavMesh);

    CnDelete m_pB2World;
    m_pB2World = NULL;

    CnAssert(m_pCoWorld->GetObjectCount() == m_Players.size());
    CnDelete m_pCoWorld;        // 物理世界由Scene释放

    CnDelete m_pGridWorld;

    CnDelete m_EventDispatcher;

    //CnDbg("delete scene dungeon=%d scene=%d channel=%d\n", m_dungeon_id, m_scene_id, m_channel_id);
}

uint32 Scene::GetUnionID()
{
    return UNION_ID(m_channel_id, m_dungeon_id, m_scene_id);
}

BattleUnitConnecter* Scene::GetBattleUnitConnecter()
{
    return GetDungeon()->GetDungeonSet()->GetBattleUnitConnecter();
}

void Scene::Update(uint32 frame_count)
{
    PERFORM_HELPER;

    ++m_HeartBeat;
    m_SceneTime += g_ServerUpdateTime;

    bool isUpdate = m_pRegular->IsGameStart();

    if(m_HeartBeat == 1)
        isUpdate = true;           // first update,使关卡初始化

    if(!isUpdate)
        return;

    // ================================================================================
    // box2d物理引擎update(n次速度约束迭代,不做位置约束迭代)
    PERFORM_NAME_BEGIN("b2World::Step");
    m_pB2World->Step((real)g_ServerUpdateTime/1000, 2, 0);
    PERFORM_NAME_END("b2World::Step");

    // ================================================================================
    // 规则引擎update
    PERFORM_NAME_BEGIN("m_pRegular::Update");
    m_pRegular->Update(frame_count*g_ServerUpdateTime);
    PERFORM_NAME_END("m_pRegular::Update");

    // ================================================================================
    // 事件分发器
    m_EventDispatcher->Update();

    // ================================================================================
    // 所有生物的Update
    m_InUnitUpdate = true;
    for(BattleUnitMap::iterator iter = m_BattleUnits.begin(); iter != m_BattleUnits.end(); iter++)
    {
        PERFORM_NAME_BEGIN("pBattleUnit::Update");
        BattleUnit* pBattleUnit = iter->second;
        pBattleUnit->Update(frame_count);
        PERFORM_NAME_END("pBattleUnit::Update");
    }

    // ================================================================================
    // 所有物件对象的Update
    for(SceneObjectMap::iterator iter = m_SceneObjects.begin(); iter != m_SceneObjects.end(); iter++)
    {
        PERFORM_NAME_BEGIN("pSceneObject::Update");
        SceneObject* pSceneObject = iter->second;
        pSceneObject->Update(frame_count);
        PERFORM_NAME_END("pSceneObject::Update");
    }

    // ================================================================================
    // 场景通知器
    PERFORM_NAME_BEGIN("m_pNotifier::Update");
    m_pNotifier->Update();
    PERFORM_NAME_END("m_pNotifier::Update");

    // ================================================================================
    // 整合在Update过程中加入的对象
    for(std::vector<SceneObject*>::iterator sIter = m_TempObjectList.begin(); sIter != m_TempObjectList.end(); sIter++)
    {
        SceneObject* pObject = *sIter;
        if(pObject->IsKindOf(&BattleUnit::ms_RTTI))
        {
            CnAssert(m_BattleUnits.find(pObject->GetId()) == m_BattleUnits.end());
            m_BattleUnits[pObject->GetId()] = CnDynamicCast(BattleUnit, pObject);
        }
        else
        {
            CnAssert(m_SceneObjects.find(pObject->GetId()) == m_SceneObjects.end());
            m_SceneObjects[pObject->GetId()] = pObject;
        }
    }
    m_TempObjectList.clear();
    
    m_InUnitUpdate = false;

    // ================================================================================
    // 销毁战斗对象
    for(BattleUnitMap::iterator dIter = m_BattleUnits.begin(); dIter != m_BattleUnits.end();)
    {
        BattleUnit* pBattleUnit = dIter->second;
        if(pBattleUnit->isDestory())
        {
            CnAssert(!pBattleUnit->IsKindOf(&Player::ms_RTTI));     // 玩家对象，不会通过逻辑销毁
            ++dIter;
           _UpdateDelObject(pBattleUnit);
            CnDelete pBattleUnit;
        }
        else
            dIter++;
    }

    // 销毁逻辑对象
    for(SceneObjectMap::iterator dIter = m_SceneObjects.begin(); dIter != m_SceneObjects.end();)
    {
        SceneObject* pSceneObject = dIter->second;
        if(pSceneObject->isDestory())
        {
             ++dIter;
            _UpdateDelObject(pSceneObject);
            CnDelete pSceneObject;
        }
        else
            dIter++;
    }

}

void Scene::_UpdateDelObject(SceneObject* pObject)
{
    if(pObject->IsKindOf(&Monster::ms_RTTI))
        DelMonster(pObject->GetId());
    else if (pObject->IsKindOf(&Boss::ms_RTTI))
    {
        // 解除世界Boss的联系
        Boss* pDelBoss = (Boss*)pObject;
        BattleUnitCommunicator* pCommunicator = pDelBoss->GetCommunicator();
        if(pCommunicator)
        {
            BattleUnitConnecter* pBattleunitConnecter = GetBattleUnitConnecter();
            pBattleunitConnecter->DisconnectBattleUnit(pDelBoss->GetInstanceName(), pDelBoss);
        }
        CnAssert(!pDelBoss->GetCommunicator());

        DelBoss(pObject->GetId());
    }
    else if(pObject->IsKindOf(&Pet::ms_RTTI))
        DelPet(pObject->GetId());
    else if(pObject->IsKindOf(&Trigger::ms_RTTI))
        DelTrigger(pObject->GetId());
    else if (pObject->IsKindOf(&Checkpoint::ms_RTTI))
        DelCheckpoint(pObject->GetId());
    else if (pObject->IsKindOf(&Spawn::ms_RTTI))
        DelSpawn(pObject->GetId());
    else if(pObject->IsKindOf(&OfflinePlayer::ms_RTTI))
        DelOfflinePlayer(pObject->GetId());
    else if(pObject->IsKindOf(&Touch::ms_RTTI))
        DelTouch(pObject->GetId());
    else if(pObject->IsKindOf(&DropObject::ms_RTTI))
        DelDropItem(pObject->GetId());
    else
        CnAssert(false);    // 删除了一个不应该的对象
}

void Scene::SendEvent( uint32 receiverId, uint32 eventId, int32 arg1, int32 arg2, int32 arg3, uint32 delayTime)
{
    m_EventDispatcher->Create(receiverId, eventId, arg1, arg2, arg3, delayTime);
}

void Scene::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    m_pRegular->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);
}

void Scene::AddPlayer( Player* player )
{
    //CnDbg("dungeon %d channel %d scene %d add player %s\n", GetDungeonID(), GetChannelID(), GetSceneID(), player->GetName().c_str());

    player->SetUnionMapID(GetUnionID());
    player->SetGameStart(false);

    m_Players[player->GetId()] = player;

    AddBattleUnit(player);
}

void Scene::DelPlayer(uint32 id)
{
    PlayerMap::iterator it;
    it = m_Players.find(id);
    if(it == m_Players.end())
    {
        CnAssert(false);        // 删除的玩家对象不存在
        return;
    }

    //CnDbg("dungeon %d channel %d scene %d del player %s\n", GetDungeonID(), GetChannelID(), GetSceneID(), it->second->GetName().c_str());

    DelBattleUnit(id);
    m_Players.erase(it);
}

void Scene::AddMonster( Monster* monster )
{
    m_Monsters[monster->GetId()] = monster;
    AddBattleUnit(monster);
}

void Scene::DelMonster(uint32 id)
{
    DelBattleUnit(id);

    MonsterMap::iterator it;
    it = m_Monsters.find(id);
    CnAssert(it != m_Monsters.end());
    m_Monsters.erase(it);
}

void Scene::AddBoss(Boss* boss)
{
    m_Bosses[boss->GetId()] = boss;
    AddBattleUnit(boss);
}

void Scene::DelBoss(uint32 id)
{
    DelBattleUnit(id);

    BossMap::iterator it;
    it = m_Bosses.find(id);
    CnAssert(it != m_Bosses.end());
    m_Bosses.erase(it);
}
void Scene::AddPet(Pet* pet)
{
    m_Pets[pet->GetId()] = pet;
    AddBattleUnit(pet);
}

void Scene::DelPet(uint32 id)
{
    DelBattleUnit(id);

    PetMap::iterator it;
    it = m_Pets.find(id);
    CnAssert(it != m_Pets.end());
    m_Pets.erase(it);
}

void Scene::AddOfflinePlayer(OfflinePlayer* pOfflinePlayer)
{
    m_OfflinePlayer[pOfflinePlayer->GetId()] = pOfflinePlayer;
    AddBattleUnit(pOfflinePlayer);
}

void Scene::DelOfflinePlayer(uint32 id)
{
    DelBattleUnit(id);

    OfflinePlayerMap::iterator it;
    it = m_OfflinePlayer.find(id);
    m_OfflinePlayer.erase(it);
}

void Scene::AddTrigger(Trigger *trigger)
{
    TriggerMap::iterator it = m_Triggers.find(trigger->GetId());
    if(it != m_Triggers.end())
    {
        CnError("trigger id %d is duplicate\n", trigger->GetId());
        return;
    }

    m_Triggers[trigger->GetId()] = trigger;
    AddSceneObject(trigger);
}

void Scene::DelTrigger(uint32 id)
{
    DelSceneObject(id);

    TriggerMap::iterator it;
    it = m_Triggers.find(id);
    m_Triggers.erase(it);
}

void Scene::AddCheckpoint(Checkpoint * checkpoint)
{
    m_Checkpoints[checkpoint->GetId()] = checkpoint;
    AddSceneObject(checkpoint);
}

void Scene::DelCheckpoint(uint32 id)
{
    DelSceneObject(id);

    CheckPointMap::iterator it;
    it = m_Checkpoints.find(id);
    m_Checkpoints.erase(it);
}

void Scene::AddSpawn(Spawn* spawn)
{
    m_Spawns[spawn->GetId()] = spawn;
    AddSceneObject(spawn);
}

void Scene::DelSpawn(uint32 id)
{
    DelSceneObject(id);

    SpawnMap::iterator it;
    it = m_Spawns.find(id);
    m_Spawns.erase(it);
}

void Scene::AddTouch(Touch* touch)
{
    m_Touchs[touch->GetId()] = touch;
    AddSceneObject(touch);
}

void Scene::DelTouch(uint32 id)
{
    DelSceneObject(id);

    TouchMap::iterator it;
    it = m_Touchs.find(id);
    CnAssert(it != m_Touchs.end());
    m_Touchs.erase(it);
}

void Scene::AddDropItem(DropObject* dropItem)
{
    m_DropItems[dropItem->GetId()] = dropItem;

    //CnDbg("add dropitem %u\n", dropItem->GetId());
    AddSceneObject(dropItem);
}

 void Scene::DelDropItem(uint32 id)
{
    DelSceneObject(id);
    DropObjectMap::iterator it;
    it = m_DropItems.find(id);
    CnAssert(it != m_DropItems.end());
    m_DropItems.erase(it);

   //CnDbg("remove dropitem %u\n", id);
}

bool Scene::IsCanUseAI()
{
    return m_CanUseAI;
}

bool Scene::IsPlayerSee(SceneObject* pSceneObject)
{
    return m_pNotifier->IsPlayerSee(pSceneObject);
}

bool Scene::IsPlayerCanSee(Player* player, SceneObject* pSceneObject)
{
    return m_pNotifier->IsPlayerCanSee(player, pSceneObject);
}

void Scene::AddBattleUnit(BattleUnit* pBattleUnit)
{
    pBattleUnit->onEnterScene(this);

    if(IsSceneObjectNotify(pBattleUnit))
        m_pNotifier->OnSceneObjectEnter(pBattleUnit);           // 通知

    m_pDungeon->OnCreatureEnter(pBattleUnit);
    m_pRegular->OnCreatureEnter(pBattleUnit);

    if(m_BattleUnits.find(pBattleUnit->GetId())!=m_BattleUnits.end())
    {
        CnAssert(false);
        CnDbg("m_InUnitUpdate=%d type=%d id=%d\n", (m_InUnitUpdate?1:0), pBattleUnit->GetObjectType(), pBattleUnit->GetId());
    }

    if(m_InUnitUpdate)
    {
        m_TempObjectList.push_back(pBattleUnit);
    }
    else
    {
        m_BattleUnits[pBattleUnit->GetId()] = pBattleUnit;
    }
}

void Scene::DelBattleUnit(uint32 id)
{
    CnAssert(!m_InUnitUpdate);

    BattleUnitMap::iterator cit = m_BattleUnits.find(id);
    if(cit == m_BattleUnits.end())
    {
        CnAssert(false);
        return;
    }

    BattleUnit* pBattleUnit = cit->second;
    m_BattleUnits.erase(cit);

    m_pRegular->OnCreatureLeave(pBattleUnit);
    m_pDungeon->OnCreatureLeave(pBattleUnit);
    if(IsSceneObjectNotify(pBattleUnit))
        m_pNotifier->OnSceneObjectLeave(pBattleUnit);       // 通知

    pBattleUnit->onLeaveScene(this);
}

void Scene::AddSceneObject(SceneObject* pSceneObject)
{
    CnAssert(m_SceneObjects.find(pSceneObject->GetId()) == m_SceneObjects.end());

    if(IsSceneObjectNotify(pSceneObject))
        m_pNotifier->OnSceneObjectEnter(pSceneObject);      // 通知

    pSceneObject->onEnterScene(this);

    if(m_InUnitUpdate)
    {
        m_TempObjectList.push_back(pSceneObject);
    }
    else
    {
        m_SceneObjects[pSceneObject->GetId()] = pSceneObject;
    }
}

void Scene::DelSceneObject(uint32 id)
{
    CnAssert(!m_InUnitUpdate);

    SceneObject* pSceneObject = NULL;
    SceneObjectMap::iterator iter;
    iter = m_SceneObjects.find(id);
    CnAssert(iter != m_SceneObjects.end());
    pSceneObject = iter->second;
    m_SceneObjects.erase(iter);

    pSceneObject->onLeaveScene(this);
    
    if(IsSceneObjectNotify(pSceneObject))
        m_pNotifier->OnSceneObjectLeave(pSceneObject);      // 通知
}

void Scene::NotifyMove(uint32 objectID, int32 angle, const CoVec2& position, const CoVec2& velocity , bool isMoving)
{
    m_pNotifier->NotifyMove(objectID, angle, position, velocity, isMoving);
}

void Scene::NotifyMessageToAll(uint32 msgId, void* msg)
{
    m_pNotifier->NotifyMessageAll(msgId, msg);
}

void Scene::NotifyMessage(SceneObject* sender, uint32 msgId, void* msg, bool exclude_sender)
{
    m_pNotifier->NotifyMessage(sender, msgId, msg, exclude_sender);     // 通知
}

BattleUnit* Scene::GetBattleUnit(uint32 objectID)
{
    if(m_BattleUnits.find(objectID) != m_BattleUnits.end())
        return m_BattleUnits[objectID];
    else
        return NULL;
}

Player* Scene::GetPlayerByPlayerId(uint64 playerID)
{
    for(PlayerMap::iterator playerIt = m_Players.begin(); playerIt != m_Players.end(); ++playerIt)
    {
        if(playerIt->second->GetPlayerID() == playerID)
        {
            return playerIt->second;
        }
    }
    return NULL;
}

Player* Scene::GetPlayerByPlayerName(const std::string& playerName)
{
    for(PlayerMap::iterator playerIt = m_Players.begin(); playerIt != m_Players.end(); ++playerIt)
    {
        if(playerIt->second->GetName() == playerName)
        {
            return playerIt->second;
        }
    }
    return NULL;
}

SceneObject* Scene::GetSceneObject(uint32 objectID)
{
    if(m_BattleUnits.find(objectID) != m_BattleUnits.end())
        return m_BattleUnits[objectID];
    else if(m_SceneObjects.find(objectID) != m_SceneObjects.end())
        return m_SceneObjects[objectID];
    else
        return NULL;
}

Trigger* Scene::GetTriggerByID(uint32 triggerID)
{
    for(TriggerMap::iterator triggerIt=m_Triggers.begin(); triggerIt != m_Triggers.end(); ++triggerIt)
    {
        if (triggerIt->second->GetTriggerId() == triggerID)
            return (Trigger*)triggerIt->second;
    }
    return NULL;
}

Trigger* Scene::GetTriggerByName(const std::string &name) const
{
    TriggerMap::const_iterator ite, iteEnd = m_Triggers.end();
    for (ite = m_Triggers.begin(); ite != iteEnd; ++ite)
    {
        if (ite->second->GetName() == name)
        {
            return ite->second;
        }
    }
    return NULL;
}

Trigger* Scene::GetFirstTrigger()
{
    int32 triggerId = 0;
    Trigger* pFirstTrigger = NULL;

    for(TriggerMap::iterator triggerIt=m_Triggers.begin(); triggerIt != m_Triggers.end(); ++triggerIt)
    {
        Trigger* pTrigger = triggerIt->second;
        if(triggerId == 0 || (int32)pTrigger->GetTriggerId() < triggerId)
        {
            triggerId = pTrigger->GetTriggerId();
            pFirstTrigger = pTrigger;
        }
    }
    return pFirstTrigger;
}

Checkpoint* Scene::GetCheckPoint(uint32 checkpointID)
{
    for(CheckPointMap::iterator checkpointIt=m_Checkpoints.begin(); checkpointIt != m_Checkpoints.end(); ++checkpointIt)
    {
        if (checkpointIt->second->GetCheckpointId() == checkpointID)
            return (Checkpoint*)checkpointIt->second;
    }
    return NULL;
}

Spawn* Scene::GetSpawn(uint32 spawnID)
{
    for (SpawnMap::iterator spawnIt = m_Spawns.begin(); spawnIt != m_Spawns.end(); ++spawnIt)
    {
        if (spawnIt->second->GetSpawnId() == spawnID)
            return (Spawn*)spawnIt->second;
    }
    return NULL;
}

Touch* Scene::GetTouch(uint32 touchID)
{
    for(TouchMap::iterator touchIt = m_Touchs.begin(); touchIt != m_Touchs.end(); ++touchIt)
    {
        if(touchIt->second->GetTouchID() == touchID)
        {
            return touchIt->second;
        }
    }
    return NULL;
}

DropObject* Scene::GetDropItem(uint32 objectID)
{
    //for(DropObjectMap::iterator it = m_DropItems.begin(); it != m_DropItems.end(); ++it)
    //{
    //    if(it->second->GetDropId() == dropItemID)
    //    {
    //        return it->second;
    //    }
    //}
   DropObjectMap::iterator it = m_DropItems.find(objectID);
   if(it != m_DropItems.end())
       return it->second;
   
    return NULL;
}

Teleport* Scene::GetTeleport(uint32 triggerID)
{
    for(TeleportList::iterator it = m_TeleportList.begin(); it != m_TeleportList.end(); ++it)
    {
        if(it->m_sourceTriggerID == triggerID)
        {
            return &(*it);
        }
    }
    return 0;
}

Player* Scene::FindPlayer(uint32 objectId)
{
    PlayerMap::iterator iter = m_Players.find(objectId);
    if(iter == m_Players.end())
        return NULL;
    else
        return iter->second;
}

bool Scene::TestCircleWithTriangles(const CoVec2& center, int32 radius)
{
    CoWorld* pCoWorld = GetCoWorld();
    return pCoWorld->TestCircleWithTriangles(center, (real)radius);
}

bool Scene::TestCircleWithCell(const CoVec2& center, int32 radius)
{
    CoWorld* pCoWorld = GetCoWorld();
    return pCoWorld->TestCircleWithCell(center, (real)radius);
}

bool Scene::TestCircleWithBound(const CoVec2& center, int32 radius)
{
    CoWorld* pCoWorld = GetCoWorld();
    real minX = pCoWorld->GetMinX();
    real minY = pCoWorld->GetMinY();
    real maxX = pCoWorld->GetMaxX();
    real maxY = pCoWorld->GetMaxY();

    if( minX > (center.x+radius) || maxX < (center.x-radius) )
        return true;
    if( minY > (center.y+radius) || maxY < (center.y-radius) )
        return true;
    return false;
}

int32 Scene::FindBattleUnitInRect( const CoVec2& center, int32 length, int32 width, int32 angle, BattleUnit** outFind, int32 outLen )
{
    CoWorld* pCoWorld = GetCoWorld();
    CoDynamicObject* findObjs[50];
    int32 count = pCoWorld->FindObjectsWithRect(center, (real)length, (real)width, ANGLE_TO_RADIAN(angle), findObjs, 50);

    int32 findCount = 0;

    for(int32 i=0; i<count; i++)
    {
        SceneObject* pSceneObject = (SceneObject*)findObjs[i]->GetOwner();

        if(!pSceneObject->IsKindOf(&BattleUnit::ms_RTTI))
            continue;

        if(findCount < outLen)
        {
            outFind[findCount] = (BattleUnit*)pSceneObject;
            findCount++;
        }
        else
            return findCount;
    }

    for(int32 i=findCount; i<outLen; i++)
        outFind[i] = NULL;

    return findCount;
}

int32 Scene::FindBattleUnitInFan( const CoVec2& center, int32 radius, int32 angleCenter, int32 angleSeperation, BattleUnit** outFind, int32 outLen )
{
    CoWorld* pCoWorld = GetCoWorld();
    CoDynamicObject* findObjs[50];
    int32 count = pCoWorld->FindObjectsWithFan(center, (real)radius, ANGLE_TO_RADIAN(angleCenter), ANGLE_TO_RADIAN(angleSeperation), findObjs, 50);

    int32 findCount = 0;

    for(int32 i=0; i<count; i++)
    {
        SceneObject* pSceneObject = (SceneObject*)findObjs[i]->GetOwner();

        if(!pSceneObject->IsKindOf(&BattleUnit::ms_RTTI))
            continue;

        if(findCount < outLen)
        {
            outFind[findCount] = (BattleUnit*)pSceneObject;
            findCount++;
        }
        else
            return findCount;
    }

    for(int32 i=findCount; i<outLen; i++)
        outFind[i] = NULL;

    return findCount;
}

int32 Scene::FindBattleUnitInCircle(const CoVec2& center, int32 radius,  BattleUnit** outFind, int32 outLen, uint32 exclude_id)
{
    CoWorld* pCoWorld = GetCoWorld();
    CoDynamicObject* findObjs[50];
    int32 count = pCoWorld->FindObjectsWithCircle(center, (real)radius, findObjs, 50);

    int32 findCount = 0;

    for(int32 i=0; i<count; i++)
    {
        SceneObject* pSceneObject = (SceneObject*)findObjs[i]->GetOwner();
        if(pSceneObject->GetId() == exclude_id)
            continue;
        
        if(!pSceneObject->IsKindOf(&BattleUnit::ms_RTTI))
            continue;
        
        if(findCount < outLen)
        {
            outFind[findCount] = (BattleUnit*)pSceneObject;
            findCount++;
        }
        else
            return findCount;
    }

    for(int32 i=findCount; i<outLen; i++)
        outFind[i] = NULL;

    return findCount;
}

int32 Scene::FindBattleUnitInCircle( const CoVec2& center, int32 radius, BattleUnitMap& outFind, uint32 exclude_id/*=0*/ )
{
    CoWorld* pCoWorld = GetCoWorld();
    CoDynamicObject* findObjs[50];
    int32 count = pCoWorld->FindObjectsWithCircle(center, (real)radius, findObjs, 50);

    for(int32 i=0; i<count; i++)
    {
        SceneObject* pSceneObject = (SceneObject*)findObjs[i]->GetOwner();
        if(pSceneObject->GetId() == exclude_id)
            continue;

        if(!pSceneObject->IsKindOf(&BattleUnit::ms_RTTI))
            continue;

        BattleUnit* pObject = NULL;
        pObject = (BattleUnit*) pSceneObject;
        outFind[pObject->GetId()] = pObject;
    }

    return outFind.size();
}

bool Scene::SortDistance(const BattleUnitDistance& a, const BattleUnitDistance& b)
{
    return a.m_distance < b.m_distance;
}

BattleUnit* Scene::FindNearestEnemyBattleUnitInCircle(BattleUnit* battleUnit, const CoVec2& center, int32 radius, uint32 exclude_id)
{
    if(!battleUnit)
        return NULL;

    BUDistanceList distanceList;
    distanceList.clear();
    BattleUnitDistance BUDistance;
    BUDistance.Clear();
    BattleUnitMap BUMap;
    BUMap.clear();
    int32 ret = FindBattleUnitInCircle(center, radius, BUMap, battleUnit->GetId());

    // 距离计算
    for(BattleUnitMap::iterator battleIt = BUMap.begin(); battleIt != BUMap.end(); ++battleIt)
    {
        BUDistance.Clear();

        BattleUnit* other = battleIt->second;

        if(other->GetId() == exclude_id)
            continue;

        CnAssert(battleUnit->GetId() != other->GetId());
        if(!battleUnit->CanAttack(other))
            continue;

        BUDistance.m_battleUnit = other;

        CoVec2 p;
        p.x = real(center.x - other->GetX());
        p.y = real(center.x - other->GetY());
        real dis = p.Length();

        BUDistance.m_distance = dis;
        distanceList.push_back(BUDistance);
    }
    sort(distanceList.begin(), distanceList.end(), SortDistance);

    BUDistanceList::iterator distaneIt = distanceList.begin();
    if(distaneIt == distanceList.end())
    {
        return NULL;
    }

    return distaneIt->m_battleUnit;
}

CoVec2 Scene::FindValidPositionInCircle(const CoVec2& center, int32 radius, int32 collider_radius)
{
    return FindValidPositionInRing(center, radius, 0, collider_radius);
}

CoVec2 Scene::FindValidPositionInRing(const CoVec2& center, int32 max_radius, int32 min_radius, int32 collider_radius)
{
    CoWorld* pCoWorld = GetCoWorld();

    int32 tryCount = 10;     // 最多尝试n次
    bool isFind = false;

    CoVec2 tryPos;

    while(tryCount > 0 && !isFind)
    {
        isFind = true;
        tryCount--;

        int32 r = Crown::SDRandom(max_radius - min_radius) + min_radius;
        real a = Crown::SDRandomFloat((real)M_PI*2);

        tryPos.x = center.x + sin(a)*r;
        tryPos.y = center.y + cos(a)*r;

        if(pCoWorld->TestCircleWithTriangles(tryPos, (real)collider_radius))
        {
            isFind = false;
            continue;
        }

        if(pCoWorld->TestCircleWithObjects(tryPos, (real)collider_radius, NULL, 0))
        {
            isFind = false;
            continue;
        }
    }

    if(isFind)
        return tryPos;
    else
        return center;
}

CoVec2 Scene::FindValidDropItemPostion(const CoVec2& center, int32 radius)
{
    uint32 cellLength = m_pGridWorld->GetCellLength();
    
    Crown::SDPoint emptyPos[100];
    int32 tryCount = 10;     // 最多尝试n次
    bool isFind = false;
    CoVec2 tryPos;

    while(tryCount > 0 && !isFind)
    {
        tryCount--;
        isFind = true;

        // 计算网格范围
        real minX = center.x - radius;
        real maxX = center.x + radius;
        real minY = center.y - radius;
        real maxY = center.y + radius;

        radius += cellLength;       // 下次扩大掉落范围

        int32 cellMinX = RealPosToCellPos(minX, cellLength);
        int32 cellMaxX = RealPosToCellPos(maxX, cellLength);
        int32 cellMinY = RealPosToCellPos(minY, cellLength);
        int32 cellMaxY = RealPosToCellPos(maxY, cellLength);

        // 搜索空位置
        uint32 emptyNum = m_pGridWorld->FindEmptyPosInRect(cellMinX, cellMinY, cellMaxX, cellMaxY, emptyPos, 100);
        emptyNum = Crown::SDMin(emptyNum, (uint32)100);
        if(!emptyNum)
        {
            isFind = false;
            continue;
        }

        while(emptyNum > 0)
        {
            // 随机一个空位置
            uint32 randIndex = Crown::SDRandom(emptyNum);
            Crown::SDPoint& findPos = emptyPos[randIndex];

            // 计算真实坐标
            tryPos.x = CellPosToRealPos(findPos.x, cellLength);
            tryPos.y = CellPosToRealPos(findPos.y, cellLength);
            
            // 碰撞范围
            if(!m_pCoWorld->TestCircleWithTriangles(tryPos, 1))
            {
                break;
            }
            
            // 此空位置不可用,去掉
            for(uint32 i=randIndex; i<emptyNum-1; i++)
            {
                emptyPos[i] = emptyPos[i+1];
            }
            emptyNum--;
        }

        if(emptyNum == 0)
        {
            isFind = false;
            continue;
        }
    }

    if(isFind)
        return tryPos;
    else
        return center;
}

bool Scene::IsEnableAoi()
{
    uint32 type = m_pDungeon->GetType();

    switch (type)
    {
    case DUNGEON_TYPE_CITY:
    case DUNGEON_TYPE_WORLD:
        return true;
    default:
        return false;
    }

    return false;
}

bool Scene::IsSceneObjectNotify(SceneObject* pObject)
{
    uint32 objType = pObject->GetObjectType();
    switch(objType)
    {   // 不需要通知给客户端的对象
        case OBJ_SPAWN:
        case OBJ_CHECKPOINT:
            return false;
    }
    return true;
}

bool Scene::FindPath( const CoVec2& start, const CoVec2& end, std::vector<CoVec2>& pPath)
{
    return m_pNavMesh->FindPath(start, end, pPath);
}

void Scene::BeginContact(b2Contact* contact)
{
    //SceneObject* objectA = (SceneObject*)contact->GetFixtureA()->GetBody()->GetUserData();
    //SceneObject* objectB = (SceneObject*)contact->GetFixtureB()->GetBody()->GetUserData();
}

void Scene::EndContact(b2Contact* contact)
{
    //SceneObject* objectA = (SceneObject*)contact->GetFixtureA()->GetBody()->GetUserData();
    //SceneObject* objectB = (SceneObject*)contact->GetFixtureB()->GetBody()->GetUserData();
}
