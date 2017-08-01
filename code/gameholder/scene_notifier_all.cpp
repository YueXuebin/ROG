#include "gameholder_pch.h"
#include "scene_notifier_all.h"
#include "sceneobject.h"
#include "battle_unit.h"
#include "player.h"
#include "scene.h"
#include "trigger.h"
#include "notifier_utils.h"

// 通知器
SceneNotifierAll::SceneNotifierAll(Scene* pScene) :
SceneNotifier(pScene)
{

}

SceneNotifierAll::~SceneNotifierAll()
{

}

void SceneNotifierAll::Update()
{
    // 移动消息通知给所有人
    if(m_creatureMoveList.list.size() > 0)
    {
        NotifyMessageAll(MSG_CREATURE_MOVE_LIST, &m_creatureMoveList);
        m_creatureMoveList.Clear();
    }
}

void SceneNotifierAll::OnSceneObjectEnter(SceneObject* pSceneObject)
{
    if(pSceneObject->IsKindOf(&BattleUnit::ms_RTTI))
        NotifyAppear((BattleUnit*)pSceneObject);
    else
        NotifySceneObjectAppear(pSceneObject);
}

void SceneNotifierAll::OnSceneObjectLeave(SceneObject* pSceneObject)
{
    NotifyDisappear(pSceneObject);
}

void SceneNotifierAll::NotifyMessageAll(uint32 msgId, void* msg)
{
    CnAssert(m_pScene);
    for(Scene::PlayerMap::iterator it=m_pScene->GetPlayers().begin(); it != m_pScene->GetPlayers().end(); ++it)
    {
        it->second->SendToGate(msgId, msg);
    }
}

void SceneNotifierAll::NotifyMessage(SceneObject* sender, uint32 msgId, void* msg, bool exclude_sender)
{
    CnAssert(m_pScene);
    for(Scene::PlayerMap::iterator it=m_pScene->GetPlayers().begin(); it != m_pScene->GetPlayers().end(); ++it)
    {
        if(exclude_sender && it->second == sender)
            continue;   // 排除自己

        it->second->SendToGate(msgId, msg);
    }
}

void SceneNotifierAll::NotifyMove(uint32 objectID, int32 angle, const CoVec2& position, const CoVec2& velocity , bool isMoving)
{
    CREATURE_MOVE move;

    move.object_id = objectID;
    move.angle = angle;
    move.pos_x = (int32)position.x;
    move.pos_y = (int32)position.y;
    move.vel_x = (int32)velocity.x;
    move.vel_y = (int32)velocity.y;
    move.is_moving = isMoving;

    m_creatureMoveList.list.push_back(move);
}

void SceneNotifierAll::NotifyAppear( BattleUnit* pBattleUnit )
{
    NotifyCreatureAppear(pBattleUnit);

    if(pBattleUnit->IsKindOf(&Player::ms_RTTI))
    {
        NotifyAroundCreatureAppear((Player*)pBattleUnit);
        NotifyAroundSceneObjectAppear((Player*)pBattleUnit);
    }
}

// 通知该玩家，周围的生物对象
void SceneNotifierAll::NotifyAroundCreatureAppear(Player* player)
{
    BATTLEUNIT_APPEAR_LIST sendMsg;

    for(Scene::BattleUnitMap::iterator it=m_pScene->GetBattleUnits().begin(); it != m_pScene->GetBattleUnits().end(); ++it)
    {
        BattleUnit* pBattleUnit = it->second;
        if(pBattleUnit == player)
            continue;

        FILL_MSG_APPEAR_LIST(sendMsg, pBattleUnit, player);
    }

    player->SendToGate(MSG_BATTLEUNIT_APPEAR_LIST, &sendMsg);
}
// 通知该玩家,周围的逻辑对象
void SceneNotifierAll::NotifyAroundSceneObjectAppear(Player* player)
{
    SCENEOBJECT_APPEAR_LIST sendMsg;

    for(Scene::SceneObjectMap::iterator it=m_pScene->GetObjects().begin(); it != m_pScene->GetObjects().end(); ++it)
    {
        SceneObject* pObject = it->second;
        CnAssert(!pObject->IsKindOf(&BattleUnit::ms_RTTI));

        if(m_pScene->IsSceneObjectNotify(pObject))
            FILL_MSG_SCENEOBJECT_APPEAR_LIST(sendMsg, pObject, player);
    }

    player->SendToGate(MSG_SCENEOBJECT_APPEAR_LIST, &sendMsg);
}

bool SceneNotifierAll::IsPlayerSee(SceneObject* pSceneObject)
{   
    return m_pScene->GetPlayerCount() > 0;      // 只要有玩家在，就有玩家看见该对象
}

bool SceneNotifierAll::IsPlayerCanSee(Player* player, SceneObject* pSceneObject)
{
    return true;
}
