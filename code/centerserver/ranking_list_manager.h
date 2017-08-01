/*
 *	排行榜
 */
#ifndef ranking_list_h__
#define ranking_list_h__

class RankQuery;

class RankListManager
{
public:
    RankListManager();
    ~RankListManager();

    void Clear();

    void Init();

    void Update();

    void DailyRefresh();

    DECLARE_SINGLETON(RankListManager)

    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);   // 接收网络消息

    void OnRankingRankReq(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);                 // 得到排名请求
    void SendRankInfo(uint64 playerID, RANKING_RANK_ACK& rankingRankAck);      // 发送玩家信息列表

    void OnRankingPlayerIDReq(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);             // 得到玩家ID
    void SendPlayerInfo(uint64 playerID, uint32 sessionID, std::map<uint64, int32> playerRankList, uint8 rank_type, uint8 reason);    // 发送玩家信息列表

    void SendRankInfoToGate(uint64 playerID, RANKING_ACK rankingAck);

    void SendTopRankToGameSvr(uint32 sessionID);                     // 通知单个GameServer Top 榜
    void SendTopRankToAll();                                         // 通知所有GameServer Top 榜
    void SendRankHourFresh();                                        // 每小时排行榜刷新;

    void OnQueryTopPlayerLevelRankAck(RANKING_TOP_RANK_NTF rankingRankAck);                             // 收到数据库查询结果

private:
    void OnChangeTwoPlayerRanking(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);     // 请求切换两个人位置
    void OnRankingInfoReq(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);             // 请求单页排行榜信息

    void OnPlayerLevelRankRefresh();                                                                // 数据库同步后排行榜信息刷新
    void OnTopPlayerLevelRankReq(uint32 count);                                                     // 查询玩家等级排行榜前count名玩家

    void OnTopUpRankQueryCreate(uint64 playerID, uint32 sessionID, uint32 msgID, void *data);       // 创建充值排行请求

    void GmActivityRankReq();                                                                       // GM 活动排行请求

private:
    RANKING_TOP_RANK_NTF m_top_player_level_rank;                                                       // 玩家等级top榜

    typedef std::map<int32, RankQuery*> RankQueryRegister;                                              // 排行查询注册列表

    RankQueryRegister m_rankQueryRegister;
};



#endif // ranking_list_h__
