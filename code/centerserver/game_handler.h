#ifndef GAME_HANDLER_H
#define GAME_HANDLER_H

class Player;
class AFTransferServerProxy;


struct GameConnecter
{
    uint32  regionID;           // regionID
    uint32  sessionID;          // sessionID
    bool    is_open;            // 服务器未封闭

    GameConnecter()
    {
        regionID = 0;
        sessionID = 0;
        is_open = true;
    }
};

class GameHandler
{
    friend class PlayerNetHandler;
public:
    GameHandler();
    ~GameHandler();

    DECLARE_SINGLETON(GameHandler)

    uint32 GetRegionIDBySessionID(uint32 sessionID);

    bool Init(const char* ip, uint32 port);

    bool IsConnectGame(uint32 centGameID);
    bool IsValidGame(uint32 centGameID);

    void RegistRegionID(uint32 centGameID, uint32 sessionID);
    void UnregistRegionID(uint32 sessionID);

    int32 SendByID(uint32 regionID, uint32 msgID, void* data, uint64 playerID = 0);
    void SendToAll(uint32 msgID, void* data, uint64 playerID = 0);
    void SendOneServer(uint32 msgID, void* data, uint64 playerID = 0);
    void SendToAllBut(uint32 regionID, uint32 msgID, void* data, uint64 playerID = 0);  // 向除指定serverId外的所有server发送

    GameConnecter* FindConnecterByRegionID(uint32 regionID);
    GameConnecter* FindConnecterBySessionID(uint32 sessionID);

    // 接收网络消息
    static void RecvMsg(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata);

    void SendDeliverPlayerInfoReq(uint32 regionID, Player* player);           // 向服务器投递角色信息

    void SendEnterDungeonReq(uint32 centGameID, uint64 playerID, const ENTER_DUNGEON_REQ& enterDungeonData);
    void SendExitDungeonReq(uint32 regionID, uint64 playerID);

    void SendChangePlayerNameAck(uint32 regionID, uint64 playerID, uint16 errCode, std::string name);

    // 各GameServer人数
    void AddPlayerToRegion(Player* player, uint32 regionID);
    void RemovePlayerFromRegion(Player* player, uint32 regionID);

    uint32 AskJumpCentGameServerID(Player* player, uint32 dungeonID);   // 判断该玩家应该去哪个GameServer

private:
    void OnAccept(uint32 sessionID, void* data);
    void OnConnect(uint32 sessionID, void* data);
    void OnClose(uint32 sessionID, void* data);
    void OnError(uint32 sessionID, void* data);
    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata);

    bool IsSealGame(uint32 centGameID);

public:
    // 登陆时候读取数据的流程
    void RecvDeliverPlayerFirstInfoAck(uint64 playerID, uint32 sessionID, void* data);

    void SendWriteBackPlayerInfoAck(uint32 regionID, uint64 playerID);

    // 退出游戏服务器的同步数据流程
    void RecvWriteBackPlayerInfoReq(uint64 playerID, uint32 sessionID, void* data);

    // 自动回存玩家数据的请求
    void RecvAutoSavePlayerInfoReq(uint64 playerID, uint32 sessionID, void* data);

    // 收到角色切换副本的请求
    void RecvChangeDungeon(uint64 playerID, uint32 sessionID, void* data);

    // 收到封闭服务器请求
    void RecvSealServer(uint32 sessionID, void* data);

    // 收到踢出玩家的请求
    void RecvKickOutPlayer(uint64 playerID, uint32 sessionID, void* data);

    // 收到请求修改角色名称
    void RecvChangePlayerNameReq(uint64 playerID, uint32 sessionID, void* data);

    // 请求离线玩家的数据
    void RecvRequestOfflinePlayerReq(uint32 sessionID, void* data);

    // 请求添加好友
    void RecvFriendAddReq(uint64 playerID, uint32 sessionID, void* data);

    // 确认添加好友
    void RecvFridentAddAns(uint64 playerID, uint32 sessionID, void* data);

    // 请求删除好友
    void RecvFriendDelReq(uint64 playerID, uint32 sessionID, void* data);

    // 请求快速添加好友
    void RecvFriendQuickAddReq(uint64 playerID, uint32 sessionID, void* data);

    // 请求聊天
    void RecvChatReq(uint64 playerID, uint32 sessionID, void* data);

    // 保存聊天数据到DB
    void SaveChatToDB(Player* player, int channel, const std::string& chat);

    // 警示公告
    void RecvProclamationReq(uint64 playerID, uint32 sessionID, void* data);

    // 行为log
    void RecvBehaviorLog(uint64 playerID, uint32 sessionID, void* data);

    // dongyi log
    void RecvLog(uint64 playerID, uint32 sessionID, void* data);

    //修改权限标识
    void RecvModifyRightFlag(uint64 playerID, uint32 sessionID, void* data);

    // 请求存档日常投资
    void SaveDayInvestmentReq(uint64 playerID, uint32 sessionID, void* data);

    // 请求存档等级投资
    void SaveLevelInvestmentReq(uint64 playerID, uint32 sessionID, void* data);

    // 请求存档等级活动
    void SaveLevelActivityReq(uint64 playerID, uint32 sessionID, void* data);

    // 投资操作通告
    void InvestmentOperationNTF(uint64 playerID, uint32 sessionID, void* data);

    // 请求投资操作
    void InvestmentOperationReq(uint64 playerID, uint32 sessionID, void* data);

    /// 接收公会BOSS排名信息
    void RecvGuildCrusadeBossRankSave(uint64 playerID, uint32 msgID, void* data);

    /// 请求公会BOSS排名信息
    void RecvGuildCrusadeBossRankReq(uint64 playerID, uint32 msgID, void* data);

    /// 接收暮光之战排名信息
    void RecvGuildTwilightWarRankSave(uint64 playerID, uint32 msgID, void* data);

    /// 请求暮光之战排名信息
    void RecvGuildTwilightwarRankReq(uint64 playerID, uint32 msgID, void* data);

    /// 一上线计算玩家属性通知
    void TeamMemberPropNotify(uint64 playerID, void* data);

    /// 360推送信息通知
    void RecvPush360Ntf(uint64 playerID, void* data);

    // 收到请求验证修改名称合法
    void RecvIsUpdatePlayerNameReq(uint64 playerID, uint32 sessionID, void* data);

private:
    AFTransferServerProxy* m_GameServerProxy;

    typedef std::map<uint32, GameConnecter> GameConnecterMap;
    GameConnecterMap m_centGameID2snID;      // <CenterID+GameID, GameConnecter>

    std::map<uint32, int32> m_GamePlayerNumMap;   // <CenterID+GameID, playernum> 各GameServer上的玩家人数

};

#endif
