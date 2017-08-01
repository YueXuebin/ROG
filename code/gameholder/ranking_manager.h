/*
 *	排行榜管理
 */
#ifndef ranking_manager_h__
#define ranking_manager_h__

class Player;

class RankingManager
{
public:
    RankingManager();
    ~RankingManager();

    DECLARE_SINGLETON(RankingManager)

    void OnRankingRecv(uint64 playerID, uint32 msgID, void* data);

    void GMRefreshRanking();

    std::vector<TOP_RANK_INFO>* GetTopPlayerLevelRankList(); // 获得玩家等级排行榜的Top榜

private:
    void OnRankingInfoReq(void* data, uint64 playerID);                     // 排行榜信息请求

    void OnRankingRankAck(void* data, uint64 playerID);                     // 得到排名应答
    void OnRankSeasonPlayerDoorInfo(RANKING_RANK_ACK* rankAck, uint64 playerID);                // 离线战斗需求排名
    void OnRankSeasonOldOfflineInfo(RANKING_RANK_ACK* rankAck, uint64 playerID);                // 离线战斗旧的排名
    void OnRankTitleInfo(uint64 playerID, RANKING_RANK_ACK* recvMsg);                           // 称号排名处理

    void OnRankBattlehall(uint64 playerID, RANKING_RANK_ACK* recvMsg);                          // 跨服排行

    void OnRankingPlayerIDAck(void* data, uint64 playerID);                 // 得到玩家应答
    void OnPlayerIDSeasonPlayerDoorInfo(RANKING_PLAYER_ID_ACK* playerIDAck, uint64 playerID);   // 离线战斗需求玩家信息

    void OnRankingChangeAck(void* data, uint64 playerID);                   // 改变玩家排名应答

    //void RankingInfoReq(int32 page, uint64 playerID, uint8 type);           // 离线玩家排行榜信息
    void RankingInfoReq(uint64 playerID, uint8 type);           // 离线玩家排行榜信息

    void OnRankAshuraTitle(RANKING_RANK_ACK* rankAck, uint64 playerID);     // 修罗场排名称号

    void OnRecvRankDailyRefresh(uint32 msgID, void* data);                  // 排行榜排名结束通知

    void OnRecvRankHoursRefresh();                                // 排行榜每小时刷新

    void OnSendRankEvent(Player* player, RANKING_RANK_ACK* recvMsg, int32 type);    // 发送排名事件

    void OnRankingTopPlayerLevelRankNtf(void* data);         // 玩家等级排行榜Top榜

private:
    RANKING_TOP_RANK_NTF m_top_player_level_rank_list;
};

#endif // ranking_manager_h__
