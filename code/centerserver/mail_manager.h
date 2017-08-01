/*
*   邮件管理器
*/

#ifndef mail_manager_h__
#define mail_manager_h__

#include "message_encoder.h"

class Player;

class MailManager
{
public:
    MailManager();
    ~MailManager();

    DECLARE_SINGLETON(MailManager)

    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);           // 接收网络消息

    static void SendMail(uint64 playerID, const std::string& receiverName, const std::string& title, const std::string& message, uint8 mailType,     // 发送邮件
                         const std::vector<REWARD_ITEM> list, uint64 tradeOrderID = 0, const std::string& condition = "");

    // 通知活动邮件 
    static void SendMessageMail(uint64 playerID, const std::string& receiverName, const std::vector<REWARD_ITEM>& list, uint32 message_define, int32 stringNum, ...);    // (发送者ID, 接收者名字, 附件列表, 活动号, 参数列表)

private:

    void RecvSendMailReq(uint64 playerID, uint32 sessionID, void* data);        // 接收发送邮件请求

    void RecvMailOptReq(uint64 playerID, uint32 sessionID, void* data);         // 邮件操作请求

    void RecvGetAttachReq(uint64 playerID, uint32 sessionID, void* data);       // 获取邮件附件

    void RecvDelExpiredMailNtf(uint64 playerID, uint32 sessionID, void* data);  // 删除过期邮件

    uint64 GenMailID();                                                         // 生成邮件ID
};


#endif // team_manager_h__
