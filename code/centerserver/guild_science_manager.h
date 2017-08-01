#ifndef guild_science_manager_h__
#define guild_science_manager_h__

#include "player.h"

// ==================================================
/// ����Ƽ�����ϵͳ
// ==================================================
class GuildScienceManager
{
public:
    GuildScienceManager();
    ~GuildScienceManager();

    DECLARE_SINGLETON(GuildScienceManager)

    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);

private:
    void onAdditionPromote(uint64 playerID, void* data);        // �Ƽ��������� �������������Ϻ�Ӿ��� ת��Game

    void AddGuildAdditionExp(GUILD_ADDITION* additionItem, int32 exp);                          // ��ӹ��ᾭ��

    void SendAdditionPromoteToGame(Player* pPlayer, GUILD_ADDITION_PROMOTE_TO_GAME& sendMsg);   // �Ƽ��ӳɴ������ ת��Game���и��˿Ƽ���������

    GUILD_ADDITION* GetAdditionItem(uint64 guildID, int32 additionID);      // �õ�һ��Ƽ��ӳ���Ϣ
    std::vector<GUILD_ADDITION>* GetScienceAdditionList(uint64 guildID);    // �Ƽ��ӳ���Ϣ
};

#endif // guild_science_manager_h__
