#ifndef investment_plan_manager_h__
#define investment_plan_manager_h__

class Player;

class InvestmentPlanManager
{
public:
    InvestmentPlanManager(Player* player);
    ~InvestmentPlanManager();

    void LoadInfo(const INVESTMENT_PLAN_INFO& info);
    void FillInfo(INVESTMENT_PLAN_INFO& info);

    void OnRecv(uint32 msgID, void * data);

    void CBInvestment(std::string str);         // 投资魔石回调
    void CBAdditional(std::string str);         // 投资魔石回调

private:
    void onInvestmentPlanInfoReq();             // 投资计划信息请求
    void onInvestmentReq(void* data);           // 投资请求
    void onAdditionalReq(void* data);           // 追加魔石
    void onRewardReq(void* data);               // 领取奖励
    void onRewardAllReq();                      // 一键领取奖励

    void sendInvestmentAck(uint32 errCode);
    void sendAdditionalAck(uint32 errCode);
    void sendRewardAck(uint32 errCode);

    int32 GetVipPrivilegeIdByInvestmentId(uint32 id);

    void CheckState();

    INVESTMENT_PLAN_TYPE_INFO* GetInvestmentOnePlan(uint32 id);
private:
    Player* m_player;

    INVESTMENT_PLAN_INFO m_investmentPlanInfo;
};

#endif // investment_plan_manager_h__

