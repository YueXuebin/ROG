#include "robot_pch.h"
#include "guild_createaction.h"
#include "robot.h"
#include "commin_const.h"

Guild_CreateAction::Guild_CreateAction(Robot* pRobot, int32 doType, int32 arg2, int32 arg3):
ActionBase(pRobot)
{
    m_Name = "GuildCreate";

    // 公会操作类型(1 创建公会, 2 加入公会, 3 退出公会 4 等待被邀请加入公会 5 公会捐献)
    m_doType   = doType;
    m_arg2     = arg2;
    m_arg3     = arg3;
}

Guild_CreateAction::~Guild_CreateAction(void)
{
}

void Guild_CreateAction::OnStart()
{
    ActionBase::OnStart();

    switch(m_doType)
    {
    case 1: // 创建公会
        CreateGuild();
        break;
    case 2: // 加入公会
        JoinGuild();
        break;
    case 3: // 退出公会
        break;
    case 4: // 等待被邀请加入公会
        break;
    case 5: // 公会捐献
        GuildDonate();
        break;
    default:
        break;
    }
}

void Guild_CreateAction::OnRecv(uint32 msgID, void* data)
{
    ActionBase::OnRecv(msgID, data);
    switch(msgID)
    {
    case MSG_CREATE_GUILD_ACK:
        {
            OnCreateGuildAck();
        }
        break;
    case MSG_GUILD_ID_NTF:
        {
            GUILD_ID_NTF* recvMsg = (GUILD_ID_NTF*)data;

            // 开放自动同意入会申请
            GUILD_SET_FLAG_REQ sendMsg;
            sendMsg.guild_flag = GUILD_AUTO_JOIN_FLAG;
            m_Robot->Send(MSG_GUILD_SET_FLAG_REQ, &sendMsg);

            uint64 guilId = U64ID(recvMsg->guild_id_h ,recvMsg->guild_id_l);
            CnInfo("create guild success! guildID:%d\n", guilId);
            End();
        }
        break;
    case MSG_APPLY_JOIN_GUILD_ANS:
        {
            APPLY_JOIN_GUILD_ACK* recvMsg = (APPLY_JOIN_GUILD_ACK*)data;
            OnJoinGuildAck(recvMsg);
        }
        break;
    case MSG_INVITE_JOIN_GUILD_CFM:
        {
            INVITE_JOIN_GUILD_REQ* recvMsg = (INVITE_JOIN_GUILD_REQ*)data;
            OnJoinGuildAns(recvMsg);
        }
        break;
    case MSG_GUILD_LIST_ACK:
        {
            GUILD_LIST_ACK* recvMsg = (GUILD_LIST_ACK*)data;
            OnGuildListAck(recvMsg);
        }
        break;
    case MSG_GUILD_INFO_NTF:
        {
            GUILD_INFO* recvMsg = (GUILD_INFO*)data;
            CnInfo("guild level:%d, exp:%d\n", recvMsg->level, recvMsg->exp);
            End();
        }
        break;
    case MSG_APPLY_INFO_NTF:
        {
            CnInfo("some one join guild\n");
        }
        break;
    default:
        break;
    }
}

void Guild_CreateAction::CreateGuild()
{
    CREATE_GUILD_REQ sendmsg;
    sendmsg.guild_name = m_Robot->GetAccount();
    m_Robot->Send(MSG_CREATE_GUILD_REQ, &sendmsg);
}

void Guild_CreateAction::JoinGuild()
{
    if(m_arg2 == 0 || m_arg3 == 0)
    {
        m_page = 1;
        // 查找列表 随机加一个
        GUILD_LIST_REQ guildListMsg;
        guildListMsg.page = m_page;
        m_Robot->Send(MSG_GUILD_LIST_REQ, &guildListMsg);
        return;
    }

    APPLY_JOIN_GUILD_REQ sendMsg;
    sendMsg.guild_id_h = m_arg2;
    sendMsg.guild_id_l = m_arg3;
    m_Robot->Send(MSG_APPLY_JOIN_GUILD_REQ, &sendMsg);
}

void Guild_CreateAction::OnCreateGuildAck()
{
    CnInfo("create guild success! guildID_h:%d , guildID_l:%d\n", m_arg2, m_arg3);

    // 开放自动同意入会申请
    GUILD_SET_FLAG_REQ sendMsg;
    sendMsg.guild_flag = GUILD_AUTO_JOIN_FLAG;
    m_Robot->Send(MSG_GUILD_SET_FLAG_REQ, &sendMsg);
    End();
}

void Guild_CreateAction::OnJoinGuildAck(APPLY_JOIN_GUILD_ACK* recvMsg)
{
    if(recvMsg->errcode == ERR_SUCCEED)
        CnInfo("join guild id_h:%d, id_l:%d success!\n", m_arg2, m_arg3);
    else
        CnInfo("join guild id_h:%d, id_l:%d faild, err:%d!\n", m_arg2, m_arg3, recvMsg->errcode);

    End();
}

void Guild_CreateAction::OnJoinGuildAns(INVITE_JOIN_GUILD_REQ* recvMsg)
{
    INVITE_JOIN_GUILD_ACK sendMsg;
    sendMsg.errcode = ERR_SUCCEED;
    sendMsg.guild_id_h = recvMsg->guild_id_h;
    sendMsg.guild_id_l = recvMsg->guild_id_l;
    m_Robot->Send(MSG_INVITE_JOIN_GUILD_ANS, &sendMsg);

    // 设置当前要加入的公会ID
    m_arg2 = recvMsg->guild_id_h;
    m_arg3 = recvMsg->guild_id_l;
    End();
}

void Guild_CreateAction::OnGuildListAck(GUILD_LIST_ACK* recvMsg)
{
    if(recvMsg->guild_list.size() == 0)
        return;

    uint32 randomNum = Crown::SDRandom((uint32)recvMsg->guild_list.size());

    const GUILD_BRIEF_INFO& guildIt = recvMsg->guild_list[randomNum];

    if(guildIt.member_count >= 20)
    {
        m_page++;
        if((int32)m_page >= recvMsg->total_page)
            return;

        GUILD_LIST_REQ guildListMsg;
        guildListMsg.page = m_page;
        m_Robot->Send(MSG_GUILD_LIST_REQ, &guildListMsg);
        return;
    }

    // 设置当前要加入的公会ID
    m_arg2 = guildIt.guild_id_h;
    m_arg3 = guildIt.guild_id_l;

    APPLY_JOIN_GUILD_REQ sendMsg;
    sendMsg.guild_id_h = m_arg2;
    sendMsg.guild_id_l = m_arg3;
    m_Robot->Send(MSG_APPLY_JOIN_GUILD_REQ, &sendMsg);
}

void Guild_CreateAction::GuildDonate()
{
    GUILD_DONATE_REQ sendMsg;
    m_Robot->Send(MSG_GUILD_DONATE_REQ, &sendMsg);
}
