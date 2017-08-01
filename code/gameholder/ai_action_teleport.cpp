#include "gameholder_pch.h"
#include "ai_action_teleport.h"
#include "ai_controller.h"
#include "battle_unit.h"
#include "pet.h"
#include "action_controller.h"

AIActionTeleport::AIActionTeleport() :
AIActionBase(AI_ACTION_TELEPORT)
{

}

AIActionTeleport::~AIActionTeleport()
{

}

void AIActionTeleport::Do()
{
    BattleUnit* pOwner = m_pController->GetOwner();
    if ( pOwner->GetObjectType() == OBJ_PET )
    {
        Pet* pet = (Pet*)pOwner;
        // 得到主人坐标
        BattleUnit* master = pet->GetMaster();
        if (!master) return;
        pet->GetActionController()->JumpTo(master->GetX(),master->GetY());
    }
}

