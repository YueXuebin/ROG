/*
 *	玩家对比管理器
 */
#ifndef contrast_manager_h__
#define contrast_manager_h__

class Player;

class ContrastManager
{
public:
    ContrastManager(Player* player);
    ~ContrastManager();

    void OnRecv(uint32 msgID, void* data);

    void ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 );

private:
    void SendContrastPlayerInfo(const OFFLINE_PLAYER_INFO* otherPlayerInfo);

private:
    Player* m_Player;

    std::string     m_ContrastPlayerName;       // 要对比的玩家名
};

#endif // contrast_manager_h__
