#include "gameholder_pch.h"
#include "game_define.h"
#include "player.h"
#include "boss.h"
#include "monster.h"
#include "scene.h"
#include "dungeon.h"
#include "regular.h"
#include "gate_handler.h"
#include "center_handler.h"
#include "team_manager.h"
#include "dungeon_manager.h"
#include "dungeon_set.h"
#include "dungeon_gain_config.h"
#include "action_player_controller.h"
#include "state_define.h"
#include "sale_config.h"
#include "trigger.h"
#include "gm_commander.h"
#include "event_define.h"
#include "pet_controller.h"
#include "prop_notifier.h"
#include "player_prop_notifier.h"
#include "battle_manager.h"
#include "relife_helper.h"
#include "state_controller.h"
#include "answer_question_manager.h"
#include "answer_question_config.h"
#include "player_dungeon_manager.h"
#include "soul_dungeon_manager.h"
#include "guild_manager.h"
#include "item_manager.h"
#include "equip_manager.h"
#include "task_manager.h"
#include "guide_manager.h"
#include "open_system_manager.h"
#include "horoscope_manager.h"
#include "skill_info_manager.h"
#include "friend_manager.h"
#include "chat_manager.h"
#include "battle_recorder.h"
#include "ashura_recorder.h"
#include "babel_manager.h"
#include "mail_manager.h"
#include "sign_manager.h"
#include "online_reward_manager.h"
#include "player_door_manager.h"
#include "ride_manager.h"
#include "activity_manager.h"
#include "game_util.h"
#include "treasury_manager.h"
#include "adventure_manager.h"
#include "offline_player_manager.h"
#include "wing_manager.h"
#include "on_recv_filter.h"
#include "vip_card_config.h"
#include "vip_reward_config.h"
#include "vip_config.h"
#include "title_manager.h"
#include "recharge_manager.h"
#include "shop_manager.h"
#include "shop_service_manager.h"
#include "ranking_manager.h"
#include "goldtree_manager.h"
#include "skill_controller.h"
#include "expend_manager.h"
#include "passive_skill_manager.h"
#include "trade_manager.h"
#include "day_investment_manager.h"
#include "level_investment_manager.h"
#include "level_activity_manager.h"
#include "special_activity_manager_gs.h"
#include "investment_manager.h"
#include "setting_manager.h"
#include "team_dungeon_config.h"
#include "mail_title_config.h"
#include "ashura_manager.h"
#include "faction_manager.h"
#include "liveness_manager.h"
#include "dungeon_times_manager.h"
#include "server_config.h"
#include "faction_manager.h"
#include "city_fight_manager.h"
#include "formula_manager.h"
#include "soul_dungeon_manager.h"
#include "gameplaytimes_notifier.h"
#include "property_set_calculater.h"
#include "city_survival_manager.h"
#include "stone_turn_gold_manager.h"
#include "touch_controller.h"
#include "guild_config.h"
#include "buff_controller.h"
#include "namelimit.h"
#include "limitbuy_manager.h"
#include "guild_manager.h"
#include "single_answer_manager.h"
#include "vip_gift_config.h"
#include "vigor_manager.h"
#include "vigor_config.h"
#include "fashion_manager.h"
#include "sword_manager.h"
#include "zombie_manager.h"
#include "bh_manager.h"
#include "enemy_manager.h"
#include "void_field_manager.h"
#include "void_field_solo_manager.h"
#include "md5.h"
#include "giftcode_config.h"
#include "guild_science_recorder.h"
#include "void_field_setting_config.h"
#include "package_manager.h"
#include "dungeon_map_manager.h"
#include "multiserver_battle_info_manager.h"
#include "world_map_manager.h"
#include "vip_manager.h"
#include "fcm_manager.h"
#include "shorcut_manager.h"
#include "welcome_reward_config.h"
#include "void_field_config.h"
#include "proclamation_manager.h"
#include "pay_point_lottery.h"
#include "vip_only_dungeon_manager.h"
#include "top_up_rank_manager.h"
#include "investment_plan_manager.h"
#include "offline_vigor_manager.h"
#include "donate_manager.h"
#include "game_setting_manager.h"
#include "answer_manager.h"
#include "battle_state_manager.h"
#include "player_saver.h"
#include "player_loader.h"
#include "config_manager.h"
#include "ai_manager.h"
#include "player_team_manager.h"
#include "cross_team_manager.h"
#include "resurgence_helper.h"
#include "egg_manager.h"
#include "contrast_manager.h"
#include "buy_cheap_manager.h"
#include "daily_fashion_manager.h"
#include "system_config_manager.h"
#include "player_config_manager.h"
#include "pet_manager.h"
#include "head_portrait_manager.h"
#include "bag_manager.h"
#include "store_manager.h"
#include "equip_strengthen_manager.h"
#include "gem_manager.h"
#include "skill_strengthen_manager.h"
#include "alchemy_manager.h"
#include "potion_manager.h"
#include "decompose_manager.h"
#include "random_package_manager.h"
#include "item_controller.h"

CnImplementRTTI(Player, Creature);

Player::Player() :
Creature(OBJ_PLAYER)
{
    m_PlayerLoader          = CnNew PlayerLoader(this);
    m_PlayerSaver           = CnNew PlayerSaver(this);    

    m_PlayerPropNotifier    = CnNew PlayerPropNotifier(this);

    m_recv_filter           = CnNew OnRecvFilter(this);
    m_player_dungeon_mgr    = CnNew PlayerDungeonManager(this);
    m_skill_info_mgr        = CnNew SkillInfoManager(this, PLACE_ROLE_SKILL, SKILL_SLOT_WHOLE);
    m_item_mgr              = CnNew ItemManager(this);
    m_package_mgr           = CnNew PackageManager(this);
    m_equip_mgr             = CnNew EquipManager(this, PLACE_ROLE_EQUIP);
    m_friend_mgr            = CnNew FriendManager(this);
    m_chat_mgr              = CnNew ChatManager(this);
    m_task_mgr              = CnNew TaskManager(this);
    m_guide_mgr             = CnNew GuideManager(this);
    m_opensys_mgr           = CnNew OpenSystemManager(this);
    m_player_door_mgr       = CnNew PlayerDoorManager(this);
    m_horoscope_mgr         = CnNew HoroscopeManager(this);
    m_battle_recorder       = CnNew BattleRecorder(this);
    m_ashura_recorder       = CnNew AshuraRecorder(this);
    m_mail_mgr              = CnNew MailManager(this);
    m_sign_mgr              = CnNew SignManager(this);
    m_online_reward_mgr     = CnNew OnlineRewardManager(this);
    m_ride_mgr              = CnNew RideManager(this);
    m_treasury_mgr          = CnNew TreasuryManager(this);
    m_goldtree_mgr          = CnNew GoldtreeManager(this);
    m_adventure_mgr         = CnNew AdventureManager(this);
    m_wing_mgr              = CnNew WingManager(this);
    m_title_mgr             = CnNew TitleManager(this);
    m_recharge_mgr          = CnNew RechargeManager(this);
    m_shop_mgr              = CnNew ShopManager(this);
    m_shop_service_mgr      = CnNew ShopServiceManager(this);
    m_expend_mgr            = CnNew ExpendManager(this);
    m_day_investment_mgr    = CnNew DayInvestmentManager(this);
    m_level_investment_mgr  = CnNew LevelInvestmentManager(this);
    m_level_activity_mgr    = CnNew LevelActivityManager(this);
    m_special_activity_mgr  = CnNew SpecialActivityManager(this);
    m_passive_skill_mgr     = CnNew PassiveSkillManager(this);
    m_setting_mgr           = CnNew SettingManager(this);
    m_liveness_mgr          = CnNew LivenessManager(this);
    m_dungeon_times_mgr     = CnNew DungeonTimesRecorder(this);
    m_notify_mgr            = CnNew GameplayTimesNotifier(this);
    m_contrast_manager      = CnNew ContrastManager(this);
    m_stone_turn_gold_mgr   = CnNew StoneTurnGoldManager(this);
    m_limitbuy_mgr          = CnNew LimitBuy_Manager(this);
    m_single_answer_mgr     = CnNew SingleAnswerManager(this);
    m_vigor_mgr             = CnNew VigorManager(this);
    m_offline_vigor_mgr     = CnNew OfflineVigorManager(this);
    m_sword_mgr             = CnNew SwordManager(this);
    m_zombie_mgr            = CnNew ZombieManager(this);
    m_fashion_mgr           = CnNew FashionManager(this);
    m_enemy_mgr             = CnNew EnemyManager(this);
    m_bh_mgr                = CnNew BhManager(this);
    m_guild_science_mgr     = CnNew GuildScienceRecorder(this);
    m_multiserver_battle_info    = CnNew MultiserverBattleInfoManager(this);
    m_vip_manager           = CnNew VIPManager(this);
    m_fcm_manager           = CnNew FCMManager(this);
    m_shorcut_mgr           = CnNew ShorcutManager(this);
    m_pay_point_lottery     = CnNew PayPointLotteryManager(this);
    m_vip_only_dungeon      = CnNew VIPOnlyDungeonManager(this);
    m_investment_plan_mgr   = CnNew InvestmentPlanManager(this);
    m_donate_mgr            = CnNew DonateManager(this);
    m_answer_mgr            = CnNew AnswerManager(this);
    m_battle_state_manager  = CnNew BattleStateManager(this);
    m_team_manager          = CnNew PlayerTeamManager(this);
    m_egg_manager           = CnNew EggManager(this);
    m_buy_cheap_mgr         = CnNew BuyCheapMgr(this);
    m_pet_mgr               = CnNew PetManager(this);
    m_bag_mgr               = CnNew BagManager(this);
    m_store_mgr             = CnNew StoreManager(this);
    m_headPortrait_mgr      = CnNew HeadPortraitManager(this);
    m_equip_quality_up      = CnNew EquipStrengthenManager(this);
    m_gem_mgr               = CnNew GemManager(this);
    m_skill_strengthen      = CnNew SkillStrengthenManager(this);
    m_alchemy_mgr           = CnNew AlchemyManager(this);
    m_potion_use_mgr        = CnNew PotionManager(this);
    m_decompose_mgr         = CnNew DecomposeManager(this);
    m_random_package_mgr    = CnNew RandomPackageManager(this);

    m_item_controller       = CnNew ItemController(this);
    m_is_buying = false;

    m_auto_save_time = 0;

    m_game_start = false;
    m_game_lock = false;

    m_relive = false;

    m_online_reward_time = 0;
    m_online_reward_flag = 0;

    m_Ping = 0;

    ClearGainPool();
}

Player::~Player()
{
    AIManager::Instance()->AddDebugListener(this, NULL);

    // 析构要写成构造的反向顺序 !!!
    CnDelete m_item_controller;
    m_item_controller = NULL;

    CnDelete m_random_package_mgr;
    m_random_package_mgr = NULL;

    CnDelete m_decompose_mgr;
    m_decompose_mgr = NULL;

    CnDelete m_alchemy_mgr;
    m_alchemy_mgr = NULL;

    CnDelete m_potion_use_mgr;
    m_potion_use_mgr = NULL;
	
    CnDelete m_skill_strengthen;
    m_skill_strengthen = NULL;

    CnDelete m_gem_mgr;
    m_gem_mgr = NULL;

    CnDelete m_equip_quality_up;
    m_equip_quality_up = NULL;

    CnDelete m_headPortrait_mgr;
    m_headPortrait_mgr = NULL;

    CnDelete m_pet_mgr;
    m_pet_mgr = NULL;

    CnDelete m_bag_mgr;
    m_bag_mgr = NULL;

    CnDelete m_store_mgr;
    m_store_mgr = NULL;

    CnDelete m_buy_cheap_mgr;
    m_buy_cheap_mgr = NULL;

    CnDelete m_egg_manager;
    m_egg_manager = NULL;

    CnDelete m_team_manager;
    m_team_manager = NULL;

    CnDelete m_battle_state_manager;
    m_battle_state_manager = NULL;

    CnDelete m_answer_mgr;
    m_answer_mgr = NULL;

    CnDelete m_donate_mgr;
    m_donate_mgr = NULL;

    CnDelete m_investment_plan_mgr;
    m_investment_plan_mgr = NULL;

    CnDelete m_vip_only_dungeon;
    m_vip_only_dungeon = NULL;

    CnDelete m_pay_point_lottery;
    m_pay_point_lottery = NULL;

    CnDelete m_shorcut_mgr;
    m_shorcut_mgr = NULL;

    CnDelete m_fcm_manager;
    m_fcm_manager = NULL;

    CnDelete m_vip_manager;
    m_vip_manager = NULL;

    CnDelete m_multiserver_battle_info;
    m_multiserver_battle_info = NULL;

    CnDelete m_guild_science_mgr;
    m_guild_science_mgr = NULL;

    CnDelete m_bh_mgr;
    m_bh_mgr = NULL;

    CnDelete m_enemy_mgr;
    m_enemy_mgr = NULL;

    CnDelete m_fashion_mgr;
    m_fashion_mgr = NULL;

    CnDelete m_zombie_mgr;
    m_zombie_mgr = NULL;

    CnDelete m_sword_mgr;
    m_sword_mgr = NULL;

    CnDelete m_vigor_mgr;
    m_vigor_mgr = NULL;

    CnDelete m_offline_vigor_mgr;
    m_offline_vigor_mgr = NULL;

    CnDelete m_single_answer_mgr;
    m_single_answer_mgr = NULL;

    CnDelete m_limitbuy_mgr;
    m_limitbuy_mgr = NULL;

    CnDelete m_stone_turn_gold_mgr;
    m_stone_turn_gold_mgr = NULL;

    CnDelete m_contrast_manager;
    m_contrast_manager = NULL;

    CnDelete m_notify_mgr;
    m_notify_mgr = NULL;

    CnDelete m_dungeon_times_mgr;
    m_dungeon_times_mgr = NULL;

    CnDelete m_liveness_mgr;
    m_liveness_mgr = NULL;

    CnDelete m_setting_mgr;
    m_setting_mgr = NULL;

    CnDelete m_passive_skill_mgr;
    m_passive_skill_mgr = NULL;

    CnDelete m_special_activity_mgr;
    m_special_activity_mgr = NULL;

    CnDelete m_level_activity_mgr;
    m_level_activity_mgr = NULL;

    CnDelete m_level_investment_mgr;
    m_level_investment_mgr = NULL;

    CnDelete m_day_investment_mgr;
    m_day_investment_mgr = NULL;

    CnDelete m_expend_mgr;
    m_expend_mgr = NULL;

    CnDelete m_shop_service_mgr;
    m_shop_service_mgr = NULL;

    CnDelete m_shop_mgr;
    m_shop_mgr = NULL;

    CnDelete m_recharge_mgr;
    m_recharge_mgr = NULL;

    CnDelete m_title_mgr;
    m_title_mgr = NULL;

    CnDelete m_wing_mgr;
    m_wing_mgr = NULL;

    CnDelete m_adventure_mgr;
    m_adventure_mgr = NULL;

    CnDelete m_goldtree_mgr;
    m_goldtree_mgr = NULL;

    CnDelete m_treasury_mgr;
    m_treasury_mgr = NULL;

    CnDelete m_ride_mgr;
    m_ride_mgr = NULL;

    CnDelete m_online_reward_mgr;
    m_online_reward_mgr = NULL;

    CnDelete m_sign_mgr;
    m_sign_mgr = NULL;

    CnDelete m_mail_mgr;
    m_mail_mgr = NULL;

    CnDelete m_ashura_recorder;
    m_ashura_recorder = NULL;

    CnDelete m_battle_recorder;
    m_battle_recorder = NULL;

    CnDelete m_horoscope_mgr;
    m_horoscope_mgr = NULL;

    CnDelete m_player_door_mgr;
    m_player_door_mgr = NULL;

    CnDelete m_task_mgr;
    m_task_mgr = NULL;

    CnDelete m_guide_mgr;
    m_guide_mgr = NULL;

    CnDelete m_opensys_mgr;
    m_opensys_mgr = NULL;

    CnDelete m_chat_mgr;
    m_chat_mgr = NULL;

    CnDelete m_friend_mgr;
    m_friend_mgr = NULL;

    CnDelete m_equip_mgr;
    m_equip_mgr = NULL;

    CnDelete m_package_mgr;
    m_package_mgr = NULL;

    CnDelete m_item_mgr;
    m_item_mgr = NULL;

    CnDelete m_skill_info_mgr;
    m_skill_info_mgr = NULL;

    CnDelete m_player_dungeon_mgr;
    m_player_dungeon_mgr = NULL;

    CnDelete m_PlayerPropNotifier;
    m_PlayerPropNotifier = NULL;

    CnDelete m_recv_filter;
    m_recv_filter = NULL;

    CnDelete m_PlayerSaver;
    m_PlayerSaver = NULL;

    CnDelete m_PlayerLoader;
    m_PlayerLoader = NULL;

    // 析构要写成构造的反向顺序 !!!
}

void Player::SetPropDirty()
{
    m_PlayerPropNotifier->SetDirty();
}

void Player::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2 , int32 arg3)
{
    // 活跃度事件
    if ((eventId > LIVENESS_EVENT_BEGIN) && (eventId < LIVENESS_EVENT_END))
    {
        m_liveness_mgr->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);
        return;
    }

    Creature::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    m_battle_state_manager->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    m_battle_recorder->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    m_title_mgr->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    m_treasury_mgr->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    m_ride_mgr->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    m_skill_info_mgr->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    m_passive_skill_mgr->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    m_player_door_mgr->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    m_contrast_manager->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    m_task_mgr->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    m_goldtree_mgr->ReceiveEvent(frameCount, eventId, arg1, arg2);

    m_vip_only_dungeon->ReceiveEvent(frameCount, eventId, arg1, arg2);

    m_stone_turn_gold_mgr->ReceiveEvent(frameCount, eventId, arg1, arg2);

    m_wing_mgr->ReceiveEvent(frameCount, eventId, arg1, arg2 ,arg3);

    m_single_answer_mgr->ReceiveEvent(frameCount, eventId, arg1, arg2 ,arg3);

    m_zombie_mgr->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    m_multiserver_battle_info->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    m_egg_manager->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);
}

void Player::OnRecv(uint32 msgID, void* data)
{
    if(!data) return;

    if(kStateInGame != GetNetState())
    {
        CnDbg("msgID=%d not in kStateInGame\n", msgID);
        return;
    }

    // 协议过滤
    if(!m_recv_filter->OnRecv(msgID, data))
        return;

    if((MSG_PLAYER_BASE_BEGIN <= msgID) && (msgID <= MSG_PLAYER_BASE_END))      // 1100
    {
        OnRecvBaseProp(msgID, data);
    }
    else if((MSG_CHANGE_MAP_BEGIN <= msgID) && (msgID <= MSG_CHANGE_MAP_END))   // 1200
    {
        m_player_dungeon_mgr->OnRecv(msgID, data);
    }
    else if((MSG_SKILL_BEGIN <= msgID) && (msgID <= MSG_SKILL_END))             // 1300
    {
        OnRecvSkill(msgID, data);
    }
    else if((MSG_BATTLE_BEGIN <= msgID) && (msgID <= MSG_BATTLE_END))           // 1400
    {
        OnRecvBattle(msgID, data);
    }
    else if((MSG_ITEM_BEGIN <= msgID) && (msgID <= MSG_ITEM_END))               // 1500
    {
        m_item_mgr->OnRecv(msgID, data);
        m_bag_mgr->OnRecv(msgID, data);
        m_gem_mgr->OnRecv(msgID, data);
        m_potion_use_mgr->OnRecv(msgID, data);
    }
    else if((MSG_EQUIP_BEGIN <= msgID) && (msgID <= MSG_EQUIP_END))             // 1600
    {
        m_equip_mgr->OnRecv(msgID, data);
        m_equip_quality_up->OnRecv(msgID, data);
    }
    else if((MSG_TASK_BEGIN <= msgID) && (msgID <= MSG_TASK_END))               // 1700
    {
        m_task_mgr->OnRecv(msgID, data);
    }
    else if((MSG_TRIGGER_BEGIN <= msgID) && (msgID <= MSG_TRIGGER_END))         // 1800
    {
        OnRecvTrigger(msgID, data);
    }
    else if((MSG_RANK_BEGIN <= msgID) && (msgID <= MSG_RANK_END))               // 2000
    {
        m_player_door_mgr->OnRecv(msgID, data);
        OnRecvOfflineInfoReq(msgID, data);
    }
    else if((MSG_VIP_BEGIN <= msgID) && (msgID <= MSG_VIP_END))                 // 2100
    {
        m_vip_manager->OnRecv(msgID, data);
    }
    else if((MSG_HOROSCOPE_BEGIN <= msgID) && (msgID <= MSG_HOROSCOPE_END))     // 2200
    {
        m_horoscope_mgr->OnRecv(msgID, data);
    }
    else if((MSG_FRIEND_BEGIN <= msgID) && (msgID <= MSG_FRIEND_END))           // 2300
    {
        m_friend_mgr->OnRecv(msgID, data);
    }
    else if((MSG_CHAT_BEGIN <= msgID) && (msgID <= MSG_CHAT_END))               // 2400
    {
        m_chat_mgr->OnRecv(msgID, data);
    }
    else if((MSG_SALE_BEGIN <= msgID) && (msgID <= MSG_SALE_END))               // 2500
    {
        // 未使用
    }
    else if((MSG_ASHURA_BEGIN <= msgID) && (msgID <= MSG_ASHURA_END))           // 2600
    {
        m_ashura_recorder->OnRecv(msgID, data);
    }
    else if ((MSG_TEAM_BEGIN <= msgID) && (msgID <= MSG_TEAM_END))          // 2800
    {
        TeamManager::Instance()->OnRecvFromPlayer(this, msgID, data);
    }
    else if ((MSG_MAIL_BEGIN <= msgID) && (msgID <= MSG_MAIL_END))          // 2900
    {
        m_mail_mgr->OnRecv(msgID, data);
    }
    else if ((MSG_ANSWER_QUESTION_BEGIN <= msgID) && (msgID <= MSG_ANSWER_QUESTION_END))    // 3000
    {
        AnswerQuestionManager::Instance()->OnRecv(this, msgID, data);
    }
    else if((MSG_GUILD_BEGIN <= msgID) && (msgID <= MSG_GUILD_END))         // 3100
    {
        OnRecvGuild(msgID, data);
    }
    else if ((MSG_SIGN_BEGIN <= msgID) && (msgID <= MSG_SIGN_END))          // 3200
    {
        m_sign_mgr->OnRecv(msgID, data);
    }
    else if((MSG_RACING_BEGIN <= msgID) && (msgID <= MSG_RACING_END))       // 3400
    {
        //m_racing_mgr->OnRecv(msgID, data);
    }
    else if((MSG_MINING_BEGIN <= msgID) && (msgID <= MSG_MINING_END))       // 3500
    {
        //m_mining_mgr->OnRecv(msgID, data);
    }
    else if((MSG_ACTIVITY_BEGIN <= msgID) && (msgID <= MSG_ACTIVITY_END))   // 3600
    {
        ActivityManager::Instance()->OnPlayerRecv(this, msgID, data);
    }
    else if((MSG_TREASURY_BEGIN <= msgID) && (msgID <= MSG_TREASURY_END))   // 3700
    {
        m_treasury_mgr->OnRecv(msgID, data);
    }
    else if((MSG_ADVENTURE_BEGIN <= msgID) && (msgID <= MSG_ADVENTURE_END)) // 3800
    {
        m_adventure_mgr->OnRecv(msgID, data);
    }
    else if((MSG_WING_BEGIN <= msgID) && (msgID <= MSG_WING_END))           // 3900
    {
        m_wing_mgr->OnRecv(msgID,data);
    }
    else if((MSG_ONLINE_REWARD_BEGIN <= msgID) && (msgID <= MSG_ONLINE_REWARD_END))     // 4000
    {
        m_online_reward_mgr->OnRecv(msgID,data);
    }
    else if((MSG_MALL_BEGIN <= msgID) && (msgID <= MSG_MALL_END))           // 4200
    {
        m_shop_mgr->OnRecv(msgID, data);
        m_limitbuy_mgr->OnRecv(msgID, data);
    }
    else if((MSG_RANKING_LIST_BEGIN <= msgID) && (msgID <= MSG_RANKING_LIST_END))       // 4300
    {
        RankingManager::Instance()->OnRankingRecv(GetPlayerID(), msgID, data);
    }
    else if((MSG_TITLE_BEGIN <= msgID) && (msgID <= MSG_TITLE_END))         // 4400
    {
        m_title_mgr->OnRecv(msgID,data);
    }
    else if((MSG_FCM_BEGIN <= msgID) && (msgID <= MSG_FCM_END))             // 4500
    {
        OnRecvFcm(msgID, data);
    }
    else if((MSG_ACTIVATION_CODE_BEGIN <= msgID) && (msgID <= MSG_ACTIVATION_CODE_END))     // 4600
    {
        OnRecvActivationCode(msgID, data);
    }
    else if((MSG_RIDE_PET_BEGIN <= msgID) && (msgID <= MSG_RIDE_PET_END))           // 5000
    {
        m_ride_mgr->OnRecv(msgID, data);
    }
    else if((MSG_PET_BEGIN <= msgID) && (msgID <= MSG_PET_END))       // 5050
    {
        m_pet_mgr->OnRecv(msgID, data);
    }
    else if((MSG_GOLDTREE_BEGIN <= msgID) && (msgID <= MSG_GOLDTREE_END))       // 5100
    {
        m_goldtree_mgr->OnRecv(msgID, data);
    }
    else if((MSG_PLAYER_RESURGENCE_BEGIN <= msgID) && (msgID <= MSG_PLAYER_RESURGENCE_END))     // 5200
    {
        OnRecvResurgenceReq(msgID, data);
    }
    else if((MSG_DAY_INVESTMENT_BEGIN <= msgID) && (msgID <= MSG_DAY_INVESTMENT_END))       // 5300
    {
        m_day_investment_mgr->OnRecv(msgID, data);
    }
    else if((MSG_PASSIVE_SKILL_BEGIN <= msgID) && (msgID <= MSG_PASSIVE_SKILL_END))     // 5400
    {
        m_passive_skill_mgr->OnRecv(msgID, data);
    }
    else if ((MSG_SETTING_BEGIN <= msgID) && (msgID <= MSG_SETTING_END))                // 5500
    {
        m_setting_mgr->OnRecv(msgID, data);
    }
    else if((MSG_LEVEL_INVESTMENT_BEGIN <= msgID) && (msgID <= MSG_LEVEL_INVESTMENT_END))       // 5600
    {
        m_level_investment_mgr->OnRecv(msgID, data);
    }
    else if((MSG_INVESTMENT_MANAGER_BEGIN <= msgID) && (msgID <= MSG_INVESTMENT_MANAGER_END))   // 5700
    {
        InvestmentManager::Instance()->OnRecv(this, msgID, data);
    }
    else if((MSG_LEVEL_ACTIVITY_MANAGER_BEGIN <= msgID) && (msgID <= MSG_LEVEL_ACTIVITY_MANAGER_END))       // 5800
    {
        m_level_activity_mgr->OnRecv(msgID, data);
    }
    else if((MSG_SPECIAL_ACTIVITY_MANAGER_BEGIN <= msgID) && (msgID <= MSG_SPECIAL_ACTIVITY_MANAGER_END))   // 5900
    {
        m_special_activity_mgr->OnRecv(msgID, data);
    }
    else if((MSG_FACTION_BEGIN <= msgID) && (msgID <= MSG_FACTION_END)) // 6100
    {
        FactionManager::Instance()->OnRecv(this, msgID, data);
    }
    else if((MSG_LIVENESS_BEGIN <= msgID) && (msgID <= MSG_LIVENESS_END))   // 6200
    {
        m_liveness_mgr->OnRecv(msgID, data);
    }
    else if ((MSG_CITY_FIGHT_BEGIN <= msgID) && (msgID <= MSG_CITY_FIGHT_END))  // 6500
    {
        CityFightManager::Instance()->OnRecv(msgID, data, this);
    }
    else if((MSG_GAME_NOTIFY_BEGIN <= msgID) && (msgID <= MSG_GAME_NOTIFY_END)) // 6600
    {
        m_notify_mgr->OnRecv(msgID, data);
    }
    else if((MSG_CONTRAST_BEGIN <= msgID) && (msgID <= MSG_CONTRAST_END))       // 6700
    {
        m_contrast_manager->OnRecv(msgID, data);
    }
    else if ((MSG_CITY_SURVIVAL_BEGIN <= msgID) && (msgID <= MSG_CITY_SURVIVAL_END))    // 6800
    {
        CitySurvivalManager::Instance()->OnRecv(msgID, data, this);
    }
    else if ((MSG_TOUCH_BEGIN <= msgID) && (msgID <= MSG_TOUCH_END))            // 6900
    {
        OnTouchRecv(msgID, data);
    }
    else if ((MSG_STONE_TURN_GOLD_BEGIN <= msgID) && (msgID <= MSG_STONE_TURN_GOLD_END))    // 7000
    {
        m_stone_turn_gold_mgr->OnRecv(msgID, data);
    }
    else if((MSG_SINGLE_ANSWER_BEGIN <= msgID) && (msgID <= MSG_SINGLE_ANSWER_END))         // 7100
    {
        m_single_answer_mgr->OnRecv(msgID, data);
    }
    else if((MSG_GAIN_BACK_RESOURCE_BEGIN <= msgID) && (msgID <= MSG_GAIN_BACK_RESOURCE_END))   // 7400
    {

    }
    else if ((MSG_VIGOR_BEGIN <= msgID) && (msgID <= MSG_VIGOR_END))        // 7500
    {
        m_vigor_mgr->OnRecv(msgID, data);
        m_offline_vigor_mgr->OnRecv(msgID, data);
    }
    else if((MSG_FASHION_BEGIN <= msgID) && (msgID <= MSG_FASHION_END))     // 7600
    {
        m_fashion_mgr->OnRecv(msgID, data);
    }
    else if((MSG_CROSS_TEAM_BEGIN <= msgID) && (msgID <= MSG_CROSS_TEAM_END))   // 7700
    {
        CrossTeamManager::Instance()->OnRecvFromPlayer(this, msgID, data);
    }
    else if((MSG_JUMPING_OFF_BEGIN <= msgID) && (msgID <= MSG_JUMPING_OFF_END))     // 8000
    {
        
    }
    else if((MSG_ENEMY_BEGIN <= msgID) && (msgID <= MSG_ENEMY_END))         // 8100
    {
        m_enemy_mgr->OnRecv(msgID, data);
    }
    else if((MSG_VOID_FIELD_BEGIN <= msgID) && (msgID <= MSG_VOID_FIELD_SOLO_END))  // 8200
    {
        OnVoidFieldRecv(msgID, data);
    }
    else if((MSG_BH_BEGIN <= msgID) && (msgID <= MSG_BH_END))               // 8300
    {
        m_bh_mgr->OnRecv(msgID, data);
    }
    else if((MSG_SWORD_BEGIN <= msgID) && (msgID <= MSG_SWORD_END))         // 8400
    {
        m_sword_mgr->OnRecv(msgID, data);
    }
    else if ((MSG_ZOMBIE_BEGIN <= msgID) && (msgID <= MSG_ZOMBIE_END))      // 8500
    {
        m_zombie_mgr->OnRecv(msgID, data);
    }
    else if((MSG_PROCLAMATION_BEGIN <= msgID) && (msgID <= MSG_PROCLAMATION_END))   // 8900
    {
        if(msgID == MSG_PLAYER_PROCLAMATION_REQ)
        {
            ProclamationManager::Instance()->OnPlayerProclamationReq(this, data);
        }
    }
    else if((MSG_SHORCUT_BEGIN <= msgID) && (msgID <= MSG_SHORCUT_END))         // 9000
    {
        m_shorcut_mgr->OnRecv(msgID, data);
    }
    else if ((MSG_MULTISERVER_BATTLE_BEGIN <= msgID) && (msgID <= MSG_MULTISERVER_BATTLE_END))      // 9200
    {
        m_multiserver_battle_info->OnRecv(msgID, data);
    }
    else if ((MSG_FIELD_BOSS_BEGIN <= msgID) && (msgID <= MSG_FIELD_BOSS_END))      // 9300
    {
        WorldMapManager::Instance()->OnPlayerRecv(this, msgID, data);
    }
    else if((MSG_INVESTMENT_PLAN_BEGIN <= msgID) && (msgID <= MSG_INVESTMENT_PLAN_END))     // 9400
    {
        m_investment_plan_mgr->OnRecv(msgID, data);
    }
    else if ((MSG_PAY_POINT_LOTTERY_BEGIN <= msgID) && (msgID <= MSG_PAY_POINT_LOTTERY_END))    //9500
    {
        m_pay_point_lottery->OnRecv(msgID, data);
    }
    else if ((MSG_DONATE_BEGIN <= msgID) && (msgID <= MSG_DONATE_END))          // 9700
    {
        m_donate_mgr->OnRecv(msgID,data);
    }
    else if ((MSG_VIP_ONLY_DUNGEON_BEGIN <= msgID) && (msgID <= MSG_VIP_ONLY_DUNGEON_END))  // 10100
    {
        m_vip_only_dungeon->OnRecv(msgID, data);
    }
    else if ((MSG_TOP_UP_RANK_BEGIN <= msgID) && (msgID <= MSG_TOP_UP_RANK_END))        // 10000
    {
        if (msgID == MSG_TOP_UP_RANK_INFO_REQ)
        {
            TopUpRankManager::Instance()->SendTopUpRankInfoAck(this, data);
        }
    }
    else if ((MSG_EGG_BEGIN <= msgID) && (msgID <= MSG_EGG_END))        // 10200
    {
        m_egg_manager->OnRecv(msgID, data);
    }
    else if((MSG_BUY_CHEAP_BEGIN <= msgID) && (msgID <= MSG_BUY_CHEAP_END))     // 10300
    {
        m_buy_cheap_mgr->OnRecv(msgID, data);
    }
    else if((MSG_DAILY_FASHION_BEGIN <= msgID) && (msgID <= MSG_DAILY_FASHION_END))       // 10400
    {
        DailyFashionManager::Instance()->OnRecv(GetPlayerID(), msgID, data);
    }
    else if((MSG_HEAD_PORTRAIT_BEGIN <= msgID) && (msgID <= MSG_HEAD_PORTRAIT_END))           // 2300
    {
        m_headPortrait_mgr->OnRecv(msgID, data);
    }
    else if((MSG_SKILL_STRENGTHEN_BEGIN <= msgID) && (msgID <= MSG_SKILL_STRENGTHEN_END))             // 10600
    {
        m_skill_strengthen->OnRecv(msgID, data);
    }
    else if((MSG_ALCHEMY_BEGIN <= msgID) && (msgID <= MSG_ALCHEMY_END))             // 10800
    {
        m_alchemy_mgr->OnRecv(msgID, data);
    }
    else if((MSG_DECOMPOSE_BEGIN <= msgID) && (msgID <= MSG_DECOMPOSE_END))             // 10800
    {
        m_decompose_mgr->OnRecv(msgID, data);
    }
}

// ==================================================
// 接收基础属性消息
// ==================================================
void Player::OnRecvBaseProp(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_CHANGE_PLAYER_NAME_REQ:
        {   // 从客户端发送来改名请求
            RecvChangePlayerNameReq(data);
        }
        break;
    case MSG_CHANGE_PLAYER_NAME_ACK:
        {   // 从 centerserver 发送来的改名应答
            RecvChangePlayerNameAck(data);
        }
        break;
    case MSG_REFRESH_VIGOR_REQ:
        {
            RecvRefreshVigorReq();
        }
        break;
    case MSG_BUY_VIGOR_REQ:
        {
            RecvBuyVigorReq();
        }
        break;
    case MSG_CAREER_SET_REQ:            // 设置职业
        {
            RecvCareerSetReq(data);
        }
        break;
    case MSG_GUIDE_INFO_SET_REQ:        // 设置引导
        {
            RecvGuideInfoSetReq(data);
        }
        break;
    case MSG_OPEN_SYSTEM_SET_REQ:       // 设置功能开启
        {
            RecvOpenSystemInfoSetReq(data);
        }
        break;
    case MSG_PLAYER_LOG_REQ:
        {
            PLAYER_LOG_REQ* log = (PLAYER_LOG_REQ*)data;
            RecvPlayerLog(log->log);
        }
        break;
    case  MSG_LOG_REQ:
        {
            LOG_REQ* curdata = (LOG_REQ*)data;
            SendtoCenterLog(curdata->idflag, "%s", curdata->log.c_str());
        }
        break;
    case MSG_WELCOME_ASK_BAG_REWARD_REQ:
        {
            if(!GetWelcomeRewardFlag(FLAG_WELCOME_REWARD))
            { 
                WELCOME_DATA* curdata = (WELCOME_DATA*)data;
                SetWelcomeRewardFlag(FLAG_WELCOME_REWARD, true);

                //const REWARD_TABLE& welcomeRewardTable = WelcomeRewardConfig::Instance()->GetRewadTable();
                //for( std::vector<REWARD_ITEM>::const_iterator rewarditem =  welcomeRewardTable.items.begin(); rewarditem != welcomeRewardTable.items.end(); rewarditem++)
                //{
                //    GetItemManager()->AddItemByNum(rewarditem->id, rewarditem->num, REWARD_REASON_WELCOME_REWARD);
                //}

                if(curdata->moneypoint > 0)
                    AddMoneyPoint(curdata->moneypoint, REWARD_REASON_WELCOME_REWARD);
                if(curdata->growthpoint > 0)
                    AddGrowthPoint(curdata->growthpoint);
            }
        }
        break;
    case MSG_SERVER_TIME_SYN_REQ:
        {
            SERVER_TIME* pingTime = (SERVER_TIME*)data;
            m_Ping = pingTime->time;
            SendServerTimeSync();
        }
        break;
    case MSG_IS_UPDATE_PLAYER_NAME_REQ:
        {
            SendToCenter(MSG_IS_UPDATE_PLAYER_NAME_REQ, data);
        }
        break;
    case MSG_IS_UPDATE_PLAYER_NAME_ACK:
        {
            // 从 centerserver 发送来的改名应答
            SendToGate(MSG_IS_UPDATE_PLAYER_NAME_ACK, data);
        }
        break;
    default:
        break;
    }
}

// ==================================================
// 接收log信息
// ==================================================
void Player::RecvPlayerLog(const std::string& log)
{
    CnInfo("player %s client log:%s\n", GetName().c_str(), log.c_str());
}

// ==================================================
// 接收技能消息
// ==================================================
void Player::OnRecvSkill(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_USE_SKILL_REQ:     // 使用技能
        {
            if(!IsGM())
                return;

            USE_SKILL_REQ* pSkillReq = (USE_SKILL_REQ*)data;
            SkillArgumentData args;
            args.fromNetArgument(pSkillReq->skill_args);
            m_ActionController->UseSkill(pSkillReq->skill_id, pSkillReq->target, args, pSkillReq->skill_obj_id, pSkillReq->skill_slot);
        }
        break;
    case MSG_SKILL_COMPONENT_INFO_REQ:
    case MSG_USE_BASE_SKILL_REQ:
        GetSkillInfoMgr()->OnRecv(msgID, data);
        break;
    default:
        CnAssert(false);
        break;
    }
}

void Player::OnRecvBattle(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_AI_DEBUG_REQ:      // AI调试
        AI_DEBUG_REQ* pDebugReq = (AI_DEBUG_REQ*)data;
        BattleUnit* pBattleUnit = GetScene()->GetBattleUnit(pDebugReq->object_id);
        if(pBattleUnit)
        {
            AIManager::Instance()->AddDebugListener(this, pBattleUnit);
        }
        break;

    }
}

// ==================================================
// 接收触发器消息
// ==================================================
void Player::OnRecvTrigger(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_TRIGGER_ACTIVATE_REQ:
        {
            OnTriggerActivate(data);
        }
        break;
    default:
        break;
    }
}

// ==================================================
// 接收奖励池消息
// ==================================================
void Player::OnRewardTableReq(uint32 msgID, void* data)
{
    REWARD_TABLE_REQ* recvMsg= (REWARD_TABLE_REQ*)data;

    std::vector<REWARD_TABLE>::iterator rewardTableIt = m_reward_info.tables.begin();
    for(; rewardTableIt != m_reward_info.tables.end(); rewardTableIt++)
    {
        if(rewardTableIt->reason == recvMsg->reason && rewardTableIt->reason_arg == recvMsg->reason_arg)
        {
            break;
        }
    }
    if(rewardTableIt == m_reward_info.tables.end())
    {
        return;
    }

    TakeInRewardTable(&(*rewardTableIt));
    m_reward_info.tables.erase(rewardTableIt);
}

// ==================================================
// 得到离线玩家信息
// ==================================================
void Player::OnRecvOfflineInfoReq(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_OFFLINE_PLAYER_CLIENT_REQ:
        {
            OFFLINE_PLAYER_REQ* recvMsg = (OFFLINE_PLAYER_REQ*)data;
            uint64 offlinePlayerID = U64ID(recvMsg->player_id_h, recvMsg->player_id_l);
            if(offlinePlayerID)
            {
                OfflinePlayerManager::Instance()->RequestOfflinePlayerInfoByID(offlinePlayerID, recvMsg->reason, msgID, this);
            }
            else if(recvMsg->name.length() > 0)
            {
                OfflinePlayerManager::Instance()->RequestOfflinePlayerInfoByName(recvMsg->name, recvMsg->reason, msgID, this);
            }
        }
        break;
    default:
        break;
    }
}

void Player::OnKill(BattleUnit* sufferer)
{
    Creature::OnKill(sufferer);

    m_battle_recorder->OnKill(sufferer);

    // behavior log
    if(sufferer->GetObjectType() == OBJ_BOSS)
    {
        SendBehaviorLog("kill boss %d", ((Boss*)sufferer)->GetCreatureId());
    }
    else if(sufferer->GetObjectType() == OBJ_PLAYER)
    {
        SendBehaviorLog("kill player %s", ((Player*)sufferer)->GetName().c_str());
    }
    else if(sufferer->GetObjectType() == OBJ_MONSTER && GetLevel() < 10)
    {
        SendBehaviorLog("kill monster %d", ((Monster*)sufferer)->GetCreatureId());
    }
}

// ==================================================
// 防沉迷消息
// ==================================================
void Player::OnRecvFcm(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_FCM_INFO_REQ:              // 防沉迷信息请求
        {
            m_fcm_manager->OnFcmInfoReq(data);
        }
        break;
    default:
        break;
    }
}

// ==================================================
// 使用激活码消息
// ==================================================
void Player::OnRecvActivationCode(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_ACTIVECODE_USE_REQ:
        {
            ACTIVATION_CODE* recvMsg = (ACTIVATION_CODE*)data;
            if (recvMsg->type == GAME_ACTIVE_CODE)
            {
                SendToCenter(msgID, data);
            }
            else
            {
                RecvUseGiftCodeReq(data);
            }
        }
        break;
    default:
        CnAssert(false);
        break;
    }
}

// ==================================================
// 玩家复活相关
// ==================================================
void Player::OnRecvResurgenceReq(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_PLAYER_RESURGENCE:
        OnResurgenceReq(data);
        break;
    case MSG_DUNGEON_ADD_LIFE_COUNT_REQ:
        {
            DUNGEON_LIFE_COUNT* dungenLifeCount = (DUNGEON_LIFE_COUNT*)data;
            GetScene()->SendEvent(0, EVENT_DUNGEON_ADD_LIFE_COUNT_REQ, GetId(), dungenLifeCount->life_count, 0);
        }
        break;
    default:
        break;
    }
}

// ==================================================
// 虚空幻境消息
// ==================================================
void Player::OnVoidFieldRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_VOID_FIELD_CHANNEL_INFO_REQ:
    case MSG_VOID_FIELD_SET_HOOK_POS_REQ:
        VoidFieldManager::Instance()->OnPlayerRecv(this, msgID, data);
        break;
        // 本服虚空
    case MSG_VOID_FIELD_SOLO_CHANNEL_INFO_REQ:
        VoidFieldSoloManager::Instance()->OnPlayerChannelInfoReq(this, data);
        break;
    case MSG_VOID_FIELD_SOLO_SET_HOOK_POS_REQ:
        VoidFieldSoloManager::Instance()->OnSetHookPosReq(this, data);
        break;
    default:
        CnAssert(false);
        break;
    }

}

void Player::OnRecvGuild(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_GUILD_SCIENCE_PLAYER_INFO_REQ:
    case MSG_GUILD_ADDITION_PROMOTE_REQ:
    case MSG_GUILD_ADDITION_PROMOTE_TO_GAME:
        m_guild_science_mgr->OnRecv(msgID, data);
        break;
    default:
        GuildManager::Instance()->OnRecv(this, msgID, data);
        break;
    }
}

void Player::OnDead(BattleUnit* killer)
{
    Creature::OnDead(killer);

    // 下坐骑
    if(GetRideManager()->GetRidePetID())
    {
        GetRideManager()->RequestDownRide();
    }

    m_battle_recorder->OnDead(killer);

    // 宠物死亡
    GetPetController()->KillAllPet();

    GetStateController()->SetDeadTime(5000);        // 玩家死亡时间

    GetScene()->SendEvent(0, BATTLE_EVENT_PLAYER_DEAD, GetId(), (killer ? killer->GetId() : 0), 0);

    // behavior log
    SendBehaviorLog("dead");
}

void Player::OnStateChange(uint32 oldState, uint32 newState)
{
    Creature::OnStateChange(oldState, newState);

    // 战斗状态的处理
    m_battle_state_manager->OnStateChange(oldState, newState);

}

void Player::PublicMarquee(const std::string& chat)
{
    CHAT_MSG chat_msg;

    if(chat.length() <=0)
        return;

    chat_msg.src_name = GetName();
    chat_msg.channel = CHAT_CHANNEL_MARQUEE;
    chat_msg.chat = chat;
    OnRecv(MSG_CHAT_REQ, &chat_msg);
}

void Player::OnMarQuee(SERVER_MARQUEE& marquee)
{
    GateHandler::Instance()->SendToAll(MSG_SERVER_MARQUEE, &marquee, BROADCAST_PLAYER_ID);
}

void Player::SendMessageNotify( MESSAGE_NTF& message )
{
    SendToGate(MSG_MESSAGE_NTF, &message);
}

FCM_INFO& Player::GetFcmInfo()
{
    return m_fcm_manager->GetFcmInfo(); 
}

bool Player::IsInMemberTurnTable(uint32 id)
{
    for(std::vector<unsigned int>::iterator it = m_guild_welfare_info.turntable_item_list.begin();
        it != m_guild_welfare_info.turntable_item_list.end(); ++it)
    {
        if(id == *it) return true;
    }
    return false;
}

// ==================================================
// 请求更改角色名
// ==================================================
void Player::RecvChangePlayerNameReq(void* data)
{
    CHANGE_PLAYER_NAME_REQ* recvMsg = (CHANGE_PLAYER_NAME_REQ*)data;
    uint16 errcode = ERR_SUCCEED;

    if(!recvMsg) return;

    // TODO !!!
    // 这里做了一步假定，玩家上线的初始名称必定以#号开头，并且不允许第二次修改
    // 当玩家修改完一次名称后，#号为非法字符，所以不可能再出现在名字开头
    errcode = NameLimit::Instance()->IsValidNameStr(recvMsg->name);
    if(errcode != ERR_SUCCEED)
    {
        SendChangePlayerNameAck(errcode, recvMsg->name);
        return;
    }
    
    // 改名卡不足
    if(!SubBagItem(RENAMED_CARD_ID, 1))
    {
        SendChangePlayerNameAck(ERR_UNFOUND_ITEM, recvMsg->name);
        return;
    }

    std::string curname = GetPlatformServerName() + "." + recvMsg->name;
    recvMsg->name = curname;

    // 检查是否已设置角色名
    if(GetName() == recvMsg->name)
    {
        SendChangePlayerNameAck(ERR_SUCCEED, recvMsg->name);
        return;
    }

    SendToCenter(MSG_CHANGE_PLAYER_NAME_REQ, recvMsg);
}

// ==================================================
// 更改角色名应答
// ==================================================
void Player::RecvChangePlayerNameAck(void* data)
{
    CHANGE_PLAYER_NAME_ACK* recvMsg = (CHANGE_PLAYER_NAME_ACK*)data;

    if(!recvMsg) return;

    recvMsg->object_id = GetId();

    if(recvMsg->errcode == ERR_SUCCEED)
    {
        // 更改个人名称
        SetName(recvMsg->name);

        // 广播本场景
        GetScene()->NotifyMessage(this, MSG_CHANGE_PLAYER_NAME_ACK, recvMsg, true);

        uint16 err = SubItemOne(REWARD_REASON_RENAMED_CARD_ID, PROP_ITEM, 1, RENAMED_CARD_ID);
        if(err != ERR_SUCCEED)
        {
            recvMsg->errcode = err;
        }
    }

    SendToGate(MSG_CHANGE_PLAYER_NAME_ACK, recvMsg);
}

void Player::RecvRefreshVigorReq()
{
   /* REFRESH_VIGOR_ACK sendMsg;
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    RefreshVigor();

    if(GetVigorReftime() > now)
    {
        sendMsg.vigor_reftime = GetVigorReftime() - now;
    }
    else
    {
        sendMsg.vigor_reftime = 0;
    }

    SendToGate(MSG_REFRESH_VIGOR_ACK, &sendMsg);*/
}

// ==================================================
// 购买体力请求
// ==================================================
void Player::RecvBuyVigorReq()
{
    int32 buyTimes = 0;
    int32 moneyPoint = 0;
    int32 addValue = 0;
    uint16 errcode = ERR_FAILED;

    // 检查魔石数
    //moneyPoint = GameParamConfig::Instance()->GetBuyVigorMoneypoint();
    //if((moneyPoint <= 0) || (GetMoneyPoint() < moneyPoint))
    //{
    //    errcode = ERR_NO_ENOUGH_MONEYPOINT;
    //    goto _ERROR;
    //}

    //// 检查每次购买添加的魔石数
    //addValue = GameParamConfig::Instance()->GetBuyVigorAddvalue();
    //if(addValue < 0)
    //{
    //    goto _ERROR;
    //}

    //// 检查购买次数
    //// buyTimes = (int32)VipConfig::Instance()->GetVipBuyVigorTimes(GetVipLevel());
    //buyTimes = (int32)VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_2 , GetVipLevel());
    //if(GetVigorBuyTimes() >= buyTimes)
    //{
    //    errcode = ERR_MAX_VIGOR_BUYTIMES;
    //    goto _ERROR;
    //}

    // 增加体力购买次数，这里不考虑临界问题
    // IncVigorBuyTimes(); //交易成功在购买次数

    // 发送魔石交易订单
    TradeManager::Instance()->SendTradeOrderReq(this, REWARD_REASON_GUILD_REWARD_VIGOR, PROP_VIGOR, 0, addValue, moneyPoint);

    return;
//
//_ERROR:
//    NotifyErrorCode(errcode);
}

void Player::RecvGuideInfoSetReq(void* data)
{
    GUIDE_INFO* recvMsg = (GUIDE_INFO*)data;

    GetGuideManager()->SetGuideInfo(recvMsg->guide_data);
}

void Player::RecvOpenSystemInfoSetReq(void* data)
{
    OPEN_SYSTEM_INFO* recvMsg = (OPEN_SYSTEM_INFO*)data;

    GetOpenSystemManager()->SetOpenSystemInfo(recvMsg->open_system_data);
}

void Player::RecvCareerSetReq(void* data)
{
    CAREER_DATA* recvMsg = (CAREER_DATA*)data;

    SetCareer(recvMsg->career);

    // 根据职业给装备和技能
    int8 career = GetCareer();
    uint32 weaponItemID = 0;
    uint32 stoneItemID = 0;
    if(career == CAREER_WARRIOR)
    {
        weaponItemID = SystemConfigManager::Instance()->GetGameConfigValue("career_warrior_equip");
        stoneItemID = SystemConfigManager::Instance()->GetGameConfigValue("career_warrior_skill");
    }
    else if(career == CAREER_MAGE)
    {
        weaponItemID = SystemConfigManager::Instance()->GetGameConfigValue("career_mage_equip");
        stoneItemID = SystemConfigManager::Instance()->GetGameConfigValue("career_mage_skill");
    }
    else if(career == CAREER_ARCHER)
    {
        weaponItemID = SystemConfigManager::Instance()->GetGameConfigValue("career_archer_equip");
        stoneItemID = SystemConfigManager::Instance()->GetGameConfigValue("career_archer_skill");
    }
    else
        CnAssert(false);

    // 初始武器
    ItemEquip* pFirstWeapon = ItemFactory::CreateEquip(weaponItemID);
    GetItemManager()->AddToBag(0, pFirstWeapon);

    // 初始技能书
    ItemStone* pFirstSkill = ItemFactory::CreateStone(stoneItemID);
    GetItemManager()->AddToBag(0, pFirstSkill);
}

void Player::RecvUseGiftCodeReq(void* data)
{
    ACTIVATION_CODE* recvMsg = (ACTIVATION_CODE*)data;

    std::string qid = this->GetAccount();
    uint32 type = recvMsg->type;
    std::string key = ServerConfig::Instance()->GetLogin360Key();
    std::string serverName = GetPlatformServerName();

    MD5 md5Coder;
    char typeStr[11];
    Crown::SDUtoa(typeStr, 11, type);
    std::string inputStr = qid + serverName + std::string(typeStr) + key;
    md5Coder.update(inputStr);
    std::string md5Str = md5Coder.toString();

    uint16 errCode = ERR_ACTIVE_CODE_NOT_FOUND;

    //发过来的码和生成的码都转换成大写再比较
    transform (md5Str.begin(), md5Str.end(), md5Str.begin(), toupper);
    transform (recvMsg->activation_code.begin(),recvMsg->activation_code.end(), recvMsg->activation_code.begin(), toupper);

    if (Crown::SDStrcmp(recvMsg->activation_code.c_str(), md5Str.c_str()) == 0)
    {
        if (IsUsedGiftCode(type))
        {
            errCode = ERR_ACTIVE_CODE_USED;
            NotifyErrorCode(errCode);
        }
        else
        {
            UseGiftCode(type);
            ACTIVECODE_USE_ACK sendMsg;
            std::vector<REWARD_ITEM> rewardItem;
            if (!GiftCodeConfig::Instance()->GetRewards(type, rewardItem))
            {
                return;
            }

            for (std::vector<REWARD_ITEM>::iterator iter = rewardItem.begin(); iter != rewardItem.end(); ++iter)
            {
                sendMsg.items.items.push_back(*iter);
            }

            sendMsg.items.reason = REWARD_REASON_GIFT;
            sendMsg.player_name = GetName();

            //ItemManager::AddRewardTableWithEmail(GetCenterSvrID(), GetPlayerID(), sendMsg.items);
        }
    }
    else
    {
        NotifyErrorCode(errCode);
    }
}

// ==================================================
// 刷新组队次数
// ==================================================
void Player::TeamDungeonCalDailyRefresh()
{
    // 保存昨天组队副本
    //if(GetGainbackResourceManager()->IsSaveMaxCount())
    //    SaveYesterdayCount(OPEN_SYS_TEAM, GameParamConfig::Instance()->GetTeamDungeonDailyTimes());
    //else
    //    SaveYesterdayCount(OPEN_SYS_TEAM, m_base_info.team_dungeon_systimes);

    // 刷新组队副本系统赠送次数
    //SetTeamDungeonSysTimes(GameParamConfig::Instance()->GetTeamDungeonDailyTimes());

    // 设置组队购买次数
    //SetTeamBuyTimes(0);
}

void Player::CBVipTrade(const std::string& param)  // 买给自己
{
    Json::Reader reader;
    Json::Value value_json;

    if(!reader.parse(param, value_json))
        return;

    uint32 cardId         = value_json["vip_card_id"].asUInt();
    uint32 num              = value_json["num"].asUInt();
    int32 needMoneyPoint   = value_json["needMoneyPoint"].asInt();

    // 更新vip数据
    CBVipOffer(cardId, num);
}

void Player::CBVipOffer(int32 cardId, uint32 num)
{
    m_vip_manager->CBVipOffer(cardId, num);
}

// ==================================================
// 增加VIP时效
// ==================================================
void Player::AddVipTime(uint32 cardId, uint32 vipTime)
{
    m_vip_manager->AddVipTime(cardId, vipTime);
}

void Player::UseGiftCode(uint32 type)
{
    if (type == 0)
    {
        CnAssert(false);
        return;
    }

    uint32 index = (type - 1) / 8;
    uint32 bitIndex = (type - 1) % 8;
    if (index >= m_gift_code_info.use_info.size())
    {
        //为新礼品卡分配占位
        m_gift_code_info.use_info.resize(index + 1);
    }

    uint32 offsetValue = 1;
    offsetValue = offsetValue << bitIndex;

    m_gift_code_info.use_info[index] = m_gift_code_info.use_info[index] | offsetValue;
}

bool Player::IsUsedGiftCode(uint32 type)
{
    if (type == 0)
    {
        CnAssert(false);
        return false;
    }

    uint32 index = (type - 1) / 8;
    uint32 bitIndex = (type - 1) % 8;
    if (index >= m_gift_code_info.use_info.size())
    {
        //如果没有记录 即没有使用
        return false;
    }

    uint32 offsetValue = 1;
    offsetValue = offsetValue << bitIndex;

    return m_gift_code_info.use_info[index] & offsetValue;
}

void Player::PlayerClear()
{
    ClearStrength();         // 力量
    ClearAgility();          // 敏捷
    ClearSpirit();           // 智力
    ClearConstitution();     // 耐力

    m_net_state         = kStateInit;

    m_session_id        = 0;
    m_login_id          = 0;
    m_player_id         = 0;
    m_base_info.Clear();

    m_citymap_id        = 0;
    m_city_x            = 0;
    m_city_y            = 0;

    m_team_id           = 0;
    m_guild_id          = 0;
    m_guild_join_time   = 0;

    //m_gain_pool.Clear();                            // 获取池
    m_reward_info.Clear();                          // 奖励信息

    m_vip_level         = 0;
}

// ==================================================
// 加载角色数据
// ==================================================
void Player::Load(uint64 playerID, DELIVER_PLAYER_INFO_REQ* recvMsg)
{
    m_PlayerLoader->Load(playerID, recvMsg);
}

void Player::Save(WRITEBACK_PLAYER_INFO_REQ& sendMsg)
{
    m_PlayerSaver->Save(sendMsg);
}

void Player::OnLastSet()
{
    m_pet_mgr->OnEndSet();
}

// ==================================================
// 护魂迷宫管理器
// ==================================================
SoulDungeonManager* Player::GetSoulDungeonManager()
{
    return m_player_dungeon_mgr->GetSoulDungeonManager();
}

// ==================================================
// 战斗力
// ==================================================
int32 Player::GetBattlePower()
{
    //if (IsTeaserDungeon())
    //    return 801204;//999999;

    return 0;
}

// ==================================================
// 设置经验值
// ==================================================
void Player::SetExp(int32 val)
{
    if (GetLevel() >= SystemConfigManager::Instance()->GetMaxLevel())
    {
        return;
    }

    val = Crown::SDRange(0, val, GetMaxExp());
    m_base_info.exp = val;

    SetPropDirty();
}

// ==================================================
// 添加经验值
// ==================================================
void Player::AddExp(int32 val)
{
    if(val <= 0)
    {
        return;
    }

    // 给技能也提供经验
    GetSkillInfoMgr()->AddExp(val);
    // 给宠物也提供经验
    GetPetMgr()->AddExp(val);

    const int32 nMaxLevel = SystemConfigManager::Instance()->GetMaxLevel();
    if (GetLevel() >= nMaxLevel)
    {
        return;
    }

    int32 nMaxExp = GetMaxExp();
    val += GetExp();
    while(val >= nMaxExp)
    {
        val -= nMaxExp;
        AddLevel();
        SendMyselfEvent(EVNET_ADD_PlAYER_EXP, nMaxExp, 0, 0);

        nMaxExp = GetMaxExp();

        SetExp(0);

        if (GetLevel() >= nMaxLevel)
        {
            return;
        }
    }

    SetExp(val);
    SendMyselfEvent(EVNET_ADD_PlAYER_EXP, val, 0, 0);

    SetPropDirty();
}

void Player::AddKillExp(int32 exp, int32 defenderLevel)
{
    // 计算等级差经验
    int32 nDiff = abs(GetLevel() - defenderLevel);
    float fPara = 1.f - 0.01f * pow((float)nDiff, 2.f);
    exp = Crown::SDMax(0, (int32)(exp * fPara));
    if (exp <= 0)
    {
        return;
    }

    int32 nMultiple(BASE_RATE_NUM);
    GetBuffController()->IsExpMultiple(&nMultiple);
    AddExp(exp * nMultiple / BASE_RATE_NUM);
}

// ==================================================
// 玩家升一级
// ==================================================
void Player::AddLevel(int32 val)
{
    CnAssert(val == 1);

    int32 oldLevel = GetLevel();
    int32 maxlevel = SystemConfigManager::Instance()->GetMaxLevel();

    if (oldLevel > 0 && oldLevel < maxlevel)
    {
        SetLevel(GetLevel() + 1); // 升一级
        CnAssert(GetLevel() > oldLevel); // 必升级成功

        if(GetLevel() >= 10)
            AddPassiveSkillPoint(1); // n级开始,每升一级加一点天赋点
    }
    else
    {
        CnAssert(false); // oldLevel不可能小于1，也不可能满级
    }
}

void Player::SetUnionMapID(uint32 val)
{
    m_base_info.unionmap_id = val;
}

// ==================================================
// 刷新体力值
// ==================================================
void Player::RefreshVigor()
{
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
    uint32 interval = VigorConfig::Instance()->GetVigorInterval();
    int32 addval = VigorConfig::Instance()->GetVigorAddVal();

    if(GetSysVigor() < 0)
    {
        CnAssert(false);
        SetSysVigor(0);
    }

    // 检查体力刷新时间
    if(GetVigorReftime() <= now &&  ((GetVigor()+addval) <= GetMaxVigor()) )
    {
        //while((GetSysVigor() > 0) && (GetVigorReftime() <= now))
        if((GetSysVigor() > 0) && (GetVigorReftime() <= now))
        {
            SubSysVigor(addval);
            AddVigorReftime(interval);
        }
        if(GetVigorReftime() <= now)
        {
            SetVigorReftime(now + interval);
        }
    }
}

// ==================================================
// 获得系统体力
// ==================================================
int32 Player::GetVigor()
{
    int32 left_sys_vigor = GetMaxVigor() - GetSysVigor();

    if(left_sys_vigor < 0)
        left_sys_vigor = 0;

    return (left_sys_vigor + GetBuyVigor());
}

// ==================================================
// 增加系统体力值
// ==================================================
void Player::AddSysVigor(int32 val)
{
    int32 oldNum = GetVigor();

    m_base_info.sys_vigor += val;

    CnAssert(m_base_info.sys_vigor >= 0);

    if(GetSysVigor() < 0) 
        SetSysVigor(0);

    SendWealthLog(WEALTH_VIGOR, val, oldNum, GetVigor());

    SetPropDirty();
}

// ==================================================
// 减少系统体力值//这里的减实际上是加体力，加是减体力
// ==================================================
void Player::SubSysVigor(int32 val)
{
    int32 oldNum = GetVigor();

    CnAssert(val >= 0);

    if(val < 0) val = 0;

    if(m_base_info.sys_vigor > val)
    {
        m_base_info.sys_vigor -= val;
        JudgeSysVigor();
    }
    else
    {
        m_base_info.sys_vigor = 0;
    }

    SetPropDirty();

    SendWealthLog(WEALTH_VIGOR, -val, oldNum, GetVigor());
}

void Player::JudgeSysVigor()
{
    int32 max_sys_vigor = GetMaxVigor();
    if(m_base_info.sys_vigor < 0)
    {
        if(m_base_info.sys_vigor < (-max_sys_vigor))
            m_base_info.sys_vigor  = (-max_sys_vigor);
    }
}
// ==================================================
// 添加购买体力值
// ==================================================
void Player::AddBuyVigor(int32 val)
{
    int32 oldNum = GetVigor();

    m_base_info.buy_vigor += val;

    CnAssert(m_base_info.buy_vigor >= 0);

    if(GetBuyVigor() < 0)
        SetBuyVigor(0);

    SetPropDirty();

    SendWealthLog(WEALTH_VIGOR, val, oldNum, GetVigor());
}

// ==================================================
// 减少购买体力值
// ==================================================
void Player::SubBuyVigor(int32 val)
{
    int32 oldNum = GetVigor();

    m_base_info.buy_vigor -= val;

    CnAssert(m_base_info.buy_vigor >= 0);

    if(GetBuyVigor() < 0) SetBuyVigor(0);

    SetPropDirty();

    SendWealthLog(WEALTH_VIGOR, -val, oldNum, GetVigor());
}

// ==================================================
// 减少体力值
// ==================================================
int32 Player::SubVigor(int32 val)
{
    int32 oldNum = GetVigor();
    int32 vary = val;

    int32 cost_buy_vigor = 0;
    int32 left_sys_vigor = 0;

    left_sys_vigor = GetMaxVigor() - GetSysVigor();

    if(left_sys_vigor < 0)
        left_sys_vigor = 0;

    if(left_sys_vigor >= val)
    {
        m_base_info.sys_vigor += val;
        val = 0;
    }
    else
    {
        val -= left_sys_vigor;
        m_base_info.sys_vigor = GetMaxVigor();
    }

    if(GetBuyVigor() >= val)
    {
        m_base_info.buy_vigor -= val;
        cost_buy_vigor += val;
        val = 0;
    }
    else
    {
        val -= m_base_info.buy_vigor;
        cost_buy_vigor += m_base_info.buy_vigor;
        m_base_info.buy_vigor = 0;
    }

    SetPropDirty();

    SendWealthLog(WEALTH_VIGOR, -vary, oldNum, GetVigor());

    return cost_buy_vigor;
}

// ==================================================
// 获得最大体力值
// ==================================================
int32 Player::GetMaxVigor()
{
    return VigorConfig::Instance()->GetMaxVigor(this);
}

void Player::SetLevel(int32 val)
{
    val = Crown::SDRange(1, val, SystemConfigManager::Instance()->GetMaxLevel());

    m_level = val;

    if(GetNetState() != Player::kStateInGame)
    {
        return;
    }

    CalcProperty();    // 重新计算角色属性

    // 发送玩家升级
    SendMyselfEvent(EVENT_PLAYER_LEVEL_UP, GetLevel(), 0, 0);

    // 发送玩家升级
    SendTaskEvent(TASK_EVENT_UP_PLAYER_LEVEL, GetLevel(), 0, 0);

    // 发送玩家级别是否激活宠物
    SendMyselfEvent(EVENT_ACIIVATION_PET, GetLevel(), ACTIVATION_LEVEL, 0);

    // 发送激活骑宠刻印
    SendMyselfEvent(EVENT_RIDE_MARK_PET, 0, 0, 0);

    m_PropNotifier->NotifyProperty(PROP_LEVEL, GetLevel()); // 广播等级

    SetPropDirty();

    SendToCenterTeamMemberProp();   // 升级队伍信息通知

    // behavior log
    SendBehaviorLog("level=%d", m_level);
}

void Player::AddPassiveSkillPoint(int32 val)
{
	m_passive_skill_mgr->AddAvailablePoints(val); // 增加天赋点
}

void Player::ResetPassiveSkillPoint()
{
    m_passive_skill_mgr->ResetPoints(); // 天赋洗点
}

ANSWER_INFO& Player::GetAnswerInfo() 
{
    return m_answer_mgr->GetAnswerInfo();
}

// ==================================================
// 打开副本是否被置位
// ==================================================
//bool Player::IsPlayerDungeonOpen(uint32 dungeonID)
//{
//    return m_player_dungeon_mgr->IsDungeonOpen(dungeonID);
//}

// ==================================================
// 打开副本置位
// ==================================================
//void Player::SetPlayerDungeonFlag(uint32 dungeonID, uint32 flag)
//{
//    m_player_dungeon_mgr->SetDungeonFlag(dungeonID, flag);
//}

uint32 Player::GetWelcomeRewardTypeBit(int32 type)
{
    switch (type)
    {
    case FLAG_MICRO_CLIENT:
        return 1;
        break;
    case FLAG_WELCOME_REWARD:
        return 2;
        break;
    default:
        CnAssert(false);
        break;
    }
    return 0;
}

void Player::SetWelcomeRewardFlag(int32 type, bool flag)
{
    uint32 bit = GetWelcomeRewardTypeBit(type);
    if(bit == 0)
        return;

    if(flag)
        m_base_info.getwelcome_reward |= (1<<bit);
    else 
        m_base_info.getwelcome_reward &= (~(1<<bit));
}

bool Player::GetWelcomeRewardFlag(int32 type)
{
    uint32 bit = GetWelcomeRewardTypeBit(type);
    if(bit == 0)
        return true;        // 已经领过了

    bool already_gain =  m_base_info.getwelcome_reward & (1<<bit);
    return already_gain;
}

// ==================================================
// 是否需要隐藏Trigger
// ==================================================
bool Player::NeedDisappearTrigger(uint32 trigger_id)
{ 
    return m_task_mgr->NeedDisappearTrigger(trigger_id); 
}

void Player::SetGameStart(bool start) 
{
    m_game_start = start;
}

void Player::SetGameLock(bool lock)
{
    if(m_game_lock == lock)
        return;
    m_game_lock = lock;

    GAME_LOCK lockInfo;
    lockInfo.lock = m_game_lock ? 1 : 0;
    SendToGate(MSG_GAME_LOCK_NTF, &lockInfo);
}

// ==================================================
//  返回主线任务ID
// ==================================================
uint32 Player::GetMainTaskID()
{
    return m_task_mgr->GetMainTaskID(); 
}

// ==================================================
// 是否为红名
// ==================================================
bool Player::IsRedName()
{
    return m_enemy_mgr->IsRedName();
}

bool Player::IsPlayerCanSee(SceneObject* obj)
{
    if(!obj) return false;

    if(!GetScene() || !obj->GetScene())
        return false;

    if( obj->GetScene() != GetScene() )
        return false;     // 一定是在同场景中才能看到

    return GetScene()->IsPlayerCanSee(this, obj);
}

void Player::SetCareer(int8 val)
{
    m_base_info.career = val;
    SetPropDirty();
}

uint32 Player::GetRidePetID()
{
    return  m_ride_mgr->GetRidePetID();
}

void Player::SetRidePetID(uint32 val)
{
    m_ride_mgr->SetRidePetID(val);
}

uint32 Player::GetFaceID()
{
    return m_fashion_mgr->GetFaceID();
}

uint32 Player::GetWingID()
{
    return GetWingManager()->GetWingInfo().transmogrification;
}

uint32 Player::GetFashionID()
{
    return m_fashion_mgr->GetFashionID();
}

void Player::GainExp(int32 val)
{
    val = m_fcm_manager->GetFcmGainValue(val);
    //m_gain_pool.exp += val;
}

int32 Player::GetMaxExp()
{   
    return PlayerConfigManager::Instance()->GetMaxExp(GetLevel());
}

int32 Player::GetFcmGainValue(int32 val)
{
    return m_fcm_manager->GetFcmGainValue(val);
}
// ==================================================
// 获得防沉迷后的奖励列表
// ==================================================
void Player::GetFcmRewardList(const std::vector<REWARD_ITEM>& inList, std::vector<REWARD_ITEM>& outList)
{
    m_fcm_manager->GetFcmRewardList(inList, outList);
}

// ==================================================
// 设置金币
// ==================================================
void Player::SetGamePoint(int32 val)
{
    if(val > MAX_GAMEPOINT_NUM)
        val = MAX_GAMEPOINT_NUM;
    m_base_info.game_point = val;
    SetPropDirty();
}

// ==================================================
// 添加金币
// ==================================================
void Player::AddGamePoint(int32 val, int32 reason)
{
    if(val == 0) 
        return;

    int64 game_point = 0;

    int32 old_game_point = m_base_info.game_point;

    game_point = m_base_info.game_point + val;

    if(game_point < 0)
    {
        game_point = 0;
    }
    else if(game_point > MAX_GAMEPOINT_NUM)
    {
        game_point = MAX_GAMEPOINT_NUM;
    }
    m_base_info.game_point = (int32)game_point;
    SetPropDirty();

    // 大于1000金币才写日志
    if(val >= 1000)
        SendtoCenterLog(LOG_GAMEPOINT, WriterToLogByGamePoint(val, reason, old_game_point, m_base_info.game_point));
}

// ==================================================
// 删除金币
// ==================================================
bool Player::SubGamePoint(int32 val, int32 reason)
{
    if(val < 0)
    {
        CnAssert(false);
        return false;
    }

    int32 old_game_point = m_base_info.game_point;

    if(m_base_info.game_point >= 0 && m_base_info.game_point >= val)
    {
        m_base_info.game_point -= val;
    }
    else
    {
        m_base_info.game_point = 0;
        CnError("SubGamePoint overflow reason:%d", reason);
        return false;
    }

    SendBehaviorLog(PROP_GAMEPOINT, val, reason);

    SetPropDirty();
    return true;

    // 大于1000金币才写日志
    if(val >= 1000)
        SendtoCenterLog(LOG_GAMEPOINT, WriterToLogByGamePoint(-val, reason, old_game_point, m_base_info.game_point));
}

void Player::SendBehaviorLog(const char* format, ...)
{
    const uint32 MAX_LEN = LOG_STR_MAX_LEN-20;
    char str[MAX_LEN];
    memset(str, 0, MAX_LEN);

    va_list arglist;
    va_start(arglist, format);
    vsnprintf(str, MAX_LEN, format, arglist);
    va_end(arglist);


    // 加上战斗力及所在副本
    char str2[20];
    SDSnprintf(str2, 20, "%d\t%d\t", GetBattlePower(), GetScene() ? GetScene()->GetDungeon()->GetDungeonID() : 0);

    std::string allStr = str2;
    allStr += str;

    SendtoCenterLog(LOG_BEHAVIOR, "%s", allStr.c_str());
}

void Player::SendPlaySystemLog(int32 playSystemId, int32 arg1, int32 arg2, const std::string& argstr)
{
    SendtoCenterLog(LOG_PlAY_SYSTEM, WriterToLogByPlaySystem(playSystemId, arg1, arg2, argstr));
}

void Player::SendWealthLog(int32 wealthId, int32 num, int32 oldNum, int32 newNum)
{
    SendtoCenterLog(LOG_WEALTH, WriterToLogByWealth(wealthId, num, oldNum, newNum));
}

// ==================================================
// 发送 360 推送等级消息
// ==================================================
void Player::SendPush360LevelMsg()
{
    PUSH_360_NTF sendMsg;
    std::string jobStr = "";
    std::string genderStr = "";
    std::string signStr = "";
    std::string signMd5 = "";
    MD5 md5Coder;

    sendMsg.type = PUSH_360_LEVEL;

    // 性别
    if(GetSex() == SEX_MALE)
        genderStr = "m";
    else
        genderStr = "w";

    // 职业
    if(GetCareer() == CAREER_WARRIOR)
        jobStr = "warrior";
    else if(GetCareer() == CAREER_MAGE)
        jobStr = "mage";
    else
        jobStr = "priest";

    // sign = md5($gender.$gkey.$head.$job.$level.$marry.$name.$playid.$qid.$server_id.$toptype.$topvalue.$key);
    //如果post里没有的数据，在sign中也要相应的去

    // 合区的时候可能会有问题，因为当一个区一个帐号有多个角色时需要专递playerid
    signStr = genderStr+"sjsy"+jobStr+Crown::ToString(GetLevel());
    signStr += GetName()+GetAccount()+GetPlatformServerName();
    signStr += "BattlePower"+Crown::ToString(GetBattlePower());
    signStr += ServerConfig::Instance()->GetLogin360Key();

    // 角色信息
    sendMsg.msg += "qid="+GetAccount()+"&";
    sendMsg.msg += "server_id="+GetPlatformServerName()+"&";
    sendMsg.msg += "gkey=sjsy&";
    sendMsg.msg += "gender="+genderStr+"&";
    sendMsg.msg += "level="+Crown::ToString(GetLevel())+"&";
    sendMsg.msg += "name="+GetName()+"&";
    sendMsg.msg += "toptype=BattlePower&";
    sendMsg.msg += "topvalue="+Crown::ToString(GetBattlePower())+"&";
    sendMsg.msg += "job="+jobStr+"&";

    // 生成 MD5 码
    md5Coder.update(signStr);

    // 赋值md5码
    signMd5 = md5Coder.toString();

    // 消息
    sendMsg.msg += "sign="+signMd5;

    SendToCenter(MSG_PUSH_360_NTF, &sendMsg);
}

void Player::SendtoCenterLog(uint32 idflag, const char * format, ...)
{
    LOG_REQ msg;
    char str[LOG_STR_MAX_LEN];
    memset(str, 0, LOG_STR_MAX_LEN);
    va_list arglist;
    va_start(arglist, format);
    vsnprintf(str, LOG_STR_MAX_LEN, format, arglist);
    va_end(arglist);

    msg.log = str;
    msg.idflag = idflag;
    CenterHandler::Instance()->SendByPlayer(this, MSG_LOG_REQ, &msg, GetPlayerID());
}

void Player::SendtoCenterLog(uint32 idflag, const std::string& log_info)
{
    LOG_REQ msg;
    msg.log = log_info;
    msg.idflag = idflag;
    msg.agreementid = 0;
    CenterHandler::Instance()->SendByPlayer(this, MSG_LOG_REQ, &msg, GetPlayerID());
}

// ==================================================
// 增加经验
// ==================================================
void Player::GainGamePoint(int32 val)
{
    val = GetFcmGainValue(val);
    //m_gain_pool.game_point += val;
    SetPropDirty();
}

// ==================================================
// 设置魔石
// ==================================================
void Player::SetMoneyPoint(int32 val)
{
    if(val > MAX_MONEYPOINT_NUM)
        val = MAX_MONEYPOINT_NUM;
    m_base_info.money_point = val;
    SetPropDirty();
}

// ==================================================
// 添加魔石
// ==================================================
void Player::AddMoneyPoint(int32 val, int32 reason, uint32 itemID)
{
    if(val == 0)
        return;

    PAY_ORDER_INFO sendMsg;
    uint64 playerID = 0;

    sendMsg.Clear();

    //CnAssert(reason);

    playerID = GetPlayerID();

    sendMsg.player_id.id_h = U64ID_HIGH(playerID);
    sendMsg.player_id.id_l = U64ID_LOW(playerID);
    sendMsg.item_id = itemID;
    sendMsg.money_point = val;
    sendMsg.reason = reason;
    sendMsg.paystate = PAY_STATE_INIT;

    SendToCenter(MSG_PAY_ORDER_REQ, &sendMsg);
}

// ==================================================
// 删除魔石
// ==================================================
bool Player::SubMoneyPoint(int32 val, int32 reason)
{
    if(val < 0)
    {
        CnAssert(false);
        return false;
    }

    //CnAssert(reason);

    if((GetMoneyPoint() >= 0) && (GetMoneyPoint() >= val))
    {
        m_base_info.money_point -= val;
    }
    else
    {
        SetMoneyPoint(0);
        CnError("SubMoneyPoint overflow reason:%d", reason);
        return false;
    }
    SendBehaviorLog(PROP_MONEYPOINT, val, reason);
    SetPropDirty();
    return true;
}

void Player::SetVigorValue(int32 val)
{
    m_base_info.vigor = val;
    SetPropDirty();
}

bool Player::AddVigorValue(int32 val, int32 reason)
{
    if(val <= 0)
        return false;

    int32 oldNum = GetVigorValue();
    m_base_info.vigor += val;

    SetPropDirty();
    SendWealthLog(WEALTH_VIGOR, val, oldNum, GetVigorValue());

    return true;
}

bool Player::SubVigorValue(int32 val, int32 reason)
{
    if(val <= 0)
        return false;

    int32 oldNum = GetVigorValue();

    m_base_info.vigor -= val;

    SetPropDirty();
    SendWealthLog(WEALTH_VIGOR, -val, oldNum, GetVigorValue());

    return true;
}

// ==================================================
// 设置绑定魔石
// ==================================================
void Player::SetBindMoneyPoint(int32 val)
{
    if(val > MAX_BIND_MONEYPOINT_NUM)
    {
        val = MAX_BIND_MONEYPOINT_NUM;
    }

    m_base_info.bind_money_point = val;
    SetPropDirty();
}

// ==================================================
// 添加绑定魔石
// ==================================================
void Player::AddBindMoneyPoint(int32 val, int32 reason)
{
    if ((val < 0) || (GetBindMoneyPoint() < 0))
    {
        CnAssert(false);
    }

    int32 old_bind_money_point = m_base_info.bind_money_point;

    int64 bindMoneyPoint = m_base_info.bind_money_point + val;
    if(bindMoneyPoint > MAX_BIND_MONEYPOINT_NUM)
    {
        bindMoneyPoint = MAX_BIND_MONEYPOINT_NUM;
    }

    m_base_info.bind_money_point = (int32)bindMoneyPoint;

    SetPropDirty();
    //CnInfo("AddBindMoneyPoint %d, Reason:%d\n", val, reason);

    SendtoCenterLog(LOG_BINDMONEYPOINT, WriterToLogByBindMoneyPoint(val, reason, old_bind_money_point, m_base_info.bind_money_point));
}

// ==================================================
// 删除绑定魔石
// ==================================================
bool Player::SubBindMoneyPoint(int32 val, int32 reason)
{
    if(val < 0)
    {
        CnAssert(false);
        return false;
    }

    int32 old_bind_money_point = m_base_info.bind_money_point;

    if((GetBindMoneyPoint() >= 0) && (GetBindMoneyPoint() >= val))
    {
        m_base_info.bind_money_point -= val;
    }
    else
    {
        SetBindMoneyPoint(0);
        CnError("SubBindMoneyPoint overflow reason:%d\n", reason);
        return false;
    }

    SetPropDirty();
    CnInfo("SubBindMoneyPoint %d, Reason:%d\n", val, reason);

    SendtoCenterLog(LOG_BINDMONEYPOINT, WriterToLogByBindMoneyPoint(-val, reason, old_bind_money_point, m_base_info.bind_money_point));
    return true;
}
// ==================================================
// 设置精魂
// ==================================================
void Player::SetSoulPoint(int32 val)
{
    if(val > MAX_SOULPOINT_NUM)
        val = MAX_SOULPOINT_NUM;
    m_base_info.soul_point = val;
    SetPropDirty();
}

// ==================================================
// 添加精魂
// ==================================================
void Player::AddSoulPoint(int32 val, int32 reason)
{
    int64 soul_point = 0;

    int32 old_soul_point = m_base_info.soul_point;

    soul_point = m_base_info.soul_point + val;

    if(soul_point < 0)
    {
        soul_point = 0;
    }
    else if(soul_point > MAX_SOULPOINT_NUM)
    {
        soul_point = MAX_SOULPOINT_NUM;
    }
    m_base_info.soul_point = (int32)soul_point;
    SetPropDirty();

    if(val >= 1000)
        SendtoCenterLog(LOG_SOUL_POINT, WriterToLogBySoulPoint(val, reason, old_soul_point, m_base_info.soul_point));
}

// ==================================================
// 删除精魂
// ==================================================
void Player::SubSoulPoint(int32 val, int32 reason)
{
    CnAssert(val >= 0);

    int32 old_soul_point = m_base_info.soul_point;

    if(GetSoulPoint() >= 0 && GetSoulPoint() >= val)
    {
        m_base_info.soul_point -= val;
    }
    else
    {
        SetSoulPoint(0);
        CnError("SubSoulPoint overflow reason:%d", reason);
    }

    SendBehaviorLog(PROP_SOULPOINT, val, reason);
    SetPropDirty();

    if(val >= 1000)
        SendtoCenterLog(LOG_SOUL_POINT, WriterToLogBySoulPoint(-val, reason, old_soul_point, m_base_info.soul_point));
}


void Player::SetStarPoint(int32 val)
{
    if(val > MAX_STAR_SOUL_POINT_NUM)
        val = MAX_STAR_SOUL_POINT_NUM;
    m_base_info.star_point = val;
    SetPropDirty();
}

void Player::AddStarPoint(int32 val)
{
    int64 start_point = 0;
    start_point = m_base_info.star_point + val;

    if(start_point < 0)
    {
        start_point = 0;
    }
    else if(start_point > MAX_STAR_SOUL_POINT_NUM)
    {
        start_point = MAX_STAR_SOUL_POINT_NUM;
    }
    m_base_info.star_point = (int32)start_point;
    SetPropDirty();
}

void Player::SubStarPoint(int32 val, int32 reason)
{
    //CnAssert(val >= 0);
    //if(GetStarPoint() >= 0 && GetStarPoint() >= val)
    //{
    //    m_base_info.star_point -= val;
    //}
    //else
    //{
    //    m_base_info.star_point = 0;
    //    CnError("SubstarPoint overflow reason:%d", reason);
    //}

    //SendBehaviorLog(PROP_STAR_SOUL, val, reason);
    //SetPropDirty();
}
// ==================================================
// 设置荣誉值
// ==================================================
void Player::SetHonorPoint(int32 val)
{
    if(val > MAX_HONORPOINT_NUM)
        val = MAX_HONORPOINT_NUM;
    m_base_info.honor_point = val;
    SetPropDirty();
}

// ==================================================
// 添加荣誉值
// ==================================================
void Player::AddHonorPoint(int32 val)
{
    int32 oldNum = GetHonorPoint();

    int64 honor_point = 0;

    //val = GetFcmGainValue(val);

    honor_point = m_base_info.honor_point + val;

    if(honor_point < 0)
    {
        honor_point = 0;
    }
    else if(honor_point > MAX_HONORPOINT_NUM)
    {
        honor_point = MAX_HONORPOINT_NUM;
    }

    m_base_info.honor_point = (int32)honor_point;

    SetPropDirty();

    SendWealthLog(WEALTH_HONORPOINT, val, oldNum, GetHonorPoint());
}

// ==================================================
// 删除荣誉值
// ==================================================
void Player::SubHonorPoint(int32 val, int32 reason)
{
    int32 oldNum = GetHonorPoint();

    CnAssert(val >= 0);

    if(GetHonorPoint() >= 0 && GetHonorPoint() >= val)
    {
        m_base_info.honor_point -= val;
    }
    else
    {
        SetHonorPoint(0);
        CnError("SubSoulPoint overflow reason:%d", reason);
    }

    SendBehaviorLog(PROP_HONORPOINT, val, reason);
    SetPropDirty();

    SendWealthLog(WEALTH_HONORPOINT, -val, oldNum, GetHonorPoint());
}

// ==================================================
// 设置公会贡献点
// ==================================================
void Player::SetGuildPoint(int32 val)
{
    if(val > MAX_GUILDPOINT_NUM)
        val = MAX_GUILDPOINT_NUM;
    m_base_info.guild_point = val;
    SetPropDirty();
}

// ==================================================
// 添加公会贡献点
// ==================================================
void Player::AddGuildPoint(int32 val, int32 reason)
{
    int32 oldNum = GetGuildPoint();

    int64 guild_point = 0;
    int32 return_point = 0;

    guild_point = m_base_info.guild_point + val;

    if(guild_point < 0)
    {
        guild_point = 0;
    }
    else if(guild_point > MAX_GUILDPOINT_NUM)
    {
        guild_point = MAX_GUILDPOINT_NUM;
    }

    //if(REWARD_REASON_GUILD_BUY_GUILD_POINT == reason)
    //{
    //    return_point = GuildConfig::Instance()->GetBuyGuildPointReturnRate();
    //    return_point = (int32)((int64)guild_point * return_point / BASE_RATE_NUM);
    //}

    m_base_info.guild_point = (int32)guild_point;

    // 发送公会贡献点信息给数据库
    GUILD_ADD_GUILDPOINT_NTF sendMsg;
    sendMsg.player_id.id_h = U64ID_HIGH(GetPlayerID());
    sendMsg.player_id.id_l = U64ID_LOW(GetPlayerID());
    sendMsg.guild_point = val;
    SendToCenter(MSG_GUILD_ADD_GUILDPOINT_NTF, &sendMsg);

    //捐献贡献点捐献了个
    SendTaskEvent(TASK_EVENT_DONATE_GUILD_POINT, val, 0);

    //总共有多少贡献点
    SendTaskEvent(TASK_EVENT_GUID_POINT, val, 0);

    //SendBehaviorLog(PROP_GUILDPOINT, val, reason);
    SetPropDirty();

    SendWealthLog(WEALTH_GUILDPOINT, val, oldNum, GetGuildPoint());
}

// ==================================================
// 删除贡献点
// ==================================================
void Player::SubGuildPoint(int32 val, int32 reason)
{
    int32 oldNum = GetGuildPoint();

    CnAssert(val >= 0);

    if(GetGuildPoint() >= 0 && GetGuildPoint() >= val)
    {
        m_base_info.guild_point -= val;
    }
    else
    {
        m_base_info.guild_point = 0;
        CnError("SubGuildPoint overflow reason:%d", reason);
    }

    //SendBehaviorLog(PROP_GUILDPOINT, val, reason);
    SetPropDirty();

    SendWealthLog(WEALTH_GUILDPOINT, -val, oldNum, GetGuildPoint());
}

// ==================================================
// 设置化身点
// ==================================================
void Player::SetIncarnationPoint(int32 val)
{
    if(val > MAX_INCARNATION_NUM)
        val = MAX_INCARNATION_NUM;
    m_base_info.incarnation_point = val;
    SetPropDirty();
}

// ==================================================
// 添加化身点
// ==================================================
void Player::AddIncarnationPoint(int32 val, int32 reason)
{
    int32 oldNum = GetIncarnationPoint();

    int64 incarnation_point = 0;
    int32 return_point = 0;

    incarnation_point = m_base_info.incarnation_point + val;

    if(incarnation_point < 0)
    {
        incarnation_point = 0;
    }
    else if(incarnation_point > MAX_INCARNATION_NUM)
    {
        incarnation_point = MAX_INCARNATION_NUM;
    }

    //if(REWARD_REASON_GUILD_BUY_GUILD_POINT == reason)
    //{
    //    return_point = GuildConfig::Instance()->GetBuyGuildPointReturnRate();
    //    return_point = (int32)((int64)guild_point * return_point / BASE_RATE_NUM);
    //}

    m_base_info.incarnation_point = (int32)incarnation_point;

    SendBehaviorLog(PROP_INCARNATION_POINT, val, reason);
    SetPropDirty();

    SendWealthLog(WEALTH_INCARNATION, val, oldNum, GetIncarnationPoint());
}

// ==================================================
// 删除化身点
// ==================================================
void Player::SubIncarnationPoint(int32 val, int32 reason)
{
    int32 oldNum = GetIncarnationPoint();

    CnAssert(val >= 0);

    if(GetIncarnationPoint() >= 0 && GetIncarnationPoint() >= val)
    {
        m_base_info.incarnation_point -= val;
    }
    else
    {
        m_base_info.incarnation_point = 0;
        CnError("SubIncarnationPoint overflow reason:%d", reason);
    }

    SendBehaviorLog(PROP_INCARNATION_POINT, val, reason);
    SetPropDirty();

    SendWealthLog(WEALTH_INCARNATION, -val, oldNum, GetIncarnationPoint());
}

// ==================================================
// 设置战魂
// ==================================================
void Player::SetBattleSoulPoint(int32 val)
{
    if(val > MAX_BATTLE_SOUL_POINT_NUM)
        val = MAX_BATTLE_SOUL_POINT_NUM;
    m_base_info.battle_soul_point = val;
    SetPropDirty();
}

// ==================================================
// 添加战魂
// ==================================================
void Player::AddBattleSoulPoint(int32 val)
{
    int64 battle_soul = 0;

    //val = GetFcmGainValue(val);
    battle_soul = m_base_info.battle_soul_point + val;

    if(battle_soul < 0)
    {
        battle_soul = 0;
    }
    else if(battle_soul > MAX_BATTLE_SOUL_POINT_NUM)
    {
        battle_soul = MAX_BATTLE_SOUL_POINT_NUM;
    }

    m_base_info.battle_soul_point = (int32)battle_soul;

    SetPropDirty();
}

// ==================================================
// 删除战魂
// ==================================================
void Player::SubBattleSoulPoint(int32 val)
{
    if(GetBattleSoulPoint() >= 0 && GetBattleSoulPoint() >= val)
    {
        m_base_info.battle_soul_point -= val;
    }
    else
    {
        SetBattleSoulPoint(0);
    }

    SetPropDirty();
}

// ==================================================
// 单个消耗物品检测
// ==================================================
uint16 Player::CheckItemOne(int32 type, int32 num, uint32 itemID)
{
    return m_expend_mgr->CheckExpend(type, num, itemID);
}

// ==================================================
// 多个消耗物品检测
// ==================================================
uint16 Player::CheckItemTable(const REWARD_TABLE& itemList)
{
    return m_expend_mgr->CheckExpendItem(itemList);
}

// ==================================================
// 单个消耗物品处理
// ==================================================
uint16 Player::SubItemOne(uint8 reason, int32 type, int32 num, uint32 itemID /*= 0*/)
{
    if(type == PROP_ITEM && itemID == 0)
    {
        // 物品类型必须有ID
        CnAssert(false);
        return ERR_FAILED;
    }

    REWARD_TABLE expendList;
    expendList.reason = reason;

    REWARD_ITEM expendItem;
    expendItem.Clear();
    //expendItem.type = type;
    expendItem.id = itemID;
    expendItem.num = num;

    expendList.items.push_back(expendItem);

    return m_expend_mgr->ExpendItem(expendList);
}

// ==================================================
// 多个消耗物品处理
// ==================================================
uint16 Player::SubItemTable(const REWARD_TABLE& itemList)
{
    return m_expend_mgr->ExpendItem(itemList);
}

// ==================================================
// 减去背包里的物品
// ==================================================
bool Player::SubBagItem(uint32 itemID,int32 num)
{
    //if(num > GetItemManager()->GetItemNum(itemID))
    //{
    //    return false;
    //}
    //else
    //{
    //    return true;
    //}
    return false;
}

void Player::GainSoulPoint(int32 val) 
{
    val = GetFcmGainValue(val);
    m_battle_recorder->GainSoulPoint(val); 
    //m_gain_pool.soul_point += val; 
    SetPropDirty();
}

// 设置公会ID
void Player::SetGuildID(uint64 id)
{
    m_guild_id = id;

    m_PropNotifier->NotifyProperty(PROP_GUILD_NAME, 0, GetGuildName());
}

// ==================================================
// 返回公会名称
// ==================================================
std::string Player::GetGuildName()
{
    return GuildManager::Instance()->GetGuildName(GetGuildID());
}

// ==================================================
// 添加邮件
// ==================================================
void Player::AddMail(MAIL_INFO* mail)
{
    m_mail_mgr->AddMail(mail);
}

// ==================================================
// 删除过期邮件
// ==================================================
void Player::DelExpiredMail(uint64 expiredMailID)
{
    m_mail_mgr->DelExpiredMail(expiredMailID);
}

void Player::SendMail(std::string recvName, std::string title, std::string message, int32 type, const std::vector<REWARD_ITEM>& list)
{
    m_mail_mgr->SendMail(GetCenterSvrID(), GetPlayerID(), recvName, title, message, type, list);
}

// ==================================================
// 计算角色属性
// ==================================================
void Player::CalcProperty()
{
    // 计算角色等级提供的属性
    CalcLevelProp();

    // 计算装备提供的属性
    m_equip_mgr->CalcAllEquipProp();

    // 计算翅膀提供的属性
    m_wing_mgr->CalWingProp();

    // 计算天赋提供的属性
    m_passive_skill_mgr->CalcPassiveSkillProp();

    // 计算地图属性
    if(GetScene())
    {
        GetScene()->GetRegular()->CalcMapProp(this);
    }

    BattleUnit::CalcProperty();

    m_pet_mgr->CalcPetProperty();

    // 属性设置为脏
    SetPropDirty();
}

void Player::ClearProperty()
{
    BattleUnit::ClearProperty();

    // 一级属性
    ClearStrength();
    ClearAgility();
    ClearSpirit();
    ClearConstitution();
}

// ==================================================
// 计算基本属性
// ==================================================
void Player::CalcLevelProp()
{
    // 根据玩家等级获取基础属性
    const std::vector<uint32>& basePropertyIdList = GetBasePropertyIdList();

    PropertySet& propSet = GetPropertySetCalculater()->GetLevelProp();
    propSet.Clear();

    for(uint32 i=0; i< basePropertyIdList.size(); i++)
    {
        Property prop;
        prop.type = basePropertyIdList[i];
        prop.value = PlayerConfigManager::Instance()->GetPropByLevel(GetLevel(), prop.type);
        propSet.Add(prop);
    }

    // 高级属性
    propSet.Set(PROP_MAX_FIRE_DEFENCE, 75);
    propSet.Set(PROP_MAX_ICE_DEFENCE, 75);
    propSet.Set(PROP_MAX_LIGHTNING_DEFENCE, 75);
    propSet.Set(PROP_MAX_ATTACK_BLOCK, 75);
    propSet.Set(PROP_MAX_MAGIC_BLOCK, 50);
}

int32 Player::GetProp(uint32 type)
{
    switch(type)
    {
        // 力量
    case PROP_STRENGTH:
        return GetStrength();
        // 智力
    case PROP_SPIRIT:
        return GetSpirit();
        // 敏捷
    case PROP_AGILITY:
        return GetAgility();
        // 体质
    case PROP_CONSTITUTION:
        return GetConstitution();
    default:
        return BattleUnit::GetProp(type);
    }
}

void Player::SetProp(uint32 type, int32 val)
{
    switch(type)
    {
        // 力量
    case PROP_STRENGTH:
        SetStrength(val);
        break;
        // 智力
    case PROP_SPIRIT:
        SetSpirit(val);
        break;
        // 敏捷
    case PROP_AGILITY:
        SetAgility(val);
        break;
        // 体质
    case PROP_CONSTITUTION:
        SetConstitution(val);
        break;
    default:
        BattleUnit::SetProp(type, val);
        break;
    }

    SetPropDirty();
}

void Player::InitNewRoleDefaultSet()
{
    // 新角色刚创建
    if (GetDailyReftime() == 0)
    {
        VOID_FIELD_SETTING_INFO& void_field_set_info = GetSettingManager()->GetVoidFieldSettingInfo();
        // 默认断线自动重连进入虚空
        void_field_set_info.auto_use_revival = VoidFieldSettingConfig::Instance()->GetAutoUseRevival();
        void_field_set_info.auto_buy_revival = VoidFieldSettingConfig::Instance()->GetAutoBuyRevival();
        void_field_set_info.current_revival = VoidFieldSettingConfig::Instance()->GetCurrentRevival();
        void_field_set_info.reconnect_on_loss = VoidFieldSettingConfig::Instance()->GetReconnectOnLoss();
    }
}

//8点刷新
void Player::OnTimeFresh()
{
    Crown::CSDDateTime now = Crown::SDNow();
    Crown::CSDDateTime nextDay = Crown::SDNow();
    if(now.GetTimeValue() >= m_base_info.void_field_reftime)
    {
        Crown::CSDDateTime  time = VoidFieldConfig::Instance()->GetUpdateTime();
        uint32 hour1 = time.GetHour();
        uint32 minute1 = time.GetMinute();
        uint32 second1 = time.GetSecond();

        SetVoideFiedlTime(0);
        if(GetScene())
            GetScene()->SendEvent(0, EVENT_VOID_FIELD_CLEAR_INTOTIME, 0, 0, 0);

        nextDay.SetTime(hour1,minute1,second1);
        nextDay.IncDay(1);
        SetVoidFieldReftime((uint32)nextDay.GetTimeValue());
    }
}
// ==================================================
// 计算每日刷新值(上线和过了一天都会调用一次)
// ==================================================
void Player::CalDailyRefresh()
{
    // 计算体力值
    //RefreshVigor();

    // 重新计算玩家罪恶值
    m_enemy_mgr->CalEvilPoint();

    // 计算是否是新的一天
    time_t curTime = Crown::SDNow().GetTimeValue();
    int32 days = GetDaysBetweenDates(curTime, (time_t)m_base_info.daily_reftime);
    if (days > 0)
    {
        CnInfo("player %s CalDailyRefresh\n", GetName().c_str());

        // 第一次登陆游戏
        /*if(m_base_info.daily_reftime == 0)
        {
            SendPush360LevelMsg();
        }*/

        // 更新刷新时间
        m_base_info.daily_reftime = (uint32)curTime;



        // 刷新任务
        m_task_mgr->CalDailyRefresh();

        //// 刷新资源挽回信息     必须放在最前面先判断是否是昨天
        //m_gainback_resource_mgr->CalDailyRefresh(days);

        //// 如果不在同一天，则刷新体力购买次数
        //SetVigorBuyTimes(0);

        //// 设置点石成金的次数为0
        //SetStoneTurnGoldTimes(0);

        //// 怪物之门每日刷新
        //m_monster_door_mgr->CalDailyRefresh();

        //// 勇者争霸每日刷新
        //m_player_door_mgr->CalDailyRefresh();

        //// 至尊魔宫每日刷新
        //m_babel_mgr->CalDailyRefresh();

        //// 活跃度每日刷新
        //m_liveness_mgr->CalDailyRefresh();

        //// 刷新副本次数
        //m_player_dungeon_mgr->DungeonTimesRefresh();

        //// 刷新宠物
        //m_follow_pet_mgr->CalDailyRefresh();

        //// 刷新限购
        //m_limitbuy_mgr->CalDailyRefresh();

        //// 单人答题
        //m_single_answer_mgr->CalDailyRefresh();

        //// 刷新组队副本
        //TeamDungeonCalDailyRefresh();

        //// 每日VIP礼包奖励领取
        //m_vip_manager->CalDailyRefresh(days);

        //// 刷新在线奖励
        //m_online_reward_mgr->CalDailyRefresh();

        //// 更新公会福利信息
        //GuildManager::Instance()->OnGuildWelfareInfoReq(this, NULL);

        ////更新公会每天的购买次数
        //GuildManager::Instance()->OnFreshDayGuildBuyTimes(this);

        //// 刷新签到数据
        //m_sign_mgr->LoginRewardsCountRefresh();
        //m_sign_mgr->OnSignInfoReq();

        //// 刷新副本进入次数
        //m_dungeon_times_mgr->CalDailyRefresh();

        //// 刷新宝库
        //m_treasury_mgr->CalDailyRefresh();

        //// 刷新打金像
        //m_goldtree_mgr->CalDailyRefresh();

        //// 刷新进入僵尸生存次数
        //m_zombie_mgr->CalDailyRefresh();

        //// 勇敢的心
        //m_bh_mgr->CalDailyRefresh();

        //// 刷新公会科技数据
        //m_guild_science_mgr->CalDailyRefresh();

        ////刷新投资理财
        //m_day_investment_mgr->CalDailyRefresh();

        ////清空充值积分累计点
        //m_pay_point_lottery->CalDailyRefresh();

        ////VIP独享副本
        //m_vip_only_dungeon->CalDailyRefresh();

        ////离线体力
        //m_offline_vigor_mgr->CalDailyRefresh();

        ////跨服战斗
        //m_multiserver_battle_info->CalDailyRefresh();

        //// 刷新越买越便宜
        //m_buy_cheap_mgr->CalDailyRefresh();

        // 写入每天登录的用户
        SendtoCenterLog(LOG_LOGIN, "");
    }

    // 修罗场刷新
    //m_ashura_recorder->CalDailyRefresh();

    // 称号刷新
    //m_title_mgr->CalDailyRefresh();
}
// ==================================================
// 排行榜排名结束通知
// ==================================================
void Player::RankingDailyRefresh()
{
    m_player_door_mgr->OnRankingDailyRefresh();
}
//每小时刷新
void Player::RankingHourRefresh()
{
    m_title_mgr->OnRankingHourRefresh();
}

// ==================================================
// 第一次进入游戏触发
// ==================================================
void Player::FirstEnterGame()
{
    // 推送gmt更新过的xml数据
    GAME_SETTING_LIST gameSettingList;
    gameSettingList = GameSettingManager::Instance()->GetGameSettingList();
    SendToGate(MSG_GAME_SETTING_LIST_ACK, &gameSettingList);

    // 任务
    m_task_mgr->OnTaskFestivalTemplateListReq();    // 先推送节日任务模板，再推送节日数据(最先做)

    m_task_mgr->NotifyGameTaskNum();

    m_task_mgr->InitTaskState();                // 初始化任务状态

    m_task_mgr->LinkageCanTakeTask();           // 联动可添加任务

    //// 善恶系统
    //m_enemy_mgr->SendEnemyInfoNtf();            // 推送善恶信息

    //// 点石成金
    //m_stone_turn_gold_mgr->UpdateStoneTurnGoldTimes();

    //// 单人问答
    //m_single_answer_mgr->NotifyNum();

    //// 称号
    //m_title_mgr->OnRankingHourRefresh();        // 刷新称号

    //// 僵尸生存战
    //m_zombie_mgr->NotifyNum();

    ////第一次进入游戏的时间
    //if(m_task_mgr->IsStartCountOnlineTime())
    //    SetLastExitTime((uint32)(Crown::SDNow().GetTimeValue()));
    ////计算离线时间统计
    //m_offline_vigor_mgr->CalOffLineVigorTime();
    //
    //// 初始化buff
    //GetBuffController()->FirstEnterGameInitBuff();

    //// 检查是否有扫荡完成
    ////NONE req;
    ////GetSoulDungeonManager()->OnRaidsSoulDungeonCmplReq(&req);

    //SendToCenterTeamMemberProp();
}
// 通知CenterServer角色属性发生改变(升级,跳副本,跳场景时调用)
void Player::SendToCenterTeamMemberProp()
{
    if(GetTeamID() || GetGuildID()) 
    {
        MEMBER_PROP_NTF member_prop;
        member_prop.level = GetLevel();                   // 等级
        member_prop.battle_power = GetBattlePower();      // 战斗力
        member_prop.dungeon_id = GetUnionMapID();         // 副本id
        SendToCenter(MSG_MEMBER_PROP_NTF, &member_prop);
    }
}

// ==================================================
// 保存昨日玩法信息
// ==================================================
void Player::SaveYesterdayCount(uint32 id, int32 count)
{
    //m_gainback_resource_mgr->SaveYesterdayCount(id, count);
}

// ==================================================
// 发送玩家登陆信息
// ==================================================
void Player::SendPlayerInfo(Scene* pScene)
{
    PLAYER_INFO sendMsg;

    // 角色属性
    FillPropInfo(sendMsg.prop, pScene->GetUnionID());

    // 填充副本信息
    m_PlayerSaver->FillPlayerDungeonInfo(sendMsg.dungeon_info);

    // 技能信息
    m_PlayerSaver->FillSkillComponentInfo(sendMsg.skill_component_info);

    // 装备信息
    m_PlayerSaver->FillEquipInfo(sendMsg.equip_info);

    // 佣兵信息
    m_PlayerSaver->FillPetInfo(sendMsg.pet_info);

    // 药剂信息
    m_PlayerSaver->FillPotionInfo(sendMsg.potion_info);

//     // 时装信息
//     m_PlayerSaver->FillFashionInfo(sendMsg.fashion_info);

    // 物品信息
    m_PlayerSaver->FillItemInfo(sendMsg.item_info);

    m_PlayerSaver->FillStoreInfo(sendMsg.store_info);
    

    // 隐藏包裹
    //m_PlayerSaver->FillHideBagInfo(sendMsg.hide_bag);

    // 任务信息
    m_PlayerSaver->FillTaskInfo(sendMsg.task_info);

    // 引导信息
    m_PlayerSaver->FillGuideInfo(sendMsg.guide_info);

    // 功能开启信息
    m_PlayerSaver->FillOpenSystemInfo(sendMsg.open_system_info);

    // 称号信息
    m_PlayerSaver->FillTitleInfo(sendMsg.title_info);

    // 勇敢的心信息
    m_PlayerSaver->FillBhInfo(sendMsg.bh_info);

    // 游戏收藏信息
    m_PlayerSaver->FillShorcutInfo(sendMsg.shorcut_info);

    // 当前服务器时间
    sendMsg.time = (uint32)Crown::SDNow().GetTimeValue();
    // 开服时间
    sendMsg.server_open_time = GameSettingManager::Instance()->GetServerOpenTime();

    // GameServer信息
    sendMsg.gameserver_id = ServerConfig::Instance()->GetMyID();
    sendMsg.gameserver_type = ServerConfig::Instance()->GetMyType();
    sendMsg.is_merge_server = ServerConfig::Instance()->IsMergeServer()?1:0;
    sendMsg.gameserver_language = ConfigManager::Instance()->GetLanguage();
    sendMsg.gameserver_timezone = Crown::SDGetTimeZone();

    // 没必要发送此信息
    sendMsg.task_info.task_game_info.branch_task_flag.clear();

    SendToGate(MSG_PLAYER_INFO, &sendMsg);
}

// ==================================================
// 发送数据给Gate
// ==================================================
bool Player::SendToGate(uint32 msgID, void* data)
{
    if (MSG_BATTLEUNIT_APPEAR_LIST == msgID)
    {
        int32 i=0;
    }

    uint32 gateID = GetGateSvrID();
    return GateHandler::Instance()->SendByGateID(gateID, msgID, data, GetPlayerID());
}

// ==================================================
// 发送数据给Center
// ==================================================
bool Player::SendToCenter(uint32 msgID, void* data)
{
    return CenterHandler::Instance()->SendByPlayer(this, msgID, data, GetPlayerID());
}

// ==================================================
// 切换副本前处理
// ==================================================
void Player::ReadyChangeToDungeon(CHANGE_DUNGEON_REQ* dungeonReq)
{
    
}

// ==================================================
// 退出场景前处理
// ==================================================
void Player::OnExitDungeon()
{
    // 状态停止
    if(GetActionController())
    {
        GetActionController()->Stop();
    }

    // 退出副本,去掉所有buff
    m_BuffController->ClearAllBuff();

    //退出副本的时候，计算在线时常
    CalOnlineCoutTime();

    // 重新计算所有属性
    //CalcProperty();
    //m_PlayerPropNotifier->ForceUpdate();

    GuildManager::Instance()->ExitGame(this);
}

//计算总的在线时常
void Player::CalOnlineCoutTime()
{
    if(!m_task_mgr) return;
    if (m_task_mgr->IsStartCountOnlineTime())
    {
        uint32 maxday = 103600000;         //最大天数20天
        //在线累计时长
        uint32 exittime = (uint32)(Crown::SDNow().GetTimeValue());
        if(exittime > m_lastexit_dugeon_time)
        {
            uint32 howtime = exittime - m_lastexit_dugeon_time;
            m_base_info.online_time = m_base_info.online_time + howtime;
            if(m_base_info.online_time > maxday)
            {
                m_base_info.online_time = maxday;
            }
            m_lastexit_dugeon_time = exittime;
        }
    }
}
// ==================================================
// 任务结算副本
// ==================================================
void Player::OnTaskSettleDungeon(uint32 dungeonID, bool success, bool raids)
{
    m_task_mgr->OnSettleDungeon(dungeonID, success, raids);
}

// ==================================================
// 发送任务事件
// ==================================================
void Player::SendTaskEvent(uint32 event_id, int32 arg1, int32 arg2, int32 arg3)
{
    m_task_mgr->SendTaskEvent(event_id, arg1, arg2, arg3);
}

void Player::SendMyselfEvent(int32 event_type, int32 arg1, int32 arg2, int32 delayTime)
{
    Scene* scene = 0;
    scene = GetScene();
    if(scene)
    {
        GetScene()->SendEvent(GetId(), event_type, arg1, arg2, delayTime);
    }
    else if(GetNetState() == Player::kStateInGame)
    {
        CnWarn("Event Error: scene is not exist! player_id:%llu, event_type:%d.\n", GetPlayerID(), event_type);
    }
}

void Player::ClearGainPool()
{
    //m_gain_pool.exp = 0;
    //m_gain_pool.game_point = 0;
    //m_gain_pool.soul_point = 0;
    //m_gain_pool.items.clear();

    m_allRecharey = 500;
}

// 副本所获，转入奖励池中
void Player::GainPoolToRewardInfo()
{
    REWARD_TABLE rewardTable;
    rewardTable.Clear();
    rewardTable.reason = REWARD_REASON_CLEAR_DUNGEON;

    REWARD_ITEM item;
    item.Clear();
    item.flag = REWARD_FLAG_NORMAL;
    item.id = PROP_EXP;
    //item.num = m_gain_pool.exp;
    rewardTable.items.push_back(item);

    item.Clear();
    item.flag = REWARD_FLAG_NORMAL;
    item.id = PROP_GAMEPOINT;
    //item.num = m_gain_pool.game_point;
    rewardTable.items.push_back(item);

    item.Clear();
    item.flag = REWARD_FLAG_NORMAL;
    item.id = PROP_SOULPOINT;
    //item.num = m_gain_pool.soul_point;
    rewardTable.items.push_back(item);

    //for(std::vector<REWARD_ITEM>::iterator iter = m_gain_pool.items.begin(); iter != m_gain_pool.items.end(); iter++)
    //{
    //    rewardTable.items.push_back(*iter);
    //}

    // VIP额外奖励
    // ...

    m_reward_info.tables.push_back(rewardTable);

    ClearGainPool();
}

// 添加奖励表到奖励池
void Player::AddRewardTableToPool(const REWARD_TABLE& table)
{
    REWARD_TABLE rewardTable = table;
    FuseSameReward(rewardTable);
    m_reward_info.tables.push_back(rewardTable);
}

void Player::SendReasonTablePool(int32 reason,int32 reasionarg)
{
    for(std::vector<REWARD_TABLE>::iterator iter = m_reward_info.tables.begin(); iter != m_reward_info.tables.end(); iter++)
    {
        if(iter->reason == reason&& (iter->reason_arg == reasionarg))
        {
            REWARD_TABLE& rewardTable = *iter;
            SendRewardTable(rewardTable);
        }
        else if(reasionarg == 0)
        {
            if(iter->reason == reason)
            {
                REWARD_TABLE& rewardTable = *iter;
                SendRewardTable(rewardTable);
            }
        }
    }
}
void Player::TakeInResasionRewardTable(int32 reason,int32 reasionarg)
{
    for(std::vector<REWARD_TABLE>::iterator iter = m_reward_info.tables.begin(); iter != m_reward_info.tables.end(); iter++)
    {
        if(iter->reason == reason&& (iter->reason_arg == reasionarg))
        {
            REWARD_TABLE rewardTable = *iter;
            TakeInRewardTable(&rewardTable);
        }
    }
}
void Player::DeleteTablePool(int32 reason,int32 reasionarg)
{
    std::vector<REWARD_TABLE>::iterator iterTable;
    for (iterTable = m_reward_info.tables.begin(); iterTable != m_reward_info.tables.end();)
    {
        if((iterTable->reason == reason) && (iterTable->reason_arg == reasionarg))
        {
            iterTable = m_reward_info.tables.erase(iterTable);

        }
        else
        {
            iterTable++;
        }
    }
}

int32 Player::JudegeRewTableNum(int32 reason ,REWARD_ITEM* rewardItem,int32 reasionarg)
{
    int32 tableItemNum = 0;
    std::vector<REWARD_TABLE>::iterator iterTable;
    std::vector<REWARD_ITEM>::iterator rewardIt;

    if(!rewardItem) return 0;

    for (iterTable = m_reward_info.tables.begin(); iterTable != m_reward_info.tables.end(); iterTable++)
    {
        if(iterTable->reason == reason&& (iterTable->reason_arg == reasionarg))
        {
            for(rewardIt = iterTable->items.begin(); rewardIt != iterTable->items.end(); rewardIt++)
            {
                //if(rewardIt->type == rewardItem->type && rewardIt->flag == rewardItem->flag && rewardIt->id == rewardItem->id)
                if(rewardIt->flag == rewardItem->flag && rewardIt->id == rewardItem->id)
                {
                    tableItemNum = tableItemNum +rewardIt->num;
                }
            }
        }
    }
    return tableItemNum;
}

void Player::SetRewTableItemNum(int32 reason, REWARD_ITEM* rewardItem, int32 reasionarg)
{
    std::vector<REWARD_TABLE>::iterator iterTable;
    std::vector<REWARD_ITEM>::iterator rewardIt;

    if(!rewardItem) return;

    for (iterTable = m_reward_info.tables.begin(); iterTable != m_reward_info.tables.end(); iterTable++)
    {
        if(iterTable->reason == reason&& (iterTable->reason_arg == reasionarg))
        {
            for(rewardIt = iterTable->items.begin(); rewardIt != iterTable->items.end(); rewardIt++)
            {
                //if(rewardIt->type == rewardItem->type && rewardIt->flag == rewardItem->flag && rewardIt->id == rewardItem->id)
                if(rewardIt->flag == rewardItem->flag && rewardIt->id == rewardItem->id)
                {
                    rewardIt->num = rewardItem->num;
                }
            }
        }
    }
}

// 奖励表物品进入包裹
ERROR_CODE Player::TakeInRewardTable(const REWARD_TABLE* table)
{
    return GetItemManager()->AddRewardTable(table);
}

// 奖励物品进入包裹
ERROR_CODE Player::TakeInRewardTable(int32 itemType, uint32 itemID, int32 itemNum, int32 reason, int32 reason_arg)
{
    REWARD_TABLE rewardTable;
    REWARD_ITEM rewardItem;

    rewardTable.Clear();
    rewardTable.reason = reason;
    rewardTable.reason_arg = reason_arg;

    //rewardItem.type = itemType;
    rewardItem.id = itemID;
    rewardItem.num = itemNum;

    rewardTable.items.push_back(rewardItem);

	return GetItemManager()->AddRewardTable(&rewardTable); 
  
}

// 进入城镇后奖励结算
void Player::OnRewardInfoCount()
{
    Scene* pScene = GetScene();
    if (pScene)
    {
        Dungeon* dungeon = pScene->GetDungeon();
        if( dungeon && IsWorldMap(dungeon->GetDungeonID()) )
        {
            for(std::vector<REWARD_TABLE>::iterator iter = m_reward_info.tables.begin(); iter != m_reward_info.tables.end(); iter++)
            {
                REWARD_TABLE& rewardTable = *iter;

                TakeInRewardTable(&rewardTable);
            }
            m_reward_info.Clear();
        }
    }
}

// ==================================================
// 添加副本计数
// ==================================================
void Player::OnAddDungeonCount(uint32 dungeonID)
{
    m_dungeon_times_mgr->AddEnterCount(dungeonID);
}

// ==================================================
// 发送请求切换副本
// ==================================================
void Player::SendChangeDungeon(uint32 dungeonID)
{
    CHANGE_DUNGEON_REQ sendMsg;
    sendMsg.dungeon_id = dungeonID;
    SendToCenter(MSG_CHANGE_DUNGEON_REQ, &sendMsg);
}

void Player::onEnterScene(Scene* pScene)
{
    Creature::onEnterScene(pScene);

    m_battle_recorder->onEnterScene(pScene);
    m_title_mgr->onEnterScene(pScene);

    GetActionController()->Stop();

    CalcProperty();

    // behavior log
    SendBehaviorLog("enter scene d=%d s=%d c=%d", pScene->GetDungeonID(), pScene->GetSceneID(), pScene->GetChannelID());
}

void Player::onLeaveScene(Scene* pScene)
{
    Creature::onLeaveScene(pScene);

    m_battle_recorder->onLeaveScene(pScene);
}

void Player::Update(uint32 frame_count)
{
    uint32 dungeonID = DUNGEON_ID(GetUnionMapID());

    Creature::Update(frame_count);

    m_recv_filter->Update();

    m_battle_recorder->Update();

    m_ashura_recorder->Update();

    m_PlayerPropNotifier->Update();

    m_treasury_mgr->Update();

    m_enemy_mgr->Update();

    m_online_reward_mgr->Update();

    m_battle_state_manager->Update();

    m_item_mgr->Update();

    m_item_controller->Update(frame_count);

    m_vigor_mgr->Update(frame_count);

    m_auto_save_time += g_ServerUpdateTime;      // 自动回存计时

    m_pet_mgr->Update(frame_count);
}

void Player::SendRewardTable(const REWARD_TABLE& rewardTable)
{

}

void Player::SendRewardTableList(const REWARD_TABLE_LIST& rewardTableList)
{

}

int32 Player::SubTeamDungeonTimes(int32 val)
{
    int32 cost_buy_times = 0;

    if(val < 1) val = 0;
    if(GetTeamDungeonSysTimes() < 0) SetTeamDungeonSysTimes(0);
    if(GetTeamDungeonBuyTimes() < 0) SetTeamDungeonBuyTimes(0);

    if(GetTeamDungeonSysTimes() >= val)
    {
        SubTeamDungeonSysTimes(val);
        val = 0;
    }
    else
    {
        val -= GetTeamDungeonSysTimes();
        SetTeamDungeonSysTimes(0);
    }

    if(val > 0)
    {
        if(GetTeamDungeonBuyTimes() >= val)
        {
            SubTeamDungeonBuyTimes(val);
            cost_buy_times += val;
            val = 0;
        }
        else
        {
            val -= GetTeamDungeonBuyTimes();
            cost_buy_times += GetTeamDungeonBuyTimes();
            SetTeamDungeonBuyTimes(0);
        }
    }

    SetPropDirty();

    return cost_buy_times;
}

// ==================================================
// 添加组队副本次数
// ==================================================
void Player::AddTeamDungeonSysTimes(int32 val)
{
    CnAssert(val > 0);
    if(val > 0)
    {
        m_base_info.team_dungeon_systimes += val;
    }
}

// ==================================================
// 删除组队副本次数
// ==================================================
void Player::SubTeamDungeonSysTimes(int32 val)
{
    CnAssert(val > 0);
    if((val >= 0) && (GetTeamDungeonSysTimes() > val))
    {
        m_base_info.team_dungeon_systimes -= val;
    }
    else
    {
        m_base_info.team_dungeon_systimes = 0;
    }
}

// ==================================================
// 添加组队副本次数
// ==================================================
void Player::AddTeamDungeonBuyTimes(int32 val)
{
    CnAssert(val > 0);
    if(val > 0)
    {
        m_base_info.team_dungeon_buytimes += val;
    }
}

// ==================================================
// 用魔石购买后处理购买组队次数
// ==================================================
void Player::DealTeamBuyTimes(int32 itemNum)
{
    IncTeamTeamBuyTimes();
    AddTeamDungeonBuyTimes(itemNum);

    SetPropDirty();
}
// ==================================================
// 删除组队副本次数
// ==================================================
void Player::SubTeamDungeonBuyTimes(int32 val)
{
    CnAssert(val > 0);
    if((val >= 0) && (m_base_info.team_dungeon_buytimes > val))
    {
        m_base_info.team_dungeon_buytimes -= val;
    }
    else
    {
        SetTeamDungeonBuyTimes(0);
    }
}

void Player::AddVoideFiedlTime(uint32 val)
{
    m_base_info.void_field_time = m_base_info.void_field_time + val;
}
// ==================================================
// 购买公会成员轮盘次数
// ==================================================
void Player::OnGuildMemberTurnTableBuyTimes()
{
    m_guild_welfare_info.turntable_item_list.clear();
    SendToGate(MSG_GUILD_WELFARE_INFO_ACK, &m_guild_welfare_info);
}

int32 Player::GetGrowthPoint() 
{
    return m_vip_manager->GetGrowthPointAll();
}

void Player::SetGrowthPoint(int32 val)
{
    return m_vip_manager->SetGrowthPointNoRecharge(val);
}

void Player::AddGrowthPoint(int32 val)
{
    return m_vip_manager->AddGrowthPointNoRecharge(val);
}

void Player::SubGrowthPoint(int32 val)
{
    return m_vip_manager->SubGrowthPointNoRecharge(val);
}

const VIP_INFO& Player::GetVipInfo() 
{
    return m_vip_manager->GetVipInfo();
}

void Player::SetVipInfo(const VIP_INFO& val) 
{
    m_vip_manager->SetVipInfo(val);
}

int32 Player::GetVipLevel()
{
    return m_vip_manager->GetVipLevel();
}

void Player::ActiveExperienceVIP()
{
    m_vip_manager->ActiveExperienceVIP();
}

bool Player::IsExperienceVIP()
{
    return m_vip_manager->IsExperienceVIP();
}

// ==================================================
// 检查进入活动
// ==================================================
uint16 Player::CheckEnterActivity(uint32 activityType)
{
    ActivityRegular* activityRegular = 0;
    uint16 errCode = ERR_SUCCEED;

    activityRegular = ActivityConfig::Instance()->GetActivityRegular(activityType);
    if(activityRegular)
    {
        if(GetLevel() < activityRegular->level)
        {
            errCode = ERR_NO_ENOUGH_PLAYER_LEVEL;
        }
    }
    else
    {
        errCode = ERR_FAILED;
    }

    if(errCode == ERR_SUCCEED)
    {
        if(!ActivityManager::Instance()->IsActivityTypeOpen(activityType))
        {
            errCode = ERR_ACTIVITY_NO_OPEN;
        }
    }

    return errCode;
}

// ==================================================
// 副本扫荡完成触发
// ==================================================
void Player::OnRaidsDungeonCmpl(uint32 dungeonID)
{
    std::map<uint32, int32> monsterList;

    DungeonTemplate* dungeonTmpl = DungeonManager::Instance()->GetDungeonTemplate(dungeonID);
    if(!dungeonTmpl)
    {
        CnError("Can't get dungeon:%u template\n", dungeonID);
        return;
    }

    dungeonTmpl->GetRaidsMonsterList(monsterList);

    for(std::map<uint32, int32>::iterator it = monsterList.begin();
        it != monsterList.end(); ++it)
    {
        SendTaskEvent(TASK_EVENT_RADIS_KILL_MONSTER, it->first, it->second, UNION_ID(0, dungeonTmpl->m_dungeon_id, 0));
    }

    OnTaskSettleDungeon(dungeonID, true, true);
}

// ==================================================
// 发送切换场景确认消息
// ==================================================
void Player::SendChangeSceneAck(uint16 errCode, uint32 newSceneUnionID, int32 x, int32 y, int32 angle)
{
    CHANGE_SCENE_ACK sendMsg;
    sendMsg.errcode = errCode;
    if(ERR_SUCCEED == sendMsg.errcode)
    {
        sendMsg.dungeon_id = newSceneUnionID;
        sendMsg.x = x;
        sendMsg.y = y;
        sendMsg.angle = angle;
        sendMsg.soldier_object_id = GetPetMgr()->GetSoldierObjectID();
    }
    else
    {
        sendMsg.dungeon_id = 0;
        sendMsg.x = 0;
        sendMsg.y = 0;
        sendMsg.angle = 0;
        sendMsg.soldier_object_id = 0;
    }
    SendToGate(MSG_CHANGE_SCENE_ACK, &sendMsg);
}

// ==================================================
// 处理触发器触发
// ==================================================
void Player::OnTriggerActivate(void* data)
{
    TRIGGER_ACTIVATE_REQ* recvMsg = (TRIGGER_ACTIVATE_REQ*)data;

    if(!recvMsg->trigger_id || !recvMsg->fun_code)
        return;

    Trigger* trigger = GetScene()->GetTriggerByID(recvMsg->trigger_id);
    if(!trigger)
    {
        CnDbg("player %s activate trigger %d not exist!\n", GetName().c_str(), recvMsg->trigger_id);
        return;
    }

    if(!trigger->IsEnable())
        return;

    // 优先判断trigger的跳转逻辑
    uint32 targetUnionID = 0;
    uint32 targetTriggerID = 0;
    if(DungeonMapManager::Instance()->AskTriggerJumpUnionID(this, recvMsg->trigger_id, targetUnionID, targetTriggerID))
    {   // 触发跳转副本trigger
        uint32 targetDungeonID = DUNGEON_ID(targetUnionID);
        uint32 targetSceneID = SCENE_ID(targetUnionID);
        uint32 srcSceneID = GetScene()->GetSceneID();

        if(targetDungeonID == GetScene()->GetDungeonID() || targetDungeonID == 0)
        {
            if(srcSceneID != targetSceneID)
            {   // 跳场景
                CHANGE_SCENE_REQ sendMsg;
                sendMsg.scene_id = SCENE_ID(targetUnionID);
                sendMsg.trigger_id = targetTriggerID;
                OnRecv(MSG_CHANGE_SCENE_REQ, &sendMsg);
            }
            else if(recvMsg->trigger_id != targetTriggerID)
            {   // 跳Trigger
                Trigger* pTargetTirgger = GetScene()->GetTriggerByID(targetTriggerID);
                if(pTargetTirgger)
                {
                    int32 newX = pTargetTirgger->GetX();
                    int32 newY = pTargetTirgger->GetY();
                    int32 newAngle = pTargetTirgger->GetAngle();
                    GetActionController()->JumpTo(newX, newY);
                }
                else if(targetTriggerID != 0)       // 目标triggerId=0表示，此trigger为到达点，不是出发点
                {
                    CnWarn("jump trigger %d no exist\n", targetTriggerID);
                }
            }
            else
            {
                // 源与目标完全一致
            }
        }
        else
        {   // 跳副本
            CHANGE_DUNGEON_REQ msg;
            msg.jump_type = DUNGEON_JUMP_TRIGGER;
            msg.dungeon_id = targetDungeonID;
            msg.scene_id = SCENE_ID(targetUnionID);
            msg.object_id = targetTriggerID;
            OnRecv(MSG_CHANGE_DUNGEON_REQ, &msg);
        }
    }
    else if (recvMsg->fun_code == TRIGGER_JUMP_DUNGEON)
    {
        uint32 dungeonID = m_task_mgr->GetDungeonIDByTrigger(recvMsg->trigger_id);      // 任务控制的副本跳转
        if(dungeonID)
        {
            TRIGGER_ACTIVATE_ACK sendMsg;
            sendMsg.dungeon_id = dungeonID;
            sendMsg.trigger_id = recvMsg->trigger_id;
            sendMsg.fun_code = recvMsg->fun_code;
            SendTriggerActivateAck(sendMsg);
        }
    }       
    else if (recvMsg->fun_code == TRIGGER_REGULAR_EVENT)
    {
        GetScene()->SendEvent(0, recvMsg->fun_code, recvMsg->trigger_id, 0, 0);
    }
    else
    {
        CnAssert(false);
        Scene* pScene = GetScene();
        CnWarn("no usage trigger dungeonId=%d sceneId=%d triggerId=%d\n", pScene->GetDungeonID(), pScene->GetSceneID(), trigger->GetTriggerId());
    }
}

// ==================================================
// 处理触摸器请求
// ==================================================
void Player::OnTouchRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_TOUCH_START_REQ:
        TOUCH_START_REQ* recvMsg = (TOUCH_START_REQ*)data;
        GetTouchController()->TouchStart(recvMsg->touch_id);
        break;
    }
}

// ==================================================
// 玩家复活请求
// ==================================================
void Player::OnResurgenceReq(void* data)
{
    GetScene()->SendEvent(0, EVENT_PLAYER_RESURGENCE_REQ, GetId(), 0, 0);
}

// ==================================================
// 成功进入地图
// ==================================================
void Player::OnEnterDungeon(Dungeon* dungeon, uint32 dungeonID)
{
    // 跳副本复活处理
    m_ResurgenceHelper->OnEnterDungeon(dungeon, dungeonID);

    // 上坐骑  由于进入地图会清空BUFF 所以应放其后面
    m_ride_mgr->ChangeRidePet(GetRidePetID());

    m_PetController->OnEnterDungeon(dungeon, dungeonID);

    //if(dungeon->GetDungeonID() == ASHURA_MAP)
    //{
    //    // 触发修罗场
    //    //SendTaskEvent(TASK_EVENT_ASHURA_TIMES, dungeonID, 0);

    //    // 活跃度
    //    SendMyselfEvent(LIVENESS_EVENT_TARGET_ASHURA, 1, 0, 0);
    //}

    SendTaskEvent(TASK_EVENT_ONLIE_TIME,  m_base_info.online_time, 0, 0);
}

// ==================================================
// 发送触发器应答
// ==================================================
void Player::SendTriggerActivateAck(TRIGGER_ACTIVATE_ACK& msg)
{
    SendToGate(MSG_TRIGGER_ACTIVATE_ACK, &msg);
}

// ==================================================
// 发送服务器时间同步
// ==================================================
void Player::SendServerTimeSync()
{
    SERVER_TIME sendMsg;
    sendMsg.time = (uint32)Crown::SDNow().GetTimeValue();
    SendToGate(MSG_SERVER_TIME_SYN_ACK, &sendMsg);
}

// ==================================================
// 发送跳副本失败消息
// ==================================================
void Player::SendChangeDungeonErrorAck(uint16 errCode)
{
    CHANGE_DUNGEON_ACK sendMsg;
    sendMsg.errcode = errCode;
    sendMsg.server_id = 0;
    sendMsg.gameserver_type = 0;
    sendMsg.player_object_id = 0;
    sendMsg.union_id = 0;
    sendMsg.x = 0;
    sendMsg.y = 0;
    sendMsg.angle = 0;
    sendMsg.now.time = (uint32)Crown::SDNow().GetTimeValue();
    sendMsg.language = ConfigManager::Instance()->GetLanguage();
    sendMsg.timezone = Crown::SDGetTimeZone();
    SendToGate(MSG_CHANGE_DUNGEON_ACK, &sendMsg);
}

// ==================================================
// 填充角色属性
// ==================================================
void Player::FillPropInfo(PROPERTY_INFO& info, uint32 unionID)
{
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    info.object_id = GetId();
    info.player_id.id_h = U64ID_HIGH(GetPlayerID());
    info.player_id.id_l = U64ID_LOW(GetPlayerID());
    info.account = GetAccount();
    info.name = GetName();
    info.sex = GetSex();
    info.career = GetCareer();
    info.group = GetGroup();
    info.level = GetLevel();
    info.exp = GetExp();
    info.max_exp = GetMaxExp();
    //info.sys_vigor = GetMaxVigor() - GetSysVigor();

    //if(info.sys_vigor < 0)
    //    info.sys_vigor = 0;

    //info.buy_vigor = GetBuyVigor();
    //info.max_vigor = GetMaxVigor();
    //info.vigor_buytimes = GetVigorBuyTimes();
    //info.team_buytimes = GetTeamBuyTimes();
    //info.team_dungeon_systimes = GetTeamDungeonSysTimes();
    //info.team_dungeon_buytimes = GetTeamDungeonBuyTimes();
    //info.stone_turn_gold_times = GetStoneTurnGoldTimes();
    //info.void_field_time = GetVoidFieldTime();
    //info.void_field_reftime = GetVoidFieldReftime();
    //// 体力刷新时间
    //if(GetVigorReftime() > now)
    //    info.vigor_reftime = GetVigorReftime() - now;
    //else
    //    info.vigor_reftime = 0;

    // 每日刷新时间
    if (GetDailyReftime() == 0)
    {
        time_t curTime = Crown::SDNow().GetTimeValue();
        m_base_info.daily_reftime = (uint32)curTime;
    }

    info.game_point = GetGamePoint();
    info.money_point = GetMoneyPoint();
    info.vigor       = GetVigorValue();
    //info.bind_money_point = GetBindMoneyPoint();
    //info.soul_point = GetSoulPoint();
    //info.star_point = GetStarPoint();
    //info.honor_point = GetHonorPoint();
    //info.guild_point = GetGuildPoint();
    //info.incarnation_point = GetIncarnationPoint();
    //info.battle_soul_point = GetBattleSoulPoint();
    //info.growth_point = GetGrowthPoint();

    info.hp = GetHP();
    info.mp = GetMP();
    info.max_hp = GetMaxHp();
    info.max_mp = GetMaxMp();

    //info.strength = GetStrength();
    //info.agility = GetAgility();
    //info.spirit = GetSpirit();
    //info.constitution = GetConstitution();

    //info.attack = GetAttack();
    //info.defence = GetArmor();
    //info.fire_defence = GetFireResistance();
    //info.ice_defence = GetFrostResistance();
    //info.thunder_defence = GetLightningResistance();
    //info.criticial_attack = GetCrit();
    //info.cri_att_red = GetCriticalAttackReduction();
    //info.cri_att_damage_add = GetCritDamage();
    //info.cri_att_damage_red = GetCriticalAttackDamageReduction();
    //info.hit_rate = GetHit();
    //info.dodge = GetDodge();
    //info.res_hp = GetHpRecovery();
    //info.res_mp = GetMpRecovery();

    //info.battle_power = GetBattlePower();

    info.guild_id_h = U64ID_HIGH(GetGuildID());
    info.guild_id_l = U64ID_LOW(GetGuildID());
    info.guild_name = GetGuildName();

    info.region = GetRegion();

    info.unionmap_id = unionID;
    info.x = GetX();
    info.y = GetY();
    info.angle = GetAngle();
    info.right_flag = GetRightFlag();

    //info.field_hook_x = GetFieldHookX();
    //info.field_hook_y = GetFieldHookY();
    info.online_time = GetOnLineTime();
    //info.getwelcome_reward = GetWelcomeReward();

    // 防沉迷信息
    //m_fcm_manager->FillInfo(info.fcm_info);

    // vip信息
    //m_vip_manager->FillInfo(info.vip_info);

    //info.vigor_info = m_vigor_mgr->GetInfo();
    //info.off_line_vigor_info = m_offline_vigor_mgr->GetInfo();
}

void Player::FillOffilePlayerInfo(OFFLINE_PLAYER_INFO& offline_info)
{
    m_PlayerSaver->FillOffilePlayerInfo(offline_info);
}

// ==================================================
// 重新加载节日任务
// ==================================================
void Player::ReloadTask()
{
    m_task_mgr->ReloadTask();

    if(m_task_mgr->IsStartCountOnlineTime())
    {
        if(GetLastExitTime() == 0)
            SetLastExitTime((uint32)(Crown::SDNow().GetTimeValue()));
    }
}

// ==================================================
// 获得天宫图的等级
// ==================================================
uint32 Player::GetHoroscopeLevel()
{
    return m_horoscope_mgr->GetHoroscopeLevel();
}

int32 Player::GetEquipAnimation() const
{
    return m_equip_mgr->GetEquipAnimation();
}

// ==================================================
// 通知装备信息
// ==================================================
void Player::NotifyEquipInfo(UPDATE_EQUIP_INFO_NTF& info)
{
    info.object_id = GetId();
    info.equip_info.wing_id = GetWingID();

    GetScene()->NotifyMessage(this, MSG_UPDATE_EQUIP_INFO_NTF, &info);

    m_PropNotifier->NotifyProperty(PROP_EQUIP_ANIMATION, GetEquipMgr()->GetEquipAnimation());
}

void Player::NotifyTitleInfo(PLAYER_TITLE_LIST& info)
{
    PLAYER_TITLE_LIST msg;
    msg.player_object_id = GetId();
    msg.list = info.list;
    if(GetScene())
    {
        GetScene()->NotifyMessage(this, MSG_PLAYER_TITLE_NTF, &msg);
    }
}
// ==================================================
// 更改玩家名称
// ==================================================
void Player::SendChangePlayerNameAck(uint16 errCode, const std::string& name)
{
    CHANGE_PLAYER_NAME_ACK sendMsg;
    sendMsg.errcode = errCode;
    sendMsg.name = name;
    sendMsg.object_id = GetId();
    SendToGate(MSG_CHANGE_PLAYER_NAME_ACK, &sendMsg);
}

// ==================================================
// 通知属性值
// ==================================================
void Player::NotifyProperty(uint32 property_id, int32 value, const std::string& name)
{
    m_PropNotifier->NotifyProperty(property_id, value, name);
}
// ==================================================
// 通知错误码
// ==================================================
void Player::NotifyErrorCode(uint16 errCode)
{
    ERROR_CODE sendMsg;
    sendMsg.errcode = errCode;
    SendToGate(MSG_ERROR_CODE, &sendMsg);
}
// ==================================================
// 发送到客户端聊天信息
// ==================================================
void Player::LogChat(const std::string& log, uint32 channel)
{
    CHAT_MSG sendMsg;
    sendMsg.channel = channel;
    sendMsg.chat = log;
    SendToGate(MSG_CHAT_ACK, &sendMsg);
}

bool Player::UseSkill(uint32 skill_id, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skill_obj_id, uint32 skill_slot)
{
    bool ret = Creature::UseSkill(skill_id, target, args, skill_obj_id, skill_slot);

    if(!ret && args.m_SkillMode == SKILL_MODE_SELF)
    {
        // 使用技能失败通知
        USE_SKILL_NTF skillAck;
        skillAck.skill.skill_id = skill_id;
        skillAck.skill.skill_obj_id = skill_obj_id;
        skillAck.skill.skill_slot = skill_slot;
        skillAck.skill.target = target;
        skillAck.skill.skill_args = args.toNetArgument();

        skillAck.error = ERR_FAILED;
        skillAck.owner_id = GetId();
        skillAck.skill_cd = 0;
        SendToGate(MSG_USE_SKILL_NTF, &skillAck);
        return false;
    }
    else
    {
        m_battle_state_manager->OnUseSkill(skill_id);
    }

    return ret;
}

void Player::MakeSkillCD(uint32 skill_id, int32 cdtime)
{
    int32 ret_cdtime = m_SkillController->MakeSkillCD(skill_id, cdtime);
    if(ret_cdtime >= 0 && GetNetState() == Player::kStateInGame)
    {
        // 技能进入CD通知
        USE_SKILL_NTF sendMsg;
        sendMsg.skill.skill_id = skill_id;
        sendMsg.owner_id = GetId();

        // 使用技能成功
        sendMsg.error = ERR_SUCCEED;
        sendMsg.skill_cd = ret_cdtime;
        SendToGate(MSG_MAKE_SKILL_CD_NTF, &sendMsg);
    }
}

void Player::ClearAllSkillCD()
{
    const std::map<uint32, int32>& skillCDMap = m_SkillController->GetSkillCDMap();
    for(std::map<uint32, int32>::const_iterator iter = skillCDMap.begin(); iter != skillCDMap.end(); iter++)
    {
        uint32 skillID = iter->first;
        MakeSkillCD(skillID, 0);
    }
}

// ==================================================
// 发送充值任务事件
// ==================================================
void Player::SendRechargeEvent()
{
    SendTaskEvent(TASK_EVENT_RECHARGE_TOTAL, 0, 0, 0);      // 充值累计
    SendTaskEvent(TASK_EVENT_RECHARGE_SINGLE, 0, 0, 0);     // 每晶充值单笔最大值
    SendTaskEvent(TASK_EVENT_RECHARGE_DAILY, 0, 0, 0);      // 充值每日
    SendTaskEvent(TASK_EVENT_RECHARGE_OPENSVR, 0, 0, 0);    // 充值累计（开服）
    SendTaskEvent(TASK_EVENT_CUMULATIVE_RECHARGE, 0, 0, 0); // 累计充值(循环)

    // 充值发生改变，成长值发生改变
    m_vip_manager->CalGrowPoint();

    // VIP独享副本次数计算，保证VIP等级正确
    m_vip_only_dungeon->CalGamePlayTimes();

    // 累积充值30元人民币后，自动激活30天投资理财 每30后，再次充值30人民币，自动激活投资理财
    m_day_investment_mgr->IsActivateDayInvestment();
}

void Player::LoadRechargeInfo(const PLAYER_RECHARGE_INFO& info)
{
    m_PlayerLoader->LoadRechargeInfo(info);
}

void Player::SendBehaviorLog(int32 type, int32 num, int32 reason)
{
    BEHAVIOR_LOG_REQ msg;
    char str[100];

    SDSnprintf(str, 100, "%d\t%d\t%d", type, num, reason);
    msg.log = str;
    CenterHandler::Instance()->SendByPlayer(this, MSG_BEHAVIOR_LOG_REQ, &msg, GetPlayerID());
}

// ==================================================
// 发送添加队友应答
// ==================================================
void Player::SendTeamMemberAddAck(uint16 errcode, const std::string& src_name, const std::string& dst_name)
{
  /*  TEAM_MEMBER_ADD_ACK sendMsg;
    sendMsg.errcode = errcode;
    sendMsg.src_name = src_name;
    sendMsg.dst_name = dst_name;
    SendToGate(MSG_TEAM_MEMBER_ADD_ACK, &sendMsg);*/
}

int32 Player::GetDaysBetweenDates(time_t t1, time_t t2)
{
    Crown::CSDDateTime localDay1 = Crown::CSDDateTime(t1);
    Crown::CSDDateTime localDay2 = Crown::CSDDateTime(t2);

    if(t1 == 0) 
        localDay1.SetDate(1970, 1, 2);
    if(t2 == 0) 
        localDay2.SetDate(1970, 1, 2);

    int32 localTimeZone = Crown::SDGetTimeZone();       // 本服时区
    int32 playerTimeZone = GetTimeZone();               // 玩家的时区

    int32 diffTimeZone = playerTimeZone - localTimeZone;
    if(diffTimeZone > 0)
    {
        localDay1.IncHour(diffTimeZone);
        localDay2.IncHour(diffTimeZone);
    }
    else if(diffTimeZone < 0)
    {
        localDay1.DecHour(-diffTimeZone);
        localDay2.DecHour(-diffTimeZone);
    }

    localDay1.SetTime(0, 0, 0);
    localDay2.SetTime(0, 0, 0);

    int64 diffDay = localDay1.DiffDayNoDst(localDay2);      // 去掉时分秒的比较，必须忽略夏令时否则进入夏令时后会少一天(除法取整)

    return int32(diffDay);
}

void Player::DealTradeOrder(int32 reason, const std::string& str)
{
    bool islog = false;
    switch(reason)
    {
    case REWARD_REASON_RIDE_ABSORB:
        {
            m_ride_mgr->RideAbsorb(str);
        }
        break;
    case REWARD_REASON_VIP_BUY:
        {
            CBVipTrade(str);
        }
        break;
    case REWARD_REASON_PROTECT_SOUL_NUM:
        {
            GetSoulDungeonManager()->AddProtectSoulNum(str);
        }
        break;
    case REWARD_REASON_GAIN_BACK_RESOURCE:
        {
            //GetGainbackResourceManager()->OnMoneyGainBack(str);
        }
        break;
    case REWARD_REASON_GAIN_BACK_RESOURCE_ALL:
        {
            //GetGainbackResourceManager()->OnMoneyGainBackAll(str);
        }
        break;
    case REWARD_REASON_JUMPING_OFF_USE_SKILL:
        {
            
        }
        break;
        //case REWARD_REASON_GUILD_ADD_PROMOTE:
        //    {
        //        GetGuildScienceManager()->CBGuildAdditionPromoteTrade(str);
        //    }
    case REWARD_REASON_GUILD_MEMBER_TURNTABLE:
        {
            uint32 id = 0;
            read_PERSONAL_TURNTABLE_ID(id, str);
            GuildManager::Instance()->OnGuilddMemberTurntableAck(this, id);
        }
        break;
    case REWARD_REASON_PET_FORMATION_LOCK:
        {
            //GetPetFormation()->onMoneyPointAck(str);
        }
        break;
    case REWARD_DONATE:
        {
            GetDonateManager()->onMoneyPointAck(str);
            break;
        }
    case REWARD_REASON_INVESTMENT_PLAN:
        {
            GetInvestmentPlan()->CBInvestment(str);
        }
        break;
    case REWARD_REASON_INVESTMENT_PLAN_ADDITIONAL:
        {
            GetInvestmentPlan()->CBAdditional(str);
        }
        break;
    case REWARD_REASON_BUY_CHEAP:
        {
            GetBuyCheapMgr()->CBBuyCheapBuy(str);
        }
        break;
    }
    SendtoCenterLog(LOG_SERVICE, WriterToLogByService(reason, 1));
}

void Player::LoadVigorInfo(const VIGOR_INFO& vigor_info)
{
    m_vigor_mgr->LoadInfo(vigor_info);
}

void Player::FillVigorInfo(VIGOR_INFO& vigor_info)
{
    m_vigor_mgr->FillInfo(vigor_info);
}

void Player::AskAutoSave()
{
    m_auto_save_time = ServerConfig::Instance()->GetAutoSavePeriod() * 1000;
}

// 获得指定城市奖励时间戳
uint32 Player::GetCityFightRewardTimeStamp(uint32 cityId)
{    
    for (uint32 i = 0; i < m_city_fight_info.city_list.size(); ++i)
    {
        if (m_city_fight_info.city_list[i].city_id == cityId)
        {
            return m_city_fight_info.city_list[i].timestamp;
        }
    }

    return 0;
}

// 设置指定城市奖励时间戳
void Player::SetCityFightRewardTimeStamp(uint32 cityId, uint32 timeStamp)
{
    for (uint32 i = 0; i < m_city_fight_info.city_list.size(); ++i)
    {
        if (m_city_fight_info.city_list[i].city_id == cityId)
        {
            // 设置新的奖励时间
            m_city_fight_info.city_list[i].timestamp = timeStamp;

            // 设置该城市信息已在玩家信息中
            return;
        }
    }

    // 未在玩家信息里的,需要添加进去
    CITY_FIGHT_ONE_CITY_INFO info;
    info.city_id = cityId;
    info.timestamp = timeStamp;
    m_city_fight_info.city_list.push_back(info);
}

int32 Player::GetRechargeCount()
{
    if(m_recharge_mgr)
        return m_recharge_mgr->GetRechargeCount();
    else
        return 0;
}

void Player::SetTotalRecharge(int32 setmoney)
{
    m_recharge_mgr->SetRechargeCount(setmoney);
    SendRechargeEvent();
}

void Player::GmtUpdate( uint32 type )
{
    switch (type)
    {
    case GAME_SETTING_TYPE_MULTISERVER_ENTER_NUM:
        {
            m_multiserver_battle_info->UpdateMultiserverBattleInfo();
        }
        break;
    case GAME_SETTING_TYPE_MULTISERVER_YESTERDAY_NUM:
    case GAME_SETTING_TYPE_MULTISERVER_YESTERDAY_3V3_NUM:
        {
            m_multiserver_battle_info->UpdateMultiserverBattleInfo();
        }
        break;
    }
}

PetList* Player::GetPetList()
{
    return &GetPetMgr()->GetPetList();
}

//uint32 Player::GetDefaultSkill()
//{
//    uint32 career = GetCareer();
//    if(career == CAREER_WARRIOR)
//        return WARRIOR_DEFAULT_SKILLID;
//    else if(career == CAREER_MAGE)
//        return MAGE_DEFAULT_SKILLID;
//    else
//        return PRIEST_DEFAULT_SKILLID;
//}

