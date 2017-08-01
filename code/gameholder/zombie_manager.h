/************************************************************************/
/* 僵尸存活                                                                     */
/************************************************************************/

#ifndef zombie_manager_h__
#define zombie_manager_h__

class Player;


class ZombieManager
{
public:
    ZombieManager(Player *player);
    ~ZombieManager();

    void LoadZombieInfo(const ZOMBIE_INFO &info);

    void FillZombieInfo(ZOMBIE_INFO &info);

    void OnRecv(uint32 msgId, void* data);

    void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

    //请求信息
    void OnZombieWarInfoReq();

    //领奖
    void OnReceiveReward();

    //是否可以进僵尸副本
    uint16 CanChangeToZombie(uint32 dungeonId);

    //累加存活时间
    void AddLiveSeconds(uint32 sec);

    //存活时间归零
    void CleanLiveTime() { m_cumulativeLiveTime = 0;}

    //得到累计存活时间
    uint32 GetLiveTime() { return m_cumulativeLiveTime; }

    //得到进入僵尸副本次数
    uint32 GetEnterCount() { return m_enterCount; }

    //得到最大允许进入僵尸副本次数
    uint32 GetMaxEnterCount();

    //僵尸副本结束处理
    void EndZombieWar();

    void EnterZombieWar();

    //新一天的刷新
    void CalDailyRefresh();

    //副本是否完成
    bool IsFinish(uint32 dungeonId);

    void FinishDungeon(uint32 dungeonId);

    void NotifyNum();//通知次数

private:
    Player *m_player;

    //今日的次数
    uint32 m_enterCount;

    //累计存活时间
    uint32 m_cumulativeLiveTime;

    //完成的僵尸副本
    std::set<uint32> m_finishDungeons;
};

#endif  //zombie_manager_h__