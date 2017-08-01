/*
 *	对象关系管理
 */
#ifndef team_relationship_h__
#define team_relationship_h__

class TeamManager;
class Player;
class TeamRelationInfo;
class SceneObject;

class TeamRelationShip
{
public:
    TeamRelationShip();
    ~TeamRelationShip();

    DECLARE_SINGLETON(TeamRelationShip)

    //
    void OnCreateTeam(const TEAM_INFO& team_info);
    void OnDestoryTeam(const TEAM_INFO& team_info);
    void OnAddMember(const TEAM_INFO& team_info);
    void OnDelMember(const TEAM_INFO& team_info);
    void OnUpdateInfo(const TEAM_INFO& team_info);

    void OnDelPlayer(Player* player);
    void OnPlayerChangeDungeon(Player* player);

    // 消息通知
    void SendAppearToTeamMemberInSameScene(SceneObject* obj);
    void SendDisappearToTeamMemberInSameScene(SceneObject* obj);
    void SendMessageToTeamMemberInSameScene(Player* sender, uint32 msgId, void* msg, bool exclude_sender);      // 发送消息给同场景中的队友
    void SendMessageToTeamMember(Player* sender, uint32 msgId, void* msg, bool exclude_sender);                 // 发送消息给队友(可能在不同场景中)

    //
    TeamRelationInfo* GetTeamRelationInfo(uint64 teamID);

    bool HasRelation(SceneObject* obj1, SceneObject* obj2);     // 判断两对象是否有关系

private:
    bool IsSameTeam(Player* player1, Player* player2);

    void UpdatePlayerTeamMember(Player* player);
    void UpdateTeamMember(const TEAM_INFO& team_info);

    void OnCreateRelation(TeamRelationInfo* pInfo, Player* player);
    void OnDestoryRelation(TeamRelationInfo* pInfo, Player* player);

private:
    typedef std::map<uint64, TeamRelationInfo*>  TeamRelationShipMap;    // <队伍id, 成员对象>
    TeamRelationShipMap m_RelationShip;         // 

};

#endif // team_relationship_h__
