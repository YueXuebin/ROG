#ifndef action_player_controller_h__
#define action_player_controller_h__

#include "action_creature_controller.h"

class Player;

/*
 *	玩家角色控制器
 */
class ActionPlayerController : public ActionCreatureController
{
public:
    ActionPlayerController(Player* player);
    virtual ~ActionPlayerController();

    virtual void Update();

    virtual bool JumpTo(int32 x, int32 y);      // 跳至某处

    virtual void RunTo(int32 x, int32 y);
    void StopAt(int32 x, int32 y);

    virtual bool UseSkill(uint32 skillId, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skillObjID, uint32 skillSlot);

    virtual bool Dance();

    const CoVec2& GetCheckPos() {return m_CheckPos;}

    virtual void OnStateChange(uint32 state);

protected:
    virtual void OnEnterScene( Scene* pScene );

private:
    void UpdateCheckPos();

    void UpdateTeamPos();

private:
    Player* m_Player;

    CoVec2  m_CheckPos;     // 用来检测角色超速移动的位置变量

    bool    m_IsRunning;    // 是否移动中,如果是会做移动预测

    uint32  m_TeamSendTimer;    // 队伍位置发送计时
};


#endif // action_player_controller_h__
