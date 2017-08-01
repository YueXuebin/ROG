#ifndef guild_createaction_h__
#define guild_createaction_h__

// 公会行为处理

#include "action_base.h"

class Guild_CreateAction : public ActionBase
{
public:
    Guild_CreateAction(Robot* pRobot, int32 doType, int32 guildID_h, int32 guildID_l);
    virtual ~Guild_CreateAction();
    
    virtual void OnStart();
    virtual void OnRecv(uint32 msgID, void* data);
private:
    void CreateGuild();
    void OnCreateGuildAck();

    void JoinGuild();
    void OnJoinGuildAck(APPLY_JOIN_GUILD_ACK* recvMsg);
    void OnGuildListAck(GUILD_LIST_ACK* recvMsg);

    void OnJoinGuildAns(INVITE_JOIN_GUILD_REQ* recvMsg);

    void GuildDonate();
private:
    int32       m_doType;
    uint32      m_arg2;
    uint32      m_arg3;

    uint32      m_page;
};

#endif // guild_createaction_h__