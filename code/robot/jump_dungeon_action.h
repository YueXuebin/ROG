#ifndef jump_dungeon_action_h__
#define jump_dungeon_action_h__

// Ìø¸±±¾
#include "action_base.h"

class JumpDungeonAction : public ActionBase
{
public:
    JumpDungeonAction(Robot* pRobot, uint32 dungeonID, uint32 channelID);
    virtual ~JumpDungeonAction();

    virtual void OnStart();

    virtual void OnRecv(uint32 msgID, void* data);

private:
    uint32  m_DungeonID;
    uint32  m_ChannelID;

};

#endif // jump_dungeon_action_h__
