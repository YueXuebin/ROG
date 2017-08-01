#include "gameholder_pch.h"
#include "ai_action_moveto.h"
#include "battle_unit.h"
#include "ai_controller.h"
#include "scene.h"
#include "action_controller.h"
#include "pet.h"
#include "sceneobject.h"
#include "npc.h"
#include "ai_group_register.h"
#include "regular.h"


AIActionMoveTo::AIActionMoveTo() :
AIActionBase(AI_ACTION_MOVE_TO)
{
    m_type = 1;

}

AIActionMoveTo::~AIActionMoveTo()
{

}

void AIActionMoveTo::Do()
{
    DoMoveTo();
}

void AIActionMoveTo::DoMoveTo()
{
    BattleUnit* pTarget = NULL;
    BattleUnit* pOwner = m_pController->GetOwner();
    switch(m_type)
    {
    case 1:     // 敌人
    case 5:     // 玩家
        {
            uint32 targetId = m_pController->GetTargetUnitId();
            if(targetId == 0)
                return;

            pTarget = pOwner->GetScene()->GetBattleUnit(targetId);
        }
        break;
    case 4:     // 主人
        {
            if(pOwner->GetObjectType() != OBJ_PET)
                return;
            Pet* pet = (Pet*) pOwner;
            pTarget = pet->GetMaster();
        }
        break;
    case 6:     // 挂机点
        {
            if(m_run)
                pOwner->GetActionController()->RunTo(m_pController->GetBaseX(), m_pController->GetBaseY());
            else
                pOwner->GetActionController()->WalkTo(m_pController->GetBaseX(), m_pController->GetBaseY());
        }
        break;
    default:
        return;
    }

    if(!pTarget)
        return;

    MoveToTarget(pTarget);
}

void AIActionMoveTo::ParseXML( TiXmlElement* pDoc )
{
    AIActionBase::ParseXML(pDoc);
    pDoc->QueryIntAttribute("type", &m_type);
}

void AIActionMoveTo::MoveToTarget(BattleUnit* pTarget)
{
    CoVec2 targetPos;
    BattleUnit* pOwner = m_pController->GetOwner();
    if (!pTarget || !pOwner || !pOwner->GetScene())
        return;

    // 走到的目标最接近的位置
    CoVec2 dir = pTarget->GetPosition() - pOwner->GetPosition();
    real length = dir.Length();
    real radius2 = (real)pOwner->GetRadius() + (real)pTarget->GetRadius();
    real dis = length - radius2 - 5;        // 距离范围之内近似算已经走到目的地

    if (dis < 0)
        return;
    
    dir.Normalize();
    CoVec2 offset = dir * dis;
    targetPos = pOwner->GetPosition() + offset;

    if(m_run)
        pOwner->GetActionController()->RunTo((int32)targetPos.x, (int32)targetPos.y);
    else
        pOwner->GetActionController()->WalkTo((int32)targetPos.x, (int32)targetPos.y);
}


