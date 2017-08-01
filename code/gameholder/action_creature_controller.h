#ifndef action_creature_controller_h__
#define action_creature_controller_h__

#include "action_controller.h"

class Creature;
class PathMover;
class PushMover;
class SceneObject;

/*
*	生物对象的行为控制器
*/
class ActionCreatureController : public ActionController
{
public:
    ActionCreatureController(Creature* creature);
    virtual ~ActionCreatureController();

    virtual void Update();

    virtual void Stop();
    virtual bool JumpTo(int32 x, int32 y);      // 跳至某处
    virtual bool PushTo(int32 x, int32 y, int32 time, bool isPassive); // isPassive是否被动推移(主动推移不发送给自己,客户端自己处理了)
    virtual void WalkTo(int32 x, int32 y);      // 走至某处
    virtual void RunTo(int32 x, int32 y);       // 跑至某处
    virtual bool AngleTo(int32 angle);          // 面向某方向
    virtual bool UseSkill(uint32 skillId, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skillObjID, uint32 skillSlot);
    virtual bool Bounce(int32 height, int32 time);      // 弹跳
    virtual bool Touch();                               // 采集

    virtual void OnStateChange(uint32 state);

    const std::vector<CoVec2>&   GetPath();

protected:
    void    SendMoveUpdate();

    virtual void OnMoveEnd(uint32 moveType);

    void    UpdatePush();

private:
    void    UpdateMove();

    void    AngleToObject(SceneObject* pObject);

protected:
    Creature*   m_Creature;

    // 移动用
    bool    m_isMoving;             // 生物是否正在移动(客户端预测)
    // 推移用
    bool    m_IsPushing;            // 是否正在推移中

    // 移动通知用
    CoVec2  m_SendPosition;
    CoVec2  m_SendVelocity;
    int32   m_SendAngle;
    int32   m_SendTimer;

    PathMover* m_PathMover;
    PushMover* m_PushMover;
};


#endif // action_creature_controller_h__
