#ifndef parse_module_job_h__
#define parse_module_job_h__
class DBRecorder;

//解析各个模块功能
class PlayerLoadParser
{
    PlayerLoadParser() {}
    ~PlayerLoadParser() { }

    DECLARE_SINGLETON(PlayerLoadParser)

public:
    bool ParseHoroscope(DBRecorder& res, uint64 playerID);          // 解析星空图

    bool ParseSkillInfo(DBRecorder& res, uint64 playerID);          // 获得技能信息

	bool ParsePassiveSkillInfo(DBRecorder& res, uint64 playerID);	// 获得天赋信息

    bool ParsePetInfo(DBRecorder& res, uint64 playerID);            // 获得跟宠信息

    bool ParseRideInfo(DBRecorder& res, uint64 playerID);           // 获得骑宠信息

    bool ParseEquipInfo(DBRecorder& res, const uint64& playerID);   // 获得装备信息

    bool ParsePotionInfo(DBRecorder& res, const uint64& playerID);  // 获得药剂信息

    bool ParseFashionInfo( DBRecorder& res, uint64 playerID );      // 获得时装信息;

    bool ParseItemInfo(DBRecorder& res, const uint64& playerID);    // 获得物品信息

    bool ParseStoreInfo(DBRecorder& res, const uint64& playerID);    //获得仓库物品信息

    bool ParseRandomPackageInfo( DBRecorder& res, uint64 playerID );      // 获得抽奖信息

    bool ParseHideItemInfo( DBRecorder& res, uint64 playerID );     // 获得隐藏物品

    bool ParseFriend(DBRecorder& res, uint64 playerID );            // 获得好友数据

    bool ParseFriendHasMe(DBRecorder& res, uint64 playerID );       // 获得我是哪些人的好友数据

    bool ParseTaskInfo(DBRecorder& res, uint64 playerID);           // 获得任务信息

    bool ParseGuideAndOpenSysInfo(DBRecorder& res, uint64 playerID);// 获得引导和功能开启信息

    bool ParsetEnemyInfo(DBRecorder& res, uint64 playerID);         // 获得善恶信息

    bool ParseRewardPool(DBRecorder& res, uint64 playerID);         // 获得角色奖励池

    bool ParseBabelInfo(DBRecorder& res, uint64 playerID);          // 获得通天塔信息

    bool ParsePayPointLotteryInfo(DBRecorder &res, uint64 playerID);    //获取充值积分抽奖信息

    bool ParseVIPOnlyDungeonInfo(DBRecorder &res, uint64 playerID); // 获取VIP独享副本信息

    bool ParseBhInfo(DBRecorder& res, uint64 playerID);             // 加载勇敢的心数据

    bool ParseDailylActivityInfo(DBRecorder& res, uint64 playerID); // 获得活动信息

    bool ParseOnlineActivityInfo(DBRecorder& res, uint64 playerID); // 获得在线活动信息

    bool ParsePlayerMailData(DBRecorder& res, uint64 playerID);          // 加载个人邮件数据

    bool ParseSystemMailData(DBRecorder& res, uint64 playerID);    // 加载系统邮件数据

    bool ParseGroupMailData(DBRecorder& res, uint64 playerID);    // 加载组邮件数据

    bool ParseMonsterDoorInfo(DBRecorder& res, uint64 playerID); // 获得怪物之门信息

    bool ParsePlayerDoorInfo(DBRecorder& res, uint64 playerID);  //获得勇者争霸信息

    bool ParseWingInfo(DBRecorder& res, uint64 playerID);        // 加载翅膀信息

    bool ParseAshuraInfo(DBRecorder& res, uint64 playerID);      // 加载修罗场信息

    bool ParseRechargeInfo(DBRecorder& res, uint64 playerID);    // 获得充值信息

    bool ParseMallInfo(DBRecorder& res, uint64 playerID);       // 加载商城信息

    bool ParseTitleInfo(DBRecorder& res, uint64 playerID);      // 获得称号信息

    bool ParseDayInvestmentInfo(DBRecorder& res, uint64 playerID);   //每日投资理财

    bool ParseLevelInvestmentInfo( DBRecorder& res, uint64 playerID );//投资理财等级

    bool ParseLevelActivityInfo( DBRecorder& res, uint64 playerID ); //获得等级活动

    bool ParseSettingInfo(DBRecorder& res, uint64 playerID);         //设置系统

    bool ParseLivenessInfo( DBRecorder& res, uint64 playerID );      //加载活跃度信息

    bool ParseDungeonTimesInfo(DBRecorder& res, uint64 playerID);    //加载副本次数信息

    bool ParsePlayerDungeonInfo(DBRecorder& res, uint64 playerID);         //获得副本信息

    bool ParseLimitBuyInfo(DBRecorder& res, uint64 playerID);        //获得购买限制

    bool ParseGainbackResourceInfo(DBRecorder& res, uint64 playerID);// 加载资源挽回信息的回调

    bool ParseSwordInfo(DBRecorder& res, uint64 playerID);           // 加载圣剑信息

    bool ParseZombieInfo(DBRecorder& res, uint64 playerID);          //加载僵尸信息

    bool ParseGiftCodeInfo(DBRecorder& res, uint64 playerID);        //加载礼品卡使用信息

    bool ParseGuildScienceInfo(DBRecorder& res, uint64 playerID);    //加载公会科技信息

    bool ParseShorcutInfo( DBRecorder& res, uint64 playerID );       // 获得收藏有理

    void ParseGuildInfoForMgr(DBRecorder& res, GUILD_INFO& guildInfo);                // 解析公会信息

    void ParseGuildMemberForMgr(DBRecorder& res, GUILD_INFO& guildInfo); // 加载公会成员信息

    void ParseVipInfo(Json::Value& json_val, VIP_INFO& vipInfo);

    bool ParseMultiserverBattleInfo(DBRecorder& res, uint64 playerID);      // 加载跨服战斗信息

    bool ParsePetFormation(DBRecorder& res, uint64 playerID);               // 加载宠物阵型信息

    bool ParseInvestmentPlan(DBRecorder& res, uint64 playerID);                 // 加载投资计划信息

    bool ParseDonate(DBRecorder& res, uint64 playerID);                         // 加载捐献信息

    bool ParseEgg(DBRecorder& res, uint64 playerID);                            // 加载砸金蛋信息

    bool ParseBuyCheap(DBRecorder& res, uint64 playerID);                       // 加载越买越便宜信息

    bool ParsePlayerStateInfo(DBRecorder& res, uint64 playerID);                // 加载角色状态信息

    bool ParseHeadPortraitInfo(DBRecorder& res, uint64 playerID);                // 加载个性图标


private:

};

#endif
