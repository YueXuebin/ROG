/*
 * 虚空幻境solo
 */
#ifndef regular_void_field_solo_h__
#define regular_void_field_solo_h__

#include "regular_world.h"

class Player;
class Scene;
class RegularDoc;
class RegularDungeon;
class Creature;

// 虚空幻境中每个玩家的记录
class VoidFieldSoloPlayerRecord
{
public:
    void Clear()
    {
        m_player_id     = 0;
        m_chests_num    = 0;
        m_add_exp_time  = 0;
        m_boss_par_flag = false;
        m_personal_info.Clear();
    }

    uint64  GetPlayerID() { return m_player_id; }
    void    SetPlayerID(uint64 val) { m_player_id = val; }

    int32   GetChestsNum() { return m_chests_num; }
    void    SetChestsNum(int32 val) { m_chests_num = val; }
    void    AddChestsNum(int32 val);
    void    SubChestsNum(int32 val);

    uint32  GetAddExpTime() { return m_add_exp_time; }
    void    SetAddExpTime(uint32 val) { m_add_exp_time = val; }

    bool    GetBossParFlag() {return m_boss_par_flag;}
    void    SetBossParFlag(bool val) {m_boss_par_flag = val;}

private:
    uint64                          m_player_id;        // 角色ID
    int32                           m_chests_num;       // 宝箱计数
    uint32                          m_add_exp_time;     // 下次增加exp时间
    bool                            m_boss_par_flag;    // 参加珍兽
public:
    VOID_FIELD_PERSONAL_INFO_NTF    m_personal_info;    // 个人信息通知
};


// 虚空幻境中规则CD计时
class VoidFieldSoloOpenConditon
{
public:
    void Clear()
    {
        m_start_time    = 0;
        m_run_time      = 0;
    }

    uint32  m_start_time;
    uint32  m_run_time;
};


class RegularVoidFieldSolo : public RegularWorld
{
public:
    RegularVoidFieldSolo(RegularDoc* pDoc);
    virtual ~RegularVoidFieldSolo();

    virtual void    OneSecondUpdate(int32 deltatime);
    virtual void    OnCreatureEnter(BattleUnit* creature);
    virtual void    OnCreatureLeave(BattleUnit* creatrue);
    virtual void    ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);
    virtual void    Update(uint32 curr_tick);
    virtual bool    IsEnemy(BattleUnit* attacker, BattleUnit* defencer);
    virtual void    SetScene(Scene* pScene);
    virtual void    OnStart();

    virtual void    FillStartPostion(Player* player, uint32 triggerID = 0);

    int32           GetCurrType();                                              ///< 获得当前类型
    void            SetCurrType(int32 mType);                                   ///< 设置当前类型

    void            UpdateRegularState();                                       ///< 更新规则状态
    void            GotoNormalState();                                          ///< 回到普通状态
    void            SetStateStartTime(int32 mType);                             ///< 设置某状态的开始时间

    void            SendVoidFieldChestsNum(Player* player, int32 num);          ///< 通知角色宝箱数量

    void            RobChests(uint32 deadID, uint32 killerID, int32 rate);      ///< 抢夺宝箱

    void            ResetAddExpTime();                                          ///< 重置玩家增加经验时间

    void            TriggerTaskEvent(uint32 taskEventId);

    VoidFieldSoloPlayerRecord*  GetRecord(Player* player);                          ///< 获得记录

    Player*         GetPlayerByUnitID(uint32 unitID);                           ///< 根据玩家或宠物的ID获取玩家

    void            KillAllMonsters();                                          ///< 杀死所有怪物，珍兽使用
    void            StopRefreshMonsters();                                      ///< 停止刷怪，珍兽使用
    void            RecoverRefreshMonsters();                                   ///< 恢复刷怪，常态使用

private:
    void            PushMessage();                                              ///< 定时推送玩家虚空幻境消息
    void            RewardPlayerExp();                                          ///< 定时奖励玩家经验

    void            AddPlayerChests(uint32 creatureID);                         ///< 添加玩家虚空宝箱
    void            AddChestsNum(Player* player);                               ///< 添加虚空宝箱数量

    void            AddDropItem(Player* player);                                ///< 添加玩家掉落物品

    // 双重概率限制
    bool            CanMonsterDrop(int32 diff_level);                           ///< 怪物是否会产生掉落，有可能不产生掉落，物品和宝箱数公用
    bool            CanStateDrop();                                             ///< 当前规则是否会产生掉落，物品和宝箱数公用
    bool            CanChestsProbability(Player* player,int32 lower);           ///<.宝箱的掉落规则

    void            SetPlayersFightMode();                                      ///< 设置玩家的战斗状态
    void            NotifyRegularType(Player* player=NULL);                     ///< 通知当前副本规则类型

    void            OnBattleEventAttack(uint32 attackerID, uint32 defencerID);  ///< 接收角色被攻击事件
    void            OnBattleEventKill(uint32 attackerID, uint32 defencerID);    ///< 接收角色死亡事件
    void            OnBattleEventMonsterDead(uint32 monsterID, uint32 killerID);///< 接收怪物死亡事件

    void            AddRecord(Player* player);                                  ///< 添加记录
    void            DelRecord(Player* player);                                  ///< 删除记录

    void            SettleChests(Player* player);                               ///< 结算宝箱

    void            AddPersonalKillInfo(Player* attacker, Player* defencer);                                ///< 记录虚空幻境击杀信息
    void            AddPersonalExpInfo(Player* player, VOID_FIELD_PERSONAL_INFO_NTF* info, int32 exp);      ///< 添加虚空幻境经验信息
    void            AddPersonalDropItemInfo(Player* player, VOID_FIELD_PERSONAL_INFO_NTF* info, REWARD_ITEM* drop_item);      ///< 添加虚空幻境掉落物品信息
    void            SendVoidFieldInfoNtf(Player* player, VOID_FIELD_PERSONAL_INFO_NTF* info);               ///< 发送虚空幻境信息

    uint32          CalPlayerTriggerID(Player* player);                                         ///< 计算TriggerID
    void            OnPlayerResurgenceReq(int32 arg1);                                          ///< 接收复活请求

    void            SendResurgenceAsk(Player* player);                                          ///< 发送复活询问
    void            SendRegularTypeMarquee(int32 regularType, uint32 channelNum);               ///< 广播频道开启类型

    void            CheckPlayerEvilState(Player* player);                                       ///< 检查玩家罪恶状态决定是否添加红名buff
    void            AddPlayerRedNameBuff(Player* player);                                       ///<.添加玩家红名buff
    void            ClearPlayerRedNameBuff(Player* player);                                     ///<.清除玩家红名buff

    void            CalIntoTime(Player* player);                                                ///<.计算在虚空幻境的时间

    // =======================================================resurgence=============================================
    typedef std::map<uint64, VoidFieldSoloPlayerRecord>     PlayerRecordMap;
    typedef std::vector<VoidFieldSoloOpenConditon>          OpenCondList;

    // ----------------------------------------------------------------------------------------------------
    OpenCondList                m_openCondList;                                 ///< 规则开启条件列表 

    // ----------------------------------------------------------------------------------------------------
    int32                       m_currType;                                     ///< 当前规则类型
    PlayerRecordMap             m_recordMap;                                    ///< 记录映射表

    uint32                      m_into_time;                                     ///<.进入虚空幻境的时间

    //=======================================================boss mode===================================================
    void            ClearBossData();    ///< 清理与BOSS有关的数据
    void            CreateBoss();
    void            StrongBoss(Creature* battleUnit);
    void            OnBossModeBattleEventKill(uint32 attackerID, uint32 defencerID);    ///< 接收角色死亡事件
    void            OnDamage(uint32 attackerID, int32 damage);
    void            UpdateDamage();
    void            OnDead(uint32 attackerID);
    void            BossLevelUp();      ///< Boss升级
    void            CheckOverRange();                                           ///< 检查每个玩家是否超出攻击范围
    void            SettleReward();                                             ///< 结算奖励
    void            SendTopOneRewardMail(uint32 centerID, uint64 playerID);     ///< 排名第一奖励
    void            SendTopOneGuildRewardMail(uint64 guildID);                  ///< 排名第一公会奖励
    void            SendKillerRewardMail(uint32 uintID);                        ///< 最后击杀者奖励
    uint32          GetPlayerSubScoreTime(uint64 playerID);                     ///< 获得玩家减少积分时间点
    void            BossKillProcess(uint32 uintID);                             ///< 珍兽boss死亡处理
    void            BossEscapeProcess();                                        ///< 珍兽boss逃跑处理
    void            PlayerEscapeProcess(Player* playerID);                       ///< 玩家逃跑

    typedef struct VoidFieldBossDamageInfo
    {
        void Clear()
        {
            center_id       = 0;
            player_id       = 0;
            player_name     = "";
            guild_id        = 0;
            damage          = 0;
            battle_power    = 0;
            level           = 0;
        }
        uint32      center_id;
        uint64      player_id;
        std::string player_name;
        uint64      guild_id;
        uint32      damage;
        int32       battle_power;
        int32       level;
    } VoidFieldBossDamageInfo;
    typedef std::map<uint64, VoidFieldBossDamageInfo> DamageMap;
    typedef std::vector<VoidFieldBossDamageInfo*>     DamageList;

    static bool SortDamage(VoidFieldBossDamageInfo* a, VoidFieldBossDamageInfo* b);                 ///< 按由大到小排序
    VoidFieldBossDamageInfo* GetNotifyDamageInfo(WORLD_BOSS_DAMAGE_LIST& list, Player* player);     ///< 在列表中查找同名角色数据
    VoidFieldBossDamageInfo* GetBossDamageInfo(uint64 playerID);                                    ///< 获得角色的伤害值
    //------------------------------------------------------------------------
    uint32              m_bossID;           ///< 珍兽Boss的ID
    BossData            m_bossData;         ///< 珍兽Boss数据
    bool                m_killed;           ///< 珍兽已经被杀
    uint32              m_killerID;         ///< 珍兽Boss的击杀者ID
    int32               m_allDamage;        ///< 对Boss造成的伤害总和
    uint32              m_bossCreatureID;   ///< BOSS 的实例ID
    uint32              m_outRadiusTime;    ///< 超出BOSS的攻击半径时间
    int32               m_outRadius;        ///< 超出BOOS的攻击半径
    DamageMap           m_damageMap;        ///< 伤害记录表
    DamageList          m_damageList;       ///< 伤害排名列表
    int32               m_startcreatebooss; // 开始创建boss时间

    typedef std::map<uint32, WORLD_BOSS_INFO> WorldBossInfoMap;
    WorldBossInfoMap                    m_WorldBossInfo;     // 世界BOSS的信息

    typedef std::map<uint64, uint32> SubScoreTimeMap;       // 玩家超出boss攻击范围时间记录，<playerid,time> time:下次减少玩家积分的时间
    SubScoreTimeMap                     m_subScopeMap;

};

#endif // regular_void_field_solo_h__
