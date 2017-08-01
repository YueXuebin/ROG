#ifndef cross_team_manager_h__
#define cross_team_manager_h__


class Player;

/*
 *	跨服组队管理器
 */
class CrossTeamManager
{
public:
    CrossTeamManager();
    ~CrossTeamManager();

    DECLARE_SINGLETON(CrossTeamManager)


    void OnRecvFromPlayer(Player* player, uint32 msgID, void* data);

    void OnRecvFromCenter(uint64 playerID, uint32 msgID, void* data);

    uint64 GetTeamID(uint64 playerID);

private:
    void OnCrossTeamShowListReq(uint64 playerID, void* data);

    void OnCrossTeamInfoReq(uint64 playerID, void* data);

    void OnCreateTeamReq(uint64 playerID, void* data);

    void OnDestoryTeamReq(uint64 playerID, void* data);

    void OnTeamMemberDelReq(uint64 playerID, void* data);

    void OnTeamMemberAddAns(uint64 playerID, void* data);

    void OnTeamQuickJoinReq(uint64 playerID, void* data);

    void OnTeamMemberReadyReq(uint64 playerID, void* data);

    void OnTeamFlagSetReq(uint64 playerID, void* data);

    void OnTeamEnterDungeonReq(uint64 playerID, void* data);

    void OnDungeonScoreInfoReq(uint64 playerID, void* data);
    void OnDungeonScoreInfoAck(uint64 playerID, void* data);

    //--------------------
    bool RecvTeamMemberReadyReq(Player* player, void* data);
    void RecvTeamBuyDungeonTimes(Player* player, void* data);

    //--------------------
    // 队友离开应答
    void SendErrorCode(uint64 playerID, uint32 gateID, uint16 errcode);
    void SendTeamMemberDelAck(uint64 delPlayerID, uint32 delPlayerGateID, uint64 teamID, const std::string& dst_name, const std::string& src_name, const std::string& leader_name);
    void SendTeamMemberAddAck(uint64 playerID, uint32 gateID, uint16 errcode, const std::string& src_name, const std::string& dst_name);    // 队友加入应答
    
    // 队伍成员消息通知
    void SendToAllTeamMember(CROSS_TEAM_INFO* pTeam, uint32 msgID, void* data, uint64 exceptID);
    void SendToAllTeamMember(uint64 teamID, uint32 msgID, void* data, uint64 exceptID = 0);

    void SendTeamUpdateInfoToMember(CROSS_TEAM_INFO* pTeam);

    //--------------------
    void NotifyTeamMemberTeamInfo(uint64 playerID);                             // 队伍信息通知
    void NotifyPlayerDelTeam(uint64 playerID, uint32 playerGateID);             // 队伍解散通知
    void NotifyTeamMemberDelMember(uint64 teamID, uint64 playerID);             // 队友离开通知
    void NotifyTeamMemberAddMember(uint64 teamID, const CROSS_TEAM_MEMBER& team_member);  // 成员加入通知
    void NotifyChangeTeamDungeon(CROSS_TEAM_INFO* team_info);

    //--------------------
    bool CreateTeam(const CROSS_TEAM_MEMBER& leader, uint32 dungeonID, uint32 teamLevel);
    bool DestoryTeamByID(uint64 teamID);

    void AddTeamMember(uint64 teamID, const CROSS_TEAM_MEMBER& team_member);

    void AddMemberToTeam(const CROSS_TEAM_MEMBER& member, uint64 teamID);
    void RemoveMemberFromTeam(uint64 playerID, uint64 teamID);

    
    CROSS_TEAM_INFO*    GetTeamInfoByPlayer(uint64 playerID);
    CROSS_TEAM_INFO*    GetTeamInfo(uint64 teamID);

    uint64 GetTeamLeader(uint64 teamID);
    CROSS_TEAM_MEMBER* GetTeamLeaderData(uint64 teamID);
    CROSS_TEAM_MEMBER* GetTeamMemberData(uint64 teamID, uint64 playerID);
    CROSS_TEAM_MEMBER* GetTeamMemberDataByName(uint64 teamID, const std::string& name);

    bool HasMember(uint64 teamID, uint64 playerID);
    bool HasOnlinePlayer(CROSS_TEAM_INFO* pTeam);

    void FillTeamInfo(const CROSS_TEAM_INFO* inInfo, CROSS_TEAM_INFO& outInfo, uint8 opt_flag);

    uint64 GenTeamID();

    CROSS_TEAM_MEMBER MakeTeamMember(Player* player);

private:
    // 队伍信息
    typedef std::map<uint64, CROSS_TEAM_INFO>  CrossTeamMap;    // <队伍id, 队伍信息>
    CrossTeamMap m_teamMap;

    // 玩家的队伍信息
    typedef std::map<uint64, uint64>  PlayerTeamMap;            // <player id, 队伍id>
    PlayerTeamMap   m_PlayerTeamMap;

};


#endif // cross_team_manager_h__
