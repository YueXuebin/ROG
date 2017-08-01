#ifndef GAME_MANAGER_TOOL_H
#define GAME_MANAGER_TOOL_H

class GmtEvent;
class GmtHttpAsync;

// ==================================================
// 昆仑登陆执行器
// ==================================================
class GmtExecutor
{
public:
    GmtExecutor();
    ~GmtExecutor();

    DECLARE_SINGLETON(GmtExecutor)

    bool            Init(const char* addrIP, uint32 addrPort);
    void            Unint();

    void            Update();

    GmtEvent*       QueryPopFront();
    void            QueryPushBack(GmtEvent* payEvent);

    GmtEvent*       CallbackPopFront();
    void            CallbackPushBack(GmtEvent* payEvent);

    void            OnGmtAskAck(void* data);
    void            OnGmtSetAck(void *data);

    void            OnGmtAskCrossCenterIpListAck(void* data);
private:

    // 解析 GM 命令
    void            ParseGmtCmd(std::string& cmdstr);        /// 解析 GMT 命令
    // 各GM命令的处理(注意，必须有响应)==============================================================
    // 查询pcu
    bool            GmtPcu(Json::Value& cmdjson, Json::Value& ackjson);
    // 发送公告信息
    bool            GmtMarquee(Json::Value& cmdjson, Json::Value& ackjson);
    // 发送邮件信息
    bool            GmtSendMail(Json::Value& cmdjson, Json::Value& ackjson);
    // 发送禁言信息
    bool            GmtChat(Json::Value& cmdjson, Json::Value& ackjson);
    // 发送禁言信息
    bool            GmtKickPlayer(Json::Value& cmdjson, Json::Value& ackjson);
    // 发送节日数据同步消息
    bool            GmtSynchronousFestivel(Json::Value& cmdjson, Json::Value& ackjson);
    // 发送警示公告信息
    bool            GmtProclamation(Json::Value& cmdjson, Json::Value& ackjson);
    // 游戏参数设置
    bool            GmtGameSetting(Json::Value& cmdjson, Json::Value& ackjson);
    // 公会公告设置
    bool            GmtGuildBulletin(Json::Value& cmdjson, Json::Value& ackjson);
    // 公会职位设置
    bool            GmtGuildOffice(Json::Value& cmdjson, Json::Value& ackjson);
    // 加载公会信息(以确保其他gmt公会操作可执行)
    bool            GmtGuildInfo(Json::Value& cmdjson, Json::Value& ackjson);
    // 游戏参数设置
    void            OnGmtGameSetting(GMT_MSG* data);
    // 游戏参数查询(异步)
    void            GmtGameAsking(Json::Value& cmdjson);
    //void            OnGmtGameAsking(GMT_MSG* data);
    void            OnGmtGameAsking(GMT_MSG_SET_GAME_LIST* data);
    // 公告信息请求
    bool            GmtAnnouceAsk(Json::Value& ackjson);
    // 删除指定的公告
    bool            GmtDeleteAnnouceAsk(Json::Value& cmdjson);
    // IP禁言
    bool            GmtAskChatBlockIP(Json::Value& cmdjson, Json::Value& ackjson);
    bool            GmtAddChatBlockIP(Json::Value& cmdjson, Json::Value& ackjson);
    bool            GmtRemoveChatBlockIP(Json::Value& cmdjson, Json::Value& ackjson);
    // 跨服设置
    bool            GmtAddCrossCenterIP(Json::Value& cmdjson, Json::Value& ackjson);
    bool            GmtRemoveCrossCenterIP(Json::Value& cmdjson, Json::Value& ackjson);
    void            GmtAskCrossCenterIP(Json::Value& cmdjson);  //(异步)

    // 同步商城数据
    bool            GmtSynchronounMallInfo(Json::Value& cmdjson, Json::Value& ackjson);

    // 清限购数据
    bool            GmtClearLimitBuyInfo(Json::Value& cmdjson, Json::Value& ackjson);

    // 查询服务器运行状态
    bool            GmtAskServerRunningState(Json::Value& cmdjson, Json::Value& ackjson);

    // 发送设置数据同步消息
    bool            GmtSynchronousGameSetting(Json::Value& cmdjson, Json::Value& ackjson);
    ///////////////////////////////////////////////////////////////////////////////////////
    // http响应
    void            PushGmtCmdResult(const std::string& ack_string, bool retOK = true);
    void            PushGmtCmdResultInfo(Json::Value& ackjson, bool retOK = true);

    // 查询列表锁
    void            QueryLock() { m_queryLock.Lock(); }
    void            QueryUnLock() { m_queryLock.Unlock(); }

    // 结果列表锁
    void            CallbackLock() { m_callbackLock.Lock(); }
    void            CallbackUnLock() { m_callbackLock.Unlock(); }
    
    // FB点赞奖励请求
    bool            GmtLikeInfo(Json::Value& cmdjson, Json::Value& ackjson);

    // 删除物品
    bool            GmtSubtractItem(Json::Value& cmdjson, Json::Value& ackjson);

    enum
    {
        MAX_SEMAPHORE_NUM = 65535
    };

    Crown::CSDMutex                 m_queryLock;
    Crown::CSDMutex                 m_callbackLock;

    Crown::CSDNamedSemaphore*       m_GmtSema;

    GmtHttpAsync*                   m_gmtAsync;

    std::list<GmtEvent*>            m_queryList;
    std::list<GmtEvent*>            m_callbackList;

    int32                           m_CheckCounter;     // 执行计数(调试用)，确保接受一条cmd，响应一条cmd。
};

#endif
