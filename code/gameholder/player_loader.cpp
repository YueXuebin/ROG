#include "gameholder_pch.h"
#include "player_loader.h"
#include "player.h"
#include "fcm_manager.h"
#include "vip_manager.h"
#include "vigor_manager.h"
#include "offline_vigor_manager.h"
#include "player_dungeon_manager.h"
#include "horoscope_manager.h"
#include "skill_info_manager.h"
#include "equip_manager.h"
#include "fashion_manager.h"
#include "sword_manager.h"
#include "ride_manager.h"
#include "item_manager.h"
#include "package_manager.h"
#include "friend_manager.h"
#include "player_door_manager.h"
#include "babel_manager.h"
#include "pay_point_lottery.h"
#include "vip_only_dungeon_manager.h"
#include "mail_manager.h"
#include "sign_manager.h"
#include "online_reward_manager.h"
#include "treasury_manager.h"
#include "goldtree_manager.h"
#include "adventure_manager.h"
#include "answer_manager.h"
#include "wing_manager.h"
#include "ashura_recorder.h"
#include "recharge_manager.h"
#include "shop_manager.h"
#include "passive_skill_manager.h"
#include "title_manager.h"
#include "day_investment_manager.h"
#include "level_investment_manager.h"
#include "level_activity_manager.h"
#include "setting_manager.h"
#include "liveness_manager.h"
#include "dungeon_times_manager.h"
#include "limitbuy_manager.h"
#include "single_answer_manager.h"
#include "bh_manager.h"
#include "task_manager.h"
#include "guide_manager.h"
#include "enemy_manager.h"
#include "zombie_manager.h"
#include "guild_science_recorder.h"
#include "shorcut_manager.h"
#include "multiserver_battle_info_manager.h"
#include "donate_manager.h"
#include "investment_plan_manager.h"
#include "skill_controller.h"
#include "buff_controller.h"
#include "state_controller.h"
#include "egg_manager.h"
#include "buy_cheap_manager.h"
#include "player_config_manager.h"
#include "pet_manager.h"
#include "head_portrait_manager.h"
#include "bag_manager.h"
#include "potion_manager.h"
#include "open_system_manager.h"
#include "store_manager.h"
#include "random_package_manager.h"


PlayerLoader::PlayerLoader(Player* player) :
m_player(player)
{

}

PlayerLoader::~PlayerLoader()
{

}

void PlayerLoader::Load(uint64 playerID, DELIVER_PLAYER_INFO_REQ* recvMsg)
{
    // 计算在角色数据
    LoadBaseInfo(playerID, recvMsg->prop_info);

    // 加载角色状态信息
    LoadStateInfo(recvMsg->state_info);

    // 设置角色的副本信息
    LoadPlayerDungeonInfo(recvMsg->player_dungeon_info);

    //// 设置开启系统信息
    //SetOpenSystemInfo(recvMsg->open_system_info);

    //// 设置公会福利信息
    //SetGuildWelfareInfo(recvMsg->prop_info.guild_welfare_info);

    //// 设置城市争夺战信息
    //SetCityFightInfo(recvMsg->prop_info.city_fight_info);

    //// 加载星宫图数据
    //LoadHoroscopeInfo(recvMsg->horoscope_info);

    // 加载技能栏数据
    LoadSkillComponentInfo(recvMsg->skill_component_info);

	//// 加载被动技能信息
	LoadPassiveSkillInfo(recvMsg->passive_skill_info);

    // 加载角色装备数据
    LoadEquipInfo(recvMsg->equip_info);

    // 加载角色药剂信息
    LoadPotionInfo(recvMsg->potion_info);

    //// 加载时装数据
    //LoadFashionInfo(recvMsg->fashion_info);

    //// 加载圣剑信息
    //LoadSwordInfo(recvMsg->sword_info);

    //// 加载宠物阵型信息
    //LoadPetFormation(recvMsg->pet_formation_info);

    //加载宠物数据
    LoadPetInfo(recvMsg->pet_info);

    ////加载骑宠数据
    //LoadRidePetInfo(recvMsg->ride_info);

    // 加载物品数据
    LoadItemInfo(recvMsg->item_info);
    // 加载仓库数据
    LoadStoreInfo(recvMsg->store_info);

    // 加载抽奖包数据
    LoadRandomPackageInfo(recvMsg->random_package_info);

    //// 加载隐藏物品数据
    //LoadHideBagInfo(recvMsg->hide_bag_info);

    // 加载好友数据
    LoadFriendInfo(recvMsg->friend_info);

    //// 加载怪物之门数据
    //LoadMonsterDoorInfo(recvMsg->monster_door_info);

    //// 加载勇者争霸数据
    //LoadPlayerDoorInfo(recvMsg->player_door_info);

    //// 加载奖励数据
    //LoadRewardInfo(recvMsg->reward_info);

    //// 加载通天塔数据
    //LoadBabelInfo(recvMsg->babel_info);

    //// 加载充值积分数据
    //LoadPayPointLotteryInfo(recvMsg->pay_point_lottery_info);

    //// 加载VIP独享副本数据
    //LoadVIPOnlyDungeonInfo(recvMsg->vip_only_dungeon_info);

    //// 加载赛马数据
    //LoadRacingInfo(recvMsg->racing_info);

    //// 加载签到数据
    LoadSignInfo(recvMsg->sign_info);

    //// 加载在线奖励数据
    //LoadOnlineRewardInfo(recvMsg->online_reward_info);

    //// 加载采矿到数据
    //LoadMiningInfo(recvMsg->mining_info);

    //// 加载宝库到数据
    //LoadTreasuryInfo(recvMsg->treasury_info);

    //// 加载金像到数据
    //LoadGoldtreeInfo(recvMsg->goldtree_info);

    //// 加载大冒险数据
    //LoadAdventureInfo(recvMsg->adventure_info);

    //// 加载答题数据
    //LoadAnswerInfo(recvMsg->answer_info);

    //// 加载翅膀数据
    LoadWingInfo(recvMsg->wing_info);

    //// 加载修罗城数据
    //LoadAshuraInfo(recvMsg->ashura_info);

    //// 加载充值信息
    //LoadRechargeInfo(recvMsg->recharge_info);

    //// 加载商城信息
    LoadMallInfo(recvMsg->restriction_info);

    //// 加载邮件数据
    LoadMailSystemInfo(recvMsg->mail_system_info);

    //// 加载称号信息
    //LoadTitleInfo(recvMsg->title_info);

    //// 加载日常投资信息
    //LoadDayInvestmentInfo(recvMsg->day_investment_info);

    //// 加载等级投资信息
    //LoadLevelInvestmentInfo(recvMsg->level_investment_info);

    //// 加载等级活动信息
    //LoadLevelActivityInfo(recvMsg->level_activity_info);

    //// 加载自动战斗设置信息
    //LoadAutoFightSettingInfo(recvMsg->auto_fight_setting_info);

    //// 加载系统设置信息
    //LoadSystemSettingInfo(recvMsg->system_setting_info);

    //// 加载玩家设置信息
    //LoadPlayerSettingInfo(recvMsg->player_setting_info);

    //// 加载自动购买设置信息
    //LoadAutobuySettingInfo(recvMsg->autobuy_setting_info);

    //// 加载虚空幻境挂机设置信息
    //LoadVoidFieldSettingInfo(recvMsg->void_field_setting_info);

    //// 加载活跃度信息
    //LoadLivenessInfo(recvMsg->liveness_info);

    //// 加载守卫地宫信息
    //LoadDungeonTimesInfo(recvMsg->dungeon_times_list);

    //// 加载限购
    //LoadLimitBuyInfo(recvMsg->limit_buy_list);

    //// 加载单人答题
    //LoadSingleAnswerInfo(recvMsg->single_answer);

    //// 加载资源挽回信息
    //LoadGainbackResourceInfo(recvMsg->gainback_resource_info);

    //// 加载勇敢的心信息
    //LoadBhInfo(recvMsg->bh_info);

    // 加载任务数据
    // # 任务的加载不能放太靠前，因为加载任务时可能会添加可接任务 #
    // # 新添加的任务需要依赖其他模块数据判断是否任务已完成       #
    LoadTaskInfo(recvMsg->task_info);

    // 加载引导信息
    LoadGuideInfo(recvMsg->guide_info);

    // 加载功能开启信息
    LoadOpenSystemInfo(recvMsg->open_system_info);

    //// 加载善恶信息
    //LoadEnemyInfo(recvMsg->enemy_info);

    //// 加载僵尸生存信息
    //loadZombieInfo(recvMsg->zombie_info);

    //// 加载公会科技信息
    //LoadGuildScienceInfo(recvMsg->guild_science_info);

    //// 收藏游戏状态
    //LoadShorcutInfo(recvMsg->shorcut_info);

    //// 加载跨服战斗信息
    //LoadMultiserverBattleInfo(recvMsg->multiserver_battle_info);

    //// 加载投资计划信息
    //LoadInvestmentPlan(recvMsg->investment_plan_info);

    //// 加载捐献
    //LoadDonateInfo(recvMsg->donate_info);
    //
    //// 加载捐献
    //LoadEggInfo(recvMsg->egg_info);

    //// 越卖越便宜
    //LoadBuyCheapInfo(recvMsg->buy_cheap);

    // 加载个性头像数据
    LoadHeadPortraitInfo(recvMsg->head_portrait);
}

// ==================================================
// 加载角色的基本信息
// ==================================================
void PlayerLoader::LoadBaseInfo(uint64 playerID, PROP_BASE_INFO& info)
{
    uint64 teamID = 0;
    uint64 guildID = 0;

    m_player->SetGateSvrID(info.gatesvr_id);
    m_player->SetCenterSvrID(info.centersvr_id);
    m_player->SetLoginID(U64ID(info.login_id_h, info.login_id_l));
    m_player->SetPlayerID(playerID);
    m_player->SetCreatureId(1);
    m_player->SetAngle(info.angle);
    m_player->SetAccount(info.account);
    m_player->SetName(info.name);
    m_player->m_base_info.platform_server_name = info.platform_server_name;
    m_player->SetRegion(info.region);
    m_player->SetTimeZone(info.timezone);
    m_player->SetSex(info.sex);
    m_player->SetCareer(info.career);
    m_player->SetLevel(info.level);       // 必须先设置等级，再设置经验
    m_player->SetExp(info.exp);
    m_player->SetGamePoint(info.game_point);
    m_player->SetMoneyPoint(info.money_point);
    m_player->SetVigorValue(info.vigor);  // 设置体力值
    m_player->SetBindMoneyPoint(info.bind_money_point);
    m_player->SetSoulPoint(info.soul_point);
    m_player->SetStarPoint(info.star_point);
    m_player->SetHonorPoint(info.honor_point);
    m_player->SetGuildPoint(info.guild_point);
    m_player->SetIncarnationPoint(info.incarnation_point);
    m_player->SetBattleSoulPoint(info.battle_soul_point);
    m_player->SetGrowthPointSpendplus(info.spend_plus);
    m_player->SetHP(info.hp);               // 角色当前hp
    m_player->SetMP(info.mp);               // 角色当前mp
    m_player->SetVipInfo(info.vip_info);
    m_player->SetSysVigor(info.sys_vigor);
    m_player->SetBuyVigor(info.buy_vigor);
    m_player->SetVigorReftime(info.vigor_reftime);
    m_player->SetVigorBuyTimes(info.vigor_buytimes);
    m_player->SetTeamBuyTimes(info.team_buytimes);
    m_player->SetTeamDungeonSysTimes(info.team_dungeon_systimes);
    m_player->SetTeamDungeonBuyTimes(info.team_dungeon_buytimes);
    m_player->SetStoneTurnGoldTimes(info.stone_turn_gold_times);
    m_player->SetVoideFiedlTime(info.void_field_time);
    m_player->SetVoidFieldReftime(info.void_field_reftime);

    m_player->SetDailyReftime(info.daily_reftime);
    //LoadLevelProp(info.level);      // 加载角色的等级属性

    guildID = U64ID(info.guild_id_h, info.guild_id_l);
    m_player->SetGuildID(guildID);
    m_player->SetGuildJoinTime(info.guild_join_time);
    //SetGuildTrunTablePos(info.turntable_pos);
    //SetGuildMemberTrunTablePos(info.member_turn_pos);


    m_player->SetUnionMapID(info.unionmap_id);
    m_player->SetX(info.x);
    m_player->SetY(info.y);

    m_player->SetCityMapID(info.city_id);
    m_player->SetCityX(info.city_x);
    m_player->SetCityY(info.city_y);

    m_player->SetOnLineTime(info.online_time);

    m_player->m_base_info.getwelcome_reward = info.getwelcome_reward;

    m_player->SetAngle(info.angle);

    teamID = U64ID(info.team_id_h, info.team_id_l);
    m_player->SetTeamID(teamID);

    m_player->SetRightFlag(info.right_flag);

    m_player->SetLoginIP(info.login_ip);

    m_player->SetChangeDungeonCount(info.change_dungeon_count);

    m_player->m_fcm_manager->LoadInfo(info.fcm_info);

    m_player->m_vip_manager->LoadInfo(info.vip_info);

    m_player->m_vigor_mgr->LoadInfo(info.vigor_info);

    m_player->m_offline_vigor_mgr->LoadInfo(info.off_line_vigor_info);

    // 隐藏属性
    if(info.career == CAREER_WARRIOR)   // 战士吸血
    {
        //m_player->SetVampirePro(PROP_UNIT_BASE, GameParamConfig::Instance()->GetWarriorVampirePro());
        //m_player->SetVampire(PROP_UNIT_BASE, GameParamConfig::Instance()->GetWarriorVampire());
    }
    else if(info.career == CAREER_MAGE)
    {

    }
    else if(info.career == CAREER_ARCHER)
    {

    }
    else
        CnAssert(false);

}

void PlayerLoader::LoadStateInfo(const PLAYER_STATE_INFO& info)
{
    m_player->GetSkillController()->LoadSkillState(info.player_state);
    m_player->GetBuffController()->LoadBuffState(info.player_state);
    m_player->GetStateController()->LoadStateInfo(info.player_state);
}

// ==================================================
// 加载角色的副本信息
// ==================================================
void PlayerLoader::LoadPlayerDungeonInfo(const PLAYER_DUNGEON_INFO& info)
{
    m_player->m_player_dungeon_mgr->LoadPlayerDungeonInfo(info);
}

void PlayerLoader::SetOpenSystemInfo(OPEN_SYSTEM_INFO& val) 
{ 
    m_player->m_open_sysetm_info = val; 
}

void PlayerLoader::SetGuildWelfareInfo(GUILD_WELFARE_INFO& val)
{
    m_player->m_guild_welfare_info = val; 
}

void PlayerLoader::SetCityFightInfo(CITY_FIGHT_PLAYER_INFO& val) 
{ 
    m_player->m_city_fight_info = val; 
}

// ==================================================
// 加载星宫图信息
// ==================================================
void PlayerLoader::LoadHoroscopeInfo(HOROSCOPE_INFO& info)
{
    m_player->m_horoscope_mgr->LoadInfo(info);
}

// ==================================================
// 加载技能信息
// ==================================================
void PlayerLoader::LoadSkillComponentInfo(SKILL_COMPONENT_INFO& info)
{
    m_player->m_skill_info_mgr->LoadSkillComponentInfo(info);
}
// ==================================================
// 加载装备信息
// ==================================================
void PlayerLoader::LoadEquipInfo(EQUIP_INFO& list)
{
    m_player->m_equip_mgr->LoadEquipInfo(list);
}

void PlayerLoader::LoadPotionInfo(POTION_INFO& info)
{
    m_player->m_potion_use_mgr->LoadPotionInfo(info);
}

void PlayerLoader::LoadFashionInfo( FASHION_BAG& list )
{
    m_player->m_fashion_mgr->LoadFashionInfo(list);
}

void PlayerLoader::LoadSwordInfo( SWORD_NEW_INFO& list )
{
    m_player->m_sword_mgr->LoadInfo(list);
}

void PlayerLoader::LoadPetFormation(PET_FORMATION_INFO& info)
{
    //m_player->m_pet_formation_mgr->LoadInfo(info);
}

// ==================================================
// 加载跟宠物信息
// ==================================================
void PlayerLoader::LoadPetInfo(PET_INFO& info)
{
    m_player->m_pet_mgr->LoadPetInfo(info);
}

// ==================================================
// 加载跟宠物信息
// ==================================================
void PlayerLoader::LoadRidePetInfo(RIDE_INFO& list)
{
    m_player->m_ride_mgr->LoadRideInfo(list);
}

// ==================================================
// 加载物品信息
// ==================================================
void PlayerLoader::LoadItemInfo(const ITEM_INFO& info)
{
    m_player->m_bag_mgr->LoadBagInfo(info);
}
// ==================================================
// 加载物品信息
// ==================================================
void PlayerLoader::LoadStoreInfo(const ITEM_INFO& info)
{
    m_player->m_store_mgr->LoadstoreInfo(info);
}


// ==================================================
// 加载抽奖信息
// ==================================================
void PlayerLoader::LoadRandomPackageInfo(const RANDOM_PACKAGE_RECORD_INFO& info)
{
    m_player->m_random_package_mgr->LoadInfo(info);
}

// ==================================================
// 加载隐藏物品列表
// ==================================================
//void PlayerLoader::LoadHideBagInfo(const HIDE_BAG_INFO& info)
//{
//     m_player->m_item_mgr->LoadHideBagInfo(info);
//}

// ==================================================
// 加载好友信息
// ==================================================
void PlayerLoader::LoadFriendInfo(FRIEND_LIST& info)
{
    m_player->m_friend_mgr->LoadInfo(info);
}

// ==================================================
// 加载怪物之门数据
// ==================================================
void PlayerLoader::LoadMonsterDoorInfo(MONSTER_DOOR_INFO& info)
{

}

// ==================================================
// 加载勇者争霸数据
// ==================================================
void PlayerLoader::LoadPlayerDoorInfo(PLAYER_DOOR_INFO& info)
{
    m_player->m_player_door_mgr->LoadInfo(info);
}

// ==================================================
// 加载奖励信息
// ==================================================
void PlayerLoader::LoadRewardInfo(REWARD_INFO& info)
{
    m_player->m_reward_info = info;
}

// ==================================================
// 加载通天塔信息
// ==================================================
void PlayerLoader::LoadBabelInfo(BABEL_INFO& info)
{
    //m_player->m_babel_mgr->LoadBabelInfo(info);
}

// ==================================================
// 加载充值积分抽奖信息
// ==================================================
void PlayerLoader::LoadPayPointLotteryInfo(PAY_POINT_LOTTERY_INFO &info)
{
    m_player->m_pay_point_lottery->LoadPayPointLotteryInfo(info);
}

// ==================================================
// 加载VIP独享副本信息
// ==================================================
void PlayerLoader::LoadVIPOnlyDungeonInfo(VIP_ONLY_DUNGEON_INFO &info)
{
    m_player->m_vip_only_dungeon->LoadVIPOnlyDungeonInfo(info);
}

// ==================================================
// 加载邮件系统信息
// ==================================================
void PlayerLoader::LoadMailSystemInfo(const MAIL_SYSTEM_INFO& info)
{
    m_player->m_mail_mgr->LoadInfo(info);
}

// ==================================================
// 加载赛马信息
// ==================================================
void PlayerLoader::LoadRacingInfo(RACING_INFO& info)
{
    //m_racing_mgr->LoadRacingInfo(info);
}

// ==================================================
// 加载签到信息
// ==================================================
void PlayerLoader::LoadSignInfo(SIGN_INFO& info)
{
    m_player->m_sign_mgr->LoadInfo(info);
}

// ==================================================
// 加载在线奖励信息
// ==================================================
void PlayerLoader::LoadOnlineRewardInfo(ONLINE_REWARD_INFO& info)
{
    m_player->m_online_reward_mgr->LoadInfo(info);
}

// ==================================================
// 加载采矿信息
// ==================================================
void PlayerLoader::LoadMiningInfo(MINING_INFO& info)
{
    //m_mining_mgr->LoadMiningInfo(info);
}

// ==================================================
// 加载宝库信息
// ==================================================
void PlayerLoader::LoadTreasuryInfo(TREASURY_INFO& info)
{
    m_player->m_treasury_mgr->LoadTreasuryInfo(info);
}

// ==================================================
// 加载金像信息
// ==================================================
void PlayerLoader::LoadGoldtreeInfo(GOLDTREE_INFO& info)
{
    m_player->m_goldtree_mgr->LoadGoldtreeInfo(info);
}

// ==================================================
// 加载大冒险数据
// ==================================================
void PlayerLoader::LoadAdventureInfo(ADVENTURE_INFO& info)
{
    m_player->m_adventure_mgr->LoadAdventureInfo(info);
}

// ==================================================
// 加载答题数据
// ==================================================
void PlayerLoader::LoadAnswerInfo(const ANSWER_INFO& info)
{
    m_player->m_answer_mgr->LoadAnswerInfo(info);
}

// ==================================================
// 加载翅膀信息
// ==================================================
void PlayerLoader::LoadWingInfo(WING_INFO& info)
{
    m_player->m_wing_mgr->LoadInfo(info);
}

// ==================================================
// 加载修罗场信息
// ==================================================
void PlayerLoader::LoadAshuraInfo(ASHURA_INFO& info)
{
    m_player->m_ashura_recorder->LoadAshuraInfo(info);
}

// ==================================================
// 加载充值信息
// ==================================================
void PlayerLoader::LoadRechargeInfo(const PLAYER_RECHARGE_INFO& info)
{
    m_player->m_recharge_mgr->LoadRechargeInfo(info);
}

// ==================================================
// 加载商城数据
// ==================================================
void PlayerLoader::LoadMallInfo(const RESTRICTION_LIST& info)
{
    m_player->m_shop_mgr->LoadMallInfo(info);
}

// ==================================================
// 加载天赋
// ==================================================
void PlayerLoader::LoadPassiveSkillInfo(const PASSIVE_SKILL_LIST& info)
{
    m_player->m_passive_skill_mgr->LoadPassiveSkillInfo(info);
}

// ==================================================
// 加载称号信息
// ==================================================
void PlayerLoader::LoadTitleInfo(TITLE_LIST& info)
{
    m_player->m_title_mgr->LoadTitleInfo(info);
}

// ==================================================
// 加载日常投资信息
// ==================================================
void PlayerLoader::LoadDayInvestmentInfo( DAY_INVESTMENT_INFO& day_investment_info )
{
    m_player->m_day_investment_mgr->LoadDayInvestmentInfo(day_investment_info);
}

// ==================================================
// 加载等级投资信息
// ==================================================
void PlayerLoader::LoadLevelInvestmentInfo( LEVEL_INVESTMENT_INFO& day_investment_info )
{
    m_player->m_level_investment_mgr->LoadLevelInvestmentInfo(day_investment_info);
}

void PlayerLoader::LoadLevelActivityInfo( LEVEL_ACTIVITY_INFO& level_activity_info )
{
    m_player->m_level_activity_mgr->LoadLevelActivityInfo(level_activity_info);
}

// ==================================================
// 加载自动战斗设置信息
// ==================================================
void PlayerLoader::LoadAutoFightSettingInfo(AUTO_FIGHT_SETTING_INFO& auto_fight_setting_info)
{
    m_player->m_setting_mgr->LoadAutoFightSettingInfo(auto_fight_setting_info);
}

// ==================================================
// 加载系统设置信息
// ==================================================
void PlayerLoader::LoadSystemSettingInfo(SYSTEM_SETTING_INFO& system_setting_info)
{
    m_player->m_setting_mgr->LoadSystemSettingInfo(system_setting_info);
}

// ==================================================
// 加载玩家设置信息
// ==================================================
void PlayerLoader::LoadPlayerSettingInfo(PLAYER_SETTING_INFO& player_setting_info)
{
    m_player->m_setting_mgr->LoadPlayerSettingInfo(player_setting_info);
}

// ==================================================
// 加载自动购买设置信息
// ==================================================
void PlayerLoader::LoadAutobuySettingInfo(AUTOBUY_SETTING_INFO& autobuy_setting_info)
{
    m_player->m_setting_mgr->LoadAutobuySettingInfo(autobuy_setting_info);
}

// ==================================================
// 加载虚空幻境挂机设置信息
// ==================================================
void PlayerLoader::LoadVoidFieldSettingInfo(VOID_FIELD_SETTING_INFO& void_field_setting_info)
{
    m_player->m_setting_mgr->LoadVoidFieldSettingInfo(void_field_setting_info);
}

void PlayerLoader::LoadLivenessInfo( const LIVENESS_INFO& info )
{
    m_player->m_liveness_mgr->LoadLivenessInfo(info);
}

void PlayerLoader::LoadDungeonTimesInfo(DUNGEON_TIMES_LIST& info)
{
    m_player->m_dungeon_times_mgr->LoadDungeonTimesInfo(info);
}

void PlayerLoader::LoadLimitBuyInfo(LIMIT_BUY_LIST& info)
{
    m_player->m_limitbuy_mgr->LoadLimitBuyInfo(info);
}

void PlayerLoader::LoadSingleAnswerInfo(SINGLE_ANSWER_INFO& single_answer_info)
{
    m_player->m_single_answer_mgr->LoadInfo(single_answer_info);
}

void PlayerLoader::LoadBhInfo(BH_INFO& info)
{
    m_player->m_bh_mgr->LoadBhInfo(info);
}

// 加载任务信息
void PlayerLoader::LoadTaskInfo(TASK_INFO& info)
{
    m_player->m_task_mgr->LoadTaskInfo(info);
}

// 加载引导信息
void PlayerLoader::LoadGuideInfo(GUIDE_INFO& info)
{
    m_player->m_guide_mgr->LoadGuideInfo(info);
}

// 加载功能开启
void PlayerLoader::LoadOpenSystemInfo(OPEN_SYSTEM_INFO& info)
{
    m_player->m_opensys_mgr->LoadOpenSystemInfo(info);
}

// 加载仇人信息
void PlayerLoader::LoadEnemyInfo(ENEMY_INFO& info)
{
    m_player->m_enemy_mgr->LoadInfo(info);
}

void PlayerLoader::loadZombieInfo(ZOMBIE_INFO &info)
{
    m_player->m_zombie_mgr->LoadZombieInfo(info);
}

void PlayerLoader::LoadGuildScienceInfo(GUILD_PLAYER_SCIENCE_INFO& info)
{
    m_player->m_guild_science_mgr->LoadGuildScienceInfo(info);
}

void PlayerLoader::LoadShorcutInfo( SHORCUT_BAG& list )
{
    m_player->m_shorcut_mgr->LoadShorcutInfo(list);
}

void PlayerLoader::LoadMultiserverBattleInfo( const MULTISERVER_BATTLE_BAG& info )
{
    m_player->m_multiserver_battle_info->LoadMultiserverBattleInfo(info);
}

void PlayerLoader::LoadInvestmentPlan(INVESTMENT_PLAN_INFO& info)
{
    m_player->m_investment_plan_mgr->LoadInfo(info);
}

void PlayerLoader::LoadDonateInfo( DONATE_INFO& info )
{
    m_player->m_donate_mgr->LoadDonateInfo(info);
}

void PlayerLoader::LoadEggInfo( EGG_ACTIVITY_INFO& info )
{
    m_player->m_egg_manager->LoadEggInfo(info);
}

void PlayerLoader::LoadBuyCheapInfo(BUY_CHEAP_INFO& info)
{
    m_player->GetBuyCheapMgr()->LoadInfo(info);
}

void PlayerLoader::LoadHeadPortraitInfo(HEAD_PORTRAIT_INFO& info)
{
    m_player->m_headPortrait_mgr->LoadInfo(info);
}
