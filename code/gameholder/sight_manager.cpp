//#include "gameholder_pch.h"
//#include "sight_manager.h"
//#include "sceneobject.h"
//#include "scene.h"
//#include "battle_unit.h"
//#include "player.h"
//#include "BoundingBoxInt.h"
//#include "scene_notifier_ex.h"
//
//SightManager::SightManager(Player *own)
//:m_pOwn(own),
//m_updateSightTime(0)
//{
//    m_BattleUnits = CnNew BattleUnitMap;
//    m_NewBattleUnits = CnNew BattleUnitMap;
//    m_OldBattleUnits = CnNew BattleUnitMap;
//    m_SceneObjects = CnNew SceneObjectMap;
//    m_NewSceneObjects = CnNew SceneObjectMap;
//    m_OldSceneObjects = CnNew SceneObjectMap;
//}
//
//SightManager::~SightManager()
//{
//    CnDelete m_BattleUnits;
//    CnDelete m_NewBattleUnits;
//    CnDelete m_OldBattleUnits;
//
//    CnDelete m_SceneObjects; 
//    CnDelete m_NewSceneObjects;
//    CnDelete m_OldSceneObjects;
//}
//
//void SightManager::Update()
//{
//    if (!m_pOwn->GetScene()->IsEnabledSight())
//        return;
//
//    m_updateSightTime += g_ServerUpdateTime;
//    if (m_updateSightTime >= UPDATE_SIGHT_INTERVAL)
//    {
//        InitSightBattleUnit();
//        InitSightSceneObject();
//
//        m_pOwn->GetScene()->GetNotifier()->NotifyAppear(m_pOwn);
//
//        m_updateSightTime = 0;
//    }
//}
//
//void SightManager::Init()
//{
//    if (!m_pOwn->GetScene()->IsEnabledSight())
//        return;
//
//    InitSightBattleUnit();
//    InitSightSceneObject();
//}
//
//void SightManager::AddPlayer(Player *player)
//{ 
//    m_Players.insert(std::make_pair(player->GetId(), player)); 
//}
//
//void SightManager::AddBattleUnit(BattleUnit *pBattleUnit)
//{
//    //检查是新发现的玩家还是上次视野中留下的
//    if (m_OldBattleUnits->find(pBattleUnit->GetId()) == m_OldBattleUnits->end()) 
//    {
//        m_NewBattleUnits->insert(std::make_pair(pBattleUnit->GetId(), pBattleUnit));
//        if (pBattleUnit->GetObjectType() == OBJ_MONSTER)
//            CnDbg("Find New Monster %u\n", pBattleUnit->GetId());
//    }
//
//    m_BattleUnits->insert(std::make_pair(pBattleUnit->GetId(), pBattleUnit));
//}
//
//void SightManager::AddSceneObject(SceneObject *pSceneObject)
//{
//    if (!pSceneObject->IsKindOf(&BattleUnit::ms_RTTI))
//    {
//        //检查是新发现的场景对象还是上次视野中留下的
//        if (m_OldSceneObjects->find(pSceneObject->GetId()) == m_OldSceneObjects->end())
//            m_NewSceneObjects->insert(std::make_pair(pSceneObject->GetId(), pSceneObject));
//
//        m_SceneObjects->insert(std::make_pair(pSceneObject->GetId(), pSceneObject));
//    }
//}
//
//void SightManager::InitSightBattleUnit()
//{
//    CnAssert(m_pOwn);
//    if (!m_pOwn)
//        return;
//
//    Scene *pScene = m_pOwn->GetScene();
//    CnAssert(pScene);
//    if (!pScene)
//        return;
//
//    BattleUnit* sightBattleUnits[SIGHT_BATTLE_UNIT_SIZE];
//    int32 findCount = pScene->FindBattleUnitInRect(m_pOwn->GetPosition(), SIGHT_RECT_LENGHT, SIGHT_RECT_WIDTH, 
//        m_pOwn->GetAngle(), sightBattleUnits, SIGHT_BATTLE_UNIT_SIZE);
//
//    m_NewBattleUnits->clear();
//    m_OldBattleUnits->clear();
//    std::swap(m_OldBattleUnits, m_BattleUnits);
//
//    m_Players.clear();
//
//    for (int32 i = 0; i < findCount; ++i)
//    {
//        BattleUnit *pBU = sightBattleUnits[i];
//        if (pBU->GetObjectType() == OBJ_PLAYER)
//        {
//            Player *p = static_cast<Player*>(pBU);
//            m_Players.insert(std::make_pair(pBU->GetId(), p));
//        }
//
//        //检查是新发现的玩家还是上次视野中留下的
//        if (m_OldBattleUnits->find(pBU->GetId()) == m_OldBattleUnits->end()) 
//            m_NewBattleUnits->insert(std::make_pair(pBU->GetId(), pBU));
//
//        m_BattleUnits->insert(std::make_pair(pBU->GetId(), pBU));
//    }
//
//    //计算离开视野的生物
//    for (BattleUnitMap::iterator iter = m_OldBattleUnits->begin(), next; iter != m_OldBattleUnits->end(); iter = next)
//    {
//        next = iter;
//        ++next;
//
//        uint32 id = iter->first;
//        if (m_BattleUnits->find(id) != m_BattleUnits->end())      //还在视野中的从old中移除
//            m_OldBattleUnits->erase(id);
//    }
//}
//
//void SightManager::InitSightSceneObject()
//{
//    CnAssert(m_pOwn);
//    if (!m_pOwn)
//        return;
//
//    Scene *pScene = m_pOwn->GetScene();
//    CnAssert(pScene);
//    if (!pScene)
//        return;
//
//    CoWorld *pCoWorld = pScene->GetCoWorld();
//    CnAssert(pCoWorld);
//    if (!pCoWorld)
//        return;
//
//    CoDynamicObject* objects[SIGHT_BATTLE_UNIT_SIZE];
//    int32 findCount = pCoWorld->FindObjectsWithRect(m_pOwn->GetPosition(), (real)SIGHT_RECT_LENGHT, 
//        (real)SIGHT_RECT_WIDTH, (real)m_pOwn->GetAngle(), objects, SIGHT_BATTLE_UNIT_SIZE);
//
//    m_NewSceneObjects->clear();
//    m_OldSceneObjects->clear();
//    std::swap(m_OldSceneObjects, m_SceneObjects);
//
//    for (int32 i = 0; i < findCount; ++i)
//    {
//        SceneObject *pSceneObj = static_cast<SceneObject*>(objects[i]->GetOwner());
//        if (!pSceneObj->IsKindOf(&BattleUnit::ms_RTTI))
//        {
//            //检查是新发现的场景对象还是上次视野中留下的
//            if (m_OldSceneObjects->find(pSceneObj->GetId()) == m_OldSceneObjects->end())
//                m_NewSceneObjects->insert(std::make_pair(pSceneObj->GetId(), pSceneObj));
//
//            m_SceneObjects->insert(std::make_pair(pSceneObj->GetId(), pSceneObj));
//        }
//    }
//
//    for (SceneObjectMap::iterator iter = m_OldSceneObjects->begin(), next; iter != m_OldSceneObjects->end(); iter = next)
//    {
//        next = iter;
//        ++next;
//
//        uint32 id = iter->first;
//        if (m_SceneObjects->find(id) != m_SceneObjects->end())        //还在视野中的从old中移除
//            m_OldSceneObjects->erase(id);
//    }
//}