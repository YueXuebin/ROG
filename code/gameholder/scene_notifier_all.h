/*
 *	场景通知器,全场景通知
 */
#ifndef scene_notifier_all_h__
#define scene_notifier_all_h__

#include "scene_notifier.h"

class Scene;
class BattleUnit;
class Player;

class SceneNotifierAll : public SceneNotifier
{
public:
    SceneNotifierAll(Scene* pScene);
    virtual ~SceneNotifierAll();

    virtual void OnSceneObjectEnter(SceneObject* pSceneObject);
    virtual void OnSceneObjectLeave(SceneObject* pSceneObject);
    virtual void NotifyMessageAll(uint32 msgId, void* msg);
    virtual void NotifyMessage(SceneObject* sender, uint32 msgId, void* msg, bool exclude_sender = false);      // // 发消息给指定范围
    virtual void NotifyMove(uint32 objectID, int32 angle, const CoVec2& position, const CoVec2& velocity , bool isMoving);

    virtual void Update();

    virtual bool IsPlayerSee(SceneObject* pSceneObject);
    virtual bool IsPlayerCanSee(Player* player, SceneObject* pSceneObject);

private:
    void NotifyAppear(BattleUnit* pBattleUnit);
    void NotifyAroundCreatureAppear(Player* player);
    void NotifyAroundSceneObjectAppear(Player* player);


private:
    CREATURE_MOVE_LIST  m_creatureMoveList;
};


#endif // scene_notifier_all_h__
