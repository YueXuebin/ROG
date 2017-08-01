/*
 *	角色对象的网络事件处理器
 */
#ifndef player_net_handler_h__
#define player_net_handler_h__

class Player;

class PlayerNetHandler
{
public:
    PlayerNetHandler(Player* player);
    ~PlayerNetHandler();

    void    Update();

    void    OnVerify();         // 角色验证完毕
    void    OnLoadStart();      // 开始从数据库加载角色数据
    void    OnLoadEnd();        // 数据库加载完毕
    void    OnLogout();         // 登出

    void    OnLoginRepeat();    // 重复登陆处理
    void    KickOut(uint32 error);

    // from gate server
    void    RecvSetPlayerIDAck(SET_PLAYER_ID_ACK* data);
    void    RecvSetGameSvrIDAck(SET_GAMESVR_ID_ACK* data);

    // from game server
    void    RecvDeliverPlayerInfoAck(PLAYER_BASE_INFO_ACK* data);
    void    RecvWriteBackPlayerInfoReq(WRITEBACK_PLAYER_INFO_REQ* recvMsg);
    void    RecvAutoSavePlayerInfoReq(WRITEBACK_PLAYER_INFO_REQ* recvMsg);
    void    RecvChangeDungeon(CHANGE_DUNGEON_REQ* recvMsg);

private:
    void    FillPlayerInfo(const WRITEBACK_PLAYER_INFO_REQ* recvMsg);

    bool    HasGSPlayer();

    void    EnterDungeonReq(uint32 centGameID, const ENTER_DUNGEON_REQ& enterDungeonData);
    void    ExitDungeonReq();
    void    NoGSKickOut();

private:
    Player*     m_player;
    bool        m_pendingLogout;        // 等待跳GS流程结束后再执行logout处理标记

    // 调试用
    bool        m_isEnterDungeon;

};

#endif // player_net_handler_h__
