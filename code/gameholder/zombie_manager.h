/************************************************************************/
/* ��ʬ���                                                                     */
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

    //������Ϣ
    void OnZombieWarInfoReq();

    //�콱
    void OnReceiveReward();

    //�Ƿ���Խ���ʬ����
    uint16 CanChangeToZombie(uint32 dungeonId);

    //�ۼӴ��ʱ��
    void AddLiveSeconds(uint32 sec);

    //���ʱ�����
    void CleanLiveTime() { m_cumulativeLiveTime = 0;}

    //�õ��ۼƴ��ʱ��
    uint32 GetLiveTime() { return m_cumulativeLiveTime; }

    //�õ����뽩ʬ��������
    uint32 GetEnterCount() { return m_enterCount; }

    //�õ����������뽩ʬ��������
    uint32 GetMaxEnterCount();

    //��ʬ������������
    void EndZombieWar();

    void EnterZombieWar();

    //��һ���ˢ��
    void CalDailyRefresh();

    //�����Ƿ����
    bool IsFinish(uint32 dungeonId);

    void FinishDungeon(uint32 dungeonId);

    void NotifyNum();//֪ͨ����

private:
    Player *m_player;

    //���յĴ���
    uint32 m_enterCount;

    //�ۼƴ��ʱ��
    uint32 m_cumulativeLiveTime;

    //��ɵĽ�ʬ����
    std::set<uint32> m_finishDungeons;
};

#endif  //zombie_manager_h__