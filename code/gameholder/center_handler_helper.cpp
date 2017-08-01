#include "gameholder_pch.h"
#include "center_handler.h"
#include "gate_handler.h"
#include "center_handler_helper.h"
#include "trade_manager.h"
#include "player_manager.h"
#include "timing_trigger_manager.h"
#include "city_fight_manager.h"
#include "city_survival_manager.h"
#include "void_field_manager.h"
#include "void_field_solo_manager.h"
#include "game_setting_manager.h"
#include "multiserver_battle_manager.h"
#include "world_map_manager.h"
#include "answer_question_manager.h"
#include "special_activity_manager_gs.h"
#include "special_activity_config.h"
#include "ranking_manager.h"
#include "top_up_rank_manager.h"
#include "world_boss_manager.h"
#include "ashura_manager.h"
#include "faction_manager.h"
#include "tower_defence_manager.h"
#include "twilight_war_manager.h"
#include "capture_flag_manager.h"
#include "task_festive_config.h"
#include "global_setting_manager.h"
#include "proclamation_manager.h"
#include "offline_player_manager.h"
#include "team_manager.h"
#include "activity_manager.h"
#include "guild_manager.h"
#include "shop_manager.h"
#include "day_investment_config.h"
#include "day_investment_manager.h"
#include "level_investment_manager.h"
#include "level_activity_manager.h"
#include "mail_manager.h"
#include "mail_title_config.h"
#include "activity_reward_config.h"
#include "item_manager.h"
#include "player_manager.h"
#include "dungeon.h"
#include "dungeon_set.h"
#include "dungeon_manager.h"
#include "scene.h"
#include "trigger.h"
#include "event_define.h"
#include "vip_statistics_manager.h"
#include "offline_event_game_manager.h"
#include "limitbuy_manager.h"
#include "player_saver.h"
#include "cross_team_manager.h"
#include "daily_fashion_manager.h"
#include "random_package_manager.h"


CenterHandlerHelper::CenterHandlerHelper(CenterHandler* centerHandler) :
m_CenterHandler(centerHandler)
{

}

CenterHandlerHelper::~CenterHandlerHelper()
{

}

void CenterHandlerHelper::OnRecv( uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata )
{
    bool IsMasterCenter = m_CenterHandler->IsMasterCenterBySessionID(sessionID);
    CenterConnecter* pCenterConnecter = m_CenterHandler->GetConnecterBySessionID(sessionID);
    if(!pCenterConnecter)
    {
        CnAssert(false);
        return;
    }
    uint32 centerID = pCenterConnecter->m_centerID;
    CnAssert(centerID);         // centerID不会为0的

    switch(msgID)
    {
    case MSG_KICKOUT_GAME:
        RecvKickoutGame(playerID, data);
        break;
    case MSG_PLAYER_PROP_LIST:
        RecvPlayerPropList(playerID, data);
        break;
    case MSG_PLAYER_DOOR_INFO_ACK:
        RecvRefreshAOfflinePlayerAck(playerID, msgID, data);
        break;
    case MSG_OFFLINE_PLAYER_INFO_ACK:
        RecvOfflinePlayerRecordAck(msgID, data);
        break;
    case MSG_CHANGE_PLAYER_NAME_ACK:
        RecvChangePlayerNameAck(playerID, msgID, data);
        break;
    case MSG_FRIEND_ADD_REQ:
        RecvFriendAddSCAck(playerID, msgID, data);
        break;
    case MSG_FRIEND_INFO_NTF:
        RecvFriendInfoNtf(playerID, msgID, data);
        break;
    case MSG_TEAM_INFO_NTF:
    case MSG_TEAM_MEMBER_RESET_STATE:
        RecvTeamMsg(playerID, msgID, data);
        break;
    case MSG_MAIL_INFO_NTF:
        RecvMailInfoNtf(playerID, msgID, data);
        break;
    case MSG_MAIL_DEL_EXPIRED_MAIL_NTF:
        RecvMailDelExpiredMailNtf(playerID, msgID, data);
        break;
    case MSG_CREATE_GUILD_ACK:
    case MSG_GUILD_INFO_NTF:
    case MSG_GUILD_ID_NTF:
    case MSG_GUILD_CRUSADE_BOSS_TIME_SAVE:
        RecvGuildMsg(playerID, msgID, data);
        break;
    case MSG_GUILD_ADDITION_PROMOTE_TO_GAME:
        RecvGuildScienceMsg(playerID, msgID, data);
        break;
    case MSG_WORLD_BOSS_START_NTF:
    case MSG_WORLD_BOSS_END_NTF:
        RecvWorldBossMsg(playerID, msgID, data);
        break;
    case MSG_ACTIVITY_LIST_NTF:
        RecvActivityListNtf(playerID, msgID, data);
        break;
    case MSG_ADVENTURE_ROB_ACK:
        RecvAdventureMsg(playerID, msgID, data);
        break;
    case MSG_MALL_INFO_NOTIFY:
    case MSG_MALL_BUY_ACK:
    case MSG_MALL_BUY_NTF:
    case MSG_MALL_INFO_REQ:
        RecvMallMsg(playerID, msgID, data);
        break;
    case MSG_RANKING_RANK_ACK:
    case MSG_RANKING_PLAYER_ID_ACK:
    case MSG_RANKING_CHANGE_ACK:
        RecvRankAck(playerID, msgID, data);
        break;
    case MSG_RANKING_DAILY_REFRESH:
    case MSG_RANKING_TOP_RANK_NTF:
    case MSG_RANKING_HOURS_FRESH_RANK_NTF:
        RankingManager::Instance()->OnRankingRecv(playerID, msgID, data);
        break;
    case MSG_RANKING_TOP_UP_RANK_QUERY_ACK:
        {
            TopUpRankManager::Instance()->OnTopUpRankQueryAck(playerID, msgID, data);
        }
        break;
    case MSG_ACTIVECODE_USE_ACK:
        RecvActiveCodeMsg(playerID, msgID, data);
        break;
    case MSG_MODIFY_CHAT:
        OnSetRight(playerID,msgID,data);
        break;
    case MSG_SAVE_DAY_INVESTMENT_INFO_ACK:
        RecvSaveDayInvestmentAck(playerID, sessionID, msgID, data);
        break;
    case MSG_SAVE_LEVEL_INVESTMENT_INFO_ACK:
        RecvSaveLevelInvestmentAck(playerID, sessionID, msgID, data);
        break;
    case MSG_SAVE_LEVEL_ACTIVITY_INFO_ACK:
        RecvSaveLevelActivityAck(playerID, sessionID, msgID, data);
        break;
    case MSG_WORLD_BOSS_INFO_LOAD:
        WorldBossManager::Instance()->OnRecv(msgID, data);
        break;
    case MSG_ASHURA_START_NTF:
    case MSG_ASHURA_END_NTF:
        AshuraManager::Instance()->OnRecv(msgID, data);
        break;
    case MSG_ASHURA_START_ANNOUNCE:
        AshuraManager::Instance()->OnRecv(msgID, data);
        break;
    case MSG_FACTION_PK_START_NTF:
    case MSG_FACTION_PK_END_NTF:
        FactionManager::Instance()->OnRecv(NULL, msgID, data);
        break;
    case MSG_TD_START_NTF:
    case MSG_TD_END_NTF:
        TowerDefenceManager::Instance()->OnRecv(msgID, data);
        break;
    case MSG_GUILD_TWILIGHT_WAR_OPEN_NTF:
    case MSG_GUILD_TWILIGHT_WAR_CLOSE_NTF:
        TwilightWarManager::Instance()->OnRecv(msgID, data);
        break;
    case MSG_CAPTURE_FLAG_START_NTF:
    case MSG_CAPTURE_FLAG_END_NTF:
        CaptureFlagManager::Instance()->OnRecv(msgID, data);
        break;
    case MSG_PLAYER_RECHARGE_INFO_NTF:
        RecvPlayerRechargeInfoNtf(playerID, msgID, data);
        break;
    case MSG_TASK_FESTIVAL_TMPL_LIST_ACK:
        TaskFestiveConfig::Instance()->OnRecvFromCenter(centerID, msgID, data);
        break;
    case MSG_GLOBAL_SETTING_INFO_LOAD:
        GlobalSettingManager::Instance()->OnRecv(msgID, data);
        break;
    case MSG_GLOBAL_SETTING_PROCLAMATION_ACK:
        ProclamationManager::Instance()->OnRecv(msgID, data);
        break;
    case MSG_IS_UPDATE_PLAYER_NAME_ACK:
        RecvChangePlayerNameAck(playerID, msgID, data);
        break;
    case MSG_LIMIT_BUY_INFO_CLEAR_REQ:
        {
            Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
            if(player)
                player->OnRecv(msgID, data);
        }
        break;
    case MSG_PLAYER_LIKE_REQ:
        {
            Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
            if(player)
                player->OnRecv(msgID, data);
        }
        break;
    case MSG_DAILY_FASHION_DB_INFO_ACK:
        {
             DailyFashionManager::Instance()->OnRecv(playerID, msgID, data);
        }
        break;
    case MSG_SERVER_PACKAGE_RECORD_LOAD:
        {
            ServerRandomPackageManager::Instance()->OnRecv(msgID, data);
        }
        break;
    default:
        break;
    }

    if((MSG_ANSWER_QUESTION_BEGIN <= msgID) && (msgID <= MSG_ANSWER_QUESTION_END))      // 3000
    {
        if(IsMasterCenter)
            RecvAnswerMsg(playerID, msgID, data);   // 答题相关只接受主CenterServer的数据
    }
    else if((MSG_TRADE_ORDER_BEGIN <= msgID) && (msgID <= MSG_TRADE_ORDER_END))         // 4700
    {
        TradeManager::Instance()->OnCSRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_SPECIAL_ACTIVITY_MANAGER_BEGIN <= msgID) && (msgID <= MSG_SPECIAL_ACTIVITY_MANAGER_END))       // 5900
    {
        // 查找角色
        Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
        if(player)
        {   // 玩家在线,直接让特殊活动模块处理
            player->OnRecv(msgID, data);
        }
        else
        {   // 玩家不在线了,涉及到领奖的时候,发邮件给奖励
            if (msgID == MSG_SPECIAL_ACTIVITY_SAVE_INFO_ACK)
            {
                RecvSaveSpecialActivityAck(playerID, sessionID, msgID, data);
            }
        }
    }
    else if((MSG_TIMING_TRIGGER_BEGIN <= msgID) && (msgID <= MSG_TIMING_TRIGGER_END))       // 6400
    {
        TimingTriggerManager::Instance()->OnRecv(msgID, data);
    }
    else if((MSG_CITY_FIGHT_BEGIN <= msgID) && (msgID <= MSG_CITY_FIGHT_END))               // 6500
    {
        CityFightManager::Instance()->OnCSRecv(playerID, msgID, data);
    }
    else if((MSG_CITY_SURVIVAL_BEGIN <= msgID) && (msgID <= MSG_CITY_SURVIVAL_END))         // 6800
    {
        CitySurvivalManager::Instance()->OnCSRecv(playerID, msgID, data);
    }
    else if((MSG_CROSS_TEAM_BEGIN <= msgID) && (msgID <= MSG_CROSS_TEAM_END))               // 7700
    {
        CrossTeamManager::Instance()->OnRecvFromCenter(playerID, msgID, data);
    }
    else if((MSG_VOID_FIELD_BEGIN <= msgID) && (msgID <= MSG_VOID_FIELD_END))               // 8200
    {
        VoidFieldManager::Instance()->OnRecv(msgID, data);
    }
    else if((MSG_VOID_FIELD_SOLO_BEGIN <= msgID) && (msgID <= MSG_VOID_FIELD_SOLO_END))     // 8250
    {
        VoidFieldSoloManager::Instance()->OnRecv(msgID, data);
    }
    else if((MSG_GAME_SETTING_BEGIN <= msgID) && (msgID <= MSG_GAME_SETTING_END))           // 9100
    {
        GameSettingManager::Instance()->OnRecv(msgID, data);
    }
    else if((MSG_MULTISERVER_BATTLE_BEGIN <= msgID) && (msgID <= MSG_MULTISERVER_BATTLE_END))   // 9200
    {
        MultiserverBattleManager::Instance()->OnRecvFromCenter(msgID, data);
    }
    else if ((MSG_FIELD_BOSS_BEGIN <= msgID) && (msgID <= MSG_FIELD_BOSS_END))              // 9300
    {
        WorldMapManager::Instance()->OnRecv(msgID, data);
    }
    else if((MSG_OFFLINE_EVENT_BEGIN <= msgID) && (msgID <= MSG_OFFLINE_EVENT_END))         // 9600
    {
        OfflineEventGameManager::Instance()->OnRecv(playerID, msgID, data);
    }
    else if((MSG_DONATE_BEGIN <= msgID) && (msgID <= MSG_DONATE_END))                       // 9700
    {
        Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
        if(player)
        {
            player->OnRecv(msgID, data);
        }
    }
    else if((MSG_VIP_STATISTICS_BEGIN <= msgID) && msgID <= MSG_VIP_STATISTICS_END)         // 9800
    {
        VipStatisticsManager::Instance()->OnRecv(msgID, data);
    }

    
}

// ==================================================
// 收到踢出游戏的消息
// ==================================================
void CenterHandlerHelper::RecvKickoutGame(uint64 playerID, void* data)
{
    CnAssert(false);            // 无此流程
    KICKOUT_GAME* recvMsg = (KICKOUT_GAME*)data;
    if(!recvMsg) return;

    // 查找角色
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return;

    // 角色已经存在，踢下线
    Scene* pScene = DungeonManager::Instance()->GetScene(player->GetUnionMapID());
    if(pScene)
        pScene->DelPlayer(player->GetId());

    PlayerManager::Instance()->DelPlayer(playerID);
}

// ==================================================
// 接收角色属性信息
// ==================================================
void CenterHandlerHelper::RecvPlayerPropList(uint64 playerID, void* data)
{
    Player* player = 0;
    PROPERTY_LIST_NTF* recvMsg = (PROPERTY_LIST_NTF*)data;

    if(!recvMsg) return;

    // 查找角色
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return;

    for(std::vector<PROPERTY_NTF>::iterator propIt = recvMsg->list.begin();
        propIt != recvMsg->list.end(); ++propIt)
    {
        switch(propIt->property_id)
        {
        case PROP_MONEYPOINT:
            player->SetMoneyPoint(propIt->value);
            break;
        default:
            CnAssert(false);
            break;
        }
    }
}

// ==================================================
// 收到角色的离线信息
// ==================================================
void CenterHandlerHelper::RecvRefreshAOfflinePlayerAck(uint64 playerID, uint32 msgID, void* data)
{
    // 查找角色
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return;

    player->OnRecv(msgID, data);
}

void CenterHandlerHelper::RecvOfflinePlayerRecordAck(uint32 msgID, void* data)
{
    OFFLINE_PLAYER_ACK* info = (OFFLINE_PLAYER_ACK*)data;

    OfflinePlayerManager::Instance()->CBRequestOfflinePlayerInfo(*info);
}

// ==================================================
// 修改角色名称应答
// ==================================================
void CenterHandlerHelper::RecvChangePlayerNameAck(uint64 playerID, uint32 msgID, void* data)
{
    // 查找角色
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return;

    player->OnRecv(msgID, data);
}

// ==================================================
// 接收队伍信息
// ==================================================
void CenterHandlerHelper::RecvTeamMsg(uint64 playerID, uint32 msgID, void* data)
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    TeamManager::Instance()->OnRecvFromCenter(player, msgID, data);
}

// ==================================================
// 添加好友
// ==================================================
void CenterHandlerHelper::RecvFriendAddSCAck(uint64 playerID, uint32 msgID, void* data)
{
    // 查找角色
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return;

    player->OnRecv(msgID, data);
}

// ==================================================
// 好友通知
// ==================================================
void CenterHandlerHelper::RecvFriendInfoNtf(uint64 playerID, uint32 msgID, void* data)
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return;

    player->OnRecv(msgID, data);
}

// ==================================================
// 接收世界BOSS信息
// ==================================================
void CenterHandlerHelper::RecvWorldBossMsg(uint64 playerID, uint32 msgID, void* data)
{
    WorldBossManager::Instance()->OnRecv(msgID, data);
}

// ==================================================
// 接收活动信息
// ==================================================
void CenterHandlerHelper::RecvActivityListNtf(uint64 playerID, uint32 msgID, void* data)
{
    ActivityManager::Instance()->OnCSRecv(msgID, data);
}

// ==================================================
// 获得运镖消息
// ==================================================
void CenterHandlerHelper::RecvAdventureMsg(uint64 playerID, uint32 msgID, void* data)
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(player)
        player->OnRecv(msgID, data);
}

// ==================================================
// 接收公会信息
// ==================================================
void CenterHandlerHelper::RecvGuildMsg(uint64 playerID, uint32 msgID, void* data)
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    GuildManager::Instance()->OnRecv(player, msgID, data);
}

void CenterHandlerHelper::RecvGuildScienceMsg(uint64 playerID, uint32 msgID, void* data)
{
    Player* pPlayer = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!pPlayer)
    {
        CnAssert(false);
        return;
    }

    pPlayer->OnRecv(msgID, data);
}

// ==================================================
// 接收购买数据
// ==================================================
void CenterHandlerHelper::RecvMallMsg(uint64 playerID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_MALL_INFO_NOTIFY:
        ShopManager::OnMallInfoNotify(data);
        break;
    case MSG_MALL_BUY_ACK:
        ShopManager::OnMallBuyAck(playerID, data);
        break;
    case MSG_MALL_BUY_NTF:
        ShopManager::OnMallBuyNtf(playerID, data);
        break;
    case MSG_MALL_INFO_REQ:
        {
            const PlayerManager::PlayerMap& playerMap = PlayerManager::Instance()->GetPlayers();

            for (PlayerManager::PlayerMap::const_iterator playerIt = playerMap.begin(); playerIt != playerMap.end(); playerIt++)
            {
                if (playerIt->second->GetNetState() == Player::kStateInGame)
                {
                    playerIt->second->GetShopManager()->GMMallInfoReq();
                }
            }
        }
        break;
    default:
        CnAssert(false);
        break;
    }
}

// ==================================================
// 接收排名数据
// ==================================================
void CenterHandlerHelper::RecvRankAck(uint64 playerID, uint32 msgID, void* data)
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(player)
    {
        player->OnRecv(msgID, data);
    }
}

// ==================================================
// 设置禁言标识
// ==================================================
void CenterHandlerHelper::OnSetRight(uint64 playerID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_MODIFY_CHAT:
        {
            CHAT_MSG* recvMsg= (CHAT_MSG*)data;
            int32 flag = recvMsg->src_flag;
            Player* player = 0;
            player = PlayerManager::Instance()->GetPlayerByName(recvMsg->src_name);
            if(player)
            {
                player->SetRightFlag(flag);
            }
        }
    }
}

// ==================================================
// 获得激活码消息
// ==================================================
void CenterHandlerHelper::RecvActiveCodeMsg(uint64 playerID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_ACTIVECODE_USE_ACK:
        OnActiveCodeUseAck(playerID, data);
        break;
    default:
        break;
    }
}

void CenterHandlerHelper::RecvSaveDayInvestmentAck( uint64 playerID, uint32 sessionID, uint32 msgID, void* data )
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    // 玩家在线直接给
    if (player && (player->GetNetState() >= Player::kStateInGame))
    {
        player->GetDayInvestmentManager()->CBSaveDayInvestment(data);
        return;
    }

    // 玩家不在线,发邮件
    SAVE_DAY_INVESTMENT_INFO_ACK* ack = (SAVE_DAY_INVESTMENT_INFO_ACK*) data;
    if (!ack)
        return;

    DayInvestmentInfo* info = DayInvestmentConfig::Instance()->GetDayInvestmentInfo(ack->day);
    if (!info)
        return;

    CenterConnecter* connecter = m_CenterHandler->GetConnecterBySessionID(sessionID);
    if(!connecter)
        return;

    std::vector<REWARD_ITEM> list;
    REWARD_ITEM attachment;
    //attachment.type = info->reward.type;
    attachment.id = info->reward.id;
    attachment.num = info->reward.num;
    list.push_back(attachment);

    MailManager::SendMail(connecter->m_centerID, playerID, "", "", Crown::ToString(MAIL_TITLE_ID_REWARD_COMPENSATION), MAIL_TYPE_PLAYER, list, playerID);

}

void CenterHandlerHelper::RecvSaveLevelInvestmentAck( uint64 playerID, uint32 sessionID, uint32 msgID, void* data )
{
    SAVE_LEVEL_INVESTMENT_INFO_ACK* ack = 0;
    Player* player = 0;
    std::vector<REWARD_ITEM> list;
    REWARD_ITEM attachment;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
        return;

    ack = (SAVE_LEVEL_INVESTMENT_INFO_ACK*) data;
    if (!ack)
        return;

    CenterConnecter* connecter = m_CenterHandler->GetConnecterBySessionID(sessionID);
    if(!connecter)
        return;

    // 玩家在线直接给
    if (player->GetNetState() >= Player::kStateInGame)
    {
        player->GetLevelInvestmentManager()->CBSaveLevelInvestment(data);
        return;
    }

    // 玩家不在线,发邮件
    attachment.id = PROP_MONEYPOINT;
    //attachment.id = 0;
    attachment.num = ack->reward_money;
    list.push_back(attachment);

    MailManager::SendMail(connecter->m_centerID, playerID, "", "", Crown::ToString(MAIL_TITLE_ID_REWARD_COMPENSATION), MAIL_TYPE_PLAYER, list, playerID);
}

// ==================================================
// 接收邮件信息
// ==================================================
void CenterHandlerHelper::RecvMailInfoNtf(uint64 playerID, uint32 msgID, void* data)
{
    MAIL_INFO* recvMsg = (MAIL_INFO*)data;
    Player* player = NULL;

    if(!recvMsg) return;

    if((recvMsg->mail_type == MAIL_TYPE_PLAYER) || 
        (recvMsg->mail_type == MAIL_TYPE_PLAYER_SYS) ||
        (recvMsg->mail_type == MAIL_TYPE_GROUP))
    {
        player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
        if(player)
        {
            player->AddMail(recvMsg);
        }
    }
    else if(recvMsg->mail_type == MAIL_TYPE_SYSTEM)
    {
        PlayerManager::Instance()->AddMailToAll(recvMsg);
    }
}

// ==================================================
// 删除过期邮件
// ==================================================
void CenterHandlerHelper::RecvMailDelExpiredMailNtf(uint64 playerID, uint32 msgID, void* data)
{
    MAIL_DEL_EXPIRED_MAIL_NTF* recvMsg = (MAIL_DEL_EXPIRED_MAIL_NTF*)data;

    if(!recvMsg) return;

    PlayerManager::Instance()->DelExpiredMail(recvMsg->mail_id_h);
}

void CenterHandlerHelper::RecvAnswerMsg(uint64 playerID, uint32 msgID, void* data)
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    AnswerQuestionManager::Instance()->OnRecv(player, msgID, data);
}

void CenterHandlerHelper::RecvSaveSpecialActivityAck( uint64 playerID, uint32 sessionID, uint32 msgID, void* data )
{
    SPECIAL_ACTIVITY_STORE_INFO* ack = (SPECIAL_ACTIVITY_STORE_INFO*) data;
    if (!ack)
        return;

    CenterConnecter* connecter = m_CenterHandler->GetConnecterBySessionID(sessionID);
    if(!connecter)
        return;

    // 玩家不在线,发邮件
    const SpecialActivityInfo* info = SpecialActivityConfig::Instance()->GetActivityInfoByIdAndParam1(ack->activity_id, ack->param);
    if (info)
    {
        const REWARD_TABLE* reward = ActivityRewardConfig::Instance()->GetReward(info->rewardId);
        if (reward)
        {
            MailManager::SendMail(connecter->m_centerID, playerID, "", "", Crown::ToString(MAIL_TITLE_ID_REWARD_COMPENSATION), MAIL_TYPE_PLAYER, reward->items, playerID);
        }
    }
}

void CenterHandlerHelper::RecvSaveLevelActivityAck( uint64 playerID, uint32 sessionID, uint32 msgID, void* data )
{
    SAVE_LEVEL_ACTIVITY_INFO_ACK* ack = (SAVE_LEVEL_ACTIVITY_INFO_ACK*) data;
    if (!ack)
        return;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    // 玩家在线直接给
    if (player && (player->GetNetState() >= Player::kStateInGame))
    {
        player->GetLevelActivityManager()->CBSaveLevelActivity(data);
        return;
    }

    CenterConnecter* connecter = m_CenterHandler->GetConnecterBySessionID(sessionID);
    if(!connecter)
        return;

    // 玩家不在线,发邮件
    const SpecialActivityInfo* info = SpecialActivityConfig::Instance()->GetActivityInfoByIdAndParam2(SPECIAL_ACTIVITY_PLAYER_LEVEL, ack->level);
    if (info)
    {
        const REWARD_TABLE* reward = ActivityRewardConfig::Instance()->GetReward(info->rewardId);
        if (reward)
        {

            MailManager::SendMail(connecter->m_centerID, playerID, "", "", Crown::ToString(MAIL_TITLE_ID_REWARD_COMPENSATION), MAIL_TYPE_PLAYER, reward->items, playerID);
        }
    }
}

void CenterHandlerHelper::RecvPlayerRechargeInfoNtf(uint64 playerID, uint32 msgID, void* data)
{
    PLAYER_RECHARGE_INFO* rechargeInfo = (PLAYER_RECHARGE_INFO*)data;
    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);

    // 玩家在线直接给
    if (player && (player->GetNetState() >= Player::kStateInGame))
    {
        player->LoadRechargeInfo(*rechargeInfo);
        player->SendRechargeEvent();
    }
}

// ==================================================
// 使用激活码应答
// ==================================================
void CenterHandlerHelper::OnActiveCodeUseAck(uint64 playerID, void* data)
{
    std::vector<REWARD_ITEM> list;
    REWARD_ITEM attachment;

    ACTIVECODE_USE_ACK* recvMsg = (ACTIVECODE_USE_ACK*)data;
    Player* player = 0;

    if(!recvMsg) return;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(player)
    {
        //ItemManager::AddRewardTableWithEmail(player->GetCenterSvrID(), player->GetPlayerID(), recvMsg->items);
        ACTIVECODE_USE_ACK ntfMsg;
        ntfMsg.title = recvMsg->title;
        GateHandler::Instance()->SendByGateID(player->GetGateSvrID(), MSG_ACTIVECODE_USE_NTF, &ntfMsg, player->GetPlayerID());
    }
}