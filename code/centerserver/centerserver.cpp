// 游戏服务器
#include "centerserver_pch.h"
#include "centerserver.h"
#include "config_loader.h"
#include "player_config.h"
#include "gate_handler.h"
#include "game_handler.h"
#include "server_info_handler.h"
#include "af_transfer_server.h"
#include "player.h"
#include "player_manager.h"
#include "team_manager.h"
#include "guild_manager.h"
#include "player_net_handler.h"
#include "dungeon_manager.h"
#include "channel_manager.h"
#include "dungeon_score_manager.h"
#include "adventure_manager.h"
#include "trade_manager.h"
#include "pay_manager.h"
#include "dbupdater.h"
#include "dbupdater_global.h"
#include "dbjob.h"
#include "dbdriver.h"
#include "answer_question_manager.h"
#include "guild_manager.h"
#include "activity_manager.h"
#include "world_boss_manager.h"
#include "city_fight_manager_gc.h"
#include "city_survival_manager_gc.h"
#include "mail_manager.h"
#include "mall_manager.h"
#include "ranking_loader_job.h"
#include "ranking_save_job.h"
#include "ranking_list_manager.h"
#include "timing_trigger_manager.h"
#include "mall_config_manager.h"
#include "guild_config.h"
#include "kunlun_login.h"
#include "dg_login.h"
#include "360_login.h"
#include "9377_login.h"
#include "kugou_login.h"
#include "kunlun_log.h"
#include "pusher_360.h"
#include "activecode_job.h"
#include "kunlun_pay.h"
#include "gmt_executor.h"
#include "world_boss_job.h"
#include "activity_reward_config.h"
#include "special_activity_config.h"
#include "special_activity_manager_gc.h"
#include "special_activity_job.h"
#include "mail_title_config.h"
#include "team_manager.h"
#include "guild_manager.h"
#include "namelimit.h"
#include "dongyi_log.h"
#include "vip_config.h"
#include "festival_job.h"
#include "festival_manager.h"
#include "gm_config.h"
#include "guild_science_config.h"
#include "global_setting_manager.h"
#include "global_setting_job.h"
#include "proclamation_manager.h"
#include "global_db_operation.h"
#include "announce_manager.h"
#include "player_info_job.h"
#include "player_load_parser.h"
#include "player_info_parser.h"
#include "player_load_job.h"
#include "player_info_parser.h"
#include "chat_manager.h"
#include "player_save_job.h"
#include "server_monitor.h"
#include "offline_event_job.h"
#include "offline_event_manager.h"
#include "top_up_rank_job.h"
#include "game_setting_job.h"
#include "game_setting_manager.h"
#include "game_common_setting.h"
#include "vip_statistics_manager.h"
#include "donate_manager.h"
#include "global_db_operation.h"
#include "season_manager.h"
#include "multserver_config.h"
#include "platform_callbacker.h"
#include "cross_team_manager.h"
#include "tw_executor.h"
#include "buy_cheap_config.h"
#include "daily_fashion_config.h"
#include "daily_fashion_manager.h"
#include "ranking_manager.h"
#include "random_package_manager.h"
#include "content_dictionary_job.h"

int IsBusy = 0;             // 是否高负载

IMPLEMENT_SINGLETON(CenterServer)

// ==================================================
// 打印LOGO
// ==================================================
void CenterServer::PrintLogo()
{
    Crown::ColorLog::Instance()->ColorPrint(0, FC_WHITE, BC_DARK_GREEN, "+=======================================+\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_GREEN, BC_BLACK, "|   ____      _                         |\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_GREEN, BC_BLACK, "|  |    \\ ___|_|___ ___ ___ _____ ___   |\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_GREEN, BC_BLACK, "|  |  |  | . | |   | . | .'|     | -_|  |\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_GREEN, BC_BLACK, "|  |____/|___|_|_|_|_  |__,|_|_|_|___|  |\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_GREEN, BC_BLACK, "|                  |___|                |\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_WHITE, BC_DARK_GREEN, "+================+======================+\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_GREEN, BC_BLACK, "|  CenterServer  |\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_WHITE, BC_DARK_GREEN, "+================+\n");
}

// ==================================================
// 显示运行状态
// ==================================================
void ShowRunning(uint32 frameCurrent)
{
    static uint32 frameFlash = 0;
    static uint32 RunningCount = 0;

    if(frameFlash > frameCurrent)
        return;

    frameFlash = frameCurrent + 1000;

    uint32 p = RunningCount++ % 3;
    if(p==0)
        if(IsBusy)
            printf("*  \b\b\b");
        else
            printf(".  \b\b\b");
    else if(p==1)
        if(IsBusy)
            printf("** \b\b\b");
        else
            printf(".. \b\b\b");
    else
        if(IsBusy)
            printf("***\b\b\b");
        else
            printf("...\b\b\b");

    if(IsBusy)
        CnInfo("InBusy\n");
}

// ==================================================
// 构造
// ==================================================
CenterServer::CenterServer()
{
    m_isRun = false;
    m_state = kInitState;
    m_InitReady = false;

    m_gameDBExecutor = CnNew DBExecutor;
    m_gmtDBExecutor = CnNew DBExecutor;
    m_globalDBExecutor = CnNew DBExecutor;
}

// ==================================================
// 析构
// ==================================================
CenterServer::~CenterServer()
{
    if(m_globalDBExecutor)
    {
        CnDelete m_globalDBExecutor;
        m_globalDBExecutor = NULL;
    }

    if(m_gmtDBExecutor)
    {
        CnDelete m_gmtDBExecutor;
        m_gmtDBExecutor = NULL;
    }

    if(m_gameDBExecutor)
    {
        CnDelete m_gameDBExecutor;
        m_gameDBExecutor = NULL;
    }
}

bool CenterServer::Init()
{
    CnInfo("CenterServer init pid = %d\n", Crown::SDGetCurrentProcessId());

    // 初始化配置文件
    if(!InitConfig())
    {
        CnError("Init config falied\n");
        return false;
    }

    // 初始化数据库事务
    if(!InitDB())
    {
        CnError("Init db falied\n");
        return false;
    }

    if(!InitSvrHandler())
    {
        CnError("Init server handler falied\n");
        return false;
    }

    if(!InitManager())
    {
        CnError("Init manager falied\n");
        return false;
    }

    if(!InitLogSys())
    {
        CnError("Init log system falied\n");
        return false;
    }

    if(!InitExtFunc())
    {
        CnError("Init extend function falied\n");
        return false;
    }

    m_isRun = true;

    return true;
}

void CenterServer::UnInit()
{
    UnInitManager();

    UnInitSvrHandler();

    UnInitExtFunc();

    UnInitDB();

    UnInitLogSys();

    UnInitConfig();
}

// ==================================================
// 初始化配置文件
// ==================================================
bool CenterServer::InitConfig()
{
    // 读取角色初始数据文件
    PlayerConfig::CreateInstance();
    if(!PlayerConfig::Instance()->LoadConfig())
    {
        CnFatal("player config error \n");
        return false;
    }

    // 读取配置文件
    std::string configFile = RES_CONFIG_PATH""CENTER_CONFIG_FILE;
    ConfigLoader::CreateInstance();
    if(!ConfigLoader::Instance()->LoadConfig(configFile.c_str()))
    {
        CnFatal("config error %s\n", configFile.c_str());
        return false;
    }

    char file_path[SD_MAX_PATH];
    char path [SD_MAX_PATH];
    SDSprintf(path, "%s", ConfigLoader::Instance()->GetSdbPath().c_str());

    // 读取名字限制
    //SDSprintf(file_path, "%s%s", RES_CONFIG_PATH, NAMELIMITE_CONFIG_FILE);
    SDSprintf(file_path, "%s%s", path, NAMELIMITE_CONFIG_FILE);
    NameLimit::CreateInstance();
    if(!NameLimit::Instance()->ReadFile(file_path))
    {
        CnFatal("config error %s\n", file_path);
        return false;
    }

    // 读取邮件标题读取
    SDSprintf(file_path, "%s%s", path, MAIL_TITLE_FILE);
    MailTitleConfig::CreateInstance();
    if(!MailTitleConfig::Instance()->LoadMailTitleInfo(file_path))
    {
        CnFatal("load %s faild", file_path);
        return false;
    }

    // 读取商城信息
    MallConfigManager::CreateInstance();
    if(!MallConfigManager::Instance()->LoadConfig(path))
    {
        CnFatal("config error %s\n", path);
        return false;
    }

    // 读取公会信息
    SDSprintf(file_path, "%s%s", path, GUILD_CONFIG_FILE);
    GuildConfig::CreateInstance();
    if(!GuildConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("config error %s\n", file_path);
        return false;
    }

    // 读取公会科技信息
    SDSprintf(file_path, "%s%s", path, GUILD_SCIENCE_FILE);
    GuildScienceConfig::CreateInstance();
    if(!GuildScienceConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("config error %s\n", file_path);
        return false;
    }

    // 读取VIP配置信息
    SDSprintf(file_path, "%s%s", path, VIP_CONFIG_FILE);
    VipConfig::CreateInstance();
    if(!VipConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("config error %s\n", file_path);
        return false;
    }

    // 读取活动奖励信息
    SDSprintf(file_path, "%s%s", path, ACTIVITY_REWARD_CONFIG_FILE);
    ActivityRewardConfig::CreateInstance();
    if(!ActivityRewardConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("config error %s\n", file_path);
        return false;
    }

    // 读取特殊活动信息
    SDSprintf(file_path, "%s%s", path, SPECIAL_ACTIVITY_CONFIG_FILE);
    SpecialActivityConfig::CreateInstance();
    if(!SpecialActivityConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("config error %s\n", file_path);
        return false;
    }

    //gm相关配置
    std::string gmConfigFile = RES_CONFIG_PATH""GM_CONFIG_FILE;
    GMConfig::CreateInstance();
    if (!GMConfig::Instance()->LoadConfig(gmConfigFile.c_str()))
    {
        CnFatal("config error %s\n", gmConfigFile.c_str());
        return false;
    }

    //通用设置
    SDSprintf(file_path, "%s%s", path, GAME_COMMON_SETTING);
    GameCommonSetting::CreateInstance();
    if (!GameCommonSetting::Instance()->LoadConfig(file_path))
    {
        CnFatal("config error %s\n", file_path);
        return false;
    }

    // 跨服设置
    SDSprintf(file_path, "%s%s", path, MULTISERVER_BATTLE_FILE);
    MultserverConfig::CreateInstance();
    if (!MultserverConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("config error %s\n", file_path);
        return false;
    }

    // 越买越便宜
    SDSprintf(file_path, "%s%s", path, BUY_CHEAP_CONFIG_FILE);
    BuyCheapConfig::CreateInstance();
    if (!BuyCheapConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("config error %s\n", file_path);
        return false;
    }

    // 每日化身
    SDSprintf(file_path, "%s%s", path, DAILY_DRAW_FASHION_FILE);
    DailyFashionConfig::CreateInstance();
    if (!DailyFashionConfig::Instance()->LoadConfig(file_path))
    {
        CnFatal("config error %s\n", file_path);
        return false;
    }

    return true;
}

// ==================================================
// 销毁配置文件
// ==================================================
void CenterServer::UnInitConfig()
{
    DailyFashionConfig::DestroyInstance();

    BuyCheapConfig::DestroyInstance();

    MultserverConfig::DestroyInstance();

    GameCommonSetting::DestroyInstance();

    GMConfig::DestroyInstance();

    SpecialActivityConfig::DestroyInstance();

    ActivityRewardConfig::DestroyInstance();

    VipConfig::DestroyInstance();

    GuildScienceConfig::DestroyInstance();

    GuildConfig::DestroyInstance();

    MallConfigManager::DestroyInstance();

    MailTitleConfig::DestroyInstance();

    NameLimit::DestroyInstance();

    ConfigLoader::DestroyInstance();

    PlayerConfig::DestroyInstance();

    GMConfig::DestroyInstance();
}

// ==================================================
// 初始化处理句柄
// ==================================================
bool CenterServer::InitSvrHandler()
{
    // 创建登陆服务器处理实例
    GateHandler::CreateInstance();
    ServerConfig& gateConfig = ConfigLoader::Instance()->GetListenGate();
    CnInfo("listen gate %s:%d\n", gateConfig.m_ip.c_str(), gateConfig.m_port);
    if(!GateHandler::Instance()->Init(gateConfig.m_ip.c_str(), gateConfig.m_port))
    {
        CnError("Init GateHandler failed\n");
        return false;
    }

    // 创建游戏服务器处理实例
    GameHandler::CreateInstance();
    ServerConfig& gameConfig = ConfigLoader::Instance()->GetListenGame();
    CnInfo("listen game %s:%d\n", gameConfig.m_ip.c_str(), gameConfig.m_port);
    if(!GameHandler::Instance()->Init(gameConfig.m_ip.c_str(), gameConfig.m_port))
    {
        CnError("Init GameHandler failed\n");
        return false;
    }

    // 创建监控服务器处理实例
    ServerInfoHandler::CreateInstance();
    ServerConfig& infoConfig = ConfigLoader::Instance()->GetListenInfo();
    if (!ServerInfoHandler::Instance()->Init(gateConfig.m_ip.c_str(), infoConfig.m_port))
    {
        CnError("Init ServerInfoHandler failed\n");
        return false;
    }

    return true;
}

// ==================================================
// 销毁处理句柄
// ==================================================
void CenterServer::UnInitSvrHandler()
{
    // 监控服务器处理实例
    ServerInfoHandler::DestroyInstance();

    // 销毁游戏服务器处理实例
    GameHandler::DestroyInstance();

    // 销毁登陆服务器处理实例
    GateHandler::DestroyInstance();
}

// ==================================================
// 初始化数据库事务
// ==================================================
bool CenterServer::InitDB()
{
    // 初始化Center数据库
    DBConfig& centerDBConf = ConfigLoader::Instance()->GetCenterDBConfig();
    std::string gameDBSemaName = "center_dbdriver_" + Crown::ToString(Crown::SDGetCurrentProcessId());
    if(!m_gameDBExecutor->Init(gameDBSemaName.c_str(), centerDBConf.m_threadNum, centerDBConf.m_db.c_str(), 
        centerDBConf.m_ip.c_str(), centerDBConf.m_user.c_str(), centerDBConf.m_passwd.c_str(), centerDBConf.m_port, 
        centerDBConf.m_charset.c_str()))
    {
        CnFatal("Init center db failed %s %s\n", centerDBConf.m_db.c_str(), centerDBConf.m_ip.c_str());
        return false;
    }

    CnInfo("Connected db %s %s:%u '%s' ok!\n", centerDBConf.m_db.c_str(), centerDBConf.m_ip.c_str(), 
        centerDBConf.m_port, centerDBConf.m_charset.c_str());

    // 初始化GMT据库
    DBConfig& gmtDBConf = ConfigLoader::Instance()->GetGmtDBConfig();
    std::string gmtDBSemaName = "center_gmt_dbdriver_" + Crown::ToString(Crown::SDGetCurrentProcessId());
    if(!m_gmtDBExecutor->Init(gmtDBSemaName.c_str(), gmtDBConf.m_threadNum, gmtDBConf.m_db.c_str(), 
        gmtDBConf.m_ip.c_str(), gmtDBConf.m_user.c_str(), gmtDBConf.m_passwd.c_str(), gmtDBConf.m_port, 
        gmtDBConf.m_charset.c_str()))
    {
        CnFatal("Init gmt db failed %s %s\n", gmtDBConf.m_db.c_str(), gmtDBConf.m_ip.c_str());
        return false;
    }
    CnInfo("Connected db %s %s:%u '%s' ok!\n", gmtDBConf.m_db.c_str(), gmtDBConf.m_ip.c_str(), 
        gmtDBConf.m_port, gmtDBConf.m_charset.c_str());

    // 初始化global据库
    DBConfig& globalDBConf = ConfigLoader::Instance()->GetGlobalDBConfig();
    std::string globalDBSemaName = "center_global_dbdriver_" + Crown::ToString(Crown::SDGetCurrentProcessId());
    if(!m_globalDBExecutor->Init(globalDBSemaName.c_str(), globalDBConf.m_threadNum, globalDBConf.m_db.c_str(), 
        globalDBConf.m_ip.c_str(), globalDBConf.m_user.c_str(), globalDBConf.m_passwd.c_str(), globalDBConf.m_port, 
        globalDBConf.m_charset.c_str()))
    {
        CnFatal("Init global db failed %s %s\n", globalDBConf.m_db.c_str(), globalDBConf.m_ip.c_str());
        return false;
    }

    CnInfo("Connected db %s %s:%u '%s' ok!\n", globalDBConf.m_db.c_str(), globalDBConf.m_ip.c_str(), 
        globalDBConf.m_port, globalDBConf.m_charset.c_str());

    // 检查数据库的版本信息
    DBUpdater::CreateInstance();
    DBUpdater::Instance()->Init(m_gameDBExecutor);      // 初始化
    if(!DBUpdater::Instance()->CheckVersionTable())     // 检查数据库版本
    {
        CnFatal("Update db failed\n");
        return false;
    }

    // 检测跨服数据库的版本信息
    DBUpdaterGlobal::CreateInstance();
    DBUpdaterGlobal::Instance()->Init(m_globalDBExecutor);
    if(!DBUpdaterGlobal::Instance()->CheckVersionTable())
    {
        CnFatal("update global db failed\n");
        return false;
    }

    // 初始化角色数据库操作
    PlayerJob::CreateInstance();
    PlayerJob::Instance()->SetDBExecutor(m_gameDBExecutor);

/////////////////////////
    // 解析各个模块数据
    PlayerLoadParser::CreateInstance();

    // 解析个人属性
    PlayerInfoParser::CreateInstance();

    // 角色数据
    PlayerInfoJob::CreateInstance();
    PlayerInfoJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    // 加载角色数据
    PlayerLoadJob::CreateInstance();
    PlayerLoadJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    // 初始化玩家信息数据库操作
    PlayerSaveJob::CreateInstance();
    PlayerSaveJob::Instance()->SetDBExecutor(m_gameDBExecutor);

////////////////////////////
    // 初始化文本字典
    ContentDictionaryJob::CreateInstance();
    ContentDictionaryJob::Instance()->SetDBExecutor(m_gameDBExecutor);


    // 初始化邮件数据库操作
    MailJob::CreateInstance();
    MailJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    // 初始化商城数据库操作
    MallJob::CreateInstance();
    MallJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    // 初始化公会数据库操作
    GuildJob::CreateInstance();
    GuildJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    // 初始化活动数据库操作
    ActivityJob::CreateInstance();
    ActivityJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    // 激活码数据操作
    ActiveCodeJob::CreateInstance();
    ActiveCodeJob::Instance()->SetDBExecutor(m_gameDBExecutor, m_globalDBExecutor);

    // 初始化离线玩家数据库操作
    OfflinePlayerJob::CreateInstance();
    OfflinePlayerJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    // 充值数据库操作
    PayJob::CreateInstance();
    PayJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    // 初始化商城数据库操作
    TradeJob::CreateInstance();
    TradeJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    // 初始化排行榜数据库加载
    RankingLoaderJob::CreateInstance();
    RankingLoaderJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    // 初始化排行榜数据库操作
    RankingSaveJob::CreateInstance();
    RankingSaveJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    // 初始化副本数据库操作
    DungeonJob::CreateInstance();
    DungeonJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    // 初始化世界BOSS数据库操作
    WorldBossJob::CreateInstance();
    WorldBossJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    // 初始化特殊活动数据库操作
    SpecialActivityJob::CreateInstance();
    SpecialActivityJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    // 初始化节日数据库操作
    FestivalJob::CreateInstance();
    FestivalJob::Instance()->SetDBExecutor(m_gameDBExecutor, m_gmtDBExecutor);
    
    // 初始化游戏设置数据库操作
    GameSettingJob::CreateInstance();
    GameSettingJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    // 初始化配置数据库操作
    GlobalSettingJob::CreateInstance();
    GlobalSettingJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    // 初始化离线事件数据库
    OfflineEventJob::CreateInstance();
    OfflineEventJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    //充值排行查询
    TopUpRankJob::CreateInstance();
    TopUpRankJob::Instance()->SetDBExecutor(m_gameDBExecutor);

    //操作全局的数据库
    GlobalDbOperation::CreateInstance();
    GlobalDbOperation::Instance()->SetDBExecutor(m_globalDBExecutor);

    return true;
}

// ==================================================
// 销毁数据库事务
// ==================================================
void CenterServer::UnInitDB()
{
    // 初始化配置数据库操作
    GlobalDbOperation::DestroyInstance();

    TopUpRankJob::DestroyInstance();

    OfflineEventJob::DestroyInstance();

    GlobalSettingJob::DestroyInstance();

    GameSettingJob::DestroyInstance();

    FestivalJob::DestroyInstance();

    SpecialActivityJob::DestroyInstance();

    WorldBossJob::DestroyInstance();

    DungeonJob::DestroyInstance();

    RankingLoaderJob::DestroyInstance();

    TradeJob::DestroyInstance();

    PayJob::DestroyInstance();

    OfflinePlayerJob::DestroyInstance();

    // 活动DB实例
    ActiveCodeJob::DestroyInstance();

    ActivityJob::DestroyInstance();

    GuildJob::DestroyInstance();

    MallJob::DestroyInstance();

    MailJob::DestroyInstance();

    ContentDictionaryJob::DestroyInstance();

    PlayerSaveJob::DestroyInstance();

    PlayerLoadJob::DestroyInstance();

    PlayerInfoJob::DestroyInstance();

    PlayerInfoParser::DestroyInstance();

    PlayerLoadParser::DestroyInstance();

    PlayerJob::DestroyInstance();

    DBUpdater::DestroyInstance();

    DBUpdaterGlobal::DestroyInstance();
}

// ==================================================
// 初始化管理器
// ==================================================
bool CenterServer::InitManager()
{
    // 平台回调接口
    InitPlatformCallbacker();

    // 角色管理实例
    PlayerManager::CreateInstance();

    // 单服队伍管理实例
    TeamManager::CreateInstance();

    // 随机包裹
    RandomPackageManager::CreateInstance();

    //// 跨服组队管理器
    //CrossTeamManager::CreateInstance();

    //// 公会管理实例
    //GuildManager::CreateInstance();

    //// 答题管理实例
    //AnswerManager::CreateInstance();

    //// 地图管理实例
    DungeonManager::CreateInstance();
    if(!DungeonManager::Instance()->Init())
    {
        CnError("Init DungeonManager failed\n");
        return false;
    }

    //// 副本积分管理器
    //DungeonScoreManager::CreateInstance();
    //DungeonScoreManager::Instance()->Init();

    // 创建活动管理实例
    ActivityManager::CreateInstance();

    //// 城市争夺管理实例
    //CityFightManager::CreateInstance();
    //CityFightManager::Instance()->Init();

    //// 圣光生存战管理实例
    //CitySurvivalManager::CreateInstance();

    //// 大冒险管理实例
    //AdventureManager::CreateInstance();

    //// 邮件管理实例
    //MailManager::CreateInstance();

    //// 商城管理实例
    MallManager::CreateInstance();

    //// 检查数据库中的商品版本号，检查是否更新
    MallManager::Instance()->UpdateDBVersion();

    //// 加载数据库中的商品信息
    MallManager::Instance()->LoadDBMallInfo();

    //// 交易管理实例
    //TradeManager::CreateInstance();

    //// 冲魔石管理实例
    //PayManager::CreateInstance();

    // 活动任务管理实例
    FestivalManager::CreateInstance();

    // 加载活动数据
    FestivalJob::Instance()->LoadLocalFestival();

    //// vip领域管理器
    //VipStatisticsManager::CreateInstance();
    //VipStatisticsManager::Instance()->Init();

    //// 捐献管理器
    //DonateManager::CreateInstance();
    //DonateManager::Instance()->Init();

    //// 设置管理实例
    //GameSettingManager::CreateInstance();
    //// 初始化游戏设置
    //GameSettingJob::Instance()->InitGameSettingInfo();

    // 排行榜实例
    RankManager::CreateInstance();
    RankManager::Instance()->Init();

    //// 特殊活动管理器
    //SpecialActivityManager::CreateInstance();

    //// 创建全局配置
    //GlobalSettingManager::CreateInstance();
    //GlobalSettingManager::Instance()->Init();
    //
    //// 警示公告
    //ProclamationManager::CreateInstance();
    //ProclamationManager::Instance()->Init();

    //// 公告管理器
    //AnnouceManager::CreateInstance();

    //// 聊天管理器
    ChatManager::CreateInstance();

    // 服务器状态监控
    ServerMonitor::CreateInstance();

    //// 离线事件管理器
    //OfflineEventManager::CreateInstance();

    //// 赛季管理器
    //SeasonManager::CreateInstance();
    //SeasonManager::Instance()->Init();

    //// 时间触发器
    //TimingTriggerManager::CreateInstance();

    //// 每日化身
    //DailyFashionManager::CreateInstance();
    //DailyFashionManager::Instance()->Init();

    return true;
}

// ==================================================
// 销毁管理器
// ==================================================
void CenterServer::UnInitManager()
{
    // 每日化身
    DailyFashionManager::DestroyInstance();

    // 时间触发器
    TimingTriggerManager::DestroyInstance();

    SeasonManager::DestroyInstance();

    // 离线事件管理器
    OfflineEventManager::DestroyInstance();

    // 监控服务器处理实例
    ServerMonitor::DestroyInstance();

    // 聊天管理器
    ChatManager::DestroyInstance();

    // 公告管理器
    AnnouceManager::DestroyInstance();

    // 警示公告
    ProclamationManager::DestroyInstance();

    // 创建全局配置
    GlobalSettingManager::DestroyInstance();

    // 特殊活动管理器
    SpecialActivityManager::DestroyInstance();

    // 排行榜管理实例
    RankListManager::DestroyInstance();
    
    // 设置管理
    GameSettingManager::DestroyInstance();

    // 捐献管理器
    DonateManager::DestroyInstance();

    // vip领域管理器
    VipStatisticsManager::DestroyInstance();

    // 活动任务管理器
    FestivalManager::DestroyInstance();

    // 冲魔石管理实例
    PayManager::DestroyInstance();

    // 交易管理实例
    TradeManager::DestroyInstance();

    // 商城管理实例
    MallManager::DestroyInstance();

    // 邮件管理实例
    MailManager::DestroyInstance();

    // 大冒险管理实例
    AdventureManager::DestroyInstance();

    // 圣光生存战管理
    CitySurvivalManager::DestroyInstance();

    // 城市争夺管理
    CityFightManager::DestroyInstance();

    // 创建活动管理
    ActivityManager::DestroyInstance();

    // 世界BOSS实例
    WorldBossManager::DestroyInstance();

    // 副本积分管理器
    DungeonScoreManager::DestroyInstance();

    // 副本管理实例
    DungeonManager::DestroyInstance();

    // 答题管理实例
    AnswerManager::DestroyInstance();

    // 公会管理实例
    GuildManager::DestroyInstance();

    // 跨服组队管理器
    CrossTeamManager::DestroyInstance();

    // 随机包裹
    RandomPackageManager::DestroyInstance();

    // 单服组队管理实例
    TeamManager::DestroyInstance();

    // 玩家管理实例
    PlayerManager::DestroyInstance();

    // 平台回调接口
    UninitPlatformCallbacker();
}

// ==================================================
// 初始化日志系统
// ==================================================
bool CenterServer::InitLogSys()
{
    // 昆仑日志
    KunlunLog::CreateInstance();
    if(!KunlunLog::Instance()->Init())
    {
        CnFatal("kunlun log init failed\n");
        return false;
    }

    // 动艺日志
    DoingGameLog::CreateInstance();
    if (!DoingGameLog::Instance()->Init())
    {
        CnFatal("dongyi log init failed\n");
        return false;
    }

    return true;
}

// ==================================================
// 销毁日志系统
// ==================================================
void CenterServer::UnInitLogSys()
{
    KunlunLog::DestroyInstance();

    DoingGameLog::DestroyInstance();
}

// ==================================================
// 初始化扩展功能
// ==================================================
bool CenterServer::InitExtFunc()
{
    ContentDictionaryJob::Instance()->LoadInfo();

    if((ConfigLoader::Instance()->GetLoginMode() == LOGIN_MODE_KUNLUN) ||           // 昆仑网站登陆模式
        (ConfigLoader::Instance()->GetLoginMode() == LOGIN_MODE_KUNLUN_CDKEY))       // 昆仑网站激活码登陆模式
    {
        // 昆仑登陆模块实例
        KlLoginExecutor::CreateInstance();
        if(!KlLoginExecutor::Instance()->Init(ConfigLoader::Instance()->GetKunlunThreadNum()))
        {
            CnError("Init KlLoginExecutor failed\n");
            return false;
        }
    }

    // 昆仑支付模块实例
    KlPayExecutor::CreateInstance();
    KlPayExecutor::Instance()->Init(ConfigLoader::Instance()->GetKunlunPayIp(), ConfigLoader::Instance()->GetKunlunPayPort());

    // 平台回调实例
    Pusher360Executor::CreateInstance();
    if(!Pusher360Executor::Instance()->Init(ConfigLoader::Instance()->GetPush360ThreadNum()))
    {
        CnError("Init Pusher360Executor failed\n");
        return false;
    }

    // GMT 工具
    GmtExecutor::CreateInstance();
    GmtExecutor::Instance()->Init(ConfigLoader::Instance()->GetGmtIP(), ConfigLoader::Instance()->GetGmtPort());

    // TW 接聊天
    TwExecutor::CreateInstance();
    TwExecutor::Instance()->Init(ConfigLoader::Instance()->GetTwIP().c_str(), ConfigLoader::Instance()->GetTwPort());

    return true;
}

// ==================================================
// 销毁扩展功能
// ==================================================
void CenterServer::UnInitExtFunc()
{
    // TW 接聊天
    TwExecutor::Instance()->Unint();
    TwExecutor::DestroyInstance();

    // GMT模块实例
    GmtExecutor::DestroyInstance();

    // 平台回调
    Pusher360Executor::DestroyInstance();

    // 昆仑支付模块实例
    KlPayExecutor::DestroyInstance();

    if((ConfigLoader::Instance()->GetLoginMode() == LOGIN_MODE_KUNLUN) ||           // 昆仑网站登陆模式
        (ConfigLoader::Instance()->GetLoginMode() == LOGIN_MODE_KUNLUN_CDKEY))       // 昆仑网站激活码登陆模式
    {
        // 昆仑登陆模块实例
        KlLoginExecutor::DestroyInstance();
    }
}

void CenterServer::Run()
{
    uint32 frameNext = 0;      // 下一帧滴答
    uint32 frameCurrent = 0;   // 当前帧滴答

    frameNext = frameCurrent = Crown::SDGetTickCount();

    // 主循环
    while (m_isRun)
    {
        frameCurrent = Crown::SDGetTickCount();

        ShowRunning(frameCurrent);  // 终端进度条
        int32 gapTick = frameNext - frameCurrent;
        if(frameNext <= frameCurrent)
        {
            Crown::ColorLog::Instance()->Update();

            // 昆仑日志
            KunlunLog::Instance()->Update(frameCurrent);
            // 昆仑支付更新
            KlPayExecutor::Instance()->Update();

            if((ConfigLoader::Instance()->GetLoginMode() == LOGIN_MODE_KUNLUN) ||           // 昆仑网站登陆模式
               (ConfigLoader::Instance()->GetLoginMode() == LOGIN_MODE_KUNLUN_CDKEY))       // 昆仑网站激活码登陆模式
            {
                // 昆仑登陆更新
                KlLoginExecutor::Instance()->Update();
            }

            // 动艺日志
            DoingGameLog::Instance()->Update();
            // GMT 更新
            GmtExecutor::Instance()->Update();
            // 平台回调
            Pusher360Executor::Instance()->Update();
            // 角色更新
            PlayerManager::Instance()->Update();
            // 网络更新
            UpdateNet();
            // 数据库更新
            UpdateDB();

            ////更新公告
            //AnnouceManager::Instance()->Update();
            //VipStatisticsManager::Instance()->Update();
            //DonateManager::Instance()->Update();
            //GlobalDbOperation::Instance()->Update();
            //SeasonManager::Instance()->Update();

            // 排行更新
            RankManager::Instance()->Update();

            if((frameNext + g_ServerUpdateTimeBusy) >= frameCurrent)
            {
                frameNext += g_ServerUpdateTime;
                if(IsBusy > 0) --IsBusy;
            }
            else
            {
                if((frameNext + g_ServerUpdateTimeThreshold) >= frameCurrent)
                {
                    // 超过阀值太多，不再追赶心跳
                    frameNext = frameCurrent + g_ServerUpdateTime;
                }
                else
                {
                    // 可能是处理的抖动，看看后续心跳是否能追赶上
                    frameNext += g_ServerUpdateTime;
                }

                if(IsBusy < 10) ++IsBusy;   // update超时，进入高负载状态
            }
        }
        else
        {
            Crown::SDSleep( gapTick );
        }
    }
}

void CenterServer::onEnd()
{
    ContentDictionaryJob::Instance()->SaveInfo();
    //VipStatisticsManager::Instance()->SaveVipStatistics();      // 保存vip领域数据
    //DonateManager::Instance()->SaveDB();                        // 保存捐献数据
    //SeasonManager::Instance()->SaveDB();                        // 保存赛季数据
}

void CenterServer::UpdateNet()
{
    TransferUnpdate();
}

void CenterServer::UpdateDB()
{
    m_gameDBExecutor->Update();
    m_gmtDBExecutor->Update();
    m_globalDBExecutor->Update();
}

int CenterServer::Send(uint32 sessionID, uint32 msgID, void* data)
{
    if(TransferSend(sessionID, msgID, data))
    {
        return -1;
    }
    return 0;
}

// ==================================================
// 收到的服务器列表请求
// ==================================================
void CenterServer::RecvServerInfoReq(uint32 sessionID, void* data)
{
    SERVER_INFO_REQ* recvMsg = (SERVER_INFO_REQ*)data;
    uint16 errCode = ERR_FAILED;
    uint32 port = 0;
    std::string ip = "";
    std::string outer_ip = "";
    ServerConfig* config = NULL;

    if(!recvMsg)
    {
        return;
    }

    if(recvMsg->version != VERSION_FULL)
    {
        errCode = ERR_VERSION_ERROR;
        CnWarn("server type:%u, id:%u version error\n", recvMsg->server_type, recvMsg->server_id);
        goto _END;
    }

    if(recvMsg->center_id == 0)
    {
        recvMsg->center_id = ConfigLoader::Instance()->GetCenterID();
    }
    else
    {
        CnWarn("RecvServerInfoReq center_id != 0\n");           // 其他服务器未连接Center时不应该知道centerID
        CnAssert(recvMsg->center_id == ConfigLoader::Instance()->GetCenterID());
    }

    // 发送Req请求，代表服务器告诉中心服务器，我已经启动，请发送其他服务器的列表
    // 此时中心服务器会将发送请求的服务器设置为激活状态，请求服务器的监听Port是通过中心服务器获得的
    // 所以在发送回的列表中，会包含这个服务器自己的监听Port和IP

    if((recvMsg->server_type == SERVER_TYPE_GAME) && (recvMsg->game_type == GAMESVR_TYPE_CROSS))
    {   // 跨服GameServer，则需要新加配置项
        if(!ConfigLoader::Instance()->AddServerConfig(recvMsg))
        {
            goto _END;
        }
    }

    // 在配置项中查找
    config = ConfigLoader::Instance()->GetServerConfig(recvMsg->server_type, recvMsg->server_id);
    if(!config)
    {
        CnError("Can't find server config type:%u, id:%u\n", recvMsg->server_type, recvMsg->server_id);
        goto _END;
    }
    
    port = config->m_port;
    ip = config->m_ip;          // 告知监听IP(内网地址)
    outer_ip = config->m_outer_ip;

    errCode = ERR_SUCCEED;
    CnDbg("SendServerInfoAck type=%d id=%d %s(%s):%d\n", recvMsg->server_type, recvMsg->server_id, ip.c_str(), outer_ip.c_str(), port);

_END:
    SendServerInfoAck(sessionID, errCode, port, ip, outer_ip);
}

// ==================================================
// 发送服务器列表
// ==================================================
void CenterServer::SendServerInfoAck(uint32 sessionID, uint16 errCode, uint32 port, const std::string& ip, const std::string& outer_ip)
{
    SERVER_INFO_ACK sendMsg;

    sendMsg.errcode = errCode;
    sendMsg.server_port = port;
    sendMsg.server_ip = ip;
    sendMsg.server_outer_ip = outer_ip;
    sendMsg.center_id = ConfigLoader::Instance()->GetCenterID();

    Send(sessionID, MSG_SERVER_INFO_ACK, &sendMsg);
}

// ==================================================
// 
// ==================================================
void CenterServer::RecvServerActiveReq(uint32 sessionID, void* data)
{
    SERVER_ACTIVE_REQ* recvMsg = (SERVER_ACTIVE_REQ*)data;
    uint16 errCode = ERR_FAILED;

    if(!recvMsg) return;

    uint32 CenterID = recvMsg->center_id;
    if(CenterID == 0)
    {   // 本组服的服务器请求激活
        CenterID = ConfigLoader::Instance()->GetCenterID();
    }
    else
    {
        CnAssert(CenterID != ConfigLoader::Instance()->GetCenterID());      // 非本組服務器CenterID不應和本組CenterID相同
    }

    errCode = ActiveServer(recvMsg->server_type, recvMsg->server_id, CenterID);
    if(errCode == ERR_SUCCEED)
    {
        if(SERVER_TYPE_GATE == recvMsg->server_type)
        {
            uint32 cent_gate_id = CalRegionID(CenterID, recvMsg->server_id);    // 因为跨服,保存全局GateServerID
            GateHandler::Instance()->RegistRegionID(cent_gate_id, sessionID);
        }
        else if(SERVER_TYPE_GAME == recvMsg->server_type)
        {
            uint32 cent_game_id = CalRegionID(CenterID, recvMsg->server_id);
            GameHandler::Instance()->RegistRegionID(cent_game_id, sessionID);
        }

        errCode = ERR_SUCCEED;
    }

    // 通知激活消息
    SendServerActiveAck(sessionID, errCode);

    // 如果没成功则不需要发下面的消息
    if(errCode != ERR_SUCCEED) 
        return;

    // 只有Gate需要获得其他服务器列表，因为是由Gate主动连接Game
    if(SERVER_TYPE_GATE == recvMsg->server_type)
    {
        SendServerListAck(sessionID, recvMsg->server_type);
    }
    else if(SERVER_TYPE_GAME == recvMsg->server_type)
    {
        SendServerListAck(sessionID, recvMsg->server_type);

        // 通知商城信息
        MallManager::Instance()->SendMallInfoToGameSvr(sessionID);

        // 通知随机包信息
        RandomPackageManager::Instance()->SendRecordInfo(sessionID);

        //// 通知公会信息
        //GuildManager::Instance()->SendAllGuildInfoToGameSvr(sessionID);

        //// 通知队伍信息
        //TeamManager::Instance()->SendAllTeamInfoToGameServer(sessionID);

        // 通知活动信息
        FestivalManager::Instance()->SendFestivalListToGameSvr(sessionID);

        //// 通知游戏设置信息
        //GameSettingManager::Instance()->SendGameSettingListToGameSvr(sessionID);

        //// 通知Top榜信息
        //RankManager::Instance()->SendTopRankToGameSvr(sessionID);

        //// 通知vip领域
        //VipStatisticsManager::Instance()->SendVipPlayerNum();

        //// 通知赛季信息
        //SeasonManager::Instance()->SendInfo(sessionID);
    }

    if(!m_InitReady)
    {
        if(ConfigLoader::Instance()->IsActiveAllServer())
        {
            CnInfo("ready\n");
            // 第一次所有Server准备完毕，发送ready通知
            m_InitReady = true;

            ServerMonitor::Instance()->OnAllServerReady();

            NONE none;
            GateHandler::Instance()->SendToAll(MSG_SERVER_ALL_READY, &none);
            GameHandler::Instance()->SendToAll(MSG_SERVER_ALL_READY, &none);
        }
    }
    else
    {   // 某Server再次连接上，发送ready通知
        NONE none;
        Send(sessionID, MSG_SERVER_ALL_READY, &none);
    }
}

uint16 CenterServer::ActiveServer(uint8 serverType, uint32 serverID, uint32 centerID)
{
    return ConfigLoader::Instance()->ActivateServer(serverType, serverID, centerID);
}

void CenterServer::UnactiveServer(uint8 serverType, uint32 serverID, uint32 centerID)
{
    ConfigLoader::Instance()->UnactiveServer(serverType, serverID, centerID);
}
// ==================================================
// 接收登陆请求
// ==================================================
void CenterServer::RecvLoginGameReq(uint64 loginID, uint32 sessionID, void* data)
{
    LOGIN_GAME_REQ* recvMsg = (LOGIN_GAME_REQ*)data;
    if(!recvMsg) return;

    uint32 centGateID = GateHandler::Instance()->GetRegionIDBySessionID(sessionID);

    // 版本号检查
    uint32 version = VERSION_FULL;
    if(recvMsg->version != version)
    {
        
            CnInfo("player %s:%u version=%d.%d.%d.%d server_version=%d.%d.%d.%d error\n", Crown::GetIpStr(recvMsg->addr_ip).c_str(), recvMsg->addr_port,
            GET_VERSION_MAJOR(recvMsg->version),
            GET_VERSION_MINOR(recvMsg->version),
            GET_VERSION_REVISION(recvMsg->version),
            GET_VERSION_PROTOCOL(recvMsg->version),
            GET_VERSION_MAJOR(VERSION_FULL),
            GET_VERSION_MINOR(VERSION_FULL),
            GET_VERSION_REVISION(VERSION_FULL),
            GET_VERSION_PROTOCOL(VERSION_FULL)
            );
        GateHandler::Instance()->SendLoginGameAck(ERR_VERSION_ERROR, centGateID, loginID);       // 版本号不匹配
        return;
    }
#ifdef WIN
#else
    if(!m_InitReady)
    {
        GateHandler::Instance()->SendLoginGameAck(ERR_SERVER_NO_READY, centGateID, loginID);       // 服务器还未准备好
        return;
    }
#endif

    switch(ConfigLoader::Instance()->GetLoginMode())
    {
        case LOGIN_MODE_TEST:           // 测试登陆模式
            VerifyTestModeLogin(centGateID, loginID, recvMsg->token, recvMsg->addr_ip, recvMsg->addr_port);
            break;
        case LOGIN_MODE_AF:             // 动艺网站登陆模式
            VerifyAfModeLogin(centGateID, loginID, recvMsg->token, recvMsg->addr_ip, recvMsg->addr_port);
            break;
        case LOGIN_MODE_KUNLUN:         // 昆仑网站登陆模式
            KlLoginExecutor::Instance()->Login(centGateID, loginID, recvMsg->token, recvMsg->addr_ip, recvMsg->addr_port);
            break;
        case LOGIN_MODE_KUNLUN_CDKEY:   // 昆仑网站激活码登陆模式
            KlLoginExecutor::Instance()->Login(centGateID, loginID, recvMsg->token, recvMsg->addr_ip, recvMsg->addr_port);
            break;
        case LOGIN_MODE_360:            // 360登陆模式
            Verify360ModeLogin(centGateID, loginID, recvMsg->token, recvMsg->addr_ip, recvMsg->addr_port);
            break;
        case LOGIN_MODE_9377:           // 9377登陆模式
        case LOGIN_MODE_TW:             // 台服登陆模式
            Verify9377ModeLogin(centGateID, loginID, recvMsg->token, recvMsg->addr_ip, recvMsg->addr_port);
            break;
        case LOGIN_MODE_KUGOU:          // kugou登陆模式(酷狗有个kgext参数，要做创角回调)
            VerifyKugouModeLogin(centGateID, loginID, recvMsg->token, recvMsg->addr_ip, recvMsg->addr_port);
            break;
        default:
            CnAssert(false);
            break;
    }
}

// ==================================================
// 请求创建角色
// ==================================================
void CenterServer::RecvCreatePlayerReq(uint64 loginID, uint32 sessionID, void* data)
{
    Player* player = NULL;
    ERROR_CODE sendMsg;

    CREATE_PLAYER_REQ* recvMsg = (CREATE_PLAYER_REQ*) data;
    if(!recvMsg) return;

    uint32 gateID = GateHandler::Instance()->GetRegionIDBySessionID(sessionID);

    player = PlayerManager::Instance()->GetPlayerByLoginID(loginID);
    if(!player || (player->GetState() != Player::kStateVerifyOk))
    {
        return;
    }

    sendMsg.errcode = NameLimit::Instance()->IsValidNameStr(recvMsg->player_name);
    if(sendMsg.errcode != ERR_SUCCEED)
    {   // 创建角色失败
        GateHandler::Instance()->SendCreatePlayerAck(sendMsg.errcode, gateID, loginID);
        return;
    }
    else
    {   // 创建角色成功

    }

    // 角色名称加前缀名
    recvMsg->player_name = recvMsg->player_name;

    PlayerJob::Instance()->CreatePlayer(gateID, loginID, player->GetAccount(), recvMsg->player_name,
        player->GetPlatformUID(), player->GetPlatformUName(), player->GetPlatform(), player->GetPlatformServerName(),
        player->GetRegion(), recvMsg->sex, recvMsg->career);
}

void CenterServer:: RecvIsCreatePlayerReq(uint64 loginID, uint32 sessionID, void* data)
{
    Player* player = 0;
    ERROR_CODE sendMsg;

    IS_CREATE_PLAYER_NAME_REQ* recvMsg = (IS_CREATE_PLAYER_NAME_REQ*)data;
    if(!recvMsg) return;

    uint32 regionID = GateHandler::Instance()->GetRegionIDBySessionID(sessionID);

    player = PlayerManager::Instance()->GetPlayerByLoginID(loginID);
    if(!player || (player->GetState() != Player::kStateVerifyOk))
    {
        return;
    }
    
    sendMsg.errcode = NameLimit::Instance()->IsValidNameStr(recvMsg->player_name);      // 名字合法性检查
    if(sendMsg.errcode != ERR_SUCCEED)
    {
        GateHandler::Instance()->SendIsCreatePlayerNameAck(sendMsg.errcode, regionID, loginID);
    }
    else
    {
        std::string curname = player->GetPlatformServerName() + "." + recvMsg->player_name;
        if(!PlayerJob::Instance()->FindPlayerNameIsExit(curname))           // 名字重复性检查
            sendMsg.errcode = ERR_SUCCEED;
        else
            sendMsg.errcode = ERR_FAILED;
        
        GateHandler::Instance()->SendIsCreatePlayerNameAck(sendMsg.errcode, regionID, loginID);
    }
};
// 查询角色信息
void CenterServer::RecvAskPlayerInfoReq(uint64 loginID, uint32 sessionID, void* data)
{
    uint32 regionID = GateHandler::Instance()->GetRegionIDBySessionID(sessionID);

    Player* player  = PlayerManager::Instance()->GetPlayerByLoginID(loginID);
    if(!player)
    {
        GateHandler::Instance()->SendLoginGameAck(ERR_PLAYER_NOT_EXIST_LOGIN, regionID, loginID);
        return;
    }

    CnAssert(player->GetState() == Player::kStateVerifyOk);   // 只有完成了登陆验证的角色，才会请求查询角色

    PlayerJob::Instance()->AskPlayerInfo(player->GetCentGateID(), loginID, player->GetAccount(), player->GetPlatformServerName());
}

// ==================================================
// 加载角色数据
// ==================================================
void CenterServer::RecvLoadPlayerReq(uint64 loginID, uint32 sessionID, void* data)
{
    uint32 regionID = GateHandler::Instance()->GetRegionIDBySessionID(sessionID);

    Player* player  = PlayerManager::Instance()->GetPlayerByLoginID(loginID);
    if(!player)
    {
        GateHandler::Instance()->SendLoginGameAck(ERR_PLAYER_NOT_EXIST_LOGIN, regionID, loginID);
        return;
    }

    CnAssert(player->GetState() == Player::kStateVerifyOk);   // 只有完成了登陆验证的角色，才会请求加载角色

    // 从数据库加载角色(非异步流程加载，创角时间更短)
    PlayerLoadJob::Instance()->LoadData(player->GetCentGateID(), loginID, player->GetAccount(), player->GetPlatformServerName(), player->GetLoginIP(), player->GetLoginPort());
}

void CenterServer::AddPlayerToCenter(uint32 gateSvrID, uint64 loginID, bool isload )
{
    Player* player = PlayerManager::Instance()->GetPlayerByLoginID(loginID);
    if(!player)
    {
        // 玩家被干掉了
        GateHandler::Instance()->SendLoginGameAck(ERR_PLAYER_NOT_EXIST_LOGIN, gateSvrID, loginID);
        return;
    }

    // 登陆时设置登陆时间
    player->SetLoginTime((uint32)Crown::SDNow().GetTimeValue());

    // 在第一次角色上线的时候，设置角色的地图ID
    player->SetCityMapID(player->GetUnionMapID());
    player->SetCityX(player->GetX());
    player->SetCityY(player->GetY());

    uint32 centerID = CalCenterIDByRegionID(player->GetCentGateID());
    CnAssert(player->GetChangeDungeonData().dungeon_id == 0);
    CnAssert(player->GetCentGameID() == 0);
    uint32 centGameID = GameHandler::Instance()->AskJumpCentGameServerID(player, DUNGEON_ID(player->GetCityMapID()));
    CnAssert(CalCenterIDByRegionID(centGameID) == ConfigLoader::Instance()->GetCenterID());       // 必为本服地图

    // 检查游戏服务器是否存在
    if(!GameHandler::Instance()->IsValidGame(centGameID))
    {
        CnInfo("player %s login gameserver not exist\n", player->GetName().c_str());
        GateHandler::Instance()->SendLoginGameAck(ERR_GAMESVR_CLOSE, centGameID, loginID);     // 角色所在的GameServer不存在
        PlayerManager::Instance()->DelPlayerByLoginID(loginID);
        return;
    }

    player->SetCentGameID(centGameID);

    if(isload)
    {
        //// 如果有公会则加载公会信息
        if(player->GetGuildID() && !GuildManager::Instance()->GetGuildInfo(player->GetGuildID()))
        {
            //这里也是异步加载
            GuildJob::Instance()->LoadGuildInfoForMgr(player->GetGuildID());
        }

        //加载天宫图信息
        //PlayerLoadJobAsync::Instance()->LoadHoroscope(player->GetPlayerID());
    }
}

// ==================================================
// 发送服务器接货确认
// ==================================================
void CenterServer::SendServerActiveAck(uint32 sessionID, uint16 errCode)
{
    SERVER_ACTIVE_ACK sendMsg;
    sendMsg.errcode = errCode;
    sendMsg.center_id = ConfigLoader::Instance()->GetCenterID();
    Send(sessionID, MSG_SERVER_ACTIVE_ACK, &sendMsg);
}

// ==================================================
// 发送服务器列表确认
// ==================================================
void CenterServer::SendServerListAck(uint32 sessionID, uint8 type)
{
    SERVER_LIST_ACK sendMsg;
    SERVER_INFO info;

    std::list<ServerConfig>* list = nullptr;
    uint8 reqType = 0;

    switch(type)
    {
    case SERVER_TYPE_GATE:
    case SERVER_TYPE_GAME:
        {
            reqType = SERVER_TYPE_GAME;
        }
        break;
    default:
        {
            CnAssert(false);
            return;
        }
        break;
    }

    list = ConfigLoader::Instance()->GetServerConfig(reqType);
    if(!list) return;

    for(std::list<ServerConfig>::iterator it = list->begin(); it != list->end(); ++it)
    {
        if(!it->m_activate) 
            continue;

        info.server_type = reqType;
        info.server_id = it->m_id;
        info.server_ip = it->m_ip;
        info.server_outer_ip = it->m_outer_ip;
        info.server_port = it->m_port;
        info.center_id = it->m_center_id;

        CnDbg("SendServerListAck id=%d %s(%s):%d\n", info.server_id, info.server_ip.c_str(), info.server_outer_ip.c_str(), info.server_port);

        sendMsg.list.push_back(info);
    }

    if(sendMsg.list.size() < 1)
    {
        return;
    }

    // 根据服务器类型，选择如何推送消息
    switch(type)
    {
    case SERVER_TYPE_GATE:
        {
            Send(sessionID, MSG_SERVER_LIST_ACK, &sendMsg);
        }
        break;
    case SERVER_TYPE_GAME:
        {
            GateHandler::Instance()->SendToAll(MSG_SERVER_LIST_ACK, &sendMsg);
        }
        break;
    default:
        break;
    }
}
