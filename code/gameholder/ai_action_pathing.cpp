#include "gameholder_pch.h"
#include "ai_action_pathing.h"
#include "battle_unit.h"
#include "ai_controller.h"
#include "checkpoint.h"
#include "action_controller.h"


AIActionPathing::AIActionPathing() :
AIActionBase(AI_ACTION_PATHING)
{
    m_CheckRange = 0;
}

AIActionPathing::~AIActionPathing()
{

}

void AIActionPathing::Do()
{
    Checkpoint* pCP = m_pController->GetCurrentCheckPoint();
    if(!pCP)
        return;
    else
    {
        BattleUnit* pOwner = m_pController->GetOwner();
        CoVec2 Diff = pOwner->GetPosition();
        Diff.x -= pCP->GetX();
        Diff.y -= pCP->GetY();

        if( Diff.Length() <= (pCP->GetRadius() + m_CheckRange) )
        {   // 移动到路点上了，进入下一个路点
            m_pController->SetBasePosition(pCP->GetX(), pCP->GetY());
            m_pController->NextPathIndex();
        }
        else
        {
            if(m_run)
                pOwner->GetActionController()->RunTo(pCP->GetX(), pCP->GetY());
            else
                pOwner->GetActionController()->WalkTo(pCP->GetX(), pCP->GetY());
        }
    }
}

void AIActionPathing::ParseXML( TiXmlElement* pDoc )
{
    AIActionBase::ParseXML(pDoc);
    
    int32 checkRange = 0;       // 距离路点范围
    pDoc->QueryIntAttribute("radius", &checkRange);
    m_CheckRange = Crown::SDMax(checkRange, 0);
}


