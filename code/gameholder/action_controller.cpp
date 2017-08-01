#include "gameholder_pch.h"
#include "battle_unit.h"
#include "action_controller.h"
#include "action_creature_controller.h"
#include "action_player_controller.h"
#include "player.h"

ActionController::ActionController(BattleUnit* owner)
{

}

ActionController::~ActionController()
{

}

ActionController* CreateActionController( BattleUnit* Owner )
{
    if(Owner->IsKindOf(&Player::ms_RTTI))
        return CnNew ActionPlayerController((Player*)Owner);
    else if(Owner->IsKindOf(&Creature::ms_RTTI))
        return CnNew ActionCreatureController((Creature*)Owner);
    else
        return CnNew ActionController(Owner);
}
