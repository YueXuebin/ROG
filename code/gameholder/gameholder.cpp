#include "gameholder_pch.h"
#include "gameholder.h"
#include "gameworld.h"
#include "config_manager.h"
#include "server_config.h"
#include "gate_handler.h"
#include "center_handler.h"
#include "af_transfer_client.h"
#include "af_transfer_server.h"
#include "player_manager.h"
#include "team_manager.h"
#include "team_relationship.h"
#include "answer_question_manager.h"
#include "dungeon_manager.h"
#include "skill_manager.h"
#include "buff_manager.h"
#include "battle_manager.h"
#include "regular_manager.h"
#include "gm_commander.h"
#include "ai_manager.h"
#include "offline_player_manager.h"
#include "answer_question_manager.h"
#include "city_fight_manager.h"
#include "city_survival_manager.h"
#include "guild_manager.h"
#include "world_boss_manager.h"
#include "activity_manager.h"
#include "ranking_manager.h"
#include "top_up_rank_manager.h"
#include "investment_manager.h"
#include "formula_manager.h"
#include "ashura_manager.h"
#include "faction_manager.h"
#include "aoi_system_manager.h"
#include "timing_trigger_manager.h"
#include "tower_defence_manager.h"
#include "void_field_manager.h"
#include "void_field_solo_manager.h"
#include "twilight_war_manager.h"
#include "capture_flag_manager.h"
#include "performance_analysis.h"
#include "global_setting_manager.h"
#include "multiserver_battle_manager.h"
#include "dungeon_map_manager.h"
#include "world_map_manager.h"
#include "game_setting_manager.h"
#include "proclamation_manager.h"
#include "vip_statistics_manager.h"
#include "task_data_factory.h"
#include "performance_analysis.h"
#include "id_pool_manager.h"
#include "id_manager.h"
#include "cross_team_manager.h"
#include "lua_engine.h"
#include "daily_fashion_manager.h"
#include "item_factory.h"
#include "random_package_manager.h"


GameHolder* GameHolder::m_instance = NULL;

// ==================================================
// 构造
// ==================================================
GameHolder::GameHolder()
{
    m_heartBeat = 0;
    m_instance = this;
    m_pGameWorld = nullptr;

    m_IsExit = false;

    sysLoadPercent = 0;
}

// ==================================================
// 析构
// ==================================================
GameHolder::~GameHolder()
{
    m_instance = NULL;
}

// ==================================================
// 初始化
// ==================================================
bool GameHolder::Init(uint32 server_id)
{
    Crown::SDSetSeed((uint32)time(nullptr));

    PerformanceManager::CreateInstance();
    PerformanceManager::Instance()->Init();

    LuaEngine::CreateInstance();
    if(!LuaEngine::Instance()->Init())
    {
        return false;
    }

    IdPoolManager::CreateInstance();
    IdPoolManager::Instance()->Init();

    ItemFactory::CreateInstance();

    GameSettingManager::CreateInstance();

    VoidFieldManager::CreateInstance();

    // 读取配置文件
    ConfigManager::CreateInstance();
    if(!ConfigManager::Instance()->Init(server_id))
    {
        return false;
    }
    
    ServerRandomPackageManager::CreateInstance();

    GmCommander::CreateInstance();

    PlayerManager::CreateInstance();

    TeamRelationShip::CreateInstance();

    TeamManager::CreateInstance();

    AnswerQuestionManager::CreateInstance();

    OfflinePlayerManager::CreateInstance();

    FactionManager::CreateInstance();

    CityFightManager::CreateInstance();

    CitySurvivalManager::CreateInstance();

    TwilightWarManager::CreateInstance();

    GuildManager::CreateInstance();

    WorldBossManager::CreateInstance();

    ActivityManager::CreateInstance();

    TimingTriggerManager::CreateInstance();

    RankingManager::CreateInstance();

    AshuraManager::CreateInstance();

    CaptureFlagManager::CreateInstance();

    TowerDefenceManager::CreateInstance();

    VoidFieldSoloManager::CreateInstance();

    WorldMapManager::CreateInstance();

    InvestmentManager::CreateInstance();        // 投资管理器
    
    GlobalSettingManager::CreateInstance();

    ProclamationManager::CreateInstance();      // 警示公告

    MultiserverBattleManager::CreateInstance();

    CrossTeamManager::CreateInstance();

    // CenterServer连接器
    CenterHandler::CreateInstance();

    // GateServer连接器
    GateHandler::CreateInstance();

    // 中心服务器连接列表
    if(!CenterHandler::Instance()->Init())
    {
        return false;
    }

    // 公式解析管理器
    FormulaManager::CreateInstance();
    if(!FormulaManager::Instance()->Init())
    {
        return false;
    }

    // 技能管理器（因为 Monster 和 Boss 在读取配置时，需要SkillManager的数据）
    SkillManager::CreateInstance();
    if(!SkillManager::Instance()->Init())
    {
        return false;
    }

    // Buff管理器
    BuffManager::CreateInstance();
    if(!BuffManager::Instance()->Init())
    {
        return false;
    }

    // AI管理器
    AIManager::CreateInstance();
    if(!AIManager::Instance()->Init())
    {
        return false;
    }

    // 战斗管理器
    BattleManager::CreateInstance();
    if(!BattleManager::Instance()->Init())
    {
        return false;
    }

    // 规则引擎管理器
    RegularManager::CreateInstance();
    if(!RegularManager::Instance()->Init())
    {
        return false;
    }
    
    // AOI系统管理器
    AOISystemManager::CreateInstance();
    if(!AOISystemManager::Instance()->Init())
    {
        return false;
    }

    // 初始化副本管理器
    DungeonManager::CreateInstance();
    if(!DungeonManager::Instance()->Init())
    {
        return false;
    }

    // 初始化大地图管理器
    DungeonMapManager::CreateInstance();
    if (!DungeonMapManager::Instance()->Init())
    {
        return false;
    }

    // 初始化玩家管理器
    PlayerManager::CreateInstance();

    // vip领域管理器
    VipStatisticsManager::CreateInstance();

    TopUpRankManager::CreateInstance();

    // 每日化身
    DailyFashionManager::CreateInstance();

    m_pGameWorld = CnNew GameWorld;
    m_pGameWorld->Init();

    return true;
}

// ==================================================
// 卸载
// ==================================================
void GameHolder::Uninit()
{
    // !!! 倒序
    if(m_pGameWorld)
    {
        m_pGameWorld->Uninit();
        CnDelete m_pGameWorld;
    }

    DailyFashionManager::DestroyInstance();

    TopUpRankManager::DestroyInstance();

    VipStatisticsManager::DestroyInstance();

    PlayerManager::DestroyInstance();       // 先释放玩家

    DungeonMapManager::DestroyInstance();

    DungeonManager::DestroyInstance();

    AOISystemManager::DestroyInstance();

    RegularManager::DestroyInstance();

    BattleManager::DestroyInstance();

    AIManager::DestroyInstance();

    BuffManager::DestroyInstance();

    SkillManager::DestroyInstance();

    FormulaManager::DestroyInstance();

    GateHandler::DestroyInstance();

    CenterHandler::DestroyInstance();

    CrossTeamManager::DestroyInstance();

    MultiserverBattleManager::DestroyInstance();

    ProclamationManager::DestroyInstance();

    GlobalSettingManager::DestroyInstance();

    InvestmentManager::DestroyInstance();

    WorldMapManager::DestroyInstance();

    VoidFieldSoloManager::DestroyInstance();

    TowerDefenceManager::DestroyInstance();

    CaptureFlagManager::DestroyInstance();

    AshuraManager::DestroyInstance();

    RankingManager::DestroyInstance();

    TimingTriggerManager::DestroyInstance();

    ActivityManager::DestroyInstance();

    WorldBossManager::DestroyInstance();

    GuildManager::DestroyInstance();

    TwilightWarManager::DestroyInstance();

    CitySurvivalManager::DestroyInstance();

    CityFightManager::DestroyInstance();

    FactionManager::DestroyInstance();

    OfflinePlayerManager::DestroyInstance();

    AnswerQuestionManager::DestroyInstance();

    TeamManager::DestroyInstance();

    TeamRelationShip::DestroyInstance();

    GmCommander::DestroyInstance();

    ServerRandomPackageManager::DestroyInstance();

    ConfigManager::DestroyInstance();

    VoidFieldManager::DestroyInstance();

    GameSettingManager::DestroyInstance();

    ItemFactory::DestroyInstance();

    IdPoolManager::DestroyInstance();

    LuaEngine::DestroyInstance();

    PerformanceManager::DestroyInstance();

    // !!! 倒序
}
void GameHolder::RequestExit()
{
    CnDbg("game holder exit\n");

    // 安全断开所有CenterServer
    CenterHandler::Instance()->SafeDeleteAllCenterServer();
    
    m_IsExit = true;
}

bool GameHolder::CanExit()
{
    // 所有玩家下线才可以安全退出
    uint32 player_num = PlayerManager::Instance()->GetPlayerNum();
    bool ret = m_IsExit && (player_num == 0);
    if(ret)
    {
        CnDbg("can exit\n");
    }
    return ret;
}

// ==================================================
// 更新
// ==================================================
void GameHolder::Update(uint32 frame_count)
{
    PERFORM_BEGIN();

    UpdateNet();

    CenterHandler::Instance()->Update();

    // 处理游戏逻辑
    if(m_pGameWorld)
        m_pGameWorld->Update(frame_count);

    // 逻辑帧同步
    PlayerManager::Instance()->SyncFrame(frame_count);


    ServerRandomPackageManager::Instance()->Update();

    ++m_heartBeat;

#ifdef WIN
    const uint32 timer = g_ServerUpdateFrequency * 10;
#else
    const uint32 timer = g_ServerUpdateFrequency * 60;
#endif
    if(m_heartBeat % timer == 0)
    {
        uint32 playerNum = PlayerManager::Instance()->GetPlayerNum();
        CnDbg("load=%d playernum=%d monster=%d\n", sysLoadPercent, playerNum, IdManager::Instance()->GetMonsterCount());
    }

#ifdef DEBUG
    // 错误检测
    uint32 checkPlayerNum = PlayerManager::Instance()->GetPlayerNumInGame();
    uint32 checkPlayerNum2 = DungeonManager::Instance()->GetPlayerNum();
    CnAssert(checkPlayerNum == checkPlayerNum2);
#endif

    PERFORM_END();

#ifdef PERFORMANCE_ANALYSIS
    // 性能分析
    PerformanceManager::Instance()->Update();
#endif
}

// ==================================================
// 更新网络
// ==================================================
void GameHolder::UpdateNet()
{
    TransferUnpdate();
}

void GameHolder::OnCrashExit()
{
    // 保存玩家数据
}
