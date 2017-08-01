#include "centerserver_pch.h"
#include "team_manager.h"
#include "player.h"
#include "player_manager.h"
#include "player_net_handler.h"
#include "config_loader.h"
#include "game_handler.h"
#include "gate_handler.h"
#include "dungeon_manager.h"
#include "offline_player_job.h"

IMPLEMENT_SINGLETON(TeamManager)

TeamManager::TeamManager()
{

}

TeamManager::~TeamManager()
{

}

// ==================================================
// 接收网络消息
// ==================================================
void TeamManager::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_CREATE_TEAM_REQ:                   // 创建队伍
        RecvCreateTeamReq(playerID, sessionID, data);
        break;
    case MSG_DESTORY_TEAM_REQ:                  // 销毁队伍
        RecvDestoryTeamReq(playerID, sessionID, data);
        break;
    case MSG_TEAM_MEMBER_DEL_REQ:               // 使某人离开队伍
        RecvTeamMemberDelReq(playerID, sessionID, data);
        break;
    case MSG_TEAM_QUICK_JOIN_REQ:               // 快速加入队伍
        RecvTeamQuickJoinReq(playerID, sessionID, data);
        break;
    case MSG_TEAM_MEMBER_UPDATE_INFO:               // 修改队伍信息
        RecvUpdateTeamMemberInfo(playerID, sessionID, data);
        break;
    default:
        CnAssert(false);
        break;
    }
}

TEAM_MEMBER TeamManager::MakeTeamMember(Player* player)
{
    TEAM_MEMBER team_member;

    // 队伍成员信息
    team_member.player_id_h = U64ID_HIGH(player->GetPlayerID());
    team_member.player_id_l = U64ID_LOW(player->GetPlayerID());
    team_member.is_offline = false;
    team_member.name = player->GetName();
    team_member.sex = player->GetSex();
    team_member.career = player->GetCareer();
    team_member.level = player->GetLevel();
    team_member.battle_power = 0; //player->GetBattlePower();
    team_member.unionmap_id = player->GetUnionMapID();
    team_member.pos_x = player->GetX();
    team_member.pos_y = player->GetY();
    return team_member;
}

TEAM_MEMBER TeamManager::MakeTeamMember(uint64 offlinePlayerID, const std::string& offlinePlayerName)
{
    TEAM_MEMBER team_member;

    // 离线成员信息
    team_member.player_id_h = U64ID_HIGH(offlinePlayerID);
    team_member.player_id_l = U64ID_LOW(offlinePlayerID);
    team_member.is_offline = true;
    team_member.name = offlinePlayerName;
    team_member.sex = 0;
    team_member.career = 0;
    team_member.level = 0;
    team_member.battle_power = 0;
    team_member.unionmap_id = 0;
    team_member.pos_x = 0;
    team_member.pos_y = 0;

    return team_member;
}

void TeamManager::FillTeamInfo(const TEAM_INFO* inInfo, TEAM_INFO& outInfo, uint8 opt_flag)
{
    outInfo.opt_flag = opt_flag;
    outInfo.team_name = inInfo->team_name;
    outInfo.team_id_l = inInfo->team_id_l;
    outInfo.team_id_h = inInfo->team_id_h;
    outInfo.team_flag = inInfo->team_flag;
    outInfo.max_member_num = inInfo->max_member_num;
}

TEAM_INFO* TeamManager::GetTeamInfo(uint64 teamID)
{
    TeamMap::iterator it = m_teamMap.find(teamID);
    if(it == m_teamMap.end())
        return NULL;
    else
        return &(it->second);
}

void TeamManager::SetTeamGameServerID(uint64 teamID, uint32 GameServerID)
{
    TEAM_INFO* teamInfo = GetTeamInfo(teamID);
    if(teamInfo)
    {
        teamInfo->game_server_id = GameServerID;
    }
    else
    {
        CnAssert(false);
    }
}

void TeamManager::SendCreateTeamAck(uint16 errCode, uint64 playerID,uint32 serverID)
{
    ERROR_CODE sendMsg;
    sendMsg.errcode = errCode;
    GameHandler::Instance()->SendByID(serverID, MSG_CREATE_TEAM_ACK, &sendMsg, playerID);
}

// 发送销毁队伍应答
void TeamManager::SendDestoryTeamAck(uint16 errCode, Player* player)
{
    if(!player) 
        return;
    ERROR_CODE sendMsg;
    sendMsg.errcode = errCode;
    player->SendToGate(MSG_DESTORY_TEAM_ACK, &sendMsg);
}

// 发送删除组队成员消息
void TeamManager::SendTeamMemberDelAck(Player* player, uint64 teamID, const std::string& src_name, const std::string& dst_name, const std::string& leader_name)
{
    TEAM_MEMBER_DEL_ACK  sendMsg;

    sendMsg.src_name = src_name;
    sendMsg.dst_name = dst_name;
    sendMsg.leader_name = leader_name;

    if(player)
    {
        player->SendToGate(MSG_TEAM_MEMBER_DEL_ACK, &sendMsg);
    }

    SendToAllTeamMember(teamID, MSG_TEAM_MEMBER_DEL_ACK, &sendMsg);
}

// 通知队伍信息
void TeamManager::SendTeamUpdateInfoToGameServer(TEAM_INFO* pTeam,TEAM_MEMBER* pMember)
{
    if(!pTeam) 
        return;

    TEAM_INFO sendMsg;
    FillTeamInfo(pTeam, sendMsg, OPT_UPDATE_FLAG);          // 队伍信息

    if (pMember)
    {
        sendMsg.member_list.push_back(*pMember);
    }
    else
    {
        for (uint32 i = 0; i < pTeam->member_list.size(); ++i)
        {
            sendMsg.member_list.push_back(pTeam->member_list[i]);
        }
    }
    

    GameHandler::Instance()->SendToAll(MSG_TEAM_INFO_NTF, &sendMsg);
}

// 通知队伍信息
void TeamManager::SendTeamUpdateInfoToMember(TEAM_INFO* pTeam,TEAM_MEMBER* pMember)
{
    if(!pTeam) 
        return;

    uint64 teamID = U64ID(pTeam->team_id_h, pTeam->team_id_l);

    // 队伍信息
    TEAM_INFO sendMsg;
    FillTeamInfo(pTeam, sendMsg, OPT_UPDATE_FLAG);

    if (pMember)
    {
        sendMsg.member_list.push_back(*pMember);
    }
    else
    {
        for (uint32 i = 0; i < pTeam->member_list.size(); ++i)
        {
            sendMsg.member_list.push_back(pTeam->member_list[i]);
        }
    }

    SendToAllTeamMember(teamID, MSG_TEAM_INFO_NTF, &sendMsg);
}

void TeamManager::NotifyPlayerDelTeam(Player* player)
{
    if(!player) return;

    TEAM_INFO team_info;
    team_info.opt_flag = OPT_INIT_FLAG;

    player->SendToGate(MSG_TEAM_INFO_NTF, &team_info);
}

// 通知空的队伍信息
void TeamManager::NotifyGameSvrEmptyTeamInfo( uint64 teamID )
{
    if(!teamID) 
        return;

    TEAM_INFO team_info;
    team_info.opt_flag = OPT_INIT_FLAG;
    team_info.team_id_h = U64ID_HIGH(teamID);
    team_info.team_id_l = U64ID_LOW(teamID);

    GameHandler::Instance()->SendToAll(MSG_TEAM_INFO_NTF, &team_info);
}

// 通知游戏服务器队伍信息
void TeamManager::NotifyGameServerTeamInfo(TEAM_INFO* teamInfo)
{
    if(!teamInfo)
        return;
   
    GameHandler::Instance()->SendToAll(MSG_TEAM_INFO_NTF, teamInfo);
}

void TeamManager::NotifyTeamMemberTeamInfo(TEAM_INFO* pTeam, Player* player)
{
    if(!pTeam || !player) 
        return;
    player->SendToGate(MSG_TEAM_INFO_NTF, pTeam);
}

TEAM_MEMBER* TeamManager::GetMemberInfo(TEAM_INFO* pTeam, uint64 playerID)
{
    if (!pTeam)
    {
        return NULL;
    }
    for (size_t i = 0; i < pTeam->member_list.size(); ++i)
    {
        TEAM_MEMBER* pMember = &pTeam->member_list[i];
        
        if (pMember->player_id_h == U64ID_HIGH(playerID) && pMember->player_id_l == U64ID_LOW(playerID))
        {
            return pMember;
        }
    }

    return NULL;
}

void TeamManager::UpdateMemberInfo(TEAM_MEMBER* team_member, TEAM_MEMBER* src_team_member)
{
    team_member->player_id_h = src_team_member->player_id_h;
    team_member->player_id_l = src_team_member->player_id_l;
    team_member->is_offline = src_team_member->is_offline;
    team_member->name = src_team_member->name;
    team_member->sex = src_team_member->sex;
    team_member->career = src_team_member->career;
    team_member->level = src_team_member->level;
    team_member->battle_power = src_team_member->battle_power;
    team_member->unionmap_id = src_team_member->unionmap_id;
    team_member->pos_x = src_team_member->pos_x;
    team_member->pos_y = src_team_member->pos_y;
    team_member->server_id = src_team_member->server_id;
    team_member->head_icon = src_team_member->head_icon;
}

// 离开队伍
void TeamManager::LeaveTeam(uint64 playerID)
{
    TEAM_MEMBER_DEL_REQ sendMsg;
    sendMsg.player_id_h = U64ID_HIGH(playerID);
    sendMsg.player_id_l = U64ID_LOW(playerID);
    TeamManager::Instance()->RecvTeamMemberDelReq(playerID, 0, &sendMsg);
}

void TeamManager::OnPlayerChangeDungeon(Player* player)
{
    // ...
}

void TeamManager::RecvCreateTeamReq(uint64 playerID, uint32 sessionID, void* data)
{
    TEAM_CREATE_REQ* recvMsg = (TEAM_CREATE_REQ*)data;
    if (!recvMsg)
        return;

    CreateTeam(recvMsg->leader, TEAM_TYPE_WORLD, recvMsg->team_name);
}

// 销毁队伍请求
void TeamManager::RecvDestoryTeamReq(uint64 playerID, uint32 sessionID, void* data)
{
    Player* player = NULL;
    uint64 team_id = 0;

    // 获得角色
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player || (player->GetState() != Player::kStateInGame))
    {
        return;
    }

    // 获取队伍ID
    team_id = player->GetTeamID();

    // 如果不存在队伍则返回
    if(team_id == 0)
    {
        return;
    }

    // 判断是否为队长
    if(!IsTeamLeader(team_id, player->GetPlayerID()))
    {
        return;
    }

    if(DestoryTeam(player))
    {
        // 通知玩家销毁队伍成功
        SendDestoryTeamAck(ERR_SUCCEED, player);
    }
}


void TeamManager::RecvTeamMemberDelReq(uint64 playerID, uint32 sessionID, void* data)
{
    TEAM_MEMBER_DEL_REQ* recvMsg = (TEAM_MEMBER_DEL_REQ*) data;
    if(!recvMsg)
        return;

    // 获得角色
    Player* src_player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!src_player)
        return;

    // 获得队伍ID
    uint64 team_id = src_player->GetTeamID();
    if(!team_id)
        return;

    // 检查是否存在此队伍
    TEAM_INFO* pTeam = GetTeamInfo(team_id);
    if(!pTeam)
        return;

    if(!HasMember(team_id, recvMsg->player_id_h, recvMsg->player_id_l))
        return;

    // 获得要删除的角色
    uint64 del_player_id = U64ID(recvMsg->player_id_h, recvMsg->player_id_l);
    Player* del_player = PlayerManager::Instance()->GetPlayerByPlayerID(del_player_id);

    // 只有队长可以踢其他人
    if(!IsTeamLeader(team_id, playerID))
    {
        if(playerID != del_player_id)
            return;
    }

    // 设置角色队伍ID
    if(del_player)
        del_player->SetTeamID(0);

    // 获得队长名字
    const TEAM_MEMBER* leader = GetTeamLeader(pTeam);
    std::string leader_name = "";
    if(leader)
    {
        leader_name = leader->name;
    }

    const TEAM_MEMBER* src_Player = GetMemberInfo(pTeam, del_player_id);
    std::string src_name = "";
    if(src_Player)
    {
        src_name = src_Player->name;
    }

    const TEAM_MEMBER* delPlayer = GetMemberInfo(pTeam, del_player_id);
    uint32 del_player_serverID = delPlayer ? delPlayer->server_id:0;

    // 删除队伍成员
    std::string del_name = DelTeamMember(team_id, del_player_id);

    bool isDestoryTeam = false;
    if(HasOnlinePlayer(pTeam))
    {
        // 通知GameServer删除队伍成员
        NotifyGameSvrDelTeamMember(team_id, del_player_id);

        // 通知队伍成员删除队伍成员
        NotifyTeamMemberDelMember(team_id, del_player_id);
    }
    else
    {
        DestoryTeamByID(team_id);        // 队伍中无在线玩家，则销毁此队伍
        isDestoryTeam = true;
    }

    // 通知队伍ID
    if(del_player)
    {
        // 通知客户端队伍列表空
        NotifyPlayerDelTeam(del_player);
    }

    SendTeamMemberDelAck(del_player, team_id, src_name, del_name, leader_name);

    // 如果队伍只有一个人,则解散
    //if(!isDestoryTeam)
    //    DestoryTeamIfOnlyOneByID(team_id);
}

// 快速加入队伍请求
void TeamManager::RecvTeamQuickJoinReq(uint64 playerID, uint32 sessionID, void* data)
{
    TEAM_JOIN* recvmsg = (TEAM_JOIN*)data;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return;

    // 检查是否已拥有队伍
    uint64 teamid = player->GetTeamID();
    if(teamid)
    {
        player->NotifyErrorCode(ERR_I_HAS_TEAM);
        return;
    }

    // 查找可以匹配的列表
    TeamMap::iterator teamit;
    for(teamit = m_teamMap.begin(); teamit != m_teamMap.end(); ++teamit)
    {
        const TEAM_INFO& teamInfo = teamit->second;

        // 排除不可见的列表和不相同的队伍类型
        if((teamInfo.team_flag & TEAM_FLAG_DISPLAY) || (teamInfo.team_flag & TEAM_FLAG_NOENTER) )
        {
            continue;
        }

        // 检测队伍人数
        if(teamInfo.member_list.size() >= GetMaxTeamMenber(recvmsg->errcode))
        {
            continue;
        }

        if(recvmsg->team_id_h == 0 && recvmsg->team_id_l == 0)
        {   // 加入任意队伍

        }
        else
        {   // 加入指定队伍
            if(recvmsg->team_id_h != teamInfo.team_id_h || recvmsg->team_id_l != teamInfo.team_id_l)
                continue;
        }

        break;
    }

    // 未找到可以快速匹配的队伍
    if(teamit == m_teamMap.end())
    {
        player->NotifyErrorCode(ERR_NO_TEAM_JOIN);
        return;
    }
    
    recvmsg->errcode = ERR_SUCCEED;
    recvmsg->team_id_l = teamit->second.team_id_l;
    recvmsg->team_id_h = teamit->second.team_id_h;

    RecvTeamMemberJoinAns(playerID, sessionID, recvmsg);
}

// 生成队伍ID
uint64 TeamManager::GenTeamID()
{
    static uint16 serialNum = 0;
    // 时间戳 32bit, 序列号16bit, 
    uint32 high = (uint32)time(0);
    uint32 low = (++serialNum << 16);
    low |= ConfigLoader::Instance()->GetCenterID() & 0xFFFF;   // CenterID

    uint64 sn = high;
    sn = sn << 32;
    sn += low;

    return sn;
}

bool TeamManager::CreateTeam(const TEAM_MEMBER &leader, uint32 teamType, const std::string& teamName)
{
    uint64 playerID = U64ID(leader.player_id_h, leader.player_id_l);

    // 获得角色
    Player* src_player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!src_player)
        return false;

    // 获得队伍ID
    uint64 teamID = src_player->GetTeamID();
    if(teamID)
        return false;

    // 创建队伍ID
    teamID = GenTeamID();

    // 检查是否有重复的队伍ID
    TeamMap::iterator teamIt = m_teamMap.find(teamID);
    if(teamIt != m_teamMap.end())
        return false;

    if(teamName.size() < 1)
        return false;

    CnAssert(teamType == TEAM_TYPE_WORLD);          //暂时只支持WORLD类型
    // 队伍信息
    TEAM_INFO team_info;
    team_info.Clear();
    team_info.team_name = teamName;
    team_info.team_id_h = U64ID_HIGH(teamID);
    team_info.team_id_l = U64ID_LOW(teamID);
    team_info.opt_flag = OPT_INIT_FLAG;
    team_info.team_type = teamType;
    team_info.game_server_id = CalGameIDByRegionID(src_player->GetCentGameID());
    team_info.max_member_num = GetMaxTeamMenber(teamType);

    // 添加队伍成员
    team_info.member_list.push_back(leader);
    m_teamMap[teamID] = team_info;

    src_player->SetTeamID(teamID);

    // 通知GS该队伍信息
    NotifyGameServerTeamInfo(&m_teamMap[teamID]);

    // 通知该玩家队伍信息
    NotifyTeamMemberTeamInfo(&m_teamMap[teamID], src_player);

    // 通知创建队伍成功
    SendCreateTeamAck(ERR_SUCCEED, playerID, leader.server_id);

    return true;
}

// 销毁只有一个人的队伍
void TeamManager::DestoryTeamIfOnlyOne(Player* player)
{
    if(!player)
        return;

    uint64 teamID = teamID = player->GetTeamID();
    if(!teamID) 
        return;

    CnAssert(HasMember(teamID, player));

    DestoryTeamIfOnlyOneByID(teamID);
}
// 销毁只有一个人的队伍
void TeamManager::DestoryTeamIfOnlyOneByID(uint64 teamID)
{
    TEAM_INFO* teamInfo = GetTeamInfo(teamID);
    if(!teamInfo)
    {
        CnAssert(false);
        return;
    }

    if(teamInfo->member_list.size() > 1)
        return;

    DestoryTeamByID(teamID);        // 销毁队伍
}

// 销毁队伍
bool TeamManager::DestoryTeam(Player* player)
{
    if(!player)
        return false;

    uint64 teamID = teamID = player->GetTeamID();
    if(!teamID) 
        return false;

    // 获得队伍ID
    teamID = player->GetTeamID();

    // 销毁队伍
    return DestoryTeamByID(teamID);
}

bool TeamManager::DestoryTeamByID(uint64 teamID)
{
    TeamMap::iterator teamIt;

    // 查找队伍是否存在
    teamIt = m_teamMap.find(teamID);
    if(teamIt == m_teamMap.end())
    {
        CnAssert(false);
        return false;
    }

    // 通知游戏服务器队伍解散
    NotifyGameSvrEmptyTeamInfo(teamID);

    // 通知游戏成员队伍解散
    for(std::vector<TEAM_MEMBER>::iterator it = teamIt->second.member_list.begin(); it != teamIt->second.member_list.end(); ++it)
    {
        uint64 member_id = U64ID(it->player_id_h, it->player_id_l);
        Player* pMember = PlayerManager::Instance()->GetPlayerByPlayerID(member_id);
        if(pMember)
        {
            pMember->SetTeamID(0);
            NotifyPlayerDelTeam(pMember);
        }
    }

    // 删除队伍
    m_teamMap.erase(teamIt);

    return true;
}

bool TeamManager::HasMember(uint64 teamID, Player* player)
{
    uint64 playerID = player->GetPlayerID();
    return HasMember(teamID, U64ID_HIGH(playerID), U64ID_LOW(playerID));
}

bool TeamManager::HasMember(uint64 teamID, uint32 playerID_h, uint32 playerID_l)
{
    TEAM_INFO* pTeam = GetTeamInfo(teamID);

    if(!pTeam)
        return false;

    for(std::vector<TEAM_MEMBER>::iterator it = pTeam->member_list.begin(); it != pTeam->member_list.end(); ++it)
    {
        if(it->player_id_h == playerID_h && it->player_id_l == playerID_l)
            return true;
    }

    return false;
}

bool TeamManager::IsTeamLeader(uint64 teamID, uint64 playerID)
{
    TEAM_INFO* pTeamInfo = GetTeamInfo(teamID);
    if(!pTeamInfo)
        return false;

    return  GetTeamLeaderID(pTeamInfo) == playerID;
}

TEAM_MEMBER* TeamManager::GetTeamLeader(TEAM_INFO* pTeam)
{
    if(!pTeam)
    {
        CnAssert(false);
        return NULL;
    }

    if(pTeam->member_list.size() < 1)
    {
        CnAssert(false);
        return NULL;
    }

    // 第一个非离线玩家是队长
    uint64 leader_id = 0;
    for(std::vector<TEAM_MEMBER>::iterator memberIt = pTeam->member_list.begin(); memberIt != pTeam->member_list.end(); memberIt++)
    {
        if(memberIt->is_offline)
            continue;
        return &(*memberIt);
    }

    return NULL;
}

uint64 TeamManager::GetTeamLeaderID(TEAM_INFO* pTeam)
{
    TEAM_MEMBER* leaderMember = GetTeamLeader(pTeam);
    if(!leaderMember)
    {
        return 0;
    }
    return U64ID(leaderMember->player_id_h, leaderMember->player_id_l);
}

void TeamManager::SetMemberProp(uint64 playerID, uint32 dungeonID)
{
    // 获得角色
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
        return;

    // 获取队伍ID
    uint64 team_id = player->GetTeamID();
    if(team_id == 0)                // 如果不存在队伍则返回
        return;

    TEAM_INFO* team_info = GetTeamInfo(team_id);
    if(!team_info)
        return;

    TEAM_MEMBER* member_data = NULL;
    for(std::vector<TEAM_MEMBER>::iterator iter = team_info->member_list.begin(); iter != team_info->member_list.end(); iter++)
    {
        uint64 memberID = U64ID(iter->player_id_h, iter->player_id_l);
        if(memberID == player->GetPlayerID())
        {   // 更新成员信息
            iter->level = player->GetLevel();
            iter->battle_power = 0; //player->GetBattlePower();
            iter->unionmap_id = dungeonID;
            iter->pos_x = player->GetX();
            iter->pos_y = player->GetY();
            member_data = &(*iter);
            break;
        }
    }

    if(!member_data)
        return;

    TEAM_INFO sendMsg;
    FillTeamInfo(team_info, sendMsg, OPT_UPDATE_FLAG);
    sendMsg.member_list.push_back(*member_data);

    SendToAllTeamMember(team_info, MSG_TEAM_INFO_NTF, &sendMsg);
    GameHandler::Instance()->SendToAll(MSG_TEAM_INFO_NTF, &sendMsg);
}

bool TeamManager::HasOnlinePlayer(TEAM_INFO* pTeam)
{
    if(!pTeam)
    {
        CnAssert(false);
        return false;
    }

    uint32 onlinePlayerNum = 0;
    for(std::vector<TEAM_MEMBER>::iterator memberIt = pTeam->member_list.begin(); memberIt != pTeam->member_list.end(); memberIt++)
    {
        if(memberIt->is_offline)
            continue;

        onlinePlayerNum++;
    }

    return onlinePlayerNum > 0;
}

void TeamManager::AddTeamMember(TEAM_INFO* pTeam, const TEAM_MEMBER& team_member)
{
    uint64 team_id = U64ID(pTeam->team_id_h, pTeam->team_id_l);
    uint64 playerID = U64ID(team_member.player_id_h, team_member.player_id_l);

    if(!pTeam)
        return;

    // 队伍信息
    pTeam->member_list.push_back(team_member);

    Player* pPlayer = PlayerManager::Instance()->GetPlayerByPlayerID(U64ID(team_member.player_id_h, team_member.player_id_l));
    if(pPlayer)
    {   // 在线玩家
        CnAssert(!team_member.is_offline);
        team_id = U64ID(pTeam->team_id_h, pTeam->team_id_l);

        // 设置角色队伍ID
        CnAssert(pPlayer->GetTeamID() == 0);
        pPlayer->SetTeamID(team_id);

        NotifyTeamMemberTeamInfo(pTeam, pPlayer);               // 通知玩家队伍信息
    }
    else
    {   // 离线玩家
        CnAssert(team_member.is_offline);
    }

    // 通知GameSver添加了队员
    NotifyGameSvrAddTeamMember(pTeam, team_member);

    // 通知其他成员添加了队员
    NotifyTeamMemberAddMember(pTeam, team_member);
}

std::string TeamManager::DelTeamMember(uint64 teamID, uint64 playerID)
{
    TEAM_INFO* pTeam = 0;
    uint32 player_id_h = 0;
    uint32 player_id_l = 0;
    std::string del_name = "";

    pTeam = GetTeamInfo(teamID);
    if(!pTeam)
    {
        return del_name;
    }

    bool isExist = false;

    player_id_h = U64ID_HIGH(playerID);
    player_id_l = U64ID_LOW(playerID);
    for(std::vector<TEAM_MEMBER>::iterator it = pTeam->member_list.begin(); it != pTeam->member_list.end(); ++it)
    {
        if((it->player_id_h == player_id_h) && (it->player_id_l == player_id_l))
        {
            del_name = it->name;
            pTeam->member_list.erase(it);
            isExist = true;
            break;
        }
    }

    CnAssert(isExist);      // 删除的角色不存在
    return del_name;
}

void TeamManager::NotifyGameSvrAddTeamMember(TEAM_INFO* pTeam, const TEAM_MEMBER& team_member)
{
    TEAM_INFO team_info;

    if(!pTeam)
        return;

    team_info.Clear();

    // 队伍信息
    team_info.team_id_h = pTeam->team_id_h;
    team_info.team_id_l = pTeam->team_id_l;
    team_info.opt_flag = OPT_ADD_FLAG;

    team_info.member_list.push_back(team_member);

    GameHandler::Instance()->SendToAll(MSG_TEAM_INFO_NTF, &team_info);
}

void TeamManager::NotifyTeamMemberAddMember(TEAM_INFO* pTeam, const TEAM_MEMBER& team_member)
{
    if(!pTeam)
        return;

    // 队伍信息
    TEAM_INFO team_info;
    FillTeamInfo(pTeam, team_info, OPT_ADD_FLAG);
    team_info.member_list.push_back(team_member);

    uint64  member_playerID = U64ID(team_member.player_id_h, team_member.player_id_l);

    SendToAllTeamMember(pTeam, MSG_TEAM_INFO_NTF, &team_info, member_playerID);
}

void TeamManager::NotifyGameSvrDelTeamMember(uint64 teamID, uint64 playerID)
{
    TEAM_INFO team_info;
    TEAM_MEMBER team_member;

    // 队伍信息
    team_info.team_id_h = U64ID_HIGH(teamID);
    team_info.team_id_l = U64ID_LOW(teamID);
    team_info.opt_flag = OPT_DEL_FLAG;

    TEAM_INFO* teamInfo = GetTeamInfo(teamID);
    team_info.team_name = teamInfo->team_name;

    // 队伍成员信息
    team_member.Clear();
    team_member.player_id_h = U64ID_HIGH(playerID);
    team_member.player_id_l = U64ID_LOW(playerID);

    team_info.member_list.push_back(team_member);

    GameHandler::Instance()->SendToAll(MSG_TEAM_INFO_NTF, &team_info);
}

void TeamManager::NotifyTeamMemberDelMember(uint64 teamID, uint64 playerID)
{
    // 队伍信息
    TEAM_INFO team_info;
    team_info.Clear();
    team_info.team_id_h = U64ID_HIGH(teamID);
    team_info.team_id_l = U64ID_LOW(teamID);
    team_info.opt_flag = OPT_DEL_FLAG;

    TEAM_INFO* teamInfo = GetTeamInfo(teamID);
    team_info.team_name = teamInfo->team_name;

    // 队伍成员信息
    TEAM_MEMBER team_member;
    team_member.Clear();
    team_member.player_id_h = U64ID_HIGH(playerID);
    team_member.player_id_l = U64ID_LOW(playerID);

    team_info.member_list.push_back(team_member);

    SendToAllTeamMember(teamID, MSG_TEAM_INFO_NTF, &team_info, playerID);
}

// 通知队伍成员
void TeamManager::SendToAllTeamMember(TEAM_INFO* pTeam, uint32 msgID, void* data, uint64 exceptID)
{
    if(!pTeam) return;

    for(std::vector<TEAM_MEMBER>::iterator it = pTeam->member_list.begin(); it != pTeam->member_list.end(); ++it)
    {
        uint64 memberID = U64ID(it->player_id_h, it->player_id_l);
        if(memberID == exceptID) 
            continue;

        GameHandler::Instance()->SendByID(it->server_id,msgID,data,memberID);
    }
}

// 发送消息队伍成员
void TeamManager::SendToAllTeamMember(uint64 teamID, uint32 msgID, void* data, uint64 exceptID)
{
    SendToAllTeamMember(GetTeamInfo(teamID), msgID, data, exceptID);
}

// 通知游戏服务器所有的队伍信息
void TeamManager::SendAllTeamInfoToGameServer(uint32 sessionID)
{
    uint32 regionID = GameHandler::Instance()->GetRegionIDBySessionID(sessionID);

    for (TeamMap::iterator teamIt = m_teamMap.begin(); teamIt != m_teamMap.end(); ++teamIt)
    {
        GameHandler::Instance()->SendByID(regionID, MSG_TEAM_INFO_NTF, &teamIt->second);
    }
}

// 队伍信息合法性检测(调试用)
void TeamManager::ValidCheck()
{
    for(TeamMap::iterator iter = m_teamMap.begin(); iter != m_teamMap.end(); iter++)
    {
        TEAM_INFO& teamInfo = iter->second;
        CnAssert(teamInfo.member_list.size() > 0);

        bool hasOnlinePlayer = false;

        for(std::vector<TEAM_MEMBER>::iterator iter2 = teamInfo.member_list.begin(); iter2 != teamInfo.member_list.end(); iter2++)
        {
            TEAM_MEMBER& teamMenber = *iter2;
            if(teamMenber.is_offline)
                continue;       // 离线玩家

            hasOnlinePlayer = true;

            uint64 playerID = U64ID(teamMenber.player_id_h, teamMenber.player_id_l);
            Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
            CnAssert(player);       // 队伍中的玩家必须存在

            CnAssert(U64ID(teamInfo.team_id_h, teamInfo.team_id_l) == player->GetTeamID());
        }
    }
}

void TeamManager::RecvTeamMemberJoinAns(uint64 playerID, uint32 sessionID, void* data)
{
    TEAM_JOIN* recvMsg = (TEAM_JOIN*)data;

    if(!recvMsg)
        return;

    TEAM_MEMBER* pLeader = NULL;

    if(recvMsg->errcode != ERR_SUCCEED)
    {   // 通知申请者,被队长拒绝加入队伍
        uint64 proposerID = U64ID(recvMsg->player.player_id_h, recvMsg->player.player_id_l);
        GameHandler::Instance()->SendByID(recvMsg->player.server_id, MSG_TEAM_QUICK_JOIN_ACK, recvMsg, proposerID);
        return;
    }

    uint64 teamID = U64ID(recvMsg->team_id_h,recvMsg->team_id_l);

    TEAM_INFO* pTeam = GetTeamInfo(teamID);
    if (!pTeam)
    {
        recvMsg->errcode = ERR_NO_TEAM;
        goto _ERROR;
    }

    pLeader = GetTeamLeader(pTeam);
    if (!pLeader)
    {
        recvMsg->errcode = ERR_NO_TEAM;
        goto _ERROR;
    }

    // 检查队伍状态
    if((pTeam->team_flag & TEAM_FLAG_DISPLAY) || (pTeam->team_flag & TEAM_FLAG_NOENTER))
    {
        recvMsg->errcode = ERR_NO_TEAM;
        goto _ERROR;
    }

    // 检测队伍人数
    if(pTeam->member_list.size() >= GetMaxTeamMenber(TEAM_TYPE_WORLD))
    {
        recvMsg->errcode = ERR_MAX_TEAM_MEMBER;
        goto _ERROR;
    }


    // 添加队伍成员
    AddTeamMember(pTeam, recvMsg->player);

    return;
_ERROR:
    uint64 leaderID = U64ID(pLeader->player_id_h, pLeader->player_id_l);
    GameHandler::Instance()->SendByID(pLeader->server_id, MSG_TEAM_QUICK_JOIN_ACK, recvMsg, leaderID);
}

void TeamManager::RecvUpdateTeamMemberInfo(uint64 playerID, uint32 sessionID, void* data)
{
    TEAM_MEMBER* recvMsg = (TEAM_MEMBER*)data;

    if(!recvMsg)
        return;

    // 获得角色
    uint64 update_player_id = U64ID(recvMsg->player_id_h, recvMsg->player_id_l);
    Player* src_player = PlayerManager::Instance()->GetPlayerByPlayerID(update_player_id);
    if(!src_player)
        return;

    // 获得队伍ID
    uint64 team_id = src_player->GetTeamID();
    if(!team_id)
        return;

    // 检查是否存在此队伍
    TEAM_INFO* pTeam = GetTeamInfo(team_id);
    if(!pTeam)
        return;

    if(!HasMember(team_id, recvMsg->player_id_h, recvMsg->player_id_l))
        return;

    TEAM_MEMBER* src_Player = GetMemberInfo(pTeam, update_player_id);
    UpdateMemberInfo(src_Player, recvMsg);

    TEAM_INFO sendMsg;
    FillTeamInfo(pTeam, sendMsg, OPT_UPDATE_FLAG);
    sendMsg.member_list.push_back(*src_Player);

    SendToAllTeamMember(pTeam, MSG_TEAM_INFO_NTF, &sendMsg);

    // 通知更新
    SendTeamUpdateInfoToGameServer(pTeam,recvMsg);
}
