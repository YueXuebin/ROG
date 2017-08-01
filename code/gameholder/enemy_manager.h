/*
 *	善恶信息管理器(玩家在虚空幻境)
 */
#ifndef enemy_manager_h__
#define ememy_manager_h__

class Player;

class EnemyManager
{
public:
    EnemyManager(Player* player);
    virtual ~EnemyManager();

    void            LoadInfo(const ENEMY_INFO& enemy_info);                         ///< 加载仇人信息

    void            FillInfo(ENEMY_INFO& enemy_info);                               ///< 填充仇人信息

    void            OnRecv(uint32 msgID, void* data);                               ///< 消息接收

    void            Update();                                                       ///< 更新

    bool            IsRedName();                                                    ///< 是否红名玩家

    bool            IsInBeatBackTime(uint64 playerID);                              ///< 是否在反击时间内

    int32           GetEvilPoint();                                                 ///< 获得善恶点

    void            AddEvilPoint(int32 val);                                        ///< 添加善恶点

    void            SubEvilPoint(int32 val);                                        ///< 减少善恶点

    ENEMY_RECORD*   GetEnemyRecord(uint64 playerID);                                ///< 获得仇人记录

    uint8           GetEnemyFlag(uint64 playerID);                                  ///< 获得仇人标记

    bool            IsEnemy(uint64 playerID, uint8 flag = 0);                       ///< 判断仇人

    void            AddEnemy(uint64 playerID, uint8 flag);                          ///< 添加仇人

    void            AddAttacker(uint32 creatureID);                                 ///< 添加攻击者名单

    int32           GetFightMode() { return m_fight_mode; }                         ///< 获得战斗模式

    void            AddAttacEvilVal(EnemyManager* defenceEnemyMgr);                 ///< 添加因攻击影响的罪恶值

    void            AddKillEvilVal(EnemyManager* defenceEnemyMgr);                  ///< 添加击杀影响的罪恶值

    void            SetFightMode(int32 val);                                        ///< 设置战斗模式

    void            SendEnemyInfoNtf();                                             ///< 善恶信息通知

    void            CalEvilPoint();                                                 ///< 玩家上线重新计算玩家罪恶值

private:
    void            OnFightModeReq(void* data);                                     ///< 设置战斗模式

    void            SendFightModeAck(int32 fightMode);                              ///< 发送战斗模式

    void            SendEvilInfoNtf();                                                 ///< 发送善恶信息

    void            IsInBossScene();                                               ////< 是否是在珍兽场景中

private:
    Player*                 m_player;
    int32                   m_fight_mode;                                           ///< 战斗模式
    ENEMY_INFO              m_enemy_info;                                           ///< 仇人列表

    ENEMY_INFO              m_notify_info;                                          ///< 通知列表

    ENEMY_ATTACKER_INFO     m_attacker_info;                                        ///< 攻击者列表
};

#endif  // enemy_manager_h__
