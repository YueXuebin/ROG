#include "gameholder_pch.h"
#include "cross_team_manager.h"
#include "dungeon_manager.h"
#include "player.h"
#include "center_handler.h"
#include "server_config.h"
#include "gate_handler.h"
#include "vip_config.h"
#include "trade_manager.h"


IMPLEMENT_SINGLETON(CrossTeamManager)

CrossTeamManager::CrossTeamManager()
{

}

CrossTeamManager::~CrossTeamManager()
{

}

void CrossTeamManager::OnRecvFromPlayer(Player* player, uint32 msgID, void* data)
{
    uint32 gateServerID = player->GetGateSvrID();
    // 对部分消息添加内容
    switch(msgID)
    {
    case MSG_CROSS_TEAM_SHOW_LIST_REQ:      // 组队列表请求
        {
            CROSS_TEAM_SHOW_LIST_REQ* req = (CROSS_TEAM_SHOW_LIST_REQ*)data;
            req->gatesvr_id = gateServerID;
        }
        break;
    case MSG_CROSS_TEAM_INFO_REQ:           // 队伍信息请求
        {
            CROSS_TEAM_INFO_REQ* req = (CROSS_TEAM_INFO_REQ*)data;
            req->gatesvr_id = gateServerID;
        }
        break;
    case MSG_CROSS_CREATE_TEAM_REQ:         // 创建队伍
        {
            CROSS_TEAM_CREATE_REQ* req = (CROSS_TEAM_CREATE_REQ*)data;
            req->gatesvr_id = gateServerID;
            req->leader = MakeTeamMember(player);
        }
        break;
    case MSG_CROSS_DESTORY_TEAM_REQ:        // 解散队伍
        {
            CROSS_TEAM_INFO_REQ* req = (CROSS_TEAM_INFO_REQ*)data;
            req->gatesvr_id = gateServerID;
        }
        break;
    case MSG_CROSS_TEAM_MEMBER_ADD_ANS:     // 被邀请应答
        {
            CROSS_TEAM_MEMBER_ADD_ACK* req = (CROSS_TEAM_MEMBER_ADD_ACK*)data;
            req->src_gate_id = gateServerID;
            req->member = MakeTeamMember(player);
        }
        break;
    case MSG_CROSS_TEAM_QUICK_JOIN_REQ:     // 快速加入
        {
            CROSS_TEAM_QUICK_JOIN_REQ* req = (CROSS_TEAM_QUICK_JOIN_REQ*)data;
            req->gatesvr_id = gateServerID;
            req->player_level = player->GetLevel();
            req->player_name = player->GetName();
            req->member = MakeTeamMember(player);
        }
        break;
    case MSG_CROSS_TEAM_MEMBER_READY_REQ:   // 准备请求
        {
            CROSS_TEAM_MEMBER_READY_NTF* req = (CROSS_TEAM_MEMBER_READY_NTF*)data;
            req->gatesvr_id = gateServerID;
            req->team_type = TEAM_TYPE_DUNGEON;
        }
        break;
    case MSG_CROSS_TEAM_DUNGEON_SCORE_INFO_REQ: // 通关记录请求
        {
            CROSS_TEAM_DUNGEON_SCORE_INFO_REQ* req = (CROSS_TEAM_DUNGEON_SCORE_INFO_REQ*)data;
            req->gatesvr_id = gateServerID;
            req->player_id_h = U64ID_HIGH(player->GetPlayerID());
            req->player_id_l = U64ID_LOW(player->GetPlayerID());
        }
        break;
    }

    // 需要在GameServer上处理的协议
    if(msgID == MSG_CROSS_TEAM_MEMBER_READY_REQ)
    {
        if(!RecvTeamMemberReadyReq(player, data))
            return;
    }
    else if(msgID == MSG_CROSS_TEAM_BUY_DUNGEON_TIMES_REQ)
    {
        RecvTeamBuyDungeonTimes(player, data);
        return;
    }


    // 所有消息都通过跨服Game处理
    if(DungeonManager::Instance()->GetDungeonInfo(DUNGEON_CROSS_TEAM_ID))
    {
        OnRecvFromCenter(player->GetPlayerID(), msgID, data);   // 不用转发
    }
    else
    {
        CenterHandler::Instance()->SendByCenterID(ServerConfig::Instance()->GetCenterID(), msgID, data, player->GetPlayerID());     // 转发处理
    }
}

void CrossTeamManager::OnRecvFromCenter(uint64 playerID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_CROSS_TEAM_SHOW_LIST_REQ:                  // 队伍显示列表请求
        OnCrossTeamShowListReq(playerID, data);
        break;
    case MSG_CROSS_TEAM_INFO_REQ:                       // 队伍信息请求
        OnCrossTeamInfoReq(playerID, data);
        break;
    case MSG_CROSS_CREATE_TEAM_REQ:                     // 创建队伍
        OnCreateTeamReq(playerID, data);
        break;
    case MSG_CROSS_DESTORY_TEAM_REQ:                    // 解散队伍
        OnDestoryTeamReq(playerID, data);
        break;
    case MSG_CROSS_TEAM_MEMBER_DEL_REQ:                 // 使某人离队
        OnTeamMemberDelReq(playerID, data);
        break;
    case MSG_CROSS_TEAM_MEMBER_ADD_ANS:                 // 被邀请者加入队伍应答
        OnTeamMemberAddAns(playerID, data);
        break;
    case MSG_CROSS_TEAM_QUICK_JOIN_REQ:                 // 快速加入队伍请求
        OnTeamQuickJoinReq(playerID, data);
        break;
    case MSG_CROSS_TEAM_MEMBER_READY_REQ:               // 准备请求
        OnTeamMemberReadyReq(playerID, data);
        break;
    case MSG_CROSS_TEAM_FLAG_SET_REQ:                   // 设置队伍标记请求
        OnTeamFlagSetReq(playerID, data);
        break;
    case MSG_CROSS_TEAM_ENTER_DUNGEON:                  // 进入组队副本请求
        OnTeamEnterDungeonReq(playerID, data);
        break;
    case MSG_CROSS_TEAM_DUNGEON_SCORE_INFO_REQ:         // 通关记录请求
        OnDungeonScoreInfoReq(playerID, data);
        break;
    case MSG_CROSS_TEAM_DUNGEON_SCORE_INFO_ACK:         // 通关记录数据返回
        OnDungeonScoreInfoAck(playerID, data);
        break;
    default:
        CnAssert(false);
        break;
    }
}

void CrossTeamManager::OnDungeonScoreInfoAck(uint64 playerID, void* data)
{
    CROSS_TEAM_DUNGEON_SCORE_INFO_LIST* ack = (CROSS_TEAM_DUNGEON_SCORE_INFO_LIST*)data;
    GateHandler::Instance()->SendByGateID(ack->gatesvr_id, MSG_CROSS_TEAM_DUNGEON_SCORE_INFO_ACK, ack, U64ID(ack->player_id_h, ack->player_id_l));
}

void CrossTeamManager::OnDungeonScoreInfoReq(uint64 playerID, void* data)
{
    CROSS_TEAM_DUNGEON_SCORE_INFO_REQ* req = (CROSS_TEAM_DUNGEON_SCORE_INFO_REQ*)data;
    //CenterHandler::Instance()->SendToMasterCenter(MSG_GS_DUNGEON_SCORE_INFO_REQ, req, playerID);
}

// 队伍进入副本请求
void CrossTeamManager::OnTeamEnterDungeonReq(uint64 playerID, void* data)
{
    uint64 teamID = GetTeamID(playerID);
    if(!teamID)
        return;

    CROSS_TEAM_INFO* pTeamInfo = GetTeamInfo(teamID);
    if(!pTeamInfo)
        return;

    if(playerID != GetTeamLeader(teamID))
    {
        CnAssert(false);            // 队长才可以请求进入副本
        return;
    }

    CROSS_TEAM_MEMBER* leaderMember = GetTeamLeaderData(teamID);

    // 检查队员状态
    for(std::vector<CROSS_TEAM_MEMBER>::iterator iter = pTeamInfo->member_list.begin(); iter != pTeamInfo->member_list.end(); iter++)
    {
        if(!iter->is_ready)
        {
            SendErrorCode(playerID, leaderMember->gatesvr_id, ERR_TEAM_MEMBER_NO_READY);    // 还有成员未准备
            return;
        }
    }

    if(pTeamInfo->channel_id != 0)
    {
        return;     // 该队伍的副本已经开启
    }

    // 检查副本是否可以进入
    uint32 errCode = DungeonManager::Instance()->RequestJumpDungeonId(pTeamInfo->dungeon_id);
    if(errCode != ERR_SUCCEED)
    {
        SendErrorCode(playerID, leaderMember->gatesvr_id, errCode);
        return;
    }

    // 设置队伍状态为不可进入
    pTeamInfo->team_flag |= TEAM_FLAG_NOENTER;

    // 重置所有成员的状态(为未准备)
    for(std::vector<CROSS_TEAM_MEMBER>::iterator iter = pTeamInfo->member_list.begin(); iter != pTeamInfo->member_list.end(); iter++)
    {
        iter->is_ready = false;

        CROSS_TEAM_MEMBER_READY_NTF sendMsg;
        sendMsg.errcode = ERR_FAILED;
        sendMsg.team_type = pTeamInfo->team_type;
        sendMsg.player_id_h = iter->player_id_h;
        sendMsg.player_id_l = iter->player_id_l;
        SendToAllTeamMember(pTeamInfo, MSG_CROSS_TEAM_MEMBER_READY_ACK, &sendMsg, 0);
    }

    // 跳转副本通知
    NotifyChangeTeamDungeon(pTeamInfo);
}

void CrossTeamManager::OnTeamFlagSetReq(uint64 playerID, void* data)
{
    TEAM_FLAG_SET_REQ* recvMsg = (TEAM_FLAG_SET_REQ*)data;

    CROSS_TEAM_INFO* pTeamInfo = GetTeamInfoByPlayer(playerID);
    if(!pTeamInfo)
        return;

    if(playerID != GetTeamLeader(U64ID(pTeamInfo->team_id_h, pTeamInfo->team_id_l)))
    {
        return;     // 只有队长可以设置队伍标记
    }

    pTeamInfo->team_flag = recvMsg->team_flag;

    SendTeamUpdateInfoToMember(pTeamInfo);
}

// 购买进入队伍副本次数请求
void CrossTeamManager::RecvTeamBuyDungeonTimes(Player* player, void* data)
{
    //int32 cost = 0;
    //int32 daily_buytimes = 0;
    //int32 refresh_times = 0;

    //if(!player)
    //    return;

    //// 获得组队购买次数的魔石花费数量
    //cost = GameParamConfig::Instance()->GetTeamDungeonBuyTimesMoneypoint();
    //if(cost < 1)
    //    return;

    //// 检查角色魔石数量
    //if(cost > player->GetMoneyPoint())
    //{
    //    player->NotifyErrorCode(ERR_NO_ENOUGH_MONEYPOINT);
    //    return;
    //}

    //// 检查购买次数限制
    //daily_buytimes = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_12 , player->GetVipLevel());
    //refresh_times = GameParamConfig::Instance()->GetTeamDungeonDailyTimes();
    //if(player->GetTeamDungeonTimes() >= (daily_buytimes + refresh_times))
    //{
    //    player->NotifyErrorCode(ERR_TEAM_DUNGEON_REACH_MAX_NUM);
    //    return;
    //}

    //if(player->GetTeamBuyTimes() >= daily_buytimes)
    //{
    //    player->NotifyErrorCode(ERR_NO_TEAM_BUY_TIMES);
    //    return;
    //}

    //TradeManager::Instance()->SendTradeOrderReq(player, REWARD_REASON_TEAM_DUNGEON_TIMES, PROP_TEAM_DUNGEON_TIMES, 0, 1, cost);
}

// 成员准备请求
bool CrossTeamManager::RecvTeamMemberReadyReq(Player* player, void* data)
{
    //CROSS_TEAM_MEMBER_READY_NTF* recvMsg = (CROSS_TEAM_MEMBER_READY_NTF*)data;

    //if(!recvMsg || !player) 
    //    return false;

    //recvMsg->errcode = ERR_SUCCEED;
    //recvMsg->player_id_h = U64ID_HIGH(player->GetPlayerID());
    //recvMsg->player_id_l = U64ID_LOW(player->GetPlayerID());
 
    //if(recvMsg->team_type == TEAM_TYPE_DUNGEON)
    //{
    //    // 检查体力
    //    int32 costVigor = GameParamConfig::Instance()->GetEnterTeamDungeonVigor();
    //    if(player->GetVigor() < costVigor)
    //    {
    //        recvMsg->errcode = ERR_NO_ENOUGH_VIGOR;
    //        player->SendToGate(MSG_CROSS_TEAM_MEMBER_READY_ACK, recvMsg);
    //        return false;
    //    }

    //    // 检查进入副本次数
    //    if(player->GetTeamDungeonTimes() < GameParamConfig::Instance()->GetTeamDungeonCostTimes())
    //    {
    //        recvMsg->errcode = ERR_NO_ENOUGH_TEAM_TIMES;
    //        player->SendToGate(MSG_CROSS_TEAM_MEMBER_READY_ACK, recvMsg);
    //        return false;
    //    }
    //}
    //else
    //{
    //    CnAssert(false);
    //    return false;
    //}

    return true;
}

void CrossTeamManager::OnTeamMemberReadyReq(uint64 playerID, void* data)
{
    CROSS_TEAM_MEMBER_READY_NTF* recvMsg = (CROSS_TEAM_MEMBER_READY_NTF*)data;
    if(!recvMsg) return;

    uint64 teamID = GetTeamID(playerID);
    if(!teamID)
    {
        return;         // 该玩家没有队伍
    }
    
    CROSS_TEAM_INFO* pTeamInfo = GetTeamInfoByPlayer(playerID);
    if(!pTeamInfo)
    {
        CnAssert(false);
        return;
    }

    // 检查队员准备状态
    bool isTeamReady = true;
    for(std::vector<CROSS_TEAM_MEMBER>::iterator iter = pTeamInfo->member_list.begin(); iter != pTeamInfo->member_list.end(); iter++)
    {
        CROSS_TEAM_MEMBER& member = *iter;
        uint64 memberID = U64ID(member.player_id_h, member.player_id_l);
        if(memberID == playerID)
        {
            member.is_ready = (recvMsg->errcode == ERR_SUCCEED)?1:0;
        }

        if(!member.is_ready)
        {
            isTeamReady = false;
        }
    }

    if(isTeamReady)
    {
        pTeamInfo->team_flag &= ~TEAM_FLAG_NOENTER;     // 队伍准备好了，就不能再进入了
    }

    // 通知队伍成员准备状态
    recvMsg->team_type = TEAM_TYPE_DUNGEON;
    recvMsg->player_id_h = U64ID_HIGH(playerID);
    recvMsg->player_id_l = U64ID_LOW(playerID);

    SendToAllTeamMember(teamID, MSG_CROSS_TEAM_MEMBER_READY_ACK, recvMsg);
}

// 快速加入队伍请求
void CrossTeamManager::OnTeamQuickJoinReq(uint64 playerID, void* data)
{
    CROSS_TEAM_QUICK_JOIN_REQ* recvMsg = (CROSS_TEAM_QUICK_JOIN_REQ*)data;

    // 检查是否已拥有队伍
    if(GetTeamID(playerID))
    {
        SendErrorCode(playerID, recvMsg->gatesvr_id, ERR_I_HAS_TEAM);
        return;
    }

    // 查找可以匹配的列表
    CrossTeamMap::iterator teamIt;
    for(teamIt = m_teamMap.begin(); teamIt != m_teamMap.end(); ++teamIt)
    {
        // 排除不可见的列表和不相同的队伍类型
        if((teamIt->second.team_flag & TEAM_FLAG_DISPLAY) || 
            (teamIt->second.team_flag & TEAM_FLAG_NOENTER) || 
            (teamIt->second.team_type != recvMsg->team_type))
        {
            continue;
        }

        // 检测队伍人数
        if(teamIt->second.member_list.size() >= GetMaxTeamMenber(recvMsg->team_type))
        {
            continue;
        }

        // 检查角色等级
        if(recvMsg->player_level < teamIt->second.team_level)
        {
            continue;
        }

        break;
    }

    // 未找到可以快速匹配的队伍
    if(teamIt == m_teamMap.end())
    {
        SendErrorCode(playerID, recvMsg->gatesvr_id, ERR_I_HAS_TEAM);
        return;
    }

    // 拼接加入组队的消息
    CROSS_TEAM_MEMBER_ADD_ACK addAck;
    addAck.errcode = ERR_SUCCEED;
    addAck.src_name = "";
    addAck.dst_name = recvMsg->player_name;
    addAck.team_id_h = teamIt->second.team_id_h;
    addAck.team_id_l = teamIt->second.team_id_l;
    addAck.src_gate_id = recvMsg->gatesvr_id;
    addAck.member = recvMsg->member;

    OnTeamMemberAddAns(playerID, &addAck);
}
// 被邀请者加入队伍应答
void CrossTeamManager::OnTeamMemberAddAns(uint64 playerID, void* data)
{
    CROSS_TEAM_MEMBER_ADD_ACK* recvMsg = (CROSS_TEAM_MEMBER_ADD_ACK*) data;

    uint64 teamID = U64ID(recvMsg->team_id_h, recvMsg->team_id_l);

    if(recvMsg->errcode != ERR_SUCCEED)
    {   // 通知邀请者,被邀请者拒绝加入队伍
        CROSS_TEAM_MEMBER* askMember = GetTeamMemberDataByName(teamID, recvMsg->src_name);
        SendTeamMemberAddAck(U64ID(askMember->player_id_h, askMember->player_id_l), askMember->gatesvr_id, recvMsg->errcode, recvMsg->src_name, recvMsg->dst_name);
        return;
    }

    // 检查被邀请者是否已经有队伍
    if(GetTeamID(playerID))
    {
        recvMsg->errcode = ERR_I_HAS_TEAM;
        SendTeamMemberAddAck(playerID, recvMsg->src_gate_id, recvMsg->errcode, recvMsg->src_name, recvMsg->dst_name);
        return;
    }

    // 获得队伍信息
    CROSS_TEAM_INFO* pTeamInfo = GetTeamInfo(teamID);
    if(!pTeamInfo)
    {
        recvMsg->errcode = ERR_NO_TEAM;
        SendTeamMemberAddAck(playerID, recvMsg->src_gate_id, recvMsg->errcode, recvMsg->src_name, recvMsg->dst_name);
        return;
    }

    // 检查队伍状态
    if((pTeamInfo->team_flag & TEAM_FLAG_DISPLAY) || (pTeamInfo->team_flag & TEAM_FLAG_NOENTER))
    {
        recvMsg->errcode = ERR_NO_TEAM;
        SendTeamMemberAddAck(playerID, recvMsg->src_gate_id, recvMsg->errcode, recvMsg->src_name, recvMsg->dst_name);
        return;
    }

    // 检测队伍人数
    if(pTeamInfo->member_list.size() >= GetMaxTeamMenber(pTeamInfo->team_type))
    {
        recvMsg->errcode = ERR_MAX_TEAM_MEMBER;
        SendTeamMemberAddAck(playerID, recvMsg->src_gate_id, recvMsg->errcode, recvMsg->src_name, recvMsg->dst_name);
        return;
    }

    // 添加队伍成员
    AddTeamMember(teamID, recvMsg->member);
}

void CrossTeamManager::OnTeamMemberDelReq(uint64 playerID, void* data)
{
    CROSS_TEAM_MEMBER_DEL_REQ* req = (CROSS_TEAM_MEMBER_DEL_REQ*)data;
    if(!req)
        return;

    uint64 teamID = GetTeamID(playerID);
    if(!teamID)
        return;

    CROSS_TEAM_INFO* pTeamInfo = GetTeamInfo(teamID);
    if(!pTeamInfo)
        return;
    
    if(!HasMember(teamID, playerID))
        return;

    // 只有队长可以踢其他人
    uint64 delPlayerID = U64ID(req->player_id_h, req->player_id_l);
    if(playerID != GetTeamLeader(teamID))
    {
        if(playerID != delPlayerID)
            return;
    }

    // 获取玩家名字
    std::string srcPlayerName;
    std::string delPlayerName;
    std::string leaderName;
    CROSS_TEAM_MEMBER* srcPlayerData = GetTeamMemberData(teamID, playerID);
    CROSS_TEAM_MEMBER* delPlayerData = GetTeamMemberData(teamID, delPlayerID);
    CROSS_TEAM_MEMBER* leaderData = GetTeamLeaderData(teamID);
    
    if(!srcPlayerData || !delPlayerData || !leaderData)
    {
        CnAssert(false);
        return;
    }
    srcPlayerName = srcPlayerData->name;
    delPlayerName = delPlayerData->name;
    leaderName = leaderData->name;
    uint32 delPlayerGateID = delPlayerData->gatesvr_id;


    RemoveMemberFromTeam(delPlayerID, teamID);

    srcPlayerData = delPlayerData = leaderData = NULL;      // 指针已经不可用

    if(HasOnlinePlayer(pTeamInfo))
    {   // 通知队伍成员删除队伍成员
        NotifyTeamMemberDelMember(teamID, delPlayerID);
    }
    else
    {   // 销毁队伍
        DestoryTeamByID(teamID);        // 队伍中无在线玩家，则销毁此队伍
    }

    // 通知被删除成员,清空队伍数据
    NotifyPlayerDelTeam(delPlayerID, delPlayerGateID);

    // 发送删除成员成功响应
    SendTeamMemberDelAck(delPlayerID, delPlayerGateID, teamID, delPlayerName, srcPlayerName, leaderName);
}

// 销毁队伍请求
void CrossTeamManager::OnDestoryTeamReq(uint64 playerID, void* data)
{
    CROSS_TEAM_INFO_REQ* req = (CROSS_TEAM_INFO_REQ*)data;
    uint64 teamID = GetTeamID(playerID);
    if(!teamID)
    {
        return;     // 队伍不存在
    }

    if(playerID != GetTeamLeader(teamID))
    {
        return;         // 只有队长可以销毁队伍
    }

    DestoryTeamByID(teamID);
}

void CrossTeamManager::OnCreateTeamReq(uint64 playerID, void* data)
{
    CROSS_TEAM_CREATE_REQ* req = (CROSS_TEAM_CREATE_REQ*)data;
    // 如果存在队伍则返回
    if(GetTeamInfoByPlayer(playerID))
    {
        SendErrorCode(playerID, req->gatesvr_id, ERR_I_HAS_TEAM);
        return;
    }

    CreateTeam(req->leader, req->dungeon_id, req->team_level);
}

bool CrossTeamManager::CreateTeam(const CROSS_TEAM_MEMBER& leader, uint32 dungeonID, uint32 teamLevel)
{
    uint64 playerID = U64ID(leader.player_id_h, leader.player_id_l);
    uint64 teamID = GetTeamID(playerID);
    if(teamID)
        return false;

    // 创建队伍ID
    teamID = GenTeamID();

    if(GetTeamInfo(teamID))
    {
        CnAssert(false);        // 检查是否有重复的队伍ID
        return false;
    }

    // 队伍信息
    CROSS_TEAM_INFO team_info;
    team_info.Clear();
    team_info.team_id_h = U64ID_HIGH(teamID);
    team_info.team_id_l = U64ID_LOW(teamID);
    team_info.team_type = TEAM_TYPE_DUNGEON;
    team_info.dungeon_id = dungeonID;
    team_info.opt_flag = OPT_INIT_FLAG;
    team_info.team_level = teamLevel;
    m_teamMap[teamID] = team_info;

    // 添加成员到队伍中
    AddMemberToTeam(leader, teamID);

    // 通知队伍信息
    NotifyTeamMemberTeamInfo(playerID);

    // 通知创建队伍成功
    ERROR_CODE errMsg;
    errMsg.errcode = ERR_SUCCEED;
    GateHandler::Instance()->SendByGateID(leader.gatesvr_id, MSG_CROSS_CREATE_TEAM_ACK, &errMsg, playerID);

    return true;
}
// 删除成员通知
void CrossTeamManager::NotifyTeamMemberDelMember(uint64 teamID, uint64 playerID)
{
    CROSS_TEAM_INFO team_info;
    CROSS_TEAM_MEMBER team_member;
    uint64  send_playerID = 0;
    Player* send_player = NULL;

    // 队伍信息
    team_info.Clear();
    team_info.team_id_h = U64ID_HIGH(teamID);
    team_info.team_id_l = U64ID_LOW(teamID);
    team_info.opt_flag = OPT_DEL_FLAG;

    // 队伍成员信息
    team_member.Clear();
    team_member.player_id_h = U64ID_HIGH(playerID);
    team_member.player_id_l = U64ID_LOW(playerID);

    team_info.member_list.push_back(team_member);

    SendToAllTeamMember(teamID, MSG_CROSS_TEAM_INFO_NTF, &team_info, playerID);
}

bool CrossTeamManager::DestoryTeamByID(uint64 teamID)
{
    CrossTeamMap::iterator tIter = m_teamMap.find(teamID);
    if(tIter == m_teamMap.end())
    {
        CnAssert(false);
        return false;
    }

    CROSS_TEAM_INFO* pTeamInfo = &tIter->second;

    // 游戏成员队伍解散
    uint32 memberIndex = 0;
    while(pTeamInfo->member_list.size() > 0)
    {
        CROSS_TEAM_MEMBER& member_data = pTeamInfo->member_list[0];
        uint64 member_id = U64ID(member_data.player_id_h, member_data.player_id_l);
        uint32 gate_id = member_data.gatesvr_id;
        RemoveMemberFromTeam(member_id, teamID);
        NotifyPlayerDelTeam(member_id, gate_id);     // 通知客户端,队伍解散
    }

    // 删除队伍
    m_teamMap.erase(tIter);

    return true;
}

void CrossTeamManager::SendErrorCode(uint64 playerID, uint32 gateID, uint16 errcode)
{
    ERROR_CODE errMsg;
    errMsg.errcode = errcode;
    GateHandler::Instance()->SendByGateID(gateID, MSG_ERROR_CODE, &errMsg, playerID);
}

void CrossTeamManager::SendTeamMemberAddAck(uint64 playerID, uint32 gateID, uint16 errcode, const std::string& src_name, const std::string& dst_name)
{
    CROSS_TEAM_MEMBER_ADD_ACK sendMsg;
    sendMsg.errcode = errcode;
    sendMsg.src_name = src_name;
    sendMsg.dst_name = dst_name;

    GateHandler::Instance()->SendByGateID(gateID, MSG_CROSS_TEAM_MEMBER_ADD_ACK, &sendMsg, playerID);
}

// 发送删除组队成员消息
void CrossTeamManager::SendTeamMemberDelAck(uint64 delPlayerID, uint32 delPlayerGateID, uint64 teamID, const std::string& dst_name, const std::string& src_name, const std::string& leader_name)
{
    CROSS_TEAM_MEMBER_DEL_ACK  sendMsg;

    sendMsg.src_name = src_name;
    sendMsg.dst_name = dst_name;
    sendMsg.leader_name = leader_name;

    GateHandler::Instance()->SendByGateID(delPlayerGateID, MSG_CROSS_TEAM_MEMBER_DEL_ACK, &sendMsg, delPlayerID);

    SendToAllTeamMember(teamID, MSG_CROSS_TEAM_MEMBER_DEL_ACK, &sendMsg);
}

void CrossTeamManager::SendToAllTeamMember(CROSS_TEAM_INFO* pTeam, uint32 msgID, void* data, uint64 exceptID)
{
    if(!pTeam) return;

    for(std::vector<CROSS_TEAM_MEMBER>::iterator it = pTeam->member_list.begin(); it != pTeam->member_list.end(); ++it)
    {
        uint64 memberID = U64ID(it->player_id_h, it->player_id_l);
        if(memberID == exceptID) 
            continue;

        GateHandler::Instance()->SendByGateID(it->gatesvr_id, msgID, data, memberID);
    }
}

// 发送消息队伍成员
void CrossTeamManager::SendToAllTeamMember(uint64 teamID, uint32 msgID, void* data, uint64 exceptID)
{
    SendToAllTeamMember(GetTeamInfo(teamID), msgID, data, exceptID);
}
// 删除队伍通知
void CrossTeamManager::NotifyPlayerDelTeam(uint64 playerID, uint32 playerGateID)
{
    CROSS_TEAM_INFO team_info;
    team_info.opt_flag = OPT_INIT_FLAG;
    team_info.team_type = 0;

    GateHandler::Instance()->SendByGateID(playerGateID, MSG_CROSS_TEAM_INFO_NTF, &team_info, playerID);
}

void CrossTeamManager::NotifyTeamMemberTeamInfo(uint64 playerID)
{
    CROSS_TEAM_INFO* pTeamInfo = GetTeamInfoByPlayer(playerID);
    if(!pTeamInfo)
    {
        CnAssert(false);
        return;
    }

    // 查找gateID
    uint32 gateID = 0;
    for(std::vector<CROSS_TEAM_MEMBER>::iterator iter = pTeamInfo->member_list.begin(); iter != pTeamInfo->member_list.end(); iter++)
    {
        const CROSS_TEAM_MEMBER& member = *iter;
        if( U64ID(member.player_id_h, member.player_id_l) == playerID )
        {
            gateID = member.gatesvr_id;
        }
    }
    if(!gateID)
    {
        CnAssert(false);
        return;
    }

    GateHandler::Instance()->SendByGateID(gateID, MSG_CROSS_TEAM_INFO_NTF, pTeamInfo, playerID);
}
// 添加成员通知
void CrossTeamManager::NotifyTeamMemberAddMember(uint64 teamID, const CROSS_TEAM_MEMBER& team_member)
{
    CROSS_TEAM_INFO* pTeamInfo = GetTeamInfo(teamID);

    CROSS_TEAM_INFO team_info;

    FillTeamInfo(pTeamInfo, team_info, OPT_ADD_FLAG);
    team_info.member_list.push_back(team_member);

    SendToAllTeamMember(teamID, MSG_CROSS_TEAM_INFO_NTF, &team_info, U64ID(team_member.player_id_h, team_member.player_id_l));
}

void CrossTeamManager::NotifyChangeTeamDungeon(CROSS_TEAM_INFO* team_info)
{
    CHANGE_DUNGEON_REQ changeDungeonReq;
    changeDungeonReq.dungeon_id = team_info->dungeon_id;
    changeDungeonReq.channel_id = team_info->channel_id;

    SendToAllTeamMember(U64ID(team_info->team_id_h, team_info->team_id_l), MSG_CROSS_TEAM_NOTIFY_DUNGEON, &changeDungeonReq);
}

// 队伍信息改变通知
void CrossTeamManager::SendTeamUpdateInfoToMember(CROSS_TEAM_INFO* pTeam)
{
    if(!pTeam) 
        return;

    uint64 teamID = U64ID(pTeam->team_id_h, pTeam->team_id_l);

    // 队伍信息
    CROSS_TEAM_INFO sendMsg;
    FillTeamInfo(pTeam, sendMsg, OPT_UPDATE_FLAG);

    SendToAllTeamMember(teamID, MSG_CROSS_TEAM_INFO_NTF, &sendMsg);
}

void CrossTeamManager::AddTeamMember(uint64 teamID, const CROSS_TEAM_MEMBER& team_member)
{
    AddMemberToTeam(team_member, teamID);

    NotifyTeamMemberTeamInfo(U64ID(team_member.player_id_h, team_member.player_id_l));               // 通知玩家队伍信息

    NotifyTeamMemberAddMember(teamID, team_member);
}

void CrossTeamManager::AddMemberToTeam(const CROSS_TEAM_MEMBER& member, uint64 teamID)
{
    CROSS_TEAM_INFO* team_info = GetTeamInfo(teamID);
    if(!team_info)
    {
        CnAssert(false);
        return;
    }

#ifdef DEBUG        // 检查该成员确实不在队伍中
    for(std::vector<CROSS_TEAM_MEMBER>::iterator mIter = team_info->member_list.begin(); mIter != team_info->member_list.end(); mIter++)
    {
        const CROSS_TEAM_MEMBER& member2 = *mIter;
        if(member2.player_id_h == member.player_id_h && member2.player_id_l == member.player_id_l)
        {
            CnAssert(false);
        }
    }
#endif

    team_info->member_list.push_back(member);

    uint64 playerID = U64ID(member.player_id_h, member.player_id_l);
    CnAssert(m_PlayerTeamMap.find(playerID) == m_PlayerTeamMap.end());
    m_PlayerTeamMap.insert(std::make_pair(playerID, teamID));
}

void CrossTeamManager::RemoveMemberFromTeam(uint64 playerID, uint64 teamID)
{
    CROSS_TEAM_INFO* team_info = GetTeamInfo(teamID);
    if(!team_info)
    {
        CnAssert(false);
        return;
    }

    bool checkExist = false;
    for(std::vector<CROSS_TEAM_MEMBER>::iterator mIter = team_info->member_list.begin(); mIter != team_info->member_list.end(); mIter++)
    {
        const CROSS_TEAM_MEMBER& member2 = *mIter;
        if(playerID == U64ID(member2.player_id_h, member2.player_id_l))
        {
            checkExist = true;
            team_info->member_list.erase(mIter);
            break;
        }
    }
    CnAssert(checkExist);

    PlayerTeamMap::iterator pIter = m_PlayerTeamMap.find(playerID);
    if(pIter == m_PlayerTeamMap.end())
    {
        CnAssert(false);
    }
    else
    {
        m_PlayerTeamMap.erase(pIter);
    }
}

// 生成队伍ID
uint64 CrossTeamManager::GenTeamID()
{
    static uint16 serialNum = 0;
    // 时间戳 32bit, 序列号16bit, 
    uint32 high = (uint32)time(0);
    uint32 low = (++serialNum << 16);
    low |= ServerConfig::Instance()->GetCenterID() & 0xFFFF;   // 区ID

    uint64 sn = high;
    sn = sn << 32;
    sn += low;

    return sn;
}

void CrossTeamManager::OnCrossTeamInfoReq(uint64 playerID, void* data)
{
    CROSS_TEAM_INFO_REQ* recvMsg = (CROSS_TEAM_INFO_REQ*)data;
    if(!recvMsg) return;
}

void CrossTeamManager::OnCrossTeamShowListReq(uint64 playerID, void* data)
{
    CROSS_TEAM_SHOW_LIST_REQ* recvMsg = (CROSS_TEAM_SHOW_LIST_REQ*)data;
    if(!recvMsg) return;

    std::vector<CROSS_TEAM_INFO*> list;
    for(CrossTeamMap::iterator teamIt = m_teamMap.begin(); teamIt != m_teamMap.end(); ++teamIt)
    {
        if((teamIt->second.team_flag & TEAM_FLAG_DISPLAY) || (teamIt->second.team_flag & TEAM_FLAG_NOENTER))
        {
            continue;
        }
        list.push_back(&teamIt->second);
    }
    
    // 计算总页数
    int32 total_page_num = 1;
    if(list.size() > 0)
        total_page_num = ((list.size()-1) / TEAM_PAGE_SHOW_NUM) + 1;

    int32 page_num = Crown::SDMax(int32(recvMsg->page_num)-1, 0);      // 页数减1

    // 当前页数>最大页数
    if(page_num >= total_page_num)
    {
        page_num = total_page_num-1;
    }

    // 计算跳过的队伍数量
    uint32 team_count = page_num * TEAM_PAGE_SHOW_NUM;

    CROSS_TEAM_SHOW_LIST_ACK sendMsg;
    sendMsg.curr_page_num = page_num + 1;  // 当前页面
    sendMsg.total_page_num = total_page_num;    // 页面总数

    // 填充队伍显示信息
    for(uint32 i=0, idx=team_count; (idx < list.size()) && (i < TEAM_PAGE_SHOW_NUM); ++idx, ++i)
    {
        CROSS_TEAM_SHOW_INFO teamShowInfo;

        if(list[idx]->member_list.size() > 0)
        {
            teamShowInfo.leader_name = list[idx]->member_list[0].name;
            teamShowInfo.leader_sex = list[idx]->member_list[0].sex;
            teamShowInfo.leader_career = list[idx]->member_list[0].career;
            teamShowInfo.leader_level = list[idx]->member_list[0].level;
        }

        teamShowInfo.team_id_h = list[idx]->team_id_h;
        teamShowInfo.team_id_l = list[idx]->team_id_l;
        teamShowInfo.team_type = list[idx]->team_type;
        teamShowInfo.max_member_num = GetMaxTeamMenber(list[idx]->team_type);
        teamShowInfo.member_num = list[idx]->member_list.size();
        teamShowInfo.dungeon_id = list[idx]->dungeon_id;

        sendMsg.team_list.push_back(teamShowInfo);
    }

    GateHandler::Instance()->SendByGateID(recvMsg->gatesvr_id, MSG_CROSS_TEAM_SHOW_LIST_ACK, &sendMsg, playerID);
}

CROSS_TEAM_INFO* CrossTeamManager::GetTeamInfo(uint64 teamID)
{
    CrossTeamMap::iterator iter = m_teamMap.find(teamID);
    if(iter == m_teamMap.end())
        return NULL;
    return &iter->second;
}

uint64 CrossTeamManager::GetTeamID(uint64 playerID)
{
    PlayerTeamMap::iterator iter = m_PlayerTeamMap.find(playerID);
    if(iter == m_PlayerTeamMap.end())
        return 0;
    return iter->second;
}

CROSS_TEAM_INFO* CrossTeamManager::GetTeamInfoByPlayer(uint64 playerID)
{
    uint64 teamID = GetTeamID(playerID);
    if(teamID == 0)
        return NULL;
    return GetTeamInfo(teamID);
}

CROSS_TEAM_MEMBER CrossTeamManager::MakeTeamMember(Player* player)
{
    CROSS_TEAM_MEMBER team_member;

    // 队伍成员信息
    team_member.gatesvr_id = player->GetGateSvrID();
    team_member.player_id_h = U64ID_HIGH(player->GetPlayerID());
    team_member.player_id_l = U64ID_LOW(player->GetPlayerID());
    team_member.is_offline = false;
    team_member.name = player->GetName();
    team_member.sex = player->GetSex();
    team_member.career = player->GetCareer();
    team_member.level = player->GetLevel();
    team_member.battle_power = player->GetBattlePower();
    return team_member;
}

bool CrossTeamManager::HasMember(uint64 teamID, uint64 playerID)
{
    CROSS_TEAM_INFO* pTeam = GetTeamInfo(teamID);
    if(!pTeam)
        return false;

    for(std::vector<CROSS_TEAM_MEMBER>::iterator it = pTeam->member_list.begin(); it != pTeam->member_list.end(); ++it)
    {
        if(U64ID(it->player_id_h, it->player_id_l) == playerID)
            return true;
    }
    return false;
}

CROSS_TEAM_MEMBER* CrossTeamManager::GetTeamMemberDataByName(uint64 teamID, const std::string& name)
{
    CROSS_TEAM_INFO* teamInfo = GetTeamInfo(teamID);
    if(!teamInfo || teamInfo->member_list.size() == 0)
    {
        CnAssert(false);
        return NULL;
    }

    // 第一个非离线玩家是队长
    uint64 leader_id = 0;
    for(std::vector<CROSS_TEAM_MEMBER>::iterator memberIt = teamInfo->member_list.begin(); memberIt != teamInfo->member_list.end(); memberIt++)
    {
        if(memberIt->name == name)
            return &(*memberIt);
    }
    return NULL;
}

CROSS_TEAM_MEMBER* CrossTeamManager::GetTeamMemberData(uint64 teamID, uint64 playerID)
{
    CROSS_TEAM_INFO* teamInfo = GetTeamInfo(teamID);
    if(!teamInfo || teamInfo->member_list.size() == 0)
    {
        CnAssert(false);
        return NULL;
    }

    // 第一个非离线玩家是队长
    uint64 leader_id = 0;
    for(std::vector<CROSS_TEAM_MEMBER>::iterator memberIt = teamInfo->member_list.begin(); memberIt != teamInfo->member_list.end(); memberIt++)
    {
        if(playerID == U64ID(memberIt->player_id_h, memberIt->player_id_l))
        return &(*memberIt);
    }
    return NULL;
}

CROSS_TEAM_MEMBER* CrossTeamManager::GetTeamLeaderData(uint64 teamID)
{
    CROSS_TEAM_INFO* teamInfo = GetTeamInfo(teamID);
    if(!teamInfo || teamInfo->member_list.size() == 0)
    {
        CnAssert(false);
        return NULL;
    }

    // 第一个非离线玩家是队长
    uint64 leader_id = 0;
    for(std::vector<CROSS_TEAM_MEMBER>::iterator memberIt = teamInfo->member_list.begin(); memberIt != teamInfo->member_list.end(); memberIt++)
    {
        if(memberIt->is_offline)
            continue;
        return &(*memberIt);
    }
    return NULL;
}

uint64 CrossTeamManager::GetTeamLeader(uint64 teamID)
{
    CROSS_TEAM_MEMBER* leaderData = GetTeamLeaderData(teamID);
    if(!leaderData)
        return 0;

    return U64ID(leaderData->player_id_h, leaderData->player_id_l);
}

bool CrossTeamManager::HasOnlinePlayer(CROSS_TEAM_INFO* pTeam)
{
    if(!pTeam)
    {
        CnAssert(false);
        return false;
    }

    uint32 onlinePlayerNum = 0;
    for(std::vector<CROSS_TEAM_MEMBER>::iterator memberIt = pTeam->member_list.begin(); memberIt != pTeam->member_list.end(); memberIt++)
    {
        if(memberIt->is_offline)
            continue;

        onlinePlayerNum++;
    }

    return onlinePlayerNum > 0;
}

void CrossTeamManager::FillTeamInfo(const CROSS_TEAM_INFO* inInfo, CROSS_TEAM_INFO& outInfo, uint8 opt_flag)
{
    outInfo.opt_flag = opt_flag;
    outInfo.team_id_l = inInfo->team_id_l;
    outInfo.team_id_h = inInfo->team_id_h;
    outInfo.team_type = inInfo->team_type;
    outInfo.team_flag = inInfo->team_flag;
    outInfo.max_member_num = inInfo->max_member_num;
}