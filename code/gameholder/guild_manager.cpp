#include "gameholder_pch.h"
#include "guild_manager.h"
#include "player_manager.h"
#include "player.h"
#include "message_encoder.h"
#include "item_manager.h"
#include "trade_manager.h"
#include "guild_config.h"
#include "guild_sale_config.h"
#include "center_handler.h"
#include "world_boss_manager.h"
#include "formula_manager.h"
#include "boss.h"
#include "battle_unit_communicator.h"
#include "gate_handler.h"
#include "twilight_war_manager.h"
#include "vip_config.h"
#include "json_helper.h"
#include "namelimit.h"
#include "event_define.h"
#include "game_define.h"


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
    //m_player = 0;
    //m_team_info.Clear();
    m_guild_member_trun_pos = 0;
    m_guild_tabletrun_pos = 0;
}

// ==================================================
// 接收网络消息
// ==================================================
void GuildManager::OnRecv(Player* player, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_GUILD_INFO_REQ:                    // 公会信息请求
        OnGuildInfoReq(player);
        break;
    case MSG_GUILD_INFO_NTF:                    // 公会信息修改通知
        OnGuildInfoNtf(data);
        break;
    case MSG_GUILD_APPLY_LIST_REQ:              // 公会申请列表请求
        OnGuildApplyListReq(player, data);
        break;
    case MSG_GUILD_LIST_REQ:                    // 公会列表查询请求
        OnGuildListReq(player, data);
        break;
    case MSG_GUILD_ID_NTF:                      // 通知公会ID
        OnGuildIDNtf(player, data);
        break;
    case MSG_CREATE_GUILD_REQ:                  // 创建公会请求
        OnCreateGuildReq(player, data);
        break;
    case MSG_CREATE_GUILD_ACK:                  // 创建公会后扣费
        OnCreateGuildAck(player, data);
        break;
    case MSG_DESTROY_GUILD_REQ:                 // 销毁公会请求
        OnDestroyGuildReq(player);
        break;
    case MSG_VIEW_GUILD_INFO_REQ:               // 公会信息请求
        OnViewGuildInfoReq(player, data);
        break;
    case MSG_INVITE_JOIN_GUILD_REQ:             // 邀请加入公会请求
        OnInviteJoinGuildReq(player, data);
        break;
    case MSG_INVITE_JOIN_GUILD_ANS:             // 邀请加入公会应答
        OnInviteJoinGuildAns(player, data);
        break;
    case MSG_APPLY_JOIN_GUILD_REQ:              // 申请加入公会请求
        OnApplyJoinGuildReq(player, data);
        break;
    case MSG_APPLY_JOIN_GUILD_ANS:              // 申请加入公会应答
        OnApplyJoinGuildAns(player, data);
        break;
    case MSG_GUILD_MEMBER_DEL_REQ:              // 请求删除队伍成员
        OnGuildMemberDelReq(player, data);
        break;
    case MSG_GUILD_SET_OFFICE_REQ:              // 设置公会职位请求
        OnGuildSetOfficeReq(player, data);
        break;
    case MSG_GUILD_SET_FLAG_REQ:                // 设置公会标记请求
        OnGuildSetFlagReq(player, data);
        break;
    case MSG_GUILD_WELFARE_INFO_REQ:            // 请求公会福利信息
        OnGuildWelfareInfoReq(player, data);
        break;
    case MSG_GUILD_INHERIT_ATEVENT_REQ:         // 公会继续会长请求
        OnGuildInheritAteventReq(player, data);
        break;
    case MSG_GUILD_DONATE_REQ:                  // 公会捐献请求
        OnGuildDonateReq(player, data);
        break;
    case MSG_GUILD_DONATE_ACK:                  // 公会捐献确认
        OnGuildDonateAck(player, data);
        break;
    case MSG_GUILD_SET_BULLETIN_REQ:            // 设置公会公告板
        OnGuildSetBulletinReq(player, data);
        break;
    case MSG_GUILD_BUY_REQ:                     // 购买公会物品请求
        OnGuildBuyReq(player, data);
        break;
    case MSG_GUILD_BUY_MEMBER_TURNTABLE_REQ:    // 公会购买个人轮盘重置
        OnGuildBuyMemberTurnTableReq(player, data);
        break;
    case MSG_GUILD_RAND_TURNTABLE_REQ:          // 公会随机龙盘
        OnGuildRandTurntable(player, data);
        break;
   // case MSG_GUILD_RAND_TURNTABLE_RESULT_REQ:
       // OnRandomTurntableResult(player, data);
        break;;
    case MSG_GUILD_MEMBER_TURNTABLE_REQ:        // 公会成员龙盘
        OnGuildMemberTurntable(player, data);
        break;
   //case MSG_GUILD_MEMBER_TURNTABLE_RESULT_REQ: 
       // OnMemberTurntableResult(player,data);
        break;
    case MSG_GUILD_REWARD_PERSONAL_WELFARE:     // 公会个人奖励
        OnGuildRewardPersonalWelfare(player, data);
        break;
    case MSG_GUILD_BOSS_LEVELUP_REQ:            // 提升公会等级请求
        OnGuildBossLevelupReq(player, data);
        break;
    case MSG_GUILD_BOSS_COUNTDOWN_REQ:          // 公会BOSS倒计时请求
        OnGuildBossCountdownReq(player, data);
        break;
    case MSG_GUILD_BOSS_SET_TIME_REQ:           // 设置公会BOSS时间请求
        OnGuildBossSetTimeReq(player, data);
        break;
    case MSG_GUILD_CRUSADE_BOSS_RANK_REQ:       // 请求公会BOSS排行信息
        OnGuildCrusadeBossRankReq(player, data);
        break;
    case MSG_GUILD_TWILIGHT_WAR_RANK_REQ:       // 请求公会暮光之城排行信息
        OnGuildTwilightWarRankReq(player, data);
        break;
    default:
        CnAssert(false);
        break;
    }
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

    return &(it->second);
}

// ==================================================
// 获取公会信息
// ==================================================
GUILD_INFO* GuildManager::GetGuildInfo(const std::string& guildName)
{
    for(GuildMap::iterator it = m_guildMap.begin(); it != m_guildMap.end(); ++it)
    {
        if(it->second.guild_name == guildName)
        {
            return &(it->second);
        }
    }
    return NULL;
}

// ==================================================
// 获取公会名称
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
// 好友信息请求
// ==================================================
void GuildManager::OnGuildInfoReq(Player* player)
{
    GUILD_INFO* guildInfo = 0;
    uint64 guildID = 0;

    if(!player) return;

    guildID = player->GetGuildID();
    if(!guildID) return;

    guildInfo = GetGuildInfo(guildID);
    if(!guildInfo) return;

    guildInfo->opt_flag = OPT_INIT_FLAG;
    player->SendToGate(MSG_GUILD_INFO_NTF, guildInfo);
}

// ==================================================
// 公会信息通知
// ==================================================
void GuildManager::OnGuildInfoNtf(void* data)
{
    GUILD_INFO* recvMsg = (GUILD_INFO*)data;
    uint64 guildID = 0;

    guildID = U64ID(recvMsg->guild_id_h, recvMsg->guild_id_l);

    if(!recvMsg) return;

    if(recvMsg->opt_flag == OPT_INIT_FLAG)
    {
        if(recvMsg->member_list.size() > 0)
        {
            OnRefreshGuildInfo(recvMsg);
        }
        else
        {
            OnDestoryGuildInfo(recvMsg);
        }
    }
    else if(recvMsg->opt_flag == OPT_ADD_FLAG)
    {
        OnAddGuildInfo(recvMsg);
    }
    else if(recvMsg->opt_flag == OPT_DEL_FLAG)
    {
        OnDelGuildInfo(recvMsg);
    }
    else if(recvMsg->opt_flag == OPT_UPDATE_FLAG)
    {
        OnUpdateGuildInfo(recvMsg);
    }
}

// ==================================================
// 公会申请列表请求
// ==================================================
void GuildManager::OnGuildApplyListReq(Player* player, void* data)
{
    NONE* recvMsg = (NONE*)data;

    if(!player || !recvMsg)
    {
        return;
    }

    player->SendToCenter(MSG_GUILD_APPLY_LIST_REQ, recvMsg);
}

// ==================================================
// 公会列表查询请求
// ==================================================
void GuildManager::OnGuildListReq(Player* player, void* data)
{
    GUILD_LIST_REQ* recvMsg = (GUILD_LIST_REQ*)data;

    if(!player || !recvMsg)
    {
        return;
    }

    player->SendToCenter(MSG_GUILD_LIST_REQ, recvMsg);
}

// ==================================================
// 创建公会请求
// ==================================================
void GuildManager::OnCreateGuildReq(Player* player, void* data)
{
    CREATE_GUILD_REQ* sendMsg = (CREATE_GUILD_REQ*)data;
    Crown::CSDDateTime now = Crown::SDNow();
    uint16 errcode = ERR_SUCCEED;

    if(!player) return;

    // 检查角色的公会功能是否开启
    //errcode = OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_GUILD, player);
    //if(errcode != ERR_SUCCEED)
    //{
    //    return;
    //}

    // 检查公会名是否合法
    errcode = NameLimit::Instance()->IsValidGuildNameStr(sendMsg->guild_name);
    if(errcode != ERR_SUCCEED)
    {
        player->NotifyErrorCode(errcode);
        return;
    }

    // 检查是否已有公会
    if(player->GetGuildID())
    {
        player->NotifyErrorCode(ERR_HAVE_GUILD);
        return;
    }

    // 检查加入公会的间隔时间
    if(player->GetGuildJoinTime() > now.GetTimeValue())
    {
        player->NotifyErrorCode(ERR_CANT_JOIN_NEW_GUILD);
        return;
    }

    // 检查创建公会所需的游戏币
    if(player->GetGamePoint() < GuildConfig::Instance()->GetCreateGuildMoneyPoint())
    {
        player->NotifyErrorCode(ERR_NO_ENOUGH_GAMEPOINT);
        return;
    }

    player->SendToCenter(MSG_CREATE_GUILD_REQ, sendMsg);
}

// ==================================================
// 创建公会后扣费
// ==================================================
void GuildManager::OnCreateGuildAck(Player* player, void* data)
{
    if(!player) return;

    // 如果创建成功，则扣除创建公会的费用
    player->SubItemOne(REWARD_REASON_CREATE_GUILD, PROP_GAMEPOINT, GuildConfig::Instance()->GetCreateGuildMoneyPoint());
}

// ==================================================
// 销毁队伍请求
// ==================================================
void GuildManager::OnDestroyGuildReq(Player* player)
{
    NONE sendMsg;

    if(!player) return;

    if(!player->GetGuildID()) return;

    player->SendToCenter(MSG_DESTROY_GUILD_REQ, &sendMsg);
}

// ==================================================
// 销毁队伍请求
// ==================================================
void GuildManager::OnViewGuildInfoReq(Player* player, void* data)
{
    VIEW_GUILD_INFO_REQ* recvMsg = (VIEW_GUILD_INFO_REQ*)data;
    GUILD_INFO* guildInfo = 0;

    if(!player || !recvMsg) return;

    guildInfo = GuildManager::Instance()->GetGuildInfo(recvMsg->guild_name);
    if(!guildInfo)
    {
        return;
    }

    player->SendToGate(MSG_VIEW_GUILD_INFO_ACK, guildInfo);
}

// ==================================================
// 邀请加入公会请求
// ==================================================
void GuildManager::OnInviteJoinGuildReq(Player* player, void* data)
{
    INVITE_JOIN_GUILD_REQ* sendMsg = (INVITE_JOIN_GUILD_REQ*)data;

    if(!player || !sendMsg) return;

    // 检查功能是否开启
    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_GUILD, player) != ERR_SUCCEED)
    //{
    //    return;
    //}

    player->SendToCenter(MSG_INVITE_JOIN_GUILD_REQ, sendMsg);
}

// ==================================================
// 邀请加入公会应答
// ==================================================
void GuildManager::OnInviteJoinGuildAns(Player* player, void* data)
{
    INVITE_JOIN_GUILD_ACK* recvMsg = (INVITE_JOIN_GUILD_ACK*)data;

    if(!player || !recvMsg) return;

    // 提示对方自己的功能未开启
    if(recvMsg->errcode == ERR_FUNCT_NOT_OPEN)
    {
        // 直接发给对方错误吗
        ERROR_CODE sendMsg;
        uint64 inviterID = U64ID(recvMsg->inviter_player_id.id_h, recvMsg->inviter_player_id.id_l);
        sendMsg.errcode = ERR_RECEIVER_FUNCT_NOT_OPEN;
        GateHandler::Instance()->SendByGateID(recvMsg->inviter_gate_id, MSG_ERROR_CODE, &sendMsg, inviterID);
        return;
    }

    // 检查功能是否开启
    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_GUILD, player) != ERR_SUCCEED)
    //{
    //    return;
    //}

    if(recvMsg->errcode != ERR_SUCCEED)
        return;

    player->SendToCenter(MSG_INVITE_JOIN_GUILD_ANS, recvMsg);
}

// ==================================================
// 申请加入公会请求
// ==================================================
void GuildManager::OnApplyJoinGuildReq(Player* player, void* data)
{
    APPLY_JOIN_GUILD_REQ* sendMsg = (APPLY_JOIN_GUILD_REQ*)data;
    Crown::CSDDateTime now = Crown::SDNow();

    ERROR_CODE curerrcode;
    curerrcode.errcode = ERR_FAILED;
    //uint16 errcode = ERR_SUCCEED;

    if(!player || !sendMsg) return;

    // 检查功能是否开启
    //curerrcode.errcode = OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_GUILD, player);
    //if(curerrcode.errcode != ERR_SUCCEED)
    //{
    //    curerrcode.errcode = ERR_FUNCT_NOT_OPEN;
    //    goto _END;
    //}

    // 如果存在公会则不能申请加入其它公会（只是临时过滤下）
    if(player->GetGuildID())
    {
        curerrcode.errcode = ERR_HAVE_GUILD;
        goto _END;
    }

    // 检查是否超过了间隔时间
    if(player->GetGuildJoinTime() > now.GetTimeValue())
    {
        curerrcode.errcode = ERR_CANT_JOIN_NEW_GUILD;
        goto _END;
    }

    player->SendToCenter(MSG_APPLY_JOIN_GUILD_REQ, sendMsg);
    return;
_END:
    player->SendToGate(MSG_APPLY_JOIN_GUILD_REQ_ACK, &curerrcode);
}

// ==================================================
// 申请加入公会应答
// ==================================================
void GuildManager::OnApplyJoinGuildAns(Player* player, void* data)
{
    APPLY_JOIN_GUILD_ACK* sendMsg = (APPLY_JOIN_GUILD_ACK*)data;

    if(!player || !sendMsg) return;

    if(!player->GetGuildID())
    {
        return;
    }

    //防止玩家在进入公会前，申请多个公会，退会后还能加入其他公会
    uint64 askPlayerID = U64ID(sendMsg->player_id_h, sendMsg->player_id_l);
    Crown::CSDDateTime now = Crown::SDNow();
    Player* recvie_player = PlayerManager::Instance()->GetPlayerByPlayerID(askPlayerID);
    if(recvie_player) 
    {
        if( recvie_player->GetGuildJoinTime() > now.GetTimeValue() )
        {
            player->NotifyErrorCode(ERR_CANT_JOIN_NEW_GUILD);
            return;
        }
    }

    player->SendToCenter(MSG_APPLY_JOIN_GUILD_ANS, sendMsg);
}

// ==================================================
// 删除公会成员
// ==================================================
void GuildManager::OnGuildMemberDelReq(Player* player, void* data)
{
    GUILD_MEMBER_DEL_REQ* recvMsg = (GUILD_MEMBER_DEL_REQ*)data;

    if(!player || !recvMsg) return;

    if(!player->GetGuildID())
    {
        return;
    }

    player->SendToCenter(MSG_GUILD_MEMBER_DEL_REQ, recvMsg);
}

// ==================================================
// 设置公会职位请求
// ==================================================
void GuildManager::OnGuildSetOfficeReq(Player* player, void* data)
{
    GUILD_SET_OFFICE_REQ* recvMsg = (GUILD_SET_OFFICE_REQ*)data;

    if(!player || !recvMsg) return;

    if(!player->GetGuildID())
    {
        return;
    }

    player->SendToCenter(MSG_GUILD_SET_OFFICE_REQ, recvMsg);
}

// ==================================================
// 设置公会标记请求
// ==================================================
void GuildManager::OnGuildSetFlagReq(Player* player, void* data)
{
    GUILD_SET_FLAG_REQ* recvMsg = (GUILD_SET_FLAG_REQ*)data;

    if(!player || !recvMsg) return;

    if(!player->GetGuildID())
    {
        return;
    }

    player->SendToCenter(MSG_GUILD_SET_FLAG_REQ, recvMsg);
}

// ==================================================
// // 设置公会ID
// ==================================================
void GuildManager::OnGuildIDNtf(Player* player, void* data)
{
    GUILD_ID_NTF* recvMsg = (GUILD_ID_NTF*)data;
    Crown::CSDDateTime now = Crown::SDNow();
    uint64 guildID = 0;
    uint32 guildJoinTime = 0;

    if(!player || !recvMsg) return;

    guildID = U64ID(recvMsg->guild_id_h, recvMsg->guild_id_l);

    // 设置公会ID
    if(player->GetGuildID() && !guildID)
    {
        guildJoinTime = (uint32)now.GetTimeValue() + GuildConfig::Instance()->GetGuildJoinInterval();
        player->SetGuildJoinTime(guildJoinTime);
    }

    if(guildID)
    {
        player->SendTaskEvent(TASK_EVENT_JOIN_GUILD, 0, 0);
    }

    player->SetGuildID(guildID);

    // behavior log
    player->SendBehaviorLog("guild %d", guildID);
}

// ==================================================
// 公会升级捐献请求
// ==================================================
void GuildManager::OnGuildDonateReq(Player* player, void* data)
{
    GUILD_DONATE_REQ* recvMsg = (GUILD_DONATE_REQ*)data;
    const GuildLevelTmpl* guildLevelTmpl = 0;
    GuildMap::iterator guildIt;
    uint16 errCode = ERR_SUCCEED;

    if(!player || !recvMsg) return;

    // 超过购买限制的直接设置为购买最大值
    recvMsg->donate_value = (recvMsg->donate_value > g_MaxBuyNum) ? (g_MaxBuyNum) : (recvMsg->donate_value);
    if(recvMsg->donate_value > g_MaxBuyNum || recvMsg->donate_value < 1)
        return;         // 单次最大捐献数

    // 检查角色是否存在公会
    if(!player->GetGuildID())
    {
        errCode = ERR_HAVENT_GUILD;
        goto _ERR;
    }

    // 获取公会
    guildIt = m_guildMap.find(player->GetGuildID());
    if(guildIt == m_guildMap.end())
    {
        errCode = ERR_UNFOUND_GUILD;
        goto _ERR;
    }

    // 获取公会的等级模板
    guildLevelTmpl = GuildConfig::Instance()->GetGuildLeveTmpl(guildIt->second.level);
    if(!guildLevelTmpl)
    {
        errCode = ERR_FAILED;
        goto _ERR;
    }

    if (recvMsg->donate_type == PROP_GAMEPOINT)
    {
        int32 addGuildPoint = recvMsg->donate_value * GuildConfig::Instance()->GetGamepointDonateTmp().guildpoint;
        int32 donateGamePoint = recvMsg->donate_value * GuildConfig::Instance()->GetGamepointDonateTmp().num;

        errCode = player->SubItemOne(REWARD_REASON_LEVEL_INVESTMENT, PROP_GAMEPOINT, donateGamePoint);
        if(errCode != ERR_SUCCEED)
        {
            goto _ERR;
        }

        // 根据捐赠Gamepoint数增加公会贡献点
        player->AddGuildPoint(addGuildPoint, REWARD_REASON_GUILD_DONATE);

        if((guildIt->second.level < GuildConfig::Instance()->GetMaxLevel())
            && (guildIt->second.exp  < guildLevelTmpl->max_exp))
        {
            // 发送给中心服务器已扣掉的游戏币
            recvMsg->donate_back_guild_point = addGuildPoint;

            player->SendToCenter(MSG_GUILD_DONATE_REQ, recvMsg);
            return;
        }
    }
    else if (recvMsg->donate_type == PROP_MONEYPOINT)
    {
        int32 need_moneypoint = recvMsg->donate_value; // 检查所需的魔石
        if (player->GetMoneyPoint() < need_moneypoint) // check
        {
            errCode = ERR_NO_ENOUGH_MONEYPOINT;
            goto _ERR;
        }
        else
        {
            TradeManager::Instance()->SendTradeOrderReq(player, REWARD_REASON_GUILD_DONATE, PROP_GUILD_DONATE, 0, need_moneypoint, need_moneypoint); // trade
        }
    }

_ERR:
    player->NotifyErrorCode(errCode);
}

// ==================================================
// 公会捐献回调
// ==================================================
void GuildManager::CBGuildDonateTrade(Player* player, uint32 needMoneyPoint)
{
    if (needMoneyPoint > 0)
    {
        GUILD_DONATE_REQ recvMsg;

        recvMsg.donate_type = PROP_MONEYPOINT;
        recvMsg.donate_value = needMoneyPoint;
        recvMsg.donate_back_guild_point = needMoneyPoint * GuildConfig::Instance()->GetMoneypointDonateTmp().guildpoint;

        player->AddGuildPoint(recvMsg.donate_back_guild_point, REWARD_REASON_GUILD_DONATE);

        player->SendToCenter(MSG_GUILD_DONATE_REQ, &recvMsg);
    }
}

// ==================================================
// 更新公会的BOSS创建时间
// ==================================================
void GuildManager::UpdateGuildBossCreateTime(uint64 guildID)
{
    Crown::CSDDateTime nowTime = Crown::SDNow();
    GUILD_INFO* guildInfo = 0;

    guildInfo = GetGuildInfo(guildID);

    if(!guildInfo)
    {
        return;
    }

    guildInfo->guild_boss_create_time = (uint32)nowTime.GetTimeValue();

    SendRefreshBossTime(guildID, GUILD_BOSS_CREATE_TIME_FLAG);
}

// ==================================================
// 更新公会的BOSS死亡时间
// ==================================================
void GuildManager::UpdateGuildBossDeadTime(uint64 guildID)
{
    Crown::CSDDateTime nowTime = Crown::SDNow();
    GUILD_INFO* guildInfo = 0;

    guildInfo = GetGuildInfo(guildID);

    if(!guildInfo)
    {
        return;
    }

    guildInfo->guild_boss_dead_time = (uint32)nowTime.GetTimeValue();

    SendRefreshBossTime(guildID, GUILD_BOSS_DEAD_TIME_FLAG);
}

// ==================================================
// 公会升级捐献应答
// ==================================================
void GuildManager::OnGuildDonateAck(Player* player, void* data)
{

}

// ==================================================
// 设置公会公告板
// ==================================================
void GuildManager::OnGuildSetBulletinReq(Player* player, void* data)
{
    GUILD_SET_BULLETIN_REQ* recvMsg = (GUILD_SET_BULLETIN_REQ*)data;

    if(!player || !recvMsg) return;

    if(!player->GetGuildID())
    {
        return;
    }

    if(recvMsg->bulletin.size() >= GUILD_BULLETIN_LEN)
    {
        player->NotifyErrorCode(ERR_FAILED);
        return;
    }

    player->SendToCenter(MSG_GUILD_SET_BULLETIN_REQ, recvMsg);
}

// ==================================================
// 公会购买请求(公会商店)
// ==================================================
void GuildManager::OnGuildBuyReq(Player* player, void* data)
{
//    GUILD_BUY_REQ* recvMsg = (GUILD_BUY_REQ*)data;
//    GUILD_INFO* guildInfo = 0;
//    const GuildLevelTmpl* guildLevelTmpl = 0;
//    const ItemTemplate* itemTmpl = 0;
//    uint16 err_code = ERR_FAILED;
//    int32 guild_point = 0;
//    int32 ret = ERR_SUCCEED;
//
//    if(!player || !recvMsg) return;
//
//    if(!player->GetGuildID())
//    {
//        err_code = ERR_HAVENT_GUILD;
//        goto _ERR;
//    }
//
//    guildInfo = GuildManager::Instance()->GetGuildInfo(player->GetGuildID());
//    if(!guildInfo)
//    {
//        goto _ERR;
//    }
//
//    guildLevelTmpl = GuildConfig::Instance()->GetGuildLeveTmpl(guildInfo->level);
//    if(!guildLevelTmpl)
//    {
//        goto _ERR;
//    }
//
//    // 获得购买物品的模板
//    itemTmpl = GuildSaleConfig::Instance()->GetSaleItem(guildLevelTmpl->sale_id, recvMsg->item_id);
//    if(!itemTmpl)
//    {
//        err_code = ERR_UNFOUND_ITEM;
//        goto _ERR;
//    }
//
//    // 检查购买价格的合法性
//    // 超过购买限制的直接设置为购买最大值
//    recvMsg->item_num = (recvMsg->item_num > g_MaxBuyNum) ? (g_MaxBuyNum) : (recvMsg->item_num);
//    if((itemTmpl->guild_buy_price <= 0) || (recvMsg->item_num < 1) || (recvMsg->item_num > g_MaxBuyNum))
//    {
//        goto _ERR;
//    }
//
//    // 检查包裹
//    if(!player->GetBag()->CanAddItem(recvMsg->item_id, recvMsg->item_num))
//    {
//        err_code = ERR_NO_ENOUGH_BAGPOS;
//        goto _ERR;
//    }
//
//    // 计算花费的游戏币
//    guild_point = itemTmpl->guild_buy_price * recvMsg->item_num;
//
//    // 检查游戏币
//    err_code = player->SubItemOne(REWARD_REASON_GUILD_BUY, PROP_GUILDPOINT, guild_point);
//    if(err_code != ERR_SUCCEED)
//    {
//        goto _ERR;
//    }
//
//    // 添加物品
//    player->GetBag()->AddItemByNum(recvMsg->item_id, recvMsg->item_num, REWARD_REASON_GUILD_BUY);
//
//    //player->SendTaskEvent(REWARD_REASON_GUILD_BUY, 0, 0);
//    return;
//
//_ERR:
//    player->NotifyErrorCode(err_code);
}

// ==================================================
// 公会购买贡献点
// ==================================================
//void GuildManager::OnGuildBuyGuildPointReq(Player* player, void* data)
//{
//    int32 cost = 0;
//    int32 guild_point = 0;
//    uint16 errCode = ERR_FAILED;
//
//    if(!player) return;
//
//    // 检查角色是否存在公会
//    if(!player->GetGuildID())
//    {
//        errCode = ERR_HAVENT_GUILD;
//        goto _ERR;
//    }
//
//    // 花费
//    cost = GuildConfig::Instance()->GetBuyGuildPointCost();
//    guild_point = GuildConfig::Instance()->GetBuyGuildPointNum();
//    if((cost < 1) || (guild_point < 1))
//    {
//        goto _ERR;
//    }
//
//    // 检查魔石是否足够
//    if(player->GetMoneyPoint() < cost)
//    {
//        errCode = ERR_NO_ENOUGH_MONEYPOINT;
//        goto _ERR;
//    }
//
//    // 发送魔石交易订单
//    TradeManager::Instance()->SendTradeOrderReq(player, REWARD_REASON_GUILD_BUY_GUILD_POINT,
//        PROP_GUILDPOINT, 0, guild_point, cost);
//
//    return;
//
//_ERR:
//    player->NotifyErrorCode(errCode);
//}

// ==================================================
// 公会购买个人轮盘重置
// ==================================================
void GuildManager::OnGuildBuyMemberTurnTableReq(Player* player, void* data)
{
    int32 cost = 0;
    int32 guild_point = 0;
    int32 memberTurnTable = 0;
    int32 turntableBuyTimes = 0;
    uint16 errCode = ERR_FAILED;

    if(!player) return;

    // 检查角色是否存在公会
    if(!player->GetGuildID())
    {
        errCode = ERR_HAVENT_GUILD;
        goto _ERR;
    }

    // 已购买的次数
    turntableBuyTimes = player->GetGuildMemberTurnTableBuyTimes();

    // 花费
    cost = FormulaManager::Instance()->CalcResetTurnTableCost(turntableBuyTimes);
    if(cost < 1)
    {
        goto _ERR;
    }

    // 检查魔石是否足够
    if(player->GetMoneyPoint() < cost)
    {
        errCode = ERR_NO_ENOUGH_MONEYPOINT;
        goto _ERR;
    }

    memberTurnTable = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_27 , player->GetVipLevel());
    if(player->GetGuildMemberTurnTableBuyTimes() >= memberTurnTable)
    {
        errCode = ERR_MAX_GUILD_MEMBER_TURNTABLE_BUY_TIMES;
        goto _ERR;
    }

    // 先增加轮盘次数，这里不考虑边界值（即扣费失效）
    player->IncGuildMemberTurnTalbeBuyTimes();

    // 发送魔石交易订单
    TradeManager::Instance()->SendTradeOrderReq(player, REWARD_REASON_GUILD_BUY_MEMBER_TURNTABLE,
        PROP_GUILD_MEMBER_TURNTABLE, 0, 0, cost);

    return;

_ERR:
    player->NotifyErrorCode(errCode);
}

// ==================================================
// 请求个人的公会福利信息
// ==================================================
void GuildManager::OnGuildWelfareInfoReq(Player* player, void* data)
{
    GUILD_WELFARE_INFO sendMsg;
    Crown::CSDDateTime now = Crown::SDNow();

    if(!player) return;

    // 检查角色是否存在公会
    if(!player->GetGuildID())
    {
        return;
    }

    sendMsg.Clear();

    // 获得个人的公会福利信息
    GUILD_WELFARE_INFO& guild_welfare_info = player->GetGuildWelfareInfo();

    if(guild_welfare_info.reward_welfare_time > now.GetTimeValue())
    {
        sendMsg.reward_welfare_time = guild_welfare_info.reward_welfare_time;
    }

    if(guild_welfare_info.reward_turntalbe_time > now.GetTimeValue())
    {
        sendMsg.reward_turntalbe_time = guild_welfare_info.reward_turntalbe_time;
    }

    if(now.GetTimeValue() >= guild_welfare_info.reward_turntalbe_time)
    {
        guild_welfare_info.turntable_item_list.clear();

        now.SetTime(0,0,0);
        now.IncDay(1);
        guild_welfare_info.reward_turntalbe_time = (uint32)now.GetTimeValue();
    }

    sendMsg.turntable_item_list = guild_welfare_info.turntable_item_list;

    sendMsg.member_turntable_buy_times = player->GetGuildMemberTurnTableBuyTimes();

    player->SendToGate(MSG_GUILD_WELFARE_INFO_ACK, &sendMsg);
}

// ==================================================
// 公会继续会长请求
// ==================================================
void GuildManager::OnGuildInheritAteventReq(Player* player, void* data)
{
    player->SendToCenter(MSG_GUILD_INHERIT_ATEVENT_REQ, data);
}

//刷新每天的公会购买次数
void GuildManager::OnFreshDayGuildBuyTimes(Player* player)
{
    if(!player)
        return;

    GUILD_WELFARE_INFO& guild_welfare_info = player->GetGuildWelfareInfo();
    player->SetGuildMemberTurnTableBuyTimes(0);
    player->SendToGate(MSG_GUILD_WELFARE_INFO_ACK, &guild_welfare_info);
}

void GuildManager::OnCloseCenterServer(uint32 centerID)
{
    GuildMap::iterator iter;
    for (iter = m_guildMap.begin(); iter != m_guildMap.end(); )
    {
        if (iter->second.center_server_id == centerID)
        {
            m_guildMap.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
}

// ==================================================
// 公会龙盘
// ==================================================
void GuildManager::OnGuildRandTurntable(Player* player, void* data)
{
    TURNTABLE_ACK errMsg;
    GUILD_INFO* guildInfo = 0;
    const GuildTurntableTmpl* guildTurntableTmpl = 0;
    int32 randTurnTableCost = 0;
    int32 totalProbability = 0;
    int32 baseNum = 0;
    int32 randNum = 0;

    if(!player) return;

    errMsg.errcode = ERR_FAILED;
    errMsg.pos = 0;

    // 检查角色是否存在公会
    if(!player->GetGuildID())
    {
        errMsg.errcode = ERR_HAVENT_GUILD;
        goto _ERR;
    }

    // 获得公会结构数据
    guildInfo = GuildManager::Instance()->GetGuildInfo(player->GetGuildID());
    if(!guildInfo)
    {
        errMsg.errcode = ERR_UNFOUND_GUILD;
        goto _ERR;
    }

    // 获得轮盘的随机模板
    guildTurntableTmpl = GuildConfig::Instance()->GetRandTurnTable(guildInfo->level);
    if(!guildTurntableTmpl)
    {
        errMsg.errcode = ERR_FAILED;
        goto _ERR;
    }

    // 计算总概率
    for(GuildTurntableRoundVert::const_iterator it = guildTurntableTmpl->list.begin(); 
        it != guildTurntableTmpl->list.end(); ++it)
    {
        baseNum += it->probability;
    }

    // 检查分母有效性
    if(baseNum <= 0)
    {
        errMsg.errcode = ERR_FAILED;
        goto _ERR;
    }

    // 检查公会贡献点
    randTurnTableCost = GuildConfig::Instance()->GetRandTurnTableCost();
    // 扣除公会贡献点
//     errMsg.errcode = player->SubItemOne(REWARD_REASON_GUILD_RAND_TURNTABLE, PROP_GUILDPOINT, randTurnTableCost);
//     if(errMsg.errcode != ERR_SUCCEED)
//     {
//         goto _ERR;
//     }
    //// 计算随机数
    randNum = Crown::SDRandom(baseNum);

    // 给予物品
    for(GuildTurntableRoundVert::const_iterator it = guildTurntableTmpl->list.begin(); 
        it != guildTurntableTmpl->list.end(); ++it)
    {
        totalProbability += it->probability;
        if(randNum >= totalProbability)
        {
            continue;
        }
        //player->TakeInRewardTable(PROP_ITEM, it->item_id, it->num, REWARD_REASON_GUILD_RAND_TURNTABLE, it->id);
        SetGuildTrunTablePos(it->id);
        player->TakeInRewardTable(PROP_ITEM, it->item_id, it->num, REWARD_REASON_GUILD_RAND_TURNTABLE, it->id);
        errMsg.errcode = ERR_SUCCEED;
        errMsg.pos = it->id;
        break;
    }

_ERR:
    player->SendToGate(MSG_GUILD_RAND_TURNTABLE_ACK, &errMsg);
}

void GuildManager::OnRandomTurntableResult(Player* player, void* data)
{
    GUILD_INFO* guildInfo = 0;
    const GuildTurntableTmpl* guildTurntableTmpl = 0;
    int32 randNum = 0;
    int32 baseNum = 0;
    int32 totalProbability = 0;
    ERROR_CODE errMsg;

    errMsg.errcode = ERR_FAILED;
    if(!player->GetGuildID())
    {
        errMsg.errcode = ERR_HAVENT_GUILD;
        goto _ERR;
    }

    // 获得公会结构数据
    guildInfo = GuildManager::Instance()->GetGuildInfo(player->GetGuildID());
    if(!guildInfo)
    {
        errMsg.errcode = ERR_UNFOUND_GUILD;
        goto _ERR;
    }


    // 获得轮盘的随机模板
    guildTurntableTmpl = GuildConfig::Instance()->GetRandTurnTable(guildInfo->level);
    if(!guildTurntableTmpl)
    {
        errMsg.errcode = ERR_FAILED;
        goto _ERR;
    }


    // 给予物品
    for(GuildTurntableRoundVert::const_iterator it = guildTurntableTmpl->list.begin(); 
        it != guildTurntableTmpl->list.end(); ++it)
    {
        if(it->id == GetGuildTrunTablePos())
        {
            player->TakeInRewardTable(PROP_ITEM, it->item_id, it->num, REWARD_REASON_GUILD_RAND_TURNTABLE, it->id);
            break;
        }
    }
    SetGuildTrunTablePos(0);

    return;
_ERR:
    player->SendToGate(MSG_GUILD_RAND_TURNTABLE_RESULT_ACK, &errMsg);
}
// ==================================================
// 公会个人龙盘
// ==================================================
void GuildManager::OnGuildMemberTurntable(Player* player, void* data)
{
    TURNTABLE_ACK errMsg;
    GUILD_INFO* guildInfo = 0;
    const GuildTurntableTmpl* guildTurntableTmpl = 0;
    const GuildMemberTurntableCost* guildMemberTurntableCost = 0;
    GuildTurntableRoundVert randList;
    int32 baseNum = 0;
    int32 totalProbability = 0;
    int32 randNum = 0;
    std::string extTxt;

    if(!player) return;

    errMsg.pos = 0;
    errMsg.errcode = ERR_FAILED;

    GUILD_WELFARE_INFO& guildWelfareInfo = player->GetGuildWelfareInfo();

    // 检查角色是否存在公会
    if(!player->GetGuildID())
    {
        errMsg.errcode = ERR_HAVENT_GUILD;
        goto _ERR;
    }

    // 获得公会结构数据
    guildInfo = GuildManager::Instance()->GetGuildInfo(player->GetGuildID());
    if(!guildInfo)
    {
        errMsg.errcode = ERR_UNFOUND_GUILD;
        goto _ERR;
    }

    // 公会成员轮盘花费
    guildMemberTurntableCost = GuildConfig::Instance()->LoadMemberTurntableCost(guildWelfareInfo.turntable_item_list.size());
    if(!guildMemberTurntableCost)
    {
        errMsg.errcode = ERR_MAX_GUILD_TURNTABLE_NUM;
        goto _ERR;
    }

    // 检查魔石是否足够
    if(player->GetMoneyPoint() < guildMemberTurntableCost->m_money_point)
    {
        errMsg.errcode = ERR_NO_ENOUGH_MONEYPOINT;
        goto _ERR;
    }

    // 获得轮盘的随机模板
    guildTurntableTmpl = GuildConfig::Instance()->GetMemberTurnTable(guildInfo->level);
    if(!guildTurntableTmpl)
    {
        goto _ERR;
    }

    // 公会成员龙盘
    for(GuildTurntableRoundVert::const_iterator it = guildTurntableTmpl->list.begin(); 
        it != guildTurntableTmpl->list.end(); ++it)
    {
        if(player->IsInMemberTurnTable(it->id)) continue;
        randList.push_back(*it);
    }

    // 判断列表数量
    if(randList.size() < 1)
    {
        errMsg.errcode = ERR_MAX_GUILD_TURNTABLE_NUM;
        goto _ERR;
    }

    // 计算总概率
    for(GuildTurntableRoundVert::const_iterator it = randList.begin(); 
        it != randList.end(); ++it)
    {
        baseNum += it->probability;
    }

    // 检查分母有效性
    if(baseNum <= 0)
    {
        errMsg.errcode = ERR_FAILED;
        goto _ERR;
    }

    //// 计算随机数
    randNum = Crown::SDRandom(baseNum);

    // 给予物品
    for(GuildTurntableRoundVert::const_iterator it = randList.begin(); 
        it != randList.end(); ++it)
    {
        totalProbability += it->probability;
        if(randNum >= totalProbability)
        {
            continue;
        }
        // 将已给予的物品直接压入玩家的列表
        guildWelfareInfo.turntable_item_list.push_back(it->id);

        extTxt = "";
        write_PERSONAL_TURNTABLE_ID(it->id, extTxt);

        //发送魔石交易订单
        TradeManager::Instance()->SendTradeOrderReq(player, REWARD_REASON_GUILD_MEMBER_TURNTABLE,
            PROP_ITEM, it->item_id,it->num, guildMemberTurntableCost->m_money_point, extTxt);
        break;
    }
    return;
_ERR:
    player->SendToGate(MSG_GUILD_MEMBER_TURNTABLE_ACK, &errMsg);
}

void GuildManager::OnGuilddMemberTurntableAck(Player* player, int32 id)
{
    if(!player)
        return;

    TURNTABLE_ACK errMsg;
    SetGuildMemberTrunTablePos(id);
    errMsg.errcode = ERR_SUCCEED;
    errMsg.pos = id;
    player->SendToGate(MSG_GUILD_MEMBER_TURNTABLE_ACK, &errMsg);
}

void GuildManager::OnMemberTurntableResult(Player* player, void* data)
{
    ERROR_CODE errMsg;
    GUILD_INFO* guildInfo = 0;
    const GuildTurntableTmpl* guildTurntableTmpl = 0;
    GuildTurntableRoundVert randList;
    int32 baseNum = 0;
    int32 totalProbability = 0;
    int32 randNum = 0;
    std::string extTxt;

    if(!player) return;

    errMsg.errcode = ERR_FAILED;

    GUILD_WELFARE_INFO& guildWelfareInfo = player->GetGuildWelfareInfo();

    // 检查角色是否存在公会
    if(!player->GetGuildID())
    {
        errMsg.errcode = ERR_HAVENT_GUILD;
        goto _ERR;
    }

    // 获得公会结构数据
    guildInfo = GuildManager::Instance()->GetGuildInfo(player->GetGuildID());
    if(!guildInfo)
    {
        errMsg.errcode = ERR_UNFOUND_GUILD;
        goto _ERR;
    }

    if(!GetGuildMemberTrunTablePos())
    {
        CnWarn("guild member turn pos cant get");
        errMsg.errcode = ERR_FAILED;
        goto _ERR;
    }

    // 获得轮盘的随机模板
    guildTurntableTmpl = GuildConfig::Instance()->GetMemberTurnTable(guildInfo->level);
    if(!guildTurntableTmpl)
    {
        goto _ERR;
    }

    // 给予物品
    for(GuildTurntableRoundVert::const_iterator it = guildTurntableTmpl->list.begin(); 
        it != guildTurntableTmpl->list.end(); ++it)
    {
        if (GetGuildMemberTrunTablePos() == it->id)
        {
            player->TakeInRewardTable(PROP_ITEM, it->item_id, it->num, REWARD_REASON_GUILD_RAND_TURNTABLE, it->id);
            SetGuildMemberTrunTablePos(0);
            break;
        }
    }
    return;
_ERR:
    player->SendToGate(MSG_GUILD_MEMBER_TURNTABLE_ACK, &errMsg);
}
// ==================================================
// 公会奖励金币
// ==================================================
void GuildManager::OnGuildRewardPersonalWelfare(Player* player, void* data)
{
    const GuildLevelTmpl* guildLevelTmpl = 0;
    GuildMap::iterator guildIt;
    uint16 errCode = ERR_SUCCEED;
    Crown::CSDDateTime now = Crown::SDNow();

    if(!player) return;

    GUILD_WELFARE_INFO& guild_welfare_info = player->GetGuildWelfareInfo();

    // 检查角色是否存在公会
    if(!player->GetGuildID())
    {
        errCode = ERR_HAVENT_GUILD;
        goto _ERR;
    }

    // 获取公会
    guildIt = m_guildMap.find(player->GetGuildID());
    if(guildIt == m_guildMap.end())
    {
        errCode = ERR_UNFOUND_GUILD;
        goto _ERR;
    }

    // 获取公会的等级模板
    guildLevelTmpl = GuildConfig::Instance()->GetGuildLeveTmpl(guildIt->second.level);
    if(!guildLevelTmpl)
    {
        errCode = ERR_FAILED;
        goto _ERR;
    }

    if(guild_welfare_info.reward_welfare_time > now.GetTimeValue())
    {
        errCode = ERR_FAILED;
        goto _ERR;
    }

    player->TakeInRewardTable(&guildLevelTmpl->reward_welfare);

    now.SetTime(0,0,0);
    now.IncDay(1);
    guild_welfare_info.reward_welfare_time = (uint32)now.GetTimeValue();
    OnGuildWelfareInfoReq(player, NULL);
    return;

_ERR:
    player->NotifyErrorCode(errCode);
}

// ==================================================
// 公会BOSS升级请求
// ==================================================
void GuildManager::OnGuildBossLevelupReq(Player* player, void* data)
{
    const GuildLevelTmpl* guildLevelTmpl = 0;
    const GuildBossTmpl* guildBossTmpl = 0;
    GuildMap::iterator guildIt;
    uint16 errCode = ERR_FAILED;

    if(!player) return;

    // 检查角色是否存在公会
    if(!player->GetGuildID())
    {
        errCode = ERR_HAVENT_GUILD;
        goto _ERR;
    }

    // 获取公会
    guildIt = m_guildMap.find(player->GetGuildID());
    if(guildIt == m_guildMap.end())
    {
        errCode = ERR_UNFOUND_GUILD;
        goto _ERR;
    }

    // 获取公会的等级模板
    guildLevelTmpl = GuildConfig::Instance()->GetGuildLeveTmpl(guildIt->second.level);
    if(!guildLevelTmpl)
    {
        goto _ERR;
    }

    // 检查是否已开启工会BOSS功能
    if(!guildLevelTmpl->HasActivity(GUILD_ACTIVITY_CRUSADE_BOSS))
    {
        goto _ERR;
    }

    // 获取公会的等级模板
    guildBossTmpl = GuildConfig::Instance()->GetGuildBossTmpl(guildIt->second.guild_boss_level);
    if(!guildBossTmpl)
    {
        goto _ERR;
    }

    // 检查公会 BOSS 等级
    if((guildIt->second.guild_boss_level >= GuildConfig::Instance()->GetMaxCrusadeBossLevel()) && 
        (guildIt->second.guild_boss_exp >= guildBossTmpl->max_exp))
    {
        goto _ERR;
    }

    // 检查魔石数量
    if(player->GetMoneyPoint() < guildBossTmpl->cost_money_point)
    {
        errCode = ERR_NO_ENOUGH_MONEYPOINT;
        goto _ERR;
    }

//     // 发送魔石交易订单
//     TradeManager::Instance()->SendTradeOrderReq(player, REWARD_REASON_GUILD_BOSS_EXP,
//         PROP_ADD_GUILD_BOSS_EXP, 0, guildBossTmpl->add_exp, guildBossTmpl->cost_money_point);

    return;

_ERR:
    player->NotifyErrorCode(errCode);
}
// ==================================================
// 公会BOSS倒计时请求
// ==================================================
void GuildManager::OnGuildBossCountdownReq(Player* player, void* data)
{
    GUILD_BOSS_COUNTDOWN sendMsg;
    const GuildLevelTmpl* guildLevelTmpl = 0;
    GuildMap::iterator guildIt;
    const GUILD_INFO* guildInfo = NULL;
    Crown::CSDDateTime now = Crown::SDNow();
    Crown::CSDDateTime startTime;
    Crown::CSDDateTime endTime;
    Crown::CSDDateTime zeroTime;
    uint32 crusadeEndTime = 0;
    uint16 errCode = ERR_FAILED;

    if(!player) return;

    // 检查角色是否存在公会
    if(!player->GetGuildID())
    {
        errCode = ERR_HAVENT_GUILD;
        goto _ERR;
    }

    // 获取公会
    guildIt = m_guildMap.find(player->GetGuildID());
    if(guildIt == m_guildMap.end())
    {
        errCode = ERR_UNFOUND_GUILD;
        goto _ERR;
    }

    guildInfo = &guildIt->second;

    // 获取公会的等级模板
    guildLevelTmpl = GuildConfig::Instance()->GetGuildLeveTmpl(guildIt->second.level);
    if(!guildLevelTmpl)
    {
        goto _ERR;
    }

    // 检查是否已开启工会BOSS功能
    if(!guildLevelTmpl->HasActivity(GUILD_ACTIVITY_CRUSADE_BOSS))
    {
        goto _ERR;
    }

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
    {   // 还没开始
        sendMsg.flag = 0;
        sendMsg.time = (uint32)(startTime.GetTimeValue() - now.GetTimeValue());
    }
    else if(now.GetTimeValue() >= endTime.GetTimeValue() || guildInfo->guild_boss_dead_time >= zeroTime.GetTimeValue())
    {   // 结束或boss死亡，返回时间为到明天的时间
        sendMsg.flag = 0;
        sendMsg.time = (now.GetHour() * 3600) + (now.GetMinute() * 60) + now.GetSecond();
        sendMsg.time = DAY_SECONDS - sendMsg.time;
        sendMsg.time = sendMsg.time + guildInfo->guild_boss_start_time;
    }
    else
    {   // 开始了
        sendMsg.flag = 1;
        sendMsg.time = (uint32)(now.GetTimeValue() - startTime.GetTimeValue());
    }

    // 向 客户端 发送请求进入公会怪物讨伐战请求
    player->SendToGate(MSG_GUILD_BOSS_COUNTDOWN_ACK, &sendMsg);
    return;

_ERR:
    player->NotifyErrorCode(errCode);
}

// ==================================================
// 公会BOSS设置时间请求
// ==================================================
void GuildManager::OnGuildBossSetTimeReq(Player* player, void* data)
{
    GUILD_BOSS_SET_TIME_REQ* recvMsg = (GUILD_BOSS_SET_TIME_REQ*)data;
    const GuildLevelTmpl* guildLevelTmpl = 0;
    GuildMap::iterator guildIt;
    std::vector<GUILD_MEMBER>::iterator memberIt;
    uint32 crusadeStartTime;
    uint64 memberID = 0;
    uint16 errCode = ERR_FAILED;

    if(!player) return;

    // 检查角色是否存在公会
    if(!player->GetGuildID())
    {
        errCode = ERR_HAVENT_GUILD;
        goto _ERR;
    }

    // 获取公会
    guildIt = m_guildMap.find(player->GetGuildID());
    if(guildIt == m_guildMap.end())
    {
        errCode = ERR_UNFOUND_GUILD;
        goto _ERR;
    }

    // 获取公会的等级模板
    guildLevelTmpl = GuildConfig::Instance()->GetGuildLeveTmpl(guildIt->second.level);
    if(!guildLevelTmpl)
    {
        goto _ERR;
    }

    // 检查是否已开启工会BOSS功能
    if(!guildLevelTmpl->HasActivity(GUILD_ACTIVITY_CRUSADE_BOSS))
    {
        goto _ERR;
    }

    // 查找公会成员
    for(memberIt = guildIt->second.member_list.begin(); memberIt != guildIt->second.member_list.end(); ++memberIt)
    {
        memberID = U64ID(memberIt->player_id_h, memberIt->player_id_l);
        if(memberID == player->GetPlayerID())
        {
            break;
        }
    }

    if(memberIt == guildIt->second.member_list.end())
    {
        goto _ERR;
    }

    // 检查是否为公会会长
    if(memberIt->office != GUILD_OFFICE_CHAIRMAN)
    {
        goto _ERR;
    }

    // 获取工会 BOSS 开启时间
    crusadeStartTime = GuildConfig::Instance()->GetCrusadeStartTime(recvMsg->start_time);
    if((crusadeStartTime == 0) || (crusadeStartTime > DAY_SECONDS))
    {
        goto _ERR;
    }

    recvMsg->start_time = crusadeStartTime;

    player->SendToCenter(MSG_GUILD_BOSS_SET_TIME_REQ, recvMsg);
    return;

_ERR:
    player->NotifyErrorCode(errCode);
}

// ==================================================
// 请求公会暮光之城排行信息
// ==================================================
void GuildManager::OnGuildCrusadeBossRankReq(Player* player, void* data)
{
    NONE* recvMsg = (NONE*)data;

    if(!recvMsg || !player) return;

    player->SendToCenter(MSG_GUILD_CRUSADE_BOSS_RANK_REQ, recvMsg);
}

// ==================================================
// 请求暮光之战开启ID
// ==================================================
//void GuildManager::OnGuildTwilightWarOpenIDReq(Player* player, void* data)
//{
//    GUILD_TWILIGHT_WAR_OPEN_ID sendMsg;
//
//    if(!player) return;
//
//    FillGuildTwilightWarOpenID(sendMsg);
//
//    player->SendToGate(MSG_GUILD_TWILIGHT_WAR_OPEN_ID_ACK, &sendMsg);
//}

// ==================================================
// 判断角色是否能进入魔物讨伐战
// ==================================================
uint16 GuildManager::CanEnterCrusadeMap(Player* player)
{
    const GuildLevelTmpl* guildLevelTmpl = 0;
    GuildMap::iterator guildIt;

    if(!player)
    {
        return ERR_FAILED;
    }

    // 检查角色是否存在公会
    if(!player->GetGuildID())
    {
        return ERR_HAVENT_GUILD;
    }

    // 获取公会
    guildIt = m_guildMap.find(player->GetGuildID());
    if(guildIt == m_guildMap.end())
    {
        return ERR_UNFOUND_GUILD;
    }

    // 获取公会的等级模板
    guildLevelTmpl = GuildConfig::Instance()->GetGuildLeveTmpl(guildIt->second.level);
    if(!guildLevelTmpl)
    {
        return ERR_FAILED;
    }

    // 检查是否已开启工会BOSS功能
    if(!guildLevelTmpl->HasActivity(GUILD_ACTIVITY_CRUSADE_BOSS))
    {
        return ERR_FAILED;
    }

    // 检查是否能进入公会BOSS战
    return CanEnterCrusadeBoss(&(guildIt->second));
}

// ==================================================
// 判断是否在工会战活动时间
// ==================================================
uint16 GuildManager::CanEnterCrusadeBoss(GUILD_INFO* guildInfo)
{
    Crown::CSDDateTime now = Crown::SDNow();
    Crown::CSDDateTime zeroTime;
    Crown::CSDDateTime startTime;
    Crown::CSDDateTime endTime;
    uint32 crusateEndTime = 0;

    if(!guildInfo) return ERR_CRUSADE_MONSTER_CLOSE;

    zeroTime = now;
    zeroTime.SetTime(0, 0, 0);

    startTime = zeroTime;
    startTime.IncSecond(guildInfo->guild_boss_start_time);

    crusateEndTime = GuildConfig::Instance()->GetCrusadeEndTime();
    endTime = startTime;
    endTime.IncSecond(crusateEndTime);

    // 检查当天公会BOSS是否已死亡过
    if(guildInfo->guild_boss_dead_time >= zeroTime.GetTimeValue())
    {
        return ERR_GUILD_BOSS_KILLED;
    }
    else
    {
        // 如果怪物还没创建过
        if((startTime.GetTimeValue() <= now.GetTimeValue()) && (now.GetTimeValue() < endTime.GetTimeValue()))
        {
            if(guildInfo->guild_boss_create_time < zeroTime.GetTimeValue())
            {
                return ERR_SUCCEED;
            }
            // 已创建，并且还没杀死
            else if((startTime.GetTimeValue() <= guildInfo->guild_boss_create_time) && 
                (guildInfo->guild_boss_create_time < endTime.GetTimeValue()))
            {
                return ERR_SUCCEED;
            }

            return ERR_CRUSADE_MONSTER_CLOSE;
        }
        else
        {
            // 没杀死，过期了
            return ERR_CRUSADE_MONSTER_CLOSE;
        }
    }

    return ERR_CRUSADE_MONSTER_CLOSE;
}

// 判断角色是否能进入暮光之战地图
uint16 GuildManager::CanEnterTwilightWarMap(Player* player)
{
    const GuildLevelTmpl* guildLevelTmpl = 0;
    GuildMap::iterator guildIt;

    if(!player)
    {
        return ERR_FAILED;
    }

    // 检查角色是否存在公会
    if(!player->GetGuildID())
    {
        return ERR_HAVENT_GUILD;
    }

    // 获取公会
    guildIt = m_guildMap.find(player->GetGuildID());
    if(guildIt == m_guildMap.end())
    {
        return ERR_UNFOUND_GUILD;
    }

    // 获取公会的等级模板
    guildLevelTmpl = GuildConfig::Instance()->GetGuildLeveTmpl(guildIt->second.level);
    if(!guildLevelTmpl)
    {
        return ERR_FAILED;
    }

    // 检查是否已开启工会BOSS功能
    if(!guildLevelTmpl->HasActivity(GUILD_ACTIVITY_TWILIGHT_WAR))
    {
        return ERR_FUNCT_NOT_OPEN;
    }

    // 检查暮光之城是否开启
    if(!TwilightWarManager::Instance()->IsStart())
    {
        return ERR_TWILIGHT_WAR_CLOSE;
    }

    return ERR_SUCCEED;
}

// 判断是否需要更新工会BOSS时间
bool GuildManager::NeedUpdateCrusadeBossTime(GUILD_INFO* guildInfo)
{
    Crown::CSDDateTime now = Crown::SDNow();
    Crown::CSDDateTime zeroTime;
    Crown::CSDDateTime startTime;

    if(!guildInfo) return false;

    zeroTime = now;
    zeroTime.SetTime(0, 0, 0);

    startTime = zeroTime;
    startTime.IncSecond(guildInfo->guild_boss_start_time);

    if(guildInfo->guild_boss_create_time < startTime.GetTimeValue())
    {
        return true;
    }

    return false;
}

//// ==================================================
//// 判断进入暮光之战的时间
//// ==================================================
//uint16 GuildManager::CanEnterTwilightWar()
//{
//    Crown::CSDDateTime now = Crown::SDNow();
//    Crown::CSDDateTime startTime;
//    Crown::CSDDateTime endTime;
//    const GuildActivityStartTimeVert* startTimeList;
//    uint32 twilightWarEndTime = 0;
//    uint32 bossDeadTime = 0;
//
//    // 上次暮光之战怪物
//    uint32 boss_id = WorldBossManager::Instance()->GetBossID_TwilightWar();
//    bossDeadTime = WorldBossManager::Instance()->GetBossDeadTime(boss_id);
//
//    // 暮光之战开启时间列表
//    startTimeList = GuildConfig::Instance()->GetTwilightWarStartTime();
//
//    // 暮光之战战持续时间
//    twilightWarEndTime = GuildConfig::Instance()->GetTwilightWarEndTime();
//
//    if(!startTimeList) return ERR_TWILIGHT_WAR_CLOSE;
//
//    for(GuildActivityStartTimeVert::const_iterator it = startTimeList->begin(); it != startTimeList->end(); ++it)
//    {
//        // 检查怪物开启时间
//        startTime = now;
//        startTime.SetTime(it->start_time/3600, it->start_time%3600/60, it->start_time%60);
//
//        // 设置结束时间
//        endTime = startTime;
//        endTime.IncSecond(twilightWarEndTime);
//
//        if((startTime.GetTimeValue() <= now.GetTimeValue()) && (now.GetTimeValue() < endTime.GetTimeValue()))
//        {
//            if((startTime.GetTimeValue() <= bossDeadTime) && (bossDeadTime < endTime.GetTimeValue()))
//            {
//                return ERR_TWILIGHT_WAR_BOSS_DEAD;
//            }
//            return ERR_SUCCEED;
//        }
//    }
//
//    return ERR_TWILIGHT_WAR_CLOSE;
//}

// ==================================================
// 请求公会暮光之城排行信息
// ==================================================
void GuildManager::OnGuildTwilightWarRankReq(Player* player, void* data)
{
    NONE* recvMsg = (NONE*)data;

    if(!recvMsg || !player) return;

    player->SendToCenter(MSG_GUILD_TWILIGHT_WAR_RANK_REQ, recvMsg);
}

// ==================================================
// 获得最近一次进入暮光之城的ID
// ==================================================
//uint32 GuildManager::GetEnterTwilightWarID()
//{
//    Crown::CSDDateTime now = Crown::SDNow();
//    Crown::CSDDateTime startTime;
//    Crown::CSDDateTime endTime;
//    const GuildActivityStartTimeVert* startTimeList = 0;
//    uint32 twilightWarEndTime = 0;
//    uint32 bossDeadTime = 0;
//
//    // 上次暮光之战怪物
//    uint32 boss_id = WorldBossManager::Instance()->GetBossID_TwilightWar();
//    bossDeadTime = WorldBossManager::Instance()->GetBossDeadTime(boss_id);
//
//    // 暮光之战开启时间列表
//    startTimeList = GuildConfig::Instance()->GetTwilightWarStartTime();
//
//    // 暮光之战战持续时间
//    twilightWarEndTime = GuildConfig::Instance()->GetTwilightWarEndTime();
//
//    if(!startTimeList || (startTimeList->size() < 0)) return 0;
//
//    for(GuildActivityStartTimeVert::const_iterator it = startTimeList->begin(); it != startTimeList->end(); ++it)
//    {
//        // 检查怪物开启时间
//        startTime = now;
//        startTime.SetTime(it->start_time/3600, it->start_time%3600/60, it->start_time%60);
//
//        // 设置结束时间
//        endTime = startTime;
//        endTime.IncSecond(twilightWarEndTime);
//
//        if(now.GetTimeValue() < startTime.GetTimeValue())
//        {
//            return it->id;
//        }
//
//        if((startTime.GetTimeValue() <= now.GetTimeValue()) && (now.GetTimeValue() < endTime.GetTimeValue()))
//        {
//            if((startTime.GetTimeValue() <= bossDeadTime) && (bossDeadTime < endTime.GetTimeValue()))
//            {
//                continue;
//            }
//            return it->id;
//        }
//    }
//
//    return 1;
//}

// ==================================================
// 发送公会BOSS时间
// ==================================================
void GuildManager::SendRefreshBossTime(uint64 guildID, int32 flag)
{
    GUILD_CRUSADE_BOSS_TIME sendMsg;

    sendMsg.guild_id.id_h = U64ID_HIGH(guildID);
    sendMsg.guild_id.id_l = U64ID_LOW(guildID);
    sendMsg.flag = flag;

    CenterHandler::Instance()->SendToMasterCenter(MSG_GUILD_CRUSADE_BOSS_TIME_SAVE, &sendMsg);
}

// ==================================================
// 更新公会信息
// ==================================================
void GuildManager::OnRefreshGuildInfo(GUILD_INFO* guildInfo)
{
    GuildMap::iterator guildIt;
    uint64 guildID = 0;

    if(!guildInfo) return;

    CnAssert(guildInfo->center_server_id != 0);     // 公会的center server id不应该为0

    guildID = U64ID(guildInfo->guild_id_h, guildInfo->guild_id_l);
    guildIt = m_guildMap.find(guildID);
    if(guildIt == m_guildMap.end())
    {
        m_guildMap[guildID] = *guildInfo;
    }
    else
    {
        guildIt->second = *guildInfo;
    }
}

// ==================================================
// 添加公会信息
// ==================================================
void GuildManager::OnAddGuildInfo(GUILD_INFO* guildInfo)
{
    GuildMap::iterator guildIt;
    uint64 guildID = 0;

    if(!guildInfo) return;

    guildID = U64ID(guildInfo->guild_id_h, guildInfo->guild_id_l);
    guildIt = m_guildMap.find(guildID);
    if(guildIt == m_guildMap.end())
    {
        return;
    }

    FillGuildUpdateInfo(guildInfo, guildIt->second);

    for(std::vector<GUILD_MEMBER>::iterator it = guildInfo->member_list.begin();
        it != guildInfo->member_list.end(); ++it)
    {
        guildIt->second.member_list.push_back(*it);
    }
}

// ==================================================
// 删除队伍信息
// ==================================================
void GuildManager::OnDelGuildInfo(GUILD_INFO* guildInfo)
{
    GuildMap::iterator guildIt;
    uint64 guildID = 0;

    if(!guildInfo) return;

    // 获取队伍
    guildID = U64ID(guildInfo->guild_id_h, guildInfo->guild_id_l);
    guildIt = m_guildMap.find(guildID);
    if(guildIt == m_guildMap.end())
    {
        return;
    }

    FillGuildUpdateInfo(guildInfo, guildIt->second);

    // 删除队员
    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildInfo->member_list.begin();
        memberIt != guildInfo->member_list.end(); ++memberIt)
    {
        for(std::vector<GUILD_MEMBER>::iterator delIt = guildIt->second.member_list.begin();
            delIt != guildIt->second.member_list.end(); ++delIt)
        {
            if((memberIt->player_id_h == delIt->player_id_h) && 
                (memberIt->player_id_l == delIt->player_id_l))
            {
                guildIt->second.member_list.erase(delIt);
                break;
            }
        }
    }

    // 如果成员为空则删除队伍
    if(guildIt->second.member_list.size() < 1)
    {
        m_guildMap.erase(guildIt);
    }
}

// ==================================================
// 更新公会信息
// ==================================================
void GuildManager::OnUpdateGuildInfo(GUILD_INFO* guildInfo)
{
    GuildMap::iterator guildIt;
    uint64 guildID = 0;

    if(!guildInfo) return;

    // 获取队伍
    guildID = U64ID(guildInfo->guild_id_h, guildInfo->guild_id_l);
    guildIt = m_guildMap.find(guildID);
    if(guildIt == m_guildMap.end())
    {
        return;
    }

    FillGuildUpdateInfo(guildInfo, guildIt->second, OPT_INIT_FLAG);

    if(guildInfo->member_list.size() > 0)
    {
        // 更新队员
        for(std::vector<GUILD_MEMBER>::iterator memberIt = guildInfo->member_list.begin();
            memberIt != guildInfo->member_list.end(); ++memberIt)
        {
            for(std::vector<GUILD_MEMBER>::iterator updateIt = guildIt->second.member_list.begin();
                updateIt != guildIt->second.member_list.end(); ++updateIt)
            {
                if((memberIt->player_id_h == updateIt->player_id_h) && 
                    (memberIt->player_id_l == updateIt->player_id_l))
                {
                    *updateIt = *memberIt;
                    break;
                }
            }
        }
    }
}

//// ==================================================
//// 更新公会属性信息
//// ==================================================
//void GuildManager::OnUpdateGuildProp(GUILD_INFO* guildInfo)
//{
//    GuildMap::iterator guildIt;
//    uint64 guildID = 0;
//
//    if(!guildInfo) return;
//
//    // 获取队伍
//    guildID = U64ID(guildInfo->guild_id_h, guildInfo->guild_id_l);
//    guildIt = m_guildMap.find(guildID);
//    if(guildIt == m_guildMap.end())
//    {
//        return;
//    }
//
//    guildIt->second.battle_power = guildInfo->battle_power;
//
//    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildInfo->member_list.begin();
//        memberIt != guildInfo->member_list.end(); ++memberIt)
//    {
//        for(std::vector<GUILD_MEMBER>::iterator updateIt = guildIt->second.member_list.begin();
//            updateIt != guildIt->second.member_list.end(); ++updateIt)
//        {
//            if((memberIt->player_id_h == updateIt->player_id_h) && 
//                (memberIt->player_id_l == updateIt->player_id_l))
//            {
//                *updateIt = *memberIt;
//                break;
//            }
//        }
//    }
//}

// ==================================================
// 销毁队伍信息
// ==================================================
void GuildManager::OnDestoryGuildInfo(GUILD_INFO* guildInfo)
{
    uint64 playerID = 0;
    Player* player = 0;
    GuildMap::iterator guildIt;
    uint64 guildID = 0;

    if(!guildInfo) return;

    // 拼队伍ID
    guildID = U64ID(guildInfo->guild_id_h, guildInfo->guild_id_l);

    // 查找队伍
    guildIt = m_guildMap.find(guildID);
    if(guildIt == m_guildMap.end())
    {
        return;
    }

    m_guildMap.erase(guildIt);
}

// ==================================================
// 填充工会信息
// ==================================================
void GuildManager::FillGuildUpdateInfo(GUILD_INFO* guildInfo, GUILD_INFO& sendMsg, uint8 opt_flag)
{
    if(!guildInfo) return;

    sendMsg.opt_flag = opt_flag;
    sendMsg.guild_id_h = guildInfo->guild_id_h;
    sendMsg.guild_id_l = guildInfo->guild_id_l;
    sendMsg.guild_name = guildInfo->guild_name;
    sendMsg.level = guildInfo->level;
    sendMsg.exp = guildInfo->exp;
    sendMsg.flag = guildInfo->flag;
    sendMsg.battle_power = guildInfo->battle_power;
    sendMsg.bulletin = guildInfo->bulletin;
    sendMsg.guild_boss_level = guildInfo->guild_boss_level;
    sendMsg.guild_boss_exp = guildInfo->guild_boss_exp;
    sendMsg.guild_boss_start_time = guildInfo->guild_boss_start_time;
    sendMsg.guild_boss_create_time = guildInfo->guild_boss_create_time;
    sendMsg.addition_list = guildInfo->addition_list;
}

// ==================================================
// 升级公会BOSS
// ==================================================
void GuildManager::StrongGuildBoss(Boss* boss, int32 strongLevel)
{
    if(strongLevel < 0) 
        strongLevel = 0;
    uint32 maxhp = FormulaManager::Instance()->CalcGuildBossHp(boss, strongLevel);
    boss->SetMaxHp(maxhp);
    boss->SetHP(maxhp);

    uint32 attack = FormulaManager::Instance()->CalcGuildBossAttack(boss, strongLevel);
    uint32 defence = FormulaManager::Instance()->CalGuildBossDefence(boss,strongLevel);

    BattleUnitCommunicator* pCommunicator = boss->GetCommunicator();
    if(pCommunicator)
    {
        pCommunicator->SetProp(PROP_ATTACK, attack);
        pCommunicator->SetProp(PROP_ARMOR,defence);
    }
}

// ==================================================
// 
// ==================================================
void GuildManager::ExitGame(Player* player)
{
    //if(!player)
    //    return;

    //if(GetGuildTrunTablePos())
    //{
    //    NONE sendMsg;
    //    OnRandomTurntableResult(player,&sendMsg);
    //}
    //if(GetGuildMemberTrunTablePos())
    //{
    //    NONE mebsendMsg;
    //    OnMemberTurntableResult(player,&mebsendMsg);
    //}
}

// ==================================================
// 得到公会加成信息
// ==================================================
GUILD_ADDITION* GuildManager::GetGuildAdditionItem(uint64 guildID, int32 additionID)
{
    // 获取公会
    GUILD_INFO* guildInfo = GuildManager::Instance()->GetGuildInfo(guildID);
    if(!guildInfo)
        return NULL;

    for(std::vector<GUILD_ADDITION>::iterator additionIt = guildInfo->addition_list.begin(); additionIt != guildInfo->addition_list.end(); ++additionIt)
    {
        if(additionIt->id == additionID)
            return &(*additionIt);
    }

    return NULL;
}
