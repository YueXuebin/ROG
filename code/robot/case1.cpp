#include "robot_pch.h"
#include "case1.h"
#include "move_random_action.h"


Case1::Case1(Robot* pRobot) :
CaseBase(pRobot)
{
    PushAction(CnNew MoveRandomAction(pRobot));
}

Case1::~Case1()
{

}

void Case1::OnRecv( uint32 msgID, void* data )
{
    CaseBase::OnRecv(msgID, data);
}

void Case1::Update( uint32 tickcount ,uint32 deltacount )
{
    CaseBase::Update(tickcount, deltacount);
}
