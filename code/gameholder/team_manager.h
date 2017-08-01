#ifndef TEAM_MANAGER_H
#define TEAM_MANAGER_H

class Player;
class TeamRelationShip;
class Monster;

/*
 *	组队系统
 */
class TeamManager
{
public:
    TeamManager();
    ~TeamManager();

    DECLARE_SINGLETON(TeamManager)

    void OnRecvFromPlayer(Player* player, uint32 msgID, void* data);

    void OnRecvFromCenter(Player* player, uint32 msgID, void* data);

    TEAM_INFO* GetTeamInfo(uint64 teamID);

    uint32 GetTeamChannelIDByPlayer(Player* player,uint32 dungeonID);           // 获取队伍在副本中的频道id

    void UpdateTeamMemberInfo(Player* player);

    bool TeamExpShare(uint64 teamID, int32 exp, Monster* pDefender);

private:
    // 队伍信息请求
    void OnRecvTeamInfoReq(Player* player);

    // 队伍信息通知
    void OnRecvTeamInfoNtf(void* data);

    // 创建队伍请求
    void OnRecvCreateTeamReq(Player* player, void* data);

    // 销毁队伍请求
    void OnRecvDestroyTeamReq(Player* player);

    // 删除队友请求
    void OnRecvTeamMemberDelReq(Player* player, void* data);

    // 快速加入队伍请求
    void OnRecvTeamQuickJoinReq(Player* player, void* data);

    // 队伍显示列表请求
    void OnRecvTeamShowListReq(Player* player, void* data);

    // 重置队员状态
    void OnRecvTeamMemberResetState(Player* player, void* data);

    // 请求飞到队友身边
    void OnRecvFlyReq(Player* player, void* data);

    //----------------------------------------------

    // 更新队伍信息
    void OnCreateTeamInfo(TEAM_INFO* team_info);

    // 添加队伍信息
    void OnAddTeamInfo(TEAM_INFO* team_info);

    // 删除队伍信息
    void OnDelTeamInfo(TEAM_INFO* team_info);

    // 更新队伍信息
    void OnUpdateTeamInfo(TEAM_INFO* team_info);

    // 销毁队伍
    void OnDestoryTeamInfo(TEAM_INFO* team_info);

    // 填充队伍信息
    void FillTeamInfo(TEAM_INFO* inInfo, TEAM_INFO& outInfo, uint8 opt_flag);

    // 通知空的队伍信息
    void NotifyMemberEmptyTeamInfo(Player* player, int32 teamType);

    // 发送添加队友应答
    void SendTeamMemberAddAck(Player* player, uint16 errcode, std::string src_name, std::string dst_name);

    // 获取成员信息
    const TEAM_MEMBER* GetTeamMemberData(uint64 teamID, uint64 memberID);
    
    TEAM_MEMBER MakeTeamMember(Player* player);

private:
    // 队伍信息
    typedef std::map<uint64, TEAM_INFO>  TeamMap;    // <队伍id, 队伍信息>
    TeamMap m_teamMap;

};

#endif
