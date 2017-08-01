/*
*	勇者争霸
*/
#ifndef player_door_manager_h__
#define player_door_manager_h__

class Player;

// ==================================================
/// 勇者争霸系统
// ==================================================
class PlayerDoorManager
{
public:
    PlayerDoorManager(Player* player);
    ~PlayerDoorManager();

    void CalDailyRefresh();   // 登录时进行数据重置

    void OnRecv(uint32 msgID, void* data);

    void ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 );

    void LoadInfo(PLAYER_DOOR_INFO& info);

    void FillPlayerDoorInfo(PLAYER_DOOR_INFO& info);

    const OFFLINE_PLAYER_INFO* GetBattleOfflinePlayerInfo();

    void SubPlayerPkCount();            // 减少勇者争霸战斗次数

    void CombatFinish(bool bWin);       // 角斗结束
    uint32 RankSwap();                  // 更改排行

    void CBBuyChallengeCount();         // 购买挑战次数回调
    void CBKillCD(uint32 needMoneyPoint);            // 购买秒CD回调

    uint32 GetRank() {return m_PlayerDoorInfo.rank;}                                        // 得到排名
    const std::string& GetOpponentName() {return m_PlayerDoorInfo.select_player.name;}      // 得到对手名字

    void OnPlayerRankByIdAck(RANKING_RANK_ACK* rankAck);
    void OnPlayerOldRankByIdAck(RANKING_RANK_ACK* rankAck);
    void OnRankingDailyRefresh();

    void OnPlayerIdByRankAck(RANKING_PLAYER_ID_ACK* playerIDAck);

    void SendTileRank(uint64 playerid);                     // 得到当前玩家的排名
    bool CardByChallengeCount(int32 num);
private:
    enum
    {
        kStateInit              = 0,                        ///< 无数据
        kStateNormal            = 1,                        ///< 有数据
        kStateRefreshPlayerReq  = 2,                        ///< 刷新勇者争霸数据中
    };

    void OnPlayerDoorInfoReq();
    void OnPlayerDoorSelectReq(void* data);

    void SaveYesterdayData();                               ///< 保存昨天数据

    void SendRefreshOfflinePlayerAck();

    void OrderPlayerList();

    void SendPlayerRankByIdReq(const std::vector<uint64>& playerIdList);    // 根据Id获取Rank
    void SendPlayerOldRankByIdReq(); // 根据Id获取旧Rank
    void SendPlayerIdByRankReq(std::vector<uint32> rankList);        // 根据Rank获取Id

    void AsyncRefreshPlayerInfoList();

    void SendMarQuee(int32 rank);             // 发送排名公告

    void OnPlayerDoorRewardReq();

    void OnPlayerDoorAddChallengeCountReq();
    void OnPlayerDoorKillCDReq();
    void OnPlayerDoorCDOverReq();
    void OnPlayerDoorRewardInfoReq();

    bool CheckVIPNoCD();    // 勇者争霸VIP免CD检查

    bool CheckReward();     // 检查排行奖励是否可以领取

    // 计算配置角色的战斗力
    uint32 CalcBattlePowerForFakePlayer(const OFFLINE_PLAYER_INFO& offlinePlayerInfo);

private:
    int32               m_state;
    Player*             m_player;

    int32               m_YesterdayRank;

    PLAYER_DOOR_INFO                m_PlayerDoorInfo;               // 勇者争霸数据
    std::vector<PLAYER_RANK_INFO>   m_RankInfos;                    // 排行信息
    std::vector<OFFLINE_PLAYER_INFO>    m_PlayerInfoList;           // 被挑战的离线玩家信息

};


#endif // player_door_manager_h__
