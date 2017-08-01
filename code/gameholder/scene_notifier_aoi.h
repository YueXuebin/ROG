/*
 *	场景通知器，根据各角色视野范围通知
 */
#ifndef scene_notifier_ex_h__
#define scene_notifier_ex_h__

#include "scene_notifier.h"
#include "aoi_manager.h"

class Player;
class AOIManager;


class SceneNotifierAOI : public SceneNotifier
{
public:
    SceneNotifierAOI(Scene* pScene);
    virtual ~SceneNotifierAOI();
    
    // 场景对象进入
    virtual void OnSceneObjectEnter(SceneObject* pSceneObject);
    // 场景对象离开
    virtual void OnSceneObjectLeave(SceneObject* pSceneObject);
    // 发消息给所有人
    virtual void NotifyMessageAll(uint32 msgId, void* msg);
    // 发消息给指定范围
    virtual void NotifyMessage(SceneObject* sender, uint32 msgId, void* msg, bool exclude_sender = false);
    // 移动通知
    virtual void NotifyMove(uint32 objectID, int32 angle, const CoVec2& position, const CoVec2& velocity , bool isMoving);

    virtual void Update();

    virtual bool IsPlayerSee(SceneObject* pSceneObject);

    virtual bool IsPlayerCanSee(Player* player, SceneObject* pSceneObject);

    AOIManager* GetAOIManager() {return m_pAOIManager;}

private:
    bool IsAOIObject(SceneObject* pSceneObject);
    void NotifyAllNoAOIObject(Player* player);

private:
    AOIManager*         m_pAOIManager;                  // Area Of Interest 管理器
    std::map<uint32, SceneObject*> m_NoAOIObjects;      // 不需要aoi管理的对象
    
    CREATURE_MOVE_LIST  m_creatureMoveList;             // 移动消息
};


#endif // scene_notifier_ex_h__
