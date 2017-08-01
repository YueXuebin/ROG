#include "gameholder_pch.h"
#include "skill_define.h"
#include "skill_object.h"
#include "state_define.h"
#include "battle_unit.h"
#include "state_controller.h"
#include "scene.h"
#include "action_controller.h"
#include "creature.h"
#include "skill_utils.h"
#include "skill_logic_cast.h"
#include "game_util.h"
#include "player.h"


SkillLogicCast::SkillLogicCast()
{
    Clear();
}

SkillLogicCast::~SkillLogicCast()
{

}

void SkillLogicCast::Clear()
{
    m_DurTime = 0;
    m_PassTime = 0;

    m_LaunchStartTime = 0;
    m_StartTime = 0;

    m_LaunchLogic = NULL;

    m_InLogic = false;

    m_IsFirst = m_IsLast = false;
}

void SkillLogicCast::Init(SkillObject* pSkill)
{
    SkillLogicBase::Init(pSkill);
    
    m_CastData = &m_SkillData->m_CastData;

    // 计算施法逻辑的持续时间
    m_DurTime = int32(m_CastData->m_CastTime / ToPercent(m_SkillArgs->m_CastSpeed));
    // 计算发射逻辑的触发时间
    m_LaunchStartTime = int32(m_SkillData->m_LaunchData.StartTime / ToPercent(m_SkillArgs->m_CastSpeed));

    // 玩家ping值对技能修正
    BattleUnit* owner = pSkill->GetOwner();
    if(owner && owner->GetObjectType() == OBJ_PLAYER)
    {
        Player* player = CnDynamicCast(Player, owner);
        uint32 ping = player->GetPing();
        m_LaunchStartTime = Crown::SDMax(int32(m_LaunchStartTime - ping), 0);
    }
}
// 打断施法
void SkillLogicCast::BreakOff()
{
    End();

    if(m_InLogic)
    {
        m_InLogic = false;
        OnLeave();
    }
}

void SkillLogicCast::Update()
{
    if(!m_InLogic && m_PassTime >= m_StartTime)
    {
        // 进入施法
        if(m_DurTime == 0 || CanCast())
        {
            OnEnter();
        }
        else
        {   // 不能进入施法状态
            End();
            m_Skill->BreakOff();
        }
    }

    if(m_InLogic)
    {
        UpdateLogic();
    }

    if(m_InLogic && m_PassTime >= (m_StartTime+m_DurTime))
    {   // 离开施法
        End();
        OnLeave();
    }
    
    m_PassTime += g_ServerUpdateTime;
}

bool SkillLogicCast::CanCast()
{
    if(m_SkillArgs->m_SkillMode != SKILL_MODE_SELF)
        return true;

    if(m_IsFirst)
        return m_Skill->CanCast();
    else
        return true;
}

void SkillLogicCast::OnEnter()
{
    m_InLogic = true;

    if(m_SkillArgs->m_SkillMode != SKILL_MODE_SELF)
        return;

    MoveMe(&m_CastData->m_MoveData);

    if(m_IsFirst)
        m_Skill->EnterCast();
}

void SkillLogicCast::OnLeave()
{
    m_InLogic = false;

    if(m_SkillArgs->m_SkillMode != SKILL_MODE_SELF)
        return;

    if(m_IsLast)
        m_Skill->LeaveCast();
}

void SkillLogicCast::UpdateLogic()
{
    // 触发发射逻辑
    if(m_LaunchLogic == NULL && m_PassTime >= (m_StartTime+m_LaunchStartTime))
    {
        m_LaunchLogic = m_Skill->CreateLogicLaunch();
    }

}

void SkillLogicCast::OnStateChange( uint32 state )
{
    if(m_SkillArgs->m_SkillMode != SKILL_MODE_SELF)
        return;     // 非自身使用技能，不关心状态

    if(state == STATE_ATTACK || state == STATE_IDLE || state == STATE_RUN)
        return;

    if(m_InLogic)
        m_Skill->BreakOff();        // 技能被打断
}

void SkillLogicCast::MoveMe(const SkillMoveData* pMoveData)
{
    if(!pMoveData || !pMoveData->HasMove())
        return;

    int32 moveTime = int32(pMoveData->m_moveTime / ToPercent(m_SkillArgs->m_CastSpeed));
    int32 bounceTime = int32(pMoveData->m_BounceTime / ToPercent(m_SkillArgs->m_CastSpeed));

    BattleUnit* Owner = m_Skill->GetOwner();
    CnAssert(Owner);

    // 寻找移动目标点
    int32 angle = 0;
    int32 distance = 0;
    CoVec2 desPos;

    if(pMoveData->m_moveMode == 0 || pMoveData->m_moveMode == 2)     // 鼠标方向 || 目标方向
    {   // 朝(m_x, m_y)方向移动
        const SHOOT_TARGET& target = m_Skill->GetTarget();
        CoVec2 dir(target.target_x - Owner->GetX(), target.target_y - Owner->GetY());
        if(pMoveData->m_moveDistance > 0)
        {
            angle = RADIAN_TO_ANGLE(dir.GetAngle());
            distance = pMoveData->m_moveDistance;
            if(pMoveData->m_moveMode == 2)
            {   // 目标方向，移动不会越过目标
                BattleUnit* pTarget = CnDynamicCast(BattleUnit, Owner->GetScene()->GetSceneObject(target.target_id));
                if(pTarget)
                {
                    int32 targetRadius = pTarget ? pTarget->GetRadius() : 0;
                    distance = int32(dir.Length() - (targetRadius + Owner->GetRadius()));      // 距离减去自身和目标的半径
                }
                else
                {
                    distance = int32(dir.Length());
                }
                distance = Crown::SDMax(distance, 0);
            }
        }
        else
        {
            angle = int32(dir.GetAngle() / M_PI * 180 + 180);
            distance = -pMoveData->m_moveDistance;
        }
        desPos = Owner->GetScene()->GetCoWorld()->RayTraceFind(Owner->GetPosition(), (int32)angle, (real)distance, (real)Owner->GetRadius());
    }
    else if(pMoveData->m_moveMode == 1)    // 角色方向
    {   // 朝角色方向移动
        if(pMoveData->m_moveDistance > 0)
        {
            angle = int32(Owner->GetAngle());
            distance = pMoveData->m_moveDistance;
        }
        else
        {
            angle = int32(Owner->GetAngle() + 180);
            distance = -pMoveData->m_moveDistance;
        }
        desPos = Owner->GetScene()->GetCoWorld()->RayTraceFind(Owner->GetPosition(), (int32)angle, (real)distance, (real)Owner->GetRadius());
    }
    else
    {
        CnAssert(false);
        return;
    }

    // 推移自己处理
    Owner->GetActionController()->PushTo((int32)desPos.x, (int32)desPos.y, moveTime, false);

    // 弹跳自己处理
    if(bounceTime > 0 && pMoveData->m_BounceHeight > 0)
    {   // 客户端弹跳
        Creature* pCreature = CnDynamicCast(Creature, Owner);
        if(pCreature)
        {
            pCreature->Bounce(pMoveData->m_BounceHeight, bounceTime, true);
        }
    }

}