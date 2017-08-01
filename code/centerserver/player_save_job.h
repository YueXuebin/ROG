#ifndef player_save_job_h__
#define player_save_job_h__

class DBExecutor;
class Player;
class PlayerSaveJob
{
public:
    PlayerSaveJob(void){ m_dbExecutor = NULL; }
    ~PlayerSaveJob(void);

    DECLARE_SINGLETON(PlayerSaveJob)

    // 设置SQL执行
    void SetDBExecutor(DBExecutor* val) {m_dbExecutor = val;}

    bool SavePlayerDataToDB(Player* player);


    bool SaveDayInvestmentInfo( Player* player );               // 保存日常投资信息

    bool SaveLevelInvestmentInfo( Player* player );             // 保存等级投资信息

    bool SaveLevelActivityInfo( Player* player );               // 保存等级活动信息


private:

    bool SavePlayerInfo(Player* player);                        // 保存角色信息

    bool SavePlayerDungeonInfo(Player* player);                 // 保存角色的副本信息

    bool SaveHoroscope(Player* player);                          // 保存星宫图信息

    bool SaveDailylActivityInfo(Player* player);                 // 保存活动信息

    bool SaveOnlineActivityInfo(Player* player);                // 保存上线活动信息

    bool SaveSkillData(Player* player);                         // 保存技能数据

	bool SavePassiveSkillInfo(Player* player);                  // 保存天赋数据

    bool SaveEquipData(Player* player);                         // 保存装备数据

    bool SavePotionData(Player* player);                        // 保存药剂数据

    bool SaveFashionInfo( Player* player );                     // 保存时装数据

    bool SavePetData(Player* player);                           //保存宠物信息

    bool SaveRideInfo(Player* player);                          //保存骑宠信息

    bool SaveItemData(Player* player);                          // 保存物品信息

    bool SaveStoreData(Player* player);                         // 保存仓库物品信息

    bool SaveRandomPackageInfo(Player* player);                       // 保存抽奖信息

    bool SaveHideItemInfo(Player* player);                      // 保存隐藏物品

    bool SaveTaskInfo(Player* player);                          // 保存任务数据

    bool SaveGuideAndOpenSysInfo(Player* player);               // 保存引导和功能开启数据

    bool SaveEnemyInfo(Player* player);                         // 保存善恶信息

    bool SaveRankBattleInfo(Player* player);                    // 保存离线战斗数据

    bool SaveOfflinePlayerInfo(Player* player);                 // 保存角色离线数据

    bool SaveOfflinePlayerRankingInfo(Player* player);          // 更新离线角色排行榜信息

    bool SaveRewardPool(Player* player);                        // 保存奖励池

    bool SaveBabelInfo(Player* player);                         // 保存通天塔

    bool SavePayPointLotteryInfo(Player *player);               // 保存充值积分抽奖信息

    bool SaveVIPOnlyDungeonInfo(Player *player);                // 保存VIP独享副本信息

    bool SaveSwordInfo(Player* player);                         // 保存圣剑信息

    bool SaveWingInfo(Player* player);                          // 保存翅膀信息

    bool SaveAshuraInfo(Player* player);                        // 保存修罗场信息

    bool SaveMallInfo(Player* player);                          // 保存商城信息

    bool SaveTitleInfo(Player* player);                         // 保存称号信息

    bool SaveSettingInfo(Player* player);                       // 保存设置信息

    bool SaveLivenessInfo( Player* player );                    // 保存活跃度信息

    bool SaveDungeonTimesInfo(Player* player);                  // 保存守卫地宫信息

    bool SaveLimitBuyInfo(Player* player);                      //保存限购

    bool SaveGainbackResourceInfo(Player* player);              //保存资源挽回信息

    bool SaveBhInfo(Player* player);                            // 保存勇敢的心数据

    bool SaveZombieInfo(Player* player);                        // 保存僵尸生存信息

    bool SaveGiftCodeInfo(Player* player);                      // 保存礼品卡信息

    bool SaveGuildScienceInfo(Player* player);                  // 保存公会信息

    bool SaveMultiserverBattleInfo( Player* player );           // 保存跨服战斗信息

    bool SaveShorcutInfo( Player* player );                     // 保存游戏收藏信息

    bool SavePetFormation(Player* player);                      // 保存宠物阵型信息

    bool SaveInvestmentPlan(Player* player);                    // 保存投资计划信息

    bool SaveDonate(Player* player);                            // 保存捐献信息

    bool SaveEgg(Player* player);                               // 保存砸金蛋

    bool SavePlayerStateInfo(Player* player);                   // 保存角色状态

    bool SaveHeadPortraitInfo(Player* player);                  // 保存个性头像

private:    
    DBExecutor*         m_dbExecutor;

    static char m_vip_buff[MAX_SQLTXT_LEN];
    static char m_open_system_buff[MAX_SQLTXT_LEN];
    static char m_guild_welfare_buff[MAX_SQLTXT_LEN];
    static char m_city_fight_buff[MAX_SQLTXT_LEN];
    static char m_fcm_buff[MAX_SQLTXT_LEN];
    static char m_vigor_buff[MAX_SQLTXT_LEN];
};
#endif 
