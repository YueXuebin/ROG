#ifndef vip_statistics_manager_h__
#define vip_statistics_manager_h__

/*
 *	vip 全服统计
 */
class VipStatisticsManager
{
public:
    VipStatisticsManager();
    ~VipStatisticsManager();

    DECLARE_SINGLETON(VipStatisticsManager)

    void Init();
    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);

    void Update();

    void SetConfig(const std::string& configJson);

    void SaveVipStatistics();
    void SendVipPlayerNum();                                // 发送玩家数

private:
    VIP_STATISTICS_INFO GetVipInfo();

    void onPlayerVipInfo(void* data);                       // 接收玩家Vip信息变更

    uint32 GetPlayerNumByVipLevel(int8 vipLevel);

private: 
    std::map<uint64, int8> m_PlayerVipMap;                  // 玩家Vip列表
    VIP_STATISTICS_AUTO_GROWTH_LIST m_VipAutoGrowth;        // vip自动增长值

    VIP_STATISTICS_CONFIG m_ConfigInfo;                     // 动态配置项

    uint32 m_minute;                                        // 每分钟检测一次
    uint32 m_minuteTotal;                                   // 服务器开启经过的分种数
};

#endif // vip_statistics_manager_h__
