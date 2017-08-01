#include "gameholder_pch.h"
#include "ai_condition_finish_path.h"
#include "ai_controller.h"
#include "battle_unit.h"
#include "checkpoint.h"
#include "action_controller.h"

AIConditionFinishPath::AIConditionFinishPath() : 
AIConditionBase(AI_CONDITION_FINISH_PATH)
{

}

AIConditionFinishPath::~AIConditionFinishPath()
{

}


bool AIConditionFinishPath::IsSatisfy()
{
    if (m_pController->IsPathingEnd())
    {
        return true;
    }
    return false;
}



