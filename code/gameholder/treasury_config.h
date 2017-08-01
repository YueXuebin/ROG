/*
 * 宝库表
 */
#ifndef treasury_config_h__
#define treasury_config_h__

class TreasuryConfig
{
public:
    TreasuryConfig();
    ~TreasuryConfig();

    DECLARE_SINGLETON(TreasuryConfig)

    bool LoadTreasuryConfig(const char* path);

    uint32 GetReward(uint32 level);                 // 获取金币数
    uint32 GetCDTime(uint32 times);                 // 获取冷却时间 单位:分钟

    uint32 GetMoneyPoint() {return m_moneyPoint;}
    uint32 GetTreasuryCount();                      // 进入次数

private:
    std::map<uint32, uint32> m_levelGamepoint;      // 等级, 金币
    std::map<uint32, uint32> m_cdTime;              // 进入冷却时间
    uint32 m_moneyPoint;                            // 每一分钟消耗魔石数
    int32  m_enterCount;                            // 进入次数
};

#endif // treasury_config_h__
