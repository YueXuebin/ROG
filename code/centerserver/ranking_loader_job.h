/*
 *	排行榜
 */
#ifndef ranking_loader_job_h__
#define ranking_loader_job_h__

class DBExecutor;
class DBRecorder;

class RankingLoaderJob
{
public:
    RankingLoaderJob() {m_dbExecutor = 0;}
    ~RankingLoaderJob() {}

    DECLARE_SINGLETON(RankingLoaderJob)

    // 设置SQL执行
    void SetDBExecutor(DBExecutor* val) {m_dbExecutor = val;}

    // 获取战力排行榜前count名
    void GetBattlePowerRanking(uint32 count, uint32 msgId);

    // 得到玩家总榜发给客户端
    void LoadPlayerListByAllFirst(uint64 playerID, std::vector<int32>& rankList);

    // 由玩家战力榜得到玩家信息列表发送给客户端
    void LoadPlayerListByBattlePowerRanking(uint64 playerID, std::vector<int32>& rankList);

    // 由玩家星空榜得到玩家信息列表发送给客户端
    void LoadPlayerListByHoroscopeRanking(uint64 playerID, std::vector<int32>& rankList);

    // 由玩家金币榜得到玩家信息列表发送给客户端
    void LoadPlayerListByGamepointRanking(uint64 playerID, std::vector<int32>& rankList);

    // 由等级榜等级排名得到玩家ID发送给客户端
    void LoadPlayerListByPlayerLevelRank(uint64 playerID);

    // 副本星数排行榜
    void LoadPlotDungonScoreRank(uint64 playerID);

    // 成就个数排行榜
    void LoadAchiveCountRank(uint64 playerID);

    // 由玩家ID得到玩家排名发给GameServer
    void LoadPlayerRankingByPlayerID(uint64 playerID, std::vector<uint64>& playerIDList, RANKING_RANK_ACK& rankingAck, const std::string rankTxt);
    // 由公会ID得到公会排名发给GameServer
    void LoadGuildRankingByGuildID(uint64 playerID, std::vector<uint64>& guildIDList, RANKING_RANK_ACK& rankingAck, const std::string rankTxt);

    // 由玩家ID得到离线玩家旧有排名
    void LoadPlayerOfflineOldRankingByPlayerID(uint64 playerID, std::vector<uint64>& playerIDList, RANKING_RANK_ACK& rankingAck, const std::string rankTxt);
    // 由玩家排名得到玩家ID发送给GameServer
    void LoadPlayerIDByPlayerOfflineRanking(uint64 playerID, uint32 sessionID, std::vector<int32>& rank, uint8 reason);
    // 切换两个玩家排名
    void ChangeTwoPlayerRanking(uint64 playerID, uint32 sessionID, uint64 player1, uint64 player2, int32 level1, int32 level2);
    // 由玩家名次得到玩家信息列表发送给客户端
    void LoadPlayerListByOfflineRanking(uint64 playerID, std::vector<int32>& rankList);

    // 由玩家名次得到玩家信息列表发送给客户端
    void LoadPlayerListByAshuraRank(uint64 playerID, std::vector<int32>& rankList);

    // 由公会榜等级排名得到玩家ID发送给客户端
    void LoadPlayerListByGuildRank(uint64 playerID, std::vector<int32>& rankList);

    // 删除公会BOSS之战的排行信息
    void DelGuildCrusadeBossRank(GUILD_DAMAGE_RANK_LIST& list);

    // 删除公会暮光之战的排行信息
    void DelGuildTwilightWarRank(GUILD_DAMAGE_RANK_LIST& list);

    // 保存公会BOSS之战的排行信息
    void SaveGuildCrusadeBossRank(GUILD_DAMAGE_RANK_LIST list);

    // 保存公会暮光之战的排行信息
    void SaveGuildTwilightWarRank(GUILD_DAMAGE_RANK_LIST& list);

    // 获取暮光之战排行信息
    void GetGuildTwilightwarRank(uint64 playerID);

    // 获得公会BOSS排行
    void GetGuildCrusadeBossRank(uint64 playerID);

    // 获取玩家等级排行榜前count名
    void GetTopPlayerLevelRank(uint32 count);

    // 由玩家翅膀排行榜得到玩家信息列表发送给客户端
    void LoadPlayerListByWingRanking(uint64 playerID, std::vector<int32>& rankList);

    // 由玩家宠物排行榜得到玩家信息列表发送给客户端
    void LoadPlayerListByPetRanking(uint64 playerID, std::vector<int32>& rankList);

    // 由玩家坐骑排行榜得到玩家信息列表发送给客户端
    void LoadPlayerListByRideRanking(uint64 playerID, std::vector<int32>& rankList);

    // 由玩家化身排行榜得到玩家信息列表发送给客户端
    void LoadPlayerListByFashionRanking(uint64 playerID, std::vector<int32>& rankList);

private:
    RankingLoaderJob( const RankingLoaderJob& );
    RankingLoaderJob& operator=( const RankingLoaderJob& );
    RankingLoaderJob* operator&();
    const RankingLoaderJob* operator&() const;

    void LoadMaxPage(uint64 playerID, RANKING_ACK& rankingAck, const std::string rankTxt);
    void CBLoadMaxPage(DBRecorder& res, uint64 playerID, RANKING_ACK rankingAck, const std::string rankTxt);
    void LoadMyRanking(uint64 playerID, RANKING_ACK rankingAck, const std::string rankTxt);
    void CBLoadMyRanking(DBRecorder& res, uint64 playerID, RANKING_ACK rankingAck, const std::string rankTxt);
    void LoadRankingTime(uint64 playerID, RANKING_ACK rankingAck);
    void CBLoadRankingTime(DBRecorder& res, uint64 playerID, RANKING_ACK rankingAck);

    void LoadGuildMaxPage(uint64 playerID, RANKING_ACK& rankingAck, const std::string rankTxt);
    void CBLoadGuildMaxPage(DBRecorder& res, uint64 playerID, RANKING_ACK rankingAck, const std::string rankTxt);
    void LoadMyGuildRanking(uint64 playerID, RANKING_ACK& rankingAck, const std::string rankTxt);
    void CBLoadMyGuildRanking(DBRecorder& res, uint64 playerID, RANKING_ACK rankingAck, const std::string rankTxt);
    //void LoadGuildRefreshTime(uint64 playerID, RANKING_ACK& rankingAck, const std::string rankTxt);
    //void CBLoadGuildRefreshTime(DBRecorder& res, uint64 playerID, RANKING_ACK rankingAck, const std::string rankTxt);

    // 得到玩家总榜发给客户端
    void CBLoadPlayerListByAllFirst(DBRecorder& res, uint64 playerID);

    // 获取战力排行榜前count名回调
    void CBGetBattlePowerRanking(DBRecorder& res, uint32 msgId);

    // 由玩家战力榜排名得到玩家ID发送给客户端
    void CBLoadPlayerListByBattlePowerRanking(DBRecorder& res, uint64 playerID);

    // 由玩家星空图榜排名得到玩家ID发送给客户端
    void CBLoadPlayerListByHoroscopeRanking(DBRecorder& res, uint64 playerID);

    // 由玩家游戏币榜排名得到玩家ID发送给客户端
    void CBLoadPlayerListByGamepointRanking(DBRecorder& res, uint64 playerID);

    // 由玩家等级榜排名得到玩家ID发送给客户端
    void CBLoadPlayerListByPlayerLevelRanking(DBRecorder& res, uint64 playerID);

    // 由玩家ID得到玩家排名发给GameServer
    void CBLoadPlayerRankingByPlayerID(DBRecorder& res, uint64 playerID, RANKING_RANK_ACK rankingAck, const std::string rankTxt);

    void CBLoadPlayerOfflineOldRankingByPlayerID(DBRecorder& res, uint64 playerID, RANKING_RANK_ACK rankingAck, const std::string rankTxt);
    void CBLoadPlayerIDByPlayerOfflineRanking(DBRecorder& res, uint64 playerID, uint32 sessionID, uint8 reason);
    void CBChangeTwoPlayerRanking(DBRecorder& res, uint64 playerID, uint32 sessionID);  // 切换两个玩家排名
    // 由离线玩家排名得到玩家ID发送给客户端
    void CBLoadPlayerListByPlayerOfflineRanking(DBRecorder& res, uint64 playerID);

    // 由玩家修罗场榜排名得到玩家ID发送给客户端
    void CBLoadPlayerListByAshuraRank(DBRecorder& res, uint64 playerID);

    // 由公会榜等级排名得到玩家ID发送给客户端
    void CBLoadPlayerListByGuildRank(DBRecorder& res, uint64 playerID);

    // 删除公会BOSS之战排行回调
    void CBDelGuildCrusadeBossRank(DBRecorder& res, GUILD_DAMAGE_RANK_LIST list);

    // 删除公会暮光之战排行回调
    void CBDelGuildTwilightWarRank(DBRecorder& res, GUILD_DAMAGE_RANK_LIST list);

    // 获得暮光之战排行信息
    void CBGetGuildTwilightWarRank(DBRecorder& res, uint64 playerID);

    // 获得公会BOSS排行信息
    void CBGetGuildCrusadeBossRank(DBRecorder& res, uint64 playerID);

    std::string GetRankListTxt(const std::vector<int32>& rankList);
    std::string GetIDListTxt(const std::vector<uint64>& playerIDList);

    void CBLoadGuildRanking(DBRecorder& res, uint64 playerID ,RANKING_RANK_ACK rankingAck);

    // 获取玩家等级排行榜前count名回调
    void CBGetTopPlayerLevelRank(DBRecorder& res);

    // 由玩家翅膀榜排名得到玩家ID发送给客户端
    void CBLoadPlayerListByWingRanking(DBRecorder& res, uint64 playerID);

    // 由玩家宠物榜排名得到玩家ID发送给客户端
    void CBLoadPlayerListByPetRanking(DBRecorder& res, uint64 playerID);

    // 由玩家坐骑榜排名得到玩家ID发送给客户端
    void CBLoadPlayerListByRideRanking(DBRecorder& res, uint64 playerID);

    // 由玩家坐骑榜排名得到玩家ID发送给客户端
    void CBLoadPlayerListByFashionRanking(DBRecorder& res, uint64 playerID);


public:
    void OnGetRefreshTime();
    bool OnSaveRefreshTime(int32 rankType, uint32 rankTime);

public:
    uint32 m_refreshTime;

private:
    DBExecutor* m_dbExecutor;
};

#endif // ranking_job_h__
