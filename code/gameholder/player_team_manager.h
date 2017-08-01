/*
 *	玩家的队伍管理器(管理同gameserver上的队友对象)
 */
#ifndef player_team_manager_h__
#define player_team_manager_h__

class Player;

class PlayerTeamManager
{
public:
    PlayerTeamManager(Player* player);
    ~PlayerTeamManager();
    
    void OnAddTeamMember(Player* player);
    void OnRemoveTeamMember(Player* player);
    void OnRemoveAll();
    bool IsTeamMate(Player* player);

    uint32 GetTeamMateNum() {return m_TeamMates.size();}

    void ShareExpToTeamInSameScene(int32 exp);          // 分享经验给同场景队友
    void ShareGamePointToTeamInSameScene(int32 game_point, int32 reason);    // 分享金币给同场景队友
    void ShareSoulPointToTeamInSameScene(int32 soul_point, int32 reason);    // 分享精魂给同场景队友

    void SendTaskEventToTeamInSameScene(uint32 event_id, int32 arg1, int32 arg2, int32 arg3);      // 发送事件给队友


private:
    Player*             m_player;
    std::vector<Player*>     m_TeamMates;       // 队友对象列表

};

#endif // player_team_manager_h__
