/*
 *	离线玩家数据管理器
 */
#ifndef offline_player_manager_h__
#define offline_player_manager_h__

class OfflinePlayer;
class Player;

// 离线玩家信息记录
struct OFFLINE_PLAYER_RECORD
{
    OFFLINE_PLAYER_INFO   record;
    int32   recordTime;             // 缓存计时
};
// 请求记录
struct REQUEST_RECORD
{
    uint64      AskPlayerID;
    uint32      Ask_MessageID;
    uint64      offlinePlayerID;
    std::string offlinePlayerName;

    REQUEST_RECORD()
    {
        Clear();
    }

    void Clear()
    {
        AskPlayerID = offlinePlayerID = 0;
        Ask_MessageID = 0;
        offlinePlayerName.clear();
    }
};


class OfflinePlayerManager
{
public:
    OfflinePlayerManager();
    ~OfflinePlayerManager();

    DECLARE_SINGLETON(OfflinePlayerManager);

    void Update();

    // 玩家查询离线玩家信息
    void RequestOfflinePlayerInfoByID(uint64 offlinePlayerID, uint8 reason, uint32 msgID, Player* player);
    void RequestOfflinePlayerInfoByName(const std::string& offlinePlayerName, uint8 reason, uint32 msgID, Player* player);

    const OFFLINE_PLAYER_INFO* GetOfflinePlayerRecord(uint64 offlinePlayerID);
    const OFFLINE_PLAYER_INFO* GetOfflinePlayerRecordByName(const std::string& offlinePlayerName);

    void AskCSOfflinePlayerInfoByName(const std::string& offlinePlayerName, uint8 reason);
    void AskCSOfflinePlayerInfo(uint64 offlinePlayerID, uint8 reason);               // 向CS请求离线玩家数据
    void    CBRequestOfflinePlayerInfo(const OFFLINE_PLAYER_ACK& record);

    void    OfflinePlayerInfoReq(uint32 msgID, void* data, Player* player);
private:
    void    OnLoadOfflinePlayerInfo(uint64 offlinePlayerID);
    void    OnSendToRequestPlayer(uint64 offlinePlayerID, uint8 reason);

    void    RefreshPlayerRecord();

private:
    std::map<uint64, OFFLINE_PLAYER_RECORD>     m_OfflinePlayerRecords;

    int32   m_MinuteTimer;
    std::vector<REQUEST_RECORD>            m_offlineReqList;   // <answer player id, 请求记录>
};


#endif // offline_player_manager_h__
