#ifndef FACTION_MANAGER_H
#define FACTION_MANAGER_H

class Player;
class RegularFactionPK;

typedef std::map<uint64, FACTION_PLAYER_INFO> FactionPlayerList;

typedef struct PlayerBattlePower
{
    PlayerBattlePower()
    {
        Clear();
    }

    void Clear()
    {
        playerID = 0;
        battlePower = 0;
    }

    uint64 playerID;
    uint32 battlePower;
}PlayerBattlePower;

// ==================================================
// 派别管理系统(圣剑争锋)
// ==================================================
class FactionManager
{
public:
    FactionManager();
    ~FactionManager();

    DECLARE_SINGLETON(FactionManager)

    void    Clear();

    void    Update();
    void    OneSecondUpdate(uint32 delayTime);

    void    OnRecv(Player* player, uint32 msgID, void* data);

    uint16  CanEnter(Player* player);

    void    SetPlayerFaction(Player* player, uint8 factionType);
    void    DelPlayer(Player* player);
    FACTION_PLAYER_INFO* GetPlayerInfo(uint64 playerID);

    void    AddTimeScore();                                     // 添加时间积分
    void    CalScore(Player* pAttacker, Player* pDeader, RegularFactionPK* regular);       // 计算积分 处理连杀

    void    SetDirty() {m_dirty = true;}
    uint32  GetCurrTime() {return m_currTime;}                  // 毫秒
    uint8   GetState() {return m_state;}

    enum
    {
        kReady  = 1,
        kStart  = 2,
        kEnd    = 3,
    };
private:
    void    RankRefresh();                                              // 刷新排名
    void    SendRankList(std::vector<FACTION_PLAYER_INFO*> rankList);   // 发送排名列表
    void    SendScore(Player* player);
    void    SendReward();

    // 两派PK请求
    void    OnFactionPKReq(Player* player, void* data);
    // 活动开启
    void    OnActivityStart(void* data);
    void    OnActivityEnd();

    void    Start();

    void    WaitBattlePowerList(std::vector<PlayerBattlePower>& list);            // 得到等待大厅玩家战斗力列表

    void    ChangeScene(std::vector<PlayerBattlePower*>& battlePowerList, uint32& channelID, int32& currChannelPlayerNum);
private:
    FactionPlayerList  m_factionPlayerList;     // 玩家信息

    int32   m_blueFactionScore;                 // 红派积分
    int32   m_redFactionScore;                  // 兰派积分

    uint32  m_nextAddScoreTime;                 // 下次加积分的时间
    bool    m_dirty;

    uint32  m_MinuteTimer;
    uint32  m_currTime;                         // 当前时间
    uint8   m_state;                            // 活动状态
};

#endif
