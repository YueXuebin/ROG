#ifndef gameplaytimes_notifier_h__
#define gameplaytimes_notifier_h__

class Player;

// 各功能模块的信息管理器
class GameplayTimesNotifier
{
public:
    GameplayTimesNotifier(Player* player);
    ~GameplayTimesNotifier();

    void OnRecv(uint32 msgID, void* data);

private:
    void OnGameNotifyReq(void* data);
    void SendGameNotify(uint32 notifyId);
    void SendGameNotifyList();

    void SendRewardNotify(uint32 notifyId);

public:
    void SetGameplayTimes(uint32 gameplayID, int32 currTimes, int32 sumTimes = 0);

    void RewardNotify(uint32 gameplayID, bool bNotify);

private:
    Player* m_player;

    typedef std::map<uint32, GAMEPLAY_TIMES_NOTIFY> GameNotifyMap;
    GameNotifyMap   m_GamePlayTimesMap;
    GameNotifyMap   m_RewardNotifyMap;
};

#endif // gameplaytimes_notifier_h__
