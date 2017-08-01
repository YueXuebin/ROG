#include "centerserver_pch.h"
#include "guild_manager.h"
#include "guild_job.h"
#include "config_loader.h"
#include "guild_config.h"
#include "game_handler.h"
#include "player.h"
#include "player_manager.h"
#include "player_net_handler.h"
#include "trade_manager.h"
#include "guild_config.h"
#include "guild_science_config.h"
#include "guild_science_manager.h"
#include "dongyi_log.h"

IMPLEMENT_SINGLETON(GuildManager)

// ==================================================
// 构造函数
// ==================================================
GuildManager::GuildManager()
{
    Clear();
}

// ==================================================
// 析构函数
// ==================================================
GuildManager::~GuildManager()
{
    
}

// ==================================================
// 清空
// ==================================================
void GuildManager::Clear()
{
}

// ==================================================
// 接收网络消息
// ==================================================
void GuildManager::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_GUILD_APPLY_LIST_REQ:                  // 公会申请列表请求
        OnGuildApplyListReq(playerID, data);
        break;
    case MSG_GUILD_LIST_REQ:                        // 公会列表信息请求
        OnGuildListReq(playerID, data);
        break;
    case MSG_CREATE_GUILD_REQ:                      // 创建公会请求
        OnCreateGuildReq(playerID, data);
        break;
    case MSG_DESTROY_GUILD_REQ:                     // 销毁公会请求
        OnDestroyGuildReq(playerID);
        break;
    case MSG_INVITE_JOIN_GUILD_REQ:                 // 邀请加入公会请求
        OnInviteJoinGuildReq(playerID, data);
        break;
    case MSG_INVITE_JOIN_GUILD_ANS:                 // 邀请加入公会应答
        OnInviteJoinGuildAns(playerID, data);
        break;
    case MSG_APPLY_JOIN_GUILD_REQ:                  // 申请加入公会请求
        OnApplyJoinGuildReq(playerID, data);
        break;
    case MSG_APPLY_JOIN_GUILD_ANS:                  // 申请加入公会应答
        OnApplyJoinGuildAns(playerID, data);
        break;
    case MSG_GUILD_MEMBER_DEL_REQ:                  // 删除公会成员请求
        OnGuildMemberDelReq(playerID, data);
        break;
    case MSG_GUILD_SET_OFFICE_REQ:                  // 设置公会职位请求
        OnGuildSetOfficeReq(playerID, data);
        break;
    case MSG_GUILD_SET_FLAG_REQ:                    // 设置公会标记请求
        OnGuildSetFlagReq(playerID, data);
        break;
    case MSG_GUILD_INHERIT_ATEVENT_REQ:             // 继承会长
        OnGuildInheritAteventReq(playerID, data);
        break;
    case MSG_GUILD_DONATE_REQ:                      // 公会捐献请求
        OnGuildDonateReq(playerID, data);
        break;
    case MSG_GUILD_ADD_GUILDPOINT_NTF:              // 公会贡献点增加累积到总贡献点中
        OnGuildAddGuildPointNtf(playerID, data);
        break;
    case MSG_GUILD_SET_BULLETIN_REQ:                // 设置公会公告
        OnGuildBulletinReq(playerID, data);
        break;
    case MSG_GUILD_BOSS_SET_TIME_REQ:               // 设置公会BOSS时间
        OnGuildBossSetTimeReq(playerID, data);
        break;
    case MSG_GUILD_CRUSADE_BOSS_TIME_SAVE:          // 保存公会BOSS时间
        OnGuildCrusadeBossTimeSave(playerID, data);
        break;
    case MSG_GUILD_ADDITION_PROMOTE_TO_CENTER:
        GuildScienceManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
        break;
    //case MSG_GUILD_UPDATE_INFO:
        //OnUpdateGuildInfo(playerID, data);
        //break;
    default:
        break;
    }
}

// ==================================================
// 更新公会信息
// ==================================================
void GuildManager::LoadGuildInfo(GUILD_INFO* guildInfo)
{
    GuildMap::iterator guildIt;
    uint64 guildID = 0;
    uint64 memberID = 0;
    Player* member = 0;

    if(!guildInfo)
        return;

    // 设置中心服务器ID
    guildInfo->center_server_id = ConfigLoader::Instance()->GetCenterID();

    guildID = U64ID(guildInfo->guild_id_h, guildInfo->guild_id_l);
    guildIt = m_guildMap.find(guildID);
    if(guildIt == m_guildMap.end())
    {
        m_guildMap[guildID] = *guildInfo;
    }
    else
    {
        return;
    }

    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildInfo->member_list.begin();
        memberIt != guildInfo->member_list.end(); ++memberIt)
    {
        memberID = U64ID(memberIt->player_id_h, memberIt->player_id_l);
        member = PlayerManager::Instance()->GetPlayerByPlayerID(memberID);
        if(member)
        {
            memberIt->is_online = GUILD_MEMBER_ONLINE_FLAG;         // 成员在线标记
        }
    }

    // 发送给公会所有成员
    SendToAllGuildMember(guildInfo, MSG_GUILD_INFO_NTF, guildInfo);

    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, guildInfo);
}

void GuildManager::SetMemberProp(uint64 playerID)
{
    GUILD_INFO sendMsg;

    Player* guildMaster = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!guildMaster)
    {
        return;
    }

    // 检查是否存在公会
    if(!guildMaster->GetGuildID())
        return;

    GUILD_INFO* guildInfo = GuildManager::Instance()->GetGuildInfo(guildMaster->GetGuildID());
    if(!guildInfo)
    {
        return;
    }

    bool b_find = false;
    std::vector<GUILD_MEMBER>::iterator memberIt;
    for(memberIt = guildInfo->member_list.begin();
        memberIt != guildInfo->member_list.end(); ++memberIt)
    {
       uint64 memberID = U64ID(memberIt->player_id_h, memberIt->player_id_l);
       if(memberID == playerID)
       {
            memberIt->level = guildMaster->GetLevel();
            //memberIt->battle_power = guildMaster->GetBattlePower();
            memberIt->name = guildMaster->GetName();
            b_find = true;
                break;
       }
      
    }

    if(!b_find)
        return;

    CalGuildBattlePower(guildInfo);

    // 发送给 新的战斗力
    FillGuildUpdateInfo(guildInfo, sendMsg);

    // 成员列表
    sendMsg.member_list.push_back(*memberIt);

    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, &sendMsg);
    SendToAllGuildMember(guildInfo, MSG_GUILD_INFO_NTF, &sendMsg);
}

// ==================================================
// 设置公会成员职位
// ==================================================
void GuildManager::SetGuildMemberOfficeNoLock(uint64 guildID, uint64 playerID, int32 office)
{
    GUILD_MEMBER* playerMemberInfo = GetGuildMember(guildID, playerID);
    if(!playerMemberInfo)
        return;

    playerMemberInfo->office = office;
    GuildJob::Instance()->SetGuildMemberOfficeNoLock(guildID, playerID, office);
}

// ==================================================
// 得到公会加成信息
// ==================================================
std::vector<GUILD_ADDITION>* GuildManager::GetScienceAdditionList(uint64 guildID)
{
    GUILD_INFO* guildInfo = GetGuildInfo(guildID);
    if(!guildInfo)
        return NULL;

    return &guildInfo->addition_list;
}

// ==================================================
// 公会成员在线状态
// ==================================================
void GuildManager::SetGuildMemberOnline(uint64 guildID, uint64 playerID)
{
    GUILD_INFO sendMsg;
    GUILD_INFO* guildInfo = 0;
    uint64 memberID = 0;

    // 查找公会
    guildInfo = GetGuildInfo(guildID);
    if(!guildInfo)
        return;

    FillGuildUpdateInfo(guildInfo, sendMsg);

    // 查找角色并设置在线状态
    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildInfo->member_list.begin();
        memberIt != guildInfo->member_list.end(); ++memberIt)
    {
        memberID = U64ID(memberIt->player_id_h, memberIt->player_id_l);
        if(memberID != playerID)
            continue;
        memberIt->is_online = GUILD_MEMBER_ONLINE_FLAG;
        sendMsg.member_list.push_back(*memberIt);
        break;
    }

    if(sendMsg.member_list.size() < 1)
        return;

    // 向所有成员广播
    SendToAllGuildMember(guildInfo, MSG_GUILD_INFO_NTF, &sendMsg, playerID);

    // 想所有GameServer广播
    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, &sendMsg);
}

// ==================================================
// 公会成员离线状态
// ==================================================
void GuildManager::SetGuildMemberOffline(uint64 guildID, uint64 playerID)
{
    GUILD_INFO sendMsg;
    GUILD_INFO* guildInfo = 0;
    uint64 memberID = 0;

    // 查找公会
    guildInfo = GetGuildInfo(guildID);
    if(!guildInfo)
    {
        return;
    }

    FillGuildUpdateInfo(guildInfo, sendMsg);

    // 查找角色并设置在线状态
    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildInfo->member_list.begin();
        memberIt != guildInfo->member_list.end(); ++memberIt)
    {
        memberID = U64ID(memberIt->player_id_h, memberIt->player_id_l);
        if(memberID != playerID) continue;
        memberIt->is_online = GUILD_MEMBER_OFFLINE_FLAG;
        memberIt->last_online = (uint32)Crown::SDNow().GetTimeValue();  // 偷懒 最好统一去数据库取一遍
        sendMsg.member_list.push_back(*memberIt);
        break;
    }

    if(sendMsg.member_list.size() < 1) return;

    // 向所有成员广播
    SendToAllGuildMember(guildInfo, MSG_GUILD_INFO_NTF, &sendMsg, playerID);

    // 想所有GameServer广播
    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, &sendMsg);
}

// ==================================================
// 获取公会信息
// ==================================================
GUILD_INFO* GuildManager::GetGuildInfo(uint64 guildID)
{
    GuildMap::iterator it;

    it = m_guildMap.find(guildID);
    if(it == m_guildMap.end())
    {
        return NULL;
    }

    CnAssert(it->second.center_server_id);

    return &(it->second);
}

// ==================================================
// 通知角色的公会ID
// ==================================================
void GuildManager::SendGuildID(Player* player)
{
    GUILD_ID_NTF sendMsg;
    if(!player)
        return;

    sendMsg.guild_id_h = U64ID_HIGH(player->GetGuildID());
    sendMsg.guild_id_l = U64ID_LOW(player->GetGuildID());

    player->SendToGate(MSG_GUILD_ID_NTF, &sendMsg);
    player->SendToGame(MSG_GUILD_ID_NTF, &sendMsg);
}

// ==================================================
// 设置职位ID
// ==================================================
void GuildManager::CBSetMemberOffice(uint64 guildID, uint64 playerID, int32 office)
{
    GUILD_INFO sendMsg;
    GUILD_INFO* guildInfo = NULL;
    uint64 memberID = 0;

    // 查找公会
    guildInfo = GetGuildInfo(guildID);
    if(!guildInfo)
    {
        return;
    }

    FillGuildUpdateInfo(guildInfo, sendMsg);

    if(guildInfo->lock > 0)
    {
        --guildInfo->lock;
    }

    // 查找角色并设置职位
    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildInfo->member_list.begin();
        memberIt != guildInfo->member_list.end(); ++memberIt)
    {
        memberID = U64ID(memberIt->player_id_h, memberIt->player_id_l);
        if(memberID != playerID) continue;
        memberIt->office = office;
        sendMsg.member_list.push_back(*memberIt);
        break;
    }

    if(sendMsg.member_list.size() < 1) 
        return;

    // 向所有成员广播
    SendToAllGuildMember(guildInfo, MSG_GUILD_INFO_NTF, &sendMsg);

    // 想所有GameServer广播
    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, &sendMsg);

    // 设置成员职位日志
    GuildLog(guildID, GUILD_LOG_CHANGE_OFFICE, playerID, office);
}

// ==================================================
// 设置职位ID无锁
// ==================================================
void GuildManager::CBSetMemberOfficeNoLock(uint64 guildID, uint64 playerID, int32 office)
{
    // 查找公会
    GUILD_INFO* guildInfo = GetGuildInfo(guildID);
    if(!guildInfo)
    {
        return;
    }

    GUILD_INFO sendMsg;
    sendMsg = *guildInfo;
    sendMsg.opt_flag = OPT_UPDATE_FLAG;

    // 向所有成员广播
    SendToAllGuildMember(guildInfo, MSG_GUILD_INFO_NTF, &sendMsg);

    // 想所有GameServer广播
    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, &sendMsg);
}

// ==================================================
// 添加公会总贡献点
// ==================================================
void GuildManager::AddGuildPointToContributeValue(uint64 guildID, uint64 playerID, int32 guildPoint)
{
    GUILD_INFO sendMsg;
    GUILD_INFO* guildInfo = 0;
    uint64 memberID = 0;

    // 查找公会
    guildInfo = GetGuildInfo(guildID);
    if(!guildInfo)
    {
        return;
    }

    FillGuildUpdateInfo(guildInfo, sendMsg);

    // 查找角色并设置职位
    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildInfo->member_list.begin();
        memberIt != guildInfo->member_list.end(); ++memberIt)
    {
        memberID = U64ID(memberIt->player_id_h, memberIt->player_id_l);
        if(memberID != playerID) continue;

        memberIt->contribute_value += guildPoint;
        sendMsg.member_list.push_back(*memberIt);
        break;
    }

    if(sendMsg.member_list.size() < 1) 
        return;

    // 向所有成员广播
    SendToAllGuildMember(guildInfo, MSG_GUILD_INFO_NTF, &sendMsg);

    // 想所有GameServer广播
    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, &sendMsg);
}

// ==================================================
// 删除公会成员
// ==================================================
void GuildManager::DelGuildMember(uint64 guildID, uint64 playerID)
{
    GUILD_INFO sendMsg;
    GUILD_INFO* guildInfo = 0;
    uint64 memberID = 0;

    guildInfo = GetGuildInfo(guildID);
    if(!guildInfo)
    {
        return;
    }

    // 从公会成员中删除此角色
    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildInfo->member_list.begin();
        memberIt != guildInfo->member_list.end(); ++memberIt)
    {
        memberID = U64ID(memberIt->player_id_h, memberIt->player_id_l);
        if(memberID != playerID) continue;
        sendMsg.member_list.push_back(*memberIt);
        guildInfo->member_list.erase(memberIt);
        break;
    }

    // 重新计算公会战斗力
    CalGuildBattlePower(guildInfo);

    // 填充公会信息
    FillGuildUpdateInfo(guildInfo, sendMsg, OPT_DEL_FLAG);

    // 更新公会战斗力
    GuildJob::Instance()->SetMemberCountAndBattlpower(guildID, guildInfo->member_list.size(), guildInfo->battle_power);

    if(sendMsg.member_list.size() < 1)
        return;

    // 向所有成员广播
    SendToAllGuildMember(guildInfo, MSG_GUILD_INFO_NTF, &sendMsg);

    // 想所有GameServer广播
    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, &sendMsg);
}

// ==================================================
// 添加公会成员
// ==================================================
void GuildManager::AddGuildMember(uint64 guildID, GUILD_MEMBER& guildMember)
{
    GUILD_INFO sendMsg;
    GUILD_INFO* guildInfo = 0;
    uint64 memberID = 0;
    Player* member = 0;

    guildInfo = GetGuildInfo(guildID);
    if(!guildInfo)
    {
        return;
    }

    // 添加成员
    guildInfo->member_list.push_back(guildMember);

    //计算战斗力
    CalGuildBattlePower(guildInfo);

    // 填充公会信息
    FillGuildUpdateInfo(guildInfo, sendMsg, OPT_ADD_FLAG);

    // 添加公会成员
    sendMsg.member_list.push_back(guildMember);

    // 广播消息给成员
    SendToAllGuildMember(guildInfo, MSG_GUILD_INFO_NTF, &sendMsg);

    // 广播消息给服务器
    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, &sendMsg);
}

void GuildManager::CalGuildBattlePower(GUILD_INFO* guildInfo)
{
    uint64 guildID = 0;

    if(!guildInfo)
    {
        return;
    }

    guildID = U64ID(guildInfo->guild_id_h, guildInfo->guild_id_l);

    // 重新计算公会战斗力
    guildInfo->battle_power = 0;
    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildInfo->member_list.begin();
        memberIt != guildInfo->member_list.end(); ++memberIt)
    {
        guildInfo->battle_power += memberIt->battle_power;
    }

    // 更新公会战斗力
    GuildJob::Instance()->SetMemberCountAndBattlpower(guildID, guildInfo->member_list.size(), guildInfo->battle_power);
}
// ==================================================
// 通知公会信息
// ==================================================
void GuildManager::SendGuildInfoToGate(Player* player)
{
    GuildMap::iterator guildIt;
    uint64 memberID = 0;

    if(!player) return;

    guildIt = m_guildMap.find(player->GetGuildID());
    if(guildIt == m_guildMap.end())
    {
        return;
    }
    GUILD_INFO sendMsg;
    sendMsg = guildIt->second;
    sendMsg.opt_flag = OPT_INIT_FLAG;

    player->SendToGate(MSG_GUILD_INFO_NTF, &sendMsg);
}

// ==================================================
// 添加公会信息
// ==================================================
void GuildManager::CreateGuildInfo(uint64 guildID, const std::string& guildName, Player* player)
{
    GUILD_INFO guildInfo;
    GUILD_MEMBER guildMember;
    GuildMap::iterator guildIt;

    if(!player) return;

    // 设置公会信息
    guildInfo.Clear();
    guildInfo.center_server_id = ConfigLoader::Instance()->GetCenterID();
    guildInfo.opt_flag = OPT_INIT_FLAG;
    guildInfo.guild_id_h = U64ID_HIGH(guildID);
    guildInfo.guild_id_l = U64ID_LOW(guildID);
    guildInfo.guild_name = guildName;
    guildInfo.level = 1;
    guildInfo.exp = 0;
    guildInfo.flag = 0;
    guildInfo.guild_boss_level = 1;
    guildInfo.guild_boss_exp = 0;
    guildInfo.guild_boss_start_time = GuildConfig::Instance()->GetCrusadeStartTime(1);
    guildInfo.battle_power = 0; //player->GetBattlePower();

    const std::vector<GuildAdditionTmpl>& guildAddtionVert = GuildScienceConfig::Instance()->GetGuildAdditions();
    for(std::vector<GuildAdditionTmpl>::const_iterator guildAddtionIt = guildAddtionVert.begin(); guildAddtionIt != guildAddtionVert.end(); ++guildAddtionIt)
    {

        GUILD_ADDITION default_addition;
        default_addition.id = guildAddtionIt->id;
        guildInfo.addition_list.push_back(default_addition);
    }

    // 设置公会成员信息
    guildMember.Clear();
    guildMember.gatesvr_id = player->GetCentGateID();
    guildMember.player_id_h = U64ID_HIGH(player->GetPlayerID());
    guildMember.player_id_l = U64ID_LOW(player->GetPlayerID());
    guildMember.office = GUILD_OFFICE_CHAIRMAN;                            // 公会职位
    guildMember.name = player->GetName();                           // 名称
    guildMember.sex = player->GetSex();                             // 性别
    guildMember.career = player->GetCareer();                       // 职业
    guildMember.level = player->GetLevel();                         // 等级
    guildMember.battle_power = //player->GetBattlePower();            // 战斗力
    guildMember.is_online = GUILD_MEMBER_ONLINE_FLAG;               // 在线标记

    guildInfo.member_list.push_back(guildMember);

    guildIt = m_guildMap.find(guildID);
    if(guildIt != m_guildMap.end())
    {
        CnAssert(false);
        return;
    }

    m_guildMap[guildID] = guildInfo;

    // 广播所有GameServer公会信息
    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, &guildInfo);

    // 发送公会信息
    player->SendToGate(MSG_GUILD_INFO_NTF, &guildInfo);
}

// ==================================================
// 发送创建公会成功的消息
// ==================================================
void GuildManager::SendCreateGuildAck(Player* player)
{
    NONE sendMsg;

    if(!player) return;

    player->SendToGame(MSG_CREATE_GUILD_ACK, &sendMsg);
}

// ==================================================
// 删除公会信息
// ==================================================
void GuildManager::DelGuildInfo(uint64 guildID)
{
    GUILD_INFO guildInfo;
    GuildMap::iterator guildIt;
    uint64 memberID = 0;
    Player* member = 0;

    guildInfo.Clear();
    guildInfo.guild_id_h = U64ID_HIGH(guildID);
    guildInfo.guild_id_l = U64ID_LOW(guildID);
    guildInfo.opt_flag = OPT_INIT_FLAG;

    guildIt = m_guildMap.find(guildID);
    if(guildIt == m_guildMap.end())
    {
        return;
    }

    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildIt->second.member_list.begin();
        memberIt != guildIt->second.member_list.end(); ++memberIt)
    {
        memberID = U64ID(memberIt->player_id_h, memberIt->player_id_l);
        member = PlayerManager::Instance()->GetPlayerByPlayerID(memberID);
        if(!member) continue;
        member->SetGuildID(0);
        SendGuildID(member);    // 通知角色的公会ID
        member->SendToGate(MSG_GUILD_INFO_NTF, &guildInfo);
    }

    m_guildMap.erase(guildIt);

    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, &guildInfo);
}

// ==================================================
// 是否公会长
// ==================================================
bool GuildManager::IsMaster(Player* player)
{
    GuildMap::iterator guildIt;
    uint64 playerID = 0;
    uint64 guildID = 0;

    if(!player)
    {
        return false;
    }

    guildID = player->GetGuildID();
    guildIt = m_guildMap.find(guildID);
    if(guildIt == m_guildMap.end())
    {
        return false;
    }

    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildIt->second.member_list.begin();
        memberIt != guildIt->second.member_list.end(); ++memberIt)
    {
        playerID = U64ID(memberIt->player_id_h, memberIt->player_id_l);
        if(playerID == player->GetPlayerID())
        {
            if(memberIt->office == GUILD_OFFICE_CHAIRMAN)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    return false;
}

// ==================================================
// 检查是否已存在此职位
// ==================================================
bool GuildManager::HasOffice(GUILD_INFO* guildInfo, int32 office)
{
    if(!guildInfo)
    {
        return false;
    }

    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildInfo->member_list.begin();
        memberIt != guildInfo->member_list.end(); ++memberIt)
    {
        if(memberIt->office == office)
        {
            return true;
        }
    }

    return false;
}

// ==================================================
// 公会是否存在此成员
// ==================================================
bool GuildManager::HasGuildMember(uint64 guildID, uint64 playerID)
{
    GuildMap::iterator guildIt;
    uint64 memberID = 0;

    guildIt = m_guildMap.find(guildID);
    if(guildIt == m_guildMap.end())
    {
        return false;
    }

    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildIt->second.member_list.begin();
        memberIt != guildIt->second.member_list.end(); ++memberIt)
    {
        memberID = U64ID(memberIt->player_id_h, memberIt->player_id_l);
        if(memberID == playerID)
        {
            return true;
        }
    }

    return false;
}

// ==================================================
// 获得公会成员信息
// ==================================================
GUILD_MEMBER* GuildManager::GetGuildMember(uint64 guildID, uint64 playerID)
{
    GuildMap::iterator guildIt;
    uint64 memberID = 0;

    guildIt = m_guildMap.find(guildID);
    if(guildIt == m_guildMap.end())
    {
        return 0;
    }

    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildIt->second.member_list.begin();
        memberIt != guildIt->second.member_list.end(); ++memberIt)
    {
        memberID = U64ID(memberIt->player_id_h, memberIt->player_id_l);
        if(memberID == playerID)
        {
            return &(*memberIt);
        }
    }

    return 0;
}

// ==================================================
// 由职位获得公会成员
// ==================================================
std::vector<GUILD_MEMBER*> GuildManager::GetGuildMemberByOffice(uint64 guildID, uint32 office)
{
    std::vector<GUILD_MEMBER*> guildMemberList;
    guildMemberList.clear();

    GuildMap::iterator guildIt = m_guildMap.find(guildID);
    if(guildIt == m_guildMap.end())
        return guildMemberList;

    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildIt->second.member_list.begin(); 
        memberIt != guildIt->second.member_list.end(); ++ memberIt)
    {
        if(memberIt->office == office)
        {
            guildMemberList.push_back(&(*memberIt));
        }
    }

    return guildMemberList;
}

// ==================================================
// 发送添加申请加入公会通知
// ==================================================
void GuildManager::SendAddApplyNtf(uint64 guildID)
{
    NONE sendMsg;
    GUILD_INFO* guildInfo = 0;

    guildInfo = GetGuildInfo(guildID);
    if(!guildInfo)
    {
        return;
    }

    // 通知所有公会成员
    SendToAllGuildMember(guildInfo, MSG_APPLY_INFO_NTF, &sendMsg);
}

// ==================================================
// 设置公会标记
// ==================================================
void GuildManager::SetGuildFlag(uint64 guildID, uint32 flag)
{
    GUILD_INFO sendMsg;
    GUILD_INFO* guildInfo = 0;

    guildInfo = GetGuildInfo(guildID);
    if(!guildInfo)
    {
        return;
    }

    // 设置公会标记
    guildInfo->flag = flag;

    // 设置要发送的公会信息
    FillGuildUpdateInfo(guildInfo, sendMsg);

    // 通知所有成员更新消息
    SendToAllGuildMember(guildInfo, MSG_GUILD_INFO_NTF, &sendMsg);

    // 通知所有游戏服务器更新消息
    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, &sendMsg);

    // 设置公会标记
    GuildLog(guildID, GUILD_LOG_CHANGE_FLAG, flag);
}

// ==================================================
// 生成公会ID
// ==================================================
uint64 GuildManager::GenGuildID()
{
    static uint16 serialNum = 0;
    // 时间戳 32bit, 序列号16bit, 
    uint32 high = (uint32)time(0);
    uint32 low = (++serialNum << 16);
    low |= ConfigLoader::Instance()->GetRegionID() & 0xFFFF;   // 区ID

    uint64 sn = high;
    sn = sn << 32;
    sn += low;

    return sn;
}

// ==================================================
// 得到公会名
// ==================================================
std::string GuildManager::GetGuildName(uint64 guildID)
{
    GUILD_INFO* guildInfo = 0;

    if(!guildID)
    {
        return "";
    }

    guildInfo = GuildManager::Instance()->GetGuildInfo(guildID);

    if(guildInfo)
    {
        return guildInfo->guild_name;
    }

    return "";
}

// ==================================================
// 请求公会申请列表
// ==================================================
void GuildManager::OnGuildApplyListReq(uint64 playerID, void* data)
{
    Player* player = 0;

    // 获得角色指针
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    // 检查角色是否有公会
    if(!player->GetGuildID())
    {
        return;
    }

    GuildJob::Instance()->GetGuildApplyList(playerID, player->GetGuildID());
}

// ==================================================
// 公会列表信息请求
// ==================================================
void GuildManager::OnGuildListReq(uint64 playerID, void* data)
{
    GUILD_LIST_REQ* recvMsg = (GUILD_LIST_REQ*)data;

    if(!recvMsg) return;

    GuildJob::Instance()->GetGuildList(playerID, recvMsg->page);
}

// ==================================================
// 销毁队伍请求
// ==================================================
void GuildManager::OnDestroyGuildReq(uint64 playerID)
{
    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    GuildJob::Instance()->DestroyGuild(player);
}

// ==================================================
// 创建队伍请求
// ==================================================
void GuildManager::OnCreateGuildReq(uint64 playerID, void* data)
{
    CREATE_GUILD_REQ* recvMsg = (CREATE_GUILD_REQ*)data;
    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    if(player->GetGuildID())
    {
        player->NotifyErrorCode(ERR_HAVE_GUILD);
        return;
    }

    // 加服务器前缀名
    recvMsg->guild_name = ConfigLoader::Instance()->GetServerPreName() + recvMsg->guild_name;

    GuildJob::Instance()->CheckGuildExist(recvMsg->guild_name, player->GetPlayerID());
}

// ==================================================
// 邀请加入公会请求
// ==================================================
void GuildManager::OnInviteJoinGuildReq(uint64 playerID, void* data)
{
    INVITE_JOIN_GUILD_REQ* recvMsg = (INVITE_JOIN_GUILD_REQ*)data;
    const GuildLevelTmpl* guildLevelTmpl = 0;
    GUILD_INFO* guildInfo = 0;
    GUILD_MEMBER* guildMember = 0;
    Player* guildMaster = 0;
    Player* receivePlayer = 0;

    if(!recvMsg) return;

    // 获得公会请求者指针
    guildMaster = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!guildMaster)
    {
        return;
    }
    recvMsg->inviter_player_name = guildMaster->GetName();

    // 检查是否存在公会
    if(!guildMaster->GetGuildID())
    {
        return;
    }

    // 检查请求者在公会中的职位
    guildMember = GuildManager::Instance()->GetGuildMember(guildMaster->GetGuildID(), guildMaster->GetPlayerID());
    if(!guildMaster || (guildMember->office > GUILD_OFFICE_ELITE))
    {
        return;
    }

    // 获得公会数据
    guildInfo = GuildManager::Instance()->GetGuildInfo(guildMaster->GetGuildID());
    if(!guildInfo)
    {
        return;
    }

    // 获得工会模板
    guildLevelTmpl = GuildConfig::Instance()->GetGuildLeveTmpl(guildInfo->level);
    if(!guildLevelTmpl)
    {
        return;
    }

    // 检查公会人数
    if((int32)guildInfo->member_list.size() >= guildLevelTmpl->max_member)
    {
        guildMaster->NotifyErrorCode(ERR_MAX_GUILD_MEMBERNUM);
        return;
    }

    // 获得被邀请者指针
    receivePlayer = PlayerManager::Instance()->GetPlayerByName(recvMsg->player_name);
    if(!receivePlayer)
    {
        guildMaster->NotifyErrorCode(ERR_PLAYER_NOTONLINE);
        return;
    }

    if(receivePlayer->GetGuildID())
    {
        guildMaster->NotifyErrorCode(ERR_INVITE_PLAYER_HAVE_GUILD);
        return;
    }

    // 复制公会数据
    recvMsg->guild_id_h = guildInfo->guild_id_h;
    recvMsg->guild_id_l = guildInfo->guild_id_l;
    recvMsg->guild_name = guildInfo->guild_name;
    // 复制邀请者的PlayerID和GateID
    recvMsg->inviter_player_id.id_h = U64ID_HIGH(playerID);
    recvMsg->inviter_player_id.id_l = U64ID_LOW(playerID);
    recvMsg->inviter_gate_id = guildMaster->GetCentGateID();

    receivePlayer->SendToGate(MSG_INVITE_JOIN_GUILD_CFM, recvMsg);
}

// ==================================================
// 邀请加入公会应答
// ==================================================
void GuildManager::OnInviteJoinGuildAns(uint64 playerID, void* data)
{
    INVITE_JOIN_GUILD_ACK* recvMsg = (INVITE_JOIN_GUILD_ACK*)data;
    const GuildLevelTmpl* guildLevelTmpl = 0;
    GUILD_INFO* guildInfo = 0;
    uint64 guildID = 0;
    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player || !recvMsg)
    {
        return;
    }

    // 临时检查一下此玩家是否已加入了其他的公会
    // 还是要通过 guild_member 表的操作来保证原子性
    if(player->GetGuildID())
    {
        return;
    }

    if(recvMsg->errcode != ERR_SUCCEED)
    {
        return;
    }

    guildID = U64ID(recvMsg->guild_id_h, recvMsg->guild_id_l);
    if(!guildID)
    {
        return;
    }

    guildInfo = GuildManager::Instance()->GetGuildInfo(guildID);
    if(!guildInfo)
    {
        return;
    }

    // 获得工会模板
    guildLevelTmpl = GuildConfig::Instance()->GetGuildLeveTmpl(guildInfo->level);
    if(!guildLevelTmpl)
    {
        return;
    }

    // 检查公会人数
    if((int32)guildInfo->member_list.size() >= guildLevelTmpl->max_member)
    {
        player->NotifyErrorCode(ERR_MAX_GUILD_MEMBERNUM);
        return;
    }

    // 将用户加入公会中
    GuildJob::Instance()->CheckPlayerJoinGuild(playerID, playerID, guildID);
}

// ==================================================
// 申请加入公会请求
// ==================================================
void GuildManager::OnApplyJoinGuildReq(uint64 playerID, void* data)
{
    APPLY_JOIN_GUILD_REQ* recvMsg = (APPLY_JOIN_GUILD_REQ*)data;
    const GuildLevelTmpl* guildLevelTmpl = 0;
    GUILD_INFO* guildInfo = 0;
    Player* player = 0;
    uint64 guildID = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player || !recvMsg)
    {
        return;
    }

    if(player->GetGuildID())
    {
        return;
    }

    guildID = U64ID(recvMsg->guild_id_h, recvMsg->guild_id_l);

    guildInfo = GuildManager::Instance()->GetGuildInfo(guildID);
    if(guildInfo)
    {
        // 获得工会模板
        guildLevelTmpl = GuildConfig::Instance()->GetGuildLeveTmpl(guildInfo->level);
        if(!guildLevelTmpl)
        {
            return;
        }

        // 检查公会人数
        if((int32)guildInfo->member_list.size() >= guildLevelTmpl->max_member)
        {
            player->NotifyErrorCode(ERR_MAX_GUILD_MEMBERNUM);
            return;
        }
    }

    GuildJob::Instance()->JoinGuildCheckFlag(guildID, playerID);
}

// ==================================================
// 申请加入公会应答
// ==================================================
void GuildManager::OnApplyJoinGuildAns(uint64 playerID, void* data)
{
    APPLY_JOIN_GUILD_ACK* recvMsg = (APPLY_JOIN_GUILD_ACK*)data;
    Player* player = 0;
    uint64 applyPlayerID = 0;
    uint64 guildID = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player || !recvMsg)
    {
        return;
    }

    applyPlayerID = U64ID(recvMsg->player_id_h, recvMsg->player_id_l);

    if(!player->GetGuildID())
    {
        return;
    }

    GuildJob::Instance()->DelGuildApply(player->GetGuildID(), applyPlayerID);

    if(recvMsg->errcode == ERR_SUCCEED)
    {
        GuildJob::Instance()->CheckPlayerJoinGuild(applyPlayerID, playerID, player->GetGuildID());
    }
}


// ==================================================
// 删除公会成员请求
// ==================================================
void GuildManager::OnGuildMemberDelReq(uint64 playerID, void* data)
{
    GUILD_MEMBER_DEL_REQ* recvMsg = (GUILD_MEMBER_DEL_REQ*)data;
    GUILD_MEMBER* guildMember = 0;
    uint64 delMemberID = 0;
    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player || !recvMsg)
    {
        return;
    }

    // 获得公会成员结构
    guildMember = GetGuildMember(player->GetGuildID(), player->GetPlayerID());
    if(!guildMember)
    {
        return;
    }

    // 获得删除的角色ID
    delMemberID = U64ID(recvMsg->player_id_h, recvMsg->player_id_l);

    // 检查删除的是否为自己
    if(delMemberID == player->GetPlayerID())
    {
        if(guildMember->office <= GUILD_OFFICE_CHAIRMAN)
        {
            return;
        }
    }
    else
    {
        // 检查职位
        if(guildMember->office > GUILD_OFFICE_VICE_CHAIRMAN)
        {
            return;
        }
    }

    // 检查玩家离开公会
    GuildJob::Instance()->CheckPlayerLeaveGuild(player->GetPlayerID(), guildMember->office, player->GetGuildID(), delMemberID);
}

// ==================================================
// 设置公会会员职位请求
// ==================================================
void GuildManager::OnGuildSetOfficeReq(uint64 playerID, void* data)
{
    GUILD_SET_OFFICE_REQ* recvMsg = (GUILD_SET_OFFICE_REQ*)data;
    GUILD_INFO* guildInfo = 0;
    GUILD_MEMBER* guildMemberA = 0;
    GUILD_MEMBER* guildMemberB = 0;
    uint64 memberID = 0;
    uint64 guildID = 0;
    Player* player = 0;
    uint16 errcode = ERR_FAILED;
    GUILD_NOTICE_ACK noticeMsg;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player || !recvMsg)
    {
        return;
    }

    guildID = player->GetGuildID();
    memberID = U64ID(recvMsg->player_id_h, recvMsg->player_id_l);

    // 获得公会结构
    guildInfo = GetGuildInfo(guildID);
    if(!guildInfo)
    {
        goto _ERROR;
    }

    if(guildInfo->lock)
    {
        goto _ERROR;        // 成员职位变更操作尚未完成
    }

    // 获得公会成员结构
    guildMemberA = GetGuildMember(guildID, playerID);
    if(!guildMemberA)
    {
        goto _ERROR;
    }

    // 是否存在此成员
    if(!HasGuildMember(guildID, memberID))
    {
        goto _ERROR;
    }

    // 最后检查设置职位的有效范围
    if((recvMsg->office < GUILD_OFFICE_CHAIRMAN) || (recvMsg->office > GUILD_OFFICE_MEMBER))
    {
        goto _ERROR;
    }

    // 只有公会长和副公会长才能设置别人的职位
    if(guildMemberA->office > GUILD_OFFICE_VICE_CHAIRMAN)
    {
        goto _ERROR;
    }

    // 检查是否将玩家升级为公会会长
    if(recvMsg->office == GUILD_OFFICE_CHAIRMAN)
    {
        // 检查当前玩家是否为会长
        if(guildMemberA->office != GUILD_OFFICE_CHAIRMAN)
        {
            goto _ERROR;
        }

        // 获得被设置公会成员结构
        guildMemberB = GetGuildMember(guildID, memberID);
        if(!guildMemberB)
        {
            goto _ERROR;
        }

        // 检查被设置公会成员是否为副会长
        if(guildMemberB->office != GUILD_OFFICE_VICE_CHAIRMAN)
        {
            goto _ERROR;
        }

        guildInfo->lock += 2;

        // 会长变副会长
        GuildJob::Instance()->SetGuildMemberOffice(guildID, playerID, GUILD_OFFICE_VICE_CHAIRMAN);

        // NOTICE
        noticeMsg.guild_notice_type = GUILD_NOTICE_TYPE_INHERIT;
        noticeMsg.player_name = guildMemberB->name;
        noticeMsg.player_office = guildMemberB->office;
        noticeMsg.player_vip = guildMemberB->vip_info;
        SendToAllGuildMember(guildInfo, MSG_GUILD_NOTICE_ACK, &noticeMsg);

        // 变会长
        GuildJob::Instance()->SetGuildMemberOffice(guildID, memberID, GUILD_OFFICE_CHAIRMAN);
    }
    else
    {
        // 如果设置的是副会长，则检查是否已存在此职位（副会长只有一个）
        if(recvMsg->office == GUILD_OFFICE_VICE_CHAIRMAN)
        {
            if(HasOffice(guildInfo, recvMsg->office))
            {
                errcode = ERR_GUILD_VICE_PRESIDENT_MAX;
                goto _ERROR;
            }
        }

        // 设置的职位，不能比当前设置者的职位还高（数越小越高）
        if(recvMsg->office <= guildMemberA->office)
        {
            goto _ERROR;
        }

        ++guildInfo->lock;

        // NOTICE
        // 获得被设置公会成员结构
        guildMemberB = GetGuildMember(guildID, memberID);
        if(!guildMemberB)
        {
            goto _ERROR;
        }
        if (guildMemberB->office > recvMsg->office)
            noticeMsg.guild_notice_type = GUILD_NOTICE_TYPE_PROMOTE;
        else
            noticeMsg.guild_notice_type = GUILD_NOTICE_TYPE_DEMOTE;
        noticeMsg.player_name = guildMemberB->name;
        noticeMsg.master_name = player->GetName();
        noticeMsg.player_office = recvMsg->office;
        noticeMsg.master_office = guildMemberA->office;
        noticeMsg.player_vip = guildMemberB->vip_info;
        noticeMsg.master_vip = guildMemberA->vip_info;
        SendToAllGuildMember(guildInfo, MSG_GUILD_NOTICE_ACK, &noticeMsg);

        // 改变职位
        GuildJob::Instance()->SetGuildMemberOffice(guildID, memberID, recvMsg->office);
    }

    return;

_ERROR:
    player->NotifyErrorCode(errcode);
}

// ==================================================
// 设置公会标记请求
// ==================================================
void GuildManager::OnGuildSetFlagReq(uint64 playerID, void* data)
{
    GUILD_SET_FLAG_REQ* recvMsg = (GUILD_SET_FLAG_REQ*)data;
    GUILD_MEMBER* guildMember = 0;
    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player || !recvMsg)
    {
        return;
    }

    // 获得公会成员结构
    guildMember = GetGuildMember(player->GetGuildID(), playerID);
    if(!guildMember)
    {
        return;
    }

    if(guildMember->office > GUILD_OFFICE_VICE_CHAIRMAN)
    {
        return;
    }

    // 设置公会标记
    GuildJob::Instance()->SetGuildFlag(player->GetGuildID(), recvMsg->guild_flag);
}

// ==================================================
// 继承会长
// ==================================================
void GuildManager::OnGuildInheritAteventReq(uint64 playerID, void* data)
{
    int32 maxGuildPoint = 0;

    Player* pPlayer = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!pPlayer)
        return;

    // 检查角色是否存在公会
    if(!pPlayer->GetGuildID())
        return;

    // 获取公会
    GUILD_INFO* guildInfo = GetGuildInfo(pPlayer->GetGuildID());
    if(!guildInfo)
        return;

    GUILD_MEMBER* MyMemberInfo = GetGuildMember(pPlayer->GetGuildID(), playerID);
    if(!MyMemberInfo)
        return;

    // 会长是否7天不上线
    std::vector<GUILD_MEMBER*> guildChairmanList = GetGuildMemberByOffice(pPlayer->GetGuildID(), GUILD_OFFICE_CHAIRMAN);
    if(guildChairmanList.size() <= 0)
    {
        CnAssert(false);
        return;
    }
    GUILD_MEMBER* guildChairman = guildChairmanList[0];
    if(guildChairman->is_online)
    {
        pPlayer->NotifyErrorCode(ERR_GUILD_NO_INHERIT_CHAIRMAN);
        return;
    }
    if((Crown::SDNow().GetTimeValue() - guildChairman->last_online) < 7 * DAY_SECONDS)
    {
        pPlayer->NotifyErrorCode(ERR_GUILD_NO_INHERIT_CHAIRMAN);
        return;
    }

    // 你上面是否有职位更高的人
    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildInfo->member_list.begin(); memberIt != guildInfo->member_list.end(); ++memberIt)
    {
        if(memberIt->office == GUILD_OFFICE_CHAIRMAN)
            continue;

        if(memberIt->office < MyMemberInfo->office)
        {
            pPlayer->NotifyErrorCode(ERR_GUILD_NO_INHERIT_CHAIRMAN);
            return;
        }
    }

    // 是否是最高贡献点的人
    std::vector<GUILD_MEMBER*> memberList = GetGuildMemberByOffice(pPlayer->GetGuildID(), MyMemberInfo->office);
    for(std::vector<GUILD_MEMBER*>::iterator memberIt = memberList.begin(); memberIt != memberList.end(); ++memberIt)
    {
        if((*memberIt)->player_id_h == MyMemberInfo->player_id_h && (*memberIt)->player_id_l == MyMemberInfo->player_id_l)
            continue;
        if((*memberIt)->contribute_value > MyMemberInfo->contribute_value)
        {
            pPlayer->NotifyErrorCode(ERR_GUILD_NO_INHERIT_CHAIRMAN);
            return;
        }
    }

    // 变更会长
    uint64 oldguildChairmanID = U64ID(guildChairman->player_id_h, guildChairman->player_id_l);
    SetGuildMemberOfficeNoLock(pPlayer->GetGuildID(), oldguildChairmanID, GUILD_OFFICE_MEMBER);
    SetGuildMemberOfficeNoLock(pPlayer->GetGuildID(), playerID, GUILD_OFFICE_CHAIRMAN);

    pPlayer->NotifyErrorCode(ERR_SUCCEED);
}

// 公会捐献请求
void GuildManager::OnGuildDonateReq(uint64 playerID, void* data)
{
    GUILD_DONATE_REQ* recvMsg = (GUILD_DONATE_REQ*)data;
    GUILD_DONATE_ACK sendGuildDonateAck;
    GUILD_INFO sendGuildInfo;
    Player* player = 0;
    const GuildLevelTmpl* guildLevelTmpl = 0;
    GUILD_INFO* guildInfo = 0;
    int32 addLevel = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player || !recvMsg)
    {
        return;
    }

    // 检查角色是否存在公会
    if(!player->GetGuildID())
    {
        return;
    }

    // 获取公会
    guildInfo = GetGuildInfo(player->GetGuildID());
    if(!guildInfo)
    {
        return;
    }

    // 获取公会的等级模板
    guildLevelTmpl = GuildConfig::Instance()->GetGuildLeveTmpl(guildInfo->level);
    if(!guildLevelTmpl)
    {
        return;
    }

    // 检查公会等级最高时
    if((guildInfo->level >= GuildConfig::Instance()->GetMaxLevel()) && 
        (guildInfo->exp >= guildLevelTmpl->max_exp))
    {
        return;
    }

    // 为公会添加经验
    if (recvMsg->donate_type == PROP_GAMEPOINT)
    {
        guildInfo->exp += (guildLevelTmpl->donate_exp * recvMsg->donate_value * GuildConfig::Instance()->GetGamepointDonateTmp().exp);
    }
    else if (recvMsg->donate_type == PROP_MONEYPOINT)
    {
        guildInfo->exp += (guildLevelTmpl->donate_exp * recvMsg->donate_value * GuildConfig::Instance()->GetMoneypointDonateTmp().exp);
    }

    // 检查公会是否需要提升等级
    while((guildInfo->level < GuildConfig::Instance()->GetMaxLevel()) && 
        (guildInfo->exp >= guildLevelTmpl->max_exp))
    {
        ++guildInfo->level;         // 提升公会等级
        ++addLevel;                 // 增加公会的等级
        guildInfo->exp -= guildLevelTmpl->max_exp;         // 公会经验清零

        // 获取公会的等级模板
        guildLevelTmpl = GuildConfig::Instance()->GetGuildLeveTmpl(guildInfo->level);
        if(!guildLevelTmpl)
            break;
    }

    if(guildInfo->exp >= guildLevelTmpl->max_exp)
        guildInfo->exp = guildLevelTmpl->max_exp;

    // 设置
    GuildJob::Instance()->SetGuildLeveAndExp(player->GetGuildID(), addLevel, guildInfo->exp);

    // 设置要发送的公会信息
    FillGuildUpdateInfo(guildInfo, sendGuildInfo);

    // 发送给公会所有成员
    SendToAllGuildMember(guildInfo, MSG_GUILD_INFO_NTF, &sendGuildInfo);

    // 通知所有的GameServer
    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, &sendGuildInfo);

    // 公会捐献日志
    GuildLog(player->GetGuildID(), GUILD_LOG_DONATE, player->GetPlayerID(), guildInfo->exp);
}

// ==================================================
// 公会贡献点增加累积到总贡献点中
// ==================================================
void GuildManager::OnGuildAddGuildPointNtf(uint64 playerID, void* data)
{
    GUILD_ADD_GUILDPOINT_NTF* recvMsg = (GUILD_ADD_GUILDPOINT_NTF*)data;
    // 获取Player
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player || !recvMsg)
        return;

    // 检查角色是否存在公会
    if(!player->GetGuildID())
        return;

    // 获取公会
    GUILD_INFO* guildInfo = GetGuildInfo(player->GetGuildID());
    if(!guildInfo)
        return;

    GuildJob::Instance()->AddGuildPointToContributeValue(player->GetGuildID(), playerID, recvMsg->guild_point);
}

// 设置公会公告
void GuildManager::OnGuildBulletinReq(uint64 playerID, void* data)
{
    GUILD_SET_BULLETIN_REQ* recvMsg = (GUILD_SET_BULLETIN_REQ*)data;
    GUILD_INFO sendGuildInfo;
    GUILD_MEMBER* guildMember = 0;
    Player* player = 0;
    GUILD_INFO* guildInfo = 0;
    uint16 errCode = ERR_FAILED;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player || !recvMsg)
    {
        return;
    }

    // 检查角色是否存在公会
    if(!player->GetGuildID())
    {
        goto _ERR;
    }

    // 公会成员
    guildMember = GetGuildMember(player->GetGuildID(), player->GetPlayerID());
    if(!guildMember)
    {
        goto _ERR;
    }

    // 获取公会
    guildInfo = GetGuildInfo(player->GetGuildID());
    if(!guildInfo)
    {
        goto _ERR;
    }

    // 检查是否为公会会长
    if(guildMember->office != GUILD_OFFICE_CHAIRMAN)
    {
        goto _ERR;
    }

    // 设置公会公告
    guildInfo->bulletin = recvMsg->bulletin;

    // 保存公告到数据库
    GuildJob::Instance()->SetGuildBulletin(player->GetGuildID(), guildInfo->bulletin);

    // 设置要发送的公会信息
    FillGuildUpdateInfo(guildInfo, sendGuildInfo);

    // 发送给公会所有成员
    SendToAllGuildMember(guildInfo, MSG_GUILD_INFO_NTF, &sendGuildInfo);

    // 通知所有的GameServer
    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, &sendGuildInfo);

    // 修改公会公告日志
    GuildLog(player->GetGuildID(), GUILD_LOG_BULLETIN, player->GetPlayerID());
    return;

_ERR:
    player->NotifyErrorCode(errCode);
}

// 公会BOSS升级请求
void GuildManager::OnGuildBossLevelupReq(uint64 playerID, void* data)
{
    //GUILD_INFO*     guildInfo = 0;
    //GuildBossTmpl*  bossTmpl = 0;
    //Player*         player = 0;
    //uint16 errCode = ERR_FAILED;

    //player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    //if(!player)
    //{
    //    return;
    //}

    //// 获取公会
    //guildInfo = GetGuildInfo(player->GetGuildID());
    //if(!guildInfo)
    //{
    //    goto _ERR;
    //}

    //// 设置公会公告
    //bossTmpl = GuildConfig::Instance()->GetGuildBossTmpl(guildInfo->level);
    //if(!bossTmpl)
    //{
    //    goto _ERR;
    //}

//    return;
//
//_ERR:
//    player->NotifyErrorCode(errCode);
}

// 公会BOSS设置时间请求
void GuildManager::OnGuildBossSetTimeReq(uint64 playerID, void* data)
{
    GUILD_BOSS_SET_TIME_REQ* recvMsg = (GUILD_BOSS_SET_TIME_REQ*)data;
    GUILD_INFO sendGuildInfo;
    GUILD_MEMBER* guildMember = 0;
    Player* player = 0;
    GUILD_INFO* guildInfo = 0;
    uint16 errCode = ERR_FAILED;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player || !recvMsg)
    {
        return;
    }

    // 获取公会
    guildInfo = GetGuildInfo(player->GetGuildID());
    if(!guildInfo)
    {
        goto _ERR;
    }

    // 设置公会BOSS开始时间
    guildInfo->guild_boss_start_time = recvMsg->start_time % DAY_SECONDS;    // 防止出错，所以做了一次取模

    NotifyGuildBossTime(guildInfo);

    // 设置要发送的公会信息
    FillGuildUpdateInfo(guildInfo, sendGuildInfo);

    // 发送给公会所有成员
    SendToAllGuildMember(guildInfo, MSG_GUILD_INFO_NTF, &sendGuildInfo);

    // 通知所有的GameServer
    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, &sendGuildInfo);

    // 设置公会BOSS的时间
    GuildJob::Instance()->SetGuildBossTime(player->GetGuildID(), 
        guildInfo->guild_boss_start_time, guildInfo->guild_boss_create_time, guildInfo->guild_boss_dead_time);

    return;

_ERR:
    player->NotifyErrorCode(errCode);
}

// ==================================================
// 保存公会BOSS时间
// ==================================================
void GuildManager::OnGuildCrusadeBossTimeSave(uint64 playerID, void* data)
{
    GUILD_CRUSADE_BOSS_TIME* recvMsg = (GUILD_CRUSADE_BOSS_TIME*)data;
    Crown::CSDDateTime nowTime = Crown::SDNow();
    GUILD_INFO sendGuildInfo;
    GUILD_INFO* guildInfo = 0;
    uint64 guild_id = 0;

    if(!recvMsg) return;

    guild_id = U64ID(recvMsg->guild_id.id_h, recvMsg->guild_id.id_l);
    if(!guild_id)
    {
        return;
    }

    guildInfo = GetGuildInfo(guild_id);
    if(!guildInfo)
    {
        return;
    }

    if(recvMsg->flag == GUILD_BOSS_CREATE_TIME_FLAG)
    {
        guildInfo->guild_boss_create_time = (uint32)nowTime.GetTimeValue();
    }
    else if(recvMsg->flag == GUILD_BOSS_DEAD_TIME_FLAG)
    {
        guildInfo->guild_boss_dead_time = (uint32)nowTime.GetTimeValue();
    }
    else
    {
        return;
    }

    // 设置新的公会boss时间
    GuildJob::Instance()->SetGuildBossTime(guild_id, guildInfo->guild_boss_start_time, 
        guildInfo->guild_boss_create_time, guildInfo->guild_boss_dead_time);

    if(recvMsg->flag == GUILD_BOSS_DEAD_TIME_FLAG)
    {
        // 发送公会BOSS倒计时时间
        SendGuildBossCountdown(guildInfo);
    }

    // 更新GameServer
    FillGuildUpdateInfo(guildInfo, sendGuildInfo),

    // 通知所有的GameServer
    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, &sendGuildInfo);

    // 发送给公会所有成员
    SendToAllGuildMember(guildInfo, MSG_GUILD_INFO_NTF, &sendGuildInfo);
}

// 填充公会的基础信息
void GuildManager::FillGuildUpdateInfo(GUILD_INFO* guildInfo, GUILD_INFO& sendMsg, uint8 opt_flag)
{
    if(!guildInfo) return;

    sendMsg.opt_flag = opt_flag;
    sendMsg.guild_id_h = guildInfo->guild_id_h;
    sendMsg.guild_id_l = guildInfo->guild_id_l;
    sendMsg.guild_name = guildInfo->guild_name;
    sendMsg.bulletin = guildInfo->bulletin;
    sendMsg.level = guildInfo->level;
    sendMsg.exp = guildInfo->exp;
    sendMsg.flag = guildInfo->flag;

    sendMsg.guild_boss_level = guildInfo->guild_boss_level;
    sendMsg.guild_boss_exp = guildInfo->guild_boss_exp;
    sendMsg.guild_boss_start_time = guildInfo->guild_boss_start_time;
    sendMsg.guild_boss_create_time = guildInfo->guild_boss_create_time;
    sendMsg.guild_boss_dead_time = guildInfo->guild_boss_dead_time;
    sendMsg.addition_list = guildInfo->addition_list;

    sendMsg.battle_power = guildInfo->battle_power;

    sendMsg.center_server_id = guildInfo->center_server_id;
}
// 通知怪物讨伐战开启时间
void GuildManager::NotifyGuildBossTime(GUILD_INFO* guildInfo)
{
    GUILD_BOSS_COUNTDOWN sendMsg;
    Crown::CSDDateTime now = Crown::SDNow();
    Crown::CSDDateTime startTime;
    Crown::CSDDateTime endTime;
    uint32 crusadeEndTime = 0;

    if(!guildInfo) return;

    // 魔物讨伐战持续时间
    crusadeEndTime = GuildConfig::Instance()->GetCrusadeEndTime();

    // 检查怪物开启时间
    startTime = now;
    startTime.SetTime(guildInfo->guild_boss_start_time/3600, 
        guildInfo->guild_boss_start_time%3600/60, guildInfo->guild_boss_start_time%60);

    // 设置结束时间
    endTime = startTime;
    endTime.IncSecond(crusadeEndTime);

    if(now.GetTimeValue() < startTime.GetTimeValue())
    {
        sendMsg.flag = 0;
        sendMsg.time = (uint32)(startTime.GetTimeValue() - now.GetTimeValue());
    }
    else if(now.GetTimeValue() >= endTime.GetTimeValue())
    {
        sendMsg.flag = 0;
        sendMsg.time = (now.GetHour() * 3600) + (now.GetMinute() * 60) + now.GetSecond();
        sendMsg.time = DAY_SECONDS - sendMsg.time;
        sendMsg.time = sendMsg.time + guildInfo->guild_boss_start_time;
    }
    else
    {
        sendMsg.flag = 1;
        sendMsg.time = (uint32)(now.GetTimeValue() - startTime.GetTimeValue());
    }

    // 发送给公会所有成员
    SendToAllGuildMember(guildInfo, MSG_GUILD_BOSS_COUNTDOWN_ACK, &sendMsg);
}

// 发送给所有公会成员
void GuildManager::SendToAllGuildMember(GUILD_INFO* guildInfo, uint32 msgID, void* data, uint64 exceptID)
{
    uint64 memberID = 0;
    Player* member = 0;

    if(!guildInfo || !data) return;

    // 向所有成员广播
    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildInfo->member_list.begin();
        memberIt != guildInfo->member_list.end(); ++memberIt)
    {
        memberID = U64ID(memberIt->player_id_h, memberIt->player_id_l);
        if(exceptID && (memberID == exceptID)) continue;
        member = PlayerManager::Instance()->GetPlayerByPlayerID(memberID);
        if(!member) continue;
        member->SendToGate(msgID, data);
    }
}

// 发送订单
void GuildManager::DeliverTradeOrder(TRADE_ORDER_INFO& tradeOrder)
{
    uint64      playerID = 0;
    Player*     player = 0;
    GUILD_INFO* guildInfo = 0;

    playerID = U64ID(tradeOrder.buyer_id.id_h, tradeOrder.buyer_id.id_l);
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        // !!! 记录 LOG
        //CnError
        return;
    }

    guildInfo = GuildManager::Instance()->GetGuildInfo(player->GetGuildID());
    if(!guildInfo)
    {
        return;
    }

    if(tradeOrder.pay_reason == REWARD_REASON_GUILD_BOSS_EXP)
    {
        AddGuildBossExp(guildInfo, tradeOrder.item_num);

//         // 给玩家发送返还工会点
//         tradeOrder.item_type = PROP_GUILDPOINT;
//         tradeOrder.item_id = 0;
//         tradeOrder.item_num = (int64)tradeOrder.total_price * GuildConfig::Instance()->GetGuildBossLevelUpRebateRate() / BASE_RATE_NUM;
//         tradeOrder.ext = "";
//         TradeManager::Instance()->DeliverTradeOrder(tradeOrder);
    }
}

// 通知游戏服务器所有的公会信息
void GuildManager::SendAllGuildInfoToGameSvr(uint32 sessionID)
{
    uint32 regionID = 0;

    regionID = GameHandler::Instance()->GetRegionIDBySessionID(sessionID);

    for (GuildMap::iterator guildIt = m_guildMap.begin(); guildIt != m_guildMap.end(); ++guildIt)
    {
        GameHandler::Instance()->SendByID(regionID, MSG_GUILD_INFO_NTF, &guildIt->second);
    }
}

// 添加工会怪物经验
void GuildManager::AddGuildBossExp(GUILD_INFO* guildInfo, int32 addExp)
{
    const GuildBossTmpl* guildBossTmpl = 0;
    GUILD_INFO sendGuildInfo;
    uint64 guildID = 0;

    if(!guildInfo || (addExp < 1))
    {
        return;
    }

    // 获取公会的等级模板
    guildBossTmpl = GuildConfig::Instance()->GetGuildBossTmpl(guildInfo->guild_boss_level);
    if(!guildBossTmpl)
    {
        return;
    }

    guildInfo->guild_boss_exp += addExp;
    if(guildInfo->guild_boss_exp >= guildBossTmpl->max_exp)
    {
        guildInfo->guild_boss_exp = guildBossTmpl->max_exp;
    }

    if((guildInfo->guild_boss_level < GuildConfig::Instance()->GetMaxCrusadeBossLevel()) && 
       (guildInfo->guild_boss_exp >= guildBossTmpl->max_exp))
    {
        guildInfo->guild_boss_exp = 0;
        ++guildInfo->guild_boss_level;
    }

    guildID = U64ID(guildInfo->guild_id_h, guildInfo->guild_id_l);

    // 保存公会的BOSS等级和经验到DB
    GuildJob::Instance()->SetGuildBossLeveAndExp(guildID, guildInfo->guild_boss_level, guildInfo->guild_boss_exp);

    // 填充公会更新信息
    FillGuildUpdateInfo(guildInfo, sendGuildInfo),

    // 发送给公会所有成员
    SendToAllGuildMember(guildInfo, MSG_GUILD_INFO_NTF, &sendGuildInfo);

    // 通知所有的GameServer
    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, &sendGuildInfo);
}

// ==================================================
// 公会BOSS倒计时请求
// ==================================================
void GuildManager::SendGuildBossCountdown(GUILD_INFO* guildInfo)
{
    GUILD_BOSS_COUNTDOWN sendMsg;
    Crown::CSDDateTime now = Crown::SDNow();
    Crown::CSDDateTime startTime;
    Crown::CSDDateTime endTime;
    Crown::CSDDateTime zeroTime;
    uint32 crusadeEndTime = 0;
    uint64 memberID = 0;
    Player* player = 0;
    uint16 errCode = ERR_FAILED;

    // 获取公会
    if(!guildInfo) return;

    // 怪物抢夺战持续时间
    crusadeEndTime = GuildConfig::Instance()->GetCrusadeEndTime();

    // 检查怪物开启时间
    startTime = now;
    startTime.SetTime(guildInfo->guild_boss_start_time/3600, 
        guildInfo->guild_boss_start_time%3600/60, guildInfo->guild_boss_start_time%60);

    // 设置结束时间
    endTime = startTime;
    endTime.IncSecond(crusadeEndTime);

    // 今天0点
    zeroTime = now;
    zeroTime.SetTime(0, 0, 0);

    if(now.GetTimeValue() < startTime.GetTimeValue())
    {
        sendMsg.flag = 0;
        sendMsg.time = (uint32)(startTime.GetTimeValue() - now.GetTimeValue());
    }
    else if(now.GetTimeValue() >= endTime.GetTimeValue() || guildInfo->guild_boss_dead_time >= zeroTime.GetTimeValue())
    {
        sendMsg.flag = 0;
        sendMsg.time = (now.GetHour() * 3600) + (now.GetMinute() * 60) + now.GetSecond();
        sendMsg.time = DAY_SECONDS - sendMsg.time;
        sendMsg.time = sendMsg.time + guildInfo->guild_boss_start_time;
    }
    else
    {
        sendMsg.flag = 1;
        sendMsg.time = (uint32)(now.GetTimeValue() - startTime.GetTimeValue());
    }

    // 公会信息
    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildInfo->member_list.begin(); 
            memberIt != guildInfo->member_list.end(); ++memberIt)
    {
        memberID = U64ID(memberIt->player_id_h, memberIt->player_id_l);
        player = PlayerManager::Instance()->GetPlayerByPlayerID(memberID);
        if(!player) continue;
        player->SendToGate(MSG_GUILD_BOSS_COUNTDOWN_ACK, &sendMsg);
    }
}

bool GuildManager::OnGuildBulletinUpdate( uint64 guildId, const std::string& bulletinstr )
{
    GUILD_INFO sendGuildInfo;
    GUILD_INFO* guildInfo = 0;

    // 获取公会
    guildInfo = GetGuildInfo(guildId);
    if(!guildInfo)
    {
        return false;
    }

    // 设置公会公告
    guildInfo->bulletin = bulletinstr;

    // 保存公告到数据库
    GuildJob::Instance()->SetGuildBulletin(guildId, guildInfo->bulletin);

    // 设置要发送的公会信息
    FillGuildUpdateInfo(guildInfo, sendGuildInfo);

    // 发送给公会所有成员
    SendToAllGuildMember(guildInfo, MSG_GUILD_INFO_NTF, &sendGuildInfo);

    // 通知所有的GameServer
    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, &sendGuildInfo);
    return true;
}

bool GuildManager::OnGuildOfficeUpdate( uint64 gid, uint64 pid, uint32 officeid )
{
    GUILD_INFO* guildInfo = 0;
    GUILD_MEMBER* guildMemberA = 0;
    uint64 memberID = 0;

    // 获得公会结构
    guildInfo = GetGuildInfo(gid);
    if(!guildInfo)
    {
        return false;
    }

    if(guildInfo->lock)
    {
        return false;           // 成员职位变更操作尚未完成
    }

    // 获得公会成员结构
    guildMemberA = GetGuildMember(gid, pid);
    if(!guildMemberA)
    {
        return false;
    }

    // 是否存在此成员
    if(!HasGuildMember(gid, pid))
    {
        return false;
    }

    // 最后检查设置职位的有效范围
    if((officeid < GUILD_OFFICE_CHAIRMAN) || (officeid > GUILD_OFFICE_MEMBER))
    {
        return false;
    }
    
    if(guildMemberA->office == officeid)
    {
        return false;
    }

    // 设置职位
    if(officeid == GUILD_OFFICE_CHAIRMAN)
    {   // 设置会长
        if(guildMemberA->office == GUILD_OFFICE_CHAIRMAN)
        {
            //会长不能设置为会长
            return false;
        }
        else
        {
            // 查找角色并设置职位
            memberID = 0;
            for(std::vector<GUILD_MEMBER>::iterator memberIt = guildInfo->member_list.begin();
                memberIt != guildInfo->member_list.end(); ++memberIt)
            {
                if(memberIt->office == GUILD_OFFICE_CHAIRMAN)
                {
                    memberID = U64ID(memberIt->player_id_h, memberIt->player_id_l);
                    break;
                }
            }
            
            // 设置为会长
            GuildJob::Instance()->SetGuildMemberOffice(gid, pid, officeid);

            // 前会长设置为普通会员
            GuildJob::Instance()->SetGuildMemberOffice(gid, memberID, GUILD_OFFICE_MEMBER);

            return true;
        }
    }
    else
    {   // 非会长设置
        if(guildMemberA->office == GUILD_OFFICE_CHAIRMAN)
        {
            //会长不能设置
            return false;
        }
        else
        {
            // 设置职位
            GuildJob::Instance()->SetGuildMemberOffice(gid, pid, officeid);
            return true;
        }
    }

}
// 公会日志
void GuildManager::GuildLog(uint64 guildId, uint32 eventId, uint64 playerId, int32 arg1)
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerId);
    std::string playerName = player ? player->GetName() : "";

    DoingGameLog::Instance()->WriteCenterLog(LOG_GUILD, 0, WriterToLogByGuild(guildId, eventId, playerName, arg1));

}