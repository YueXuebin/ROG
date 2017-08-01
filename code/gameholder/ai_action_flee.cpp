#include "gameholder_pch.h"
#include "ai_action_flee.h"
#include "battle_unit.h"
#include "ai_controller.h"
#include "scene.h"
#include "math.h"
#include "action_controller.h"

AIActionFlee::AIActionFlee() :
AIActionBase(AI_ACTION_FLEE)
{
    m_range = 0;
    m_range_min = 0;
}

AIActionFlee::~AIActionFlee()
{

}

void AIActionFlee::ParseXML( TiXmlElement* pDoc )
{
    AIActionBase::ParseXML(pDoc);
    pDoc->QueryIntAttribute("range", &m_range);
    pDoc->QueryIntAttribute("range_min", &m_range_min);
    
}

void AIActionFlee::Do()
{
    BattleUnit* pOwner = m_pController->GetOwner();
    BattleUnit* pTarget = pOwner->GetScene()->GetBattleUnit( m_pController->GetTargetUnitId() );

    if (!pTarget)
        return;
    if (!pOwner->CanAttack(pTarget))
        return;

    int32 pOwnerX = pOwner->GetX();
    int32 pOwnerY = pOwner->GetY();
    int32 pTargetX = pTarget->GetX();
    int32 pTargetY = pTarget->GetY();

    // 寻找一个远离目标的位置
    int32 i = 8;        // 尝试次数
    real x = 0;
    real y = 0;
    do 
    {
        real disX = real(pOwnerX - pTargetX);
        real disY = real(pOwnerY - pTargetY);

        real a = atan2(disY, disX);

        a = a + Crown::SDRandomFloat((real)M_PI*3/2) - (real)M_PI*3/4;

        real r = (real)Crown::SDRandom(m_range - m_range_min) + m_range_min;

        x = r * cos(a) + pOwnerX;
        y = r * sin(a) + pOwnerY;
    } 
    while ( !pOwner->CheckPosition((int32)x , (int32)y) && i-- );

    if (i <= 0)
        return;

    if (m_run)
    {
        pOwner->GetActionController()->RunTo((int32)x,(int32)y);
    }
    else
    {
        pOwner->GetActionController()->WalkTo((int32)x,(int32)y);
    }
    
}

