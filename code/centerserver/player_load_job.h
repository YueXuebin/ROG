#ifndef player_load_job_h__
#define player_load_job_h__


class DBExecutor;
class DBRecorder;
class Player;

/*
 *	加载角色数据的数据库操作
 */
class PlayerLoadJob
{
    PlayerLoadJob() { m_dbExecutor = NULL; }
    ~PlayerLoadJob() {}

    DECLARE_SINGLETON(PlayerLoadJob)

    // 设置SQL执行
    void SetDBExecutor(DBExecutor* val) {m_dbExecutor = val;}

    void LoadData(uint32 gateSvrID,  uint64 loginID, const std::string& account, const std::string& platform_server_name, uint32 addrIP, uint16 addrPort);   // 加载数据

private:
    uint16 LoadModuleData(Player* player);                                  // 加载各个功能模块里面的数据

    uint16 LoadPlayerData(const std::string& account, uint64 loginID, const std::string& login_server_name);      // 加载角色数据

    bool LoadHoroscope(uint64 playerID);                                    // 加载星空图

    bool LoadSkillData(uint64 playerID);                                    // 加载技能数据

	bool LoadPassiveSkillData(uint64 playerID);                             // 加载天赋数据

    bool LoadPetData(uint64 playerID);                                      // 加载宠物

    bool LoadRideData(uint64 playerID);                                     // 加载骑宠

    bool LoadEquipData(uint64 playerID);                                    // 加载装备

    bool LoadPotionData(uint64 playerID);                                   // 加载药剂

    bool LoadFashionData(uint64 playerID);                                  // 加载时装数据

    bool LoadItemData(const uint64 playerID);                           	// 加载物品数据

    bool LoadStoreData(const uint64 & playerID);                            // 加载仓库数据

    bool LoadRandomPackageData(uint64 playerID);                                  // 加载奖数据

    bool LoadHideItemInfo(uint64 playerID);                                 // 加载隐藏物品

    bool LoadFrined(uint64 playerID);                                       // 加载好友

    bool LoadTaskData(uint64 playerID);                                     // 加载任务数据

    bool LoadGuideAndOpenSysData(uint64 playerID);                          // 加载引导和功能开启数据

    bool LoadEnemyInfo(uint64 playerID);                                    // 加载善恶信息

    bool LoadRewardPool(uint64 playerID);                                   // 加载角色奖励池

    bool LoadBabelInfo(uint64 playerID);                                    // 加载通天塔数据

    bool LoadPayPointLotteryInfo(uint64 playerID);                          // 加载充值积分抽奖数据

    bool LoadVIPOnlyDungeonInfo(uint64 playerID);                           // 加载VIP独享副本

    bool LoadBhInfo(uint64 playerID);                                       // 加载勇敢的心数据

    bool LoadDailyActivityInfo(uint64 playerID);                            // 加载日常活动信息

    bool LoadOnlineActivityInfo(uint64 playerID);                           // 加载上线活动信息

    bool LoadPlayerMailData(uint64 playerID);                               // 加载个人邮件数据

    bool LoadSystemMailData(uint64 playerID);                               // 加载系统邮件数据

    bool LoadGroupMailData(uint64 playerID);                                // 加载组邮件数据

    bool LoadMonsterDoorInfo(uint64 playerID);                              // 获得怪物之门信息

    bool LoadPlayerDoorInfo(uint64 playerID);                               // 加载勇者争霸数据

    bool LoadWingInfo(uint64 playerID);                                     // 加载翅膀信息

    bool LoadAshuraInfo(uint64 playerID);                                   // 加载修罗场信息

    bool LoadRechargeInfo(uint64 playerID);                                 // 加载充值信息

    bool LoadMallInfo(uint64 playerID);                                     // 加载商城信息

    bool LoadTitleInfo(uint64 playerID);                                    // 加载称号信息

    bool LoadDayInvestmentInfo( uint64 playerID);                           // 加载日常投资理财信息

    bool LoadLevelInvestmentInfo( uint64 playerID );                        // 加载等级投资理财信息

    bool LoadLevelActivityInfo( uint64 playerID );                          // 获得等级活动

    bool LoadSettingInfo( uint64 playerID );                                // 获得设置信息

    bool LoadLivenessInfo( uint64 playerID );                               // 加载活跃度信息

    bool LoadDungeonTimesInfo(uint64 playerID);                             // 加载副本次数信息

    bool LoadPlayerDungeonInfo(uint64 playerID);                            // 加载角色副本信息

    bool LoadLimitBuyInfo(uint64 playerID);                                 // 加载限购信息

    bool LoadGainbackResourceInfo(uint64 playerID);                         // 加载资源挽回信息

    bool LoadSwordInfo(uint64 playerID);                                    // 加载圣剑信息

    bool LoadZombieInfo(uint64 playerID);                                   // 加载僵尸生存信息

    bool LoadGiftCodeInfo(uint64 playerID);                                 // 加载礼品卡信息

    bool LoadGuildScienceInfo(uint64 playerID);                             // 加载公会科技信息

    bool LoadShorcutData( uint64 playerID );                                // 加载游戏收藏信息

    bool LoadGuildInfoForMgr(uint64 guildID);                               // 加载公会信息

    bool LoadGuildMemberForMgr(uint64 guildID, GUILD_INFO& guildInfo);      // 加载公会成员信息

    bool LoadMultiserverBattleData(uint64 playerID);                        // 加载跨服战斗信息

    bool LoadPetFormation(uint64 playerID);                                 // 加载宠物阵型信息

    bool LoadInvestmentPlan(uint64 playerID);                               // 加载投资计划信息

    bool LoadDonate(uint64 playerID);                                       // 加载捐献信息

    bool LoadEgg(uint64 playerID);                                          // 加载砸金蛋信息

    bool LoadBuyCheap(uint64 playerID);                                     // 加载越卖越便宜信息

    bool LoadPlayerStateInfo(uint64 playerID);                              // 加载角色状态信息

    bool LoadHeadPortraitInfo(uint64 playerID);                              // 加载个性图标

private:
    DBExecutor*         m_dbExecutor;
};

#endif
