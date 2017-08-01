/*
 *	角色保存器
 */
#ifndef player_loader_h__
#define player_loader_h__

class Player;

class PlayerSaver
{
    friend class Player;
public:
    PlayerSaver(Player* player);
    ~PlayerSaver();
    
    void Save(WRITEBACK_PLAYER_INFO_REQ& sendMsg);

private:
    void FillBaseInfo(PROP_BASE_INFO& info);                                        // 填充角色基本属性信息

    void FillStateInfo(PLAYER_STATE_INFO& info);                                    // 状态信息(buff，技能CD)

    void FillPlayerDungeonInfo(PLAYER_DUNGEON_INFO& info);                          // 填充角色的副本信息

    void FillOpenSystemInfo(OPEN_SYSTEM_INFO& info);                                // 填充功能开启信息

    void FillGuildWelfareInfo(GUILD_WELFARE_INFO& val);                             // 填充公会福利信息

    void FillCityFightInfo(CITY_FIGHT_PLAYER_INFO& val);                            // 填充城市争夺战信息

    void FillHoroscopeInfo(HOROSCOPE_INFO& horoscope_info);                         // 填充天宫图信息

    void FillSkillComponentInfo(SKILL_COMPONENT_INFO& info);                        // 填充技能信息

    void FillEquipInfo(EQUIP_INFO& info);                                           // 填充装备信息

    void FillPotionInfo(POTION_INFO& info);                                         // 填充药剂信息

    void FillFashionInfo(FASHION_BAG& info);                                        // 填充装备信息

    void FillPetInfo(PET_INFO& info);                                            // 填充跟宠信息

    void FillRideInfo(RIDE_INFO& info);                                             // 填充坐骑信息

    void FillItemInfo(ITEM_INFO& info);                                               // 填充包裹信息

    void FillStoreInfo(ITEM_INFO& info);                                               // 填充仓库信息

    void FillRandomPackageInfo(RANDOM_PACKAGE_RECORD_INFO& info);                                        // 填充抽奖信息

    //void FillHideBagInfo(HIDE_BAG_INFO& info);                                      // 填充隐藏物品列表

    void FillTaskInfo(TASK_INFO& info);                                             // 填充任务信息

    void FillGuideInfo(GUIDE_INFO& info);                                           // 填充引导信息

    void FillEnemyInfo(ENEMY_INFO& info);                                           // 填充仇人信息

    void FillMonsterDoorInfo(MONSTER_DOOR_INFO& monster_door_info);                 // 填充怪物之门信息

    void FillPlayerDoorInfo(PLAYER_DOOR_INFO& player_door_info);                    // 填充勇者争霸信息

    void FillOffilePlayerInfo(OFFLINE_PLAYER_INFO& offline_info);                   // 填充离线角色的信息

    void FillRewardInfo(REWARD_INFO& reward_info);                                  // 填充奖励池数据

    void FillBabelInfo(BABEL_INFO& babel_info);                                     // 填充通天塔信息

    void FillPayPointLotteryInfo(PAY_POINT_LOTTERY_INFO &info);                     // 填充充值积分抽奖信息

    void FillVIPOnlyDungeonInfo(VIP_ONLY_DUNGEON_INFO &info);                       // 填充VIP独享副本信息

    void FillRacingInfo(RACING_INFO& racing_info);                                  // 填充赛马信息

    void FillSignInfo(SIGN_INFO& sign_info);                                        // 填充签到信息

    void FillOnlineRewardInfo(ONLINE_REWARD_INFO& info);                            // 填充在线奖励信息

    void FillMiningInfo(MINING_INFO& mining_info);                                  // 填充采矿信息

    void FillTreasuryInfo(TREASURY_INFO& info);                                     // 填充宝库信息

    void FillGoldtreeInfo(GOLDTREE_INFO& info);                                     // 填充金像信息

    void FillAdventureInfo(ADVENTURE_INFO& info);                                   // 填充大冒险数据

    void FillAnswerInfo(ANSWER_INFO& info);                                         // 填充答题数据

    void FillWingInfo(WING_INFO& info);                                             // 填充翅膀信息

    void FillAshuraInfo(ASHURA_INFO& info);                                         // 填充修罗场信息

    void FillMallInfo(RESTRICTION_LIST& info);                                      // 填充商城信息

    void FillPassiveSkillInfo(PASSIVE_SKILL_LIST& info);                            // 填充被动技能信息

    void FillTitleInfo(TITLE_LIST& info);                                           // 填充称号信息

    void FillDayInvestmentInfo(DAY_INVESTMENT_INFO& day_investment_info);           // 填充日常投资信息

    void FillLevelInvestmentInfo(LEVEL_INVESTMENT_INFO& level_investment_info);     // 填充等级投资信息

    void FillLevelActivityInfo(LEVEL_ACTIVITY_INFO& level_activity_info);           // 填充等级活动信息

    void FillAutoFightSettingInfo(AUTO_FIGHT_SETTING_INFO& auto_fight_setting_info);// 填充自动战斗设置信息

    void FillSystemSettingInfo(SYSTEM_SETTING_INFO& system_setting_info);           // 填充系统设置信息

    void FillPlayerSettingInfo(PLAYER_SETTING_INFO& player_setting_info);           // 填充玩家设置信息

    void FillAutobuySettingInfo(AUTOBUY_SETTING_INFO& autobuy_setting_info);        // 填充自动购买设置信息

    void FillVoidFieldSettingInfo(VOID_FIELD_SETTING_INFO& void_field_setting_info);// 填充虚空幻境挂机设置信息

    void FillLivenessInfo(LIVENESS_INFO& info);                                     // 填充活跃度信息

    void FillDungeonTimesInfo(DUNGEON_TIMES_LIST& info);                            // 填充副本进入次数信息

    void FillLimitBuyInfo(LIMIT_BUY_LIST& info);                                    // 填充购买商品数量信息

    void FillSingleAnswerInfo(SINGLE_ANSWER_INFO& single_answer_info);              // 填充单人答题系统

    void FillSwordInfo(SWORD_NEW_INFO& info);                                           // 填充圣剑信息

    void FillPetFormation(PET_FORMATION_INFO& info);                                // 获得宠物阵型管理器

    void FillZombieInfo(ZOMBIE_INFO& info);

    void FillBhInfo(BH_INFO& info);                                                 // 填充勇敢的心信息

    void FillGiftCodeInfo(GIFT_CODE_INFO& info);

    void FillGuildScienceInfo(GUILD_PLAYER_SCIENCE_INFO& info);                     // 填充个人公会科技信息

    void FillShorcutInfo(SHORCUT_BAG& info);                                        // 填充收藏游戏信息

    void FillMultiserverBattleInfo(MULTISERVER_BATTLE_BAG& info);                   // 填充跨服战斗信息

    void FillInvestmentPlan(INVESTMENT_PLAN_INFO& info);                            // 填充投资理财计划信息

    void FillDonateInfo(DONATE_INFO& info);                                         // 填充捐献信息

    void FillEggInfo(EGG_ACTIVITY_INFO& info);                                      // 填充砸金蛋信息

    void FillBuyCheapInfo(BUY_CHEAP_INFO& info);                                    // 越买越便宜信息

    void FillHeadPortraitInfo(HEAD_PORTRAIT_INFO& info);                            // 填充个性头像

private:
    Player* m_player;
};


#endif // player_loader_h__
