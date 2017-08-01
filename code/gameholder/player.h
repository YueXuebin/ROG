/*
*	玩家角色对象
*/
#ifndef player_h__
#define player_h__

#include "creature.h"
#include "item_base.h"
#include "property.h"

class LimitBuy_Manager;
class PlayerDungeonManager;
class SoulDungeonManager;
class SkillInfoManager;
class ItemManager;
class EquipManager;
class TaskManager;
class GuideManager;
class OpenSystemManager;
class PlayerDoorManager;
class LightCityManager;
class MailManager;
class HoroscopeManager;
class FriendManager;
class ChatManager;
class BattleRecorder;
class AshuraRecorder;
class BabelManager;
class HorseRacingManager;
class SignManager;
class VigorManager;
class OnlineRewardManager;
class GmCommander;
struct EquipPropSet;
class PlayerPropNotifier;
class RideManager;
class MiningManager;
class TreasuryManager;
class AdventureManager;
class WingManager;
class OnRecvFilter;
class TitleManager;
class RechargeManager;
class ShopManager;
class ShopServiceManager;
class GoldtreeManager;
class ExpendManager;
class DayInvestmentManager;
class LevelInvestmentManager;
class LevelActivityManager;
class SpecialActivityManager;
class PassiveSkillManager;
class SettingManager;
class Dungeon;
class LivenessManager;
class DungeonTimesRecorder;
class GameplayTimesNotifier;
class StoneTurnGoldManager;
class SingleAnswerManager;
class GainBackResourceManager;
class FashionManager;
class SwordManager;
class ZombieManager;
class BhManager;
class EnemyManager;
class SightManager;
class GuildScienceRecorder;
class PackageManager;
class MultiserverBattleInfoManager;
class VIPManager;
class FCMManager;
class ShorcutManager;
class PetSoulManager;
class PetFormationManager;
class PayPointLotteryManager;
class VIPOnlyDungeonManager;
class InvestmentPlanManager;
class OfflineVigorManager;
class DonateManager;
class AnswerManager;
class BattleStateManager;
class PlayerTeamManager;
class PlayerLoader;
class PlayerSaver;
class EggManager;
class ContrastManager;
class BuyCheapMgr;
class PetManager;
class Pet;
class HeadPortraitManager;
class BagManager;
class StoreManager;
class EquipStrengthenManager;
class GemManager;
class SkillStrengthenManager;
class AlchemyManager;
class PotionManager;
class DecomposeManager;
class RandomPackageManager;
class ItemController;

typedef std::vector<Pet*> PetList;

class Player : public Creature
{
    CnDeclareRTTI
public:
    friend class GmCommander;
    friend class OnRecvFilter;
    friend class ExpendManager;
    friend class PlayerLoader;
    friend class PlayerSaver;

    Player();
    virtual ~Player();

    void OnRecv(uint32 msgID, void* data);                                          // 接收网络数据
    void OnRecvBaseProp(uint32 msgID, void* data);                                  // 基础属性消息
    void OnRecvSkill(uint32 msgID, void* data);                                     // 技能消息
    void OnRecvBattle(uint32 msgID, void* data);                                    // 战斗消息
    void OnRecvTrigger(uint32 msgID, void* data);                                   // 触发器消息
    void OnRewardTableReq(uint32 msgID, void* data);                                // 得到一种奖励
    void OnRecvOfflineInfoReq(uint32 msgID, void* data);                            // 得到离线玩家信息
    void OnRecvFcm(uint32 msgID, void* data);                                       // 防沉迷消息
    void OnRecvActivationCode(uint32 msgID, void* data);                            // 使用激活码消息
    void OnRecvResurgenceReq(uint32 msgID, void* data);                             // 复活请求
    void OnVoidFieldRecv(uint32 msgID, void* data);                                 // 虚空幻境消息
    void OnRecvGuild(uint32 msgID, void* data);                                     // 公会信息

    void ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 ); // 接收事件

    int32 JudegeRewTableNum(int32 reason, REWARD_ITEM* rewardItem, int32 reasionarg);       // 判断奖励池里某种物品的个数
    void SetRewTableItemNum(int32 reason, REWARD_ITEM* rewardItem, int32 reasionarg);       // 设置奖励池的某个物品个数

    void SendMyselfEvent(int32 event_type, int32 arg1, int32 arg2, int32 delayTime = 0);     // 发送事件

    // ============================================================
    void PlayerClear();                                                             // 清空

    PlayerPropNotifier* GetPlayerPropNtf() {return m_PlayerPropNotifier;}

    PlayerLoader*       GetPlayerLoader() {return m_PlayerLoader;}
    PlayerSaver*        GetPlayerSaver() {return m_PlayerSaver;}

    void Load(uint64 playerID, DELIVER_PLAYER_INFO_REQ* recvMsg);                   // 加载角色数据
    void Save(WRITEBACK_PLAYER_INFO_REQ& sendMsg);                                  // 保存角色数据

    void OnLastSet();

    // 基础属性
    void FillPropInfo(PROPERTY_INFO& info, uint32 unionID);                         // 填充角色属性
    // 装备信息
    EquipManager*   GetEquipMgr() {return m_equip_mgr;}                             // 获得装备
    // 时装信息
    FashionManager*   GetFashionManager() {return m_fashion_mgr; }                  // 获得时装
    // 勇敢的心
    BhManager*   GetBhManager() {return m_bh_mgr; }                                 // 获得勇敢的心
    // 包裹
    ItemManager*    GetItemManager() { return m_item_mgr; }                         // 获得包裹
    // 抽奖包裹
    PackageManager* GetPackageManager() { return m_package_mgr; }                   // 抽奖包裹
    // 善恶系统
    EnemyManager*   GetEnemyMgr() { return m_enemy_mgr; }                           // 获得善恶管理器
    // 好友
    void LoadFriendInfo(FRIEND_LIST& info);                                         // 加载好友信息发
    // 任务
    TaskManager*    GetTaskManager() {return m_task_mgr;}                           // 获得任务管理器
    void ReloadTask();                                                              // 重新加载节日任务
    // 引导
    GuideManager*   GetGuideManager() {return m_guide_mgr;}                         // 获得引导管理器
    // 功能开启
    OpenSystemManager*  GetOpenSystemManager() {return m_opensys_mgr;}              // 功能开启管理器
    // 勇者争霸
    PlayerDoorManager*  GetPlayerDoorManager() {return m_player_door_mgr;}          // 获得勇者争霸信息
    // 离线角色信息
    void FillOffilePlayerInfo(OFFLINE_PLAYER_INFO& offline_info);                   // 填充离线角色的信息
    // 星空图
    HoroscopeManager*   GetHoroscopeManager() {return m_horoscope_mgr;}             // 获得星空图管理器
    uint32 GetHoroscopeLevel();
    // 技能
    SkillInfoManager*   GetSkillInfoMgr() {return m_skill_info_mgr;}                 // 获得技能信息管理器
    // 切换地图管理器
    PlayerDungeonManager*   GetPlayerDungeonManager() { return m_player_dungeon_mgr; }  // 获得角色副本管理器
    // 精魂副本管理器
    SoulDungeonManager*      GetSoulDungeonManager();
    // 充值积分抽奖
    PayPointLotteryManager* GetPayPointLottery() { return m_pay_point_lottery; }    // 获得充值积分抽奖管理器
    // VIP独享副本
    VIPOnlyDungeonManager* GetVIPOnlyDungeon() { return m_vip_only_dungeon; }       // 获得VIP独享副本管理器
    // 宠物竞赛
    HorseRacingManager* GetRacingManager() {return m_racing_mgr;}                   // 获得赛马管理器
    // 签到
    SignManager*    GetSignManager() {return m_sign_mgr;}                           // 获得签到管理器
    // 在线奖励
    OnlineRewardManager*    GetOnlineRewardManager() { return m_online_reward_mgr; }    // 获得在线奖励管理器
    // 坐骑
    RideManager*    GetRideManager() {return m_ride_mgr;}
    // 采矿
    MiningManager* GetMiningManager() {return m_mining_mgr;}                        // 获得采矿管理器
    // 宝库
    TreasuryManager* GetTreasuryManager() {return m_treasury_mgr;}                  // 获得宝库管理器
    // 金像
    GoldtreeManager* GetGlodtreeManager() {return m_goldtree_mgr;}                  // 获得金像管理器
    // 答题数据
    AnswerManager* GetAnswerManager() {return m_answer_mgr;}
    // 翅膀
    WingManager* GetWingManager() {return m_wing_mgr;}                              // 获得翅膀管理器
    // 圣剑
    SwordManager* GetSwordManager() {return m_sword_mgr;}                           // 获得圣剑管理器
    // 捐献
    DonateManager* GetDonateManager(){return m_donate_mgr;}                         // 获得捐献管理器
    // 宠物护魂
    PetSoulManager* GetPetSoulManager() {return m_pet_soul_mgr;}                    // 获得宠物护魂管理器
    // 宠物阵型
    PetFormationManager* GetPetFormation() {return m_pet_formation_mgr;}            // 获得宠物阵型管理器
    // 修罗场
    AshuraRecorder* GetAshuraRecorder() {return m_ashura_recorder;}                 // 获得修罗场管理器
    // 称号
    TitleManager * GetTitleManager() {return m_title_mgr;}
    // 充值数据
    RechargeManager* GetRechargeManager() {return m_recharge_mgr;}                  // 获得充值管理器
    void LoadRechargeInfo(const PLAYER_RECHARGE_INFO& info);                        // 加载充值信息
    void SendRechargeEvent();                                                       // 发送充值任务事件
    // 商城
    ShopManager* GetShopManager() {return m_shop_mgr;}                              // 获得商城管理器
    // 商城服务
    ShopServiceManager* GetShopServiceManager() {return m_shop_service_mgr;}   // 获得商城服务管理器
    // 日常投资
    DayInvestmentManager* GetDayInvestmentManager() {return m_day_investment_mgr;}  // 获得日常投资信息
    // 等级投资
    LevelInvestmentManager* GetLevelInvestmentManager() {return m_level_investment_mgr;}    // 获得等级投资信息
    // 等级活动
    LevelActivityManager* GetLevelActivityManager() {return m_level_activity_mgr;}          // 获得等级活动信息
    // 单人答题
    SingleAnswerManager* GetSingleAnswerManager() {return m_single_answer_mgr;}             // 获得单人答题系统
    // 勇者问答
    ANSWER_INFO& GetAnswerInfo();                                                           // 获得答题信息
    // 被动技能
    PassiveSkillManager* GetPassiveSkillManager() {return m_passive_skill_mgr;}     // 获得被动技能管理器
    // 收藏游戏
    ShorcutManager*   GetShorcutManager() {return m_shorcut_mgr; }                  // 收藏游戏
    // 战斗状态管理器
    BattleStateManager* GetBattleStateManager() {return m_battle_state_manager;}    // 战斗状态管理器
    // 设置
    SettingManager* GetSettingManager() {return m_setting_mgr;}                     // 获得设置管理器
    // 活跃度
    LivenessManager* GetLivenessManager() {return m_liveness_mgr;}                  // 获得活跃度管理器
    // 副本进入次数
    DungeonTimesRecorder* GetDungeonTimesManager() {return m_dungeon_times_mgr;}    // 获得副本进入次数管理器
    // 限购
    LimitBuy_Manager* GetLimitBuyManager() {return m_limitbuy_mgr;}                 // 获得购买商品数量管理器
    // 公会科技
    GuildScienceRecorder* GetGuildScienceManager() {return m_guild_science_mgr;}            // 获得个人公会科技管理器
    // 跨服战斗信息
    MultiserverBattleInfoManager* GetMultiserverBattleInfoManager() { return m_multiserver_battle_info; }   // 跨服战斗信息
    // 组队
    PlayerTeamManager* GetTeamManager() { return m_team_manager; }                  // 组队信息
    // 体力
    VigorManager*    GetVigorManager() {return m_vigor_mgr;}                    // 获得体力管理器
    void LoadVigorInfo(const VIGOR_INFO& vigor_info);                           // 加载体力信息
    void FillVigorInfo(VIGOR_INFO& vigor_info);                                 // 填充体力信息
    // 离线体力
    OfflineVigorManager* GetOffLineVigorManager() {return m_offline_vigor_mgr;}                        // 离线体力 
    // 投资理财计划
    InvestmentPlanManager* GetInvestmentPlan() {return m_investment_plan_mgr;}      // 获得投资理财计划管理器
    // 点石成金
    StoneTurnGoldManager* GetStoneTurnGoldMgr() { return m_stone_turn_gold_mgr; }   // 点石成金系统
    // 僵尸生存
    ZombieManager* GetZombieManager() { return m_zombie_mgr; }
    // 药剂使用
    PotionManager* GetPotionUseManager() { return m_potion_use_mgr;}

    // 分解
    DecomposeManager* GetDecomposeMgr() {return m_decompose_mgr;}

    // 游戏通知notify(加好友、邮件等)
    GameplayTimesNotifier* GetNotifyManager() {return m_notify_mgr;}
    
    // 砸金蛋活动
    EggManager* GetEggManager() { return m_egg_manager; }                           // 砸金蛋活动

    BuyCheapMgr* GetBuyCheapMgr() { return m_buy_cheap_mgr; }                       // 越买越便宜

    PetManager* GetPetMgr() {return m_pet_mgr;}

    HeadPortraitManager* GetHeadPortraitMgr() {return m_headPortrait_mgr;}          // 个性头像

    BagManager* GetBagMgr() {return m_bag_mgr;}
    StoreManager* GetStoreMgr() {return m_store_mgr;}

    bool NeedDisappearTrigger(uint32 trigger_id);                                   // 是否需要隐藏Trigger

    BattleRecorder* GetBattleRecorder() {return m_battle_recorder;}                 // 获得战斗信息

    RandomPackageManager* GetRandomPackageMgr() {return m_random_package_mgr;}

    ItemController* GetItemController() {return m_item_controller;}

    void    SetPropDirty();

    uint32  GetMainTaskID();                                                        // 返回主线任务ID

    bool    IsRedName();                                                            // 是否为红名

    bool    IsPlayerCanSee(SceneObject* obj);                                         // 是否可以看见对象

    // 力量
    int32   GetStrength() {return m_strength;}
    void    SetStrength(int32 val) {m_strength = val; SetPropDirty();}
    void    ClearStrength() {m_strength = 0;}

    // 敏捷
    int32   GetAgility() { return m_agility; }
    void    SetAgility(int32 val) { m_agility = val; SetPropDirty();}
    void    ClearAgility() { m_agility = 0; }

    // 精神
    int32   GetSpirit() { return m_spirit; }
    void    SetSpirit(int32 val) { m_spirit = val; SetPropDirty();}
    void    ClearSpirit() { m_spirit = 0; }

    // 体质
    int32   GetConstitution() { return m_constitution; }
    void    SetConstitution(int32 val) { m_constitution = val; SetPropDirty();}
    void    ClearConstitution() { m_constitution = 0; }

    uint32 GetNetState() { return m_net_state; }                                    // 获得角色状态
    void SetNetState(uint32 val) { m_net_state = val; }                             // 设置状态

    uint32 GetSessionID() { return m_session_id; }                                  // 获得网络ID
    void SetSessionID(uint32 val) { m_session_id = val; }                           // 设置网络ID

    uint64 GetLoginID() { return m_login_id; }                                      // 获得登陆ID
    void SetLoginID(uint64 val) { m_login_id = val; }                               // 设置登陆ID

    uint64 GetPlayerID() { return m_player_id; }                                    // 获得角色ID
    void SetPlayerID(uint64 val) { m_player_id = val; }                             // 设置角色ID

    const std::string& GetAccount() { return m_base_info.account; }                 // 获得账号名称
    void SetAccount(const std::string& val) { m_base_info.account = val; }          // 设置账号名称

    const std::string& GetName() {return m_base_info.name;}                         // 获得角色名称
    void SetName(const std::string& val) { m_base_info.name = val; }                // 设置角色名称

    const std::string& GetPlatformServerName() { return m_base_info.platform_server_name; }// 获得平台服务器名(不应有Set方法)

    int32 GetRegion() { return m_base_info.region; }                                // 获得游戏区号
    void SetRegion(int32 val) { m_base_info.region = val; }                         // 设置游戏区号

    int32 GetTimeZone() { return m_base_info.timezone; }                                // 获得时区
    void SetTimeZone(int32 val) { m_base_info.timezone = val; }                         // 设置时区

    int8 GetSex() { return m_base_info.sex; }                                       // 获得性别
    void SetSex(int8 val) { m_base_info.sex = val; }                                // 设置性别

    int8 GetCareer() { return m_base_info.career; }                                 // 获得职业
    void SetCareer(int8 val);                          // 设置职业

    uint32  GetRidePetID();                                                         // 获得坐骑ID
    void    SetRidePetID(uint32 val);                                               // 设置坐骑ID

    uint32  GetFaceID();                                                            // 获得脸谱ID
    uint32  GetWingID();                                                            // 获得翅膀ID
    uint32  GetFashionID();                                                         // 获得时装ID

    bool    GetGameStart() {return m_game_start;}                                   // 游戏是否可以开始
    void    SetGameStart(bool start);

    bool    GetGameLock() {return m_game_lock;}                                     // 玩家是否上锁
    void    SetGameLock(bool lock);

    void    SetRelive(bool val) { m_relive = val; }                                 // 设置是否重新复活

    int32 GetExp() { return m_base_info.exp; }                                      // 获得经验
    void SetExp(int32 val);                                                         // 设置经验
    void AddExp(int32 val);                                                         // 添加经验值

    void AddKillExp(int32 exp, int32 defenderLevel);

    int32 GetVigor();                                                               // 获得体力
    int32 GetSysVigor() { return m_base_info.sys_vigor; }                           // 获得系统体力
    int32 GetBuyVigor() { return m_base_info.buy_vigor; }                           // 获得购买体力

    void SetSysVigor(int32 val) { m_base_info.sys_vigor = val;}                     // 获得系统体力
    void AddSysVigor(int32 val);                                                    // 添加系统体力值
    void SubSysVigor(int32 val);                                                    // 减少系统体力值
    void JudgeSysVigor();                                                           // 判断系统体力

    void SetBuyVigor(int32 val) { m_base_info.buy_vigor = val; }                    // 获得购买体力
    void AddBuyVigor(int32 val);                                                    // 添加购买体力值
    void SubBuyVigor(int32 val);                                                    // 减少购买体力值

    int32 SubVigor(int32 val);                                                      // 减少体力值

    void RefreshVigor();                                                            // 刷新体力值

    int32 GetMaxVigor();

    int32 GetVigorBuyTimes() { return m_base_info.vigor_buytimes; }                 // 获得体力购买次数
    void SetVigorBuyTimes(int32 val) { m_base_info.vigor_buytimes = val; }          // 设置体力购买次数
    void IncVigorBuyTimes() { ++m_base_info.vigor_buytimes; }                       // 增加体力购买次数

    int32 GetTeamBuyTimes() { return m_base_info.team_buytimes; }                   // 获得组队副本购买次数
    void SetTeamBuyTimes(int32 val) { m_base_info.team_buytimes = val; }            // 设置组队副本购买次数
    void IncTeamTeamBuyTimes() { ++m_base_info.team_buytimes; }                     // 增加组队副本的购买次数
    void DealTeamBuyTimes(int32 itemNum);

    int32 GetTeamDungeonTimes() {
        return (m_base_info.team_dungeon_systimes + m_base_info.team_dungeon_buytimes); }   // 获得组队副本次数
    int32 SubTeamDungeonTimes(int32 val);                                                   // 删除组队副本次数
    int32 GetTeamDungeonSysTimes() { return m_base_info.team_dungeon_systimes; }            // 获得组队副本次数
    void SetTeamDungeonSysTimes(int32 val) { m_base_info.team_dungeon_systimes = val; }     // 设置组队副本次数
    void AddTeamDungeonSysTimes(int32 val);                                                 // 添加组队副本次数
    void SubTeamDungeonSysTimes(int32 val);                                                 // 删除组队副本次数

    int32 GetTeamDungeonBuyTimes() { return m_base_info.team_dungeon_buytimes; }            // 获得组队副本购买次数
    void SetTeamDungeonBuyTimes(int32 val) { m_base_info.team_dungeon_buytimes = val; }     // 设置组队副本购买次数
    void AddTeamDungeonBuyTimes(int32 val);                                                 // 添加组队副本购买次数
    void SubTeamDungeonBuyTimes(int32 val);                                                 // 删除组队副本购买次数


    void OnGuildMemberTurnTableBuyTimes();                                                                  // 购买公会成员轮盘次数
    int32 GetGuildMemberTurnTableBuyTimes() { return m_guild_welfare_info.member_turntable_buy_times; }     // 获得公会成员轮盘购买次数
    void IncGuildMemberTurnTalbeBuyTimes() { ++m_guild_welfare_info.member_turntable_buy_times; }           // 增加公会成员轮盘购买次数
    void SetGuildMemberTurnTableBuyTimes(int32 val) {m_guild_welfare_info.member_turntable_buy_times = val;}// 每天刷新轮盘的购买次数

    int32 GetStoneTurnGoldTimes() { return m_base_info.stone_turn_gold_times; }             // 获得点石成金的次数
    void SetStoneTurnGoldTimes(int32 val) { m_base_info.stone_turn_gold_times = val; }      // 设置点石成金的次数
    void IncStoneTurnGoldTimes() { ++m_base_info.stone_turn_gold_times; }                   // 增加点石成金的次数

    uint32 GetVoidFieldTime() {return m_base_info.void_field_time;}                         // 获得虚空幻境的挂机时间
    void SetVoideFiedlTime(uint32 val) {m_base_info.void_field_time = val;}                 // 设置虚空幻境的挂机时间
    void AddVoideFiedlTime(uint32 val);                                                     // 添加虚空幻境的挂机时间

    uint32 GetVoidFieldReftime() {return m_base_info.void_field_reftime;}                   // 虚空幻境的刷新时间
    void SetVoidFieldReftime(uint32 val) {m_base_info.void_field_reftime = val;}            //设置虚空幻境的刷新时间
    
    uint32 GetDailyReftime() { return m_base_info.daily_reftime; }                          // 获得每日刷新时间
    void SetDailyReftime(uint32 val) { m_base_info.daily_reftime = val; }                   // 设置每日刷新时间

    int32 GetGrowthPoint();                                                                 // 获得成长点
    void SetGrowthPoint(int32 val);                                                         // 设置成长点
    void AddGrowthPoint(int32 val);                                                         // 添加成长点
    void SubGrowthPoint(int32 val);                                                         // 删除成长点

    int32 GetGrowthPointSpendplus() { return m_base_info.spend_plus; }                      // 获得增加过成点后花费剩余磨石
    void SetGrowthPointSpendplus(int32 val) {m_base_info.spend_plus = val;}                 // 设置增加过成点后花费剩余磨石

    int32 GetVipLevel();                                                                    // 返回VIP等级
    void ActiveExperienceVIP();                                                             // 获得体验VIP
    bool IsExperienceVIP();

    uint16 CheckEnterActivity(uint32 activityType);                                         // 检查进入活动

    FCM_INFO& GetFcmInfo();                                                           // 返回防沉迷信息

    GUILD_WELFARE_INFO& GetGuildWelfareInfo() { return m_guild_welfare_info; }      // 获得公会福利信息

    bool IsInMemberTurnTable(uint32 id);                                            // 大转盘

    CITY_FIGHT_PLAYER_INFO& GetCityFightInfo() { return m_city_fight_info; }        // 获得城市争夺战信息
    uint32 GetCityFightRewardTimeStamp(uint32 cityId);                              // 获得指定城市奖励时间戳
    void SetCityFightRewardTimeStamp(uint32 cityId, uint32 timeStamp);              // 设置指定城市奖励时间戳

    void GainExp(int32 val);                                                        // 增加经验

    int32 GetMaxExp();                                         // 获得最大经验值

    int32 GetFcmGainValue(int32 val);                                               // 获取防沉迷后的奖励数据值
    void GetFcmRewardList(const std::vector<REWARD_ITEM>& 
        inList, std::vector<REWARD_ITEM>& outList);                                 // 获得长沉迷后的奖励列表

    int32 GetGamePoint() { return m_base_info.game_point; }                         // 获得金币
    void SetGamePoint(int32 val);                                                   // 设置金币
    void AddGamePoint(int32 val, int32 reason);                                     // 添加金币
    bool SubGamePoint(int32 val, int32 reason);                                     // 减少金币

    void GainGamePoint(int32 val);                                                  // 增加经验

    int32 GetBattlePower();                                                         // 战斗力

    ExpendManager* GetExpendManager() {return m_expend_mgr;}

    int32 GetMoneyPoint() { return m_base_info.money_point; }                               // 获得魔石
    void SetMoneyPoint(int32 val);                                                          // 设置魔石
    void AddMoneyPoint(int32 val, int32 reason, uint32 itemID = 0);                         // 添加魔石
    bool SubMoneyPoint(int32 val, int32 reason);                                            // 删除魔石

    int32  GetVigorValue() { return m_base_info.vigor; }                                    // 获得当前体力值
    void   SetVigorValue(int32 val);                                                        // 设置体力
    bool   AddVigorValue(int32 val, int32 reason);                                          // 添加体力
    bool   SubVigorValue(int32 val, int32 reason);                                          // 删除体力
    int32  GetMaxVigorValue(){ return PLAYER_VIGOR_MAX; }                                   // 获取最大体力值
    uint32 GetVigorReftime() { return m_base_info.vigor_reftime; }                          // 获得体力刷新时间
    void   SetVigorReftime(uint32 val) { m_base_info.vigor_reftime = val; }                 // 设置体力刷新时间
    void   AddVigorReftime(uint32 val) { m_base_info.vigor_reftime += val; }                // 添加体力刷新时间


    int32 GetBindMoneyPoint() { return m_base_info.bind_money_point; }                      // 获得绑定魔石
    void SetBindMoneyPoint(int32 val);                                                      // 设置绑定魔石
    void AddBindMoneyPoint(int32 val, int32 reason);                                        // 添加绑定魔石
    bool SubBindMoneyPoint(int32 val, int32 reason);                                        // 删除绑定魔石

    int32 GetSoulPoint() { return m_base_info.soul_point; }                                 // 获得精魂
    void SetSoulPoint(int32 val);                                                           // 设置精魂
    void AddSoulPoint(int32 val, int32 reason);                                             // 添加精魂
    void SubSoulPoint(int32 val, int32 reason);                                             // 删除精魂

    int32 GetStarPoint() { return m_base_info.star_point;}
    void SetStarPoint(int32 val);                                                           // 设置星魂
    void AddStarPoint(int32 val);                                                           // 添加星魂
    void SubStarPoint(int32 val, int32 reason);                                             // 删除星魂

    int32 GetHonorPoint() { return m_base_info.honor_point; }                               // 获得荣誉
    void SetHonorPoint(int32 val);                                                          // 设置荣誉
    void AddHonorPoint(int32 val);                                                          // 添加荣誉
    void SubHonorPoint(int32 val, int32 reason);                                            // 删除荣誉

    int32 GetGuildPoint() { return m_base_info.guild_point; }                               // 获得公会捐献点
    void SetGuildPoint(int32 val);
    void AddGuildPoint(int32 val, int32 reason);
    void SubGuildPoint(int32 val, int32 reason);

    int32 GetIncarnationPoint() { return m_base_info.incarnation_point; }                   // 化身点
    void SetIncarnationPoint(int32 val);
    void AddIncarnationPoint(int32 val, int32 reason);
    void SubIncarnationPoint(int32 val, int32 reason);

    int32 GetBattleSoulPoint() { return m_base_info.battle_soul_point; }                    // 获得战魂(不用了)
    void SetBattleSoulPoint(int32 val);                                                     // 设置战魂
    void AddBattleSoulPoint(int32 val);                                                     // 添加战魂
    void SubBattleSoulPoint(int32 val);                                                     // 删除精魂

    uint16 CheckItemOne(int32 type, int32 num, uint32 itemID = 0);                          // 单个消耗物品检测(返回错误码, 成功为ERR_SUCCEED)
    uint16 CheckItemTable(const REWARD_TABLE& itemList);                                    // 多个消耗物品检测(返回错误码, 成功为ERR_SUCCEED)
    uint16 SubItemOne(uint8 reason, int32 type, int32 num, uint32 itemID = 0);              // 单个消耗物品处理(返回错误码, 成功为ERR_SUCCEED)
    uint16 SubItemTable(const REWARD_TABLE& itemList);                                      // 多个消耗物品处理(返回错误码, 成功为ERR_SUCCEED)
    bool  SubBagItem(uint32 itemID,int32 num);                                              // 背包里的物品是否足够减去

    void GainSoulPoint(int32 val);                                                          // 精魂
    void GainItem(REWARD_ITEM item) { /*m_gain_pool.items.push_back(item);*/ }              // 物品放入获取池

    void SetLevel(int32 val);
    void AddLevel(int32 val = 1);                                                                        // 增加角色等级
	void AddPassiveSkillPoint(int32 val);													// 增加天赋点
    void ResetPassiveSkillPoint();                                                          // 天赋洗点

    uint32 GetChangeDungeonCount() { return m_base_info.change_dungeon_count; }             // 获得切换地图次数
    void SetChangeDungeonCount(uint32 val) { m_base_info.change_dungeon_count = val; }      // 设置切换地图次数

    uint32 GetUnionMapID() { return m_base_info.unionmap_id; }                              // 获得地图ID
    void SetUnionMapID(uint32 val);                                                         // 设置地图ID

    uint32 GetCityMapID() { return m_citymap_id; }                                          // 获得主城ID
    void SetCityMapID(uint32 val) { m_citymap_id = val; }                                   // 设置主城ID

    int32 GetCityX() { return m_city_x; }                                                   // 获得主城地图X坐标
    void SetCityX(int32 val) { m_city_x = val; }                                            // 设置主城地图X坐标

    int32 GetCityY() { return m_city_y; }                                                   // 获得主城地图Y坐标
    void SetCityY(int32 val) { m_city_y = val; }                                            // 设置主城地图Y坐标

    uint32 GetGateSvrID() { return m_base_info.gatesvr_id; }                                // 获得登陆服务器ID
    void SetGateSvrID(uint32 val) { m_base_info.gatesvr_id = val; }                         // 设置登陆服务器ID

    uint32 GetCenterSvrID() { return m_base_info.centersvr_id; }                            // 获得中心服务器ID
    void SetCenterSvrID(uint32 val) { m_base_info.centersvr_id = val; }                     // 设置中心服务器ID

    uint32 GetRightFlag() { return m_base_info.right_flag; }                                // 获得权利标记
    void SetRightFlag(uint32 val) { m_base_info.right_flag = val; }                         // 设置权利标记

    uint32 GetLoginIP() { return m_base_info.login_ip; }                                    // 获得登陆IP
    void SetLoginIP(uint32 val) { m_base_info.login_ip = val; }                             // 设置登陆IP

    uint32 GetCityFightCD(){return m_base_info.city_fight_cd;}
    void SetCityFightCD(uint32 cd){m_base_info.city_fight_cd = cd;}

    int32 GetWelcomeReward() {return m_base_info.getwelcome_reward;}                        // 领奖标记(欢迎界面奖励,微端奖励)
    void SetWelcomeRewardFlag(int32 type, bool flag);
    bool GetWelcomeRewardFlag(int32 type);
    uint32 GetWelcomeRewardTypeBit(int32 type);

    uint32 GetOnLineTime() {return m_base_info.online_time;}                                // 玩家的在线总时长
    void   SetOnLineTime(uint32 time) {m_base_info.online_time = time;}

    uint32 GetFirstEnterTime() {return m_first_entertime;}
    void SetFirstEnterTime(uint32 time) {m_first_entertime = time;}

    uint32 GetLastExitTime() {return m_lastexit_dugeon_time;}
    void SetLastExitTime(uint32 time) {m_lastexit_dugeon_time = time;}


    bool IsGM() {return GetRightFlag() & RIGHT_FLAG_GM;}                            // 是否是GM
    bool IsGMJump() {return GetRightFlag() & RIGHT_FLAG_GM_JUMP_DUNGEON;}           // 是否有GM跳地图权限
    bool IsGMNoCD() {return GetRightFlag() & RIGHT_FLAG_GM_NO_COOLDOWN;}            // 是否有GM无技能CD权限

    void LogChat(const std::string& log, uint32 channel = CHAT_CHANNEL_INTEGRATED);         // 发送信息到客户端聊天信息

    uint64 GetGuildID() { return m_guild_id; }                                              // 返回公会ID
    void SetGuildID(uint64 id);                                                             // 设置公会ID

    uint32 GetGuildJoinTime() { return m_guild_join_time; }                                 // 获得公会加入时间
    void SetGuildJoinTime(uint32 val) { m_guild_join_time = val; }                          // 设置公会加入时间

    std::string GetGuildName();                                                             // 返回公会名称

    void AddMail(MAIL_INFO* mail);                                                          // 添加邮件
    void DelExpiredMail(uint64 expiredMailID);                                              // 删除过期邮件

    void SendMail(std::string recvName, std::string title, 
        std::string message, int32 type, const std::vector<REWARD_ITEM>& list);             // 发邮件

    const VIP_INFO& GetVipInfo();                                                           // 获得VIP信息
    void SetVipInfo(const VIP_INFO& val);                                                   // 设置VIP信息

    virtual void CalcProperty();                                                             // 计算角色属性
    virtual void ClearProperty();                                                           // 清除角色属性

    virtual int32   GetProp(uint32 type);
    virtual void    SetProp(uint32 type, int32 val);

    void CalDailyRefresh();                                                     // 计算每日刷新值

    void InitNewRoleDefaultSet();                                               // 新角色的一些默认设置

    void RankingDailyRefresh();                                                 // 排行榜排名结束通知

    void RankingHourRefresh();                                                  // 排行榜排名每小时刷新

    //const GainPool& GetGainPool() { return m_gain_pool; }                       // 获取池

    void FirstEnterGame();                                                      // 第一次进入游戏触发
    void SendPlayerInfo(Scene* pScene);                                         // 发送玩家登陆信息

    virtual bool SendToGate(uint32 msgID, void* data);                                  // 发送数据给Gate
    bool SendToCenter(uint32 msgID, void* data);                                // 发送数据给Center

    void ReadyChangeToDungeon(CHANGE_DUNGEON_REQ* dungeonReq);                  // 进入副本前处理

    void OnExitDungeon();                                                         // 退出场景前处理
    void CalOnlineCoutTime();                                                     // 计算总的在线时常

    void OnTaskSettleDungeon(uint32 dungeonID, bool success, bool raids = false);   // 任务结算副本
    void SendTaskEvent(uint32 event_id, int32 arg1, int32 arg2, int32 arg3=0);      // 发送任务事件

    void OnAddDungeonCount(uint32 dungeonID);                                   // 添加副本计数
    void SendChangeDungeon(uint32 dungeonID);                                   // 发送请求切换副本

    void ClearGainPool();                                                       // 清空获取池
    void GainPoolToRewardInfo();                                                // 获取池数据到奖励池
    void OnRewardInfoCount();                                                   // 做结算
    void AddRewardTableToPool(const REWARD_TABLE& table);                       // 添加奖励表到奖励池
    ERROR_CODE TakeInRewardTable(const REWARD_TABLE* table);                          // 奖励表物品进入包裹,注意背包格子满的问题
    ERROR_CODE TakeInRewardTable(int32 itemType, uint32 itemID, int32 itemNum, int32 reason, int32 reason_arg = 0);   // 奖励物品进入包裹。注意背包格子满的问题
    REWARD_INFO& GetRewardInfo() {return m_reward_info;}                        // 得到奖励池信息
    void SendReasonTablePool(int32 reason,int32 reasionarg);                    // 发送指定原因的的奖励
    void DeleteTablePool(int32 reason,int32 reasionarg);                        // 删除指定原因的奖励
    void TakeInResasionRewardTable(int32 reason,int32 reasionarg);              // 放到背包里面

    int32 GetDaysBetweenDates(time_t t1, time_t t2);                            // 计算两个日期间隔天数     (t1>t2正, t1<t2负)

    virtual void onEnterScene(Scene* pScene);
    virtual void onLeaveScene(Scene* pScene);
    virtual void Update(uint32 frame_count);

    virtual void OnKill(BattleUnit* sufferer);
    virtual void OnDead(BattleUnit* killer);

    virtual void OnStateChange(uint32 oldState, uint32 newState);        // 状态改变回调

    virtual bool UseSkill(uint32 skill_id, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skill_obj_id, uint32 skill_slot);

    void MakeSkillCD(uint32 skill_id, int32 cdtime);                            // cdtime<0使用技能默认CD时间
    void ClearAllSkillCD();                                                     // 重置所有技能CD

    void SendBehaviorLog(int32 type, int32 num, int32 reason);                  // 行为日志
    void SendPlaySystemLog(int32 playSystemId, int32 arg1 = 0, int32 arg2 = 0, const std::string& argstr = "");     // 玩法日志
    void SendWealthLog(int32 wealthId, int32 num, int32 oldNum, int32 newNum);  // 财产日志

    void PublicMarquee(const std::string& chat);                                       // 发布跑马灯

    // 以下接口是虚拟接口
    int32 AllRecharey(){return m_allRecharey;}      // 玩家总共冲了多少钱

    enum
    {
        kStateInit              = 0,                                    // 初始状态
        kStateLoginReq          = 1,                                    // 登陆请求
        kStateVerifyOk          = 2,                                    // 验证OK
        kStateLoadingData       = 3,                                    // 正在加载数据
        kStateDataLoaded        = 4,                                    // 加载数据结束
        kStateInGame            = 5,                                    // 在游戏中
        kStateJoinTeam          = 6,                                    // 加入组队
        kStateSyncData          = 7,                                    // 同步数据
    };

    virtual int32   GetEquipAnimation() const;

    virtual void NotifyEquipInfo(UPDATE_EQUIP_INFO_NTF& info);                             // 通知装备信息

    void NotifyTitleInfo(PLAYER_TITLE_LIST& info);                      // 通知称号信息

    void SendChangePlayerNameAck(uint16 errCode, const std::string& name);    // 玩家名称

    void NotifyProperty(uint32 property_id, int32 value, const std::string& name = ""); // 通知属性
    void NotifyErrorCode(uint16 errCode);                               // 通知错误码

    void SendRewardTable(const REWARD_TABLE& rewardTable);              // 发送奖励通知

    void SendRewardTableList(const REWARD_TABLE_LIST& rewardTableList); // 发送奖励表的列表通知

    void OnRaidsDungeonCmpl(uint32 dungeonID);                          // 副本扫荡完成触发

    void OnEnterDungeon(Dungeon* dungeon, uint32 dungeonID);            // 成功进入地图

    void CBVipTrade(const std::string& param);                          // VIP时效购买回调
    void CBVipOffer(int32 cardId, uint32 num);                          // VIP时效赠送回调

    // 处理订单的扩展信息
    void DealTradeOrder(int32 reason, const std::string& str);

    // 使玩家再下次Update，回存角色数据
    void AskAutoSave();
    void SetBuying(bool buying) {m_is_buying = buying;}
    bool GetBuying() {return m_is_buying;}

    static void OnMarQuee(SERVER_MARQUEE& marquee);                     // 发跑马灯公告
    void SendMessageNotify( MESSAGE_NTF& message );                     // 发上飘字公告,只用在活动的飘黄字上

    void SendChangeDungeonErrorAck(uint16 errCode);                                                              // 发送变更副本
    void SendChangeSceneAck(uint16 errCode, uint32 newSceneUnionID, int32 x, int32 y, int32 angle);         // 发送切换场景确认消息

    void SendToCenterTeamMemberProp();                                      // 队伍信息更改通知

    void SaveYesterdayCount(uint32 id, int32 count);                        // 保存昨天进入次数

    void SendPush360LevelMsg();                                             // 发送360 推送等级消息

    // log相关
    void SendtoCenterLog(uint32 idflag, const char * format, ...);          // 发送log
    void SendtoCenterLog(uint32 idflag, const std::string& log_info);       // 发送log
    void SendBehaviorLog(const char* format, ...);                          // 行为log

    void SendTeamMemberAddAck(uint16 errcode, const std::string& src_name, const std::string& dst_name);  // 发送添加队友应答

    void SendServerTimeSync();                                      // 发送服务器时间同步
    void OnTimeFresh();                                             // 8点刷新

    uint32  GetPing() {return m_Ping;}

    int32 GetRechargeCount();                                                   // 获得充值总数
    void SetTotalRecharge(int32 setmoney);                                       // 此接口仅用于测试
    void GmtUpdate(uint32 type);                                    // gmt数据更新

    virtual PetList* GetPetList();

private:
    void CalcLevelProp();                                           // 计算等级属性

    void OnFcmInfoReq(void* data);                                  // 请求防沉迷信息

    void RecvChangePlayerNameReq(void* data);                       // 更改角色名请求
    void RecvChangePlayerNameAck(void* data);                       // 更改角色名应答

    void RecvRefreshVigorReq();                                     // 刷新体力值
    void RecvBuyVigorReq();                                         // 购买体力请求

    void RecvCareerSetReq(void* data);                              // 设置职业

    void RecvGuideInfoSetReq(void* data);                           // 设置引导

    void RecvOpenSystemInfoSetReq(void* data);                      // 设置功能开启

    void RecvUseGiftCodeReq(void* data);                            // 使用平台礼品码请求

    void TeamDungeonCalDailyRefresh();                              // 刷新组队次数

    void RecvPlayerLog(const std::string& log);

    void OnTriggerActivate(void* data);                             // 处理触发器触发
    void OnTouchRecv(uint32 msgID, void* data);                     // 处理触摸器请求
    void OnResurgenceReq(void* data);                               // 玩家复活请求

    void SendTriggerActivateAck(TRIGGER_ACTIVATE_ACK& msg);         // 发送触发器应答

    void AddVipTime(uint32 cardId, uint32 vipTime);                 // 添加VIP时效

    void UseGiftCode(uint32 type);                                  // 标记使用礼品码
    bool IsUsedGiftCode(uint32 type);                               // 是否已经使用过礼品码

public:
    uint32          m_auto_save_time;                               // 自动回存计时

private:
    PlayerLoader*   m_PlayerLoader;                                 // 角色加载器
    PlayerSaver*    m_PlayerSaver;                                  // 角色保存器

    bool            m_is_buying;                                    // 正在购买中

    bool            m_game_start;                                   // 游戏是否开始
    bool            m_game_lock;                                    // 玩家是否被锁(锁住后，不可以进行任何战斗操作)
    bool            m_relive;                                       // 死亡后是否要复活

    uint32          m_first_entertime;                              // 第一次进入游戏的时间
    uint32          m_lastexit_dugeon_time;                         // 上一次退出副本的时间

    uint32          m_Ping;                                         // 该玩家的客户端ping值

    // 玩家角色4属性(力敏体智)
    int32           m_strength;                                     // 力量
    int32           m_agility;                                      // 敏捷
    int32           m_constitution;                                 // 体力
    int32           m_spirit;                                       // 智力

    uint32          m_net_state;                                    // 角色状态
    // ============================================================
    uint32          m_session_id;                                   // 网络ID
    uint64          m_login_id;                                     // 登陆ID
    uint64          m_player_id;                                    // 角色ID
    PROP_BASE_INFO  m_base_info;                                    // 角色基本信息

    uint32          m_citymap_id;                                   // 城市的地图ID
    int32           m_city_x;                                       // 城市的X坐标
    int32           m_city_y;                                       // 城市的Y坐标

    uint64          m_team_id;                                      // 队伍ID

    uint64          m_guild_id;                                     // 公会ID
    uint32          m_guild_join_time;                              // 公会加入时间

    uint32          m_online_reward_time;                           // 在线奖励时间
    uint32          m_online_reward_flag;                           // 在线奖励标记

    // ============================================================
    int32                   m_vip_level;                            // 当前VIP等级，严格按照成长点换算(不存储db)
    VIPManager*             m_vip_manager;                          // VIP管理系统
    OPEN_SYSTEM_INFO        m_open_sysetm_info;                     // 开启系统信息
    GUILD_WELFARE_INFO      m_guild_welfare_info;                   // 公会福利信息
    CITY_FIGHT_PLAYER_INFO  m_city_fight_info;                      // 城市争夺战信息
    FCMManager*             m_fcm_manager;                          // 防沉迷信息

    OnRecvFilter*           m_recv_filter;                          // 协议过滤器
    PlayerDungeonManager*   m_player_dungeon_mgr;                   // 副本系统
    SkillInfoManager*       m_skill_info_mgr;                       // 技能系统
    ItemManager*            m_item_mgr;                             // 包裹
    PackageManager*         m_package_mgr;                          // 抽奖
    EquipManager*           m_equip_mgr;                            // 装备管理系统
    TaskManager*            m_task_mgr;                             // 任务管理系统
    GuideManager*           m_guide_mgr;                            // 引导管理系统
    OpenSystemManager*      m_opensys_mgr;                          // 功能开启管理系统
    PlayerDoorManager*      m_player_door_mgr;                      // 勇者争霸系统
    LightCityManager*       m_city_mgr;                             // 圣光城管理系统
    MailManager*            m_mail_mgr;                             // 邮件管理系统
    HoroscopeManager*       m_horoscope_mgr;                        // 星空图管理系统
    FriendManager*          m_friend_mgr;                           // 好友管理系统
    ChatManager*            m_chat_mgr;                             // 聊天管理系统
    BattleRecorder*         m_battle_recorder;                      // 战斗信息管理系统
    AshuraRecorder*         m_ashura_recorder;                      // 修罗场管理系统
    HorseRacingManager*     m_racing_mgr;                           // 赛马系统
    SignManager*            m_sign_mgr;                             // 签到系统
    OnlineRewardManager*    m_online_reward_mgr;                    // 在线奖励系统
    TreasuryManager*        m_treasury_mgr;                         // 宝库系统
    GoldtreeManager*        m_goldtree_mgr;                         // 金像系统
    RideManager*            m_ride_mgr;                             // 坐骑系统
    MiningManager*          m_mining_mgr;                           // 采矿系统
    AdventureManager*       m_adventure_mgr;                        // 大冒险系统
    WingManager*            m_wing_mgr;                             // 翅膀系统
    TitleManager*           m_title_mgr;                            // 称号系统
    RechargeManager*        m_recharge_mgr;                         // 充值系统
    ShopManager*            m_shop_mgr;                             // 商城系统
    ShopServiceManager*     m_shop_service_mgr;                     // 商城服务系统
    DayInvestmentManager*   m_day_investment_mgr;                   // 日常投资管理系统
    LevelInvestmentManager* m_level_investment_mgr;                 // 等级投资管理系统
    LevelActivityManager*   m_level_activity_mgr;                   // 等级活动管理系统
    SpecialActivityManager* m_special_activity_mgr;                 // 特殊活动管理系统
    ExpendManager*          m_expend_mgr;                           // 货币消耗管理系统
    PassiveSkillManager*    m_passive_skill_mgr;                    // 被动技能系统
    SettingManager*         m_setting_mgr;                          // 设置系统
    LivenessManager*        m_liveness_mgr;                         // 活跃度系统
    DungeonTimesRecorder*   m_dungeon_times_mgr;                    // 守卫地宫系统
    StoneTurnGoldManager*   m_stone_turn_gold_mgr;                  // 点石成金系统
    SingleAnswerManager*    m_single_answer_mgr;                    // 单人答题系统
    VigorManager*           m_vigor_mgr;                            // 体力系统
    OfflineVigorManager*    m_offline_vigor_mgr;                    // 离线体力
    FashionManager*         m_fashion_mgr;                          // 时装系统
    SwordManager*           m_sword_mgr;                            // 圣剑系统
    ZombieManager*          m_zombie_mgr;                           // 僵尸生存 
    BhManager*              m_bh_mgr;                               // 勇敢的心系统
    EnemyManager*           m_enemy_mgr;                            // 善恶系统
    GuildScienceRecorder*   m_guild_science_mgr;                    // 个人公会科技系统
    ShorcutManager*         m_shorcut_mgr;                          // 游戏收藏
    MultiserverBattleInfoManager*   m_multiserver_battle_info;      // 跨服匹配信息
    PetSoulManager*         m_pet_soul_mgr;                         // 宠物护魂
    PetFormationManager*    m_pet_formation_mgr;                    // 宠物阵型信息
    PayPointLotteryManager* m_pay_point_lottery;                    // 充值积分抽奖
    VIPOnlyDungeonManager*  m_vip_only_dungeon;                     // VIP独享副本
    InvestmentPlanManager*  m_investment_plan_mgr;                  // 投资理财系统
    DonateManager*          m_donate_mgr;                           // 捐献
    AnswerManager*          m_answer_mgr;                           // 勇者问答
    BattleStateManager*     m_battle_state_manager;                 // 战斗状态管理器
    PlayerTeamManager*      m_team_manager;                         // 队伍信息管理器
    EggManager*             m_egg_manager;                          // 砸金蛋
    BuyCheapMgr*            m_buy_cheap_mgr;                        // 越卖越便宜
    PetManager*             m_pet_mgr;                              // 宠物管理器
    BagManager*             m_bag_mgr;                              // 背包管理器
    StoreManager*           m_store_mgr;                             // 仓库管理器
    EquipStrengthenManager* m_equip_quality_up;                     // 装备强化管理器
    SkillStrengthenManager* m_skill_strengthen;                     // 技能强化管理器
    AlchemyManager*         m_alchemy_mgr;                          // 领地制作物品
    GemManager*             m_gem_mgr;                              // 宝石
    PotionManager*          m_potion_use_mgr;                       // 药剂使用管理器
    DecomposeManager*       m_decompose_mgr;                        // 分解管理器
    RandomPackageManager*   m_random_package_mgr;

    GameplayTimesNotifier*  m_notify_mgr;                           // 玩法信息管理器
    ContrastManager*        m_contrast_manager;                     // 玩家数据对比管理器
    ItemController*         m_item_controller;                      // 物品控制器

    //GainPool                m_gain_pool;                            // 获取池(副本战斗中所获，先放入此)
    REWARD_INFO             m_reward_info;                          // 奖励池

    PlayerPropNotifier*     m_PlayerPropNotifier;                   // 玩家属性通知器

    LimitBuy_Manager*       m_limitbuy_mgr;                         // 限制购买

    HeadPortraitManager*    m_headPortrait_mgr;                     // 个性头像

    int32                   m_allRecharey;                          // 玩家已经冲了多少钱
    int32                   m_neednum;                              // 需要的个数
    int32                   m_addprotype;                           // 当前加的哪个属性

    GIFT_CODE_INFO          m_gift_code_info;                       // 使用礼品卡信息
};
#endif // player_h__
