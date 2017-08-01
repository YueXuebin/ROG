/*
 *	跨服组队管理器(主要是消息的转发)
 */
#ifndef cross_team_manager_h__
#define cross_team_manager_h__

class Player;

class CrossTeamManager
{
public:
    CrossTeamManager();
    ~CrossTeamManager();

    DECLARE_SINGLETON(CrossTeamManager)

    void OnRecv( uint64 playerID, uint32 sessionID, uint32 msgID, void* data );

    void LeaveTeam(uint64 playerID);                                                // 离开队伍

private:
    void ReSendToGame(Player* player, uint32 msgID, void* data);
    void ReSendToGame(uint64 playerID, uint32 gateSvrID, uint32 msgID, void* data);
};


#endif // cross_team_manager_h__
