#include "gameholder_pch.h"
#include "ai_action_move_random.h"
#include "battle_unit.h"
#include "ai_controller.h"
#include "action_controller.h"
#include "soldier.h"

AIActionMoveRandom::AIActionMoveRandom() :
AIActionBase(AI_ACTION_MOVE_RANDOM)
{
    m_IsBasePose = true;
    m_MinAngle = 0;
    m_MaxAngle = 0;
    m_MinRadius = 0;
    m_MaxRadius = 0;
}

AIActionMoveRandom::~AIActionMoveRandom()
{

}

void AIActionMoveRandom::Do()
{
    BattleUnit* pOwner = m_pController->GetOwner();

    int32 radius = int32( Crown::SDRandomFloat((real)m_MaxRadius-m_MinRadius) + m_MinRadius );
    if(radius <= 0)
        return;

    if (pOwner->IsKindOf(&Soldier::ms_RTTI))
    {
        Soldier* pSoldier = CnDynamicCast(Soldier, pOwner);
        m_pController->SetBasePosition(pSoldier->GetMaster()->GetX(), pSoldier->GetMaster()->GetY());
    }

    // 巡逻目标点(不能在墙里面)
    int32 tryCount = 5;     // 最多试5次
    int32 px, py;
    do 
    {
        real angle = (real)( (Crown::SDRandomFloat((real)m_MaxAngle-m_MinAngle)+m_MinAngle+pOwner->GetAngle()) / 180 * M_PI );
        if(m_IsBasePose)
        {
            px = (int32)( m_pController->GetBaseX() + radius * (real)cos((real)angle) );
            py = (int32)( m_pController->GetBaseY() + radius * (real)sin((real)angle) );
        }
        else
        {
            px = (int32)( pOwner->GetX() + radius * (real)cos((real)angle) );
            py = (int32)( pOwner->GetY() + radius * (real)sin((real)angle) );
        }
        tryCount--;
    } 
    while ( !pOwner->CheckPosition(px, py) && tryCount>0 );
    if (tryCount <= 0)
        return;
    if (m_run)
        pOwner->GetActionController()->RunTo(px,py);
    else
        pOwner->GetActionController()->WalkTo(px, py);
}

void AIActionMoveRandom::ParseXML( TiXmlElement* pDoc )
{
    AIActionBase::ParseXML(pDoc);

    pDoc->QueryIntAttribute("min_angle", &m_MinAngle);
    pDoc->QueryIntAttribute("max_angle", &m_MaxAngle);
    pDoc->QueryIntAttribute("min_radius", &m_MinRadius);
    pDoc->QueryIntAttribute("max_radius", &m_MaxRadius);

    int32 tmpInt = 0;
    pDoc->QueryIntAttribute("character_pose", &tmpInt);
    m_IsBasePose = (tmpInt == 0);
    
}
