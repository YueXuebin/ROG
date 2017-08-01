//------------------------------------------------------------------------------
//
//    Copyright (C) DoinGame. All rights reserved.
//
//    File Name:   mail_manager.cpp
//    Author:      Tomren
//    Create Time: 2013-08-16
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------
#include "gameholder_pch.h"
#include "player.h"
#include "center_handler.h"
#include "mail_manager.h"
#include "item_manager.h"
#include "game_util.h"
#include "mail_title_config.h"

// ==================================================
// 构造函数
// ==================================================
MailManager::MailManager(Player* player) :
m_player(player)
{
    Clear();
}

// ==================================================
// 析构函数
// ==================================================
MailManager::~MailManager()
{
    
}

// ==================================================
// 清空任务
// ==================================================
void MailManager::Clear()
{
    m_mail_info.Clear();
    m_last_send_time = 0;
}

// ==================================================
// 清空任务
// ==================================================
void MailManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_MAIL_INFO_REQ:                // 邮件信息请求
        {
            OnMailInfoReq();
        }
        break;
    case MSG_MAIL_OPT_REQ:
        {
            OnMailOptReq(data);
        }
        break;
    case MSG_MAIL_GET_ATTACH_REQ:
        {
            OnMailGetAttachReq(data);
        }
        break;
    case MSG_MAIL_SEND_REQ:
        {
            OnMailSendReq(data);
        }
        break;
    case MSG_MAIL_OPEN_SYSTEM_REQ:
        {
            OnMailOpenSystemReq(data);
        }
        break;
    default:
        break;
    }
}

// ==================================================
// 加载邮件信息
// ==================================================
void MailManager::LoadInfo(const MAIL_SYSTEM_INFO& info)
{
    m_mail_info = info;
}

// ==================================================
// 添加邮件
// ==================================================
void MailManager::AddMail(MAIL_INFO* mail)
{
    if(!mail) return;

    if((mail->mail_type == MAIL_TYPE_PLAYER) ||
        (mail->mail_type == MAIL_TYPE_PLAYER_SYS))
    {
        m_mail_info.player_mail_list.push_back(*mail);
    }
    else if(mail->mail_type == MAIL_TYPE_SYSTEM)
    {
        m_mail_info.system_mail_list.push_back(*mail);
    }
    else if(mail->mail_type == MAIL_TYPE_GROUP)
    {
        m_mail_info.system_mail_list.push_back(*mail);
    }
}

// ==================================================
// 删除邮件
// ==================================================
void MailManager::DelMail(uint8 mail_type, uint32 mail_id_h, uint32 mail_id_l)
{
    if((mail_type == MAIL_TYPE_PLAYER) || (mail_type == MAIL_TYPE_PLAYER_SYS))
    {
        for(std::vector<MAIL_INFO>::iterator mailIt=m_mail_info.player_mail_list.begin();
            mailIt != m_mail_info.player_mail_list.end(); ++mailIt)
        {
            if((mailIt->mail_id.id_h == mail_id_h) && (mailIt->mail_id.id_l == mail_id_l))
            {
                m_mail_info.player_mail_list.erase(mailIt);
                return;
            }
        }
    }
    else if((mail_type == MAIL_TYPE_SYSTEM) || (mail_type == MAIL_TYPE_GROUP))
    {
        for(std::vector<MAIL_INFO>::iterator mailIt=m_mail_info.system_mail_list.begin();
            mailIt != m_mail_info.system_mail_list.end(); ++mailIt)
        {
            if((mailIt->mail_id.id_h == mail_id_h) && (mailIt->mail_id.id_l == mail_id_l))
            {
                m_mail_info.system_mail_list.erase(mailIt);
                return;
            }
        }
    }
}

// ==================================================
// 删除过期邮件
// ==================================================
void MailManager::DelExpiredMail(uint64 expiredMailID)
{
    uint64 mailID = 0;

    for(std::vector<MAIL_INFO>::iterator mailIt=m_mail_info.player_mail_list.begin();
        mailIt != m_mail_info.player_mail_list.end(); )
    {
        mailID = U64ID(mailIt->mail_id.id_h, mailIt->mail_id.id_l);
        if(mailID <= expiredMailID)
        {
            mailIt = m_mail_info.player_mail_list.erase(mailIt);
        }
        else
        {
            ++mailIt;
        }
    }

    for(std::vector<MAIL_INFO>::iterator mailIt=m_mail_info.system_mail_list.begin();
        mailIt != m_mail_info.system_mail_list.end(); )
    {
        mailID = U64ID(mailIt->mail_id.id_h, mailIt->mail_id.id_l);
        if(mailID <= expiredMailID)
        {
            mailIt = m_mail_info.system_mail_list.erase(mailIt);
        }
        else
        {
            ++mailIt;
        }
    }
}

// ==================================================
// 发送邮件
// ==================================================
void MailManager::SendMail(uint32 centerID, uint64 playerID, const std::string& playerName, const std::string& title, const std::string& message, uint8 mailType, 
                           const std::vector<REWARD_ITEM>& list, uint64 receiverID, uint64 tradeOrderID, std::string* condition)
{
    MAIL_SEND_REQ sendMsg;
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    sendMsg.Clear();

    sendMsg.mail_id.Clear();
    sendMsg.title = title;
    sendMsg.message = message;
    sendMsg.mail_type = mailType;
    sendMsg.receiver_id.Clear();

    if(receiverID)
    {
        sendMsg.receiver_id.id_h = U64ID_HIGH(receiverID);
        sendMsg.receiver_id.id_l = U64ID_LOW(receiverID);
    }

    sendMsg.receiver_name = playerName;
    sendMsg.sender_id.id_h = U64ID_HIGH(playerID);
    sendMsg.sender_id.id_l = U64ID_LOW(playerID);
    sendMsg.sender_name = "";
    sendMsg.trade_order_id.id_h = U64ID_HIGH(tradeOrderID);
    sendMsg.trade_order_id.id_l = U64ID_LOW(tradeOrderID);
    sendMsg.send_time = now;
    sendMsg.mail_flag = MAIL_FLAG_NEW;
    sendMsg.attachment_flag = 0;
    sendMsg.attachment_list = list;

    if(condition == NULL)
    {
        sendMsg.receiver_condition = "{}";
    }
    else
    {
        sendMsg.receiver_condition = *condition;
    }

    CenterHandler::Instance()->SendByCenterID(centerID, MSG_MAIL_SEND_REQ, &sendMsg, playerID);
}

// ==================================================
// 邮件信息请求
// ==================================================
void MailManager::OnMailInfoReq()
{
    m_player->SendToGate(MSG_MAIL_INFO_ACK, &m_mail_info);
}

// ==================================================
// 操作邮件请求
// ==================================================
void MailManager::OnMailOptReq(void* data)
{
    MAIL_OPT_REQ* recvMsg = (MAIL_OPT_REQ*)data;
    MAIL_INFO* mail_info = 0;
    bool isNotify = false;

    if(!recvMsg) return;

    // 获取邮件
    mail_info = GetMail(recvMsg->mail_id.id_h, recvMsg->mail_id.id_l);
    if(!mail_info)
    {
        m_player->NotifyErrorCode(ERR_FAILED);
        return;
    }

    // 邮件类型
    recvMsg->mail_type = mail_info->mail_type;

    if(recvMsg->mail_flag == MAIL_FLAG_READED)
    {   // 设置邮件已读
        if(mail_info->mail_flag == MAIL_FLAG_NEW)
        {
            mail_info->mail_flag = MAIL_FLAG_READED;
            recvMsg->attach_flag = mail_info->attachment_flag;
            isNotify = true;
        }
    }
    else if(recvMsg->mail_flag == MAIL_FLAG_DELETED)
    {   // 设置邮件删除
        if(mail_info->mail_flag < MAIL_FLAG_DELETED)
        {
            mail_info->mail_flag = MAIL_FLAG_DELETED;
            recvMsg->attach_flag = mail_info->attachment_flag;
            DelMail(mail_info->mail_type, mail_info->mail_id.id_h, mail_info->mail_id.id_l);
            isNotify = true;
        }
    }

    if(isNotify)
    {
        CenterHandler::Instance()->SendByPlayer(m_player, MSG_MAIL_OPT_REQ, recvMsg, m_player->GetPlayerID());
    }
}

// ==================================================
// 获取邮件附件请求
// ==================================================
void MailManager::OnMailGetAttachReq(void* data)
{
    MAIL_GET_ATTACH* recvMsg = (MAIL_GET_ATTACH*)data;
    MAIL_OPT_REQ mail_opt_req;
    MAIL_INFO* mail_info = NULL;

    if(!recvMsg) return;

    // 根据ID获取邮件
    mail_info = GetMail(recvMsg->mail_id_h, recvMsg->mail_id_l);
    if(!mail_info)
    {
        m_player->NotifyErrorCode(ERR_FAILED);
        return;
    }

    // 检查附件是否已被获取
    if(mail_info->attachment_flag)
    {
        m_player->NotifyErrorCode(ERR_FAILED);
        return;
    }

    REWARD_TABLE reward;
    reward.reason = 0;
    reward.reason_arg = 0;
    reward.items = mail_info->attachment_list;

    ERROR_CODE errCode = m_player->GetItemManager()->AddRewardTable(&reward);

    if(errCode.errcode != ERR_SUCCEED)
    {
        m_player->NotifyErrorCode(errCode.errcode);
        return;
    }

    // 设置邮件附件标记
    mail_info->attachment_flag = MAIL_ATTACH_FLAG_TAKE;

    // 邮件操作请求
   /* mail_opt_req.mail_id.id_h = mail_info->mail_id.id_h;
    mail_opt_req.mail_id.id_l = mail_info->mail_id.id_l;
    mail_opt_req.mail_type = mail_info->mail_type;
    mail_opt_req.mail_flag = mail_info->mail_flag;
    mail_opt_req.attach_flag = mail_info->attachment_flag;*/

    // 发送给客户端获取附件应答
    m_player->SendToGate(MSG_MAIL_GET_ATTACH_ACK, recvMsg);

    // 发送给中心服务器邮件操作信息
    CenterHandler::Instance()->SendByCenterID(m_player->GetCenterSvrID(), MSG_MAIL_GET_ATTACH_REQ, recvMsg, m_player->GetPlayerID());
}

// ==================================================
// 发送邮件请求
// ==================================================
void MailManager::OnMailSendReq(void* data)
{
    MAIL_SEND_REQ* recvMsg = (MAIL_SEND_REQ*) data;
    ERROR_CODE sendMsg;
    uint32 mailID = 0;
    uint32 now = 0;

    if(!recvMsg) return;

    now = (uint32)Crown::SDNow().GetTimeValue();

    if(m_last_send_time >= now) return;
    m_last_send_time = now;

    // 强制修正由客户端发送过来的邮件数据
    recvMsg->mail_id.Clear();
    recvMsg->receiver_id.Clear();
    recvMsg->sender_id.id_h = U64ID_HIGH(m_player->GetPlayerID());
    recvMsg->sender_id.id_l = U64ID_LOW(m_player->GetPlayerID());
    recvMsg->sender_name = m_player->GetName();
    recvMsg->send_time = now;
    recvMsg->mail_flag = MAIL_FLAG_NEW;
    recvMsg->attachment_flag = 0;

    // 只有GM发送的系统邮件才能发附件
    if((m_player->GetRightFlag() & RIGHT_FLAG_GM) == RIGHT_FLAG_GM)
    {
        // 如果是用户邮件则不能发送附件
        if(recvMsg->mail_type == MAIL_TYPE_PLAYER)
        {
            // 强制将附件清空
            recvMsg->attachment_list.clear();
        }
    }
    else
    {
        // 非GM用户只能发玩家邮件
        if(recvMsg->mail_type != MAIL_TYPE_PLAYER)
        {
            sendMsg.errcode = ERR_FAILED;
            m_player->SendToGate(MSG_MAIL_SEND_ACK, &sendMsg);
            return;
        }
        // 强制将附件清空
        recvMsg->attachment_list.clear();
    }

    m_player->SendToCenter(MSG_MAIL_SEND_REQ, recvMsg);
}

// ==================================================
// 获取邮件
// ==================================================
MAIL_INFO* MailManager::GetMail(uint32 mail_id_h, uint32 mail_id_l)
{
    for (std::vector<MAIL_INFO>::iterator it = m_mail_info.player_mail_list.begin(); 
        it != m_mail_info.player_mail_list.end(); ++it)
    {
        if((it->mail_id.id_h == mail_id_h) && (it->mail_id.id_l == mail_id_l))
        {
            return &(*it);
        }
    }

    for (std::vector<MAIL_INFO>::iterator it = m_mail_info.system_mail_list.begin(); 
        it != m_mail_info.system_mail_list.end(); ++it)
    {
        if((it->mail_id.id_h == mail_id_h) && (it->mail_id.id_l == mail_id_l))
        {
            return &(*it);
        }
    }

    return 0;
}

void MailManager::SendMessageMail(uint32 centerID, uint64 playerID, std::string playerName, const std::vector<REWARD_ITEM>& list, uint8 messageType, int32 stringNum, ...)
{
    std::string message = "";
    message += Crown::ToString(messageType);
    va_list marker;
    va_start(marker, stringNum);
    for(int i = 0; i < stringNum; ++i)
    {
        message += "|";
        message += va_arg(marker, char*);
    }
    va_end(marker);

    SendMail(centerID, playerID, playerName, "", message, MAIL_TYPE_PLAYER_SYS, list);
}

void MailManager::OnMailOpenSystemReq( void* data )
{
    ERROR_CODE* recvMsg = (ERROR_CODE*) data;

    if(!recvMsg) return;

    uint32 mailID = recvMsg->errcode;
    std::vector<REWARD_ITEM> list;
        //SendMail(m_player->GetCenterSvrID(), m_player->GetPlayerID(), "", MailTitleConfig::Instance()->GetMailTitle(mailID), MailTitleConfig::Instance()->GetMailMessage(mailID), MAIL_TYPE_PLAYER_SYS, list, m_player->GetPlayerID());
    SendMail(m_player->GetCenterSvrID(), m_player->GetPlayerID(), "", "", Crown::ToString(mailID) , MAIL_TYPE_PLAYER_SYS, list, m_player->GetPlayerID());
}
