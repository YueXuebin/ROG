#include "gameholder_pch.h"
#include "ai_condition_master_in_range.h"
#include "ai_controller.h"
#include "battle_unit.h"
#include "scene.h"
#include "game_util.h"
#include "pet.h"

AIConditionMasterInRange::AIConditionMasterInRange() :
AIConditionBase(AI_CONDITION_MASTER_IN_RANGE)
{
    m_existence = true;
    m_Range = 0;
}

AIConditionMasterInRange::~AIConditionMasterInRange()
{

}

void AIConditionMasterInRange::ParseXML( TiXmlElement* pDoc )
{
    AIConditionBase::ParseXML(pDoc);

    int32 existence = 1;
    pDoc->QueryIntAttribute("range",&m_Range);
    pDoc->QueryIntAttribute("inout",&existence);
    if (existence == 1)
    {
        m_existence = true;
    }
    else
    {
        m_existence = false;
    }
}

bool AIConditionMasterInRange::IsSatisfy()
{
    BattleUnit* pOwner = m_pController->GetOwner();
    if ( pOwner->GetObjectType() != OBJ_PET )
        return false;
    Pet* pet = (Pet*) pOwner;

    if(pet->GetMaster() == NULL)
    {
        return false;
    }

    // 搜索范围内的所有目标
    BattleUnit* uints[20];
    int32 ret = pet->GetScene()->FindBattleUnitInCircle(pet->GetPosition(), m_Range, uints, 20, pOwner->GetId());

    BattleUnit* pTarget = NULL;
    for(int32 i=0; i<ret; i++)
    {
        BattleUnit* pUnit = uints[i];
        if(pUnit == pet)
            continue;

        if( pUnit == pet->GetMaster() )
        {
            pTarget = pUnit;
            break;
        }
    }

    if ( (pTarget != NULL) == m_existence )
    {
        if(pet->GetMaster())
        {
            m_pController->SetTargetUnitId(pet->GetMaster()->GetId());
        }
        return true;
    }
    return false;
}

