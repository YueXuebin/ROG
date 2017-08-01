#include "gameholder_pch.h"
#include "action_player_controller.h"
#include "player.h"
#include "state_define.h"
#include "game_define.h"
#include "state_controller.h"
#include "team_manager.h"


ActionPlayerController::ActionPlayerController(Player* player) :
ActionCreatureController(player),
m_Player(player)
{
    m_IsRunning = false;
    m_TeamSendTimer = 0;
}

ActionPlayerController::~ActionPlayerController()
{

}

void ActionPlayerController::StopAt(int32 x, int32 y)
{
    m_IsRunning = false;
}

void ActionPlayerController::RunTo( int32 x, int32 y )
{
    m_IsRunning = false;

    if(m_Player->GetGameLock())
        return;

    if(!m_Player->CanChangePostion())
        return;

    // 检查新位置是否合法
    bool isValid = true;
    if(!m_Player->CheckPostionNoRadius(x, y))
        isValid = false;

    if(!m_Player->IsGM())       // GM可以随意跳
    {
        if(Vec2DistanceSquared(CoVec2(x, y), m_CheckPos) > g_MoveDelta*g_MoveDelta)        // 移动不合法(加速了)
            isValid = false;
    }

    if(!isValid)
    {   // 位置不合法
        MOVE_ACK ack;
        ack.errcode = ERR_FAILED;
        ack.x = m_Player->GetX();
        ack.y = m_Player->GetY();
        m_Player->SendToGate(MSG_MOVE_ACK, &ack);
    }
    else
    {   
        m_IsRunning = true;
        m_Player->SetPosition(x, y);
    }
}

void ActionPlayerController::Update()
{
    ActionCreatureController::Update();
    //if(m_IsPushing)
    //{   // 推动处理
    //    UpdatePush();
    //    m_CheckPos = m_Player->GetPosition();
    //}
    //else
    //{   // 移动处理
    //    if(m_IsRunning)
    //    {
    //        int32 moveDistance = m_Player->GetMoveSpeed() / g_ServerUpdateTime;

    //        int32 x = (int32)( m_Player->GetX() + moveDistance * cos(ANGLE_TO_RADIAN(m_Player->GetAngle())) );
    //        int32 y = (int32)( m_Player->GetY() + moveDistance * sin(ANGLE_TO_RADIAN(m_Player->GetAngle())) );

    //        if(m_Player->CheckPostionNoRadius(x, y))
    //        {
    //            m_Player->SetPosition(x, y);
    //            m_isMoving = true;
    //        }
    //        else
    //            m_isMoving = false;
    //    }
    //    else
    //    {
    //        m_isMoving = false;
    //    }

    //    SendMove();
    //}
    
    UpdateCheckPos();

    UpdateTeamPos();
}

void ActionPlayerController::UpdateCheckPos()
{
    if(m_IsPushing)
    {   // 推动时不检测
        m_CheckPos = m_Player->GetPosition();
        return;
    }

     // 移动处理
    CoVec2 dir(m_Player->GetX() - m_CheckPos.x, m_Player->GetY() - m_CheckPos.y);
    real dirLength = dir.Length();
    if(dirLength <= 1)
        return;

    real moveDistance = (real)m_Player->GetMoveSpeed() / g_ServerUpdateFrequency * 1.0f;        // 校验速度

    if(dirLength < moveDistance)
    {   // 到达目的地了
        m_CheckPos = m_Player->GetPosition();
    }
    else
    {   // 还需要继续移动
        real dirAngle = dir.GetAngle();
        real x = m_CheckPos.x + moveDistance * cos( dirAngle );
        real y = m_CheckPos.y + moveDistance * sin( dirAngle );
        m_CheckPos.Set(x, y);
    }
}

void ActionPlayerController::UpdateTeamPos()
{
    if(m_Player->GetTeamID() == 0)
        return;

    m_TeamSendTimer += g_ServerUpdateTime;
    if(m_TeamSendTimer >= g_TeamPostionSyncTime)
    {
        m_TeamSendTimer = 0;

        TeamManager::Instance()->UpdateTeamMemberInfo(m_Player);
    }
}

bool ActionPlayerController::UseSkill( uint32 skillId, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skillObjID, uint32 skillSlot )
{
    if(m_Player->GetGameLock())
        return true;

    return ActionCreatureController::UseSkill(skillId, target, args, skillObjID, skillSlot);
}

bool ActionPlayerController::Dance()
{
    // 跳舞状态转换
    if(!m_Player->CanChangeTo(STATE_DANCE))
    {
        return false;
    }

    m_Player->ChangeState(STATE_DANCE);
    return true;
}

bool ActionPlayerController::JumpTo( int32 x, int32 y )
{
    bool ret = ActionCreatureController::JumpTo(x, y);
    if(ret && time==0)
    {
        m_CheckPos = m_Player->GetPosition();       // 更新角色的检查位置
    }
    return ret;
}

void ActionPlayerController::OnEnterScene( Scene* pScene )
{
    m_CheckPos = m_Player->GetPosition();
}

void ActionPlayerController::OnStateChange(uint32 state)
{
    ActionCreatureController::OnStateChange(state);

    if(m_IsRunning)
    {
        if(IsServerState(state))
        {
            m_IsRunning = false;
        }
    }
}

