#ifndef mail_job_h__
#define mail_job_h__

class DBExecutor;
class DBRecorder;

class Player;


class MailJob
{
    MailJob();
    ~MailJob();

    DECLARE_SINGLETON(MailJob)

    void InitKeywordMap();

    /// 设置SQL执行
    void SetDBExecutor(DBExecutor* val) { m_dbExecutor = val; }

    //bool SaveMailData(Player* player);                ///< 保存邮件数据
    bool SavePlayerMailData(Player* player);            ///< 保存个人邮件数据

    bool SaveSystemMailData(Player* player);            ///< 保存系统邮件数据

    void SearchMailReceiver(uint64 playerID, MAIL_SEND_REQ* mail_send_Req);         // 发送邮件搜索接收者的角色ID

    void SendToPlayerMailBox(uint64 playerID, MAIL_SEND_REQ* mail_send_Req);        // 发送到角色邮箱

    void SendToSystemMailBox(uint64 playerID, MAIL_SEND_REQ* mail_send_Req);        // 发送到系统邮箱

    void SendToGroupMailBox(uint64 playerID, MAIL_SEND_REQ* mail_send_Req);         // 发送到组邮箱

    void SendGroupMailToCreatePlayer(MAIL_SEND_REQ* mail_send_Req, std::string& condition);   // 发送组邮件给当前已创建角色

    void SendGroupMailToGuildMember(MAIL_SEND_REQ* mail_send_Req, std::string& condition, std::vector<std::string>& parameter); // 发送组邮件给公会成员

    void SetPlayerMailFlag(uint64 playerID, uint64 mailID, uint8 mailFlag);                     // 设置个人邮件标记

    void SetPlayerMailAccach(uint64 playerID, uint64 mailID);                                   // 设置个人邮件获取附件

    void SetSystemMailFlag(uint64 playerID, uint64 mailID, uint8 mailFlag, uint8 attachFlag);   // 设置系统邮件标记

    void SetSystemMailAttach(uint64 playerID, uint64 mailID);                                   // 设置系统邮件获取附件

    void SetGroupMailFlag(uint64 playerID, uint64 mailID, uint8 mailFlag, uint8 attachFlag);    // 设置组邮件标记

    void SetGroupMailAttach(uint64 playerID, uint64 mailID);                                    // 设置组邮件获取附件

private:
    MailJob( const MailJob& );
    MailJob& operator=( const MailJob& );
    MailJob* operator&();
    const MailJob* operator&() const;

    void CBSearchMailReceiverName(DBRecorder& res, uint64 playerID, MAIL_SEND_REQ mail_send_req);       ///< 发送邮件搜索接收者的角色名称

    void CBSearchMailReceiverID(DBRecorder& res, uint64 playerID, MAIL_SEND_REQ mail_send_req);         ///< 发送邮件搜索接收者的角色ID

    void CBSendToPlayerMailBox(DBRecorder& res, uint64 playerID, MAIL_SEND_REQ mail_send_req);          ///< 发送到个人邮箱回调

    void CBSendToSystemMailBox(DBRecorder& res, uint64 playerID, MAIL_SEND_REQ mail_send_req);          ///< 发送到系统邮箱回调

    void CBSendToGroupMailBox(DBRecorder& res, uint64 playerID, MAIL_SEND_REQ mail_send_req);           ///< 发送到组邮箱回调

    void CBSendGroupMailToCreatePlayer(DBRecorder& res, MAIL_SEND_REQ mail_send_req, std::string condition);        ///< 发送组邮件到当前角色

    void CBSendGroupMailToCreatePlayerByCondition(DBRecorder& res, MAIL_INFO mail_info);                ///< 发送组邮件到角色列表

    void GetGuildMemberForGroupMail(DBRecorder& res, MAIL_SEND_REQ mail_send_req, std::string guildName, std::string condition);    ///< 发送组偶见到当前公会成员

    void NotifySendGroupMailToGuildMember(DBRecorder& res, MAIL_INFO mail_info);                        ///< 通知公会成员组邮件

    void FillMailInfo(MAIL_SEND_REQ& mail_send_req, MAIL_INFO& mail_info);                              ///< 填充邮件信息

    void CBSetPlayerMailFlag(DBRecorder& res);                                                          ///< 设置个人邮件标记

    void CBSetSystemMailFlag(DBRecorder& res);                                                          ///< 设置系统邮件标记

    void CBSetGroupMailFlag(DBRecorder& res);                                                           ///< 设置组邮件标记

    bool ParseGroupMailCondition(int32 type, Json::Value& json_val, std::string& condition, std::vector<std::string>& parameter);   ///< 解析组邮件的判断条件

    bool CheckConditionOp(std::string op);                                                              ///< 检查条件符号

    typedef std::map<std::string, std::string> CondWordMap;                                             ///< 关键字映射表
    typedef std::map<int32, CondWordMap> MailTypeKeywordMap;                                            ///< 件类型关键字映射表

    MailTypeKeywordMap      m_mailTypeKeywordMap;                                                       ///< 关键字映射表（根据类型能确定表名，然后能确定关键字）
    std::set<std::string>   m_mailConditionOpSet;                                                       ///< 邮件条件操作符集合
    DBExecutor*             m_dbExecutor;                                                               ///< SQL执行器
};

#endif
