/*
 *	角色加载器
 */
#ifndef player_saver_h__
#define player_saver_h__

class Player;


class PlayerLoader
{
    friend class Player;
public:
    PlayerLoader(Player* player);
    ~PlayerLoader();

    void Load(uint64 playerID, DELIVER_PLAYER_INFO_REQ* recvMsg);

private:
    void LoadBaseInfo(uint64 playerID, PROP_BASE_INFO& info);                       // 加载角色的基本信息

    void LoadStateInfo(const PLAYER_STATE_INFO& info);                              // 状态信息(buff，技能CD)

    void LoadPlayerDungeonInfo(const PLAYER_DUNGEON_INFO& info);                    // 加载角色的副本信息

    void SetOpenSystemInfo(OPEN_SYSTEM_INFO& val);                                  // 设置系统信息

    void SetGuildWelfareInfo(GUILD_WELFARE_INFO& val);                              // 设置公会福利信息

    void SetCityFightInfo(CITY_FIGHT_PLAYER_INFO& val);                             // 设置城市争夺战信息

    void LoadHoroscopeInfo(HOROSCOPE_INFO& info);                                   // 加载星宫图信息

    void LoadSkillComponentInfo(SKILL_COMPONENT_INFO& info);                                           // 加载技能信息

	void LoadPassiveSkillInfo(const PASSIVE_SKILL_LIST& info);                      // 加载被动技能信息

    void LoadEquipInfo(EQUIP_INFO& list);                                           // 加载装备信息

    void LoadPotionInfo(POTION_INFO& info);                                         // 加载药剂信息

    void LoadFashionInfo(FASHION_BAG& list);                                        // 加载装备信息

    void LoadSwordInfo(SWORD_NEW_INFO& info);                                           // 加载圣剑信息

    void LoadPetFormation(PET_FORMATION_INFO& info);                                // 获得宠物阵型管理器

    void LoadPetInfo(PET_INFO& info);                                            // 加载跟宠信息

    void LoadRidePetInfo(RIDE_INFO& list);                                          // 加载坐骑信息

    void LoadItemInfo(const ITEM_INFO& info);                                        // 加载物品信息

    void LoadStoreInfo(const ITEM_INFO& info);                                      // 加载仓库信息

    void LoadRandomPackageInfo(const RANDOM_PACKAGE_RECORD_INFO& info);                                  // 加载抽奖信息

    //void LoadHideBagInfo(const HIDE_BAG_INFO& info);                                // 加载隐藏物品列表

    void LoadFriendInfo(FRIEND_LIST& info);                                         // 加载好友信息发

    void LoadMonsterDoorInfo(MONSTER_DOOR_INFO& info);                              // 加载怪物之门数据

    void LoadPlayerDoorInfo(PLAYER_DOOR_INFO& info);                                // 加载勇者争霸信息

    void LoadRewardInfo(REWARD_INFO& info);                                         // 加载奖励信息

    void LoadBabelInfo(BABEL_INFO& info);                                           // 加载通天塔信息（至尊魔宫）

    void LoadPayPointLotteryInfo(PAY_POINT_LOTTERY_INFO &info);                     // 加载充值积分抽奖信息

    void LoadVIPOnlyDungeonInfo(VIP_ONLY_DUNGEON_INFO &info);                       // 加载VIP独享副本信息

    void LoadMailSystemInfo(const MAIL_SYSTEM_INFO& info);                          // 加载邮件系统信息

    void LoadRacingInfo(RACING_INFO& racing_info);                                  // 加载赛马数据

    void LoadSignInfo(SIGN_INFO& sign_info);                                        // 加载签到信息

    void LoadOnlineRewardInfo(ONLINE_REWARD_INFO& info);                            // 加载在线奖励信息

    void LoadMiningInfo(MINING_INFO& info);                                         // 加载采矿信息

    void LoadTreasuryInfo(TREASURY_INFO& info);                                     // 加载宝库信息

    void LoadGoldtreeInfo(GOLDTREE_INFO& info);                                     // 加载金像信息

    void LoadAdventureInfo(ADVENTURE_INFO& info);                                   // 加载大冒险数据

    void LoadAnswerInfo(const ANSWER_INFO& info);                                   // 加载答题数据

    void LoadWingInfo(WING_INFO& info);                                             // 加载翅膀信息

    void LoadAshuraInfo(ASHURA_INFO& info);                                         // 加载修罗场信息

    void LoadRechargeInfo(const PLAYER_RECHARGE_INFO& info);                        // 加载充值信息

    void LoadMallInfo(const RESTRICTION_LIST& info);                                // 加载商城信息

    void LoadTitleInfo(TITLE_LIST& info);                                           // 加载称号信息

    void LoadDayInvestmentInfo(DAY_INVESTMENT_INFO& day_investment_info);           // 加载日常投资信息

    void LoadLevelInvestmentInfo(LEVEL_INVESTMENT_INFO& level_investment_info);     // 加载等级投资信息

    void LoadLevelActivityInfo(LEVEL_ACTIVITY_INFO& level_activity_info);           // 加载等级活动信息

    void LoadAutoFightSettingInfo(AUTO_FIGHT_SETTING_INFO& auto_fight_setting_info);// 加载自动战斗设置信息

    void LoadSystemSettingInfo(SYSTEM_SETTING_INFO& system_setting_info);           // 加载系统设置信息

    void LoadPlayerSettingInfo(PLAYER_SETTING_INFO& player_setting_info);           // 加载玩家设置信息

    void LoadAutobuySettingInfo(AUTOBUY_SETTING_INFO& autobuy_setting_info);        // 加载自动购买设置信息

    void LoadVoidFieldSettingInfo(VOID_FIELD_SETTING_INFO& void_field_setting_info);// 加载虚空幻境挂机设置信息

    void LoadLivenessInfo(const LIVENESS_INFO& info);                               // 加载活跃度信息

    void LoadDungeonTimesInfo(DUNGEON_TIMES_LIST& info);                            // 加载副本进入次数信息

    void LoadLimitBuyInfo(LIMIT_BUY_LIST& info);                                    // 加载购买商品数量信息

    void LoadSingleAnswerInfo(SINGLE_ANSWER_INFO& single_answer_info);              // 加载单人答题系统

    void LoadBhInfo(BH_INFO& info);                                                 // 加载勇敢的心信息

    void LoadTaskInfo(TASK_INFO& info);                                             // 加载任务信息

    void LoadGuideInfo(GUIDE_INFO& info);                                           // 加载引导信息

    void LoadOpenSystemInfo(OPEN_SYSTEM_INFO& info);                                // 加载功能开启

    void LoadEnemyInfo(ENEMY_INFO& info);                                           // 加载仇人信息

    void loadZombieInfo(ZOMBIE_INFO& info);

    void LoadGuildScienceInfo(GUILD_PLAYER_SCIENCE_INFO& info);                     // 加载个人公会科技信息

    void LoadShorcutInfo(SHORCUT_BAG& list);                                        // 加载收藏游戏信息

    void LoadMultiserverBattleInfo(const MULTISERVER_BATTLE_BAG& info);             // 加载跨服战斗信息

    void LoadInvestmentPlan(INVESTMENT_PLAN_INFO& info);                            // 加载投资理财计划信息

    void LoadDonateInfo(DONATE_INFO& info);                                         // 加载捐献信息

    void LoadEggInfo(EGG_ACTIVITY_INFO& info);                                            // 加载砸金蛋信息

    void LoadBuyCheapInfo(BUY_CHEAP_INFO& info);                                    // 加载越买越便宜信息

    void LoadHeadPortraitInfo(HEAD_PORTRAIT_INFO& info);                            // 加载个性头像

private:
    Player* m_player;
};


#endif // player_saver_h__
