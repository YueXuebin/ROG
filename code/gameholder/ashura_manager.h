#ifndef ashura_manager_h__
#define ashura_manager_h__

// 修罗城管理器 (暂时用于排行)

#include "ashura_config.h"
class Player;

typedef struct RankInfo
{
    void Clear()
    {
        name        = "";
        killNum     = 0;
        deadNum     = 0;
        killRank    = 0;
        deadRank    = 0;
    }

    std::string name;
    int32       killNum;
    int32       deadNum;
    int32       killRank;
    int32       deadRank;
    int32       level;
}RankInfo;

class AshuraManager
{
    typedef std::vector<RankInfo> RankList;
public:
    AshuraManager();
    ~AshuraManager();

    void Clear();

    DECLARE_SINGLETON(AshuraManager)

    void    OnRecv(uint32 msgID, void* data);

    void    Update();                   // 心跳

    void    AddKillNum(const std::string& killerName, int32 killLevel);         // 杀人数加1
    void    AddDeadNum(const std::string& deaderName, int32 deadLevel);         // 死亡数加1

    void    RefreshRankList();                                  // 刷新排行榜
    void    RankSort();                                         // 排序
    void    SendRankToOne(Player* pPlayer);                     // 发送数据给刚进来的玩家

    int16   CanChangeToAshura(CHANGE_DUNGEON_REQ* dungeonReq);  // 能否跳转入修罗场
    uint32  GetCurrTime() {return m_currentTime;}
    bool    IsOpen() {return m_isOpen;}

private:
    void    OnStart(void* data);
    void    OnEnd();

    void    Notify();

    void    SendReward();
    REWARD_TABLE CalAshuraRewardList(RankInfo rankinfo);

    void CalPlayerNumReward(REWARD_TABLE& rewardTable, RankInfo rankInfo);             // 人头计算
    void CalNumberReward(const AshRewardList& rewardListTmp, int32 num, uint32& gamepoint, uint32& soulpoint, uint32& exp);

    void CalTitleRewardList(REWARD_TABLE& rewardTable, RankInfo rankInfo);             // 称号计算
    void CalTitleReward(int32 num, const std::map<uint32, REWARD_TABLE>& rewardTmp, REWARD_TABLE& rewardTable);

    void CalRankReward(REWARD_TABLE& rewardTable, const RankInfo& rankInfo);

private:
    RankList m_RankList;

    ASHURA_RANK_ACK m_RankListToOne;     // 保存一个临时的前10的排名 给刚进来的玩家

    bool    m_dirty;

    uint32  m_MinuteTimer;
    uint32  m_currentTime;          // 活动开启到现在的时间
    bool    m_isOpen;               // 活动是否开启
};

#endif // ashura_manager_h__
