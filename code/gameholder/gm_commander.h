/*
 *	GM命令处理器
 */
#ifndef gm_commander_h__
#define gm_commander_h__

class Player;

extern bool    g_AIStop;        // AI停止

class GmCommander
{
    DECLARE_SINGLETON(GmCommander)
public:
    GmCommander();
    ~GmCommander();

    void    ParseGMCommand(Player* who, const std::string& cmd);

private:
    void    ParseGMCommand2(Player* who, const std::string& cmd, std::list<std::string>& cmdSplitStrings);

    // ============================================================
    void    AddDropOjbect(Player* who, std::list<std::string>& arglist);    // 添加掉落物品
    void    AddMonster(Player* who, std::list<std::string>& arglist);       // 添加怪物
    void    AddBoss(Player* who, std::list<std::string>& arglist);          // 添加Boss
    void    AddOfflinePlayer(Player* who, std::list<std::string>& arglist); // 添加离线玩家
    void    KillMonster(Player* who, std::list<std::string>& arglist);      // 杀死怪物
    void    KillBoss(Player* who, std::list<std::string>& arglist);         // 杀死boss
    void    AiStop(Player* who, std::list<std::string>& arglist);           // 开启或关闭AI
    void    AiStopPet(Player* who, std::list<std::string>& arglist);        // 开启或关闭佣兵AI
    void    DamageMe(Player* who, std::list<std::string>& arglist);         // 伤害自己
    void    Damage(Player* who, std::list<std::string>& arglist);           // 伤害周围的敌人
    void    NoDamage(Player* who, std::list<std::string>& arglist);         // 不受伤害
    void    FullHpMp(Player* who, std::list<std::string>& arglist);         // 满血满蓝
    void    UpdateActivity(Player* who, std::list<std::string>& arglist);   // 更新活动
    void    AddActivity(Player* who, std::list<std::string>& arglist);      // 添加活动
    void    DeleteActivity(Player* who, std::list<std::string>& arglist);   // 删除活动

    // ============================================================
    void    SetCareer(Player* who, std::list<std::string>& arglist);        // 职业
    void    SetGroup(Player* who, std::list<std::string>& arglist);         // group
    void    AddExp(Player* who, std::list<std::string>& arglist);           // 添加经验
    void    LevelUp(Player* who, std::list<std::string>& arglist);          // 升级
	void	AddPassiveSkillPoint(Player* who, std::list<std::string>& arglist); // 添加天赋点
    void	ResetPassiveSkillPoint(Player* who, std::list<std::string>& arglist); // 天赋洗点
    void    SetLevel(Player* who, std::list<std::string>& arglist);         // 设置等级
    void    SetGamePoint(Player* who, std::list<std::string>& arglist);     // 设置游戏币
    void    AddGamePoint(Player* who, std::list<std::string>& arglist);     // 添加游戏币
    void    SetMoneyPoint(Player* who, std::list<std::string>& arglist);    // 设置金币
    void    AddMoneyPoint(Player* who, std::list<std::string>& arglist);    // 添加金币
    void    AddBindMoneyPoint(Player* who, std::list<std::string>& arglist);// 添加绑定金币
    void    SetSoulPoint(Player* who, std::list<std::string>& arglist);     // 设置精魂
    void    AddSoulPoint(Player* who, std::list<std::string>& arglist);     // 添加精魂
    void    SetStarPoint(Player* who, std::list<std::string>& arglist);     // 设置星魂
    void    AddStarPoint(Player* who, std::list<std::string>& arglist);     // 添加星魂
    void    SetHonorPoint(Player* who, std::list<std::string>& arglist);    // 设置荣誉
    void    AddHonorPoint(Player* who, std::list<std::string>& arglist);    // 添加荣誉
    void    SetGuildPoint(Player* who, std::list<std::string>& arglist);    // 设置公会贡献点
    void    AddGuildPoint(Player* who, std::list<std::string>& arglist);    // 添加公会贡献点
    void    AddGuildExp(Player* who, std::list<std::string>& arglist);      // 添加工会经验
    void    SetVigor(Player* who, std::list<std::string>& arglist);         // 设置体力
    void    AddVigor(Player* who, std::list<std::string>& arglist);         // 添加体力
    void    SetVipCard(Player* who, std::list<std::string>& arglist);       // 设置VIP时效
    void    ActiveExperienceVip(Player* who, std::list<std::string>& arglist);  // 激活体验VIP
    void    SetGrowthPoint(Player* who, std::list<std::string>& arglist);   // 设置成长点
    void    AddGrowthPoint(Player* who, std::list<std::string>& arglist);   // 添加成长点
    void    SetFcmFlag(Player* who, std::list<std::string>& arglist);       // 设置防沉迷标记
    void    SetFcmInGameTime(Player* who, std::list<std::string>& arglist); // 设置防沉迷上线时间
    void    SetFcmOutGameTime(Player* who, std::list<std::string>& arglist);// 设置防沉迷下线时间
    void    SetGMRight(Player* who, std::list<std::string>& arglist);       // 设置GM权限

    // ============================================================
    void    SetMaxHp(Player* who, std::list<std::string>& arglist);
    void    SetMaxMp(Player* who, std::list<std::string>& arglist);
    void    SetResHp(Player* who, std::list<std::string>& arglist);
    void    SetResMp(Player* who, std::list<std::string>& arglist);
    void    SetAttack(Player* who, std::list<std::string>& arglist);
    void    SetDefence(Player* who, std::list<std::string>& arglist);
    void    SetDodge(Player* who, std::list<std::string>& arglist);
    void    SetEnergyShield(Player* who, std::list<std::string>& arglist);  // 设置能量盾
    void    SetHitRate(Player* who, std::list<std::string>& arglist);
    void    SetCriticalAttack(Player* who, std::list<std::string>& arglist);
    void    SetCriticalDamageAttack(Player* who, std::list<std::string>& arglist);
    void    SetAttackSpeed(Player* who, std::list<std::string>& arglist);   // 设置攻击速度
    void    SetCastSpeed(Player* who, std::list<std::string>& arglist);     // 设置施法速度
    void    SetMoveSpeed(Player* who, std::list<std::string>& arglist);     // 设置移动速度
    void    SetFireDefence(Player* who, std::list<std::string>& arglist);
    void    SetIceDefence(Player* who, std::list<std::string>& arglist);
    void    SetThunderDefence(Player* who, std::list<std::string>& arglist);
    void    SetBlock(Player* who, std::list<std::string>& arglist);     // 设置格挡
    void    SetMagicBlock(Player* who, std::list<std::string>& arglist);    // 设置法术格挡

    void    SetCriticalRed(Player* who, std::list<std::string>& arglist);
    void    SetCriticalDamageDef(Player* who, std::list<std::string>& arglist);

    // ============================================================
    void    EquipOpenSlot(Player* who, std::list<std::string>& arglist);    // 装备开孔
    void    UpEquipLevel(Player* who, std::list<std::string>& arglist);     // 设置装备升级
    void    UpEquipStar(Player* who, std::list<std::string>& arglist);      // 设置装备升星
    void    FinishEquipStar(Player* who, std::list<std::string>& arglist);  // 完成装备升星
    void    UpEqupQuality(Player* who, std::list<std::string>& arglist);    // 设置装备升品质
    void    AddFollowPet(Player* who, std::list<std::string>& arglist);     // 添加跟宠
    void    MinMarkRide(Player* who, std::list<std::string>& arglist);      // 骑宠刻印
    void    AddFashion(Player* who, std::list<std::string>& arglist);       // 添加时装
    void    FashiionCompose(Player* who, std::list<std::string>& arglist);  // 合成时装
    void    EquipInlaidJewel(Player* who, std::list<std::string>& arglist); // 宝石镶嵌
    void    OnSynthetizeJewelReq(Player* who, std::list<std::string>& arglist);// 宝石合成
    void    OnUpgradeJewelReq(Player* who, std::list<std::string>& arglist);// 宝石提升
    void    AddProtectSoul(Player* who, std::list<std::string>& arglist);  // 添加护魂

    // ============================================================
    void    AddItem(Player* who, std::list<std::string>& arglist);          // 添加物品
    void    DelItemByID(Player* who, std::list<std::string>& arglist);      // 删除物品
    void    DelItemByPos(Player* who, std::list<std::string>& arglist);     // 删除物品
    void    ClearBag(Player* who, std::list<std::string>& arglist);         // 删除物品
    void    MoveItem(Player* who, std::list<std::string>& arglist);         // 移动物品

    // ============================================================
    void    TaskTalk(Player* who, std::list<std::string>& arglist);         // 与NPC对话
    void    TaskKillMonster(Player* who, std::list<std::string>& arglist);  // 与任务击杀怪物
    void    OpenDungeon(Player* who, std::list<std::string>& arglist);      // 开启副本
    void    ClearDungeon(Player* who, std::list<std::string>& arglist);     // 通关副本
    void    AddTask(Player* who, std::list<std::string>& arglist);          // 添加任务
    void    TaskComplete(Player* who, std::list<std::string>& arglist);     // 强制完成任务
    void    TakeTask(Player* who, std::list<std::string>& arglist);         // 接受任务
    void    TaskChangeDungeon(Player* who, std::list<std::string>& arglist);// 切换副本
    void    TaskRewards( Player* who, std::list<std::string>& arglist );    // 请求任务奖励
    void    TaskQuality( Player* who, std::list<std::string>& arglist );    // 提高任务品质
    void    TaskOneComplete( Player* who, std::list<std::string>& arglist );// 立即完成任务
    void    TaskRefresh( Player* who, std::list<std::string>& arglist );    // 刷新讨伐令任务
    void    TaskReceiveBox( Player* who, std::list<std::string>& arglist ); // 领取宝箱
    void    TaskreFreshTime( Player* who, std::list<std::string>& arglist );// 刷新日常等计数
    void    TaskGuardRefresh( Player* who, std::list<std::string>& arglist );// 变更护卫任务

    // ===========================================================
    void    RacingRefresh(Player* who, std::list<std::string>& arglist);    // 刷新赛马信息
    void    RacingEnter(Player* who, std::list<std::string>& arglist);      // 参加赛马
    void    RacingReward(Player* who, std::list<std::string>& arglist);     // 领取赛马奖励

    // ============================================================
    void    ClearTeamDungeon(Player* who, std::list<std::string>& arglist); // 清除队伍副本信息
    void    IsGameStart(Player* who, std::list<std::string>& arglist);      // 游戏是否开启

    // ============================================================
    void    ChatSystem(Player* who, std::list<std::string>& arglist);       // 发布系统消息
    void    ChatMarquee(Player* who, std::list<std::string>& arglist);      // 发布跑马灯

    void    OnDungeonClear(Player* who, std::list<std::string>& arglist);      // 副本通关
    void    OnAddRide(Player* who, std::list<std::string>& arglist);           // 添加骑宠

    // ============================================================
    void    AddMail(Player* who, std::list<std::string>& arglist);          // 添加邮件

    // ============================================================
    void    Sign(Player* who, std::list<std::string>& arglist);             // 签到

    // ============================================================
    void    TracePlayerList(Player* who, std::list<std::string>& arglist);  // 玩家列表
    void    TraceDungeonList(Player* who, std::list<std::string>& arglist); // 副本列表
    void    TracePlayerNum(Player* who, std::list<std::string>& arglist);   // 玩家数量

    // ============================================================
    void    ReloadMall(Player* who, std::list<std::string>& arglist);       // 重新加载商城信息
    void    MallBuyReq(Player* who, std::list<std::string>& arglist);       // 商城购买请求
    void    MallInfoReq(Player* who, std::list<std::string>& arglist);      // 商城信息请求

    void    BindMoneyPointBuyReq(Player* who, std::list<std::string>& arglist); // 绑定魔石商店购买请求

    void    AddTitle(Player* who, std::list<std::string>& arglist);         // 添加称号
    void    EquipTitle(Player* who, std::list<std::string>& arglist);       // 装备称号

    void    RefreshRanking(Player* who, std::list<std::string>& arglist);   // 重新生成排行榜
    void    useskill(Player* who, std::list<std::string>& arglist);

    void    AskBattleUnitProperty(Player* who, std::list<std::string>& arglist);    // 查询角色属性
    void    KillAll(Player* who, std::list<std::string>& arglist);          // 杀死所有
    void    GetBableRewrd(Player* who, std::list<std::string>& arglist);
    void    Changedungeon(Player* who, std::list<std::string>& arglist);
    
    void    CalcPlayerProp(Player* who, std::list<std::string>& arglist);   // 重新计算玩家属性

    void    OnFollowEvoluti(Player* who, std::list<std::string>& arglist);  // 进化

    void    DayInvestment(Player* who, std::list<std::string>& arglist);    // 日常投资
    void    LevelInvestment(Player* who, std::list<std::string>& arglist);  // 等级投资
    void    InvestmentManager(Player* who, std::list<std::string>& arglist);// 投资管理

    void    LevelActivity(Player* who, std::list<std::string>& arglist);    // 等级活动
    void    SpecialActivity(Player* who, std::list<std::string>& arglist);    // 等级活动

    void    AcceptLivenessReward(Player* who, std::list<std::string>& arglist);    // 活跃度
    void    AddBattleSoul(Player* who, std::list<std::string>& arglist);    // 添加战魂

    void    RaidsMonsterDoor(Player* who, std::list<std::string>& ardlist); // 怪物之门扫荡


    void  RightNowTrain(Player* who, std::list<std::string>& arglist);      // 坐骑立即完成
    void  StartTrain(Player* who, std::list<std::string>& arglist);
    void  SetTrainModels(Player* who, std::list<std::string>& arglist);
    void  EndTrain(Player* who, std::list<std::string>& arglist);
    void  OpenTrainPos(Player* who, std::list<std::string>& arglist);       // 设置训练位
    void  RideAbsorb(Player* who, std::list<std::string>& arglist);
    void  UpdateRide(Player* who, std::list<std::string>& arglist);         // 刷新坐骑
    void  CatchRide(Player* who, std::list<std::string>& arglist);          // 抓取坐骑
    void  RideSendGif(Player* who, std::list<std::string>& arglist);        // 赠送馈礼

    // ============================================================
    void    SetGuildBossTime(Player* who, std::list<std::string>& ardlist); // 设置工会BOSS

    void    onAdvanceReq(Player* who, std::list<std::string>& arglist);     // 升级或一键升级翅膀
    void    onActivateReq(Player* who, std::list<std::string>& arglist);    // 激活翅膀
    void    LockSpendCondition(Player* who, std::list<std::string>& arglist);
    void    OnHoroscopeLevelUpReq(Player* who, std::list<std::string>& arglist);// 星空图升级

    void    RanomLeveupExp(Player* who, std::list<std::string>& arglist);   // 激发一键激发，升阶
    void    OnFollowPetSwallow(Player* who, std::list<std::string>& arglist);// 跟宠吞噬

    // 城市争夺战
    void    EnterCityFight(Player* who, std::list<std::string>& arglist);   // 进入城市争夺战
    void    CityFightCityInfo(Player* who, std::list<std::string>& arglist);    // 城市争夺战排行

    void AddRecharge(Player* who, std::list<std::string>& arglist );            // 模拟添加充值数

    // 圣光生存战
    void CitySurvival(Player* who, std::list<std::string>& arglist);

    void StartSoulRains(Player* who, std::list<std::string>& arglist);
    void FinishSoulRains(Player* who, std::list<std::string>& arglist);
    void StopSoulRains(Player* who, std::list<std::string>& arglist);

    void CalSingleAnswerInt(Player* who, std::list<std::string>& arglist);
    void CalSingleAnswerQuesitonReq(Player* who, std::list<std::string>& arglist);
    void CalSingleAnswerResultReq(Player* who, std::list<std::string>& arglist);
    void CalSingleAnswerTimeCDReq(Player* who, std::list<std::string>& arglist);

    //vip福利领取
    void GetVipGiftReq(Player* who, std::list<std::string>& arglist);

    void OnRequestZombieInfo(Player* who, std::list<std::string>& arglist);

    void SetMultiserverIntegralPoint(Player* who, std::list<std::string>& arglist);     // 设置跨服积分
    void SetMultiserverExploitPoint(Player* who, std::list<std::string>& arglist);      // 设置跨服功勋点

    void AddCrossServer(Player* who, std::list<std::string>& arglist);                  // 添加跨服CenterServer

    void SetMultiserverAlliancePoint(Player* who, std::list<std::string>& arglist);     // 设置跨服联盟值
    void SetMultiserverSeasonPoint(Player* who, std::list<std::string>& arglist);       // 设置跨服赛季币

    void SetSkillFashion(Player* who, std::list<std::string>& arglist);                 // 设置化身技能

    void SetActivityRank(Player* who, std::list<std::string>& arglist);                 // 排行活动刷新

    void SetLikeInfo(Player* who, std::list<std::string>& arglist);                     // 点赞测试

    void SetEggInfo(Player* who, std::list<std::string>& arglist);                      // 砸金蛋添加蛋

    void ReloadLua(Player* who, std::list<std::string>& arglist);                       // 重新加载所有lua脚本

    void SetResetFestival(Player* who, std::list<std::string>& arglist);                // 重新加载活动

    void SetFormula(Player* who, std::list<std::string>& arglist);                // 测试公式

    typedef void (GmCommander::*GMFunc)(Player* , std::list<std::string>&);
    typedef std::map<std::string, GMFunc> GMFuncMap;
    GMFuncMap m_gmCmdMap;
};


#endif // gm_commander_h__
