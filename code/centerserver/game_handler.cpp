#include "centerserver_pch.h"
#include "game_handler.h"
#include "centerserver.h"
#include "af_transfer_server.h"
#include "gate_handler.h"
#include "player.h"
#include "player_manager.h"
#include "player_net_handler.h"
#include "dungeon_manager.h"
#include "dungeon_score_manager.h"
#include "gate_handler.h"
#include "dbjob.h"
#include "config_loader.h"
#include "team_manager.h"
#include "guild_manager.h"
#include "answer_question_manager.h"
#include "mail_manager.h"
#include "trade_manager.h"
#include "pay_manager.h"
#include "adventure_manager.h"
#include "mall_manager.h"
#include "ranking_list_manager.h"
#include "activecode_manager.h"
#include "kunlun_log.h"
#include "world_boss_manager.h"
#include "special_activity_manager_gc.h"
#include "city_fight_manager_gc.h"
#include "city_survival_manager_gc.h"
#include "void_field_manager.h"
#include "void_field_solo_manager.h"
#include "dongyi_log.h"
#include "festival_job.h"
#include "global_setting_manager.h"
#include "multiserver_battle_manager.h"
#include "field_boss_manager.h"
#include "gmt_executor.h"
#include "global_setting_job.h"
#include "proclamation_manager.h"
#include "global_db_operation.h"
#include "announce_manager.h"
#include "chat_manager.h"
#include "server_monitor.h"
#include "player_save_job.h"
#include "vip_statistics_manager.h"
#include "donate_manager.h"
#include "offline_event_manager.h"
#include "platform_callbacker.h"
#include "namelimit.h"
#include "cross_team_manager.h"
#include "daily_fashion_manager.h"


IMPLEMENT_SINGLETON(GameHandler)

GameHandler::GameHandler()
{
    m_GameServerProxy = NULL;
    m_GamePlayerNumMap.clear();
}

GameHandler::~GameHandler()
{
    if(m_GameServerProxy)
        CnDelete m_GameServerProxy;

    m_GamePlayerNumMap.clear();
}

void GameHandler::RegistRegionID(uint32 centGameID, uint32 sessionID)
{
    CnAssert(m_centGameID2snID.find(centGameID) == m_centGameID2snID.end());

    GameConnecter connecter;
    connecter.regionID = centGameID;
    connecter.sessionID = sessionID;
    m_centGameID2snID[centGameID] = connecter;

    std::map<uint32, int>::iterator PlayerNumIter = m_GamePlayerNumMap.find(centGameID);
    CnAssert(PlayerNumIter == m_GamePlayerNumMap.end());
    m_GamePlayerNumMap[centGameID] = 0;

    uint32 centerID = CalCenterIDByRegionID(centGameID);
    CnInfo("register gameserver cid=%d gid=%d\n", centerID, CalGameIDByRegionID(centGameID));
}

void GameHandler::UnregistRegionID(uint32 sessionID)
{
    for(GameConnecterMap::iterator it = m_centGameID2snID.begin(); it != m_centGameID2snID.end(); ++it)
    {
        GameConnecter& connecter = it->second;
        if(connecter.sessionID != sessionID)
            continue;
        CnInfo("unregister gameserver id=%d\n", it->first);
        uint32 CentGameID = GetRegionIDBySessionID(sessionID);
        uint32 centerID = CalCenterIDByRegionID(CentGameID);
        uint32 serverID = CalGameIDByRegionID(CentGameID);

        CnInfo("unregister gameserver cid=%d gid=%d\n", centerID, serverID);
        CnAssert(centerID);
        CnAssert(serverID);
        CenterServer::Instance()->UnactiveServer(SERVER_TYPE_GAME, serverID, centerID);

        std::map<uint32, int>::iterator PlayerNumIter = m_GamePlayerNumMap.find(CentGameID);
        CnAssert(PlayerNumIter != m_GamePlayerNumMap.end());
        m_GamePlayerNumMap.erase(PlayerNumIter);

        m_centGameID2snID.erase(it);
        break;
    }
}

bool GameHandler::IsConnectGame(uint32 centGameID)
{
    return m_centGameID2snID.find(centGameID) != m_centGameID2snID.end();
}

bool GameHandler::IsSealGame(uint32 centGameID)
{
    GameConnecter* pConnecter = FindConnecterByRegionID(centGameID);
    if(!pConnecter)
    {
        CnAssert(false);
        return true;
    }
    return !pConnecter->is_open;
}

bool GameHandler::IsValidGame(uint32 centGameID)
{
    return IsConnectGame(centGameID) && !IsSealGame(centGameID);
}

bool GameHandler::Init(const char* ip, uint32 port)
{
    m_GameServerProxy = (AFTransferServerProxy*)CreateTransferServerProxy(ip, port, &RecvMsg, 0);
    if(!m_GameServerProxy)
    {
        return false;
    }
    return true;
}

uint32 GameHandler::GetRegionIDBySessionID(uint32 sessionID)
{
    for(GameConnecterMap::iterator it = m_centGameID2snID.begin(); it != m_centGameID2snID.end(); it++)
    {
        GameConnecter& connecter = it->second;
        if(connecter.sessionID == sessionID)
            return it->first;
    }
    return 0;
}

int32 GameHandler::SendByID(uint32 regionID, uint32 msgID, void* data, uint64 playerID)
{
    GameConnecterMap::iterator it = m_centGameID2snID.find(regionID);
    if(it == m_centGameID2snID.end())
    {
        return 0;
    }
    GameConnecter& connecter = it->second;
    int32 ret = TransferSend(connecter.sessionID, msgID, data, playerID);
    return ret;
}

void GameHandler::SendOneServer(uint32 msgID, void* data, uint64 playerID)
{
    if(m_centGameID2snID.size() <= 0) return;

    GameConnecterMap::iterator it = m_centGameID2snID.begin();
    GameConnecter& connecter = it->second;
    if(connecter.sessionID)
    {
        TransferSend(connecter.sessionID, msgID, data, playerID);
    }
}
void GameHandler::SendToAll(uint32 msgID, void* data, uint64 playerID)
{
    for(GameConnecterMap::iterator it = m_centGameID2snID.begin(); it != m_centGameID2snID.end(); ++it)
    {
        GameConnecter& connecter = it->second;
        TransferSend(connecter.sessionID, msgID, data, playerID);
    }
}

void GameHandler::SendToAllBut( uint32 regionID, uint32 msgID, void* data, uint64 playerID /*= 0*/ )
{
    for(GameConnecterMap::iterator it = m_centGameID2snID.begin(); it != m_centGameID2snID.end(); ++it)
    {
        if (it->first == regionID)
            continue;

        GameConnecter& connecter = it->second;
        TransferSend(connecter.sessionID, msgID, data, playerID);
    }
}

GameConnecter* GameHandler::FindConnecterByRegionID(uint32 regionID)
{
    for(GameConnecterMap::iterator it = m_centGameID2snID.begin(); it != m_centGameID2snID.end(); ++it)
    {
        if (it->first == regionID)
        {
            return &it->second;
        }
    }
    return NULL;
}

GameConnecter* GameHandler::FindConnecterBySessionID(uint32 sessionID)
{
    for(GameConnecterMap::iterator it = m_centGameID2snID.begin(); it != m_centGameID2snID.end(); ++it)
    {
        GameConnecter& connecter = it->second;
        if (connecter.sessionID == sessionID)
        {
            return &it->second;
        }
    }
    return NULL;
}

void GameHandler::RecvMsg(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata)
{
    switch(msgID)
    {
    case MSG_NET_ACCEPT:
        GameHandler::Instance()->OnAccept(sessionID, data);
        break;
    case MSG_NET_CONNECT:
        GameHandler::Instance()->OnConnect(sessionID, data);
        break;
    case MSG_NET_CLOSE:
        GameHandler::Instance()->OnClose(sessionID, data);
        break;
    case MSG_NET_ERROR:
        GameHandler::Instance()->OnError(sessionID, data);
        break;
    default:
        GameHandler::Instance()->OnRecv(playerID, sessionID, msgID, data, userdata);
        break;
    }
}

void GameHandler::SendDeliverPlayerInfoReq(uint32 regionID, Player* player)
{
    DELIVER_PLAYER_INFO_REQ sendMsg;

    if(!player) return;

    CnAssert(player->GetState() == Player::kStateDelivingData || player->GetState() == Player::kStateChangeDungeonSyncData);

    sendMsg.Clear();

    // 复制装备信息 复制角色属性信息
    player->FillPropInfo(sendMsg.prop_info);

    // 复制角色状态信息
    player->FillStateInfo(sendMsg.state_info);

    // 复制角色副本信息
    player->FillPlayerDungeonInfo(sendMsg.player_dungeon_info);

    // 复制开启系统信息
    player->FillOpenSystemInfo(sendMsg.open_system_info);

    // 复制公会福利信息
    //player->FillGuildWelfareInfo(sendMsg.prop_info.guild_welfare_info);

    // 复制城市争夺战信息
    //player->FillCityFightInfo(sendMsg.prop_info.city_fight_info);

    // 复制星空图信息
    player->FillHoroscopeInfo(sendMsg.horoscope_info);

    // 复制技能信息
    player->FillSkillInfo(sendMsg.skill_component_info);

    // 复制装备信息
    player->FillEquipInfo(sendMsg.equip_info);

    // 复制药剂信息
    player->FillPotionInfo(sendMsg.potion_info);

    //// 复制时装信息
    //player->FillFashionInfo(sendMsg.fashion_info);

    //复制跟宠宠物信息 
    player->FillPetInfo(sendMsg.pet_info);

    //复制跟宠宠物上阵信息 
    player->FillPetFormation(sendMsg.pet_formation_info);

    //复制骑宠信息
    player->FillRidePetInfo(sendMsg.ride_info);

    // 复制物品信息
    player->FillItemInfo(sendMsg.item_info);

    // 复制物品信息
    player->FillStoreInfo(sendMsg.store_info);
    

    // 复制抽奖信息
    player->FillRandomPackageInfo(sendMsg.random_package_info);

    // 复制好友信息
    player->FillFriendInfo(sendMsg.friend_info);

    // 复制任务信息
    player->FillTaskInfo(sendMsg.task_info);

    // 复制引导信息
    player->FillGuideInfo(sendMsg.guide_info);

    // 复制善恶信息
    player->FillEnemyInfo(sendMsg.enemy_info);

    // 复制怪物之门数据
    player->FillMonsterDoorInfo(sendMsg.monster_door_info);

    // 复制勇者争霸数据
    player->FillPlayerDoorInfo(sendMsg.player_door_info);

    // 复制奖励数据信息
    player->FillRewardInfo(sendMsg.reward_info);

    // 复制通天塔信息
    player->FillBabelInfo(sendMsg.babel_info);

    // 复制充值积分抽奖信息
    player->FillPayPointLotteryInfo(sendMsg.pay_point_lottery_info);

    // 复制VIP独享副本信息
    player->FillVIPOnlyDungeonInfo(sendMsg.vip_only_dungeon_info);

    // 复制邮件系统信息
    player->FillMailSystemInfo(sendMsg.mail_system_info);

    // 复制签到信息
    player->FillSignInfo(sendMsg.sign_info);

    // 复制在线奖励信息
    player->FillOnlineRewardInfo(sendMsg.online_reward_info);

    // 复制赛马信息
    player->FillRacingInfo(sendMsg.racing_info);

    // 复制采矿信息
    player->FillMiningInfo(sendMsg.mining_info);

    // 复制宝库信息
    player->FillTreasuryInfo(sendMsg.treasury_info);

    // 复制金像信息
    player->FillGoldtreeInfo(sendMsg.goldtree_info);

    // 复制大冒险信息
    player->FillAdventureInfo(sendMsg.adventure_info);

    // 复制答题信息
    player->FillAnswerInfo(sendMsg.answer_info);

    // 复制翅膀信息
    player->FillWingInfo(sendMsg.wing_info);

    // 复制修罗场信息
    player->FillAshuraInfo(sendMsg.ashura_info);

    // 复制充值信息
    player->FillRechargeInfo(sendMsg.recharge_info);

    // 复制商城信息
    player->FillMallInfo(sendMsg.restriction_info);

    // 复制商城信息
    player->FillPassiveSkill(sendMsg.passive_skill_info);

    // 复制称号信息
    player->FillTitleInfo(sendMsg.title_info);

    // 填充日常投资信息
    player->FillDayInvestmentInfo(sendMsg.day_investment_info);

    // 填充等级投资信息
    player->FillLevelInvestmentInfo(sendMsg.level_investment_info);

    // 填充等级活动信息
    player->FillLevelActivityInfo(sendMsg.level_activity_info);

    // 填充自动战斗设置信息
    player->FillAutoFightSettingInfo(sendMsg.auto_fight_setting_info);

    // 填充系统设置信息
    player->FillSystemSettingInfo(sendMsg.system_setting_info);

    // 填充玩家设置信息
    player->FillPlayerSettingInfo(sendMsg.player_setting_info);

    // 填充自动购买设置信息
    player->FillAutobuySettingInfo(sendMsg.autobuy_setting_info);

    // 填充虚空幻境挂机设置信息
    player->FillVoidFieldSettingInfo(sendMsg.void_field_setting_info);

    // 填充活跃度信息
    player->FillLivenessInfo(sendMsg.liveness_info);

    // 填充活跃度信息
    player->FillDungeonTimesInfo(sendMsg.dungeon_times_list);

    // 限制购买信息
    player->FillLimitBuyInfo(sendMsg.limit_buy_list);

    // 单人答题信息
    player->FillSingleAnswerInfo(sendMsg.single_answer);

    // 圣剑信息
    player->FillSwordInfo(sendMsg.sword_info);

    // 填充护魂副本扫荡
    //player->FillSoulDungeonInfo(sendMsg.soul_radis);

    // 僵尸信息填充
    player->FillZombieInfo(sendMsg.zombie_info);

    // 复制勇敢的心信息
    player->FillBhInfo(sendMsg.bh_info);

    // 礼品码信息填充
    player->FillGiftCodeInfo(sendMsg.gift_code_info);

    // 公会科技信息
    player->FillGuildScienceInfo(sendMsg.guild_science_info);

    // 复制游戏收藏信息
    player->FillShorcutInfo(sendMsg.shorcut_info);

    // 复制跨服战斗信息
    player->FillMultiserverBattleInfo(sendMsg.multiserver_battle_info);

    // 复制投资计划信息
    player->FillInvestmentPlanInfo(sendMsg.investment_plan_info);

    // 复制捐献信息
    player->FillDonateInfo(sendMsg.donate_info);

    // 复制砸金蛋信息
    player->FillEggInfo(sendMsg.egg_info);

    // 复制越买越便宜信息
    player->FillBuyCheapInfo(sendMsg.buy_cheap);

    // 复制个性图标
    player->FillHeadPortraitInfo(sendMsg.head_portrait);

    // 复制签到
    player->FillSignInfo(sendMsg.sign_info);

    int32 ret = SendByID(regionID, MSG_DELIVER_PLAYER_INFO_REQ, &sendMsg, player->GetPlayerID());
    if(ret != 0)
    {
        // Server内部发送协议错误，强制玩家下线
        CnAssert(false);
        CnError("MSG_DELIVER_PLAYER_INFO_REQ failed! player %s\n", player->GetName().c_str());
        player->SetState(Player::kStateLogoutSyncData);
        CnAssert(player->GetCentGameID());
        player->SetCentGameID(0);
        player->GetNetHandler()->KickOut(ERR_FAILED);
    }
}

void GameHandler::SendEnterDungeonReq(uint32 centGameID, uint64 playerID, const ENTER_DUNGEON_REQ& enterDungeonData)
{
    SendByID(centGameID, MSG_ENTER_DUNGEON_REQ, (void*)&enterDungeonData, playerID);
}

void GameHandler::SendExitDungeonReq(uint32 regionID, uint64 playerID)
{
    NONE sendMsg;
    SendByID(regionID, MSG_EXIT_DUNGEON_REQ, &sendMsg, playerID);
}

void GameHandler::SendChangePlayerNameAck(uint32 regionID, uint64 playerID, uint16 errCode, std::string name)
{
    // 改名后 更新公会信息
    GuildManager::Instance()->SetMemberProp(playerID);

    CHANGE_PLAYER_NAME_ACK sendMsg;
    sendMsg.errcode = errCode;
    sendMsg.name = name;
    SendByID(regionID, MSG_CHANGE_PLAYER_NAME_ACK, &sendMsg, playerID);
}

void GameHandler::OnAccept(uint32 sessionID, void* data)
{
    CnInfo("Accept from gameserver\n");
}

void GameHandler::OnConnect(uint32 sessionID, void* data)
{

}

void GameHandler::OnClose(uint32 sessionID, void* data)
{
    uint32 centGameID = GetRegionIDBySessionID(sessionID);
    uint32 gameSvrID = CalGameIDByRegionID(centGameID);
    if(gameSvrID)
    {
        PlayerManager::Instance()->OnCloseGameServer(gameSvrID);
        DungeonManager::Instance()->OnCloseGameServer(centGameID);
        ServerMonitor::Instance()->OnGameServerClose(gameSvrID);
    }

    UnregistRegionID(sessionID);
}

void GameHandler::OnError(uint32 sessionID, void* data)
{
    uint32 centGameID = GetRegionIDBySessionID(sessionID);
    uint32 gameSvrID = CalGameIDByRegionID(centGameID);
    if(gameSvrID)
    {
        PlayerManager::Instance()->OnCloseGameServer(gameSvrID);
        DungeonManager::Instance()->OnCloseGameServer(centGameID);
        ServerMonitor::Instance()->OnGameServerClose(gameSvrID);
    }

    UnregistRegionID(sessionID);
}

void GameHandler::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata)
{
    switch(msgID)
    {
    case MSG_SERVER_INFO_REQ:
        CenterServer::Instance()->RecvServerInfoReq(sessionID, data);
        break;
    case MSG_SERVER_ACTIVE_REQ:
        CenterServer::Instance()->RecvServerActiveReq(sessionID, data);
        break;
    case MSG_SERVER_DUNGEON_LIST_NTF:
        DungeonManager::Instance()->RecvServerDungeonListNtf(sessionID, data);
        break;
    case MSG_DELIVER_PLAYER_INFO_ACK:
        RecvDeliverPlayerFirstInfoAck(playerID, sessionID, data);
        break;
    case MSG_WRITEBACK_PLAYER_INFO_REQ:
        RecvWriteBackPlayerInfoReq(playerID, sessionID, data);
        break;
    case MSG_AUTO_SAVE_PLAYER_INFO_REQ:
        RecvAutoSavePlayerInfoReq(playerID, sessionID, data);
        break;
    case MSG_CHANGE_DUNGEON_REQ:
        RecvChangeDungeon(playerID, sessionID, data);
        break;
    case MSG_GAMESERVER_SEAL_REQ:
        RecvSealServer(sessionID, data);        //请求封闭GameServer
        break;
    case MSG_SAVE_DUNGEON_SCORE_INFO:
    case MSG_DUNGEON_SCORE_INFO_REQ:
    case MSG_GS_DUNGEON_SCORE_INFO_REQ:
        DungeonScoreManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
        break;
    case MSG_GS_KICKOUT_GAME:
        RecvKickOutPlayer(playerID, sessionID, data);
        break;
    case MSG_CHANGE_PLAYER_NAME_REQ:
        RecvChangePlayerNameReq(playerID, sessionID, data);
        break;
    case MSG_OFFLINE_PLAYER_INFO_REQ:
        RecvRequestOfflinePlayerReq(sessionID, data);
        break;
    case MSG_FRIEND_ADD_REQ:
        RecvFriendAddReq(playerID, sessionID, data);
        break;
    case MSG_FRIEND_ADD_ANS:
        RecvFridentAddAns(playerID, sessionID, data);
        break;
    case MSG_FRIEND_DEL_REQ:
        RecvFriendDelReq(playerID, sessionID, data);
        break;
    case MSG_FRIEND_QUICK_ADD_REQ:
        RecvFriendQuickAddReq(playerID, sessionID, data);
        break;
    case MSG_CHAT_REQ:
        RecvChatReq(playerID, sessionID, data);
        break;
    case MSG_BEHAVIOR_LOG_REQ:
        RecvBehaviorLog(playerID, sessionID, data);
        break;
    case MSG_LOG_REQ:
        RecvLog(playerID , sessionID , data);
        break;
    case MSG_SAVE_DAY_INVESTMENT_INFO_REQ:
        SaveDayInvestmentReq(playerID, sessionID, data);
        break;
    case MSG_SAVE_LEVEL_INVESTMENT_INFO_REQ:
        SaveLevelInvestmentReq(playerID, sessionID, data);
        break;
    case MSG_INVESTMENT_OPERATION_NTF:
        InvestmentOperationNTF(playerID, sessionID, data);
        break;
    case MSG_INVESTMENT_OPERATION_REQ:
        InvestmentOperationReq(playerID, sessionID, data);
        break;
    case MSG_GUILD_CRUSADE_BOSS_RANK_SAVE:
        RecvGuildCrusadeBossRankSave(playerID, msgID, data);
        break;
    case MSG_GUILD_CRUSADE_BOSS_RANK_REQ:
        RecvGuildCrusadeBossRankReq(playerID, msgID, data);
        break;
    case MSG_GUILD_TWILIGHT_WAR_RANK_SAVE:
        RecvGuildTwilightWarRankSave(playerID, msgID, data);
        break;
    case MSG_GUILD_TWILIGHT_WAR_RANK_REQ:
        RecvGuildTwilightwarRankReq(playerID, msgID, data);
        break;
    case MSG_SAVE_LEVEL_ACTIVITY_INFO_REQ:
        SaveLevelActivityReq(playerID, sessionID, data);
        break;
    case MSG_MEMBER_PROP_NTF:
        TeamMemberPropNotify(playerID , data);
        break;
    case MSG_PUSH_360_NTF:
        RecvPush360Ntf(playerID, data);
        break;
    case MSG_GMT_ASK_ACK:
        GmtExecutor::Instance()->OnGmtAskAck(data);
        break;
    case MSG_GMT_SET_ACK:
        GmtExecutor::Instance()->OnGmtSetAck(data);
        break;
    case MSG_CROSS_SERVER_LIST_ACK:
        GmtExecutor::Instance()->OnGmtAskCrossCenterIpListAck(data);
        break;
    case MSG_IS_UPDATE_PLAYER_NAME_REQ:
        RecvIsUpdatePlayerNameReq(playerID, sessionID, data);
        break;
    default:
        break;
    }

    if((MSG_TEAM_BEGIN <= msgID) && (msgID <= MSG_TEAM_END))                                // 2800
    {
        TeamManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_MAIL_BEGIN <= msgID) && (msgID <= MSG_MAIL_END))                           // 2900
    {
        MailManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_ANSWER_QUESTION_BEGIN <= msgID) && (msgID <= MSG_ANSWER_QUESTION_END))     // 3000
    {
        AnswerManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_GUILD_BEGIN <= msgID) && (msgID <= MSG_GUILD_END))                         // 3100
    {
        GuildManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_WORLD_BOSS_BEGIN <= msgID) && (msgID <= MSG_WORLD_BOSS_END))               // 3300
    {
        WorldBossManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_ADVENTURE_BEGIN <= msgID) && (msgID <= MSG_ADVENTURE_END))                 // 3800
    {
        AdventureManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_MALL_BEGIN <= msgID) && (msgID <= MSG_MALL_END))                           // 4200
    {
        MallManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_RANKING_LIST_BEGIN <= msgID) && (msgID <= MSG_RANKING_LIST_END))           // 4300
    {
        RankListManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if ((MSG_ACTIVATION_CODE_BEGIN <= msgID) && (msgID <= MSG_ACTIVATION_CODE_END))    // 4600
    {
        ActiveCodeManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_TRADE_ORDER_BEGIN <= msgID) && (msgID <= MSG_TRADE_ORDER_END))             // 4700
    {
        TradeManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_PAY_ORDER_BEGIN <= msgID) && (msgID <= MSG_PAY_ORDER_END))                 // 4800
    {
        PayManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_SPECIAL_ACTIVITY_MANAGER_BEGIN <= msgID) && (msgID <= MSG_SPECIAL_ACTIVITY_MANAGER_END))       // 5900
    {
        SpecialActivityManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_CITY_FIGHT_BEGIN <= msgID) && (msgID <= MSG_CITY_FIGHT_END))               // 6500
    {
        CityFightManager::Instance()->OnGSRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_CITY_SURVIVAL_BEGIN <= msgID) && (msgID <= MSG_CITY_SURVIVAL_END))         // 6800
    {
        CitySurvivalManager::Instance()->OnGSRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_CROSS_TEAM_BEGIN <= msgID) && (msgID <= MSG_CROSS_TEAM_END))               // 7700
    {
        CrossTeamManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_VOID_FIELD_BEGIN <= msgID) && (msgID <= MSG_VOID_FIELD_END))               // 8200
    {
        VoidFieldManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_VOID_FIELD_SOLO_BEGIN <= msgID) && (msgID <= MSG_VOID_FIELD_SOLO_END))     // 8250
    {
        VoidFieldSoloManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_GLOBAL_SETTING_BEGIN <= msgID) && (msgID <= MSG_GLOBAL_SETTING_END))       // 8800
    {
        switch(msgID)
        {
        case MSG_GLOBAL_SETTING_INFO_FILL:
        case MSG_GLOBAL_SETTING_INFO_LOAD_REQ:
            GlobalSettingManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
            break;
        case MSG_GLOBAL_SETTING_PROCLAMATION_REQ:
            ProclamationManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
            break;
        default:
            CnAssert(false);
            break;
        }
    }
    else if((MSG_MULTISERVER_BATTLE_BEGIN <= msgID) && (msgID <= MSG_MULTISERVER_BATTLE_END))   // 9200
    {
        MultiserverBattleManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if ((MSG_FIELD_BOSS_BEGIN <= msgID)&&(msgID <= MSG_FIELD_BOSS_END))            // 9300
    {
        FieldBossManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_OFFLINE_EVENT_BEGIN <= msgID) && (msgID <= MSG_OFFLINE_EVENT_END))     // 9600
    {
        OfflineEventManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_DONATE_BEGIN <= msgID) && (msgID <= MSG_DONATE_END))                   // 9700
    {
        DonateManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_VIP_STATISTICS_BEGIN <= msgID) && (msgID <= MSG_VIP_STATISTICS_END))   // 9800
    {
        //VipStatisticsManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }
    else if((MSG_DAILY_FASHION_BEGIN <= msgID) && (msgID <= MSG_DAILY_FASHION_END))   // 9800
    {
        DailyFashionManager::Instance()->OnRecv(playerID, sessionID, msgID, data);
    }


}

void GameHandler::RecvDeliverPlayerFirstInfoAck(uint64 playerID, uint32 sessionID, void* data)
{
    PLAYER_BASE_INFO_ACK* recvMsg = (PLAYER_BASE_INFO_ACK*)data;
    if(!recvMsg) return;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return;

    player->GetNetHandler()->RecvDeliverPlayerInfoAck(recvMsg);
}

void GameHandler::RecvWriteBackPlayerInfoReq(uint64 playerID, uint32 sessionID, void* data)
{
    WRITEBACK_PLAYER_INFO_REQ* recvMsg = (WRITEBACK_PLAYER_INFO_REQ*)data;
    if(!recvMsg) return;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return;

    player->GetNetHandler()->RecvWriteBackPlayerInfoReq(recvMsg);
}
// ==================================================
// 收到GameServer请求自动回存玩家信息
// ==================================================
void GameHandler::RecvAutoSavePlayerInfoReq(uint64 playerID, uint32 sessionID, void* data)
{
    WRITEBACK_PLAYER_INFO_REQ* recvMsg = (WRITEBACK_PLAYER_INFO_REQ*)data;
    if(!recvMsg) return;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return;

    if(player->GetState() != Player::kStateInGame)
        return;

    player->GetNetHandler()->RecvAutoSavePlayerInfoReq(recvMsg);
}

void GameHandler::RecvSealServer(uint32 sessionID, void* data)
{
    GameConnecter* pConnecter = FindConnecterBySessionID(sessionID);
    if(!pConnecter)
    {
        CnDbg("session %d recv seal but no register\n", sessionID);
        return;
    }

    pConnecter->is_open = false;

    CnInfo("seal gameserver id=%d center id=%d\n", CalGameIDByRegionID(pConnecter->regionID),CalCenterIDByRegionID(pConnecter->regionID));

    NONE none;
    SendByID(pConnecter->regionID, MSG_GAMESERVER_SEAL_ACK, &none);
}
// ==================================================
// 收到角色切换副本的请求
// ==================================================
void GameHandler::RecvChangeDungeon(uint64 playerID, uint32 sessionID, void* data)
{
    CHANGE_DUNGEON_REQ* recvMsg = (CHANGE_DUNGEON_REQ*)data;
    if(!recvMsg) return;

    // 获得角色
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return;

    player->GetNetHandler()->RecvChangeDungeon(recvMsg);
}
// ==================================================
// 收到踢出玩家的请求
// ==================================================
void GameHandler::RecvKickOutPlayer(uint64 playerID, uint32 sessionID, void* data)
{
    KICKOUT_GAME* recvMsg = (KICKOUT_GAME*)data;
    if(!recvMsg) return;

    // 获得角色
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return;

    // Game 上没有玩家的角色数据
    // 则直接将角色状态由 kStateInGame 改为 kStateDelivingData
    // 这样就不会走 Game 的下线流程了，玩家数据直接在 center 上被删掉
    if(recvMsg->errcode == ERR_PLAYER_NOT_EXIT)
    {
        // !!! TODO 这里的处理是有问题的，如果角色在跳转地图时在 GS 上没有找到这个角色
        // 此时玩家状态为kInGame，这里临时强制设为 kStateLogoutSyncData
        // 这种情况在 GS 上有逻辑 bug 时会出现
        player->SetState(Player::kStateLogoutSyncData);
    }

    player->GetNetHandler()->KickOut(recvMsg->errcode);
}
// ==================================================
// 收到请求修改角色名称
// ==================================================
void GameHandler::RecvChangePlayerNameReq(uint64 playerID, uint32 sessionID, void* data)
{
    CHANGE_PLAYER_NAME_REQ* recvMsg = (CHANGE_PLAYER_NAME_REQ*) data;

    Player* player = 0;

    if(!recvMsg) return;

    // 获得角色
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return;

    if(player->GetState() != Player::kStateInGame)
    {
        GameHandler::Instance()->SendChangePlayerNameAck(player->GetCentGameID(), playerID, ERR_FAILED, recvMsg->name);
        return;
    }

    PlayerJob::Instance()->ChangePlayerName(playerID, recvMsg->name);
}

// ==================================================
// 请求离线战斗的角色信息
// ==================================================
void GameHandler::RecvRequestOfflinePlayerReq(uint32 sessionID, void* data)
{
    OFFLINE_PLAYER_INFO_REQ* recvMsg = (OFFLINE_PLAYER_INFO_REQ*)data;

    uint64 playerID = U64ID(recvMsg->player_id_h, recvMsg->player_id_l);
    std::string playerName = recvMsg->player_name;

    OFFLINE_PLAYER_ACK offline_player_ack;
    offline_player_ack.reason = recvMsg->reason;

    uint32 RegionID = GetRegionIDBySessionID(sessionID);

    bool isOnline = false;
    // 有ID就用ID查，无ID用名字查
    if(playerID)
    {
        Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
        if(player)
        {   // 如果玩家在线，则直接返回数据
            OFFLINE_PLAYER_INFO& player_info = player->GetOfflinePlayerInfo();
            offline_player_ack.offline_player_info = player_info;
            if(player_info.level > 0)       // 有可能玩家数据正在加载中，此时level==0
            {
                isOnline = true;
                GameHandler::Instance()->SendByID(RegionID, MSG_OFFLINE_PLAYER_INFO_ACK, &offline_player_ack);
            }
            else
                isOnline = false;
        }

        if(!isOnline)
        {   // 如果玩家不在线或离线数据不可用，则去数据库查询
            PlayerJob::Instance()->RequestOfflinePlayerRecord(playerID, RegionID, recvMsg->reason);
        }
    }
    else if(playerName.length() > 0)
    {
        Player* player = PlayerManager::Instance()->GetPlayerByName(playerName);
        if(player)
        {   // 如果玩家在线，则直接返回数据
            OFFLINE_PLAYER_INFO& player_info = player->GetOfflinePlayerInfo();
            offline_player_ack.offline_player_info = player_info;
            if(player_info.level > 0)
            {
                isOnline = true;
                GameHandler::Instance()->SendByID(RegionID, MSG_OFFLINE_PLAYER_INFO_ACK, &offline_player_ack);
            }
            else
                isOnline = false;
        }

        if(!isOnline)
        {   // 如果玩家不在线，则去数据库查询
            PlayerJob::Instance()->RequestOfflinePlayerRecordByName(playerName, RegionID, recvMsg->reason);
        }
    }
}
// ==================================================
// 请求添加好友
// ==================================================
void GameHandler::RecvFriendAddReq(uint64 playerID, uint32 sessionID, void* data)
{
    FRIEND_ADD_REQ* recvMsg = (FRIEND_ADD_REQ*) data;
    Player* src_player = NULL;
    Player* dst_player = NULL;
    uint32 errorCode = ERR_SUCCEED;
    bool ret = false;

    if(!recvMsg) return;

    src_player = PlayerManager::Instance()->GetPlayerByName(recvMsg->src_name);
    // 玩家不存在或者玩家不在线，就不能去执行添加好友的动作
    if (!src_player|| (src_player->GetState() != Player::kStateInGame))
    {
        return;
    }

    dst_player = PlayerManager::Instance()->GetPlayerByName(recvMsg->dst_name);

    if(!dst_player)
    {
        errorCode = ERR_PLAYER_NOTONLINE;
        goto _ERROR;
    }

    // 是否已经有这个好友了
    if(src_player->HasFriend(recvMsg->dst_name))
    {
        errorCode = ERR_FRIEND_EXIST;
        goto _ERROR;
    }

    // 添加好友
    ret = src_player->AddFriend(dst_player);

    if(ret)
    {   // 添加好友
        PlayerJob::Instance()->AddFriend(src_player->GetPlayerID(), dst_player->GetPlayerID());
    }
    else
    {
        errorCode = ERR_FRIEND_EXIST;
        // 重复添加好友了
        CnWarn("add friend %s %s error\n", src_player->GetName().c_str(), dst_player->GetName().c_str());
        goto _ERROR;
    }

    src_player->SendFriendAddAck(errorCode, recvMsg->src_name, recvMsg->dst_name);
    return;
   
_ERROR:
    src_player->SendFriendAddAck(errorCode, recvMsg->src_name, recvMsg->dst_name);

    //src_player->SendFriendAddAck(recvMsg->errcode, recvMsg->src_name, recvMsg->dst_name);


    //// 检查是否为机器人
    //if(ConfigLoader::Instance()->GetFriendRobotName() == recvMsg->dst_name)
    //{
    //    src_player->AddRobotFriend();
    //    // 添加好友
    //    PlayerJob::Instance()->AddFriend(src_player->GetPlayerID(), 1);
    //   
    //    return;
    //}

    //dst_player = PlayerManager::Instance()->GetPlayerByName(recvMsg->dst_name);
    //if(dst_player)
    //{
    //    if(dst_player->GetState() == Player::kStateInGame)
    //    {
    //        dst_player->SendToGate(MSG_FRIEND_ADD_CFM, recvMsg);
    //    }
    //}
    //else
    //{
    //    src_player = PlayerManager::Instance()->GetPlayerByName(recvMsg->src_name);
    //    if(src_player && (src_player->GetState() == Player::kStateInGame))  // 角色的非游戏状态将收不到消息
    //    {
    //        src_player->SendFriendAddAck(ERR_PLAYER_NOTONLINE, recvMsg->src_name, recvMsg->dst_name);
    //    }
    //}

}

void GameHandler::RecvFridentAddAns(uint64 playerID, uint32 sessionID, void* data)
{
    FRIEND_ADD_ACK* recvMsg = (FRIEND_ADD_ACK*) data;
    Player* src_player = 0;
    Player* dst_player = 0;

    src_player = PlayerManager::Instance()->GetPlayerByName(recvMsg->src_name);
    if(!src_player || (src_player->GetState() != Player::kStateInGame))
    {
        return;
    }

    if(recvMsg->errcode == ERR_SUCCEED)
    {
        // 通过发起者的PlayerID 保证这条消息不是作弊的
        uint64 playerID = U64ID(recvMsg->player_id_h, recvMsg->player_id_l);
        if(playerID != src_player->GetPlayerID())
        {
            return;
        }

        // 互添好友
        dst_player = PlayerManager::Instance()->GetPlayerByName(recvMsg->dst_name);
        if(dst_player && (src_player->GetState() == Player::kStateInGame))
        {
            // 添加好友
            bool ret1 = src_player->AddFriend(dst_player);

            // 添加好友
            bool ret2 = dst_player->AddFriend(src_player);

            if(ret1 != ret2)
            {
                CnWarn("add friend %s %s no both\n", src_player->GetName().c_str(), dst_player->GetName().c_str());
            }

            if(ret1 && ret2)
            {   // 添加好友
                PlayerJob::Instance()->AddFriend(src_player->GetPlayerID(), dst_player->GetPlayerID());
            }
            else
            {
                // 重复添加好友了
                CnWarn("add friend %s %s no both\n", src_player->GetName().c_str(), dst_player->GetName().c_str());
            }
        }
    }
    else
    {
        src_player->SendFriendAddAck(recvMsg->errcode, recvMsg->src_name, recvMsg->dst_name);
    }
}

// 请求删除好友
void GameHandler::RecvFriendDelReq(uint64 playerID, uint32 sessionID, void* data)
{
    FRIEND_DEL_REQ* recvMsg = (FRIEND_DEL_REQ*) data;
    Player* srcPlayer = NULL;
    uint64 del_playerID = 0;

    if(!recvMsg) return;

    // 获得角色
    srcPlayer = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(srcPlayer && (srcPlayer->GetState() == Player::kStateInGame))
    {
        srcPlayer->DelFriend(recvMsg->player_id_l, recvMsg->player_id_h);
    }

    del_playerID = U64ID(recvMsg->player_id_h, recvMsg->player_id_l);

    // 删除好友
    PlayerJob::Instance()->DelFriend(playerID, del_playerID);

    // 由于是单向加好与，所以这里，一方删除，另外一方不会也删除对方
    /*Player* desPlayer = PlayerManager::Instance()->GetPlayerByPlayerID(del_playerID);
    if(desPlayer && (desPlayer->GetState() == Player::kStateInGame))
    {
        desPlayer->DelFriend((uint32)(U64ID_LOW(playerID)), (uint32)(U64ID_HIGH(playerID)));
    }*/
}

// 请求快速添加好友
void GameHandler::RecvFriendQuickAddReq(uint64 playerID, uint32 sessionID, void* data)
{
    FRIEND_QUICK_ADD_REQ* recvMsg = (FRIEND_QUICK_ADD_REQ*)data;
    Player* player = 0;
    bool add_robot = false;

    if(!recvMsg) return;

    // 获得角色
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player || (player->GetState() != Player::kStateInGame))
    {
        return;
    }

    add_robot = recvMsg->add_robot? true: false;

    player->QuickFriendList(add_robot);
}

//修改禁言标识
void GameHandler::RecvModifyRightFlag(uint64 playerID, uint32 sessionID, void* data)
{
    CHAT_MSG* recvMsg = (CHAT_MSG*)data;

    //Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);

    // if(!recvMsg) return;
    std::string name = recvMsg->src_name;
    Player* player = PlayerManager::Instance()->GetPlayerByName(recvMsg->src_name);

    if(player)
    {
        player->SendToGame(MSG_MODIFY_CHAT, data);
    }

}

void GameHandler::RecvChatReq(uint64 playerID, uint32 sessionID, void* data)
{
    CHAT_MSG* recvMsg = (CHAT_MSG*)data;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    Player* sender = NULL;

    if(!recvMsg)
        return;

    // ip禁言判断
    if(player)
    {
        uint32 playerIP = player->GetLoginIP();
        std::string ip_str = Crown::SDInetNtoa(playerIP);
        if(ChatManager::Instance()->IsChatBlockIP(playerIP))
            return;
    }

    if(player)
    {
        int32 vip_level = 0;
        vip_level = player->GetVipLevelByGrowthPoint();
        recvMsg->src_vip = vip_level;
    }

    bool b_send = false;

    switch(recvMsg->channel)
    {
    case CHAT_CHANNEL_SYSTEM:   // 系统聊天
        {
            if(player && !player->IsGM())
                return;
            GateHandler::Instance()->SendToAll(MSG_CHAT_ACK, data, BROADCAST_PLAYER_ID);
            b_send = true;
        }
        break;
    case CHAT_CHANNEL_WORLD:    // 世界频道
    case CHAT_CHANNEL_HORN:     // 喇叭聊天
        {
            GateHandler::Instance()->SendToAll(MSG_CHAT_ACK, data, BROADCAST_PLAYER_ID);
            b_send = true;
        }
        break;
    case CHAT_CHANNEL_TEAM:     // 队伍聊天
        {
            if(player)
            {
                TeamManager::Instance()->SendToAllTeamMember(player->GetTeamID(), MSG_CHAT_ACK, data);
                b_send = true;
            }
        }
        break;
    case CHAT_CHANNEL_PRIVATE:  // 私聊
        {
            recvMsg->src_name = player->GetName();

            if(recvMsg->dst_name == ConfigLoader::Instance()->GetAnchorName())
            {
                // 给自己发一次
                GateHandler::Instance()->SendByID(player->GetCentGateID(), MSG_CHAT_ACK, data, player->GetPlayerID());
                b_send = true;
                break;
            }

            sender = PlayerManager::Instance()->GetPlayerByName(recvMsg->dst_name);
            if(sender && (sender->GetState() == Player::kStateInGame))
            {
                // 给自己发一次，给对方发一次
                GateHandler::Instance()->SendByID(player->GetCentGateID(), MSG_CHAT_ACK, data, player->GetPlayerID());
                GateHandler::Instance()->SendByID(sender->GetCentGateID(), MSG_CHAT_ACK, data, sender->GetPlayerID());
                b_send = true;
            }
            else
            {
                recvMsg->errcode = ERR_PLAYER_NOT_EXIT;
                GateHandler::Instance()->SendByID(player->GetCentGateID(), MSG_CHAT_ACK, data, player->GetPlayerID());
            }
        }
        break;
    case CHAT_CHANNEL_GUILD:    // 公会
        {
            recvMsg->dst_name = "";
            GUILD_INFO* guildInfo = GuildManager::Instance()->GetGuildInfo(player->GetGuildID());
            if(guildInfo)
            {
                GuildManager::Instance()->SendToAllGuildMember(guildInfo, MSG_CHAT_ACK, recvMsg);
                b_send = true;
            }
        }
        break;
    case CHAT_CHANNEL_MARQUEE:      // 跑马灯
        {
            if(recvMsg->dst_name != "")
            {
                player = PlayerManager::Instance()->GetPlayerByName(recvMsg->dst_name);
                if(player  && (player->GetState() == Player::kStateInGame))
                {
                    player->SendToGate(MSG_CHAT_ACK, data);
                    b_send = true;
                }
            }
            else
            {
                AnnouceManager::Instance()->OnInsert(recvMsg);

                if (recvMsg->isloop == 0)
                    GateHandler::Instance()->SendToAll(MSG_CHAT_ACK, data, BROADCAST_PLAYER_ID);
                b_send = true;
            }
        }
        break;
    case CHAT_CHANNEL_EMERGENCY:    // 应急频道
        {
            if(player && !player->IsGM()) return;

            if(recvMsg->dst_name != "")
            {
                player = PlayerManager::Instance()->GetPlayerByName(recvMsg->dst_name);
                if(player  && (player->GetState() == Player::kStateInGame))
                {
                    player->SendToGate(MSG_CHAT_ACK, data);
                    b_send = true;
                }
            }
            else
            {
                GateHandler::Instance()->SendToAll(MSG_CHAT_ACK, data, BROADCAST_PLAYER_ID);
                b_send = true;
            }
        }
        break;
    default:
        break;
    }

    //插入聊天信息
    if(b_send)
    {
        if(player)
        {
            SaveChatToDB(player, recvMsg->channel, recvMsg->chat);      // 保持聊天信息到跨服DB

            GetPlatformCallbacker()->OnChat(player, sender, recvMsg->channel, recvMsg->chat);
        }
    }
}

void GameHandler::SaveChatToDB(Player* player, int channel, const std::string& chat)
{
    if(chat.length() == 0)
        return;

    Chat_Info chatinfo;
    chatinfo.Clear();
    chatinfo.server_id = ConfigLoader::Instance()->GetRegionID();
    chatinfo.plaform_name = ConfigLoader::Instance()->GetPlatformName();

    if(player)
    {
        chatinfo.player_id = player->GetPlayerID();
        chatinfo.player_name = player->GetName();
    }
    else
    {
        chatinfo.player_id = 0;
        chatinfo.player_name = "";
    }

    chatinfo.context = Crown::SDStrTrim(chat, "\r\t\n");      // 去掉聊天中的回车符
    chatinfo.chat_type = channel;

    GlobalDbOperation::Instance()->SaveChatInfo(chatinfo);
}

void GameHandler::RecvProclamationReq(uint64 playerID, uint32 sessionID, void* data)
{
    GLOBAL_PROCLAMATION_BAG* recvMsg = (GLOBAL_PROCLAMATION_BAG*)data;

    if(!recvMsg) return;
    //GlobalSettingManager::Instance()->ReloadProclamationList();
    ProclamationManager::Instance()->ReloadProclamationList();
    GateHandler::Instance()->SendToAll(MSG_GLOBAL_SETTING_PROCLAMATION_NTF, data, BROADCAST_PLAYER_ID);
}

void GameHandler::SendWriteBackPlayerInfoAck(uint32 gameSvrID, uint64 playerID)
{
    PLAYER_BASE_INFO_ACK sendMsg;
    sendMsg.errcode = ERR_SUCCEED;
    SendByID(gameSvrID, MSG_WRITEBACK_PLAYER_INFO_ACK, &sendMsg, playerID);
}

void GameHandler::RecvBehaviorLog(uint64 playerID, uint32 sessionID, void* data)
{
    BEHAVIOR_LOG_REQ* msg = (BEHAVIOR_LOG_REQ*)data;
    KunlunLog::Instance()->LogBehavior(playerID, msg->log);
}

void GameHandler::RecvLog(uint64 playerID, uint32 sessionID, void* data)
{
    LOG_REQ* msg = (LOG_REQ*)data;
    if(msg)
    {
        if(msg->idflag == LOG_LOGIN)
        {
            Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
            if (!player)
                return;
            std::string str = WriterToLogByLogin(player->GetLoginTime(), player->GetPlatformUName().c_str(), player->GetName().c_str(), player->GetLoginIP());
            DgLog(msg->idflag, playerID, "%s", str.c_str());
        }
        else
            DgLog(msg->idflag, playerID, "%s", msg->log.c_str());
    }
}

void GameHandler::SaveDayInvestmentReq( uint64 playerID, uint32 sessionID, void* data )
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if (!player)
        return;

    SAVE_DAY_INVESTMENT_INFO_REQ* req = (SAVE_DAY_INVESTMENT_INFO_REQ*) data;
    player->SetDayInvestmentInfo(req->investment_info);

    if(PlayerSaveJob::Instance()->SaveDayInvestmentInfo(player))
    {
        // 存档成功,发应答回gs
        SAVE_DAY_INVESTMENT_INFO_ACK ack;
        ack.Clear();
        ack.day = req->day;

        player->SendToGame(MSG_SAVE_DAY_INVESTMENT_INFO_ACK, &ack);
    }
}

void GameHandler::SaveLevelInvestmentReq( uint64 playerID, uint32 sessionID, void* data )
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if (!player)
        return;

    SAVE_LEVEL_INVESTMENT_INFO_REQ* req = (SAVE_LEVEL_INVESTMENT_INFO_REQ*) data;
    player->SetLevelInvestmentInfo(req->investment_info);

    if(PlayerSaveJob::Instance()->SaveLevelInvestmentInfo(player))
    {
        // 存档成功,发应答回gs
        SAVE_LEVEL_INVESTMENT_INFO_ACK ack;
        ack.Clear();
        ack.level = req->level;
        ack.reward_money = req->reward_money;

        player->SendToGame(MSG_SAVE_LEVEL_INVESTMENT_INFO_ACK, &ack);
    }
}
// 队伍成员信息通知
void GameHandler::TeamMemberPropNotify(uint64 playerID, void* data)
{
    MEMBER_PROP_NTF* recmsg =  (MEMBER_PROP_NTF*) data;
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);

    if (!player)
        return;

    player->SetLevel(recmsg->level);
    //player->SetBattlePower(recmsg->battle_power);

    GuildManager::Instance()->SetMemberProp(playerID);
    TeamManager::Instance()->SetMemberProp(playerID, recmsg->dungeon_id);
}

/// 360推送信息通知
void GameHandler::RecvPush360Ntf(uint64 playerID, void* data)
{
    PUSH_360_NTF* recvMsg = (PUSH_360_NTF*) data;

    if(!recvMsg)
    {
        return;
    }

    GetPlatformCallbacker()->OnGameServerPushNtf(recvMsg);
}

void GameHandler::SaveLevelActivityReq( uint64 playerID, uint32 sessionID, void* data )
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if (!player)
        return;

    SAVE_LEVEL_ACTIVITY_INFO_REQ* req = (SAVE_LEVEL_ACTIVITY_INFO_REQ*) data;
    player->SetLevelActivityInfo(req->level_activity_info);

    if (PlayerSaveJob::Instance()->SaveLevelActivityInfo(player))
    {
        // 存档成功,发应答回gs
        SAVE_LEVEL_ACTIVITY_INFO_ACK ack;
        ack.Clear();
        ack.level = req->level;

        player->SendToGame(MSG_SAVE_LEVEL_ACTIVITY_INFO_ACK, &ack);
    }
}

void GameHandler::InvestmentOperationReq( uint64 playerID, uint32 sessionID, void* data )
{
    INVESTMENT_OPERATION_REQ* req = (INVESTMENT_OPERATION_REQ*) data;
    if (!req)
        return;

    TradeJob::Instance()->LoadInvestmentOperation(req->count, playerID);
}

void GameHandler::InvestmentOperationNTF( uint64 playerID, uint32 sessionID, void* data )
{
    TradeJob::Instance()->SaveInvestmentOperation(data);
}

// ==================================================
// 接收公会BOSS排名信息
// ==================================================
void GameHandler::RecvGuildCrusadeBossRankSave(uint64 playerID, uint32 msgID, void* data)
{
    GUILD_DAMAGE_RANK_LIST* recvMsg = (GUILD_DAMAGE_RANK_LIST*)data;

    if(!recvMsg) return;

    RankingLoaderJob::Instance()->DelGuildCrusadeBossRank(*recvMsg);
}

// ==================================================
// 请求公会BOSS排名信息
// ==================================================
void GameHandler::RecvGuildCrusadeBossRankReq(uint64 playerID, uint32 msgID, void* data)
{
    RankingLoaderJob::Instance()->GetGuildCrusadeBossRank(playerID);
}

// ==================================================
// 接收暮光之战排名信息
// ==================================================
void GameHandler::RecvGuildTwilightWarRankSave(uint64 playerID, uint32 msgID, void* data)
{
    GUILD_DAMAGE_RANK_LIST* recvMsg = (GUILD_DAMAGE_RANK_LIST*)data;

    if(!recvMsg) return;

    RankingLoaderJob::Instance()->DelGuildTwilightWarRank(*recvMsg);
}

// ==================================================
// 请求暮光之战排名信息
// ==================================================
void GameHandler::RecvGuildTwilightwarRankReq(uint64 playerID, uint32 msgID, void* data)
{
    RankingLoaderJob::Instance()->GetGuildTwilightwarRank(playerID);
}

void GameHandler::RecvIsUpdatePlayerNameReq( uint64 playerID, uint32 sessionID, void* data )
{
    Player* player = 0;
    ERROR_CODE sendMsg;

    IS_CREATE_PLAYER_NAME_REQ* recvMsg = (IS_CREATE_PLAYER_NAME_REQ*)data;
    if(!recvMsg) return;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);

    sendMsg.errcode = NameLimit::Instance()->IsValidNameStr(recvMsg->player_name);      // 名字合法性检查
    if(sendMsg.errcode != ERR_SUCCEED)
    {
        SendByID(player->GetCentGameID(), MSG_IS_UPDATE_PLAYER_NAME_ACK, &sendMsg, playerID);
    }
    else
    {
        std::string curname = player->GetPlatformServerName() + "." + recvMsg->player_name;
        if(!PlayerJob::Instance()->FindPlayerNameIsExit(curname))           // 名字重复性检查
            sendMsg.errcode = ERR_SUCCEED;
        else
            sendMsg.errcode = ERR_FAILED;

        SendByID(player->GetCentGameID(), MSG_IS_UPDATE_PLAYER_NAME_ACK, &sendMsg, playerID);
    }
}

void GameHandler::AddPlayerToRegion(Player* player, uint32 regionID)
{
    std::map<uint32, int32>::iterator PlayerNumIter = m_GamePlayerNumMap.find(regionID);
    CnAssert(PlayerNumIter != m_GamePlayerNumMap.end());
    PlayerNumIter->second++;
    CnAssert(PlayerNumIter->second >= 0);
}

void GameHandler::RemovePlayerFromRegion(Player* player, uint32 regionID)
{
    std::map<uint32, int32>::iterator PlayerNumIter = m_GamePlayerNumMap.find(regionID);
    CnAssert(PlayerNumIter != m_GamePlayerNumMap.end());
    PlayerNumIter->second--;
    CnAssert(PlayerNumIter->second >= 0);
}

uint32 GameHandler::AskJumpCentGameServerID(Player* player, uint32 dungeonID)
{
    CnAssert(player);

    uint32 lastCentGameID = player->GetCentGameID();
    if(lastCentGameID == 0)
    {
        CnAssert(player->GetState() == Player::kStateLoadingData);      // 只有第一次登陆时
    }
    

    // 竞技场地图,进入全跨服GameServer
    if(dungeonID == DUNGEON_PVP_MAP)
    {
        const std::vector<uint32>* crossCentGameList = DungeonManager::Instance()->GetGameListByDungeon(dungeonID, true);
        if(!crossCentGameList || crossCentGameList->size() == 0)
            return 0;           // 没有该副本对应的Game

        uint32 crossCentGameID = 0;
        // 查找跨服Game
        for(std::vector<uint32>::const_iterator iter = crossCentGameList->begin(); iter != crossCentGameList->end(); iter++)
        {
            uint32 centGameID = *iter;
            if(DungeonManager::Instance()->HasDungeonIdOnGame(dungeonID, centGameID, true))
            {
                crossCentGameID = centGameID;
                CnAssert(CalCenterIDByRegionID(crossCentGameID));
                CnAssert(CalGameIDByRegionID(crossCentGameID));
                break;
            }
        }

        return crossCentGameID;
    }

    const std::vector<uint32>* centGameList = DungeonManager::Instance()->GetGameListByDungeon(dungeonID, false);
    if(!centGameList || centGameList->size() == 0)
        return 0;           // 没有该副本对应的Game

    uint32 wantToGameServerID = 0;

    // 1.部分地图(野外),统一去同一个GameServer(小跨服)
    uint32 dungeonType = DungeonManager::Instance()->GetDungeonType(dungeonID);
    if(dungeonType == DUNGEON_TYPE_WORLD)
    {
        uint32 smallGameID = 99999;
        // 查找ID数最小的GameServer
        for(std::vector<uint32>::const_iterator iter = centGameList->begin(); iter != centGameList->end(); iter++)
        {
            uint32 gameID = CalGameIDByRegionID(*iter);
            if(smallGameID > gameID)
            {
                smallGameID = gameID;
                wantToGameServerID = smallGameID;
            }
        }
    }

    if(wantToGameServerID == 0)
    {
        // 2.再根据玩家的的队伍GameServer判断
        uint64 teamID = player->GetTeamID();
        if(teamID)
        {
            const TEAM_INFO* pTeamInfo = TeamManager::Instance()->GetTeamInfo(teamID);
            if(pTeamInfo)
            {
                wantToGameServerID = pTeamInfo->game_server_id;
                CnAssert(wantToGameServerID);
            }
            else
                CnAssert(false);
        }

        if(!GameHandler::Instance()->IsValidGame(CalRegionID(ConfigLoader::Instance()->GetCenterID(), wantToGameServerID)))
        {
            wantToGameServerID = 0;
        }
    }

    if(wantToGameServerID == 0)
    {
        // 3.派往玩家所在区的GameServer中人数最少的那个
        const std::vector<uint32>* RegionGameServerIDs = ConfigLoader::Instance()->GetRegionGameServerIDs(player->GetRegion());
        if(RegionGameServerIDs)
        {
            int32 playerNum = 999999;
            for(uint32 gsI=0; gsI<RegionGameServerIDs->size(); gsI++)
            {
                uint32 GameServerID = (*RegionGameServerIDs)[gsI];
                uint32 CenterServerID = CalCenterIDByRegionID(player->GetCentGateID());
                CnAssert(GameServerID);
                CnAssert(CenterServerID);
                uint32 centGameID = CalRegionID(CenterServerID, GameServerID);

                if(!DungeonManager::Instance()->HasDungeonIdOnGame(dungeonID, centGameID, false))
                    continue;

                std::map<uint32, int32>::iterator it = m_GamePlayerNumMap.find(centGameID);
                if(it != m_GamePlayerNumMap.end())
                {
                    uint32 centGameID = it->first;
                    int32 num = it->second;
                    if(num < playerNum)
                    {
                        playerNum = num;
                        wantToGameServerID = GameServerID;
                    }
                }
                else
                    CnAssert(false);
            }
        }
        else
        {
            CnWarn("player %s region %d no gameserver\n", player->GetName().c_str(), player->GetRegion());
        }
    }

    // 派往人数最少的GameServer
    if(wantToGameServerID == 0)
    {
        int32 playerNum = 999999;

        for(std::vector<uint32>::const_iterator gIter = centGameList->begin(); gIter != centGameList->end(); gIter++)
        {
            uint32 centGameID = *gIter;

            std::map<uint32, int32>::iterator it = m_GamePlayerNumMap.find(centGameID);
            if(it == m_GamePlayerNumMap.end())
            {
                continue;
            }

            int32 num = it->second;

            if (num < playerNum)
            {
                playerNum = num;
                wantToGameServerID = centGameID;
            }
        }
    }

    CnAssert(wantToGameServerID);

    // 如果原服务器断开,纠正队伍服务器ID
    uint64 teamID = player->GetTeamID();
    if(teamID)
    {
        TeamManager::Instance()->SetTeamGameServerID(teamID, wantToGameServerID);
    }

    return CalRegionID(ConfigLoader::Instance()->GetCenterID(), wantToGameServerID);       // 必为本服地图
}
