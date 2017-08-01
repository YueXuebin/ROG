#ifndef GAMESERVER_H
#define GAMESERVER_H


class Player;
class DBExecutor;

class CenterServer
{
public:
    CenterServer();         // 构造
    ~CenterServer();        // 析构

    DECLARE_SINGLETON(CenterServer)

    bool Init();            // 初始化
    void UnInit();          // 销毁

    bool InitConfig();      // 初始化配置文件
    void UnInitConfig();    // 销毁配置文件

    bool InitSvrHandler();  // 初始化处理句柄
    void UnInitSvrHandler();// 销毁处理句柄

    bool InitDB();          // 初始化数据库事务
    void UnInitDB();        // 销毁数据库事务

    bool InitManager();     // 初始化管理器
    void UnInitManager();   // 销毁管理器

    bool InitLogSys();      // 初始化日志系统
    void UnInitLogSys();    // 销毁日志系统

    bool InitExtFunc();     // 初始化扩展功能
    void UnInitExtFunc();   // 销毁扩展功能

    void Run();             // 运行

    void onEnd();           // 关闭服务器前处理

    void UpdateNet();       // 更新网络
    void UpdateDB();        // 更新数据库

    uint16 ActiveServer(uint8 serverType, uint32 serverID, uint32 centerID);
    void UnactiveServer(uint8 serverType, uint32 serverID, uint32 centerID);

    // 发送网络消息
    int Send(uint32 sessionID, uint32 msgID, void* data);

    // 打印LOGO
    static void PrintLogo();

    // 设置终端处理
    static void SignalHandler(int sig);

    void RecvServerInfoReq(uint32 sessionID, void* data);
    void RecvServerActiveReq(uint32 sessionID, void* data);

    void RecvLoginGameReq(uint64 loginID, uint32 sessionID, void* data);
    void RecvCreatePlayerReq(uint64 loginID, uint32 sessionID, void* data);
    void RecvIsCreatePlayerReq(uint64 loginID, uint32 sessionID, void* data);
    void RecvAskPlayerInfoReq(uint64 loginID, uint32 sessionID, void* data);
    void RecvLoadPlayerReq(uint64 loginID, uint32 sessionID, void* data);

    void AddPlayerToCenter(uint32 gateSvrID, uint64 loginID, bool isload = true);

    DBExecutor*  GetGameDBExecutor() {return m_gameDBExecutor;}

    void SetRun(bool val) { m_isRun = val; }

private:
    enum
    {
        kInitState              = 0,    ///< 初始化状态
        kUpdateDBState          = 1     ///< 更新数据库状态
    };

    /**
    * @brief 获得服务器状态
    */
    uint32 GetState() { return m_state; }

    /**
    * @brief 设置服务器状态
    * @param[in] val : 状态值
    */
    void SetState(uint32 val) { m_state = val; }

    void SendServerInfoAck(uint32 sessionID, uint16 errCode, uint32 port, const std::string& ip, const std::string& outer_ip);
    void SendServerActiveAck(uint32 sessionID, uint16 errCode);
    void SendServerListAck(uint32 sessionID, uint8 type);

    uint32              m_state;            ///< 服务器的状态
    DBExecutor*         m_gameDBExecutor;   ///< 游戏SQL执行器
    DBExecutor*         m_gmtDBExecutor;    ///< 游戏SQL执行器
    DBExecutor*         m_globalDBExecutor;    ///< 游戏SQL执行器
    bool                m_isRun;            ///< 是否在运行

    bool                m_InitReady;        ///< 所有Server准备好了

};

#endif
