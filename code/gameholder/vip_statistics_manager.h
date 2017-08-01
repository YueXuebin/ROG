/*
    VIP领域
 */
#ifndef vip_statistics_manager_h__
#define vip_statistics_manager_h__

class Player;

class VipStatisticsManager
{
public:
    VipStatisticsManager();
    ~VipStatisticsManager();

    DECLARE_SINGLETON(VipStatisticsManager)

    void OnRecv(uint32 msgID, void* data);

    uint32 GetPlayerNumByVipLevel(int8 vipLevel);           // 由vip等级得到玩家数

    void VipLevelChange(Player* pPlayer);                   // vip 等级变更

private:
    void onRecvInfo(void* data);

    VIP_STATISTICS_INFO m_vipInfo;
};

#endif // vip_statistics_manager_h__
