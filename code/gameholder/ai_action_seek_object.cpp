#include "gameholder_pch.h"
#include "ai_action_seek_object.h"
#include "battle_unit.h"
#include "scene.h"
#include "game_util.h"
#include "ai_controller.h"
#include "ai_group_register.h"
#include "regular.h"

AIActionSeekObject::AIActionSeekObject() :
AIActionBase(AI_ACTION_SEEK_OBJECT)
{
    m_type = 0;
    m_radius = 0;
}

AIActionSeekObject::~AIActionSeekObject()
{

}

void AIActionSeekObject::Do()
{
    BattleUnit* pOwner = m_pController->GetOwner();
    if(!pOwner || !pOwner->GetScene())
        return;

    BattleUnit* EnemyBattleUnit = NULL;

    // 找一个离的最近的敌人
    EnemyBattleUnit = pOwner->GetScene()->FindNearestEnemyBattleUnitInCircle(pOwner, pOwner->GetPosition(), m_radius);
    if(!EnemyBattleUnit)
        return;

    if(EnemyBattleUnit->GetId() == m_pController->GetTargetUnitId())
        return;

    CnAssert(pOwner->CanAttack(EnemyBattleUnit));
    m_pController->SetTargetUnitId(EnemyBattleUnit->GetId());
}

void AIActionSeekObject::ParseXML( TiXmlElement* pDoc )
{
    AIActionBase::ParseXML(pDoc);

    pDoc->QueryIntAttribute("type",&m_type);
    pDoc->QueryIntAttribute("radius",&m_radius);
}
