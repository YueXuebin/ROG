/******************************************************************************
Copyright (C) Doingame Corporation. All rights reserved.
* @brief 加载游戏配置
******************************************************************************/

#ifndef guild_config_h__
#define guild_config_h__

typedef struct GuildMemberTurntableCost
{
    void Clear()
    {
        m_times             = 0;
        m_money_point       = 0;
    }

    int32 m_times;
    int32 m_money_point;
}GuildMemberTurntableCost;

typedef struct GuildLevelTmpl
{
    void Clear()
    {
        level                   = 0;
        activity.clear();
        max_exp                 = 0;
        max_member              = 0;
        sale_id                 = 0;
        donate_game_point       = 0;
        donate_back_guild_point = 0;
        donate_exp              = 0;
        rand_turntable_id       = 0;
        member_turntable_id     = 0;
        reward_welfare.Clear();
    }

    bool HasActivity(uint32 val) const
    {
        if(activity.find(val) != activity.end())
        {
            return true;
        }
        return false;
    }

    int32               level;
    std::set<uint32>    activity;
    int32               max_exp;
    int32               max_member;
    uint32              sale_id;
    int32               donate_game_point;
    int32               donate_back_guild_point;
    int32               donate_exp;
    uint32              rand_turntable_id;
    uint32              member_turntable_id;
    REWARD_TABLE        reward_welfare;
}GuildLevelTmpl;
//
//typedef struct GuildPromoteLevelTmpl
//{
//    void Clear()
//    {
//        level       = 0;
//        addvalue    = 0;
//        exp         = 0;
//    }
//
//    int32               level;
//    int32               addvalue;
//    int32               exp;
//}GuildPromoteLevelTmpl;
//
//typedef struct GuildAdditionTmpl
//{
//    void Clear()
//    {
//        id          = 0;
//        maxlevel    = 0;
//        levels.clear();
//    }
//
//    int32               id;
//    int32               maxlevel;
//    std::vector<GuildPromoteLevelTmpl>        levels;
//}GuildAdditionTmpl;
//
//typedef struct GuildPromoteDonateGamePointTmpl
//{
//    void Clear()
//    {
//        times              = 0;
//        guild_point         = 0;
//        exp                 = 0;
//        donate_game_point   = 0;
//    }
//
//    int32               times;
//    int32               guild_point;
//    int32               exp;
//    int32               donate_game_point;
//}GuildPromoteDonateGamePointTmpl;
//
//typedef struct GuildPromoteDonateMoneyPointTmpl
//{
//    void Clear()
//    {
//        times              = 0;
//        guild_point         = 0;
//        exp                 = 0;
//        donate_money_point  = 0;
//    }
//
//    int32               times;
//    int32               guild_point;
//    int32               exp;
//    int32               donate_money_point;
//}GuildPromoteDonateMoneyPointTmpl;

typedef struct GuildTurntableRound
{
    void Clear()
    {
        id          = 0;
        item_id     = 0;
        num         = 0;
        probability = 0;
    }

    uint32 id;
    uint32 item_id;
    int32 num;
    int32 probability;
}GuildTurntableRound;

typedef std::vector<GuildTurntableRound> GuildTurntableRoundVert;

typedef struct GuildTurntableTmpl
{
    void Clear()
    {
        turntable_id = 0;
        list.clear();
    }
    uint32 turntable_id;
    GuildTurntableRoundVert list;
}GuildTurntableTmpl;

typedef struct GuildBossTmpl
{
    void Clear()
    {
        level               = 0;
        boss_id             = 0;
        max_exp             = 0;
        cost_money_point    = 0;
        add_exp             = 0;
        rewardTable.Clear();
    }
    int32           level;              ///< BOSS的等级
    uint32          boss_id;            ///< BOSS的ID
    int32           max_exp;            ///< 最大经验
    int32           cost_money_point;   ///< 花费魔石
    int32           add_exp;            ///< 添加经验数
    REWARD_TABLE    rewardTable;        ///< BOSS奖励
}GuildBossTmpl;

typedef struct GuildActivityStartTime
{
    void Clear()
    {
        id = 0;
        start_time = 0;
    }
    uint32 id;
    uint32 start_time;
}GuildActivityStartTime;
typedef std::vector<GuildActivityStartTime> GuildActivityStartTimeVert;

typedef struct TwilightWarRankReward
{
    void Clear()
    {
        rank = 0;
        reward_table.Clear();
    }
    uint32 rank;
    REWARD_TABLE reward_table;
}TwilightWarRankReward;

typedef struct CityFightWarCityConfig
{
    void Clear()
    {
        city_id = 0;
        dungeon_id = 0;
        reward_table.Clear();
    }

    uint32 city_id;
    uint32 dungeon_id;
    REWARD_TABLE reward_table;
}CityFightWarCityConfig;
typedef std::map<int32, CityFightWarCityConfig> CityFightWarCityMap;

typedef struct CityFightScoreReward
{
    void Clear()
    {
        socre = 0;
        reward_table.Clear();
    }

    uint32 socre;
    REWARD_TABLE reward_table;
}CityFightScoreReward;
typedef std::list<CityFightScoreReward> CityFightScoreRewardList;

typedef struct DonateTmp
{
    void Clear()
    {
        num         = 0;
        guildpoint  = 0;
        exp         = 0;
    }

    int32   num;
    int32   guildpoint;
    int32   exp;
}DonateTmp;

class GuildConfig
{
public:
    GuildConfig();
    virtual ~GuildConfig();

    DECLARE_SINGLETON(GuildConfig)

    bool LoadConfig(const char* path);

    int32 GetCreateGuildMoneyPoint() { return m_create_guild_money_point; }

    uint32 GetCrusadeBossDungeonID() { return m_crusade_boss_dungeon_id; }

    uint32 GetTwilightWarDungeonID() { return m_twilight_war_dungeon_id; }

    int32 GetRandTurnTableCost() { return m_rand_turntable_cost; }

    int32 GetMemberTrunTalbeTimesCost() { return m_member_turntable_times_cost; }

    int32 GetBuyGuildPointCost() { return m_buy_guildpoint_cost; }

    //int32 GetBuyGuildPointNum() { return m_buy_guildpoint_num; }

    //int32 GetBuyGuildPointReturnRate() { return m_buy_guildpoint_return_rate; }

    int32 GetGuildBossLevelUpRebateRate() { return m_guild_boss_levelup_rebate_rate; }

    int32 GetCrusadeBossMaxPlayerNum() { return m_crusade_boss_max_player_num; }

    int32 GetTwilightWarMaxGuildNum() { return m_twilight_war_max_guild_num; }

    int32 GetTwilightWarMaxPlayerNum() { return m_twilight_war_max_player_num; }

    int32 GetMaxLevel();

    int32 GetMaxCrusadeBossLevel() { return m_max_crusade_boss_level; }

    int32 GetMaxExp(int32 level);

    uint32 GetCrusadeStartTime(uint32 id);

    uint32 GetCrusadeEndTime() { return m_crusade_end_time; }

    uint32 GetGuildJoinInterval() { return m_join_guild_interval; }

    const GuildLevelTmpl* GetGuildLeveTmpl(int32 level);

    const GuildMemberTurntableCost* LoadMemberTurntableCost(int32 times);

    const GuildTurntableTmpl* GetRandTurnTable(int32 level);

    const GuildTurntableTmpl* GetMemberTurnTable(int32 level);

    const GuildBossTmpl* GetGuildBossTmpl(int32 level);

    const GuildBossTmpl* GetTwilightWarTmplByLevel(int32 level);

    const GuildBossTmpl* GetTwilightWarTmplByID(int32 boss_id);

    const TwilightWarRankReward* GetTwilightWarRankReward(int32 rank);

    //uint32 GetTwilightWarStartTime(uint32 id);

    //const GuildActivityStartTimeVert* GetTwilightWarStartTime() { return &m_twilight_war_start_time; }

    //uint32 GetTwilightWarEndTime() { return m_twilight_war_end_time; }

    uint32 GetTwilightWarRanRewardNum();

    const CityFightWarCityMap* GetCityFightCityMap() { return &m_city_fight_war_city_map; }

    const CityFightWarCityConfig* GetCityFightWarCity(uint32 cityId);

    uint32 GetCityFightWarCityID(uint32 dungeonID);

    void GetCityFightScoreReward(uint32 socre, REWARD_TABLE& reward_table);

    uint32 GetCityFightMaxGuildNumPerRoom() { return m_city_fight_guild_num_per_room; }

    uint32 GetCityFightMaxPlayerNumPerGuild() { return m_city_fight_player_num_per_guild; }

    uint32 GetCityFightWinScore() { return m_city_fight_win_score; }

    const DonateTmp& GetGamepointDonateTmp() {return m_gamepointDonate;}
    const DonateTmp& GetMoneypointDonateTmp() {return m_moneypointDonate;}
private:
    void Clear();

    bool LoadLevelUpConfig(TiXmlElement* xmlEle);

    bool LoadPromoteConfig(TiXmlElement* xmlEle);

    bool LoadPromoteDonateGamePointConfig(TiXmlElement* xmlEle);

    bool LoadPromoteDonateMoneyPointConfig(TiXmlElement* xmlEle);

    bool LoadMemberTurntableCost(TiXmlElement* xmlEle);

    bool LoadRandTurntable(TiXmlElement* xmlEle);

    bool LoadMemberTurntable(TiXmlElement* xmlEle);

    bool LoadCrusadeBoss(TiXmlElement* xmlEle);

    bool LoadTwilightWar(TiXmlElement* xmlEle);

    bool LoadTwilightWarBossTmpl(TiXmlElement* xmlEle);

    bool LoadTwilightWarReward(TiXmlElement* xmlEle);

    //bool LoadTwilightWarStartTime(TiXmlElement* xmlEle);

    bool LoadActivityList(TiXmlElement* xmlEle);

    bool LoadCityFightWar(TiXmlElement* xmlEle);

    bool LoadDonate(TiXmlElement* xmlEle);

    int32 m_create_guild_money_point;                       ///< 创建公会话费的游戏币

    uint32 m_crusade_boss_dungeon_id;                       ///< 争夺BOSS副本ID
    uint32 m_twilight_war_dungeon_id;                       ///< 暮光之战副本ID

    int32 m_rand_turntable_cost;                            ///< 随机龙盘消耗贡献点

    int32 m_crusade_boss_max_player_num;                    ///< 公会BOSS工会内最大成员数
    int32 m_twilight_war_max_guild_num;                     ///< 暮光之战最大公会数
    int32 m_twilight_war_max_player_num;                    ///< 暮光之战最大成员数

    int32 m_buy_guildpoint_cost;                            ///< 购买公会贡献点的花费
    int32 m_buy_guildpoint_num;                             ///< 购买公会贡献点的获得
    int32 m_buy_guildpoint_return_rate;                     ///< 购买公会贡献返回率

    int32 m_guild_boss_levelup_rebate_rate;                 ///< 公会BOSS升级返点比率

    uint32 m_join_guild_interval;                           ///< 加入公会间隔时间

    typedef std::vector<GuildLevelTmpl> GuildLevelVert;     ///< 升级经验LevelUpVert
    GuildLevelVert m_levelup;                               ///< 升级经验表

    //typedef std::vector<GuildAdditionTmpl> GuildAdditionVert;///< 科技加成
    //GuildAdditionVert m_additions;                          ///< 科技加成表

    //typedef std::vector<GuildPromoteDonateGamePointTmpl> GuildPromoteDonateGamePointVert;   // 科技升级gamepoint
    //GuildPromoteDonateGamePointVert m_promoteGamePoints;    ///< 科技升级gamepoint表

    //typedef std::vector<GuildPromoteDonateMoneyPointTmpl> GuildPromoteDonateMoneyPointVert; // 科技升级moneypoint
    //GuildPromoteDonateMoneyPointVert m_promoteMoneyPoints;  ///< 科技升级moneypoint表

    //int32 m_addition_promote_game_point_time;               ///< 科技升级使用gamepoint次数
    //int32 m_addition_promote_money_point_time;              ///< 科技升级使用moneypoint次数

    typedef std::vector<GuildMemberTurntableCost> GuildMemberTurntableCostVert;
    GuildMemberTurntableCostVert m_member_turntable_cost;       ///< 公会龙盘消耗
    int32                        m_member_turntable_times_cost; ///< 公会购买龙盘次数消耗

    typedef std::vector<GuildTurntableTmpl> TurntableVert;
    TurntableVert m_rand_turntable;                             ///< 随机轮盘
    TurntableVert m_member_turntable;                           ///< 公会成员轮盘

    GuildActivityStartTimeVert m_crusade_start_time;            ///< 工会讨伐开始时间

    uint32 m_crusade_end_time;                                  ///< 公会讨伐结束时间

    int32 m_max_crusade_boss_level;                             ///< 公会BOSS 的最大等级

    typedef std::map<int32, GuildBossTmpl> GuildBossTmplMap;
    GuildBossTmplMap m_crusade_boss;                            ///< 公会BOSS
    GuildBossTmplMap m_twilight_boss;                           ///< 公会BOSS

    typedef std::map<int32, TwilightWarRankReward> TWarRankRewardMap;
    TWarRankRewardMap   m_twarRankReward;                       ///< 暮光之战战斗奖励

    // 城市争夺战
    int32 m_city_fight_guild_num_per_room;           // 每个房间公会最大数量
    int32 m_city_fight_player_num_per_guild;         // 每个房间每个公会最大玩家数量
    int32 m_city_fight_win_score;                    // 每个城市胜利所需分数

    CityFightWarCityMap m_city_fight_war_city_map;   // 城市争夺战城市表
    CityFightScoreRewardList m_city_fight_score_reward_list;    // 城市争夺战得分奖励

    DonateTmp       m_gamepointDonate;
    DonateTmp       m_moneypointDonate;
};

#endif
