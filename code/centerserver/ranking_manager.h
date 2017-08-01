/*
 *	排行榜
 */
#ifndef ranking_manager_h__
#define ranking_manager_h__


class RankManager
{
public:
    RankManager();
    ~RankManager();

    void Clear();

    DECLARE_SINGLETON(RankManager)

    void Init();

    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);   ///< 接收网络消息

    void Update();

    void SaveRanking(uint32 rankTime);                  // 保存所有排行榜信息
    void OnRefreshRanking(uint32 msgID, void* data);    // 重新加载排行榜信息

private:
    void SaveAllFirstOfRanking();                       // 保存排行榜所有第一名信息
    void SaveGamepointRanking(uint32 rankTime);         // 保存游戏币榜
    void SavePlayerLevelRanking(uint32 rankTime);       // 保存等级榜
    void SaveGuildRanking(uint32 rankTime);             // 保存公会榜
    void SaveWingRanking(uint32 rankTime);              // 保存翅膀排行榜
    void SavePlotDungonScoreRanking(uint32 rankTime);   // 剧情副本分数排行榜
    void SaveAchiveCountRanking(uint32 rankTime);       // 成就个数排行榜

    uint32 m_quarterHourRefreshTime;                    // 15分钟刷新时间
    uint32 m_hourRefreshTime;                           // 每小时刷新时间
    uint32 m_dailyRefreshTime;                          // 每日刷新时间
};



#endif // ranking_manager_h__
