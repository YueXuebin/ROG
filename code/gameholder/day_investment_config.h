/*
*    日常投资配置
*/
#ifndef day_investment_config_h__
#define day_investment_config_h__

class Player;
typedef struct DayInvestmentInfo
{
    uint32 day;
    REWARD_ITEM reward;

    void Clear()
    {
        day = 0;
        reward.Clear();
    }

}DayInvestmentInfo;

typedef std::map<uint32, DayInvestmentInfo> DayInvestmentInfoMap;

typedef struct InvestmentPlanDayInfo
{
    uint32 day;
    std::map<uint32, REWARD_TABLE> moneypointRewardMap;      // <魔石数, 单项奖励>

    void Clear()
    {
        day = 0;
        moneypointRewardMap.clear();
    }
}InvestmentPlanDayInfo;

typedef std::map<uint32, InvestmentPlanDayInfo> InvestmentPlanDayMap;

typedef struct InvestmentPlanType
{
    uint32 id;                                      // 投资类型
    uint32 total_day;                               // 天数
    uint32 additional_overdue_day;                  // 追加过期天数
    uint32 vipPrivilege;                            // vip等级
    InvestmentPlanDayMap dayMap;                    // 天数奖励列表
    
    void Clear()
    {
        id = 0;
        total_day = 0;
        vipPrivilege = 0;
        additional_overdue_day = 0;
        dayMap.clear();
    }
}InvestmentPlanType;

class DayInvestmentConfig
{
public:
    DayInvestmentConfig();
    ~DayInvestmentConfig();

    DECLARE_SINGLETON(DayInvestmentConfig)
    
    bool LoadDayInvestmentConfig(const char* path);
    bool LoadInvestmentType(TiXmlElement* rootEle);

    uint32 GetInvestmentTotalDay();
    uint32 GetInvestmentMagicStoneCount();
    DayInvestmentInfo* GetDayInvestmentInfo(uint32 day);
    uint32 GiveDayInvestmentReward(Player* player, uint32 day);         // 返回奖励的魔石数

    const std::map<int32, InvestmentPlanType>& GetInvestmentPlanMap(){return m_investmentPlanMap;}
    const InvestmentPlanType* GetInvestmentPlanTmp(uint32 id);
private:
    bool LoadNode(TiXmlElement* TitleEle);

private:
    uint32 m_total_day;
    uint32 m_investment_magic_stone;
    DayInvestmentInfoMap m_day_investment_map;

    std::map<int32, InvestmentPlanType> m_investmentPlanMap;         // 投资类型列表
};

#endif
