#include "gameholder_pch.h"
#include "message_encoder.h"
#include "scene_notifier_aoi.h"
#include "sight_manager.h"
#include "player.h"
#include "trigger.h"
#include "scene.h"
#include "aoi_object.h"
#include "notifier_utils.h"
#include "team_relationship.h"


SceneNotifierAOI::SceneNotifierAOI(Scene* pScene) :
SceneNotifier(pScene)
{
    m_pAOIManager = CnNew AOIManager(pScene, this);
}

SceneNotifierAOI::~SceneNotifierAOI()
{
    CnDelete m_pAOIManager;
}

void SceneNotifierAOI::OnSceneObjectEnter(SceneObject* pSceneObject)
{
    if(!pSceneObject->GetCoObject())
        return;     // 必须是有位置属性的对象

    if (IsAOIObject(pSceneObject))
    {
        m_pAOIManager->Enter(pSceneObject);
        if (pSceneObject->IsKindOf(&Player::ms_RTTI))
            NotifyAllNoAOIObject((Player*)pSceneObject);
    }
    else
    {
        if(pSceneObject->IsKindOf(&BattleUnit::ms_RTTI))
            NotifyCreatureAppear((BattleUnit*)pSceneObject);
        else
            NotifySceneObjectAppear(pSceneObject);

        m_NoAOIObjects.insert(std::make_pair(pSceneObject->GetId(), pSceneObject));
    }
}

void SceneNotifierAOI::OnSceneObjectLeave(SceneObject* pSceneObject)
{
    if(!pSceneObject->GetCoObject())
        return;     // 必须是有位置属性的对象

    if (IsAOIObject(pSceneObject))
    {
        m_pAOIManager->Leave(pSceneObject);
    }
    else
    {
        NotifyDisappear(pSceneObject);
        m_NoAOIObjects.erase(pSceneObject->GetId());
    }
}

void SceneNotifierAOI::Update()
{
    m_pAOIManager->Update();
}

void SceneNotifierAOI::NotifyMessageAll(uint32 msgId, void* msg)
{
    CnAssert(m_pScene);
    for(Scene::PlayerMap::iterator it=m_pScene->GetPlayers().begin(); it != m_pScene->GetPlayers().end(); ++it)
    {
        it->second->SendToGate(msgId, msg);
    }
}

void SceneNotifierAOI::NotifyMessage(SceneObject* sender, uint32 msgId, void* msg, bool exclude_sender)
{
    CnAssert(m_pScene);
    if(!IsAOIObject(sender))
    {   // 通知所有玩家
        for(Scene::PlayerMap::iterator it=m_pScene->GetPlayers().begin(); it != m_pScene->GetPlayers().end(); ++it)
        {
            if(exclude_sender && it->second == sender)
                continue;   // 排除自己

            it->second->SendToGate(msgId, msg);
        }
    }
    else
    {
        // 通知能看见sender所在单元格的玩家
        m_pAOIManager->NotifyMessage(sender, msgId, msg, exclude_sender);

        // sender是玩家,则通知队友
        if(sender->GetObjectType() == OBJ_PLAYER)
        {
            Player* senderPlayer = (Player*)sender;
            TeamRelationShip::Instance()->SendMessageToTeamMemberInSameScene(senderPlayer, msgId, msg, true);
        }
    }
}

void SceneNotifierAOI::NotifyMove(uint32 objectID, int32 angle, const CoVec2& position, const CoVec2& velocity , bool isMoving)
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

    SceneObject* sender = m_pScene->GetSceneObject(objectID);
    CnAssert(sender);
    NotifyMessage(sender, MSG_CREATURE_MOVE_LIST, &m_creatureMoveList);

    m_creatureMoveList.Clear();
}
// 是AOIObject就做范围通知
bool SceneNotifierAOI::IsAOIObject(SceneObject* pSceneObject)
{
    if(pSceneObject->IsAppearNotifierAll())
        return false;

    uint32 objType = pSceneObject->GetObjectType();
    switch (objType)
    {
    // 要做范围通知的生物
    case OBJ_PLAYER:
    case OBJ_MONSTER:
    case OBJ_PET:
    case OBJ_OFFLINE_PLAYER:
        return true;
    // 不做范围通知的生物
    //case OBJ_NPC:
    case OBJ_BOSS:
        return false;
    // 要做范围通知的场景对象
    case OBJ_TOUCH:
    case OBJ_DROP_OBJECT:
        return true;
    // 不做范围通知的场景对象
    case OBJ_TRIGGER:
    case OBJ_SPAWN:
    case OBJ_CHECKPOINT:
        return false;
    default:
        CnAssert(false);
        return false;
    }

    return false;
}

void SceneNotifierAOI::NotifyAllNoAOIObject(Player* player)
{
    BATTLEUNIT_APPEAR_LIST sendBattleUnitMsg;
    SCENEOBJECT_APPEAR_LIST sendSceneObjectMsg;
    uint32 count1 = 0;
    uint32 count2 = 0;

    std::map<uint32, SceneObject*>::iterator iter = m_NoAOIObjects.begin();
    for (; iter != m_NoAOIObjects.end(); ++iter)
    {
        SceneObject *pSceneObject = iter->second;
        if (pSceneObject->IsKindOf(&BattleUnit::ms_RTTI))
        {
            if (player == pSceneObject)
                continue;

            FILL_MSG_APPEAR_LIST(sendBattleUnitMsg, (BattleUnit*)pSceneObject, player);
            count1++;
        }
        else
        {
            FILL_MSG_SCENEOBJECT_APPEAR_LIST(sendSceneObjectMsg, pSceneObject, player);
            count2++;
        }
    }
        
    if(count1>0)
        player->SendToGate(MSG_BATTLEUNIT_APPEAR_LIST, &sendBattleUnitMsg);
    if(count2>0)
        player->SendToGate(MSG_SCENEOBJECT_APPEAR_LIST, &sendSceneObjectMsg);
}

bool SceneNotifierAOI::IsPlayerSee(SceneObject* pSceneObject)
{
    if(IsAOIObject(pSceneObject))
    {
        return m_pAOIManager->IsPlayerSee(pSceneObject);
    }
    else
    {
        return m_pScene->GetPlayerCount() > 0;
    }
}

bool SceneNotifierAOI::IsPlayerCanSee(Player* player, SceneObject* pSceneObject)
{
    if(IsAOIObject(pSceneObject))
    {
        return m_pAOIManager->IsPlayerCanSee(player, pSceneObject);
    }
    else
    {
        return true;
    }
}

