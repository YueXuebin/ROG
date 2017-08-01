#include "robot_pch.h"
#include "jump_dungeon_action.h"
#include "robot.h"


JumpDungeonAction::JumpDungeonAction( Robot* pRobot, uint32 dungeonID, uint32 channelID ) :
ActionBase(pRobot)
{
    m_Name = "JumpDungeon";

    m_DungeonID = dungeonID;
    m_ChannelID = channelID;

}

JumpDungeonAction::~JumpDungeonAction()
{

}

void JumpDungeonAction::OnStart()
{
    ActionBase::OnStart();

    bool dungeonValid = true;

    if(dungeonValid)
    {
        CHANGE_DUNGEON_REQ req;
        req.dungeon_id = m_DungeonID;
        req.channel_id = m_ChannelID;
        m_Robot->Send(MSG_CHANGE_DUNGEON_REQ, &req);
    }
    else
    {
        End();
    }
}

void JumpDungeonAction::OnRecv( uint32 msgID, void* data )
{
    ActionBase::OnRecv(msgID, data);

    switch(msgID)
    {
    case MSG_CHANGE_DUNGEON_ACK:
        {
           End();
        }
        break;
    }
}