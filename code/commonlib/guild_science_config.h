#ifndef guild_science_config_h__
#define guild_science_config_h__

// 科技等级对产出加成模板
typedef struct GuildPromoteLevelTmpl
{
    void Clear()
    {
        level       = 0;
        addvalue    = 0;
        exp         = 0;
    }

    int32               level;      // 科技等级
    int32               addvalue;   // 加成万分比
    int32               exp;        // 升到下一级所需经验量
}GuildPromoteLevelTmpl;

// 公会加成类型模板
typedef struct GuildAdditionTmpl
{
    void Clear()
    {
        id          = 0;
        maxlevel    = 0;
        levels.clear();
    }

    int32               id;         // 科技类型
    int32               maxlevel;   // 最大等级
    std::vector<GuildPromoteLevelTmpl>        levels;       // 等级对产出的加成列表
}GuildAdditionTmpl;
typedef std::vector<GuildAdditionTmpl> GuildAdditionVert;   // 科技加成

// 个人捐献次数模板
typedef struct GuildPromoteDonatePointTmpl
{
    void Clear()
    {
        times               = 0;
        guild_point         = 0;
        exp                 = 0;
        donate_num          = 0;
    }

    int32               times;              // 捐献次数
    int32               guild_point;        // 增加贡献点数
    int32               exp;                // 增加科技经验数
    int32               donate_num;         // 花费货币数
}GuildPromoteDonatePointTmpl;
typedef std::vector<GuildPromoteDonatePointTmpl> GuildPromoteDonatePointVert;       // 提升科技消耗产出列表(按次数变化)

// 公会科技
class GuildScienceConfig
{
public:
    GuildScienceConfig();
    ~GuildScienceConfig();

    DECLARE_SINGLETON(GuildScienceConfig)

    bool LoadConfig(const char* path);

    const GuildAdditionVert& GetGuildAdditions(){return m_additions;}                       // 得到科技加成表
    const GuildPromoteDonatePointVert& GetPromoteGamePt() {return m_promoteGamePoints;}     // 得到金币提升科技花费与产出
    const GuildPromoteDonatePointVert& GetPromoteMoneyPt() {return m_promoteMoneyPoints;}   // 得到魔石提升科技花费与产出

    const GuildAdditionTmpl* GetScienceAdditionTmpl(int32 id);                  // 由科技ID得到科技加成信息
    const GuildPromoteLevelTmpl* GetPromoteLevelTmpl(int32 id, int32 level);    // 得到一项科技等级对产出加成信息

    const GuildPromoteDonatePointTmpl* GetPromoteDonateGamePointTmpl(int32 times);  // 由次数得到金币提升模板
    const GuildPromoteDonatePointTmpl* GetPromoteDonateMoneyPointTmpl(int32 times);  // 由次数得到金币提升模板
private:
    bool LoadPromote(TiXmlElement* xmlEle);                     // 加载公会科技加成
    bool LoadDonateGamePoint(TiXmlElement* xmlEle);             // 加载个人金币提升科技模板
    bool LoadDonateMoneyPoint(TiXmlElement* xmlEle);            // 加载个人魔石提升科技模板

private:
    GuildAdditionVert   m_additions;                    // 科技加成表

    GuildPromoteDonatePointVert m_promoteGamePoints;    // 金币提升科技花费与产出
    GuildPromoteDonatePointVert m_promoteMoneyPoints;   // 魔石提升科技花费与产出
};

#endif // guild_science_config_h__
