/*
 *	排行榜
 */
#ifndef ranking_save_job_h__
#define ranking_save_job_h__

class DBExecutor;
class DBRecorder;

class RankingSaveJob
{
public:
    RankingSaveJob() {m_dbExecutor = 0;}
    ~RankingSaveJob() {}

    DECLARE_SINGLETON(RankingSaveJob)

    // 设置SQL执行
    void SetDBExecutor(DBExecutor* val) {m_dbExecutor = val;}

    void Init();

    bool SaveRankUpdateTime(int32 rankType, uint32 rankTime);   // 保存玩家指定类型排行榜更新时间

    bool SaveAllFirstOfRanking();               // 保存所有榜第一名玩家信息
    bool ClearAllFirstOfRanking();              // 清空所有榜第一名玩家信息

    bool SaveBattlePowerRank();                 // 保存玩家战力榜
    bool ClearBattlePowerRank();                // 清空玩家战力榜

    bool SaveHoroscopeRank();                   // 保存玩家星空图榜
    bool ClearHoroscopeRank();                  // 清空玩家星空图榜

    bool SaveGamepointRank();                   // 保存玩家游戏币榜
    bool ClearGamepointRank();                  // 清空玩家游戏币榜

    bool SavePlayerLevelRank();                 // 保存玩家等级榜
    bool ClearPlayerLevelRank();                // 清空玩家等级榜

    bool SaveGuildRank();                       // 保存公会榜
    bool ClearGuildRank();                      // 清空公会榜

    bool SaveAshuraRank();                      // 保存修罗场榜
    bool ClearAshuraRank();                     // 清空修罗场榜

    bool SaveOldOfflinePlayerRank();            // 保存离线排行榜旧有数据

    bool SaveWingBattlePowerRank();             // 保存翅膀战力榜
    bool ClearWingBattlePowerRank();            // 清空翅膀战力榜

    bool SavePetBattlePowerRank();              // 保存宠物战力榜
    bool ClearPetBattlePowerRank();             // 清空宠物战力榜

    bool SaveRideBattlePowerRank();             // 保存坐骑战力榜
    bool ClearRideBattlePowerRank();            // 清空坐骑战力榜`

    bool SaveFashionBattlePowerRank();          // 保存化身战力榜
    bool ClearFashionBattlePowerRank();         // 清空化身战力榜


    bool SavePlotDungonScoreRank();             // 保存剧情副本得分榜
    bool SaveAchiveCountRank();                 // 保存成就个数榜

    void GetRankingRefreshTime(uint32& hourRefreshTime, uint32& dailyRefreshTime);

private:
    RankingSaveJob( const RankingSaveJob& );
    RankingSaveJob& operator=( const RankingSaveJob& );
    RankingSaveJob* operator&();
    const RankingSaveJob* operator&() const;

    DBExecutor* m_dbExecutor;
};

#endif // ranking_save_job_h__
