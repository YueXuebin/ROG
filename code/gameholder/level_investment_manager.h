#ifndef level_investment_manager_h__
#define level_investment_manager_h__


class Player;

class LevelInvestmentManager
{
public:
    typedef std::map<uint32, int32> LevelInvesementStatusMap;

    LevelInvestmentManager(Player* player);
    ~LevelInvestmentManager();

    void Clear();
    
    void LoadLevelInvestmentInfo(LEVEL_INVESTMENT_INFO& level_investment_info);
    void FillLevelInvestmentInfo(LEVEL_INVESTMENT_INFO& level_investment_info);
    
    void OnRecv(uint32 msgID, void * data);
    
    void CBLevelInvestmentTrade(uint32 money);                      // 等级投资扣费回调

    void SaveLevelInvestment(uint32 level, uint32 reward_money);    // 向gc请求立即存档等级投资信息
    void CBSaveLevelInvestment(void* data);                         // gc存档等级投资信息回调

    bool IsInvested();

    void GetAllReward();                                            // 领取可领的所有奖励
private:
    void OnLevelInvestmentReq(void *data);
    void OnLevelInvestmentInfoReq(void *data);
    void OnLevelInvestmentAcceptRewardReq(void *data);
    void InvestmentMarquee(uint32 type, uint32 money);                                       // 投资理财公告

    void Update();

private:
    bool m_waiting_invest_trade_call_back;                          // 等待投资扣费回调

    Player* m_player;
    uint32 m_invested_money;
    LevelInvesementStatusMap m_level_investment_map;

};

#endif