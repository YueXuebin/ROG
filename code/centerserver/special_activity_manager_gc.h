/*
 *	特殊活动管理
 */
#ifndef special_activity_manager_gc_h__
#define special_activity_manager_gc_h__


typedef struct DailyRechargeActivityInfo
{
    int32 status;
    int32 money_point;

    void Clear()
    {
        status = 0;
        money_point = 0;
    }
}DailyRechargeActivityInfo;
typedef std::vector<DailyRechargeActivityInfo> DailyRechargeActivityInfoList;

class SpecialActivityManager
{
public:
    SpecialActivityManager();
    ~SpecialActivityManager();

    DECLARE_SINGLETON(SpecialActivityManager)
    
    int32 GetDaysBetweenDates(time_t t1, time_t t2);
    
    void OnRecv (uint64 playerID, uint32 sessionID, uint32 msgID, void* data);

    void CBGetTotalRechargeByPlayerId(uint64 playerID, uint32 moneyPoint);
    void GetFirstTimeRechargeActivityInfo(uint64 playerID, std::string type);
    void CBGetFirstTimeRechargeActivityInfo(uint64 playerID, std::string type, int32 status);

    void GetDailyRechargeActivityInfo(uint64 playerID, void* data, std::string type);
    void CBGetOldDailyRechargeActivityInfo(uint64 playerID, DailyRechargeActivityInfoList& infoList, int32 time);
    void CBGetCurDailyRechargeActivityInfo(uint64 playerID, int32 param, std::string type, int32 money, int32 status, uint32 dtime);

    void GetUpgradeWingActivityInfo(uint64 playerID, std::string type, int32 param = -1);
    void CBGetUpgradeWingActivityInfo(uint64 playerID, std::string type, int32 param, int32 status);

    void GetMicroClientActivityInfo(uint64 playerID);
    void CBGetMicroClientActivityInfo(uint64 playerID, int32 status);
private:
    void OnSpecialActivityInfoReq(uint64 playerID, void *data);
    void OnSpecialActivityAcceptRewardReq(uint64 playerID, void *data);
    void SendSpecialActivitySaveInfoReq(uint64 playerID, void *data);
    void OnSpecialActivityLoadInfoReq(uint64 playerID, void *data);
    void SendSpecialActivityLoadInfoAck(  uint64 playerID, int32 activityId, int32 status, int32 param );

    // BattlePowerRankingActivity
    void OnBattlePowerRankingRefresh();
    void OnGetBattlePowerActivityInfoAck(void* data);
    void GiveBattlePowerRankingReward();
    void OnGetBattlePowerRankingAck(void* data);

    // FirstTimeRechargeActivity
    void OnFirstTimeRechargeInfoReq(uint64 playerID, void* data);
    void CBOnFirstTimeRechargeInfoReq(uint64 playerID, int32 status);
    void OnFirstTimeRechargeAcceptRewardReq(uint64 playerID, void* data);
    void CBOnFirstTimeRechargeAcceptRewardReq(uint64 playerID, int32 status);
    void SendFirstTimeRechargeInfoAck(uint64 playerID, int32 status);

    // DailyRechargeActivity
    void OnDailyRechargeInfoReq(uint64 playerID, void* data);
    void CBOnDailyRechargeInfoReq(uint64 playerID, int32 money, int32 status, uint32 dtime);
    void OnDailyRechargeAcceptRewardReq(uint64 playerID, void* data);
    void CBOnDailyRechargeAcceptRewardReq(uint64 playerID, int32 param, int32 money, int32 status, uint32 dtime);

    // UpgradeWingActivity
    void OnUpgradeWingInfoReq(uint64 playerID, void* data);
    void CBOnUpgradeWingInfoReq(uint64 playerID, int32 param, int32 status);

private:

};

#endif // special_activity_manager_gc_h__
