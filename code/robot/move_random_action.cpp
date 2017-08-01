#include "robot_pch.h"
#include "move_random_action.h"
#include "robot.h"



MoveRandomAction::MoveRandomAction(Robot* pRobot) :
MoveAction(pRobot, 0, 0)
{
    m_Name = "MoveRandom";
    // Ä¿µÄÎ»ÖÃ
    int32 diffx = rand()%200;
    int32 diffy = rand()%200;

    if(rand()%2) diffx = -diffx;
    if(rand()%2) diffy = -diffy;

    int32 srcX = m_Robot->GetPropInfo().x;
    int32 srcY = m_Robot->GetPropInfo().y;

    m_desX = srcX + diffx;
    m_desY = srcY + diffy;
}

MoveRandomAction::~MoveRandomAction()
{

}

