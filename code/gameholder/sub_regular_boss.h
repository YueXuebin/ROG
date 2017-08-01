/*
 *	大地图Boss规则
 */
#ifndef sub_regular_boss_h__
#define sub_regular_boss_h__

#include "sub_regular.h"
#include "world_map_helper.h"

class Player;
class Scene;
class Boss;
class RegularDoc;
class RegularFiledBoss;
struct HatredInfo;
class HatredManager;

class SubRegularBoss : public SubRegular
{
public:
    SubRegularBoss(Regular* pRegular);
    virtual ~SubRegularBoss();

    virtual void    OneSecondUpdate(int32 deltatime);
    virtual void    OnCreatureEnter(BattleUnit* creature);
    virtual void    OnCreatureLeave(BattleUnit* creatrue);
    virtual void    ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);
    virtual void    Update(uint32 curr_tick);

    void    OnFieldBossScourgeCorpseReq(Player* player, uint32 msgID, void* data);                                                 ///< 野外Boss鞭尸请求
    //=====================================master regular=====================================================
    bool    IsInBossLeakTime();                                                 ///< 是否霸气侧漏时间
    std::string GetBossKillerName();                                            ///< Boss击者
    uint32 GetFreshBossTime() {return m_how_time;}                     ///<.得到刷新boss的倒计时
private:
    //=====================================Boss刷新=====================================================
    bool            NeedRefreshBoss();  // Boss是否需要刷新
    void            CreateBoss();       // Boss创建
    void            BossRefreshCheck(); // Boss刷新检查
    uint32          CalNextRefreshBossTime(); // 计算Boss下次刷新时间点
    void            BossDestroyCheck(); // Boss销毁检查

    //=====================================玩家行为=====================================================
    void            OnBattleEventKill(uint32 attackerID, uint32 defencerID);    ///< 接收角色死亡事件,扣伤害值
    bool            IsPlayerEscape(uint64 playerID);                             ///< 角色离开当前副本，扣伤害值
    bool            IsPlayerOffLine(uint64 playerID);                           ///< 角色下线，清除伤害值
    void            IsPlayerOfflineReq(uint64 playerID);                         ///<.角色是否下线的请求

    //=====================================Boss死亡=====================================================
    void            OnBossDeadProcess(uint32 killerID);                    // Boss死亡
    void            OnBossCorpseProcess(uint32 attackerID, int32 damage);    // Boss鞭尸
    void            DestroyBoss();                          // Boss销毁
    void            BossReincarnation();                    // Boss转生

    //=====================================Boss伤害值=====================================================
    void            OnDamage(uint32 attackerID, int32 damage, uint32 defenderID);
    void            DamageRankNtf();                                            ///< 伤害排行通知

    void            ResetDamageByPlayerState();                                 ///< 检查每个玩家状态，重新设置伤害值

    void            SubDamage(uint32 playerID);                                  ///< 扣伤害值
    void            ClearDamage(uint64 playerID);                                ///< 清除指定伤害值

    //void            SubDamangeByLeveDugeon();                                   ///<.离开场景，伤害值根据时间减少

    uint32          GetPlayerSubDamageTime(uint64 playerID);                     ///< 获得玩家减少伤害值时间点
    HatredInfo*     GetNotifyDamageInfo(WORLD_BOSS_DAMAGE_LIST& list, Player* player);     ///< 在列表中查找同名角色数据
    HatredInfo*     GetBossDamageInfo(uint32 objectID);                                    ///< 获得角色的伤害值

    //=====================================Boss奖励=====================================================
    void            DamageRankReward();                                              ///< 排行榜奖励
    void            ScourgeCorpseCheck();                                            ///< 鞭尸奖励检查
    void            ScourgeCorpseReward(uint64 playerID);                            ///< 鞭尸奖励检查
    void            BossDeadReward();                                               ///<.boss死亡的掉落奖励
    void            CheckDropItemTime();                                            ///<.检查物品显示掉落
    void            AddDropOjectList(uint32 bosscreateid,  Player* player);         ///<.添加掉落物品列表
    void            BossDeadDropItemEvent(uint32 playerid, int32 angle, uint32 bosscreateid);//boss死亡掉落物品
    void            CorpseBossDropEvent(uint32 playerid, int32 angle, uint32 bosscreateid);//boss鞭尸掉落物品
    void            LeveSceneDropItem(Player* player);
    void            SetAttackTarget();                                                      //设置boss的攻击对象
    void            SendLeveSene(Player *player);                                           //玩家离开场景

    bool            IsInBossScourgeCorpseTime();                                                 ///< 是否鞭尸的时间
    bool            JudgeIsCreateBoss();

    HatredManager*  GetBossHatredMgr();

private:
    typedef std::map<uint64, WorldMapPlayerRecord>     PlayerRecordMap;
    typedef std::map<uint64, uint32> SubDamageTimeMap;       // 玩家离开副本时间记录，<playerid,time> time:下次减少伤害值的时间
    typedef std::map<uint64, bool> ScourgeCorpseMap;         // 玩家鞭尸记录<playerid, 是否鞭尸>,1s内是否鞭尸，1s概率掉落

    WorldMapBossData                                   m_bossData;         ///< Boss数据
    SubDamageTimeMap                                    m_subDamageMap;     ///< 扣伤害值时间表
    ScourgeCorpseMap                                    m_scourgeCorpseMap; ///< 玩家鞭尸记录
    int32                                               m_maxdamagePlayerid;///<.记录最大的伤害排名玩家id

    uint32                                              m_next_refresh_time;///<.下一次创建boss时间
    uint32                                              m_how_time;         ///<.刷新boss的倒计时
    uint32                                              m_orderid;          ///<. 当前boss创建的时间id;

    RegularFiledBoss*            m_masterRegular;           ///< 管理者
};

#endif // sub_regular_boss_h__
