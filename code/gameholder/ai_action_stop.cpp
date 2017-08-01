#include "gameholder_pch.h"
#include "ai_action_stop.h"
#include "battle_unit.h"
#include "ai_controller.h"
#include "scene.h"
#include "math.h"
#include "action_controller.h"

AIActionStop::AIActionStop() :
AIActionBase(AI_ACTION_STOP)
{

}

AIActionStop::~AIActionStop()
{

}

void AIActionStop::Do()
{
    m_pController->GetOwner()->GetActionController()->Stop();
}

