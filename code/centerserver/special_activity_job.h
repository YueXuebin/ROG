/*
 *	特殊活动操作db类
 */
#ifndef special_activity_job_h__
#define special_activity_job_h__

class DBExecutor;
class DBRecorder;

class SpecialActivityJob
{
public:
    SpecialActivityJob();
    ~SpecialActivityJob();

    DECLARE_SINGLETON( SpecialActivityJob )

    // 设置SQL执行
    void SetDBExecutor( DBExecutor* val ) {m_dbExecutor = val;}

    bool SaveBattlePowerActivityInfo( void* data );
    void GetBattlePowerActivityInfo( uint32 msgId );

    bool SaveFirstTimeRechargeActivityInfo( uint64 playerID, int32 status );
    void GetFirstTimeRechargeActivityInfo( uint64 playerID, std::string type );

    bool SaveDailyRechargeActivityInfo( uint64 playerID, int32 status, uint32 time );
    void GetCurDailyRechargeActivityInfo( uint64 playerID, int32 param, std::string type );

    void GetOldDailyRechargeActivityInfo( uint64 playerID );
    bool DeleteOldDailyRechargeActivityInfo( uint64 playerID, uint32 time );

    bool SaveUpgradeWingActivityInfo( uint64 playerID, int32 status );
    void GetUpgradeWingActivityInfo( uint64 playerID, std::string type, int32 param );

    bool SaveMicroClientActivityInfo( uint64 playerID, int32 status );
    void GetMicroClientActivityInfo( uint64 playerID );

    bool SaveCityFightActivityInfo( CITY_FIGHT_INFO_NTF* info );
    void GetCityFightActivityInfo();

    bool SaveVipStatisticsInfo(VIP_STATISTICS_AUTO_GROWTH_LIST& info);
    void GetVipStatisticsInfo(VIP_STATISTICS_AUTO_GROWTH_LIST& info, std::map<uint64, int8>& playerVipMap);

    bool SaveDonateAllInfo(DONATE_ALL_INFO& info);
    void GetDonateAllInfo(DONATE_ALL_INFO& info);

    bool SaveSeasonInfo(SEASON_INFO& info);
    void GetSeasonInfo(SEASON_INFO& info);

    bool SaveRankListInfo( uint32 key, TOP_UP_RANK_INFO_ACK& info );
    void GetRankListInfo( uint32 key, TOP_UP_RANK_INFO_ACK& info );

    bool SaveDailyFashionAllInfo(DAILY_DRAW_FASHION_INFO& info);
    void GetDailyFashionAllInfo(DAILY_DRAW_FASHION_INFO& info);

private:
    void CBGetBattlePowerActivityInfo( DBRecorder& res, uint32 msgId );

    void CBGetFirstTimeRechargeActivityInfo( DBRecorder& res, uint64 playerID, std::string type );

    void CBGetCurDailyRechargeActivityInfo( DBRecorder& res, uint64 playerID, int32 param, std::string type );

    void CBGetOldDailyRechargeActivityInfo( DBRecorder& res, uint64 playerID, int32 time);

    void CBGetUpgradeWingActivityInfo( DBRecorder& res, uint64 playerID, std::string type, int32 param );

    void CBGetMicroClientActivityInfo( DBRecorder& res, uint64 playerID );

    void CBCityFightActivityInfo( DBRecorder& res );

private:
    DBExecutor* m_dbExecutor;

};

#endif