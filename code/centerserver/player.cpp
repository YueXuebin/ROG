#include "centerserver_pch.h"
#include "player.h"
#include "player_manager.h"
#include "player_net_handler.h"
#include "gate_handler.h"
#include "game_handler.h"
#include "config_loader.h"
#include "mail_job.h"
#include "player_job.h"
#include "guild_manager.h"
#include "dongyi_log.h"
#include "vip_config.h"
#include "multiserver_battle_manager.h"

Player::Player()
{
    m_pNetHandler = CnNew PlayerNetHandler(this);
    m_state = kStateInit;
    m_auto_save_timer = 0;

}

Player::~Player()
{
    if(m_pNetHandler)
    {
        CnDelete m_pNetHandler;
        m_pNetHandler = NULL;
    }
}

void Player::Clear()
{
    m_InGameServerType      = 0;
    m_InDungeonCheck        = false;
    m_dirty                 = false;
    m_ticket                = 0;
    m_state                 = kStateInit;
    m_session_id            = 0;
    m_login_id              = 0;
    m_player_id             = 0;
    m_login_time            = 0;
    m_logout_time           = 0;
    m_login_ip              = 0;
    m_login_port            = 0;
    m_prop_info.Clear();
    m_player_dungeon_info.Clear();
    m_platform_uid          = "";
    m_platform_uname        = "";
    m_platform              = "";
    m_MoveSpeed             = 0;
    m_dungeon_type          = 0;
    m_change_map_count      = 0;
    m_enemy_info.Clear();
    m_skill_info.Clear();
    m_item_info.Clear();
    m_package_record.Clear();
    m_equip_info.Clear();
    m_potion_info.Clear();
    m_rideinfo.Clear();
    m_pet_info.Clear();
    m_friend_mgr.Clear();
    m_friend_has_me.Clear();
    m_task_info.Clear();
    m_monster_door_info.Clear();
    m_player_door_info.Clear();
    m_offline_player_info.Clear();
    m_reward_info.Clear();
    m_babel_info.Clear();
    m_horoscope_info.Clear();
    m_sign_info.Clear();
    m_open_sysetm_info.Clear();
    m_online_reward_info.Clear();
    m_mining_info.Clear();
    m_racing_info.Clear();
    m_treasury_info.Clear();
    m_goldtree_info.Clear();
    m_adventure_info.Clear();
    m_wing_info.Clear();
    m_ashura_info.Clear();
    m_title_list.Clear();
    m_recharge_info.Clear();
    m_mall_list.Clear();
    m_day_investment_info.Clear();
    m_level_investment_info.Clear();
    m_level_activity_info.Clear();
    m_auto_fight_setting_info.Clear();
    m_system_setting_info.Clear();
    m_player_setting_info.Clear();
    m_autobuy_setting_info.Clear();
    m_dungeon_times_info.Clear();
    m_limitbuy_info.Clear();
    m_single_answer_info.Clear();
    m_fashion_bag.Clear();
    m_bh_info.Clear();
    m_shorcut_info.Clear();
    m_MultiserverBattleInfo.Clear();

    m_team_id               = 0;
    m_gatesvr_id            = 0;
    m_gamesvr_id            = 0;
}
// 玩家数据加载完成
void Player::OnCreate()
{
    CnDbg("create player %s %s\n", GetPlatformServerName().c_str(), GetName().c_str());
    m_pNetHandler->OnLoadEnd();
}

void Player::OnDestory()
{
    // 通知好友我下线了
    FRIEND_INFO info;
    info.Clear();

    info.gatesvr_id = 0;
    info.player_id_l = U64ID_LOW(GetPlayerID());
    info.player_id_h = U64ID_HIGH(GetPlayerID());
    info.name = GetName();
    info.sex = GetSex();
    info.level = GetLevel();
    info.logout_time = GetLogoutTime();
    info.head_portrait = GetHeadPortraitInfo();

    NofityFriendInfo(info);

    // 更新公会成员中的上下线状态
    if(GetGuildID())
    {
        GuildManager::Instance()->SetGuildMemberOffline(GetGuildID(), GetPlayerID());
    }
    MultiserverBattleManager::Instance()->onExitGame(this);
}

void Player::Update()
{
    m_pNetHandler->Update();

    // 自动回存DB
    m_auto_save_timer += g_ServerUpdateTime;
    if(GetState() == kStateInGame && m_auto_save_timer > ConfigLoader::Instance()->GetAutoSavePeriod() * 1000)
    {
        m_auto_save_timer = 0;
        PlayerJob::Instance()->AutoSavePlayerDataToDB(this);
    }
}

// 角色状态变迁
void Player::SetState(uint32 val)
{
    uint32 OldState = m_state;
    uint32 NewState = val;
    m_state = val;

    if(OldState == NewState)
    {
        CnAssert(false);        // 相同状态的两次设置
        return;
    }

    //CnDbg("player %s state %d\n", GetName().c_str(), m_state);

    CnAssert(OldState != kStateLogoutSyncData);     // 进入登出状态后，就不可以在进入其他状态了

    // 状态变迁的检测
    switch(NewState)
    {
    case kStateDelivingData:
        CnAssert(OldState == kStateDataLoaded);
        break;
    case kStateInGame:
        CnAssert(OldState == kStateDelivingData || OldState == kStateChangeDungeonSyncData);
        break;
    case kStateChangeDungeonSyncData:
        CnAssert(OldState == kStateInGame);
        break;
    default:
        break;
    }

    // 玩家状态改变log
    if(NewState < kStateLoadingData)
    {
        DoingGameLog::Instance()->WriteCenterLog(LOG_PLAYER_STATE, GetLoginID(), WriterToLogByPlayerState(GetPlatformUName(), NewState, OldState));
    }
    else
    {
        DoingGameLog::Instance()->WriteCenterLog(LOG_PLAYER_STATE, GetPlayerID(), WriterToLogByPlayerState(GetName(), NewState, OldState));
    }
}

void Player::SetCentGameID(uint32 val)
{
    CnAssert(CalGameIDByRegionID(val));
    CnAssert(CalCenterIDByRegionID(val));
    if(m_gamesvr_id)
        GameHandler::Instance()->RemovePlayerFromRegion(this, m_gamesvr_id);

    m_gamesvr_id = val;

    if(m_gamesvr_id)
        GameHandler::Instance()->AddPlayerToRegion(this, m_gamesvr_id);
}

// ==================================================
// 填充角色属性
// ==================================================
void Player::FillPropInfo(PROP_BASE_INFO& prop_info)
{
    prop_info.gatesvr_id = GetCentGateID();
    prop_info.login_id_l = U64ID_LOW(GetLoginID());
    prop_info.login_id_h = U64ID_HIGH(GetLoginID());
    prop_info.centersvr_id = ConfigLoader::Instance()->GetCenterID();
    prop_info.change_dungeon_count = GetChangeMapCount();
    prop_info.account = GetAccount();
    prop_info.name = GetName();
    prop_info.platform_server_name = GetPlatformServerName();
    prop_info.region = GetRegion();
    prop_info.timezone = Crown::SDGetTimeZone();        // 将此Center的时区 跟随玩家走 跨服时进行时区计算
    prop_info.sex = GetSex();
    prop_info.career = GetCareer();
    prop_info.level = GetLevel();
    prop_info.exp = GetExp();
    prop_info.daily_reftime = GetDailyReftime();

    //prop_info.sys_vigor = GetSysVigor();
    //prop_info.buy_vigor = GetBuyVigor();
    //prop_info.vigor_reftime = GetVigorReftime();
    
    //prop_info.vigor_buytimes = GetVigorBuyTimes();
    //prop_info.team_buytimes = GetTeamBuyTimes();
    //prop_info.team_dungeon_systimes = GetTeamDungeonSysTimes();
    //prop_info.team_dungeon_buytimes = GetTeamDungeonBuyTimes();
    //prop_info.stone_turn_gold_times = GetStoneTurnGoldTimes();
    //prop_info.void_field_time = GetVoidFieldTime();
    //prop_info.void_field_reftime = GetVoidFieldReftime();
    prop_info.game_point = GetGamePoint();
    prop_info.money_point = GetMoneyPoint();
    prop_info.vigor = GetVigorValue();
    prop_info.vigor_reftime = GetVigorReftime();
    //prop_info.bind_money_point = GetBindMoneyPoint();
    //prop_info.soul_point = GetSoulPoint();
    //prop_info.star_point = GetStarPoint();
    //prop_info.honor_point = GetHonorPoint();
    //prop_info.guild_point = GetGuildPoint();
    //prop_info.incarnation_point = GetIncarnationPoint();
    //prop_info.battle_soul_point = GetBattleSoulPoint();
    //prop_info.spend_plus = GetGrowthPointSpendplus();
    prop_info.hp = GetHP();
    prop_info.mp = GetMP();
    prop_info.guild_id_h = U64ID_HIGH(GetGuildID());
    prop_info.guild_id_l = U64ID_LOW(GetGuildID());
    prop_info.guild_join_time = GetGuildJoinTime();
    prop_info.unionmap_id = GetUnionMapID();
    prop_info.x = GetX();
    prop_info.y = GetY();
    prop_info.city_id = GetCityMapID();
    prop_info.city_x = GetCityX();
    prop_info.city_y = GetCityY();
    //prop_info.field_map_id = GetFieldMapID();
    //prop_info.field_map_x = GetFieldX();
    //prop_info.field_map_y = GetFieldY();
    //prop_info.field_hook_x = GetFieldHookX();
    //prop_info.field_hook_y = GetFieldHookY();
    prop_info.online_time = GetOnLineTime();
    //prop_info.getwelcome_reward = GetWelcomeReward();
    prop_info.angle = GetAngle();
    prop_info.team_id_h = U64ID_HIGH(GetTeamID());
    prop_info.team_id_l = U64ID_LOW(GetTeamID());
    prop_info.right_flag = GetRightFlag();
    prop_info.login_ip = GetLoginIP();

    //prop_info.vip_info = m_prop_info.vip_info;

    //prop_info.fcm_info = m_prop_info.fcm_info;
    //prop_info.vigor_info = m_prop_info.vigor_info;
    //prop_info.off_line_vigor_info = m_prop_info.off_line_vigor_info;
}
// ==================================================
// 角色状态信息
// ==================================================
void Player::FillStateInfo(PLAYER_STATE_INFO& state_info)
{
    state_info = m_state_info;
}

void Player::SetStateInfo(const PLAYER_STATE_INFO& state_info)
{
    m_state_info = state_info;
}

// ==================================================
// 填充角色副本信息
// ==================================================
void Player::FillPlayerDungeonInfo(PLAYER_DUNGEON_INFO& info)
{
    info = m_player_dungeon_info;
}

// ==================================================
// 设置角色副本信息
// ==================================================
void Player::SetPlayerDungeonInfo(const PLAYER_DUNGEON_INFO& info)
{
    m_player_dungeon_info = info;
}

// ==================================================
// 获得角色副本信息
// ==================================================
PLAYER_DUNGEON_INFO& Player::GetPlayerDungeonInfo()
{
    return m_player_dungeon_info;
}

// ==================================================
// 填充装备属性
// ==================================================
void Player::FillEquipInfo(EQUIP_INFO& equip_info)
{
    equip_info = m_equip_info;
}

// ==================================================
// 填充药剂信息
// ==================================================
void Player::FillPotionInfo(POTION_INFO& potion_info) const
{
    potion_info = m_potion_info;
}

// ==================================================
// 填充包裹
// ==================================================
void Player::FillItemInfo(ITEM_INFO& item_info)
{
    item_info = m_item_info;
}

void Player::SetStoreInfo(const ITEM_INFO& store_info)
{
    m_store_info = store_info;
}

void Player::FillStoreInfo(ITEM_INFO& store_info)
{
    store_info = m_store_info;
}

// ==================================================
// 填充抽奖信息
// ==================================================
void Player::FillRandomPackageInfo(RANDOM_PACKAGE_RECORD_INFO& info)
{
    info = m_package_record;
}


//void Player::FillHideBagInfo(HIDE_BAG_INFO& hide_bag_info)
//{
//    hide_bag_info = m_hide_bag_info;
//}

void Player::FillPetInfo(PET_INFO& pet_info)
{ 
    pet_info = m_pet_info;
}

void Player::SetPetInfo(const PET_INFO& pet_info)
{
    m_pet_info = pet_info ;
}

void Player::FillRidePetInfo(RIDE_INFO& ride_info)
{
    ride_info = m_rideinfo;
}
void Player::SetRidePetInfo(const RIDE_INFO& ride_info)
{
    m_rideinfo = ride_info;
}
// ==================================================
// 填充好友
// ==================================================
void  Player::FillFriendInfo(FRIEND_LIST& friend_list)
{
    friend_list = m_friend_mgr;
}

// ==================================================
// 填充善恶信息
// ==================================================
void Player::FillEnemyInfo(ENEMY_INFO& enemy_info)
{
    enemy_info = m_enemy_info;
}

// ==================================================
// 填充任务
// ==================================================
void Player::FillTaskInfo(TASK_INFO& task_info)
{
    task_info = m_task_info;
}

void Player::FillGuideInfo(GUIDE_INFO& guide_info) const
{
    guide_info = m_guide_info;
}

void Player::FillOpenSystemInfo(OPEN_SYSTEM_INFO& info) const
{
    info = m_open_sysetm_info;
}

// 怪物之门战斗信息
void  Player::FillMonsterDoorInfo(MONSTER_DOOR_INFO& monster_door_info)
{
    monster_door_info = m_monster_door_info;
}

// 勇者争霸战斗信息
void Player::FillPlayerDoorInfo(PLAYER_DOOR_INFO& player_door_info)
{
    player_door_info = m_player_door_info;
}

// ==================================================
// 填充奖励信息
// ==================================================
void Player::FillRewardInfo(REWARD_INFO& reward_info)
{
    reward_info = m_reward_info;
}

// ==================================================
// 填充通天塔信息
// ==================================================
void Player::FillBabelInfo(BABEL_INFO& babel_info)
{
    babel_info = m_babel_info;
}

//==================================================
// 填充充值积分抽奖数据
//==================================================
void Player::FillPayPointLotteryInfo(PAY_POINT_LOTTERY_INFO &pay_point_lottery_info)
{
    pay_point_lottery_info = m_pay_point_lottery_info;
}

//===================================================
// 填充VIP独享副本数据
//===================================================
void Player::FillVIPOnlyDungeonInfo(VIP_ONLY_DUNGEON_INFO &vip_only_dungeon_info)
{
    vip_only_dungeon_info = m_vip_only_dungeon_info;
}

// ==================================================
// 填充邮件系统信息
// ==================================================
void Player::FillMailSystemInfo(MAIL_SYSTEM_INFO& mail_system_info)
{
    mail_system_info = m_mail_info;
}
// ==================================================
// 星空图属性
// ==================================================
void Player::FillHoroscopeInfo(HOROSCOPE_INFO& horoscope_info)
{
    horoscope_info = m_horoscope_info;
}

// ==================================================
// 填充技能信息
// ==================================================
void Player::FillSkillInfo(SKILL_COMPONENT_INFO& skill_info)
{
    skill_info = m_skill_info;
}

// ==================================================
// 填充签到信息
// ==================================================
void Player::FillSignInfo(SIGN_INFO& sign_info)
{
    sign_info = m_sign_info;
}

// ==================================================
// 填充公会福利信息
// ==================================================
//void Player::FillGuildWelfareInfo(GUILD_WELFARE_INFO& guild_welfare_info)
//{
//    guild_welfare_info = m_prop_info.guild_welfare_info;
//}

// ==================================================
// 填充城市争夺战信息
// ==================================================
//void Player::FillCityFightInfo(CITY_FIGHT_PLAYER_INFO& city_fight_info)
//{
//    city_fight_info = m_prop_info.city_fight_info;
//}

// ==================================================
// 填充防沉迷信息
// ==================================================
//void Player::FillFcmInfo(FCM_INFO& fcm_info)
//{
//    fcm_info = m_prop_info.fcm_info;
//}

// ==================================================
// 填充在线信息
// ==================================================
void Player::FillOnlineRewardInfo(ONLINE_REWARD_INFO& online_reward_info)
{
    online_reward_info = m_online_reward_info;
}

// ==================================================
// 填充时装信息
// ==================================================
void Player::FillFashionInfo(FASHION_BAG& fashion_bag)
{
    fashion_bag = m_fashion_bag;
}

// ==================================================
// 设置时装信息
// ==================================================
void Player::SetFashionInfo(const FASHION_BAG& fashion_bag)
{
    m_fashion_bag = fashion_bag;
}

// ==================================================
// 填充勇敢的心数据
// ==================================================
void Player::FillBhInfo(BH_INFO& bh_info)
{
    bh_info = m_bh_info;
}

// ==================================================
// 设置勇敢的心数据
// ==================================================
void Player::SetBhInfo(const BH_INFO& bh_info)
{
    m_bh_info = bh_info;
}


// ==================================================
// 设置包裹
// ==================================================
void Player::SetItemInfo(const ITEM_INFO& item_info)
{
    m_item_info = item_info;
}

// ==================================================
// 设置抽奖
// ==================================================
void Player::SetRandomPackageInfo(const RANDOM_PACKAGE_RECORD_INFO& info)
{
    m_package_record = info;
}

// ==================================================
// 设置隐藏物品
// ==================================================
//void Player::SetHideBagInfo(const HIDE_BAG_INFO& hide_bag_info)
//{
//    m_hide_bag_info = hide_bag_info;
//}

// ==================================================
// 设置善恶信息
// ==================================================
void Player::SetEnemyInfo(const ENEMY_INFO& enemy_info)
{
    m_enemy_info.evil_point = enemy_info.evil_point;
    m_enemy_info.update_time = enemy_info.update_time;
    m_enemy_info.enemy_list = enemy_info.enemy_list;
}

// ==================================================
// 设置装备
// ==================================================
void Player::SetEquipInfo(const EQUIP_INFO& equip_info)
{
    m_equip_info = equip_info;
}

// 设置药剂
void Player::SetPotionInfo(const POTION_INFO& potion_info)
{
    m_potion_info = potion_info;
}

// ==================================================
// 更新任务
// ==================================================
void Player::SetTaskInfo(const TASK_INFO& task_info)
{
    m_task_info.task_game_info = task_info.task_game_info;
    m_task_info.task_festival_info = task_info.task_festival_info;
}
// 引导
void Player::SetGuideInfo(const GUIDE_INFO& guide_info)
{
    m_guide_info = guide_info;
}
// 功能开启
void Player::SetOpenSystemInfo(const OPEN_SYSTEM_INFO& info)
{
    m_open_sysetm_info = info;
}

// 怪物之门
void Player::SetMonsterDoorInfo(const MONSTER_DOOR_INFO& monster_door_info)
{
    m_monster_door_info = monster_door_info;
}

// 勇者争霸
void Player::SetPlayerDoorInfo(const PLAYER_DOOR_INFO& player_door_info)
{
    m_player_door_info = player_door_info;
}

// ==================================================
// 离线角色信息
// ==================================================
void Player::SetOfflinePlayerInfo(const OFFLINE_PLAYER_INFO& offline_info)
{
    m_offline_player_info = offline_info;
}

OFFLINE_PLAYER_INFO& Player::GetOfflinePlayerInfo() 
{
    return m_offline_player_info; 
}

// ==================================================
// 更新角色的社交状态
// ==================================================
void Player::UpdateSocialState()
{
    // 更新好友数据
    for(std::vector<FRIEND_INFO>::iterator friendIter = m_friend_mgr.list.begin(); friendIter != m_friend_mgr.list.end(); friendIter++)
    {
        FRIEND_INFO& friendInfo = *friendIter;

        uint64 player_id = U64ID(friendInfo.player_id_h, friendInfo.player_id_l);
        Player* myFriend = PlayerManager::Instance()->GetPlayerByPlayerID(player_id);
        if(!myFriend)
            continue;

        friendInfo.gatesvr_id = myFriend->GetCentGateID();
    }

    // 更新公会成员的状态
    if(GetGuildID())
    {
        GuildManager::Instance()->SetGuildMemberOnline(GetGuildID(), GetPlayerID());
    }
}

// ==================================================
// 添加游戏币
// ==================================================
void Player::AddMoneyPoint(int32 val, int32 type)
{
    int32 old_money_point = m_prop_info.money_point;
    m_prop_info.money_point += val;
    // 写添加魔石的日志
    DoingGameLog::Instance()->WriteCenterLog(LOG_PAY, GetPlayerID(), WriterToLogByMoneyPointPay(type, val, old_money_point, m_prop_info.money_point));
}

// ==================================================
// 设置属性信息
// ==================================================
void Player::SetPropInfo(PROPERTY_INFO& prop_info)
{
    m_prop_info = prop_info;
}

// ==================================================
// 奖励池数据
// ==================================================
void Player::SetRewardInfo(const REWARD_INFO& reward_info)
{
    m_reward_info = reward_info;
}

// ==================================================
// 通天塔数据
// ==================================================
void Player::SetBabelInfo(const BABEL_INFO& babel_info)
{
    m_babel_info = babel_info;
}

//==================================================
// 设置充值积分抽奖数据
//==================================================
void Player::SetPayPointLotteryInfo(const PAY_POINT_LOTTERY_INFO &pay_point_lottery_info)
{
    m_pay_point_lottery_info = pay_point_lottery_info;
}

//===================================================
// 设置VIP独享副本数据
//===================================================
void Player::SetVIPOnlyDungeonInfo(const VIP_ONLY_DUNGEON_INFO &vip_only_dungeon_info)
{
    m_vip_only_dungeon_info = vip_only_dungeon_info;
}

// ==================================================
// 复制邮件系统信息
// ==================================================
void Player::SetMailSystemInfo(const MAIL_SYSTEM_INFO& mail_system_info)
{
    // 邮件系统直接由GS操作了
    CnAssert(mail_system_info.player_mail_list.size() == 0);
    CnAssert(mail_system_info.system_mail_list.size() == 0);
}

// ==================================================
// 设置星空图信息
// ==================================================
void Player::SetHoroscopeInfo(const HOROSCOPE_INFO& horoscope_info)
{
    m_horoscope_info = horoscope_info;
}

// ==================================================
// 设置技能信息
// ==================================================
void Player::SetSkillInfo(const SKILL_COMPONENT_INFO& skill_info)
{
    m_skill_info = skill_info;
}

// ==================================================
// 设置签到信息
// ==================================================
void Player::SetSignInfo(const SIGN_INFO& sign_info)
{
    m_sign_info = sign_info;
}

// ==================================================
// 设置公会福利信息
// ==================================================
//void Player::SetGuildWelfareInfo(const GUILD_WELFARE_INFO& guild_welfare_info)
//{
//    m_prop_info.guild_welfare_info = guild_welfare_info;
//}

// ==================================================
// 设置公会福利信息
// ==================================================
//void Player::SetCityFightInfo(const CITY_FIGHT_PLAYER_INFO& city_fight_info)
//{
//    m_prop_info.city_fight_info = city_fight_info;
//}

// ==================================================
// 设置防沉迷信息
// ==================================================
//void Player::SetFcmInfo(const FCM_INFO& fcm_info)
//{
//    m_prop_info.fcm_info = fcm_info;
//}

// ==================================================
// 设置在线奖励信息
// ==================================================
void Player::SetOnlineRewardInfo(const ONLINE_REWARD_INFO& online_reward_info)
{
    m_online_reward_info = online_reward_info;
}

//==================================================
// 填充体力信息
//==================================================
//void Player::FillVigorInfo(VIGOR_INFO& vigor_info)
//{
//    vigor_info = m_prop_info.vigor_info;
//}

//==================================================
// 设置体力信息
// ==================================================
//void Player::SetVigorInfo(const VIGOR_INFO& vigor_info)
//{
//    m_prop_info.vigor_info = vigor_info;
//}

// 离线体力信息
//void Player::FillOffLineVigorInfo(OFF_LINE_VIGOR_INFO& offline_vigor_info)
//{
//    offline_vigor_info = m_prop_info.off_line_vigor_info;
//}
//
//void Player::SetOffLineVigorInfo(const OFF_LINE_VIGOR_INFO& offline_vigor_info)
//{
//    m_prop_info.off_line_vigor_info = offline_vigor_info;
//}

// ==================================================
// 添加好友
// ==================================================
bool Player::AddFriend(Player* player)
{
    FRIEND_LIST sendMsg;
    FRIEND_INFO info;
    uint64 friendID = 0;
    uint64 mineID = 0;
    
    if(!player) return false;

    for(std::vector<FRIEND_INFO>::iterator it = m_friend_mgr.list.begin();
            it != m_friend_mgr.list.end(); ++it)
    {
        friendID = U64ID(it->player_id_h, it->player_id_l);
        if(friendID == player->GetPlayerID())
        {
            return false;       // 已经有该好友了
        }
    }

    info.Clear();
    info.gatesvr_id = player->GetCentGateID();
    info.player_id_l = U64ID_LOW(player->GetPlayerID());
    info.player_id_h = U64ID_HIGH(player->GetPlayerID());
    info.level = player->GetLevel();
    info.name = player->GetName();
    info.sex = player->GetSex();
    info.logout_time = player->GetLogoutTime();
    info.head_portrait = player->GetHeadPortraitInfo();
    //info.career = player->GetCareer();
    //info.unionmap_id = player->GetUnionMapID();
    //info.battle_power = player->GetBattlePower();
    //info.vip_level = player->GetVipLevelByGrowthPoint();

    // 在Center上加入新角色
    m_friend_mgr.list.push_back(info);


    // 检查我要加的这个好友，是不是也加了我
    for(std::vector<FRIEND_INFO>::iterator it = player->m_friend_mgr.list.begin();
        it != player->m_friend_mgr.list.end(); ++it)
    {
        mineID = U64ID(it->player_id_h, it->player_id_l);
        if(mineID == GetPlayerID())
        {
            // 如果他也加了我为好友，就加入我是谁的好友列表
            m_friend_has_me.list.push_back(info);
        }
    }

    // 将新角色信息发送到
    sendMsg.Clear();
    sendMsg.opt_flag = OPT_ADD_FLAG;
    sendMsg.list.push_back(info);

    SendToGate(MSG_FRIEND_INFO_NTF, &sendMsg);      // 发送到Gate

    SendToGame(MSG_FRIEND_INFO_NTF, &sendMsg);      // 发送到Game

    return true;
}

// ==================================================
// 通知好友我的信息
// ==================================================
void Player::NotifyFriendMyInfo()
{
    // 信息
    FRIEND_INFO info;
    info.Clear();

    info.gatesvr_id = GetCentGateID();
    info.player_id_l = U64ID_LOW(GetPlayerID());
    info.player_id_h = U64ID_HIGH(GetPlayerID());
    info.name = GetName();
    info.sex = GetSex();
    info.level = GetLevel();
    info.logout_time = GetLogoutTime();
    info.head_portrait = GetHeadPortraitInfo();

    NofityFriendInfo(info);
}

void Player::NofityFriendInfo(const FRIEND_INFO& info)
{
    FRIEND_LIST sendMsg;
    Player* player = 0;
    uint64 friendID = 0;

    // 角色信息发送给好友
    sendMsg.Clear();
    sendMsg.opt_flag = OPT_UPDATE_FLAG;
    sendMsg.list.push_back(info);

    // m_friend_mgr
    // 把我的信息通知给把我加入好友的人
    for (std::vector<FRIEND_INFO>::iterator it = m_friend_has_me.list.begin();
        it != m_friend_has_me.list.end(); ++it)
    {
        friendID = U64ID(it->player_id_h, it->player_id_l);
        player = PlayerManager::Instance()->GetPlayerByPlayerID(friendID);
        CnAssert(player != this);
        if(player)
        {
            player->SendToGate(MSG_FRIEND_INFO_NTF, &sendMsg);
            player->SendToGame(MSG_FRIEND_INFO_NTF, &sendMsg);
        }
    }
}

// ==================================================
// 删除好友
// ==================================================
void Player::DelFriend(uint32 player_id_l, uint32 player_id_h)
{
    FRIEND_LIST sendMsg;
    FRIEND_INFO info;
    bool is_del = false;

    for(std::vector<FRIEND_INFO>::iterator it = m_friend_mgr.list.begin();
        it != m_friend_mgr.list.end(); ++it)
    {
        if((it->player_id_l == player_id_l) && (it->player_id_h == player_id_h))
        {
            // 删除好友
            m_friend_mgr.list.erase(it);
            is_del = true;

            // 检查我要删的这个好友，是不是也删除了我
            uint64 friendID = 0;
            Player* player = NULL;
            uint64 mineID = 0;
            //friendID = U64ID(it->player_id_h, it->player_id_l);
            friendID = U64ID(player_id_h, player_id_l);
            bool hasMe = false;

            player = PlayerManager::Instance()->GetPlayerByPlayerID(friendID);

            // 如果用户不在线,就中断执行
            if(!player)
            {
                 break;
            }

            for(std::vector<FRIEND_INFO>::iterator it_new = player->m_friend_mgr.list.begin();
                it_new != player->m_friend_mgr.list.end(); ++it_new)
            {
                mineID = U64ID(it_new->player_id_h, it_new->player_id_l);
                if(mineID == GetPlayerID())
                {
                    hasMe = true;
                    break;
                }
            }
            // 如果对方也没有我, 就把他从我是谁的好友列表中剔除
            if(!hasMe)
            {
                for(std::vector<FRIEND_INFO>::iterator Iter = m_friend_has_me.list.begin();
                    Iter != m_friend_has_me.list.end(); ++Iter)
                {
                    friendID = U64ID(Iter->player_id_h, Iter->player_id_l);
                    if(friendID == player->GetPlayerID())
                    {
                        m_friend_has_me.list.erase(Iter);
                        break;
                    }
                }
            }

            break;
        }
    }

    if(!is_del) return;

    // 在Center上删除的角色
    info.Clear();
    info.player_id_l = player_id_l;
    info.player_id_h = player_id_h;

    // 将删除的角色信息发送到
    sendMsg.Clear();
    sendMsg.opt_flag = OPT_DEL_FLAG;
    sendMsg.list.push_back(info);

    SendToGate(MSG_FRIEND_INFO_NTF, &sendMsg);      // 发送到Gate

    SendToGame(MSG_FRIEND_INFO_NTF, &sendMsg);      // 发送到Game
}

// ==================================================
// 添加机器人
// ==================================================
void Player::AddRobotFriend()
{
    FRIEND_LIST sendMsg;
    FRIEND_INFO info;

    for(std::vector<FRIEND_INFO>::iterator it = m_friend_mgr.list.begin();
        it != m_friend_mgr.list.end(); ++it)
    {
        if(it->name == ConfigLoader::Instance()->GetFriendRobotName())
        {
            return;
        }
    }

    info.Clear();
    info.gatesvr_id = 0;
    info.player_id_l = 1;
    info.player_id_h = 0;
    info.name = ConfigLoader::Instance()->GetFriendRobotName();
    info.sex = 1;
    info.level = 10;

    m_friend_mgr.list.push_back(info);

    // 将新角色信息发送到
    sendMsg.Clear();
    sendMsg.opt_flag = OPT_ADD_FLAG;
    sendMsg.list.push_back(info);

    SendToGate(MSG_FRIEND_INFO_NTF, &sendMsg);      // 发送到Gate

    SendToGame(MSG_FRIEND_INFO_NTF, &sendMsg);      // 发送到Game
}

// ==================================================
// 请求快速加好友列表
// ==================================================
void Player::QuickFriendList(bool robot)
{
    FRIEND_INFO friend_info;
    FRIEND_LIST sendMsg;
    std::list<Player*> quick_list;

    sendMsg.Clear();

    if(robot)
    {
        if(!HasFriend(ConfigLoader::Instance()->GetFriendRobotName()))
        {
            friend_info.Clear();
            friend_info.gatesvr_id = 0;
            friend_info.player_id_l = 1;
            friend_info.player_id_h = 0;
            friend_info.name = ConfigLoader::Instance()->GetFriendRobotName();
            friend_info.sex = 1;
            friend_info.level = 10;
            sendMsg.list.push_back(friend_info);
        }
    }
    else
    {
        PlayerManager::Instance()->GetQuickFriendList(quick_list, this);
        for(std::list<Player*>::iterator it = quick_list.begin(); it != quick_list.end(); ++it)
        {
            friend_info.Clear();
            friend_info.gatesvr_id = (*it)->GetCentGateID();
            friend_info.player_id_l = U64ID_LOW( (*it)->GetPlayerID() );
            friend_info.player_id_h = U64ID_HIGH( (*it)->GetPlayerID() );
            friend_info.name = (*it)->GetName();
            friend_info.sex = (*it)->GetSex();
            friend_info.level = (*it)->GetLevel();
            sendMsg.list.push_back(friend_info);
        }
    }

    SendToGate(MSG_FRIEND_QUICK_ADD_ACK, &sendMsg);
}

// ==================================================
// 是否已在好留列表中
// ==================================================
bool Player::HasFriend(std::string name)
{
    for(std::vector<FRIEND_INFO>::iterator it = m_friend_mgr.list.begin();
            it != m_friend_mgr.list.end(); ++it)
    {
        if(name == it->name)
        {
            return true;
        }
    }
    return false;
}

// ==================================================
// 获取邮件
// ==================================================
MAIL_INFO* Player::GetMail(uint32 mail_id_h, uint32 mail_id_l)
{
    for (std::vector<MAIL_INFO>::iterator it = m_mail_info.player_mail_list.begin(); 
        it != m_mail_info.player_mail_list.end(); ++it)
    {
        if((it->mail_id.id_h == mail_id_h) && (it->mail_id.id_l == mail_id_l))
        {
            return &(*it);
        }
    }

    for (std::vector<MAIL_INFO>::iterator it = m_mail_info.system_mail_list.begin(); 
        it != m_mail_info.system_mail_list.end(); ++it)
    {
        if((it->mail_id.id_h == mail_id_h) && (it->mail_id.id_l == mail_id_l))
        {
            return &(*it);
        }
    }

    return 0;
}

// ==================================================
// 阅读邮件
// ==================================================
void Player::ReadMail(uint8 mail_type, uint32 mail_id_h, uint32 mail_id_l, uint8 attach_flag)
{
    uint64 mailID = 0;

    if((mail_type == MAIL_TYPE_PLAYER) || (mail_type == MAIL_TYPE_PLAYER_SYS))
    {
        for(std::vector<MAIL_INFO>::iterator mailIt=m_mail_info.player_mail_list.begin();
            mailIt != m_mail_info.player_mail_list.end(); ++mailIt)
        {
            if((mailIt->mail_id.id_h == mail_id_h) && (mailIt->mail_id.id_l == mail_id_l))
            {
                mailIt->mail_flag = MAIL_FLAG_READED;

                mailID = U64ID(mail_id_h, mail_id_l);
                MailJob::Instance()->SetPlayerMailFlag(GetPlayerID(), mailID, MAIL_FLAG_READED);
                return;
            }
        }
    }
    else if(mail_type == MAIL_TYPE_SYSTEM)
    {
        for(std::vector<MAIL_INFO>::iterator mailIt=m_mail_info.system_mail_list.begin();
            mailIt != m_mail_info.system_mail_list.end(); ++mailIt)
        {
            if((mailIt->mail_id.id_h == mail_id_h) && (mailIt->mail_id.id_l == mail_id_l))
            {
                mailIt->mail_flag = MAIL_FLAG_READED;

                mailID = U64ID(mail_id_h, mail_id_l);
                MailJob::Instance()->SetSystemMailFlag(GetPlayerID(), mailID, MAIL_FLAG_READED, mailIt->attachment_flag);
                return;
            }
        }
    }
    else if(mail_type == MAIL_TYPE_GROUP)
    {
        for(std::vector<MAIL_INFO>::iterator mailIt=m_mail_info.system_mail_list.begin();
            mailIt != m_mail_info.system_mail_list.end(); ++mailIt)
        {
            if((mailIt->mail_id.id_h == mail_id_h) && (mailIt->mail_id.id_l == mail_id_l))
            {
                mailIt->mail_flag = MAIL_FLAG_READED;

                mailID = U64ID(mail_id_h, mail_id_l);
                MailJob::Instance()->SetGroupMailFlag(GetPlayerID(), mailID, MAIL_FLAG_READED, mailIt->attachment_flag);
                return;
            }
        }
    }
}

// ==================================================
// 添加邮件
// ==================================================
void Player::AddMail(MAIL_INFO& mail_info)
{
    if((mail_info.mail_type == MAIL_TYPE_PLAYER) || (mail_info.mail_type == MAIL_TYPE_PLAYER_SYS))
    {
        m_mail_info.player_mail_list.push_back(mail_info);
    }
    else if((mail_info.mail_type == MAIL_TYPE_SYSTEM) || (mail_info.mail_type == MAIL_TYPE_GROUP))
    {
        m_mail_info.system_mail_list.push_back(mail_info);
    }
}

// ==================================================
// 删除邮件
// ==================================================
void Player::DelMail(uint8 mail_type, uint32 mail_id_h, uint32 mail_id_l, uint8 attach_flag)
{
    uint64 mailID = 0;

    if((mail_type == MAIL_TYPE_PLAYER) || (mail_type == MAIL_TYPE_PLAYER_SYS))
    {
        for(std::vector<MAIL_INFO>::iterator mailIt=m_mail_info.player_mail_list.begin();
            mailIt != m_mail_info.player_mail_list.end(); ++mailIt)
        {
            if((mailIt->mail_id.id_h == mail_id_h) && (mailIt->mail_id.id_l == mail_id_l))
            {
                mailID = U64ID(mail_id_h, mail_id_l);
                MailJob::Instance()->SetPlayerMailFlag(GetPlayerID(), mailID, MAIL_FLAG_DELETED);

                m_mail_info.player_mail_list.erase(mailIt);
                return;
            }
        }
    }
    else if(mail_type == MAIL_TYPE_SYSTEM)
    {
        for(std::vector<MAIL_INFO>::iterator mailIt=m_mail_info.system_mail_list.begin();
            mailIt != m_mail_info.system_mail_list.end(); ++mailIt)
        {
            if((mailIt->mail_id.id_h == mail_id_h) && (mailIt->mail_id.id_l == mail_id_l))
            {
                mailID = U64ID(mail_id_h, mail_id_l);
                MailJob::Instance()->SetSystemMailFlag(GetPlayerID(), mailID, MAIL_FLAG_DELETED,  mailIt->attachment_flag);

                m_mail_info.system_mail_list.erase(mailIt);
                return;
            }
        }
    }
    else if(mail_type == MAIL_TYPE_GROUP)
    {
        for(std::vector<MAIL_INFO>::iterator mailIt=m_mail_info.system_mail_list.begin();
            mailIt != m_mail_info.system_mail_list.end(); ++mailIt)
        {
            if((mailIt->mail_id.id_h == mail_id_h) && (mailIt->mail_id.id_l == mail_id_l))
            {
                mailID = U64ID(mail_id_h, mail_id_l);
                MailJob::Instance()->SetGroupMailFlag(GetPlayerID(), mailID, MAIL_FLAG_DELETED,  mailIt->attachment_flag);

                m_mail_info.system_mail_list.erase(mailIt);
                return;
            }
        }
    }
}

// ==================================================
// 删除过期邮件
// ==================================================
void Player::DelExpiredMail(uint32 mail_id_h)
{
    for(std::vector<MAIL_INFO>::iterator mailIt=m_mail_info.player_mail_list.begin();
        mailIt != m_mail_info.player_mail_list.end();)
    {
        if(mailIt->mail_id.id_h <= mail_id_h)
        {
            mailIt = m_mail_info.player_mail_list.erase(mailIt);
        }
        else
        {
            ++mailIt;
        }
    }

    for(std::vector<MAIL_INFO>::iterator mailIt=m_mail_info.system_mail_list.begin();
        mailIt != m_mail_info.system_mail_list.end(); )
    {
        if(mailIt->mail_id.id_h <= mail_id_h)
        {
            mailIt = m_mail_info.system_mail_list.erase(mailIt);
        }
        else
        {
            ++mailIt;
        }
    }
}

// ==================================================
// 发送添加好友应答
// ==================================================
void Player::SendFriendAddAck(uint16 errcode, const std::string& src_name, const std::string& dst_name)
{
    FRIEND_ADD_ACK sendMsg;
    sendMsg.errcode = errcode;
    sendMsg.src_name = src_name;
    sendMsg.dst_name = dst_name;
    SendToGate(MSG_FRIEND_ADD_ACK, &sendMsg);
}


// ==================================================
// 发送队友重置状态消息(此功能好像没有什么作用，但还是先留着)
// ==================================================
void Player::SendTeamMemberResetState()
{
    NONE sendMsg;
    SendToGame(MSG_TEAM_MEMBER_RESET_STATE, &sendMsg);
}

// ==================================================
// 商城购买错误通知
// ==================================================
void Player::SendMallBuyNtf(uint16 errCode)
{
    ERROR_CODE sendMsg;
    sendMsg.errcode = errCode;
    SendToGate(MSG_MALL_BUY_NTF, &sendMsg);
}

// ==================================================
// 通知用户属性
// ==================================================
void Player::NotifyGamePlayerProp(uint8 key, int32 val)
{
    PROPERTY_NTF proerti_ntf;
    proerti_ntf.property_id = key;
    proerti_ntf.value = val;
    proerti_ntf.str = "";

    PROPERTY_LIST_NTF sendMsg;
    sendMsg.list.push_back(proerti_ntf);
    SendToGame(MSG_PLAYER_PROP_LIST, &sendMsg);
}

// ==================================================
// 通知用户属性
// ==================================================
void Player::NotifyGatePlayerProp(uint8 key, int32 val)
{
    PROPERTY_NTF proerti_ntf;
    proerti_ntf.property_id = key;
    proerti_ntf.value = val;
    proerti_ntf.str = "";

    PROPERTY_LIST_NTF sendMsg;
    sendMsg.list.push_back(proerti_ntf);

    SendToGate(MSG_PLAYER_PROP_LIST, &sendMsg);
}

// ==================================================
// 发送错误码
// ==================================================
void Player::NotifyErrorCode(uint16 errCode)
{
    ERROR_CODE sendMsg;
    sendMsg.errcode = errCode;
    SendToGate(MSG_ERROR_CODE, &sendMsg);
}

// ==================================================
// 发送数据
// ==================================================
void Player::SendToGate(uint32 msgID, void* data, uint32 gateId)
{
    uint64 playerID = 0;
    if (gateId <= 0)
    {
        gateId = GetCentGateID();
    }

    playerID = GetPlayerID();
    if(playerID <= 0)
    {
        playerID = GetLoginID();
    }

    GateHandler::Instance()->SendByID(gateId, msgID, data, playerID);
}

// ==================================================
// 发送数据
// ==================================================
void Player::SendToGame(uint32 msgID, void* data)
{
    GameHandler::Instance()->SendByID(GetCentGameID(), msgID, data, GetPlayerID());
}

// ==================================================
// 填充赛马信息
// ==================================================
void Player::FillRacingInfo(RACING_INFO& racing_info)
{
    racing_info = m_racing_info;
}

// ==================================================
// 设置赛马信息
// ==================================================
void Player::SetRacingInfo(const RACING_INFO& racing_info)
{
    m_racing_info = racing_info;
}
// ==================================================
// 加载采矿信息
// ==================================================
void Player::FillMiningInfo(MINING_INFO& mining_info)
{
    mining_info = m_mining_info;
}

// ==================================================
// 设置采矿信息
// ==================================================
void Player::SetMiningInfo(const MINING_INFO& mining_info)
{
    m_mining_info = mining_info;
}

// ==================================================
// 加载宝库信息
// ==================================================
void Player::FillTreasuryInfo(TREASURY_INFO& treasury_info)
{
    treasury_info = m_treasury_info;
}

// ==================================================
// 设置宝库信息
// ==================================================
void Player::SetTreasuryInfo(const TREASURY_INFO& treasury_info)
{
    m_treasury_info = treasury_info;
}

// ==================================================
// 加载金像信息
// ==================================================
void Player::FillGoldtreeInfo(GOLDTREE_INFO& goldtree_info)
{
    goldtree_info = m_goldtree_info;
}

// ==================================================
// 填充金像信息
// ==================================================
void Player::SetGoldtreeInfo(const GOLDTREE_INFO& goldtree_info)
{
    m_goldtree_info = goldtree_info;
}

// ==================================================
// 填充大冒险信息
// ==================================================
void Player::FillAdventureInfo(ADVENTURE_INFO& adventure_info)
{
    adventure_info = m_adventure_info;
}

// ==================================================
// 设置大冒险信息
// ==================================================
void Player::SetAdventureInfo(const ADVENTURE_INFO& adventure_info)
{
    m_adventure_info = adventure_info;
}

// ==================================================
// 填充答题信息
// ==================================================
void Player::FillAnswerInfo(ANSWER_INFO& answer_info)
{
    answer_info = m_answer_info;
}

// ==================================================
// 设置答题信息
// ==================================================
void Player::SetAnswerInfo(const ANSWER_INFO& answer_info)
{
    m_answer_info = answer_info;
}

// ==================================================
// 填充翅膀信息
// ==================================================
void Player::FillWingInfo(WING_INFO& wing_info)
{
    wing_info = m_wing_info;
}

// ==================================================
// 设置翅膀信息
// ==================================================
void Player::SetWingeInfo(const WING_INFO& wing_info)
{
    m_wing_info = wing_info;
}

// ==================================================
// 填充修罗场信息
// ==================================================
void Player::FillAshuraInfo(ASHURA_INFO& ashura_info)
{
    ashura_info = m_ashura_info;
}

// ==================================================
// 设置修罗场信息
// ==================================================
void Player::SetAshuraInfo(const ASHURA_INFO& ashura_info)
{
    m_ashura_info = ashura_info;
}

// ==================================================
// 填充称号信息
// ==================================================
void Player::SetTitleInfo(const TITLE_LIST& titlelist)
{
    m_title_list = titlelist;
}

// ==================================================
// 填充称号信息
// ==================================================
void Player::FillTitleInfo(TITLE_LIST& titlelist)
{
    titlelist = m_title_list;
}

// ==================================================
// 设置充值信息
// ==================================================
void Player::SetRechargeInfo(PLAYER_RECHARGE_INFO& recharege_info)
{
    m_recharge_info = recharege_info;
}

// ==================================================
// 填充充值信息
// ==================================================
void Player::FillRechargeInfo(PLAYER_RECHARGE_INFO& recharege_info)
{
    recharege_info = m_recharge_info;
}

// ==================================================
// 加载商城信息
// ==================================================
void Player::SetMallInfo(const RESTRICTION_LIST& mall_list)
{
    m_mall_list = mall_list;
}

// ==================================================
// 填充商城信息
// ==================================================
void Player::FillMallInfo(RESTRICTION_LIST& mall_list)
{
    mall_list = m_mall_list;
}

// ==================================================
// 加载日常投资信息
// ==================================================
void Player::SetDayInvestmentInfo( const DAY_INVESTMENT_INFO& day_investment_info )
{
    m_day_investment_info = day_investment_info;
}

// ==================================================
// 填充日常投资信息
void Player::FillDayInvestmentInfo( DAY_INVESTMENT_INFO& day_investment_info )
{
    day_investment_info = m_day_investment_info;
}

// ==================================================
// 加载等级投资信息
// ==================================================
void Player::SetLevelInvestmentInfo( const LEVEL_INVESTMENT_INFO& level_investment_info )
{
    m_level_investment_info = level_investment_info;
}

// ==================================================
// 填充等级投资信息
void Player::FillLevelInvestmentInfo( LEVEL_INVESTMENT_INFO& level_investment_info )
{
    level_investment_info = m_level_investment_info;
}


// 设置被动技能信息
// ==================================================
void Player::SetPassiveSkill(const PASSIVE_SKILL_LIST& passive_skill_list)
{
    m_passive_skill_list = passive_skill_list;
}

// ==================================================
// 填充被动技能信息
// ==================================================
void Player::FillPassiveSkill(PASSIVE_SKILL_LIST& passive_skill_list)
{
    passive_skill_list = m_passive_skill_list;
}

// ==================================================
// 设置自动战斗设置信息
// ==================================================
void Player::SetAutoFightSettingInfo(const AUTO_FIGHT_SETTING_INFO& auto_fight_setting_info)
{
    m_auto_fight_setting_info = auto_fight_setting_info;
}

// ==================================================
// 填充自动战斗设置信息
// ==================================================
void Player::FillAutoFightSettingInfo(AUTO_FIGHT_SETTING_INFO& auto_fight_setting_info)
{
     auto_fight_setting_info = m_auto_fight_setting_info;
}

// ==================================================
// 设置系统设置信息
// ==================================================
void Player::SetSystemSettingInfo(const SYSTEM_SETTING_INFO& system_setting_info)
{
    m_system_setting_info = system_setting_info;
}

// ==================================================
// 填充系统设置信息
// ==================================================
void Player::FillSystemSettingInfo(SYSTEM_SETTING_INFO& system_setting_info)
{
    system_setting_info = m_system_setting_info;
}

// ==================================================
// 设置玩家设置信息
// ==================================================
void Player::SetPlayerSettingInfo(const PLAYER_SETTING_INFO& player_setting_info)
{
    m_player_setting_info = player_setting_info;
}

// ==================================================
// 填充玩家设置信息
// ==================================================
void Player::FillPlayerSettingInfo(PLAYER_SETTING_INFO& player_setting_info)
{
    player_setting_info = m_player_setting_info;
}

// ==================================================
// 设置自动购买设置信息
// ==================================================
void Player::SetAutobuySettingInfo(const AUTOBUY_SETTING_INFO& autobuy_setting_info)
{
    m_autobuy_setting_info = autobuy_setting_info;
}

// ==================================================
// 填充自动购买设置信息
// ==================================================
void Player::FillAutobuySettingInfo(AUTOBUY_SETTING_INFO& autobuy_setting_info)
{
    autobuy_setting_info = m_autobuy_setting_info;
}

// ==================================================
// 设置虚空挂机设置信息
// ==================================================
void Player::SetVoidFieldSettingInfo(const VOID_FIELD_SETTING_INFO& void_field_setting_info)
{
    m_voidfield_setting_info = void_field_setting_info;
}

// ==================================================
// 填充虚空挂机设置信息
// ==================================================
void Player::FillVoidFieldSettingInfo(VOID_FIELD_SETTING_INFO& void_field_setting_info)
{
    void_field_setting_info = m_voidfield_setting_info;
}

void Player::SetLevelActivityInfo( const LEVEL_ACTIVITY_INFO& level_activity_info )
{
    m_level_activity_info = level_activity_info;
}

void Player::FillLevelActivityInfo( LEVEL_ACTIVITY_INFO& level_activity_info )
{
    level_activity_info = m_level_activity_info;
}

void Player::SetLivenessInfo( const LIVENESS_INFO& liveness_info )
{
    m_liveness_info = liveness_info;
}

void Player::FillLivenessInfo( LIVENESS_INFO& liveness_info )
{
    liveness_info = m_liveness_info;
}

void Player::SetDungeonTimesInfo(const DUNGEON_TIMES_LIST& dungeon_times_info)
{
    m_dungeon_times_info = dungeon_times_info;
}

void Player::FillDungeonTimesInfo(DUNGEON_TIMES_LIST& dungeon_times_info)
{
    dungeon_times_info = m_dungeon_times_info;
}

void Player::SetLimitBuyInfo(const LIMIT_BUY_LIST& limibuylist)
{
    m_limitbuy_info = limibuylist;
}

void Player::FillLimitBuyInfo(LIMIT_BUY_LIST& limibuylist)
{
    limibuylist = m_limitbuy_info;
}

void Player::QuicklyAutoSave()
{
    m_auto_save_timer += ConfigLoader::Instance()->GetAutoSavePeriod() * 1000 / 3;      // 增加三分之一个周期
}

void Player::SetSingleAnswerInfo(const SINGLE_ANSWER_INFO& single_answer_info)
{
    m_single_answer_info = single_answer_info;
}
void Player::FillSingleAnswerInfo(SINGLE_ANSWER_INFO& single_answer_info)
{
    single_answer_info = m_single_answer_info;
}

void Player::SetZombieInfo(const ZOMBIE_INFO& zombie_info)
{
    m_zombie_info = zombie_info;
}

void Player::FillZombieInfo(ZOMBIE_INFO& zombie_info)
{
    zombie_info = m_zombie_info;
}

ZOMBIE_INFO& Player::GetZombieInfo()
{
    return m_zombie_info;
}

void Player::SetGiftCodeInfo(const GIFT_CODE_INFO& info)
{
    m_gift_code_info = info;
}

void Player::FillGiftCodeInfo(GIFT_CODE_INFO& info)
{
    info = m_gift_code_info;
}


// ==================================================
// 返回成长点对应的VIP等级，不考虑VIP时效
// ==================================================
int32 Player::GetVipLevelByGrowthPoint()
{
    //int32 vipLevel = VipConfig::Instance()->GetVipLevel(GetGrowthPoint() + m_prop_info.vip_info.growth_point_no_recharge);

    //if (vipLevel < 0)
    //{
    //    vipLevel = 0;
    //    CnAssert(false);
    //    CnWarn("VIP level cannot be negative! \n");
    //}

    //return vipLevel;
    return 0;
}

void Player::FillPetFormation(PET_FORMATION_INFO& info)
{
    info = m_pet_formation;
}

void Player::SetPetFormation(const PET_FORMATION_INFO& info)
{
    m_pet_formation = info;
}

// ==================================================
// 返回圣剑信息
// ==================================================
void Player::FillSwordInfo(SWORD_NEW_INFO& info)
{
    info = m_sword_info;
}

// ==================================================
// 设置圣剑信息
// ==================================================
void Player::SetSwordInfo(const SWORD_NEW_INFO& info)
{
    m_sword_info = info;
}

// ==================================================
// 设置公会信息
// ==================================================
void Player::SetGuildScienceInfo(const GUILD_PLAYER_SCIENCE_INFO& info)
{
    m_guild_player_science_info = info;
}

// ==================================================
// 填充公会信息
// ==================================================
void Player::FillGuildScienceInfo(GUILD_PLAYER_SCIENCE_INFO& info)
{
    info = m_guild_player_science_info;
}

// ==================================================
// 填充游戏收藏信息
// ==================================================
void Player::FillShorcutInfo(SHORCUT_BAG& shorcut_bag)
{
    shorcut_bag = m_shorcut_info;
}

// ==================================================
// 设置游戏收藏信息
// ==================================================
void Player::SetShorcutInfo(const SHORCUT_BAG& shorcut_bag)
{
    m_shorcut_info = shorcut_bag;
}

void Player::FillMultiserverBattleInfo( MULTISERVER_BATTLE_BAG& info )
{
    info = m_MultiserverBattleInfo;
}

void Player::SetMultiserverBattleInfo( const MULTISERVER_BATTLE_BAG& info )
{
    m_MultiserverBattleInfo = info;
}

void Player::FillInvestmentPlanInfo(INVESTMENT_PLAN_INFO& info)
{
    info = m_investment_plan_info;
}

void Player::SetInvestmentPlanInfo(const INVESTMENT_PLAN_INFO& info)
{
    m_investment_plan_info = info;
}

void Player::FillDonateInfo(DONATE_INFO& info)
{
    info = m_donate_info;
}

void Player::SetDonateInfo(const DONATE_INFO& info )
{
    m_donate_info = info;
}

void Player::FillEggInfo( EGG_ACTIVITY_INFO& info )
{
    info = m_egg_info;
}

void Player::SetEggInfo( const EGG_ACTIVITY_INFO& info )
{
    m_egg_info = info;
}

void Player::FillBuyCheapInfo(BUY_CHEAP_INFO& info )
{
    info = m_buy_cheap_info;
}

void Player::SetBuyCheapInfo( const BUY_CHEAP_INFO& info )
{
    m_buy_cheap_info = info;
}

void Player::FillHeadPortraitInfo(HEAD_PORTRAIT_INFO& info)
{
    info = m_head_portrait_info;
}

void Player::SetHeadPortraitInfo( const HEAD_PORTRAIT_INFO& info )
{
    m_head_portrait_info = info;
}