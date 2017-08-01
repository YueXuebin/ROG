/*
 *	在线奖励系统
 */
#ifndef ONLINE_REWARD_MANAGER_H
#define ONLINE_REWARD_MANAGER_H

class Player;

class OnlineRewardManager
{
public:
    OnlineRewardManager(Player* player);
    ~OnlineRewardManager();

    void Clear();
    void OnRecv(uint32 msgID, void* data);

    void LoadInfo(const ONLINE_REWARD_INFO& info);
    void FillInfo(ONLINE_REWARD_INFO& info);

    void CalDailyRefresh();

    void Update();

    ONLINE_REWARD_INFO& GetInfo() {return m_online_reward_info;}

    uint32 GetOnlineRewardTime() { return m_online_reward_info.online_reward_time; }            // 获得在线奖励时间
    void SetOnlineRewardTime(uint32 val) { m_online_reward_info.online_reward_time = val; }     // 设置在线奖励时间

    uint32 GetOnlineRewardDay() { return m_online_reward_info.online_reward_day; }              // 获得在线奖励的天数
    void SetOnlineRewardDay(uint32 val) { m_online_reward_info.online_reward_day = val; }       // 获得在线奖励的天数

    uint32 GetOnlineRewardFlag() { return m_online_reward_info.online_reward_flag; }            // 获得在线奖励标记

    //随机物品的类型
    enum 
    {
        SingleType,
        CrityType
    };

private:
    bool IsOnlineRewardFlag(uint32 id);                                                         // 是否在线奖励被标记
    void SetOnlineRewardFlag(uint32 id);                                                        // 设置在线奖励标记

    bool IsLevelRewardFlag(int32 level);                                                        // 是否领取过等级奖励
    void SetLevelRewardFlag(int32 level);                                                       // 设置领取过等级奖励

    void RecvOnlineRewardReq(void* data);                                                       // 领取在线奖励
    void InitOnlineRewardInfo();                                                                // 请求在线奖励信息
    void InitOnlineRewardGrowList();
    void OnGrowLevelReq(void* data);                                                            // 等级奖励
    void BestowRewardReq(void* data);                                                           // 天数全部奖励

    bool RandomGrowValue(int32 curnum);
    bool AddCritGrowreward();
    bool RandomSingleGrowValue();
    bool CalculationGrowValue(REWARD_ITEM* curitem);                                            // 计算要增加物品的成长值
    int32 OnGetGrowBestowtime();                                                                // 计算领取成长赐福的时间差;
    void CalBestowReward(BESTOW_REWARD_ACK& sendMsg, int32 days, int32 curTime);                 // 领取福利奖励的天数和奖励分发

    int32 CalAddItemNum(int32 num);                                                             //

    void AddGrowReward(int32 type, int32 num ,int32 id,int32 randomtype);                       // 添加相应的成长值，type统计改变了哪个类型的奖励
    void ClearGrowList();                                                                       // 设置成长列表的数量为0

    void FindGrowReward(REWARD_TABLE& rewardtable);                                             // 查找成长奖励

private:
    Player*             m_player;
    uint32              m_secondTimer;

    ONLINE_REWARD_INFO  m_online_reward_info;

    std::vector<REWARD_ITEM>  m_onlinechangItem;                                                // 统计改变了哪些奖励值，是单个的还是暴击的，如果是暴击，那么是所有的growlist列表
};

#endif