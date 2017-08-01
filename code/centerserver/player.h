/*
*	玩家角色对象
*/
#ifndef player_h__
#define player_h__

class PlayerNetHandler;


class Player
{
public:
    Player();
    ~Player();

    PlayerNetHandler*   GetNetHandler() {return m_pNetHandler;}

    void OnCreate();                                        // 角色数据加载完成,创建完成
    void OnDestory();                                       // 角色销毁之前

    void Update();
    void Clear();                                           // 清空

    void FillPropInfo(PROP_BASE_INFO& prop_info);           // 填充属性
    PROPERTY_INFO& GetPropInfo() { return m_prop_info; }    // 获得角色信息

    PLAYER_DUNGEON_INFO& GetPlayerDungeonInfo();                    // 获得角色副本信息
    void FillPlayerDungeonInfo(PLAYER_DUNGEON_INFO& info);          // 填充角色副本信息
    void SetPlayerDungeonInfo(const PLAYER_DUNGEON_INFO& info);     // 设置角色副本信息

    // 状态信息
    void FillStateInfo(PLAYER_STATE_INFO& state_info);
    void SetStateInfo(const PLAYER_STATE_INFO& state_info);
    PLAYER_STATE_INFO&  GetStateInfo() {return m_state_info;} 

    // 装备
    void FillEquipInfo(EQUIP_INFO& equip_info);                                       // 填充装备
    void SetEquipInfo(const EQUIP_INFO& equip_info);                                  // 设置装备
    EQUIP_INFO& GetEquipInfo() { return m_equip_info; }                               // 获得装备信息

    // 药剂信息
    void FillPotionInfo(POTION_INFO& potion_info) const;
    void SetPotionInfo(const POTION_INFO& potion_info);
    POTION_INFO& GetPotionInfo() {return m_potion_info;}

    //跟宠物
    void FillPetInfo(PET_INFO& pet_info);                                           // 填充宠物
    void SetPetInfo(const PET_INFO& pet_info);                                      // 设置宠物
    PET_INFO& GetPetInfo() { return m_pet_info; }                                   // 获得宠物信息

    //骑宠物
    void FillRidePetInfo(RIDE_INFO& ride_info);                                     // 填充宠物
    void SetRidePetInfo(const RIDE_INFO& ride_info);                                // 设置宠物
    RIDE_INFO& GetRideInfo() {return m_rideinfo;}                                   // 获取跟宠信息

    // 包裹
    void FillItemInfo(ITEM_INFO& item_info);                                           ///< 填充物品
    void SetItemInfo(const ITEM_INFO& item_info);                                      ///< 设置物品
    ITEM_INFO& GetItemInfo() { return m_item_info; }                                   ///< 获得物品信息

    // 仓库
    ITEM_INFO& GetStoreInfo() { return m_store_info; }                                     ///< 获得仓库信息
    void SetStoreInfo(const ITEM_INFO& store_info);                                        ///< 设置仓库
    void FillStoreInfo(ITEM_INFO& store_info);                                               ///< 填充仓库

    // 抽奖信息
    void FillRandomPackageInfo(RANDOM_PACKAGE_RECORD_INFO& info);                                           ///< 填充抽奖
    void SetRandomPackageInfo(const RANDOM_PACKAGE_RECORD_INFO& info);                                      ///< 设置抽奖
    RANDOM_PACKAGE_RECORD_INFO& GetRandomPackageInfo() { return m_package_record; }                                   ///< 获得抽奖信息

    // 隐藏物品
    //void FillHideBagInfo(HIDE_BAG_INFO& hide_bag_info);                             ///< 隐藏物品
    //void SetHideBagInfo(const HIDE_BAG_INFO& hide_bag_info);                        ///< 设置隐藏物品
    //HIDE_BAG_INFO&  GetHideBagInfo() {return m_hide_bag_info; }

    // 好友
    void FillFriendInfo(FRIEND_LIST& friend_list);                                  ///< 填充好友
    FRIEND_LIST&  GetFriendInfo() { return m_friend_mgr; }                          ///< 获得好友信息
    FRIEND_LIST&  GetFriendHasMeInfo() { return m_friend_has_me; }                          ///< 获得好友信息

    // 善恶信息
    void FillEnemyInfo(ENEMY_INFO& enemy_info);                                     ///< 填充善恶信息
    void SetEnemyInfo(const ENEMY_INFO& enemy_info);                                ///< 设置善恶信息
    ENEMY_INFO&  GetEnemyInfo() { return m_enemy_info; }                            ///< 获得善恶信息

    // 任务
    void FillTaskInfo(TASK_INFO& task_info);                                        // 填充任务
    void SetTaskInfo(const TASK_INFO& task_info);                                   // 设置任务
    TASK_INFO& GetTaskInfo() { return m_task_info; }                                // 获得任务信息

    // 引导
    void FillGuideInfo(GUIDE_INFO& guide_info) const;                               // 填充引导
    void SetGuideInfo(const GUIDE_INFO& guide_info);                                // 设置引导
    GUIDE_INFO& GetGuideInfo() { return m_guide_info; }

    // 功能开启
    void FillOpenSystemInfo(OPEN_SYSTEM_INFO& info) const;                          // 填充功能开启
    void SetOpenSystemInfo(const OPEN_SYSTEM_INFO& info);                           // 设置功能开启
    OPEN_SYSTEM_INFO& GetOpenSystemInfo() {return m_open_sysetm_info;}

    // 奖励池
    void FillRewardInfo(REWARD_INFO& reward_info);                                  ///< 填充奖励数据
    void SetRewardInfo(const REWARD_INFO& reward_info);                             ///< 设置奖励池数据
    REWARD_INFO& GetRewardInfo() { return m_reward_info; }                          ///< 获得奖励池数据

    // 通天塔
    void FillBabelInfo(BABEL_INFO& babel_info);                                     ///< 填充通天塔数据
    void SetBabelInfo(const BABEL_INFO& babel_info);                                ///< 设置通天塔数据
    BABEL_INFO& GetBabelInfo() { return m_babel_info; }                             ///< 获得通天塔数据

    //充值积分抽奖
    void FillPayPointLotteryInfo(PAY_POINT_LOTTERY_INFO &pay_point_lottery_info);           ///< 填充充值积分抽奖数据
    void SetPayPointLotteryInfo(const PAY_POINT_LOTTERY_INFO &pay_point_lottery_info);      ///< 设置充值积分抽奖数据
    PAY_POINT_LOTTERY_INFO& GetPayPointLotteryInfo() { return m_pay_point_lottery_info; }   ///< 获得充值积分抽奖数据

    //VIP独享副本
    void FillVIPOnlyDungeonInfo(VIP_ONLY_DUNGEON_INFO &vip_only_dungeon_info);              //< 填充VIP独享副本数据
    void SetVIPOnlyDungeonInfo(const VIP_ONLY_DUNGEON_INFO &vip_only_dungeon_info);         //< 设置VIP独享副本数据
    VIP_ONLY_DUNGEON_INFO& GetVIPOnlyDungeonInfo() { return m_vip_only_dungeon_info; }      //< 获得VIP独享副本数据

    // 怪物之门
    void FillMonsterDoorInfo(MONSTER_DOOR_INFO& monster_door_info);                 ///< 怪物之门信息
    void SetMonsterDoorInfo(const MONSTER_DOOR_INFO& monster_door_info);            ///< 怪物之门信息
    MONSTER_DOOR_INFO& GetMonsterDoorInfo() { return m_monster_door_info; }         ///< 获得怪物之门战斗信息

    // 勇者争霸
    void FillPlayerDoorInfo(PLAYER_DOOR_INFO& player_door_info);                    ///< 勇者争霸信息
    void SetPlayerDoorInfo(const PLAYER_DOOR_INFO& player_door_info);               ///< 勇者争霸信息
    PLAYER_DOOR_INFO& GetPlayerDoorInfo() {return m_player_door_info;}              ///< 获得勇者争霸战斗信息

    // 邮件
    void FillMailSystemInfo(MAIL_SYSTEM_INFO& mail_info);                           ///< 邮件系统信息
    void SetMailSystemInfo(const MAIL_SYSTEM_INFO& mail_system_info);               ///< 复制邮件系统信息
    MAIL_SYSTEM_INFO& GetMailSysInfo() { return m_mail_info; }                      ///< 邮件系统信息

    // 星空图
    void FillHoroscopeInfo(HOROSCOPE_INFO& horoscope_info);                         ///< 星空图信息
    void SetHoroscopeInfo(const HOROSCOPE_INFO& horoscope_info);                    ///< 设置星空图信息
    HOROSCOPE_INFO& GetHoroscopeInfo() { return m_horoscope_info; }                 ///< 获得星宫图信息

    // 技能
    void FillSkillInfo(SKILL_COMPONENT_INFO& skill_info);                                     ///< 填充技能信息
    void SetSkillInfo(const SKILL_COMPONENT_INFO& skill_info);                                ///< 设置技能信息
    SKILL_COMPONENT_INFO& GetSkillInfo() { return m_skill_info; }                             ///< 获得技能信息

    // 签到
    void FillSignInfo(SIGN_INFO& sign_info);                                        ///< 填充签到信息
    void SetSignInfo(const SIGN_INFO& sign_info);                                   ///< 设置签到信息
    SIGN_INFO& GetSignInfo() { return m_sign_info; }                                ///< 获得签到信息

    //void FillGuildWelfareInfo(GUILD_WELFARE_INFO& guild_welfare_info);                      // 填充公会福利信息
    //void SetGuildWelfareInfo(const GUILD_WELFARE_INFO& guild_welfare_info);                 // 设置公会福利信息
    //GUILD_WELFARE_INFO& GetGuildWelfareInfo() { GUILD_WELFARE_INFO info; return info; }     // 获得公会福利信息

    //void FillCityFightInfo(CITY_FIGHT_PLAYER_INFO& city_fight_info);                        ///< 填充城市争夺战信息
    //void SetCityFightInfo(const CITY_FIGHT_PLAYER_INFO& city_fight_info);                   ///< 设置城市争夺战信息
    //CITY_FIGHT_PLAYER_INFO& GetCityFightInfo() { return m_prop_info.city_fight_info; }      ///< 获得城市争夺战信息

    // 防沉迷信息
    //void FillFcmInfo(FCM_INFO& fcm_info);                                                   ///< 填充防沉迷信息
    //void SetFcmInfo(const FCM_INFO& fcm_info);                                              ///< 设置防沉迷信息
    //FCM_INFO& GetFcmInfo() { return m_prop_info.fcm_info; }                                 ///< 获得防沉迷信息

    // 体力信息
    //void FillVigorInfo(VIGOR_INFO& vigor_info);                                             ///< 填充体力信息
    //void SetVigorInfo(const VIGOR_INFO& vigor_info);                                        ///< 设置体力信息
    //VIGOR_INFO& GetVigorInfo() { return m_prop_info.vigor_info; }                           ///< 获得体力信息


    ////离线体力
    //void FillOffLineVigorInfo(OFF_LINE_VIGOR_INFO& offline_vigor_info);                   ///< 加载离线体力信息
    //void SetOffLineVigorInfo(const OFF_LINE_VIGOR_INFO& offline_vigor_info);              ///< 填充离线体力信息
    //OFF_LINE_VIGOR_INFO& GetOffLineVigorManager() {return m_prop_info.off_line_vigor_info;}///<.离线体力 

    // 在线奖励
    void FillOnlineRewardInfo(ONLINE_REWARD_INFO& online_reward_info);                      ///< 填充在线奖励信息
    void SetOnlineRewardInfo(const ONLINE_REWARD_INFO& online_reward_info);                 ///< 设置在线奖励信息
    ONLINE_REWARD_INFO& GetOnlineRewardInfo() { return m_online_reward_info; }              ///< 获得在线奖励信息

    // 赛马
    void FillRacingInfo(RACING_INFO& racing_info);                                          ///< 填充赛马信息
    void SetRacingInfo(const RACING_INFO& racing_info);                                     ///< 设置赛马信息
    RACING_INFO& GetRacingInfo() { return m_racing_info; }                                  ///< 获得赛马信息
    // 采矿
    void FillMiningInfo(MINING_INFO& mining_info);                                          ///< 填充采矿信息
    void SetMiningInfo(const MINING_INFO& mining_info);                                     ///< 设置采矿信息
    MINING_INFO& GetMiningInfo() { return m_mining_info; }                                  ///< 获得采矿信息
    // 宝库
    void FillTreasuryInfo(TREASURY_INFO& treasury_info);                                    ///< 填充宝库信息
    void SetTreasuryInfo(const TREASURY_INFO& treasury_info);                               ///< 设置宝库信息
    TREASURY_INFO& GetTreasuryInfo() { return m_treasury_info; }                            ///< 获得宝库信息
    // 金像
    void FillGoldtreeInfo(GOLDTREE_INFO& goldtree_info);                                    ///< 填充金像信息
    void SetGoldtreeInfo(const GOLDTREE_INFO& goldtree_info);                               ///< 设置金像信息
    GOLDTREE_INFO& GetGoldtreeInfo() { return m_goldtree_info; }                            ///< 获得金像信息
    // 大冒险
    void FillAdventureInfo(ADVENTURE_INFO& adventure_info);                                 ///< 填充大冒险信息
    void SetAdventureInfo(const ADVENTURE_INFO& adventure_info);                            ///< 设置大冒险信息
    ADVENTURE_INFO& GetAdventureInfo() { return m_adventure_info; }                         ///< 获得大冒险信息
    // 答题信息
    void FillAnswerInfo(ANSWER_INFO& answer_info);                                          ///< 填充答题信息
    void SetAnswerInfo(const ANSWER_INFO& answer_info);                                     ///< 设置答题信息
    ANSWER_INFO& GetAnswerInfo() { return m_answer_info; }                                  ///< 获得答题信息
    // 翅膀
    void FillWingInfo(WING_INFO& wing_info);                                                ///< 填充翅膀信息
    void SetWingeInfo(const WING_INFO& wing_info);                                          ///< 设置翅膀信息
    WING_INFO& GetWingInfo() { return m_wing_info; }                                        ///< 获得翅膀信息
    // 修罗场
    void FillAshuraInfo(ASHURA_INFO& ashura_info);                                          ///< 填充修罗场信息
    void SetAshuraInfo(const ASHURA_INFO& ashura_info);                                     ///< 设置修罗场信息
    ASHURA_INFO& GetAshuraInfo() { return m_ashura_info; }                                  ///< 获得修罗场信息

    // 称号
    void SetTitleInfo(const TITLE_LIST& titlelist);                                         ///< 设置称号信息
    void FillTitleInfo(TITLE_LIST& titlelist);                                              ///< 填充称号信息
    TITLE_LIST& GetTitleInfo() {return m_title_list;}                                       ///< 获得称号信息

    // 充值信息
    void SetRechargeInfo(PLAYER_RECHARGE_INFO& recharege_info);                             ///< 设置充值信息
    void FillRechargeInfo(PLAYER_RECHARGE_INFO& recharege_info);                            ///< 填充充值信息
    PLAYER_RECHARGE_INFO& GetRechargeInfo() {return m_recharge_info; }                      ///< 获得充值信息

    // 商城
    void SetMallInfo(const RESTRICTION_LIST& mall_list);                                    ///< 设置商城信息
    void FillMallInfo(RESTRICTION_LIST& mall_list);                                         ///< 填充商城信息
    RESTRICTION_LIST& GetMallInfo() {return m_mall_list;}                                   ///< 获得商城信息

    // 日常投资
    void SetDayInvestmentInfo(const DAY_INVESTMENT_INFO& day_investment_info);              ///< 设置日常投资信息
    void FillDayInvestmentInfo(DAY_INVESTMENT_INFO& day_investment_info);                   ///< 填充日常投资信息
    DAY_INVESTMENT_INFO& GetDayInvestmentInfo() {return m_day_investment_info;}             ///< 获得日常投资信息

    // 等级投资
    void SetLevelInvestmentInfo(const LEVEL_INVESTMENT_INFO& level_investment_info);        ///< 设置等级投资信息
    void FillLevelInvestmentInfo(LEVEL_INVESTMENT_INFO& level_investment_info);             ///< 填充等级投资信息
    LEVEL_INVESTMENT_INFO& GetLevelInvestmentInfo() {return m_level_investment_info;}       ///< 获得等级投资信息

    // 等级活动
    void SetLevelActivityInfo(const LEVEL_ACTIVITY_INFO& level_activity_info);              ///< 设置等级活动信息
    void FillLevelActivityInfo(LEVEL_ACTIVITY_INFO& level_activity_info);                   ///< 填充等级活动信息
    LEVEL_ACTIVITY_INFO& GetLevelActivityInfo() {return m_level_activity_info;}             ///< 获得等级活动信息

    // 天赋信息
    void SetPassiveSkill(const PASSIVE_SKILL_LIST& passive_skill_list);                     ///< 设置天赋信息
    void FillPassiveSkill(PASSIVE_SKILL_LIST& passive_skill_list);                          ///< 填充天赋信息
    PASSIVE_SKILL_LIST& GetPassiveSkillInfo() {return m_passive_skill_list;}                ///< 获得天赋信息

    // 自动战斗设置信息
    void SetAutoFightSettingInfo(const AUTO_FIGHT_SETTING_INFO& auto_fight_setting_info);   ///设置自动战斗设置信息
    void FillAutoFightSettingInfo(AUTO_FIGHT_SETTING_INFO& auto_fight_setting_info);        ///填充自动战斗设置信息
    AUTO_FIGHT_SETTING_INFO& GetAutoFightSettingInfo() {return m_auto_fight_setting_info;}  ///获得自动战斗设置信息

    // 系统设置信息
    void SetSystemSettingInfo(const SYSTEM_SETTING_INFO& system_setting_info);              ///设置系统设置信息
    void FillSystemSettingInfo(SYSTEM_SETTING_INFO& system_setting_info);                   ///填充系统设置信息
    SYSTEM_SETTING_INFO& GetSystemSettingInfo() {return m_system_setting_info;}             ///获得系统设置信息

    // 玩家设置信息
    void SetPlayerSettingInfo(const PLAYER_SETTING_INFO& player_setting_info);              ///设置玩家设置信息
    void FillPlayerSettingInfo(PLAYER_SETTING_INFO& player_setting_info);                   ///填充玩家设置信息
    PLAYER_SETTING_INFO& GetPlayerSettingInfo() {return m_player_setting_info;}             ///获得玩家设置信息

    // 自动购买设置信息
    void SetAutobuySettingInfo(const AUTOBUY_SETTING_INFO& autobuy_setting_info);           ///设置自动购买设置信息
    void FillAutobuySettingInfo(AUTOBUY_SETTING_INFO& autobuy_setting_info);                ///填充自动购买设置信息
    AUTOBUY_SETTING_INFO& GetAutobuySettingInfo() {return m_autobuy_setting_info;}          ///获得自动购买设置信息

    // 虚空挂机设置信息
    void SetVoidFieldSettingInfo(const VOID_FIELD_SETTING_INFO& void_field_setting_info);   ///设置虚空挂机设置信息
    void FillVoidFieldSettingInfo(VOID_FIELD_SETTING_INFO& void_field_setting_info);        ///填充虚空挂机设置信息
    VOID_FIELD_SETTING_INFO& GetVoidFieldSettingInfo() {return m_voidfield_setting_info;}   ///获得虚空挂机设置信息

    // 活跃度信息
    void SetLivenessInfo(const LIVENESS_INFO& liveness_info);           // 设置活跃度信息
    void FillLivenessInfo(LIVENESS_INFO& liveness_info);                // 填充活跃度信息
    LIVENESS_INFO& GetLivenessInfo() {return m_liveness_info;}          // 获得活跃度信息

    //限购
    void SetLimitBuyInfo(const LIMIT_BUY_LIST& limibuylist);            // 设置活跃度信息
    void FillLimitBuyInfo(LIMIT_BUY_LIST& limibuylist);                 // 填充活跃度信息
    LIMIT_BUY_LIST& GetLimitBuyInfo() {return m_limitbuy_info;}         // 获得活跃度信息

    // 守卫地宫信息
    void SetDungeonTimesInfo(const DUNGEON_TIMES_LIST& td_info);                            ///< 设置守卫地宫信息
    void FillDungeonTimesInfo(DUNGEON_TIMES_LIST& td_info);                                 ///< 填充守卫地宫信息
    DUNGEON_TIMES_LIST& GetDungeonTimesInfo() {return m_dungeon_times_info;}                ///< 获得守卫地宫信息

    //单人答题
    void SetSingleAnswerInfo(const SINGLE_ANSWER_INFO& single_answer_info);                 // 加载单人答题系统
    void FillSingleAnswerInfo(SINGLE_ANSWER_INFO& single_answer_info);                      // 填充单人答题系统
    SINGLE_ANSWER_INFO& GetSingleAnswerInfo() {return m_single_answer_info;}                // 获得单人答题系统

    // 时装
    void FillFashionInfo(FASHION_BAG& fashion_bag);                                         // 填充时装
    void SetFashionInfo(const FASHION_BAG& fashion_bag);                                    // 设置时装
    FASHION_BAG& GetFashionInfo() { return m_fashion_bag; }                                 // 获得时装信息

    //宠物阵型
    void FillPetFormation(PET_FORMATION_INFO& info);
    void SetPetFormation(const PET_FORMATION_INFO& info);
    PET_FORMATION_INFO& GetPetFormation(){return m_pet_formation;}

    //圣剑
    void FillSwordInfo(SWORD_NEW_INFO& info);
    void SetSwordInfo(const SWORD_NEW_INFO& info);
    SWORD_NEW_INFO& GetSwordInfo(){return m_sword_info;}

    // 僵尸
    void SetZombieInfo(const ZOMBIE_INFO& zombie_info);
    void FillZombieInfo(ZOMBIE_INFO& zombie_info);
    ZOMBIE_INFO& GetZombieInfo();

    // 勇敢的心
    void FillBhInfo(BH_INFO& bh_info);                                      ///< 填充勇敢的心数据
    void SetBhInfo(const BH_INFO& bh_info);                                 ///< 设置勇敢的心数据
    BH_INFO& GetBhInfo() { return m_bh_info; }                              ///< 获得勇敢的心数据

    // 礼品卡信息
    void SetGiftCodeInfo(const GIFT_CODE_INFO& info);
    void FillGiftCodeInfo(GIFT_CODE_INFO& info);
    GIFT_CODE_INFO& GetGiftCodeInfo() { return m_gift_code_info; }

    // 游戏收藏
    void FillShorcutInfo(SHORCUT_BAG& shorcut_bag);                                         ///< 填充游戏收藏
    void SetShorcutInfo(const SHORCUT_BAG& shorcut_bag);                                    ///< 设置游戏收藏
    SHORCUT_BAG& GetShorcutInfo() { return m_shorcut_info; }                                 ///< 获得游戏收藏信息

    void SetGuildScienceInfo(const GUILD_PLAYER_SCIENCE_INFO& info);
    void FillGuildScienceInfo(GUILD_PLAYER_SCIENCE_INFO& info);
    GUILD_PLAYER_SCIENCE_INFO& GetGuildPlayerScienceInfo() { return m_guild_player_science_info;}
    
    // 跨服战斗
    void FillMultiserverBattleInfo(MULTISERVER_BATTLE_BAG& info);                                           ///< 填充跨服战斗信息
    void SetMultiserverBattleInfo(const MULTISERVER_BATTLE_BAG& info);                                      ///< 设置跨服战斗信息
    MULTISERVER_BATTLE_BAG& GetMultiserverBattleInfo() { return m_MultiserverBattleInfo; }                                   ///< 获得跨服战斗信息信息

    // 投资计划信息
    void FillInvestmentPlanInfo(INVESTMENT_PLAN_INFO& info);
    void SetInvestmentPlanInfo(const INVESTMENT_PLAN_INFO& info);
    INVESTMENT_PLAN_INFO& GetInvestmentPlanInfo(){return m_investment_plan_info;}

    //捐献
    void FillDonateInfo(DONATE_INFO& info);
    void SetDonateInfo(const DONATE_INFO& info);
    DONATE_INFO& GetDonateInfo(){return m_donate_info;}

    // 砸金蛋信息
    void FillEggInfo(EGG_ACTIVITY_INFO& info);                   ///< 填充砸金蛋数据
    void SetEggInfo(const EGG_ACTIVITY_INFO& info);              ///< 设置砸金蛋数据
    EGG_ACTIVITY_INFO& GetEggInfo() { return m_egg_info; }                  ///< 获得砸金蛋数据

    // 越买越便宜信息
    void FillBuyCheapInfo(BUY_CHEAP_INFO& info);                            ///< 填充越买越便宜数据
    void SetBuyCheapInfo(const BUY_CHEAP_INFO& info);                       ///< 设置越买越便宜数据
    BUY_CHEAP_INFO& GetBuyCheapInfo() { return m_buy_cheap_info; }          ///< 获得越买越便宜数据

    // 个性头像
    void FillHeadPortraitInfo(HEAD_PORTRAIT_INFO& info);                            ///< 填充个性头像数据
    void SetHeadPortraitInfo(const HEAD_PORTRAIT_INFO& info);                       ///< 设置个性头像数据
    HEAD_PORTRAIT_INFO& GetHeadPortraitInfo() { return m_head_portrait_info; }          ///< 获得个性头像宜数据

    void SetOfflinePlayerInfo(const OFFLINE_PLAYER_INFO& offline_info);     ///< 离线角色信息
    OFFLINE_PLAYER_INFO& GetOfflinePlayerInfo();                            ///< 获得离线的角色信息

    void UpdateSocialState();                                               ///< 更新角色的社交状态

    uint32 GetTicket() { return m_ticket; }                                 ///< 获得票据号
    void IncTicket() { ++m_ticket; }                                        ///< 增长票据号

    uint32 GetState() { return m_state; }                                   ///< 获得状态
    void SetState(uint32 val);                                              ///< 设置状态

    uint64 GetLoginID() { return m_login_id; }                              ///< 获得登陆ID
    void SetLoginID(uint64 val) { m_login_id = val; }                       ///< 设置登陆ID

    uint64 GetPlayerID() { return m_player_id; }                            ///< 获得角色ID
    void SetPlayerID(uint64 val) { m_player_id = val; }                     ///< 设置角色ID

    uint32 GetLoginTime() { return m_login_time; }                          ///< 获得登陆时间
    void SetLoginTime(uint32 val) { m_login_time = val; }                   ///< 设置登陆时间

    uint32 GetLogoutTime() { return m_logout_time; }                        ///< 获得登出时间
    void SetlogoutTime(uint32 val) { m_logout_time = val; }                 ///< 设置登出时间

    uint32 GetLoginIP() { return m_login_ip; }                              ///< 获得登陆IP
    void SetLoginIP(uint32 val) { m_login_ip = val; }                       ///< 设置登陆IP

    uint32 GetLoginPort() { return m_login_port; }                          ///< 获得登陆IP
    void SetLoginPort(uint32 val) { m_login_port = val; }                   ///< 设置登陆IP

    uint32 GetSessionID() { return m_session_id; }                          ///< 获得网络ID
    void SetSessionID(uint32 val) { m_session_id = val; }                   ///< 设置网络ID

    void SetPropInfo(PROPERTY_INFO& prop_info);                             ///< 复制角色属性

    const std::string& GetAccount() { return m_prop_info.account; }         ///< 获得账号名称
    void SetAccount(const std::string& val) { m_prop_info.account = val; }  ///< 设置账号名称

    const std::string& GetName() { return m_prop_info.name; }               ///< 获得角色名称
    void SetName(const std::string& val) { m_prop_info.name = val; }        ///< 设置角色名称

    const std::string& GetPlatformUID() { return m_platform_uid; }                 ///< 获取平台账号ID
    void SetPlatformUID(const std::string& val) { m_platform_uid = val; }         ///< 设置平台账号ID

    const std::string& GetPlatformUName() { return m_platform_uname; }             ///< 获取平台账号
    void SetPlatformUName(const std::string& val) { m_platform_uname = val; }     ///< 设置平台账号

    const std::string& GetPlatform() { return m_platform; }                        ///< 获得平台名
    void SetPlatform(const std::string& val) { m_platform = val; }          ///< 设置平台名

    const std::string& GetPlatformServerName() { return m_prop_info.platform_server_name; }         // 获得平台服务器名
    void SetPlatformServerName(const std::string& val) {m_prop_info.platform_server_name = val;}    // 仅创建时设置

    int32 GetRegion() { return m_prop_info.region; }                        ///< 获得区号
    void SetRegion(int32 val) { m_prop_info.region = val; }                 ///< 设置区号

    int8 GetSex() { return m_prop_info.sex; }                               ///< 获得性别
    void SetSex(int8 val) { m_prop_info.sex = val; }                        ///< 设置性别

    int8 GetCareer() { return m_prop_info.career; }                         ///< 获得职业
    void SetCareer(int val) { m_prop_info.career = val; }                   ///< 设置职业

    int32 GetLevel() { return m_prop_info.level; }                          ///< 获得角色等级
    void SetLevel(int32 val) { m_prop_info.level = val; }                   ///< 设置角色等级

    int32 GetMaxHP() { return m_prop_info.max_hp; }                         ///< 获得最大生命

    int32 GetHP() { return m_prop_info.hp; }                                ///< 获得生命
    void SetHP(int32 val) { m_prop_info.hp = val; }                         ///< 设置生命

    int32 GetMaxMP() { return m_prop_info.max_mp; }                         ///< 获得最大魔法

    int GetMP() { return m_prop_info.mp; }                                  ///< 获得魔法
    void SetMP(int val) { m_prop_info.mp = val; }                           ///< 设置魔法

    int32 GetExp() { return m_prop_info.exp; }                              ///< 获得经验值
    void SetExp(int32 val) { m_prop_info.exp = val; }                       ///< 设置经验值

    //int32 GetSysVigor() { return m_prop_info.sys_vigor; }                   ///< 获得系统体力值
    //void  SetSysVigor(int32 val) { m_prop_info.sys_vigor = val; }           ///< 设置系统体力值

    //int32 GetBuyVigor() { return m_prop_info.buy_vigor; }                   ///< 获得购买体力值
    //void  SetBuyVigor(int32 val) { m_prop_info.buy_vigor = val; }           ///< 设置购买体力值

    uint32 GetDailyReftime() { return m_prop_info.daily_reftime; }          ///< 获得每日刷新时间
    void SetDailyReftime(uint32 val) { m_prop_info.daily_reftime = val; }   ///< 设置每日刷新时间

    //int32 GetVigorBuyTimes() { return m_prop_info.vigor_buytimes; }         ///< 获得体力购买次数
    //void  SetVigorBuyTimes(int32 val) { m_prop_info.vigor_buytimes = val; } ///< 设置体力购买次数

    //int32 GetTeamBuyTimes() { return m_prop_info.team_buytimes; }                           ///< 返回队伍副本购买次数
    //void  SetTeamBuyTimes(int32 val) { m_prop_info.team_buytimes = val; }                   ///< 设置队伍副本购买次数

    //int32 GetTeamDungeonSysTimes() { return m_prop_info.team_dungeon_systimes; }            ///< 获得队伍副本次数
    //void  SetTeamDungeonSysTimes(int32 val) { m_prop_info.team_dungeon_systimes = val; }    ///< 设置队伍副本次数

    //int32 GetTeamDungeonBuyTimes() { return m_prop_info.team_dungeon_buytimes; }            ///< 获得队伍购买副本次数
    //void  SetTeamDungeonBuyTimes(int32 val) { m_prop_info.team_dungeon_buytimes = val; }    ///< 设置队伍购买副本次数

    //int32 GetStoneTurnGoldTimes() { return m_prop_info.stone_turn_gold_times; }             ///< 获得点石成金的次数
    //void  SetStoneTurnGoldTimes(int32 val) { m_prop_info.stone_turn_gold_times = val; }     ///< 设置点石成金的次数

    //uint32 GetVoidFieldTime() {return m_prop_info.void_field_time;}                         ///<.获得虚空幻境的挂机时间
    //void SetVoideFiedlTime(uint32 val) {m_prop_info.void_field_time = val;}                 ///<.设置虚空幻境的挂机时间

    //uint32 GetVoidFieldReftime() {return m_prop_info.void_field_reftime;}                  ///<.虚空幻境的刷新时间
    //void SetVoidFieldReftime(uint32 val) {m_prop_info.void_field_reftime = val;}           ///<.设置虚空幻境的刷新时间

    //int32 GetBattlePower() { return m_prop_info.battle_power; }         ///< 获得战斗力
    //void  SetBattlePower(int32 val) { m_prop_info.battle_power = val; } ///< 设置战斗力

    int32 GetGamePoint() { return m_prop_info.game_point; }             ///< 获得金币
    void SetGamePoint(int32 val) { m_prop_info.game_point = val; }      ///< 设置金币

    int32 GetMoneyPoint() { return m_prop_info.money_point; }           ///< 获得游戏币
    void AddMoneyPoint(int32 val, int32 type);                          ///< 添加游戏币
    void SubMoneyPoint(int32 val) { m_prop_info.money_point -= val; }   ///< 减少游戏币
    void SetMoneyPoint(int32 val) { m_prop_info.money_point = val; }    ///< 设置游戏币

    int32  GetVigorValue() { return m_prop_info.vigor; }                      ///< 获得当前体力值
    void   SetVigorValue(int32 val) { m_prop_info.vigor = val; }              ///< 设置体力值
    int32  GetMaxVigorValue(){ return PLAYER_VIGOR_MAX; }                     ///< 获取最大体力值
    uint32 GetVigorReftime() { return m_prop_info.vigor_reftime; }            ///< 获得体力刷新时间
    void   SetVigorReftime(uint32 val) { m_prop_info.vigor_reftime = val; }   ///< 设置体力刷新时间

    //int32 GetBindMoneyPoint() { return m_prop_info.bind_money_point; }          ///< 获得绑定游戏币
    //void SetBindMoneyPoint(int32 val) { m_prop_info.bind_money_point = val; }   ///< 设置绑定游戏币

    //int32 GetSoulPoint() { return m_prop_info.soul_point; }                     ///< 获得精魂
    //void SetSoulPoint(int32 val) { m_prop_info.soul_point = val; }              ///< 设置精魂

    //int32 GetStarPoint() { return m_prop_info.star_point;}                      ///<.获得星魂
    //void SetStarPoint(int32 val) { m_prop_info.star_point = val; }              ///<.设置星魂

    //int32 GetHonorPoint() { return m_prop_info.honor_point; }                   ///< 获得荣誉
    //void SetHonorPoint(int32 val) { m_prop_info.honor_point = val; }            ///< 设置荣誉

    //int32 GetGuildPoint() { return m_prop_info.guild_point; }                   ///< 获得公会贡献点
    //void SetGuildPoint(int32 val) { m_prop_info.guild_point = val; }            ///< 设置公会贡献点

    //int32 GetIncarnationPoint() { return m_prop_info.incarnation_point; }           ///< 获得化身点
    //void SetIncarnationPoint(int32 val) { m_prop_info.incarnation_point = val; }    ///< 设置化身点

    //int32 GetBattleSoulPoint() { return m_prop_info.battle_soul_point; }           ///< 获得战魂
    //void SetBattleSoulPoint(int32 val) { m_prop_info.battle_soul_point = val; }    ///< 设置战魂

    //int32 GetGrowthPoint() { return m_prop_info.growth_point; }         ///< 获取成长点
    //void SetGrowthPoint(int32 val) { m_prop_info.growth_point = val; }  ///< 设置成长点

    //int32 GetGrowthPointSpendplus() { return m_prop_info.spend_plus; }         ///< 获取增加过成点后花费剩余磨石
    //void SetGrowthPointSpendplus(int32 val) { m_prop_info.spend_plus = val; }  ///< 设置增加过成点后花费剩余磨石

    uint32 GetMoveSpeed() { return m_MoveSpeed; }                       ///< 获得移动速度
    void SetMoveSpeed(uint32 val) { m_MoveSpeed = val; }                ///< 设置移动速度

    int32 GetAngle() { return m_prop_info.angle; }                      ///< 获得移动角度
    void SetAngle(int32 val) { m_prop_info.angle = val; }               ///< 设置移动角度

    uint32 GetRightFlag() { return m_prop_info.right_flag; }            ///< 获得权利标记
    void SetRightFlag(uint32 val) { m_prop_info.right_flag = val; }     ///< 设置权利标记

    int32 GetX() { return m_prop_info.x; }                              ///< 获得 x 坐标
    void SetX(int32 val) { m_prop_info.x = val;}                        ///< 设置 x 坐标

    int32 GetY() { return m_prop_info.y; }                              ///< 获得 y 坐标
    void SetY(int32 val) { m_prop_info.y = val; }                       ///< 设置 y 坐标

    uint64 GetGuildID() { return U64ID(m_prop_info.guild_id_h, m_prop_info.guild_id_l); }
    void SetGuildID(uint64 val)
    {
        m_prop_info.guild_id_h = U64ID_HIGH(val);
        m_prop_info.guild_id_l = U64ID_LOW(val);
    }

    //uint32 GetFactionID() { return m_prop_info.faction_id; }            ///< 获得派别
    //void SetFactionID(uint32 val) { m_prop_info.faction_id = val; }     ///< 设置派别

    uint32 GetGuildJoinTime() { return m_prop_info.guild_join_time; }           ///< 获得公会加入时间
    void SetGuildJoinTime(uint32 val) { m_prop_info.guild_join_time = val; }    ///< 设置公会加入时间

    bool IsGM() {return GetRightFlag() & RIGHT_FLAG_GM;}                ///< 是否是GM
    bool IsGMJump() { return GetRightFlag() & RIGHT_FLAG_GM_JUMP_DUNGEON;}      ///< 是有GM跳地图功能

    int32 GetUnionMapID() { return m_prop_info.unionmap_id; }           ///< 获得副本ID(不存DB)
    void SetUnionMapID(int32 val) { m_prop_info.unionmap_id = val; }    ///< 设置副本ID(不存DB)

    //uint32 GetMapType() { return m_prop_info.map_type; }                         ///< 获得进入地图类型
    //void SetMapType(uint32 val) { m_prop_info.map_type = val; }                  ///< 设置进入地图类型

    uint32 GetCityMapID() { return m_city_id; }                             // 获得主城ID
    void SetCityMapID(uint32 val) { m_city_id = val; }                      // 设置主城ID

    int32 GetCityX() { return m_city_x; }                                   // 获得城市地图X坐标
    void SetCityX(int32 val) { m_city_x = val; }                            // 设置城市地图X坐标

    int32 GetCityY() { return m_city_y; }                                   // 获得城市地图Y坐标
    void SetCityY(int32 val) { m_city_y = val; }                            // 设置城市地图Y坐标

    //uint32 GetFieldMapID() { return m_prop_info.field_map_id; }                         ///< 获得野外ID
    //void SetFieldMapID(uint32 val) { m_prop_info.field_map_id = val; }                  ///< 设置野外ID

    //int32 GetFieldX() { return m_prop_info.field_map_x; }                                ///< 获得野外地图X坐标
    //void SetFieldX(int32 val) { m_prop_info.field_map_x = val; }                         ///< 设置野外地图X坐标

    //int32 GetFieldY() { return m_prop_info.field_map_y; }                                ///< 获得野外地图Y坐标
    //void SetFieldY(int32 val) { m_prop_info.field_map_y = val; }                         ///< 设置野外地图Y坐标

    //int32 GetFieldHookX() { return m_prop_info.field_hook_x; }                                ///< 获得野外地图挂机点X坐标
    //void SetFieldHookX(int32 val) { m_prop_info.field_hook_x = val; }                         ///< 设置野外地图挂机点X坐标

    //int32 GetFieldHookY() { return m_prop_info.field_hook_y; }                                ///< 获得野外地图挂机点Y坐标
    //void SetFieldHookY(int32 val) { m_prop_info.field_hook_y = val; }                         ///< 设置野外地图挂机点Y坐标

    uint32 GetOnLineTime() {return m_prop_info.online_time;}                                  ///<.玩家的在线总时长
    void   SetOnLineTime(uint32 time) {m_prop_info.online_time = time;}

    //int32 GetWelcomeReward() {return m_prop_info.getwelcome_reward;}                        ///<欢迎界面的奖励是否领取
    //void  SetWelcomeReward(int32 val) {m_prop_info.getwelcome_reward = val;}                ///<设置欢迎界面的奖励是否领取

    const ENTER_DUNGEON_REQ& GetChangeDungeonData() {return m_ChangeDungeonData;}               // 跳副本数据
    void SetChangeDungeonData(const ENTER_DUNGEON_REQ& data) {m_ChangeDungeonData = data;}
    
    int32 GetChangeMapCount() { return m_change_map_count; }                // 切换地图次数
    void SetChangeMapCount(int32 val) { m_change_map_count = val; }         // 切换地图次数

//    void SetRaidsDungeonBuyVigor(int32 val);                                // 设置扫荡副本消耗购买体力数

    //int32 IsVip() { return m_prop_info.vip_info.is_vip; }                                   ///< 是不是VIP
    //void SetVipInfo(const VIP_INFO& val) { m_prop_info.vip_info = val; }                          ///< VIP信息
    int32 GetVipLevelByGrowthPoint();                                                       ///<得到等级

    bool AddFriend(Player* player);                                                         // 添加好友
    void DelFriend(uint32 player_id_l, uint32 player_id_h);                                 // 删除好友
    void AddRobotFriend();                                                                  // 添加机器人
    void QuickFriendList(bool robot);                                                       // 快速好友列表
    bool HasFriend(std::string name);                                                       // 是否已在好留列表中
    void NotifyFriendMyInfo();                                                              // 通知好友我的信息
    void NofityFriendInfo(const FRIEND_INFO& info);

    MAIL_INFO* GetMail(uint32 mail_id_h, uint32 mail_id_l);                                 // 获取邮件
    void ReadMail(uint8 mail_type, uint32 mail_id_h, uint32 mail_id_l, uint8 attach_flag);  // 阅读邮件
    void AddMail(MAIL_INFO& mail_info);                                                     // 添加邮件
    void DelMail(uint8 mail_type, uint32 mail_id_h, uint32 mail_id_l, uint8 attach_flag);   // 删除邮件
    void DelExpiredMail(uint32 mail_id_h);                                                  // 删除过期邮件

    void QuicklyAutoSave();

    uint64 GetTeamID() { return m_team_id; }                                                // 获取队伍ID
    void SetTeamID(uint64 val) { m_team_id = val; }                                         // 设置队伍ID

    uint32 GetCentGateID() { return m_gatesvr_id; }                                          // 获得登陆服务器ID
    void SetCentGateID(uint32 val) { m_gatesvr_id = val; }                                   // 设置登陆服务器ID

    uint32 GetCentGameID() { return m_gamesvr_id; }                                          // 获得游戏服务器ID
    void SetCentGameID(uint32 val);                                                          // 设置游戏服务器ID

    void SendFriendAddAck(uint16 errcode, const std::string& src_name, const std::string& dst_name);      // 发送添加好友应答

    void SendTeamMemberAddAck(uint16 errcode, const std::string& src_name, const std::string& dst_name);  // 发送添加队友应答
    void SendTeamMemberResetState();                                                        // 发送队友重置状态消息

    void SendMallBuyNtf(uint16 errCode);                                                    // 商城购买错误通知

    void NotifyGamePlayerProp(uint8 key, int32 val);                                        // 通知用户属性
    void NotifyGatePlayerProp(uint8 key, int32 val);                                        // 通知用户属性

    void NotifyErrorCode(uint16 errCode);                                                   // 发送错误码标识

    void SendToGate(uint32 msgID, void* data, uint32 gateId = 0);                           // 发送数据
    void SendToGame(uint32 msgID, void* data);                                              // 发送数据


    enum
    {
        kStateInit                  = 0,                    ///< 初始状态
        kStateLoginReq              = 1,                    ///< 登陆请求(未使用)
        kStateVerifyOk              = 2,                    ///< 验证OK
        kStateLoadingData           = 3,                    ///< 正在加载数据
        kStateDataLoaded            = 4,                    ///< 加载数据结束
        kStateDelivingData          = 5,                    ///< 正在向GameServer投递数据(GS上有角色)
        kStateInGame                = 6,                    ///< 进入场景(GS上有角色，进场景)
        kStateChangeDungeonSyncData = 7,                    ///< 退出副本同步数据(GS上有角色)
        kStateLogoutSyncData        = 8,                    ///< 已登出，正在同步数据
    };

    bool                m_InDungeonCheck;                   // 进出副本标记(调试用)
    int32               m_InGameServerType;                 // 进入的GameServer类型(调试用)

private:
    PlayerNetHandler*   m_pNetHandler;                      ///< 网络事件处理器

    bool                m_dirty;                            // 脏标记
    uint32              m_auto_save_timer;                  // 自动回存计时
    uint32              m_ticket;                           // 角色上线后数据的版本号
    uint32              m_state;                            // 角色状态
    uint32              m_session_id;                       // 网络ID
    uint64              m_login_id;                         // 登陆ID
    uint64              m_player_id;                        // 角色ID
    uint32              m_login_time;                       // 登陆时间
    uint32              m_logout_time;                      // 登出时间
    uint32              m_login_ip;                         // 登陆IP
    uint32              m_login_port;                       // 登陆Port
    PROPERTY_INFO       m_prop_info;                        // 属性信息
    PLAYER_DUNGEON_INFO m_player_dungeon_info;              // 角色副本信息
    std::string         m_platform_uid;                     // 平台账号ID
    std::string         m_platform_uname;                   // 平台账号
    std::string         m_platform;                         // 平台(360,9377)
    uint32              m_city_id;                          // 主城id
    uint32              m_city_x;                           // 主城x
    uint32              m_city_y;                           // 主城y
    uint32              m_MoveSpeed;                            // 移动速度
    uint8               m_dungeon_type;                     // 副本类型

    ENTER_DUNGEON_REQ   m_ChangeDungeonData;                // 跳副本数据
    uint32              m_change_map_count;                 // 切换地图次数

    PLAYER_STATE_INFO   m_state_info;                       // 角色状态信息
    ENEMY_INFO          m_enemy_info;                       // 善恶信息
    FRIEND_LIST         m_friend_mgr;                       // 好友列表
    FRIEND_LIST         m_friend_has_me;                    // 我是哪些人的好友
    SKILL_COMPONENT_INFO    m_skill_info;                       // 技能信息
    ITEM_INFO           m_item_info;                        // 物品信息
    ITEM_INFO           m_store_info;                       // 仓库物品信息
    RANDOM_PACKAGE_RECORD_INFO  m_package_record;           // 随机包裹信息
    //HIDE_BAG_INFO       m_hide_bag_info;                  // 隐藏物品
    EQUIP_INFO          m_equip_info;                       // 装备信息
    POTION_INFO         m_potion_info;                      // 药剂使用信息
    PET_INFO            m_pet_info;                         // 跟宠信息
    RIDE_INFO           m_rideinfo;                         // 骑宠信息
    TASK_INFO           m_task_info;                        // 任务信息
    GUIDE_INFO          m_guide_info;                       // 引导信息
    MAIL_SYSTEM_INFO    m_mail_info;                        // 邮件系统信息
    MONSTER_DOOR_INFO   m_monster_door_info;                // 离线战斗信息
    PLAYER_DOOR_INFO    m_player_door_info;                 // 勇者争霸信息
    OFFLINE_PLAYER_INFO m_offline_player_info;              // 离线角色信息
    REWARD_INFO         m_reward_info;                      // 奖励池数据
    BABEL_INFO          m_babel_info;                       // 通天塔数据
    HOROSCOPE_INFO      m_horoscope_info;                   // 星空图信息
    SIGN_INFO           m_sign_info;                        // 签到信息
    OPEN_SYSTEM_INFO    m_open_sysetm_info;                 // 功能开启信息
    ONLINE_REWARD_INFO  m_online_reward_info;               // 在线奖励信息
    RACING_INFO         m_racing_info;                      // 赛马信息
    MINING_INFO         m_mining_info;                      // 采矿信息
    TREASURY_INFO       m_treasury_info;                    // 宝库信息
    GOLDTREE_INFO       m_goldtree_info;                    // 金像信息
    ADVENTURE_INFO      m_adventure_info;                   // 大冒险信息
    ANSWER_INFO         m_answer_info;                      // 答题信息
    WING_INFO           m_wing_info;                        // 翅膀信息
    ASHURA_INFO         m_ashura_info;                      // 修罗场信息
    TITLE_LIST          m_title_list;                       // 称号信息
    PLAYER_RECHARGE_INFO m_recharge_info;                   // 充值信息
    RESTRICTION_LIST    m_mall_list;                        // 商城信息
    DAY_INVESTMENT_INFO m_day_investment_info;              // 日常投资信息
    LEVEL_INVESTMENT_INFO m_level_investment_info;          // 等级投资信息
    LEVEL_ACTIVITY_INFO m_level_activity_info;              // 等级活动信息
    PASSIVE_SKILL_LIST  m_passive_skill_list;               // 被动技能信息
    AUTO_FIGHT_SETTING_INFO m_auto_fight_setting_info;      // 自动战斗设置信息
    SYSTEM_SETTING_INFO m_system_setting_info;              // 系统设置信息
    PLAYER_SETTING_INFO m_player_setting_info;              // 玩家设置信息
    AUTOBUY_SETTING_INFO m_autobuy_setting_info;            // 自动购买设置信息
    VOID_FIELD_SETTING_INFO m_voidfield_setting_info;       // 虚空幻境挂机设置信息
    LIVENESS_INFO       m_liveness_info;                    // 活跃度信息
    DUNGEON_TIMES_LIST  m_dungeon_times_info;               // 守卫地宫信息
    LIMIT_BUY_LIST      m_limitbuy_info;                    // 限购信息
    SINGLE_ANSWER_INFO  m_single_answer_info;               // 单人答题信息
    FASHION_BAG         m_fashion_bag;                      // 资源挽回信息
    SWORD_NEW_INFO      m_sword_info;                       // 圣剑信息
    ZOMBIE_INFO         m_zombie_info;                      // 僵尸生存信息
    BH_INFO             m_bh_info;                          // 勇敢的心信息
    GIFT_CODE_INFO      m_gift_code_info;                   // 使用礼品卡信息
    GUILD_PLAYER_SCIENCE_INFO   m_guild_player_science_info;// 公会科技信息
    SHORCUT_BAG         m_shorcut_info;                     // 游戏收藏信息
    MULTISERVER_BATTLE_BAG  m_MultiserverBattleInfo;        // 跨服PK信息
    INVESTMENT_PLAN_INFO    m_investment_plan_info;         // 投资计划信息

    PET_FORMATION_INFO  m_pet_formation;                    // 宠物阵型信息
    PAY_POINT_LOTTERY_INFO m_pay_point_lottery_info;        // 充值积分抽奖信息
    VIP_ONLY_DUNGEON_INFO m_vip_only_dungeon_info;          // VIP独享副本信息
    DONATE_INFO         m_donate_info;                      // 捐献信息
    EGG_ACTIVITY_INFO   m_egg_info;                         // 砸金蛋信息
    BUY_CHEAP_INFO      m_buy_cheap_info;                   // 越买越便宜
    bool                m_team_flag;                        // 队伍进入副本标志
    HEAD_PORTRAIT_INFO  m_head_portrait_info;               // 个性头像

    //LimitSellManager*    m_limit_sell_manager;               ///< 限购管理器

    uint64              m_team_id;                          ///< 队伍ID
    // ================================================================================
    uint32              m_gatesvr_id;                       ///< 登陆服务器 ID
    uint32              m_gamesvr_id;                       ///< 游戏服务器 ID;(类型)
    uint32              m_region_id;                     ///游戏服务器ID;
    
};

#endif // player_h__
