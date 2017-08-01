#include "gameholder_pch.h"
#include "scene_notifier.h"
#include "scene_notifier_all.h"
#include "scene_notifier_aoi.h"
#include "notifier_utils.h"
#include "player.h"
#include "scene.h"

SceneNotifier* CreateNotifier(Scene* pScene, bool isAOINotifer )
{
    if(isAOINotifer)
        return CnNew SceneNotifierAOI(pScene);
    else
        return CnNew SceneNotifierAll(pScene);
}

SceneNotifier::SceneNotifier(Scene* pScene) :
m_pScene(pScene)
{

}

SceneNotifier::~SceneNotifier()
{

}

// 把该逻辑对象，通知给所有其他玩家
void SceneNotifier::NotifySceneObjectAppear( SceneObject* pSceneObject )
{
    if(!m_pScene->GetPlayers().size()) 
        return;

    for(Scene::PlayerMap::iterator it = m_pScene->GetPlayers().begin(); it != m_pScene->GetPlayers().end(); ++it)
    {
        Player* SeePlayer = it->second;
        SCENEOBJECT_APPEAR_LIST sendMsg;
        FILL_MSG_SCENEOBJECT_APPEAR_LIST(sendMsg, pSceneObject, SeePlayer);
        it->second->SendToGate(MSG_SCENEOBJECT_APPEAR_LIST, &sendMsg);
    }
}

// 把该生物对象，通知给所有其他玩家
void SceneNotifier::NotifyCreatureAppear(BattleUnit* pBattleUnit)
{
    if(!m_pScene->GetPlayers().size()) 
        return;

    BATTLEUNIT_APPEAR_LIST sendMsg;

    FILL_MSG_APPEAR_LIST(sendMsg, pBattleUnit, NULL);

    for(Scene::PlayerMap::iterator it=m_pScene->GetPlayers().begin(); it != m_pScene->GetPlayers().end(); ++it)
    {
        if(!it->second || it->second == pBattleUnit)
            continue;   // 排除自己

        it->second->SendToGate(MSG_BATTLEUNIT_APPEAR_LIST, &sendMsg);
    }
}

void SceneNotifier::NotifyDisappear(SceneObject* pSceneObject)
{
    uint32 id = pSceneObject->GetId();
    if(pSceneObject->IsKindOf(&BattleUnit::ms_RTTI))
    {
        BATTLEUNIT_DISAPPEAR sendMsg1;
        sendMsg1.object_id = id;

        for(Scene::PlayerMap::iterator it = m_pScene->GetPlayers().begin(); it != m_pScene->GetPlayers().end(); ++it)
        {
            if(it->second->GetId() == id)    // 排除自己
                continue;
            it->second->SendToGate(MSG_BATTLEUNIT_DISAPPEAR, &sendMsg1);
        }
    }
    else
    {
        SCENEOBJECT_DISAPPEAR sendMsg2;
        sendMsg2.object_id = id;
        sendMsg2.object_type = pSceneObject->GetObjectType();

        for(Scene::PlayerMap::iterator it = m_pScene->GetPlayers().begin(); it != m_pScene->GetPlayers().end(); ++it)
        {
            if(it->second->GetId() == id)    // 排除自己
                continue;

            it->second->SendToGate(MSG_SCENEOBJECT_DISAPPEAR, &sendMsg2);
        }
    }
}

