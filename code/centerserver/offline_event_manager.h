/*
 *	离线事件管理器
 *  玩家离线时发生的行为通知(比如,离线充值)
 */
#ifndef offline_event_manager_h__
#define offline_event_manager_h__

class Player;

class OfflineEventManager
{
public:
    OfflineEventManager();
    ~OfflineEventManager();

    DECLARE_SINGLETON(OfflineEventManager)

    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);

    void SendOfflineEvent(uint64 playerID, uint32 eventID, const std::string& eventInfo);
    void SendOfflineEvent(const std::string& account_name, const std::string& server_name, uint32 eventID, const std::string& eventInfo);

    void OnPlayerChangeDungeon(Player* pPlayer);
private:
    void OnOfflineEventSend(void* data, uint64 playerID);
};

#endif // offline_event_manager_h__