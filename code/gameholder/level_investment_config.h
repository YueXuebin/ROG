#ifndef level_investment_config_h__
#define level_investment_config_h__

typedef std::map<uint32, uint32> LevelInvestmentInfoMap;
typedef std::vector<uint32> LevelInvestmentRangeList;

class LevelInvestmentConfig
{
public:
    LevelInvestmentConfig();
    ~LevelInvestmentConfig();

    DECLARE_SINGLETON(LevelInvestmentConfig)

    bool LoadLevelInvestmentConfig(const char* path);
    const LevelInvestmentInfoMap* GetLevelInvestmentMap();
    uint32 GetLevelInvestmentRewardPercent(uint32 level);
    bool IsMoneyInInvestmentRange(uint32 money);

private:
    bool LoadNode(TiXmlElement* element);

private:
    LevelInvestmentInfoMap m_level_investment_map;
    LevelInvestmentRangeList m_level_investment_range_list;

};

#endif