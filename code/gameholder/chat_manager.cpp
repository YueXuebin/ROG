#include "gameholder_pch.h"
#include "chat_manager.h"
#include "gm_commander.h"
#include "player.h"
#include "scene.h"
#include "regular_dungeon.h"
#include "center_handler.h"
#include "item_manager.h"
#include "game_setting_manager.h"
#include "gate_handler.h"
// ==================================================
// 构造函数
// ==================================================
ChatManager::ChatManager(Player* player) :
m_player(player)
{
    m_chat_time = 0;
}

// ==================================================
// 析构函数
// ==================================================
ChatManager::~ChatManager()
{
    m_player = 0;
    m_chat_time = 0;
}

// ==================================================
// 接收消息
// ==================================================
void ChatManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_CHAT_REQ:
        {
            OnChatReq(data);            // 聊天信息
        }
        break;
    case MSG_MARQUEE_REQ:
        {
            OnMarqueeReq(data);         // marquee
        }
        break;
    default:
        break;
    }
}

// ==================================================
// 聊天请求
// ==================================================
void ChatManager::OnChatReq(void* data)
{
    CHAT_MSG* recvMsg= (CHAT_MSG*)data;

    // 检查消息是否有效
    if(!recvMsg || (recvMsg->chat.length() > CHAT_MAX_LENGTH))
    {
        return;
    }

    switch(recvMsg->channel)
    {
    case CHAT_CHANNEL_MARQUEE:          // 跑马灯
        {
            // 把目标名字强制置空
          recvMsg->dst_name = "";
        }
        break;
    case CHAT_CHANNEL_SYSTEM:           // 系统频道
    case CHAT_CHANNEL_EMERGENCY:        // 应急频道
        {   // 把目标名字强制置空
            recvMsg->dst_name = "";
            // GM才可以发(系统,紧急,公告)
            if(!m_player->IsGM())
                return;
        }
        break;
    case CHAT_CHANNEL_WORLD:            // 世界频道
        {   // 把目标名字强制置空
            recvMsg->dst_name = "";
            if(m_player->GetLevel() < (int32)GameSettingManager::Instance()->GetChatWorldLevel())
            {
                m_player->NotifyErrorCode(ERR_CHAT_WORLD_LEVEL);
                return;
            }
        }
        break;
    case CHAT_CHANNEL_TEAM:             // 队伍频道
        if(m_player->GetLevel() < (int32)GameSettingManager::Instance()->GetChatTeamLevel())
        {
            m_player->NotifyErrorCode(ERR_CHAT_TEAM_LEVEL);
            return;
        }
        break;
    case CHAT_CHANNEL_PRIVATE:          // 私聊频道
        if(m_player->GetLevel() < (int32)GameSettingManager::Instance()->GetChatPrivateLevel())
        {
            m_player->NotifyErrorCode(ERR_CHAT_PRIVATE_LEVEL);
            return;
        }
        break;
    case CHAT_CHANNEL_GUILD:            // 公会频道
        if(m_player->GetLevel() < (int32)GameSettingManager::Instance()->GetChatGuildLevel())
        {
            m_player->NotifyErrorCode(ERR_CHAT_GUILD_LEVEL);
            return;
        }
        break;
    case CHAT_CHANNEL_GMCMD:            // GM 命令
        {
            // GM命令
            if(recvMsg->chat.substr(0,4) == "/gm ")
            {
                GmCommander::Instance()->ParseGMCommand(m_player, recvMsg->chat.substr(4));
                return;
            }
        }
        break;
    case CHAT_CHANNEL_DEBUG:            // 调试命令
        {
            if(recvMsg->chat.substr(0, 7) == "/debug ")
            {
                ParseDebugCommand(recvMsg->chat.substr(7));
            }
            return;
        }
        break;
    case CHAT_CHANNEL_HORN:             //喇叭
        {
            if(m_player->GetLevel() < (int32)GameSettingManager::Instance()->GetChatHornLevel())
            {
                m_player->NotifyErrorCode(ERR_CHAT_HORN_LEVEL);
                return;
            }

            //int32 itemNum = m_player->GetItemManager()->GetItemNum(CHAT_CHANNEL_HORN_ITEM);
            //if (itemNum >= 1)
            //{
            //    //m_player->GetBag()->DelItemByID(CHAT_CHANNEL_HORN_ITEM, 1);
            //} 
            //else
            //{
            //    m_player->NotifyErrorCode(ERR_NO_ENOUGH_ITEM);
            //    return;
            //}
        }
        break;
    case CHAT_CHANNEL_INTEGRATED:       // 综合频道
    default:                            // 未知类型，直接返回
        {
            CnAssert(false);
            return;
        }
        break;
    }

    // 检查是否可以发送聊天消息
    if(!CanChat(recvMsg))
    {
        return;
    }

    recvMsg->src_flag = m_player->GetRightFlag();
    recvMsg->src_name = m_player->GetName();
    m_player->SendToCenter(MSG_CHAT_REQ, recvMsg);
}

// ==================================================
// 检查是否能聊天
// ==================================================
bool ChatManager::CanChat(const CHAT_MSG* pChat)
{
    // 检查功能开启
    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_CHAT, m_player) != ERR_SUCCEED)
    //{
    //    return false;
    //}

    // 是否被禁言
    if(m_player->GetRightFlag() & RIGHT_FLAG_FORBID_CHAT)
    {
        m_player->SendToGate(MSG_CHAT_ACK, (void*)pChat);       // 被禁言了，自己仍然可以看见
        return false;
    }

    // 检查条聊天频繁
    if(IsChatFrequently(pChat))
    {
        return false;
    }

    return true;
}

// ==================================================
// 检查聊天是否频繁
// ==================================================
bool ChatManager::IsChatFrequently(const CHAT_MSG* pChat)
{
    // 检查聊天的频率阀值
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    if(m_chat_time > (now + CHAT_THRESHOLD_TIME))
    {
        m_player->NotifyErrorCode(ERR_CHAT_FREQUENTLY);
        return true;
    }
    else
    {
        if(m_chat_time > now)
            m_chat_time += CHAT_INTERVAL_TIME;
        else
            m_chat_time = now + CHAT_INTERVAL_TIME;
    }

    return false;
}

// ==================================================
// 调试命令
// ==================================================
void ChatManager::ParseDebugCommand(const std::string& debugCmd)
{
    std::vector<std::string> cmds = Crown::SDSplitStrings(debugCmd, ' ');
    std::string strCmd;

    if(cmds.size() < 1)
    {
        return;
    }

    if(cmds[0] == "gamelock")
    {
        if(m_player->GetScene() && m_player->GetScene()->GetRegular())
        {
            Regular* regular = m_player->GetScene()->GetRegular();
            strCmd = debugCmd + " svr_gamestart=" + Crown::ToString(m_player->GetGameStart());
            strCmd += " svr_gamelock=" + Crown::ToString(m_player->GetGameLock());
            if(regular->IsKindOf(&RegularDungeon::ms_RTTI))
            {
                RegularDungeon* pRegularDungeon = CnDynamicCast(RegularDungeon, regular);
                strCmd += " state=" + Crown::ToString(pRegularDungeon->GetState());
                strCmd += " ready_time=" + Crown::ToString(pRegularDungeon->GetReadyTime());
                strCmd += " run_time=" + Crown::ToString(pRegularDungeon->GetRunTime());
                strCmd += " end_time=" + Crown::ToString(pRegularDungeon->GetEndTime());
                strCmd += " current_time=" + Crown::ToString(pRegularDungeon->GetCurrentTime());
            }
            else
            {
                strCmd += " regular is world";
            }


            SendLog(strCmd);
        }
    }
}

// ==================================================
// 发送 Log
// ==================================================
void ChatManager::SendLog(const std::string& strCmd)
{
    LOG_REQ sendMsg;

    if(!m_player)
        return;

    sendMsg.idflag = LOG_DEBUG;
    sendMsg.log = strCmd;

    CenterHandler::Instance()->SendByPlayer(m_player, MSG_LOG_REQ, &sendMsg);
}

void ChatManager::OnMarqueeReq(void * data)
{
    MARQUEE* recvMsg= (MARQUEE*)data;

    recvMsg->src_vip = m_player->GetVipLevel();
    recvMsg->src_flag = m_player->GetRightFlag();
    recvMsg->src_name = m_player->GetName();
    GateHandler::Instance()->SendToAll(MSG_MARQUEE_NTF, recvMsg, BROADCAST_PLAYER_ID);
}

