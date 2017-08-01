#ifndef multiserver_battle_info_manager_h__
#define multiserver_battle_info_manager_h__

class Player;

/*
 *	跨服匹配战信息管理器
 */
class MultiserverBattleInfoManager
{
public:
    MultiserverBattleInfoManager(Player* pPlayer);
    ~MultiserverBattleInfoManager(void);

    void OnRecv(uint32 msgID, void* data);

    void ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 );//接受事件

    void LoadMultiserverBattleInfo(const MULTISERVER_BATTLE_BAG& info);
    void FillMultiserverBattleInfo(MULTISERVER_BATTLE_BAG& info);
    void UpdateMultiserverBattleInfo();

    void OnMultiserverBattlePlayerReq(uint32 msgID, void* data);

    void OnWin(uint32 battleType, int32 currentIntegral);
    void onFail(uint32 battleType, int32 currentIntegral);

    void OnloadInfo(MULTISERVER_BATTLE_PK_END_INFO &sendMsg, uint32 battleType, int32 currentIntegral, int32 currentFailIntegral);
    // 段位经验
    void AddIntegralPoint(int32 val);
    void SubIntegralPoint(int32 val);
    void ChangeIntegralPoint(int32 val);
    int32 GetIntegralPoint() {return m_info.integral;}
    void SetIntegralPoint(int32 val);
    // 功勋值(1V1)
    void AddExploitPoint(int32 val);
    void SubExploitPoint(int32 val);
    int32 GetExploitPoint() {return m_info.exploit_point;}
    void SetExploitPoint(int32 val);
    // 联盟值(3V3)
    void AddAlliancePoint(int32 val);
    void SubAlliancePoint(int32 val);
    void ChangeAlliancePoint(int32 val);
    int32 GetAlliancePoint() {return m_info.alliance_point;}
    void SetAlliancePoint(int32 val);
    // 赛季币
    void AddSeasonPoint(int32 val);
    void SubSeasonPoint(int32 val);
    int32 GetSeasonPoint() {return m_info.season_point;}
    void SetSeasonPoint(int32 val);

    int32 GetSeasonTime(){return m_info.season_time;}       // 赛季结束时间

    void AddWinCount(int32 type, int32 val, uint32 time);
    void AddLoseCount(int32 type, int32 val, uint32 time);
    void AddInvolvedCount(int32 type, int32 val, uint32 time);

    void CheckNewSeason();                     // 新赛季

    void CalDailyRefresh();

private:
    void OnBattleInfoReq(void* data);
    MULTISERVER_BATTLE_ITEM_INFO* GetCurrentBattleInfo(uint32 type);    // 当天战斗信息

    void OnClearDayInfo();                                              // 重置当天计数
    void OnWaitAck(void* data);                                         // 等待超时奖励
    void revcBattleInfoAck(uint16 error);

    void OnGetLevelExploitReq(void* data);                              // 获取等级对应的功勋奖励
    void OnGetLevelAllianceReq(void *data);                             // 获取等级对应的联盟值奖励

    void OnExploitBuyReq(void* data);                                  // 功勋点购买
    void SendExploitBuyAck(uint16 error);                              // 功勋点购买回应

    void OnAllianceBuyReq(void* data);
    void SendAllianceBuyAck(uint16 error);

    void OnSeasonBuyReq(void* data);
    void SendSeasonBuyAck(uint16 error);

    bool CheckSalelimit(uint32 id, uint32 num, uint32 limit);          // 判断限制条件
    bool AddSaleNumber(uint32 id, uint32 num);                         // 添加购买次数
    void ResetSaleNumber();                       // 重置购买次数

    void onOffEventChaosReward(const std::string& infoStr);

private:
    Player* m_player;

    MULTISERVER_BATTLE_BAG m_info;
};

#endif