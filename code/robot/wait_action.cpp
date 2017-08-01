#include "robot_pch.h"
#include "wait_action.h"


WaitAction::WaitAction( Robot* pRobot, int32 millisecond )
: ActionBase(pRobot),
m_millisecond(millisecond)
{
    m_Name = "Wait";
}

WaitAction::~WaitAction()
{

}

void WaitAction::Update( uint32 tickcount, uint32 deltacount )
{
    ActionBase::Update(tickcount, deltacount);

    if(m_MilliSecondTimer > m_millisecond)
        End();
}
