//------------------------------------------------------------------------------
//
//    Copyright (C) DoinGame. All rights reserved.
//
//    File Name:   mail_manager.h
//    Author:      Tomren
//    Create Time: 2013-08-16
//    Description: 
//
//    Version history:
//
//
//------------------------------------------------------------------------------

#ifndef MAIL_MANAGER_H
#define MAIL_MANAGER_H

#include "message_encoder.h"

class Player;

// ==================================================
/// 角色的邮件管理系统
// ==================================================
class MailManager
{
public:
    MailManager(Player* player);
    ~MailManager();

    void    Clear();

    void    OnRecv(uint32 msgID, void* data);

    void    LoadInfo(const MAIL_SYSTEM_INFO& info);

    Player* GetPlayer() { return m_player; }                                        ///< 获得角色指针

    void    AddMail(MAIL_INFO* mail);                                               ///< 添加邮件
    void    DelMail(uint8 mail_type, uint32 mail_id_h, uint32 mail_id_l);           ///< 删除邮件
    void    DelExpiredMail(uint64 expiredMailID);                                   ///< 删除过期邮件

    ///< 发送邮件 (发送的中心服务器ID, 发送者ID, 接收者名, 标题, 信息, 邮件类型, 物品列表, 接收者ID, 贸易定单ID)
    static void SendMail(uint32 centerID, uint64 playerID, const std::string& playerName, const std::string& title, const std::string& message, uint8 mailType, 
                            const std::vector<REWARD_ITEM>& list, uint64 receiverID = 0, uint64 tradeOrderID = 0, std::string* condition = NULL);

    // 通知活动邮件
    static void SendMessageMail(uint32 centerID, uint64 playerID, std::string playerName, const std::vector<REWARD_ITEM>& list, uint8 messageType, int32 stringNum, ...);
private:
    void    OnMailInfoReq();                                                ///< 邮件信息请求
    void    OnMailOptReq(void* data);                                       ///< 操作邮件请求
    void    OnMailGetAttachReq(void* data);                                 ///< 获取邮件附件请求
    void    OnMailSendReq(void* data);                                      ///< 发送邮件请求
    void    OnMailOpenSystemReq(void* data);                                ///< 系统开启邮件请求

    MAIL_INFO*  GetMail(uint32 mail_id_h, uint32 mail_id_l);                ///< 获取邮件
    bool   CanTakeAttach(MAIL_INFO* mail_info);                             ///< 是否能获取附件

    Player*             m_player;                                           ///< 角色指针
    MAIL_SYSTEM_INFO    m_mail_info;                                        ///< 邮件系统信息
    uint32              m_last_send_time;                                   ///< 上次发送邮件的时间戳
};

#endif
