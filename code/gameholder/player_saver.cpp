#include "gameholder_pch.h"
#include "player_saver.h"
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
#include "open_system_manager.h"
#include "enemy_manager.h"
#include "zombie_manager.h"
#include "guild_science_recorder.h"
#include "shorcut_manager.h"
#include "multiserver_battle_info_manager.h"
#include "donate_manager.h"
#include "investment_plan_manager.h"
#include "property_set_calculater.h"
#include "skill_controller.h"
#include "buff_controller.h"
#include "state_controller.h"
#include "egg_manager.h"
#include "buy_cheap_manager.h"
#include "pet_manager.h"
#include "head_portrait_manager.h"
#include "bag_manager.h"
#include "store_manager.h"
#include "potion_manager.h"
#include "random_package_manager.h"

PlayerSaver::PlayerSaver(Player* player) :
m_player(player)
{

}

PlayerSaver::~PlayerSaver()
{

}

void PlayerSaver::Save(WRITEBACK_PLAYER_INFO_REQ& sendMsg)
{
    // 填充角色基本属性信息
    FillBaseInfo(sendMsg.prop_info);

    // 填充角色状态信息
    FillStateInfo(sendMsg.state_info);

    // 填充角色副本信息
    FillPlayerDungeonInfo(sendMsg.player_dungeon_info);

    // 填充开启系统信息
    FillOpenSystemInfo(sendMsg.open_system_info);

    // 填充公会福利信息
    FillGuildWelfareInfo(sendMsg.prop_info.guild_welfare_info);

    // 填充城市争夺战信息
    FillCityFightInfo(sendMsg.prop_info.city_fight_info);

    // 填充角色天宫图信息
    FillHoroscopeInfo(sendMsg.horoscope_info);

    // 填充角色技能信息
    FillSkillComponentInfo(sendMsg.skill_component_info);

    // 填充角色装备信息
    FillEquipInfo(sendMsg.equip_info);

    // 填充药剂使用信息
    FillPotionInfo(sendMsg.potion_info);

//     // 填充时装信息
//     FillFashionInfo(sendMsg.fashion_info);

    // 填充宠物信息
    FillPetInfo(sendMsg.pet_info);

    //填充骑宠信息
    FillRideInfo(sendMsg.ride_info);

    // 物品信息
    FillItemInfo(sendMsg.item_info);

    // 仓库信息
    FillStoreInfo(sendMsg.store_info);

    // 抽奖信息
    FillRandomPackageInfo(sendMsg.random_package_info);

    // 隐藏物品列表
    //FillHideBagInfo(sendMsg.hide_bag_info);

    // 填充任务信息
    FillTaskInfo(sendMsg.task_info);

    // 填充引导信息
    FillGuideInfo(sendMsg.guide_info);

    FillEnemyInfo(sendMsg.enemy_info);

    // 填充怪物之门信息
    FillMonsterDoorInfo(sendMsg.monster_door_info);

    // 填充勇者争霸信息
    FillPlayerDoorInfo(sendMsg.player_door_info);

    // 填充离线数据信息
    FillOffilePlayerInfo(sendMsg.offline_info);

    // 填充奖励池信息
    FillRewardInfo(sendMsg.reward_info);

    // 填充通天塔信息
    FillBabelInfo(sendMsg.babel_info);

    // 充值积分抽奖
    FillPayPointLotteryInfo(sendMsg.pay_point_lottery_info);

    // VIP独享副本
    FillVIPOnlyDungeonInfo(sendMsg.vip_only_dungeon_info);

    // 填充赛马信息
    FillRacingInfo(sendMsg.racing_info);

    // 填充签到信息
    FillSignInfo(sendMsg.sign_info);

    // 填充在线奖励信息
    FillOnlineRewardInfo(sendMsg.online_reward_info);

    // 填充采矿信息
    FillMiningInfo(sendMsg.mining_info);

    // 填充宝库信息
    FillTreasuryInfo(sendMsg.treasury_info);

    // 填充金像信息
    FillGoldtreeInfo(sendMsg.goldtree_info);

    // 填充大冒险信息
    FillAdventureInfo(sendMsg.adventure_info);

    // 填充答题信息
    FillAnswerInfo(sendMsg.answer_info);

    // 填充翅膀信息
    FillWingInfo(sendMsg.wing_info);

    // 填充修罗场信息
    FillAshuraInfo(sendMsg.ashura_info);

    // 填充商城信息
    FillMallInfo(sendMsg.restriction_info);

    // 填充被动技能信息
    FillPassiveSkillInfo(sendMsg.passive_skill_info);

    // 填充称号信息
    FillTitleInfo(sendMsg.title_info);

    // 填充日常投资信息
    FillDayInvestmentInfo(sendMsg.day_investment_info);

    // 填充等级投资信息
    FillLevelInvestmentInfo(sendMsg.level_investment_info);

    // 填充等级活动信息
    FillLevelActivityInfo(sendMsg.level_activity_info);

    // 填充自动战斗设置信息
    FillAutoFightSettingInfo(sendMsg.auto_fight_setting_info);
    
    // 填充系统设置信息
    FillSystemSettingInfo(sendMsg.system_setting_info);

    // 填充玩家设置信息
    FillPlayerSettingInfo(sendMsg.player_setting_info);

    // 填充自动购买设置信息
    FillAutobuySettingInfo(sendMsg.autobuy_setting_info);

    // 填充虚空幻境挂机设置信息
    FillVoidFieldSettingInfo(sendMsg.void_field_setting_info);

    // 填充活跃度信息
    FillLivenessInfo(sendMsg.liveness_info);

    // 填充副本进入次数信息
    FillDungeonTimesInfo(sendMsg.dungeon_times_list);

    // 填充限购信息
    FillLimitBuyInfo(sendMsg.limit_buy_list);

    // 填充单人答题信息
    FillSingleAnswerInfo(sendMsg.single_answer);

    // 填充圣剑信息
    FillSwordInfo(sendMsg.sword_info);

    // 填充宠物上阵信息
    FillPetFormation(sendMsg.pet_formation_info);

    // 填充僵尸副本信息
    FillZombieInfo(sendMsg.zombie_info);

    // 填充勇敢的心信息
    FillBhInfo(sendMsg.bh_info);

    // 填充礼品码信息
    FillGiftCodeInfo(sendMsg.gift_code_info);

    // 填充公会科技信息
    FillGuildScienceInfo(sendMsg.guild_science_info);

    // 收藏游戏状态
    FillShorcutInfo(sendMsg.shorcut_info);

    // 抽奖跨服战斗信息
    FillMultiserverBattleInfo(sendMsg.multiserver_battle_info);

    // 投资理财信息
    FillInvestmentPlan(sendMsg.investment_plan_info);

    // 捐献信息
    FillDonateInfo(sendMsg.donate_info);

    // 捐献信息
    FillEggInfo(sendMsg.egg_info);

    // 越买越便宜信息
    FillBuyCheapInfo(sendMsg.buy_cheap);

    // 填充个性头像信息信息
    FillHeadPortraitInfo(sendMsg.head_portrait);
}

// ==================================================
// 填充角色基本属性信息
// ==================================================
void PlayerSaver::FillBaseInfo(PROP_BASE_INFO& info)
{
    info.Clear();

    info.gatesvr_id = m_player->GetGateSvrID();
    info.login_id_l = U64ID_LOW(m_player->GetLoginID());
    info.login_id_h = U64ID_HIGH(m_player->GetLoginID());
    info.change_dungeon_count = m_player->GetChangeDungeonCount();
    info.account = m_player->GetAccount();
    info.name = m_player->GetName();
    info.platform_server_name = m_player->GetPlatformServerName();
    info.region = m_player->GetRegion();
    info.sex = m_player->GetSex();
    info.career = m_player->GetCareer();
    info.level = m_player->GetLevel();
    info.exp = m_player->GetExp();
    info.sys_vigor = m_player->GetSysVigor();
    info.buy_vigor = m_player->GetBuyVigor();
    info.vigor_reftime = m_player->GetVigorReftime();
    info.daily_reftime = m_player->GetDailyReftime();
    info.vigor_buytimes = m_player->GetVigorBuyTimes();
    info.team_buytimes = m_player->GetTeamBuyTimes();
    info.team_dungeon_systimes = m_player->GetTeamDungeonSysTimes();
    info.team_dungeon_buytimes = m_player->GetTeamDungeonBuyTimes();
    info.stone_turn_gold_times = m_player->GetStoneTurnGoldTimes();
    info.void_field_time = m_player->GetVoidFieldTime();
    info.void_field_reftime = m_player->GetVoidFieldReftime();
    info.battle_power = m_player->GetBattlePower();
    info.game_point = m_player->GetGamePoint();
    info.money_point = m_player->GetMoneyPoint();
    info.vigor = m_player->GetVigorValue();
    info.bind_money_point = m_player->GetBindMoneyPoint();
    info.soul_point = m_player->GetSoulPoint();
    info.star_point = m_player->GetStarPoint();
    info.honor_point = m_player->GetHonorPoint();
    info.guild_point = m_player->GetGuildPoint();
    info.incarnation_point = m_player->GetIncarnationPoint();
    info.battle_soul_point = m_player->GetBattleSoulPoint();
    info.spend_plus = m_player->GetGrowthPointSpendplus();
    info.hp = m_player->GetHP();                    // 保存hp
    info.mp = m_player->GetMP();                    // 保存mp
    info.guild_join_time = m_player->GetGuildJoinTime();
    info.unionmap_id = m_player->GetUnionMapID();
    info.x = m_player->GetX();
    info.y = m_player->GetY();
    info.city_id = m_player->GetCityMapID();
    info.city_x = m_player->GetCityX();
    info.city_y = m_player->GetCityY();
    info.online_time = m_player->GetOnLineTime();
    info.getwelcome_reward = m_player->GetWelcomeReward();
    info.angle = m_player->GetAngle();
    info.right_flag = m_player->GetRightFlag();
    info.login_ip = 0;

    m_player->m_vip_manager->FillInfo(info.vip_info);

    m_player->m_fcm_manager->FillInfo(info.fcm_info);

    m_player->m_vigor_mgr->FillInfo(info.vigor_info);

    m_player->m_offline_vigor_mgr->FillInfo(info.off_line_vigor_info);

}

void PlayerSaver::FillStateInfo(PLAYER_STATE_INFO& info)
{   
    m_player->GetSkillController()->FillSkillState(info.player_state);
    m_player->GetBuffController()->FillBuffState(info.player_state);
    m_player->GetStateController()->FillStateInfo(info.player_state);
}

void PlayerSaver::FillPlayerDungeonInfo(PLAYER_DUNGEON_INFO& dungeon_info)
{
    m_player->m_player_dungeon_mgr->FillPlayerDungeonInfo(dungeon_info);
}

void PlayerSaver::FillGuildWelfareInfo(GUILD_WELFARE_INFO& val) 
{ 
    val = m_player->m_guild_welfare_info; 
}

void PlayerSaver::FillCityFightInfo(CITY_FIGHT_PLAYER_INFO& val)
{ 
    val = m_player->m_city_fight_info; 
}

// ==================================================
// 填充天宫图信息
// ==================================================
void PlayerSaver::FillHoroscopeInfo(HOROSCOPE_INFO& horoscope_info)
{
    m_player->m_horoscope_mgr->FillInfo(horoscope_info);
}

// ==================================================
// 填充技能信息
// ==================================================
void PlayerSaver::FillSkillComponentInfo(SKILL_COMPONENT_INFO& info)
{
    m_player->m_skill_info_mgr->FillSkillComponentInfo(info);
}

// ==================================================
// 填充装备信息
// ==================================================
void PlayerSaver::FillEquipInfo(EQUIP_INFO& info)
{
    m_player->m_equip_mgr->FillEquipInfo(info);
    info.wing_id = m_player->GetWingID();
}

// ==================================================
// 填充药剂信息
// ==================================================
void PlayerSaver::FillPotionInfo(POTION_INFO& info)
{
    m_player->m_potion_use_mgr->FillPotionInfo(info);
}

void PlayerSaver::FillFashionInfo( FASHION_BAG& info )
{
    m_player->m_fashion_mgr->FillFashionInfo(info);
}

// ==================================================
// 填充跟宠信息
// ==================================================
void PlayerSaver::FillPetInfo(PET_INFO& info)
{
    m_player->m_pet_mgr->FillPetInfo(info);
}

// ==================================================
// 填充骑宠物信息
// ==================================================
void PlayerSaver::FillRideInfo(RIDE_INFO& info)
{
    m_player->m_ride_mgr->FillRideInfo(info);
}

// ==================================================
// 填充包裹信息
// ==================================================
void PlayerSaver::FillItemInfo(ITEM_INFO& info)
{
    m_player->m_bag_mgr->FillBagInfo(info);
}

// ==================================================
// 填充仓库信息
// ==================================================
void PlayerSaver::FillStoreInfo(ITEM_INFO& info)
{
     m_player->m_store_mgr->FillStoreInfo(info);
}

// ==================================================
// 填充抽奖信息
// ==================================================
void PlayerSaver::FillRandomPackageInfo(RANDOM_PACKAGE_RECORD_INFO& info)
{
    m_player->m_random_package_mgr->FillInfo(info);
}

// ==================================================
// 填充隐藏物品列表
// ==================================================
//void PlayerSaver::FillHideBagInfo(HIDE_BAG_INFO& info)
//{
//    m_player->m_item_mgr->FillHideBagInfo(info);
//}

// 填充任务信息
void PlayerSaver::FillTaskInfo(TASK_INFO& task_info)
{
    m_player->m_task_mgr->FillTaskInfo(task_info);
}

// 填充引导信息
void PlayerSaver::FillGuideInfo(GUIDE_INFO& guide_info)
{
    m_player->m_guide_mgr->FillGuideInfo(guide_info);
}

// 功能开启
void PlayerSaver::FillOpenSystemInfo(OPEN_SYSTEM_INFO& open_system_info)
{
    m_player->m_opensys_mgr->FillOpenSystemInfo(open_system_info);
}

// 填充仇人信息
void PlayerSaver::FillEnemyInfo(ENEMY_INFO& info)
{
    m_player->m_enemy_mgr->FillInfo(info);
}

// ==================================================
// 填充离线战斗数据
// ==================================================
void PlayerSaver::FillMonsterDoorInfo(MONSTER_DOOR_INFO& monster_door_info)
{
}

void PlayerSaver::FillPlayerDoorInfo(PLAYER_DOOR_INFO& player_door_info)
{
    m_player->m_player_door_mgr->FillPlayerDoorInfo(player_door_info);
}

// ==================================================
// 填充离线角色的信息
// ==================================================
void PlayerSaver::FillOffilePlayerInfo(OFFLINE_PLAYER_INFO& offline_info)
{
    offline_info.player_id_l = U64ID_LOW(m_player->GetPlayerID());
    offline_info.player_id_h = U64ID_HIGH(m_player->GetPlayerID());
    offline_info.vip = m_player->GetVipLevel();
    offline_info.name = m_player->GetName();
    offline_info.sex = m_player->GetSex();
    offline_info.career = m_player->GetCareer();
    offline_info.level = m_player->GetLevel();
    offline_info.exp = m_player->GetExp();
    offline_info.max_exp = m_player->GetMaxExp();
    //offline_info.strength = m_player->GetStrength();
    //offline_info.agility = m_player->GetAgility();
    //offline_info.spirit = m_player->GetSpirit();
    //offline_info.constitution = m_player->GetConstitution();
    offline_info.guild_name = m_player->GetGuildName();
    offline_info.battle_power = m_player->GetBattlePower();
    //offline_info.attack = m_player->GetBaseDamage();
    //offline_info.defence = m_player->GetArmor();
    //offline_info.fire_defence = m_player->GetFireResistance();
    //offline_info.ice_defence = m_player->GetIceResistance();
    //offline_info.thunder_defence = m_player->GetLightningResistance();
    //offline_info.attack_speed = m_player->GetAttackSpeed();
    //offline_info.move_speed = m_player->GetMoveSpeed();
    //offline_info.critical_attack = m_player->GetCrit();
    //offline_info.cri_att_red = m_player->GetCriticalAttackReduction();
    //offline_info.cri_att_damage_add = m_player->GetCritDamage();
    //offline_info.cri_att_damage_red = m_player->GetCriticalAttackDamageReduction();
    //offline_info.hit_rate = m_player->GetHit();
    //offline_info.dodge = m_player->GetDodge();
    //offline_info.res_hp = m_player->GetHpRecovery();
    //offline_info.res_mp = m_player->GetMpRecovery(); 
    //offline_info.max_hp = m_player->GetMaxHp();
    //offline_info.max_mp = m_player->GetMaxMp();
    offline_info.wing = m_player->m_wing_mgr->GetWingInfo().transmogrification;
    offline_info.score = m_player->m_player_dungeon_mgr->GetPlotDungonScore();
    offline_info.achive_count = m_player->m_task_mgr->GetAchiveCount();
    //offline_info.fashion_id = m_player->GetFashionManager()->GetFashionID();

    // 战斗力信息
//     m_player->m_property_set_calculater->FillBattlePowerInfo(offline_info.battle_power_info);
//
//     // 装备信息
//     const EquipManager::EquipList& equipList = m_player->GetEquipManager()->GetEquipList();
//     for(EquipManager::EquipList::const_iterator iter = equipList.begin(); iter != equipList.end(); iter++)
//     {
//         const EQUIP_INFO& equipInfo = *iter;
// 
//         OFFLINE_EQUIP_INFO offlineEquipInfo;
//         offlineEquipInfo.appear_id = equipInfo.appear_id;
//         offlineEquipInfo.star = equipInfo.star;
//         offlineEquipInfo.level = equipInfo.level;
//         offlineEquipInfo.quality = equipInfo.quality;
//         offlineEquipInfo.enchant1 = equipInfo.enchant1;
//         offlineEquipInfo.enchant2 = equipInfo.enchant2;
//         offlineEquipInfo.enchant3 = equipInfo.enchant3;
//         offlineEquipInfo.cur_slot = equipInfo.cur_slot;
//         offlineEquipInfo.max_slot = equipInfo.max_slot;
//         offlineEquipInfo.jewel1 = equipInfo.jewel1;
//         offlineEquipInfo.jewel2 = equipInfo.jewel2;
//         offlineEquipInfo.jewel3 = equipInfo.jewel3;
//         offlineEquipInfo.jewel4 = equipInfo.jewel4;
// 
//         offline_info.equip_list.push_back(offlineEquipInfo);
// 
//     }
}

// ==================================================
// 填充奖励池数据
// ==================================================
void PlayerSaver::FillRewardInfo(REWARD_INFO& reward_info)
{
    reward_info = m_player->m_reward_info;
}

// ==================================================
// 填充通天塔信息
// ==================================================
void PlayerSaver::FillBabelInfo(BABEL_INFO& babel_info)
{
    //m_player->m_babel_mgr->FillBabelInfo(babel_info);
}

// ==================================================
// 填充充值积分抽奖信息
// ==================================================
void PlayerSaver::FillPayPointLotteryInfo(PAY_POINT_LOTTERY_INFO &info)
{
    m_player->m_pay_point_lottery->FillPayPointLotteryInfo(info);
}

// ==================================================
// 填充VIP独享副本信息
// ==================================================
void PlayerSaver::FillVIPOnlyDungeonInfo(VIP_ONLY_DUNGEON_INFO &info)
{
    m_player->m_vip_only_dungeon->FillVIPOnlyDungeonInfo(info);
}

// ==================================================
// 填充赛马信息
// ==================================================
void PlayerSaver::FillRacingInfo(RACING_INFO& racing_info)
{
    //m_racing_mgr->FillRacingInfo(racing_info);
}

// ==================================================
// 填充签到信息
// ==================================================
void PlayerSaver::FillSignInfo(SIGN_INFO& sign_info)
{
    m_player->m_sign_mgr->FillSignInfo(sign_info);
}

// ==================================================
// 填充在线奖励信息
// ==================================================
void PlayerSaver::FillOnlineRewardInfo(ONLINE_REWARD_INFO& online_reward_info)
{
    m_player->m_online_reward_mgr->FillInfo(online_reward_info);
}

// ==================================================
// 填充采矿信息
// ==================================================
void PlayerSaver::FillMiningInfo(MINING_INFO& mining_info)
{
    //m_mining_mgr->FillMiningInfo(mining_info);
}
// ==================================================
// 填充宝库信息
// ==================================================
void PlayerSaver::FillTreasuryInfo(TREASURY_INFO& info)
{
    m_player->m_treasury_mgr->FillTreasuryInfo(info);
}

// ==================================================
// 填充金像信息
// ==================================================
void PlayerSaver::FillGoldtreeInfo(GOLDTREE_INFO& info)
{
    m_player->m_goldtree_mgr->FillGoldtreeInfo(info);
}

// ==================================================
// 填充大冒险信息
// ==================================================
void PlayerSaver::FillAdventureInfo(ADVENTURE_INFO& info)
{
    m_player->m_adventure_mgr->FillAdventureInfo(info);
}

// ==================================================
// 填充答题数据
// ==================================================
void PlayerSaver::FillAnswerInfo(ANSWER_INFO& info)
{
    m_player->m_answer_mgr->FillAnswerInfo(info);
}

// ==================================================
// 填充翅膀信息
// ==================================================
void PlayerSaver::FillWingInfo(WING_INFO& info)
{
    m_player->m_wing_mgr->FillInfo(info);
}

// ==================================================
// 填充修罗场信息
// ==================================================
void PlayerSaver::FillAshuraInfo(ASHURA_INFO& info)
{
    m_player->m_ashura_recorder->FillAshuraInfo(info);
}

// ==================================================
// 保存商城数据
// ==================================================
void PlayerSaver::FillMallInfo(RESTRICTION_LIST& info)
{
    m_player->m_shop_mgr->FillMallInfo(info);
}

// ==================================================
// 保存被动技能
// ==================================================
void PlayerSaver::FillPassiveSkillInfo(PASSIVE_SKILL_LIST& info)
{
    m_player->m_passive_skill_mgr->FillPassiveSkillInfo(info);
}

// ==================================================
// 填充称号信息
// ==================================================
void PlayerSaver::FillTitleInfo(TITLE_LIST& info)
{
    m_player->m_title_mgr->FillTitleInfo(info);
}

// ==================================================
// 填充日常投资信息
// ==================================================
void PlayerSaver::FillDayInvestmentInfo( DAY_INVESTMENT_INFO& day_investment_info )
{
    m_player->m_day_investment_mgr->FillDayInvestmentInfo(day_investment_info);
}

// ==================================================
// 填充等级投资信息
// ==================================================
void PlayerSaver::FillLevelInvestmentInfo( LEVEL_INVESTMENT_INFO& level_investment_info )
{
    m_player->m_level_investment_mgr->FillLevelInvestmentInfo(level_investment_info);
}

void PlayerSaver::FillLevelActivityInfo( LEVEL_ACTIVITY_INFO& level_activity_info )
{
    m_player->m_level_activity_mgr->FillLevelActivityInfo(level_activity_info);
}

// ==================================================
// 填充自动战斗设置信息
// ==================================================
void PlayerSaver::FillAutoFightSettingInfo(AUTO_FIGHT_SETTING_INFO& auto_fight_setting_info)
{
    m_player->m_setting_mgr->FillAutoFightSettingInfo(auto_fight_setting_info);
}

// ==================================================
// 填充系统设置信息
// ==================================================
void PlayerSaver::FillSystemSettingInfo(SYSTEM_SETTING_INFO& system_setting_info)
{
    m_player->m_setting_mgr->FillSystemSettingInfo(system_setting_info);
}

// ==================================================
// 填充玩家设置信息
// ==================================================
void PlayerSaver::FillPlayerSettingInfo(PLAYER_SETTING_INFO& player_setting_info)
{
    m_player->m_setting_mgr->FillPlayerSettingInfo(player_setting_info);
}

// ==================================================
// 填充自动购买设置信息
// ==================================================
void PlayerSaver::FillAutobuySettingInfo(AUTOBUY_SETTING_INFO& autobuy_setting_info)
{
    m_player->m_setting_mgr->FillAutobuySettingInfo(autobuy_setting_info);
}

// ==================================================
// 填充虚空幻境挂机设置信息
// ==================================================
void PlayerSaver::FillVoidFieldSettingInfo(VOID_FIELD_SETTING_INFO& void_field_setting_info)
{
    m_player->m_setting_mgr->FillVoidFieldSettingInfo(void_field_setting_info);
}

void PlayerSaver::FillLivenessInfo( LIVENESS_INFO& info )
{
    m_player->m_liveness_mgr->FillLivenessInfo(info);
}

void PlayerSaver::FillDungeonTimesInfo(DUNGEON_TIMES_LIST& info)
{
    m_player->m_dungeon_times_mgr->FillDungeonTimesInfo(info);
}

void PlayerSaver::FillLimitBuyInfo(LIMIT_BUY_LIST& info)
{
    m_player->m_limitbuy_mgr->FillLimitBuyInfo(info);
}

void PlayerSaver::FillSingleAnswerInfo(SINGLE_ANSWER_INFO& single_answer_info)
{
    m_player->m_single_answer_mgr->FillInfo(single_answer_info);
}

void PlayerSaver::FillSwordInfo( SWORD_NEW_INFO& info )
{
    m_player->m_sword_mgr->FillInfo(info);
}

void PlayerSaver::FillPetFormation(PET_FORMATION_INFO& info)
{
    //m_player->m_pet_formation_mgr->FillInfo(info);
}

void PlayerSaver::FillZombieInfo(ZOMBIE_INFO &info)
{
    m_player->m_zombie_mgr->FillZombieInfo(info);
}

void PlayerSaver::FillBhInfo(BH_INFO& info)
{
    m_player->m_bh_mgr->FillBhInfo(info);
}

void PlayerSaver::FillGiftCodeInfo(GIFT_CODE_INFO& info)
{
    info = m_player->m_gift_code_info;
}

void PlayerSaver::FillGuildScienceInfo(GUILD_PLAYER_SCIENCE_INFO& info)
{
    m_player->m_guild_science_mgr->FillGuildScienceInfo(info);
}

void PlayerSaver::FillShorcutInfo( SHORCUT_BAG& info )
{
    m_player->m_shorcut_mgr->FillShorcutInfo(info);
}

void PlayerSaver::FillMultiserverBattleInfo(MULTISERVER_BATTLE_BAG& info)
{
    m_player->m_multiserver_battle_info->FillMultiserverBattleInfo(info);
}

void PlayerSaver::FillInvestmentPlan(INVESTMENT_PLAN_INFO& info)
{
    m_player->m_investment_plan_mgr->FillInfo(info);
}

void PlayerSaver::FillDonateInfo( DONATE_INFO& info )
{
    m_player->m_donate_mgr->FillDonateInfo(info);
}

void PlayerSaver::FillEggInfo( EGG_ACTIVITY_INFO& info )
{
    m_player->m_egg_manager->FillEggInfo(info);
}

void PlayerSaver::FillBuyCheapInfo(BUY_CHEAP_INFO& info)
{
    m_player->GetBuyCheapMgr()->FillInfo(info);
}

void PlayerSaver::FillHeadPortraitInfo(HEAD_PORTRAIT_INFO& info)
{
    m_player->m_headPortrait_mgr->FillInfo(info);
}