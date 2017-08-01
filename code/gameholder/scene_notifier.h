#ifndef scene_notifier_h__
#define scene_notifier_h__

class SceneObject;
class BattleUnit;
class Player;
class Scene;

/*
 *	场景通知器接口
 */
class SceneNotifier
{
public:
    SceneNotifier(Scene* pScene);
    virtual ~SceneNotifier();

    // 场景对象进入
    virtual void OnSceneObjectEnter(SceneObject* pSceneObject) = 0;
    // 场景对象离开
    virtual void OnSceneObjectLeave(SceneObject* pSceneObject) = 0;
    // 发送消息给所有人
    virtual void NotifyMessageAll(uint32 msgId, void* msg) = 0;
    // 发送消息给指定范围
    virtual void NotifyMessage(SceneObject* sender, uint32 msgId, void* msg, bool exclude_sender) = 0;
    // 移动通知
    virtual void NotifyMove(uint32 objectID, int32 angle, const CoVec2& position, const CoVec2& velocity , bool isMoving) = 0;
    // update
    virtual void Update() = 0;

    // 是否有玩家看见该对象
    virtual bool IsPlayerSee(SceneObject* pSceneObject) = 0;
    // 某玩家是否可以看见某对象
    virtual bool IsPlayerCanSee(Player* player, SceneObject* pSceneObject) = 0;

    Scene* GetScene() { return m_pScene; }

protected:
    void NotifySceneObjectAppear(SceneObject* pSceneObject);
    void NotifyCreatureAppear(BattleUnit* pBattleUnit);
    // disappear
    void NotifyDisappear(SceneObject* pSceneObject);

protected:
    Scene*  m_pScene;

};

SceneNotifier*  CreateNotifier(Scene* pScene, bool isAOINotifer = false);

#endif // scene_notifier_h__
