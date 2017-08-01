#ifndef day_investment_manager_h__
#define day_investment_manager_h__


class Player;
class DayInvestmentManager
{
public:
    DayInvestmentManager(Player* player);
    ~DayInvestmentManager();

    typedef std::map<uint32, int32> DayInvestmentStatusMap;         // <天, 状态>

    void Clear();
    
    void LoadDayInvestmentInfo(DAY_INVESTMENT_INFO& day_investment_info);
    void FillDayInvestmentInfo(DAY_INVESTMENT_INFO& day_investment_info);

    void OnRecv(uint32 msgID, void * data);
    
    bool IsInvested();

    void CBDayInvestmentTrade(uint32 investment_magic_stone);       // 日常投资扣费回调

    void SaveDayInvestment(uint32 day);                             // 向gc请求立即存档日常投资信息
    void CBSaveDayInvestment(void* data);                           // gc存档日常投资信息回调
    
    void SendDayInvestmentAck(int32 errCode, uint32 investedMoney,int32 active_time = 0); // 发送日常投资回执给玩家

    void IsActivateDayInvestment();                                 //是否激活投资理财

    void CalDailyRefresh();                                         //投资理财的刷新
    
    void GetAllReward();                   // 领取所有可领的奖励
private:
    void OnDayInvestmentReq();
    void OnDayInvestmentInfoReq();
    void OnDayInvestmentAcceptRewardReq(void *data);

    bool CheckIfCanAcceptReward(int32 day);

    void Update();
    void FillDayInvestmentInfoList(int32 count, std::vector<ONE_DAY_INVESTMENT_INFO>& info_list);

    int32 NeedActiveTime();                                       //还需要多少天才能激活投资理财
    void GetHaveNotAward(REWARD_TABLE& rewardTable);              //30天到了，计算还没有领取的奖励

private:
    bool m_waiting_invest_trade_call_back;                          // 等待投资扣费回调

    uint32 m_invest_begin_time;                                     // 投资时间
    DayInvestmentStatusMap m_day_investment_map;                    // 投资状态

    int32 m_recharge_num;                                            //记录30后累计的充值总数
    Player* m_player;
};

#endif
