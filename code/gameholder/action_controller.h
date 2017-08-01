#ifndef action_controller_h__
#define action_controller_h__

#include "skill_define.h"

class BattleUnit;
class Scene;

// 移动类型
enum
{
    MOVE_TYPE_NONE = 0,
    MOVE_TYPE_MOVE = 1,
    MOVE_TYPE_PUSH = 2,
};

/*
 *	行为控制器(基类)
 */
class ActionController
{
    friend class PathMover;
    friend class PushMover;
public:
    ActionController(BattleUnit* owner);
    virtual ~ActionController();

    virtual void Update() {}

    virtual void Stop() {return;}
    virtual bool JumpTo(int32 x, int32 y) {return false;}
    virtual bool PushTo(int32 x, int32 y, int32 time, bool isPassive) {return false;}
    virtual void WalkTo(int32 x, int32 y) {}
    virtual void RunTo(int32 x, int32 y) {}
    virtual bool AngleTo(int32 angle) {return false;}
    virtual bool UseSkill(uint32 skillId, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skillObjID, uint32 skillSlot) {return false;}
    virtual bool Bounce(int32 height, int32 time) {return false;}
    virtual bool Dance() {return false;}
    virtual bool Touch() {return false;}

    virtual void OnStateChange(uint32 state) {}

    virtual void OnEnterScene( Scene* pScene ) {}
    virtual void OnLeaveScene( Scene* pScene ) {}

protected:
    virtual void OnMoveEnd(uint32 moveType) {}             // 移动结束

private:


};

ActionController*   CreateActionController(BattleUnit* Owner);      // factory模式


#endif // action_controller_h__
