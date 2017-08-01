#include "gameholder_pch.h"
#include "game_define.h"
#include "config_manager.h"
#include "server_config.h"
#include "formula_cal.h"
#include "formula_config.h"
#include "item_config_manager.h"
#include "online_reward_config.h"
#include "sale_config.h"
#include "guild_config.h"
#include "vigor_config.h"
#include "vip_card_config.h"
#include "vip_config.h"
#include "vip_reward_config.h"
#include "horoscope_config.h"
#include "synthetize_config.h"
#include "task_game_config.h"
#include "task_festive_config.h"
#include "fashion_config.h"
#include "battle_offline_config.h"
#include "skill_config_manager.h"
#include "passive_skill_config_manager.h"
#include "design_config_manager.h"
#include "dungeon_gain_config.h"
#include "dungeon_score_config.h"
#include "ashura_config.h"
#include "team_dungeon_config.h"
#include "answer_question_config.h"
#include "adventure_config.h"
#include "bouns_config_manager.h"
#include "sign_makeup_config.h"
#include "wing_attr_config_manager.h"
#include "mall_config.h"
#include "mall_config_manager.h"
#include "world_boss_config.h"
#include "treasury_config.h"
#include "title_config.h"
#include "goldtree_config.h"
#include "player_door_reward_config.h"
#include "activity_config.h"
#include "honor_sale_config.h"
#include "guild_sale_config.h"
#include "day_investment_config.h"
#include "level_investment_config.h"
#include "special_activity_config.h"
#include "activity_reward_config.h"
#include "auto_fight_config.h"
#include "system_setting_config.h"
#include "mail_title_config.h"
#include "common_reward_config.h"
#include "bind_money_point_sale_config.h"
#include "dance_config.h"
#include "liveness_config.h"
#include "td_config.h"
#include "group_pk_config.h"
#include "mall_config.h"
#include "faction_pk_config.h"
#include "ride_config.h"
#include "city_survival_config.h"
#include "stone_turn_gold_config.h"
#include "capture_flag_config.h"
#include "namelimit.h"
#include "limitbuy_config.h"
#include "singgle_answer_config.h"
#include "vip_gift_config.h"
#include "equip_lighting.h"
#include "guild_task_config.h"
#include "sword_config.h"
#include "zombie_config.h"
#include "void_field_config.h"
#include "void_field_solo_config.h"
#include "brave_heart_config.h"
#include "giftcode_config.h"
#include "guild_science_config.h"
#include "void_field_setting_config.h"
#include "package_config.h"
#include "shorcut_config.h"
#include "welcome_reward_config.h"
#include "protect_config.h"
#include "center_config.h"
#include "multiserver_battle_config.h"
#include "world_map_config.h"
#include "chaos_faction_config.h"
#include "bag_grid_open_cost_config.h"
#include "vip_only_dungeon_config.h"
#include "pay_point_lottery_config.h"
#include "off_line_vigor_config.h"
#include "donate_config.h"
#include "egg_config.h"
#include "buy_cheap_config.h"
#include "daily_fashion_config.h"

#include "monster_config_manager.h"
#include "item_config_manager.h"
#include "alchemy_config_manager.h"
#include "gem_config_manager.h"
#include "player_config_manager.h"
#include "system_config_manager.h"
#include "dungeon_config_manager.h"
#include "task_config_manager.h"



IMPLEMENT_SINGLETON(ConfigManager)


ConfigManager::ConfigManager()
{

}

ConfigManager::~ConfigManager()
{
    UnInit();
}

bool ConfigManager::Init(uint32 server_id)
{
    char file_path[SD_MAX_PATH];

    // 读取center_config配置
    CenterConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", RES_CONFIG_PATH, CENTER_CONFIG_FILE);
    if(!CenterConfig::Instance()->ReadFile(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取服务器配置信息
    ServerConfig::CreateInstance();
    // 设置服务器ID
    ServerConfig::Instance()->SetMyID(server_id);
    if(!ServerConfig::Instance()->LoadConfig())
    {
        return false;
    }

    char path[SD_MAX_PATH];
    SDSprintf(path, "%s", GetSdbPath().c_str());

 
    // 创建公式计算实例
    FormulaCal::CreateInstance();

    // 读取公式配置信息
    FormulaConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, FORMULA_CONFIG_FILE);
    if(!FormulaConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed", file_path);
        return false;
    }

    // 读取关卡相关配置
    DesignConfigManager::CreateInstance();
    if(!DesignConfigManager::Instance()->LoadConfig(path))
    {
        return false;
    }

    // 物品相关配置
    ItemConfigManager::CreateInstance();
    if(!ItemConfigManager::Instance()->LoadConfig(path))
    {
        return false;
    }

    // 领地相关配置
    AlchemyConfigManager::CreateInstance();
    if(!AlchemyConfigManager::Instance()->LoadConfig(path))
    {
        return false;
    }

    // 怪物相关配置
    MonsterConfigManager::CreateInstance();
    if (!MonsterConfigManager::Instance()->LoadConfig(path))
    {
        return false;
    }

    // 玩家相关配置
    PlayerConfigManager::CreateInstance();
    if (!PlayerConfigManager::Instance()->LoadConfig(path))
    {
        return false;
    }

    // 读取技能相关配置
    SkillConfigManager::CreateInstance();
    if (!SkillConfigManager::Instance()->LoadConfig(path))
    {
        return false;
    }

    // 读取天赋相关配置
	PassiveSkillConfigManager::CreateInstance();
	if (!PassiveSkillConfigManager::Instance()->LoadConfig(path))
	{
		return false;
	}

    // 读取宝石相关配置
    GemConfigManager::CreateInstance();
    if (!GemConfigManager::Instance()->LoadConfig(path))
    {
        return false;
    }

    // 游戏系统相关配置
    SystemConfigManager::CreateInstance();
    if (!SystemConfigManager::Instance()->LoadConfig(path))
    {
        return false;
    }

    // 关卡配置
    DungeonConfigManager::CreateInstance();
    if (!DungeonConfigManager::Instance()->LoadConfig(path))
    {
        return false;
    }

	// 读取签到奖励表
	BounsConfigManager::CreateInstance();
	if (!BounsConfigManager::Instance()->LoadConfig(path))
	{
		return false;
	}

    // 读取翅膀信息
    WingAttrConfigManager::CreateInstance();
    if(!WingAttrConfigManager::Instance()->LoadConfig(path))
    {
        return false;
    }

    // 任务配置
    TaskConfigManager::CreateInstance();
    if(!TaskConfigManager::Instance()->LoadConfig(path))
    {
        return false;
    }

    // 创建商城，但不需要读取配置信息（GS上的商城信息会通过CenterServer同步过来）
    // MallConfig::CreateInstance();
    // 读取商城信息
    MallConfigManager::CreateInstance();
    if(!MallConfigManager::Instance()->LoadConfig(path))
    {
        return false;
    }



    // --------------------------------------------------------------------------------------------------------------------------------------- //
    // --------------------------------------------------------------------------------------------------------------------------------------- //
    // --------------------------------------------------------------------------------------------------------------------------------------- //
    // --------------------------------------------------------------------------------------------------------------------------------------- //
    // --------------------------------------------------感觉以下的读取配置相关的，是以前圣剑游戏内的----------------------------------------- //
    // --------------------------------------------------------------------------------------------------------------------------------------- //
    // --------------------------------------------------------------------------------------------------------------------------------------- //
    // --------------------------------------------------------------------------------------------------------------------------------------- //
    // --------------------------------------------------------------------------------------------------------------------------------------- //

    // 读取在线奖励配置信息
    OnlineRewardConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, ONLINE_REWARD_CONFIG_FILE);
    if(!OnlineRewardConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取NPC贩卖配置信息
    SaleConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, SALE_CONFIG_FILE);
    if(!SaleConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取绑定魔石商店信息
    BindMoneyPointSaleConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, BIND_MONEY_POINT_SALE_CONFIG_FILE);
    if(!BindMoneyPointSaleConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取公会配置
    GuildConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, GUILD_CONFIG_FILE);
    if(!GuildConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    // 读取公会科技配置
    GuildScienceConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, GUILD_SCIENCE_FILE);
    if(!GuildScienceConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    // 读取角色体力配置信息
    VigorConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, VIGOR_CONFIG_FILE);
    if(!VigorConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    // 读取VIP礼包配置信息
    VipRewardConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, VIP_REWARD_CONFIG_FILE);
    if(!VipRewardConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    // 读取VIP时效卡配置信息
    VipCardConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, VIP_CARD_CONFIG_FILE);
    if(!VipCardConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    // 读取VIP配置信息
    VipConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, VIP_CONFIG_FILE);
    if(!VipConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    // 星宫图配置信息
    HoroscopeConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, HOROSCOPE_CONFIG_FILE);
    if(!HoroscopeConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    // 读取合成配置信息
    SynthetizeConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, SYNTHETIZE_CONFIG_FILE);
    if(!SynthetizeConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取荣誉商店表配置信息
    HonorSaleConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, HONOR_SALE_CONFIG_FILE);
    if(!HonorSaleConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取公会商店表配置信息
    GuildSaleConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, GUILD_SALE_CONFIG_FILE);
    if(!GuildSaleConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    Ride_Config::CreateInstance();
    SDSprintf(file_path, "%s%s", path, RIDE_PET_LEVEL_CONFIG_FILE);
    if(!Ride_Config::Instance()->LoadRideLevelConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    SDSprintf(file_path, "%s%s", path, RIDE_PET_CONFIG_FILE);
    if(!Ride_Config::Instance()->LoadRidePetConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    SDSprintf(file_path, "%s%s", path, RIDE_PET_RIDE_CONST_CONFIG_FILE);
    if(!Ride_Config::Instance()->LoadConstConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    NameLimit::CreateInstance();
    SDSprintf(file_path, "%s%s", RES_CONFIG_PATH, NAMELIMITE_CONFIG_FILE);
    if(!NameLimit::Instance()->ReadFile(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取时装配置信息
    FashionConfig::CreateInstance();
    if(!FashionConfig::Instance()->LoadConfig(path))
    {
        return false;
    }

    // 读取抽奖配置信息
    PackageConfig::CreateInstance();
    if(!PackageConfig::Instance()->LoadConfig(path))
    {
        return false;
    }

    //// 读取任务配置信息
    //TaskGameConfig::CreateInstance();
    //SDSprintf(file_path, "%s%s", path, TASK_GAME_CONFIG_FILE);
    //if(!TaskGameConfig::Instance()->LoadConfig(file_path))
    //{
    //    CnFatal("load %s failed\n", file_path);
    //    return false;
    //}

    // 创建活动任务配置(数据来源CenterServer)
    TaskFestiveConfig::CreateInstance();

    // 读取组队副本配置信息
    TeamDungeonConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, TEAMDUNGEON_CONFIG_FILE);    // 组队副本配置信息
    if(!TeamDungeonConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取离线角色战斗配置信息
    BattleOfflineConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, BATTLE_PLAYER_CONFIG_FILE);
    if(!BattleOfflineConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取副本奖励表
    DungeonGainConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, DUNGEON_GAIN_CONFIG_FILE);
    if (!DungeonGainConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取修罗场奖励表
    AshuraConfig::CreateInstance();
    if(!AshuraConfig::Instance()->LoadConfig(path))
    {
        return false;
    }

    WorldBossConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, WORLD_BOSS_CONFIG_FILE);
    if(!WorldBossConfig::Instance()->LoadWorldBossConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取答题表
    AnswerQuestionConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, ANSWER_FILE);
    if (!AnswerQuestionConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取大冒险表
    AdventureConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, ADVENTURE_CONFIG_FILE);
    if (!AdventureConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取补签魔石表
    SignMakeupConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, SIGN_MAKEUP_CONFIG_FILE);
    if (!SignMakeupConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    //读取称号信息
    TitleConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, TITLE_CONFIG_FILE);
    if (!TitleConfig::Instance()->LoadTitleConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取宝库信息
    TreasuryConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, TREASURY_CONFIG_FILE);
    if(!TreasuryConfig::Instance()->LoadTreasuryConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取金像信息
    GoldtreeConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, GOLDTREE_CONFIG_FILE);
    if(!GoldtreeConfig::Instance()->LoadGoldtreeConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取勇者争霸奖励信息
    PlayerDoorRewardConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, PLAYER_DOOR_REWARD_CONFIG_FILE);
    if(!PlayerDoorRewardConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取活动信息
    ActivityConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, ACTIVITY_CONFIG_FILE);
    if(!ActivityConfig::Instance()->LoadActivitySchedule(file_path))
    {
        CnFatal("load %s faild", file_path);
        return false;
    }

    // 读取活动规则限制表
    SDSprintf(file_path, "%s%s", path, ACTIVITY_REGULAR_CONFIG_FILE);
    if(!ActivityConfig::Instance()->LoadActivityRegular(file_path))
    {
        CnFatal("load %s faild", file_path);
        return false;
    }

    // 读取活动奖励配置信息
    ActivityRewardConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, ACTIVITY_REWARD_CONFIG_FILE);
    if(!ActivityRewardConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 特殊活动信息
    SpecialActivityConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, SPECIAL_ACTIVITY_CONFIG_FILE);
    if(!SpecialActivityConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 日常投资信息
    DayInvestmentConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, DAY_INVESTMENT_CONFIG_FILE);
    if(!DayInvestmentConfig::Instance()->LoadDayInvestmentConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 等级投资信息
    LevelInvestmentConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, LEVEL_INVESTMENT_CONFIG_FILE);
    if(!LevelInvestmentConfig::Instance()->LoadLevelInvestmentConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取自动战斗配置信息
    AutoFightConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, AUTO_FIGHT_CONFIG_FILE);
    if(!AutoFightConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s faild", file_path);
        return false;
    }

    // 读取系统设置信息
    SystemSettingConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, SYSTEM_SETTING_CONFIG_FILE);
    if(!SystemSettingConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s faild", file_path);
        return false;
    }

    // 读取虚空幻境挂机设置信息
    VoidFieldSettingConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, VOID_FIELD_SETTING_CONFIG_FILE);
    if(!VoidFieldSettingConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s faild", file_path);
        return false;
    }

    // 读取邮件标题读取
    MailTitleConfig::CreateInstance();
    SDSprintf(file_path,  "%s%s", path, MAIL_TITLE_FILE);
    if(!MailTitleConfig::Instance()->LoadMailTitleInfo(file_path))
    {
        CnFatal("load %s faild", file_path);
        return false;
    }

    // 读取跳舞信息
    DanceConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, DANCE_FILE);
    if(!DanceConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s faild", file_path);
        return false;
    }

    // 读取通用奖励信息
    CommonRewardConfig::CreateInstance();
    SDSprintf(file_path,  "%s%s", path, COMMON_REWARD_FILE);
    if(!CommonRewardConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s faild", file_path);
        return false;
    }

    // 读取活跃度信息
    LivenessConfig::CreateInstance();
    SDSprintf(file_path,  "%s%s", path, LIVENESS_CONFIG_FILE);
    if(!LivenessConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s faild", file_path);
        return false;
    }

    // 读取守卫地宫信息
    TdConfig::CreateInstance();
    SDSprintf(file_path,  "%s%s", path, TOWER_DEFENCE_FILE);
    if(!TdConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s faild", file_path);
        return false;
    }

    // 读取圣剑争辉信息
    GroupPkConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, GROUP_PK_FILE);
    if(!GroupPkConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s faild", file_path);
        return false;
    }

    // 读取圣剑争辉信息
    FactionPkConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, FACTION_PK_FILE);
    if(!FactionPkConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s faild", file_path);
        return false;
    }

    CaptureFlagConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, CAPTURE_FLAG_FILE);
    if(!CaptureFlagConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s faild", file_path);
        return false;
    }

    // 读取圣光生存配置信息
    CitySurvivalConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, CITY_SURVIVAL_CONFIG_FILE);
    if(!CitySurvivalConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 点石成金配置信息
    StoneTurnGoldConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, STONE_TURN_GOLD_CONFIG_FILE);
    if(!StoneTurnGoldConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    //限购信息
    LimitBuyConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, LIMIT_BUY_CONFIG_FILE);
    if(!LimitBuyConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    //vip福利
    VipGiftConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, VIP_GIFT_CONFIG_FILE);
    if(!VipGiftConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("config error %s\n", file_path);
        return false;
    }

    SingleAnswerConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, SINGLE_ANSWER_CONFIG_FILE);
    if(!SingleAnswerConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取角色体力配置信息
    EquipLightingConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, EQUIP_LIGHTING);
    if(!EquipLightingConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    //读取角色圣剑系统配置信息
    SwordConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, SWORD_CONFIG_FILE);
    if(!SwordConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    // 公会任务
    GuildTaskConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, GUILD_TASK);
    if(!GuildTaskConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    // 僵尸生存战
    ZombieConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, ZOMBIE_FILE);
    if (!ZombieConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    // 虚空幻境
    VoidFieldConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, VOID_FIELD_FILE);
    if(!VoidFieldConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    // 虚空幻境SOLO
    VoidFieldSoloConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, VOID_FIELD_SOLO_FILE);
    if(!VoidFieldSoloConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    // 读取勇敢的心信息
    BraveHeartConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, BRAVE_HEART_CONFIG_FILE);
    if(!BraveHeartConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    //礼品卡相关配置
    GiftCodeConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, GIFTSCODE_CONFIG_FILE);
    if (!GiftCodeConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    // 读取游戏收藏配置信息
    ShorcutConfig::CreateInstance();
    if(!ShorcutConfig::Instance()->LoadConfig(path))
    {
        return false;
    }

    // 欢迎奖励
    WelcomeRewardConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, WELCOMEREWARD_CONFIG_FILE);
    if(!WelcomeRewardConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取NPC贩卖配置信息
    ProtectConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, PROTECT_CONFIG_FILE);
    if(!ProtectConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 野外Boss
    WorldMapConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, FIELD_BOSS_FILE);
    if(!WorldMapConfig::Instance()->LoadConfig(file_path))
    {   
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    // 读取跨服pk配置
    MultiserverBattleConfig::CreateInstance();
    if(!MultiserverBattleConfig::Instance()->LoadConfig(path))
    {
        return false;
    }

    // 大乱斗配置文件
    ChaosFactionConfig::CreateInstance();
    if(!ChaosFactionConfig::Instance()->LoadConfig(path))
    {
        return false;
    }

    //背包个开启花费
    BagGridOpenCostConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, BAG_GRID_OPEN_COST_FILE);
    if (!BagGridOpenCostConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    //充值积分抽奖
    PayPointLotteryConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, PAY_POINT_LOTTERY_FILE);
    if (!PayPointLotteryConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    //VIP独享副本
    VIPOnlyDungeonConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, VIP_ONLY_DUNGEON_FILE);
    if (!VIPOnlyDungeonConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    //离线体力
    OffLineVigorConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, OFF_LINE_VIGOR);
    if(!OffLineVigorConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    //捐献
    DonateConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, DONATE);
    if (!DonateConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }

    //砸金蛋
    EggConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, EGG_FILE);
    if (!EggConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }
    
    // 越买越便宜
    BuyCheapConfig::CreateInstance();

    // 每日化身
    DailyFashionConfig::CreateInstance();
    SDSprintf(file_path, "%s%s", path, DAILY_DRAW_FASHION_FILE);
    if (!DailyFashionConfig::Instance()->LoadConfig(file_path))
    {
        return false;
    }



    return true;
}

void ConfigManager::UnInit()
{
    TaskConfigManager::DestroyInstance();
    AlchemyConfigManager::DestroyInstance();
    DesignConfigManager::DestroyInstance();
    ItemConfigManager::DestroyInstance();
    MonsterConfigManager::DestroyInstance();
    PlayerConfigManager::DestroyInstance();
    SkillConfigManager::DestroyInstance();
	PassiveSkillConfigManager::DestroyInstance();
    GemConfigManager::DestroyInstance();
    SystemConfigManager::DestroyInstance();
    DungeonConfigManager::DestroyInstance();

    // !!! 倒序
    DailyFashionConfig::DestroyInstance();
    BuyCheapConfig::DestroyInstance();
    EggConfig::DestroyInstance();
    DonateConfig::DestroyInstance();
    OffLineVigorConfig::DestroyInstance();
    VIPOnlyDungeonConfig::DestroyInstance();
    PayPointLotteryConfig::DestroyInstance();
    BagGridOpenCostConfig::DestroyInstance();
    ChaosFactionConfig::DestroyInstance();
    MultiserverBattleConfig::DestroyInstance();
    WorldMapConfig::DestroyInstance();
    BraveHeartConfig::DestroyInstance();
    VoidFieldSoloConfig::DestroyInstance();
    VoidFieldConfig::DestroyInstance();
    ZombieConfig::DestroyInstance();
    GuildTaskConfig::DestroyInstance();
    SwordConfig::DestroyInstance();
    EquipLightingConfig::DestroyInstance();
    SingleAnswerConfig::DestroyInstance();
    VipGiftConfig::DestroyInstance();
    LimitBuyConfig::DestroyInstance();
    StoneTurnGoldConfig::DestroyInstance();
    CitySurvivalConfig::DestroyInstance();
    //MallConfig::DestroyInstance();
    CaptureFlagConfig::DestroyInstance();
    FactionPkConfig::DestroyInstance();
    GroupPkConfig::DestroyInstance();
    TdConfig::DestroyInstance();
    LivenessConfig::DestroyInstance();
    CommonRewardConfig::DestroyInstance();
    DanceConfig::DestroyInstance();
    MailTitleConfig::DestroyInstance();
    VoidFieldSettingConfig::DestroyInstance();
    SystemSettingConfig::DestroyInstance();
    AutoFightConfig::DestroyInstance();
    LevelInvestmentConfig::DestroyInstance();
    DayInvestmentConfig::DestroyInstance();
    SpecialActivityConfig::DestroyInstance();
    ActivityRewardConfig::DestroyInstance();
    ActivityConfig::DestroyInstance();
    PlayerDoorRewardConfig::DestroyInstance();
    GoldtreeConfig::DestroyInstance();
    TreasuryConfig::DestroyInstance();
    TitleConfig::DestroyInstance();
    WingAttrConfigManager::DestroyInstance();
    MallConfigManager::DestroyInstance();
    SignMakeupConfig::DestroyInstance();
    BounsConfigManager::DestroyInstance();
    AdventureConfig::DestroyInstance();
    AnswerQuestionConfig::DestroyInstance();
    WorldBossConfig::DestroyInstance();
    AshuraConfig::DestroyInstance();
    DungeonGainConfig::DestroyInstance();
    BattleOfflineConfig::DestroyInstance();
    TeamDungeonConfig::DestroyInstance();
    TaskFestiveConfig::DestroyInstance();
//    TaskGameConfig::DestroyInstance();
    PackageConfig::DestroyInstance();
    FashionConfig::DestroyInstance();
    NameLimit::DestroyInstance();
    Ride_Config::DestroyInstance();
    GuildSaleConfig::DestroyInstance();
    HonorSaleConfig::DestroyInstance();
    //NpcConfig::DestroyInstance();
    SynthetizeConfig::DestroyInstance();
    HoroscopeConfig::DestroyInstance();
    VipConfig::DestroyInstance();
    VipCardConfig::DestroyInstance();
    VipRewardConfig::DestroyInstance();
    VigorConfig::DestroyInstance();
    GuildScienceConfig::DestroyInstance();
    GuildConfig::DestroyInstance();
    BindMoneyPointSaleConfig::DestroyInstance();
    SaleConfig::DestroyInstance();
    OnlineRewardConfig::DestroyInstance();
    //OpenSysConfig::DestroyInstance();
    FormulaConfig::DestroyInstance();
    FormulaCal::DestroyInstance();
    ServerConfig::DestroyInstance();
    GiftCodeConfig::DestroyInstance();
    ShorcutConfig::DestroyInstance();
    WelcomeRewardConfig::DestroyInstance();
    ProtectConfig::DestroyInstance();
    CenterConfig::DestroyInstance();
    // !!! 倒序
}

void ConfigManager::SetLanguage(const std::string& language)
{
    if(language.length() == 0)          // 默认中文版本
        m_language = "zh_cn";
    else
        m_language = language;

    if(m_language == "en")
    {
        m_sdb_path = "../res/sdb_en/";
    }
    else if(m_language == "tw")
    {
        m_sdb_path = "../res/sdb_tw/";
    }
    else if(m_language == "ina")
    {
        m_sdb_path = "../res/sdb_ina/";
    }
    else if(m_language == "pt")
    {
        m_sdb_path = "../res/sdb_pt/";
    }
    else if(m_language == "sp")
    {
        m_sdb_path = "../res/sdb_sp/";
    }
    else
    {
        m_sdb_path = "../res/sdb/";
    }

    CnInfo("language=%s\n", m_language.c_str());
    CnInfo("sdb_path=%s\n", m_sdb_path.c_str());
}

bool ConfigManager::IsTW()
{
    if(m_language == "tw")
    {
        return true;
    }
    return false;
}
