#ifndef guild_science_recorder_h__
#define guild_science_recorder_h__

#include "player.h"

class GuildScienceRecorder
{
public:
    GuildScienceRecorder(Player* pPlayer);
    ~GuildScienceRecorder();

    void OnRecv(uint32 msgID, void* data);

    void LoadGuildScienceInfo(GUILD_PLAYER_SCIENCE_INFO& info);                             // ���ظ��˹���Ƽ���Ϣ
    void FillGuildScienceInfo(GUILD_PLAYER_SCIENCE_INFO& info);                             // �����˹���Ƽ���Ϣ

    void CalDailyRefresh();                                                                 // ÿ��ˢ��

    void AddAdditionPromoteNum(int32 additionID, int32& num);                               // ����Ƽ�����Ʒ��ȡ�ļӳ�

    //void CBGuildAdditionPromoteTrade(const std::string& str);                             // �ص�����Ƽ�����
private:
    void onGuildSciencePlayerInfo();                                                        // �Ƽ�������Ϣ����(�����ж� ת����Cente����)

    void onAdditionPromoteReq(void* data);                                                  // �Ƽ��ӳ�����
    void PromoteGamePoint(GUILD_PLAYER_ADDITION* playerAdditionItem);                       // ��Ҽӳ�
    void PromoteMoneyPoint(GUILD_PLAYER_ADDITION* playerAdditionItem);                      // ħʯ�ӳ�

    void onGuildAdditionPromoteToGame(void* data);                                          // �Ƽ��ӳɻص� (�Ѵ��������ӹ��ᾭ��, �����˿Ƽ���Ϣ����)

    GUILD_PLAYER_ADDITION* GetGuildPlayerAddition(int32 additionID);                        // ���˼ӳ���Ϣ
    GUILD_ADDITION* GetGuildAddition(int32 additionID);                                     // ����ӳ���Ϣ
private:
    Player* m_player;

    std::vector<GUILD_PLAYER_ADDITION>	m_additionList;     // ���˼ӳ���Ϣ

    bool    m_lock;                                         // �첽��
};

#endif // guild_science_recorder_h__
