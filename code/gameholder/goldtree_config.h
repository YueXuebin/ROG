/*
 * 金像表
 */
#ifndef goldtree_config_h__
#define goldtree_config_h__

typedef struct LevelInfo
{
    void clear()
    {
        level = 0;
        gamepoint = 0;
        boss_hp = 0;
    }

    uint32 level;
    uint32 gamepoint;
    uint32 boss_hp;
}LevelInfo;

class GoldtreeConfig
{
public:
    GoldtreeConfig();
    ~GoldtreeConfig();

    DECLARE_SINGLETON(GoldtreeConfig)

    bool LoadGoldtreeConfig(const char* path);

    uint32 GetCDTime(uint32 times);                 // 获取冷却时间 单位:分钟

    uint32 GetMoneyPoint() {return m_moneyPoint;}
    uint32 GetGamePointByLevel(uint32 level);       // 通过等级得到游戏币
    uint32 GetBossHpByLevel(uint32 level);          // 通过等级得到Boss血量
    uint32 GetGoldtreeCount();                      // 一天可进入次数

private:
    std::map<uint32, LevelInfo> m_levelGamepoint;   // 等级, 金币
    std::map<uint32, uint32> m_cdTime;              // 进入冷却时间
    uint32  m_moneyPoint;                           // 第一分钟消耗魔石数
    int32   m_enterCount;                           // 可进入次数
};

#endif // goldtree_config_h__
