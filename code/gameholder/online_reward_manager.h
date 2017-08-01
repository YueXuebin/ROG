/*
 *	���߽���ϵͳ
 */
#ifndef ONLINE_REWARD_MANAGER_H
#define ONLINE_REWARD_MANAGER_H

class Player;

class OnlineRewardManager
{
public:
    OnlineRewardManager(Player* player);
    ~OnlineRewardManager();

    void Clear();
    void OnRecv(uint32 msgID, void* data);

    void LoadInfo(const ONLINE_REWARD_INFO& info);
    void FillInfo(ONLINE_REWARD_INFO& info);

    void CalDailyRefresh();

    void Update();

    ONLINE_REWARD_INFO& GetInfo() {return m_online_reward_info;}

    uint32 GetOnlineRewardTime() { return m_online_reward_info.online_reward_time; }            // ������߽���ʱ��
    void SetOnlineRewardTime(uint32 val) { m_online_reward_info.online_reward_time = val; }     // �������߽���ʱ��

    uint32 GetOnlineRewardDay() { return m_online_reward_info.online_reward_day; }              // ������߽���������
    void SetOnlineRewardDay(uint32 val) { m_online_reward_info.online_reward_day = val; }       // ������߽���������

    uint32 GetOnlineRewardFlag() { return m_online_reward_info.online_reward_flag; }            // ������߽������

    //�����Ʒ������
    enum 
    {
        SingleType,
        CrityType
    };

private:
    bool IsOnlineRewardFlag(uint32 id);                                                         // �Ƿ����߽��������
    void SetOnlineRewardFlag(uint32 id);                                                        // �������߽������

    bool IsLevelRewardFlag(int32 level);                                                        // �Ƿ���ȡ���ȼ�����
    void SetLevelRewardFlag(int32 level);                                                       // ������ȡ���ȼ�����

    void RecvOnlineRewardReq(void* data);                                                       // ��ȡ���߽���
    void InitOnlineRewardInfo();                                                                // �������߽�����Ϣ
    void InitOnlineRewardGrowList();
    void OnGrowLevelReq(void* data);                                                            // �ȼ�����
    void BestowRewardReq(void* data);                                                           // ����ȫ������

    bool RandomGrowValue(int32 curnum);
    bool AddCritGrowreward();
    bool RandomSingleGrowValue();
    bool CalculationGrowValue(REWARD_ITEM* curitem);                                            // ����Ҫ������Ʒ�ĳɳ�ֵ
    int32 OnGetGrowBestowtime();                                                                // ������ȡ�ɳ��͸���ʱ���;
    void CalBestowReward(BESTOW_REWARD_ACK& sendMsg, int32 days, int32 curTime);                 // ��ȡ���������������ͽ����ַ�

    int32 CalAddItemNum(int32 num);                                                             //

    void AddGrowReward(int32 type, int32 num ,int32 id,int32 randomtype);                       // �����Ӧ�ĳɳ�ֵ��typeͳ�Ƹı����ĸ����͵Ľ���
    void ClearGrowList();                                                                       // ���óɳ��б������Ϊ0

    void FindGrowReward(REWARD_TABLE& rewardtable);                                             // ���ҳɳ�����

private:
    Player*             m_player;
    uint32              m_secondTimer;

    ONLINE_REWARD_INFO  m_online_reward_info;

    std::vector<REWARD_ITEM>  m_onlinechangItem;                                                // ͳ�Ƹı�����Щ����ֵ���ǵ����Ļ��Ǳ����ģ�����Ǳ�������ô�����е�growlist�б�
};

#endif