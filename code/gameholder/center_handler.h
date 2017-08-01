#ifndef CENTER_HANDLER_H
#define CENTER_HANDLER_H

class TransferClientProxy;
class Player;
class Scene;
class ServerInfo;
class CenterHandlerHelper;
class Dungeon;

class CenterConnecter
{
public:
    CenterConnecter() {Clear();}

    void Clear()
    {
        m_id            = 0;
        m_sessionID     = 0;
        m_centerID      = 0;
        m_centerType    = 0;
        m_state         = HANDLER_STATE_INIT;
        m_closedtime    = 0;
        m_IsSeal        = false;
        m_ip.clear();
        m_port          = 0;
        m_centerServer  = 0;
    }

    uint32                  m_id;               // 标示ID
    uint32                  m_sessionID;        // 会话 ID
    uint32                  m_centerID;         // 中心服务器 ID
    uint8                   m_centerType;       // 中心服务器类型
    uint32                  m_state;            // 网络状态
    uint32                  m_closedtime;       // 上次关闭连接的时间
    bool                    m_IsSeal;           // 是否封闭
    std::string             m_ip;               // IP 地址
    uint32                  m_port;             // 端口号
    TransferClientProxy*    m_centerServer;     // 中心服务器
};

class CenterHandler
{
    friend class GmCommander;
    friend class CenterHandlerHelper;
public:
    CenterHandler();
    ~CenterHandler();

    DECLARE_SINGLETON(CenterHandler)

    bool Init();

    void Clear();

    void Update();

    static void RecvMsg(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata);

    bool SendToAll(uint32 msgID, void* data, uint64 playerID=0);      // 暂时不需要此功能

    bool SendToMasterCenter(uint32 msgID, void* data, Player* player=NULL);
    bool SendBySessionID(uint32 sessionID, uint32 msgID, void* data, uint64 playerID=0);
    bool SendByCenterID(uint32 centerID, uint32 msgID, void* data, uint64 playerID=0);
    bool SendByID(uint32 id, uint32 msgID, void* data, uint64 playerID=0);
    bool SendByPlayer(Player* player, uint32 msgID, void* data, uint64 playerID=0);

    void AutoSavePlayerInfoReq(Player* player);                             // 自动回存角色信息

    void SafeDeleteAllCenterServer();
    void SafeDeleteCenterServerByID(uint32 id);                             // 删除与某Center的连接

    void SavePlayerDungeonPos(Dungeon* dungeon, Player* player);            // 记录角色所在副本和位置


private:
    void Update2Second();

    void OnConnect(uint32 sessionID, void* data, void* userdata);
    void OnClose(uint32 sessionID, void* data, void* userdata);
    void OnError(uint32 sessionID, void* data, void* userdata);
    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata);

    int32 GetState() { return m_state; }
    void SetState(int32 val) { m_state = val; }

    bool _SendToCenter(CenterConnecter* pConnecter, uint32 msgID, void* data, uint64 playerID=0);       // 所有Send调用

    void OnConnectClose(uint32 sessionID, uint32 netState, void* userdata);

    void _DisconnectCenterServerByID(uint32 ID);

    void SetCenterID(uint32 sessionID, uint32 centerID);                    // 设置中心服务器ID

    CenterConnecter* GetConnecterByID(uint32 id);                           // 获得中心服务器配置
    CenterConnecter* GetConnecterByCenterID(uint32 centerID);
    CenterConnecter* GetConnecterBySessionID(uint32 sessionID);

    bool IsMasterCenterBySessionID(uint32 sessionID);

    void SendServerInfoReq(CenterConnecter* connecter);                     // 发送请求服务器信息
    void RecvServerInfoAck(uint32 sessionID, void* data);                   // 接收服务器信息

    void SendServerActiveReq(uint32 sessionID, uint8 centerType);           // 发送请求服务器激活
    void RecvServerActiveAck(uint32 sessionID, void* data);                 // 接收服务器激活

    void SendInfoReqToWorkCenter();                                         // 给从服务器发送信息请求

    void RecvServerReadyAck(void* data);                                    // 所有服务器准备好了

    void RecvCrossServerAdd(void* data);                                    // 添加跨服CenterServer列表项
    void RecvCrossServerRemove(void* data);                                 // 删除跨服CenterServer列表项
    void RecvCrossServerAsk(uint32 sessionID, void* data);                  // 查询跨服CenterServer列表

    void RecvDeliverPlayerInfoReq(uint64 playerID, void* data);             // 收到角色的基本信息
    void SendDeliverPlayerFirstInfoAck(uint32 regionID, uint16 errCode, uint64 playerID);   // 确认角色的基本信息

    void RecvEnterDungeonReq(uint64 playerID, uint32 sessionID, void* data);        // 请求进入场景
    void RecvExitDungeonReq(uint64 playerID);                                       // 请求退出场景

    void SendWriteBackPlayerInfoReq(Player* player);                                // 和Center同步角色信息
    void RecvWriteBackPlayerFirstInfoAck(uint64 playerID);                          // 收到角色的基本信息

    void RecvSealServerAck(uint32 sessionID, void* data);                           // 封闭服务器确认

    void FillPlayerInfo(Player* player, WRITEBACK_PLAYER_INFO_REQ& sendMsg);

    bool ConnectCenterServer();
    void OnCloseCenterServer(uint32 centerID);                                      //  中心服务器关闭

    int32 FindValidID();
    
    void TellCenterSealGame(uint32 id);

private:
    enum {
        kStateInit                  = 1,
        kStateMasterInfoAck         = 2,
        kStateMasterActivateAck     = 3
    };
    typedef std::vector<CenterConnecter>    CenterConnecterList;
    CenterConnecterList                     m_centerList;                       // 中心服务器连接列表
    int32                                   m_state;                            // 状态

    std::vector<uint32>                     m_DeleteIDs;                        // 要断开的Center

    CenterHandlerHelper*                    m_CenterHelper;                     // 游戏协议处理

    int32   m_SecondTimer;
};

#endif
