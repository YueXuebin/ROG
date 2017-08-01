#ifndef team_manager_h__
#define team_manager_h__


class Player;

/*
*   单服组队管理器
*/
class TeamManager
{
public:
    TeamManager();
    ~TeamManager();

    DECLARE_SINGLETON(TeamManager)

    TEAM_INFO* GetTeamInfo(uint64 teamID);

    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);       // 接收网络消息

    void LeaveTeam(uint64 playerID);                                                // 离开队伍

    void OnPlayerChangeDungeon(Player* player);                                     // 玩家跳副本

    void ValidCheck();                                                              // 队伍信息合法性检测(调试用)

    void SendToAllTeamMember(TEAM_INFO* teamInfo, uint32 msgID, void* data, uint64 exceptID = 0);   // 通知队伍成员

    void SendToAllTeamMember(uint64 teamID, uint32 msgID, void* data, uint64 exceptID = 0);         // 通知队伍成员

    void SendAllTeamInfoToGameServer(uint32 sessionID);                                    // 通知游戏服务器所有的队伍信息

    void SetMemberProp(uint64 playerID, uint32 dungeonID);

    void SetTeamGameServerID(uint64 teamID, uint32 GameServerID);

private:    // 组队副本

    void RecvCreateTeamReq(uint64 playerID, uint32 sessionID, void* data);              // 创建队伍请求

    void RecvDestoryTeamReq(uint64 playerID, uint32 sessionID, void* data);             // 销毁队伍请求

    void RecvTeamMemberDelReq(uint64 playerID, uint32 sessionID, void* data);           // 删除队伍成员请求

    void RecvTeamQuickJoinReq(uint64 playerID, uint32 sessionID, void* data);           // 快速加入队伍请求

    void RecvTeamMemberJoinAns(uint64 playerID, uint32 sessionID, void* data);          // 队长确认申请

    void RecvUpdateTeamMemberInfo(uint64 playerID, uint32 sessionID, void* data);       // 更新队员信息

    //------------------------------------------------------

    uint64 GenTeamID();

    TEAM_MEMBER MakeTeamMember(Player* player);
    TEAM_MEMBER MakeTeamMember(uint64 offlinePlayerID, const std::string& offlinePlayerName);

    void FillTeamInfo(const TEAM_INFO* inInfo, TEAM_INFO& outInfo, uint8 opt_flag);

    bool CreateTeam(const TEAM_MEMBER &leader, uint32 teamType, const std::string& teamName);                  // 创建队伍

    bool DestoryTeam(Player* player);                                   // 销毁队伍
    bool DestoryTeamByID(uint64 teamID);                                // 销毁队伍

    void DestoryTeamIfOnlyOne(Player* player);                          // 销毁只有一个人的队伍
    void DestoryTeamIfOnlyOneByID(uint64 teamID);                       // 销毁只有一个人的队伍

    bool HasMember(uint64 teamID, Player* player);                      // 检查队伍中是否有此成员
    bool HasMember(uint64 teamID, uint32 playerID_h, uint32 playerID_l);

    bool IsTeamLeader(uint64 teamID, uint64 playerID);                  // 检查是否为队长

    TEAM_MEMBER* GetTeamLeader(TEAM_INFO* pTeam);    

    uint64 GetTeamLeaderID(TEAM_INFO* pTeam);

    bool HasOnlinePlayer(TEAM_INFO* pTeam);                             // 是否有在线玩家

    void AddTeamMember(TEAM_INFO* teamInfo, const TEAM_MEMBER& team_member);    // 添加队伍成员

    std::string DelTeamMember(uint64 teamID, uint64 playerID);          // 删除队伍成员

    //------------------------------------------------------

    void SendCreateTeamAck(uint16 errCode, uint64 playerID, uint32 serverID);             // 发送创建队伍应答

    void SendDestoryTeamAck(uint16 errCode, Player* player);            // 发送销毁队伍应答

    void SendTeamMemberDelAck(Player* player, uint64 teamID, const std::string& src_name, const std::string& dst_name, const std::string& leader_name);  // 发送删除组队成员消息

    void SendTeamUpdateInfoToGameServer(TEAM_INFO* pTeam, TEAM_MEMBER* pMember = NULL);                     // 通知队伍信息

    void SendTeamUpdateInfoToMember(TEAM_INFO* pTeam, TEAM_MEMBER* pMember = NULL);                  // 通知队伍信息

    //------------------------------------------------------

    void NotifyPlayerDelTeam(Player* player);                           // 通知玩家离开队伍

    void NotifyGameSvrEmptyTeamInfo(uint64 teamID);     // 通知空的队伍信息

    void NotifyGameServerTeamInfo(TEAM_INFO* teamInfo);                    // 通知队伍信息

    void NotifyTeamMemberTeamInfo(TEAM_INFO* pTeam, Player* player);    // 通知队伍信息

    void NotifyGameSvrAddTeamMember(TEAM_INFO* pTeam, const TEAM_MEMBER& team_member);

    void NotifyTeamMemberAddMember(TEAM_INFO* pTeam, const TEAM_MEMBER& team_member);

    void NotifyGameSvrDelTeamMember(uint64 teamID, uint64 playerID);    // 通知删除队员

    void NotifyTeamMemberDelMember(uint64 teamID, uint64 playerID);     // 通知删除队员

    uint32 GetTeamChannelID(TEAM_INFO* pTeam, uint32 dungeonID);         // 获取队伍在副本中的频道id

    TEAM_MEMBER* GetMemberInfo(TEAM_INFO* pTeam, uint64 playerID);
    void UpdateMemberInfo(TEAM_MEMBER* team_member, TEAM_MEMBER* src_team_member);              // 更新队员信息

private:
    typedef std::map<uint64, TEAM_INFO>  TeamMap;                       // <队伍id, 队伍信息>
    TeamMap m_teamMap;
};


#endif // team_manager_h__
