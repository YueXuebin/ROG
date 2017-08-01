#ifndef offline_event_game_manager_h__
#define offline_event_game_manager_h__

class OfflineEventGameManager
{
public:
    OfflineEventGameManager();
    ~OfflineEventGameManager();

    DECLARE_SINGLETON(OfflineEventGameManager)

    void OnRecv(uint64 playerID, uint32 msgID, void* data);

    void sendOfflineEvent(uint64 playerID, uint32 centerID, uint32 eventID, std::string& eventInfo);
private:
    void onOfflineEventNtf(uint64 playerID, void* data);
};

#endif // offline_event_game_manager_h__
