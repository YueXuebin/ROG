#include "gameholder_pch.h"
#include "state_touch.h"
#include "battle_unit.h"
#include "state_define.h"
#include "touch_controller.h"

StateTouch::StateTouch(BattleUnit* owner) :
State(STATE_TOUCH, owner)
{

}

StateTouch::~StateTouch()
{

}

bool StateTouch::CanChangeTo(uint32 state)
{
    return true;
}

void StateTouch::onLeaveState()
{
    State::onLeaveState();
}

