#ifndef vip_manager_h__
#define vip_manager_h__

class Player;
class ExperienceVIP;

// ==================================================
// 玩家VIP信息管理
// ==================================================
class VIPManager
{
public:
    VIPManager(Player* player);
    ~VIPManager();

    void OnRecv(uint32 msgID, void* data);

    void LoadInfo(const VIP_INFO& info);
    void FillInfo(VIP_INFO& info);

    void CalDailyRefresh(int32 days);

    void InitVipGift();
    bool GetVipGitFlag(uint32 level);
    void SetVipGifFlag(uint32 level, int32 flag);
    void RecVipGifReq(void* data);                                  // 领取vip的福利请求

    bool CheckVipTimeValid();                                       // 检查VIP是否到期

    int32 GetGrowthPointAll() {return m_vip_info.growth_point_recharge + m_vip_info.growth_point_no_recharge;}

    int32 GetGrowthPointNoRecharge() {return m_vip_info.growth_point_no_recharge;}
    void SetGrowthPointNoRecharge(int32 val);                                                         // 设置成长点
    void AddGrowthPointNoRecharge(int32 val);                                                         // 添加成长点
    void SubGrowthPointNoRecharge(int32 val);                                                         // 删除成长点

    const VIP_INFO& GetVipInfo() { return m_vip_info; }                                     // 获得VIP信息
    void SetVipInfo(const VIP_INFO& val) { m_vip_info = val; }                              // 设置VIP信息

    int32 GetVipLevelByGrowthPoint();                                                       // 返回成长点对应的VIP等级
    int32 GetVipLevel();                                                                    // 返回VIP等级

    void CBVipOffer(int32 cardId, uint32 num);                              // VIP时效赠送回调
    void AddVipTime(uint32 cardId, uint32 vipTime);
    void CalGrowPoint();                                                    //计算成长值

    void SendVipInitAck();

    bool IsExperienceVIP();     // 是否是体验VIP

    void ActiveExperienceVIP();    // 获取体验VIP

private:
    void RecvVipRewardReq(void* data);                                  // 领取VIP礼包请求
    void RecvVipCardBuyReq(void* data);                                 // 购买VIP月卡请求

    void OnExperienceVIPReceiveReq(void *data);
    void OnExperienceVIPExpireReq(void *data);
    void OnExperienceVIPEndReq(void *data);
    void OnexperienceVIPStateReq(void *data);

    void UpdateVip(int32 oldVipLevel);

private:
    Player*             m_player;
    ExperienceVIP*      m_experienceVIP;            // 体验VIP
    VIP_INFO            m_vip_info;

};


#endif // vip_manager_h__
