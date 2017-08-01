#include "gameholder_pch.h"
#include "team_manager.h"
#include "player_manager.h"
#include "dungeon_manager.h"
#include "player.h"
#include "message_encoder.h"
#include "center_handler.h"
#include "event_define.h"
#include "ride_manager.h"
#include "team_dungeon_config.h"
#include "gate_handler.h"
#include "trade_manager.h"
#include "vip_config.h"
#include "dungeon_data.h"
#include "team_relationship.h"
#include "action_controller.h"
#include "game_define.h"
#include "center_handler.h"
#include "server_config.h"
#include "namelimit.h"
#include "head_portrait_manager.h"
#include "monster.h"
#include "game_util.h"

IMPLEMENT_SINGLETON(TeamManager)


TeamManager::TeamManager()
{
}

TeamManager::~TeamManager()
{
}

void TeamManager::OnRecvFromPlayer(Player* player, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_TEAM_SHOW_LIST_REQ:                // 队伍显示列表请求
        OnRecvTeamShowListReq(player, data);
        break;
    case MSG_TEAM_INFO_REQ:                     // 我的队伍信息请求
        OnRecvTeamInfoReq(player);
        break;
    case MSG_CREATE_TEAM_REQ:                   // 创建队伍请求
        OnRecvCreateTeamReq(player, data);
        break;
    case MSG_DESTORY_TEAM_REQ:                  // 销毁队伍请求
        OnRecvDestroyTeamReq(player);
        break;
    case MSG_TEAM_MEMBER_DEL_REQ:               // 踢除队友
        OnRecvTeamMemberDelReq(player, data);
        break;
    case MSG_TEAM_QUICK_JOIN_REQ:               // 快速加入队伍请求
        OnRecvTeamQuickJoinReq(player, data);
        break;
    case MSG_TEAM_FLY_REQ:                      // 请求飞到队友身边
        OnRecvFlyReq(player, data);
        break;
    default:
        CnAssert(false);
        break;
    }
}

void TeamManager::OnRecvFromCenter(Player* player, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_TEAM_INFO_NTF:                     // 队伍信息通知
        if (player)
        {
            player->SendToGate(MSG_TEAM_INFO_NTF, data);
        }
        else
        {
            OnRecvTeamInfoNtf(data);
        }
        break;
    case MSG_TEAM_MEMBER_RESET_STATE:           // 重置角色的组队状态
        OnRecvTeamMemberResetState(player, data);
        break;
    default:
        CnAssert(false);
        break;
    }
}

// 获取队伍信息
TEAM_INFO* TeamManager::GetTeamInfo(uint64 teamID)
{
    TeamMap::iterator it = m_teamMap.find(teamID);
    if(it == m_teamMap.end())
        return NULL;

    return &(it->second);
}
// 获取成员信息
const TEAM_MEMBER* TeamManager::GetTeamMemberData(uint64 teamID, uint64 memberID)
{
    TEAM_INFO* pTeamInfo = GetTeamInfo(teamID);
    if(!pTeamInfo)
        return NULL;

    for(std::vector<TEAM_MEMBER>::iterator iter = pTeamInfo->member_list.begin(); iter != pTeamInfo->member_list.end(); iter++)
    {
        TEAM_MEMBER* memberData = &(*iter);
        if(U64ID(memberData->player_id_h, memberData->player_id_l) == memberID)
            return memberData;
    }
    return NULL;
}

// 飞到队友身边
void TeamManager::OnRecvFlyReq(Player* player, void* data)
{
    TEAM_FLY_REQ* req = (TEAM_FLY_REQ*)data;

    uint64 teamID = player->GetTeamID();
    if(!teamID)
        return;     // 该玩家没有队伍

    const TEAM_MEMBER* memberData = GetTeamMemberData(teamID, U64ID(req->dst_player_id_h, req->dst_player_id_l));
    if(!memberData)
        return;     // 没有该玩家

    if(U64ID(memberData->player_id_h, memberData->player_id_l) == player->GetPlayerID())
    {
        return;     // 不能飞自己
    }

    if(!IsDungeonCanFly(DUNGEON_ID(memberData->unionmap_id)))
    {
        return;     // 队友所在的副本,不能飞
    }

    Player* teamMember = PlayerManager::Instance()->GetPlayerByPlayerID(U64ID(memberData->player_id_h, memberData->player_id_l));

    // 和队友在同一个场景里
    if(teamMember && player->GetUnionMapID() == teamMember->GetUnionMapID())
    {
        int32 newX = teamMember->GetX();
        int32 newY = teamMember->GetY();
        CoVec2 newPos = player->GetScene()->FindValidPositionInCircle(CoVec2(newX, newY), g_TeamFlyNearDistance, player->GetRadius());
        int32 newAngle = teamMember->GetAngle();
        player->GetActionController()->JumpTo(int32(newPos.x), int32(newPos.y));
        player->GetActionController()->AngleTo(newAngle);
        return;
    }

    // 发送跳副本请求
    CHANGE_DUNGEON_REQ flyReq;
    flyReq.jump_type = DUNGEON_JUMP_PLAYER;
    flyReq.dungeon_id   = DUNGEON_ID(memberData->unionmap_id);
    flyReq.channel_id   = CHANNEL_ID(memberData->unionmap_id);
    flyReq.scene_id     = SCENE_ID(memberData->unionmap_id);
    flyReq.fly_player_id.id_h = memberData->player_id_h;
    flyReq.fly_player_id.id_l = memberData->player_id_l;

    if(flyReq.dungeon_id == 0 || flyReq.channel_id == 0 || flyReq.scene_id == 0)
    {
        return;         // 队友的副本信息不正确(可能发到CenterServer后，还未转发回来)
    }

    player->OnRecv(MSG_CHANGE_DUNGEON_REQ, &flyReq);
}

// 队伍信息请求
void TeamManager::OnRecvTeamInfoReq(Player* player)
{
    if(!player) return;

    uint64 teamID = player->GetTeamID();
    if(!teamID)
        return;

    TEAM_INFO* teamInfo = GetTeamInfo(teamID);
    if(!teamInfo)
        return;

    TEAM_INFO sendMsg;
    FillTeamInfo(teamInfo, sendMsg, OPT_ADD_FLAG);
    for (uint32 i = 0; i < teamInfo->member_list.size(); ++i)
    {
        sendMsg.member_list.push_back(teamInfo->member_list[i]);
    }

    player->SendToGate(MSG_TEAM_INFO_NTF, &sendMsg);
}

// 队伍信息通知
void TeamManager::OnRecvTeamInfoNtf(void* data)
{
    TEAM_INFO* recvMsg = (TEAM_INFO*)data;

    if(!recvMsg) 
        return;

    if(recvMsg->opt_flag == OPT_INIT_FLAG)
    {
        if(recvMsg->member_list.size() > 0)
        {
            OnCreateTeamInfo(recvMsg);
        }
        else
        {
            OnDestoryTeamInfo(recvMsg);
        }
    }
    else if(recvMsg->opt_flag == OPT_ADD_FLAG)
    {
        OnAddTeamInfo(recvMsg);     // 添加队员
    }
    else if(recvMsg->opt_flag == OPT_DEL_FLAG)
    {
        OnDelTeamInfo(recvMsg);     // 删除队员
    }
    else if(recvMsg->opt_flag == OPT_UPDATE_FLAG)
    {
        OnUpdateTeamInfo(recvMsg);  // 更新队员信息
    }
}

// 创建队伍请求
void TeamManager::OnRecvCreateTeamReq(Player* player, void* data)
{
    TEAM_CREATE_REQ* recvMsg = (TEAM_CREATE_REQ*)data;
    if(!player || !recvMsg) 
        return;

    // 检查队伍名的合法性
    std::string team_name = recvMsg->team_name;
    uint16 error = ERR_FAILED;
    error = NameLimit::Instance()->IsValidNameStr(team_name);
    if (error != ERR_SUCCEED)
    {
        ERROR_CODE ntf;
        ntf.errcode = error;
        player->SendToGate(MSG_ERROR_CODE, &ntf);
        return;
    }

    recvMsg->leader = MakeTeamMember(player);
    recvMsg->leader.server_id = CalRegionID(ServerConfig::Instance()->GetCenterID(), ServerConfig::Instance()->GetMyID());

    CenterHandler::Instance()->SendToMasterCenter(MSG_CREATE_TEAM_REQ, recvMsg, player);
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
    team_member.battle_power = player->GetBattlePower();
    team_member.unionmap_id = player->GetUnionMapID();
    team_member.pos_x = player->GetX();
    team_member.pos_y = player->GetY();
    team_member.server_id = ServerConfig::Instance()->GetRegionID();
    team_member.head_icon = player->GetHeadPortraitMgr()->GetCurHeadIconId();

    return team_member;
}

// 销毁队伍请求
void TeamManager::OnRecvDestroyTeamReq(Player* player)
{
    if(!player) return;

    NONE sendMsg;
    player->SendToCenter(MSG_DESTORY_TEAM_REQ, &sendMsg);
}

// 删除队友
void TeamManager::OnRecvTeamMemberDelReq(Player* player, void* data)
{
    TEAM_MEMBER_DEL_REQ* recvMsg = (TEAM_MEMBER_DEL_REQ*)data;

    if(!player || !recvMsg)
        return;

    CenterHandler::Instance()->SendToMasterCenter(MSG_TEAM_MEMBER_DEL_REQ, recvMsg, player);
}

// 快速加入队伍请求
void TeamManager::OnRecvTeamQuickJoinReq(Player* player, void* data)
{
    TEAM_JOIN* recvMsg = (TEAM_JOIN*)data;

    if(!recvMsg || !player) 
        return;

    // 服务器上没有队伍
    if(m_teamMap.size() < 1)
    {
        player->NotifyErrorCode(ERR_NO_TEAM_JOIN);
        return;
    }

    recvMsg->player = MakeTeamMember(player);
    player->SendToCenter(MSG_TEAM_QUICK_JOIN_REQ, data);
}

// 队伍显示列表请求
void TeamManager::OnRecvTeamShowListReq(Player* player, void* data)
{
    TEAM_SHOW_LIST_REQ* recvMsg = (TEAM_SHOW_LIST_REQ*)data;

    if(!recvMsg || !player) return;

    std::vector<TEAM_INFO*> list;
    for(TeamMap::iterator teamIt = m_teamMap.begin(); teamIt != m_teamMap.end(); ++teamIt)
    {
        if((teamIt->second.team_flag & TEAM_FLAG_DISPLAY) || 
            (teamIt->second.team_flag & TEAM_FLAG_NOENTER))
        {
            continue;
        }
        list.push_back(&teamIt->second);
    }

    uint32 total_page_num = (list.size() / TEAM_PAGE_SHOW_NUM);

    // 页数减1
    if(recvMsg->page_num > 0) 
        --recvMsg->page_num;

    // 计算队伍的总页数
    if(total_page_num <= recvMsg->page_num)
    {
        recvMsg->page_num = total_page_num;
        if(recvMsg->page_num > 0)
        {
            recvMsg->page_num = recvMsg->page_num - 1;
        }
    }

    // 计算跳过的队伍数量
    uint32 team_count = recvMsg->page_num * TEAM_PAGE_SHOW_NUM;

    TEAM_SHOW_LIST_ACK sendMsg;
    sendMsg.curr_page_num = recvMsg->page_num + 1;  // 当前页面
    sendMsg.total_page_num = total_page_num + 1;    // 页面总数

    // 填充队伍显示信息
    for(uint32 i=0, idx=team_count; (idx < list.size()) && (i < TEAM_PAGE_SHOW_NUM); ++idx, ++i)
    {
        sendMsg.team_list.push_back(*list[idx]);
    }

    player->SendToGate(MSG_TEAM_SHOW_LIST_ACK, &sendMsg);
}

// 重置队员状态ID
void TeamManager::OnRecvTeamMemberResetState(Player* player, void* data)
{
    if(!player) return;

    if(player->GetNetState() == Player::kStateJoinTeam)
    {
        player->SetNetState(Player::kStateInGame);
    }
}

// 创建队伍信息
void TeamManager::OnCreateTeamInfo(TEAM_INFO* team_info)
{
    if(!team_info) return;

    uint64 teamID = U64ID(team_info->team_id_h, team_info->team_id_l);
    TeamMap::iterator teamIt = m_teamMap.find(teamID);
    if(teamIt != m_teamMap.end())
    {
        // 不应该出现这种情况
        CnAssert(false);
        teamIt->second = *team_info;
    }

    m_teamMap[teamID] = *team_info;

    // 设置成员队伍ID
    for(std::vector<TEAM_MEMBER>::iterator tIter = team_info->member_list.begin(); tIter != team_info->member_list.end(); tIter++)
    {
        Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(U64ID(tIter->player_id_h, tIter->player_id_l));
        if(!player)
            continue;
        CnAssert(player->GetTeamID() == 0);     // 之前应该没有队伍
        player->SetTeamID(teamID);
        player->SendToCenterTeamMemberProp();
    }

    TeamRelationShip::Instance()->OnCreateTeam(m_teamMap[teamID]);
}

// 添加队友信息
void TeamManager::OnAddTeamInfo(TEAM_INFO* team_info)
{
    if(!team_info) return;

    uint64 teamID = U64ID(team_info->team_id_h, team_info->team_id_l);
    TeamMap::iterator teamIt = m_teamMap.find(teamID);
    if(teamIt == m_teamMap.end())
        return;

    for(std::vector<TEAM_MEMBER>::iterator it = team_info->member_list.begin(); it != team_info->member_list.end(); ++it)
    {
        teamIt->second.member_list.push_back(*it);

        // 设置成员队伍ID
        Player* teamMember = PlayerManager::Instance()->GetPlayerByPlayerID(U64ID(it->player_id_h, it->player_id_l));
        if(teamMember)
        {
            CnAssert(teamMember->GetTeamID() == 0);     // 之前应该没有队伍
            teamMember->SetTeamID(teamID);
            teamMember->SendToCenterTeamMemberProp();   // 通知CenterServer新队员的信息
        }
    }

    TeamRelationShip::Instance()->OnAddMember(teamIt->second);
}

// 删除队友信息
void TeamManager::OnDelTeamInfo(TEAM_INFO* team_info)
{
    if(!team_info) return;

    // 获取队伍
    uint64 teamID = U64ID(team_info->team_id_h, team_info->team_id_l);
    TeamMap::iterator teamIt = m_teamMap.find(teamID);
    if(teamIt == m_teamMap.end())
        return;

    // 删除队员
    for(std::vector<TEAM_MEMBER>::iterator memberIt = team_info->member_list.begin(); memberIt != team_info->member_list.end(); ++memberIt)
    {
        for(std::vector<TEAM_MEMBER>::iterator delIt = teamIt->second.member_list.begin(); delIt != teamIt->second.member_list.end(); ++delIt)
        {
            if((memberIt->player_id_h == delIt->player_id_h) && (memberIt->player_id_l == delIt->player_id_l))
            {
                // 设置队伍ID
                uint64 playerID = U64ID(delIt->player_id_h, delIt->player_id_l);
                Player* teamMember = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
                if(teamMember)
                {
                    CnAssert(teamMember->GetTeamID() != 0);     // 之前应该有队伍
                    teamMember->SetTeamID(0);
                }
                // 删除队伍成员信息
                teamIt->second.member_list.erase(delIt);
                break;
            }
        }
    }

    TeamRelationShip::Instance()->OnDelMember(teamIt->second);

    // 如果成员为空则删除队伍
    if(teamIt->second.member_list.size() < 1)
    {
        CnAssert(false);
        TeamRelationShip::Instance()->OnDestoryTeam(teamIt->second);
        m_teamMap.erase(teamIt);
    }
}

// 更新队伍信息
void TeamManager::OnUpdateTeamInfo(TEAM_INFO* team_info)
{
    if(!team_info) return;

    uint64 teamID = U64ID(team_info->team_id_h, team_info->team_id_l);

    TEAM_INFO* pTeam = GetTeamInfo(teamID);
    if(!pTeam)
        return;

    // 填充队伍信息
    FillTeamInfo(team_info, *pTeam, OPT_INIT_FLAG);

    // 更新队员信息
    for(std::vector<TEAM_MEMBER>::iterator memberIt = team_info->member_list.begin(); memberIt != team_info->member_list.end(); ++memberIt)
    {
        bool checkExist = false;        // 检查数据合理性
        for(std::vector<TEAM_MEMBER>::iterator updateIt = pTeam->member_list.begin(); updateIt != pTeam->member_list.end(); ++updateIt)
        {
            if((memberIt->player_id_h == updateIt->player_id_h) && (memberIt->player_id_l == updateIt->player_id_l))
            {
                *updateIt = *memberIt;
                checkExist = true;
                break;
            }
        }
        CnAssert(checkExist);

        // 检查队伍ID
        Player* teamMember = PlayerManager::Instance()->GetPlayerByPlayerID(U64ID(memberIt->player_id_h, memberIt->player_id_l));
        if(teamMember)
        {
            CnAssert(teamMember->GetTeamID() == teamID);     // 检查队伍ID
        }
    }

    TeamRelationShip::Instance()->OnUpdateInfo(*pTeam);
}

// 销毁队伍信息
void TeamManager::OnDestoryTeamInfo(TEAM_INFO* team_info)
{
    if(!team_info) return;

    // 拼队伍ID
    uint64 teamID = U64ID(team_info->team_id_h, team_info->team_id_l);

    // 查找队伍
    TeamMap::iterator teamIt = m_teamMap.find(teamID);
    if(teamIt == m_teamMap.end())
    {
        return;
    }

    // 设置成员队伍ID
    for (std::vector<TEAM_MEMBER>::iterator memberIt = teamIt->second.member_list.begin(); memberIt != teamIt->second.member_list.end(); ++memberIt)
    {
        uint64 teamMemberID = U64ID(memberIt->player_id_h, memberIt->player_id_l);
        Player* teamMember = PlayerManager::Instance()->GetPlayerByPlayerID(teamMemberID);
        if(teamMember)
        {
            CnAssert(teamMember->GetTeamID() != 0);
            teamMember->SetTeamID(0);
        }
    }

    TeamRelationShip::Instance()->OnDestoryTeam(teamIt->second);
    m_teamMap.erase(teamIt);
}

// 填充队伍信息
void TeamManager::FillTeamInfo(TEAM_INFO* inInfo, TEAM_INFO& outInfo, uint8 opt_flag)
{
    outInfo.opt_flag = opt_flag;
    outInfo.team_name = inInfo->team_name;
    outInfo.team_id_l = inInfo->team_id_l;
    outInfo.team_id_h = inInfo->team_id_h;
    outInfo.team_flag = inInfo->team_flag;
    outInfo.max_member_num = inInfo->max_member_num;
}

// 通知空的队伍信息
void TeamManager::NotifyMemberEmptyTeamInfo( Player* player, int32 teamType )
{
    TEAM_INFO team_info;

    if(!player)
        return;

    team_info.Clear();
    team_info.opt_flag = OPT_INIT_FLAG;

    player->SendToGate(MSG_TEAM_INFO_NTF, &team_info);
}

uint32 TeamManager::GetTeamChannelIDByPlayer(Player* player,uint32 dungeonID)
{
    if (!player)
    {
        return 0;
    }

    uint64 teamID = player->GetTeamID();
    if(!teamID)
        return 0;

    TEAM_INFO* pTeam = GetTeamInfo(teamID);
    if (!pTeam)
    {
        return 0;
    }

    for (uint32 i = 0; i < pTeam->member_list.size(); ++i)
    {
        const TEAM_MEMBER& member = pTeam->member_list[i];
        uint64 memberID = U64ID(member.player_id_h,member.player_id_l);
        Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(memberID);
        if (player)
        {
            uint32 dungeon_id = DUNGEON_ID(player->GetUnionMapID());
            if (dungeon_id == dungeonID)
            {
                return CHANNEL_ID(player->GetUnionMapID());
            }
        }
    }

    return 0;
}

void TeamManager::UpdateTeamMemberInfo(Player* player)
{
    uint64 teamID = player->GetTeamID();
    if(!teamID)
        return;     // 该玩家没有队伍

    const TEAM_MEMBER* memberData = GetTeamMemberData(teamID, player->GetPlayerID());
    if(!memberData)
        return;     // 没有该玩家


    TEAM_INFO* pTeamInfo = GetTeamInfo(teamID);
    if(!pTeamInfo)
        return;

    TEAM_MEMBER	team_member = MakeTeamMember(player);
    CenterHandler::Instance()->SendToMasterCenter(MSG_TEAM_MEMBER_UPDATE_INFO, &team_member, player);
}

bool TeamManager::TeamExpShare(uint64 teamID, int32 exp, Monster* pDefender)
{
    if (!pDefender)
    {
        CnAssert(false);
        return false;
    }

    TEAM_INFO* pTeamInfo = GetTeamInfo(teamID);
    if (!pTeamInfo)
    {
        return false;
    }

    if (pTeamInfo->member_list.size() <= 1)
    {
        return false;
    }

    uint64 player_id(0);
    real dis(0.f);
    Player* pPlayer(NULL);
    std::vector<Player*> playr_list;

    float fSum(0.f);

    for (std::vector<TEAM_MEMBER>::const_iterator cIter = pTeamInfo->member_list.begin(); cIter != pTeamInfo->member_list.end(); ++cIter)
    {
        player_id = U64ID(cIter->player_id_h, cIter->player_id_l);
        pPlayer = PlayerManager::Instance()->GetPlayerByPlayerID(player_id);
        if (!pPlayer)
        {
            continue;
        }

        if (pPlayer->GetScene() != pDefender->GetScene())
        {
            continue;
        }

        dis = DistanceBySceneObject(pPlayer, pDefender);
        if (dis > 3000)
        {
            continue;
        }

        playr_list.push_back(pPlayer);
        fSum += pow((float)(pPlayer->GetLevel() + 10), 3.f);
    }

    if (playr_list.size() <= 1)
    {
        return false;
    }

    // 总经验
    exp *= playr_list.size();

    int32 nExp(0);
    float fPara(0.f);

    for (std::vector<Player*>::iterator mIter = playr_list.begin(); mIter != playr_list.end(); ++mIter)
    {
        pPlayer = *mIter;

        fPara = (pow((float)(pPlayer->GetLevel() + 10), 3.f)) / fSum;
        nExp = int32(fPara * exp);

        pPlayer->AddKillExp(nExp, 0);
    }

    return true;
}
