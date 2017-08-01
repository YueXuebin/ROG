#include "centerserver_pch.h"
#include "player_net_handler.h"
#include "player.h"
#include "player_job.h"
#include "player_manager.h"
#include "game_handler.h"
#include "gate_handler.h"
#include "dungeon_manager.h"
#include "team_manager.h"
#include "cross_team_manager.h"
#include "kunlun_log.h"
#include "dongyi_log.h"
#include "offline_event_manager.h"
#include "config_loader.h"


PlayerNetHandler::PlayerNetHandler(Player* player) :
m_player(player)
{
    m_pendingLogout = false;
    m_isEnterDungeon = false;
}

PlayerNetHandler::~PlayerNetHandler()
{

}

void PlayerNetHandler::Update()
{
    if(GameHandler::Instance())
    {
        if(m_player->GetCentGameID())
        {
            //CnAssert(GameHandler::Instance()->IsConnectGame(m_player->GetGameSvrID()));     // 存在的玩家，对应的GS一定存在
        }
        else
        {
            // 该player所在的GS挂了
        }
    }

    if(m_pendingLogout)
    {
        if(m_player->GetState() == Player::kStateInGame)
        {
            m_player->SetState(Player::kStateLogoutSyncData);
            ExitDungeonReq();
        }
        else if(m_player->GetState() == Player::kStateDelivingData)
        {
            if(!GateHandler::Instance()->IsConnectGate(m_player->GetCentGateID()))
            {
                CnAssert(!m_player->m_InDungeonCheck);
                m_player->SetState(Player::kStateLogoutSyncData);
                ExitDungeonReq();
            }
        }
        else if(m_player->GetState() == Player::kStateChangeDungeonSyncData)
        {
            if(!GateHandler::Instance()->IsConnectGate(m_player->GetCentGateID()))
            {
                CnAssert(!m_player->m_InDungeonCheck);
                m_player->SetState(Player::kStateLogoutSyncData);
                ExitDungeonReq();
            }
        }
    }
}

void PlayerNetHandler::OnLoginRepeat()
{
    // 踢出玩家的流程
    char idstr[30];
    Crown::SDUtoa64<30>(idstr, m_player->GetLoginID());
    CnDbg("Kickout account: loginID:%s, account:%s\n", idstr, m_player->GetAccount().c_str());
    KickOut(ERR_OTHER_LOGIN);
}

void PlayerNetHandler::KickOut(uint32 error)
{
    GateHandler::Instance()->SendKickOutGame(m_player->GetCentGateID(), m_player->GetLoginID(), error);
}

void PlayerNetHandler::OnVerify()
{
    m_player->SetState(Player::kStateVerifyOk);
}

void PlayerNetHandler::OnLoadStart()
{
    CnAssert(m_player->GetState() == Player::kStateVerifyOk);
    m_player->SetState(Player::kStateLoadingData);
}

void PlayerNetHandler::OnLoadEnd()
{
    CnAssert(m_player->GetState() == Player::kStateLoadingData);
    m_player->SetState(Player::kStateDataLoaded);


    // 设置角色在GateServer上的PlayerID
    GateHandler::Instance()->SendSetPlayerIDReq(m_player->GetCentGateID(), m_player->GetLoginID(), m_player->GetPlayerID());

    // 昆仑登陆log
    KunlunLog::Instance()->LogLogin(m_player->GetPlatformUID(), m_player->GetPlayerID(), 
        m_player->GetPlatformUName(), m_player->GetLoginIP(), m_player->GetLoginTime());
}

void PlayerNetHandler::OnLogout()
{
    bool DeleteMe = false;

    switch(m_player->GetState())
    {
    case Player::kStateInit:                    // 初始状态（Gate 上删了干净）
    case Player::kStateLoginReq:                // 登陆请求（Gate 上删了 Center上删了已有请求标记，干净）
    case Player::kStateVerifyOk:                // 验证OK（Gate 上删了 Center上删了干净）
    case Player::kStateLoadingData:             // 正在加载数据
    case Player::kStateDataLoaded:              // 加载数据结束
        {
            DeleteMe = true;
        }
        break;
    case Player::kStateDelivingData:            // 正在跟GameServer同步数据
        {
            m_pendingLogout = true;             // 延时下线
        }
        break;
    case Player::kStateInGame:                  // 在游戏中
        {
            m_player->SetState(Player::kStateLogoutSyncData);

            ExitDungeonReq();
        }
        break;
    case Player::kStateChangeDungeonSyncData:   // 切换地图同步数据
        {
            m_pendingLogout = true;             // 延时下线
        }
        break;
    case Player::kStateLogoutSyncData:          // Gate应该只会转发一次玩家离线消息
        {
            char idstr[30];
            Crown::SDUtoa64<30>(idstr, m_player->GetPlayerID());
            if(m_player->GetCentGameID() != 0)
            {
                CnWarn("Player %s logout twice\n", idstr);
            }
            else
            {   // GS断线,玩家退出处理
                CnDbg("logout but no gs\n");
                PlayerJob::Instance()->SavePlayerDataAndDestory(m_player);
            }
        }
        break;
    default:
        CnAssert(false);
        break;
    }

    if(DeleteMe)
    {
        if(m_player->GetState() <= Player::kStateVerifyOk)
        {
            PlayerManager::Instance()->DelPlayerByLoginID(m_player->GetLoginID());
        }
        else
        {
            PlayerManager::Instance()->DelPlayer(m_player->GetPlayerID());
        }
    }
}

void PlayerNetHandler::RecvSetPlayerIDAck( SET_PLAYER_ID_ACK* data )
{
    CnAssert(m_player->GetState() == Player::kStateDataLoaded);
    m_player->SetState(Player::kStateDelivingData);

    GameHandler::Instance()->SendDeliverPlayerInfoReq(m_player->GetCentGameID(), m_player);
}

void PlayerNetHandler::RecvDeliverPlayerInfoAck( PLAYER_BASE_INFO_ACK* data )
{
    if(data->errcode != ERR_SUCCEED)
    {
        // 踢出玩家的流程
        char idstr[30];
        Crown::SDUtoa64<30>(idstr, m_player->GetLoginID());
        CnError("Kickout account: loginID:%s, account:%s\n", idstr, m_player->GetAccount().c_str());
        KickOut(ERR_FAILED);
        return;
    }

    GateHandler::Instance()->SendSetGameSvrIDReq(m_player->GetCentGateID(), m_player->GetPlayerID(), m_player->GetCentGameID());

    switch(m_player->GetState())
    {
    case Player::kStateLogoutSyncData:  // 通知GameServer角色下线
        CnDbg("RecvDeliverPlayerInfoAck but player %s in logout\n", m_player->GetName().c_str());
        ExitDungeonReq();
        break;
    case Player::kStateDelivingData:
    case Player::kStateChangeDungeonSyncData:
        break;
    default:
        CnAssert(false);
        break;
    }
}

void PlayerNetHandler::RecvSetGameSvrIDAck( SET_GAMESVR_ID_ACK* data )
{
    if(!data)
    {
        CnAssert(false);
        return;
    }

    const ENTER_DUNGEON_REQ& enterDungeonReq = m_player->GetChangeDungeonData();
    uint32 unionID = UNION_ID(enterDungeonReq.channel_id, enterDungeonReq.dungeon_id, enterDungeonReq.scene_id);
    if(unionID)
    {
        
    }
    else
    {   // 登陆游戏后，第一次进副本
        unionID = m_player->GetUnionMapID();
        CnAssert(m_player->GetChangeMapCount() == 0);           // 第一次进副本

        ENTER_DUNGEON_REQ firstReq;
        firstReq.is_sameserver = false;
        firstReq.jump_type = DUNGEON_JUMP_TRIGGER;
        firstReq.dungeon_id = DUNGEON_ID(unionID);
        firstReq.scene_id = SCENE_ID(unionID);
        firstReq.channel_id = CHANNEL_ID(unionID);
        m_player->SetChangeDungeonData(firstReq);
    }

    switch(m_player->GetState())
    {
    case Player::kStateDelivingData:
    case Player::kStateChangeDungeonSyncData:
        {
            if(m_player->GetState() == Player::kStateDelivingData)
            {
                CnDbg("player %s first in game %d\n", m_player->GetName().c_str(), DUNGEON_ID(unionID));
            }
            m_player->SetState(Player::kStateInGame);
            EnterDungeonReq(m_player->GetCentGameID(), m_player->GetChangeDungeonData());

            // clear ChangeDungeonData
            CnAssert(m_player->GetChangeDungeonData().dungeon_id);
            ENTER_DUNGEON_REQ clearReq;
            m_player->SetChangeDungeonData(clearReq);
        }
        break;
    case Player::kStateLogoutSyncData:
        // 玩家下线了，就不再发EnterDungeon
        break;
    default:
        CnAssert(false);
        break;
    }
}

void PlayerNetHandler::FillPlayerInfo(const WRITEBACK_PLAYER_INFO_REQ* recvMsg)
{

    std::list<char> li;

    for (char c = 'a'; c <= 'z'; c++) {
        li.push_back(c); // 向后插入数据
    }


    // 复制角色属性信息
    m_player->SetName(recvMsg->prop_info.name);
    m_player->SetRegion(recvMsg->prop_info.region);
    m_player->SetSex(recvMsg->prop_info.sex);
    m_player->SetCareer(recvMsg->prop_info.career);
    m_player->SetLevel(recvMsg->prop_info.level);
    m_player->SetExp(recvMsg->prop_info.exp);
    //m_player->SetSysVigor(recvMsg->prop_info.sys_vigor);
    //m_player->SetBuyVigor(recvMsg->prop_info.buy_vigor);
    //m_player->SetVigorReftime(recvMsg->prop_info.vigor_reftime);
    m_player->SetDailyReftime(recvMsg->prop_info.daily_reftime);
    //m_player->SetVigorBuyTimes(recvMsg->prop_info.vigor_buytimes);
    //m_player->SetTeamBuyTimes(recvMsg->prop_info.team_buytimes);
    //m_player->SetTeamDungeonSysTimes(recvMsg->prop_info.team_dungeon_systimes);
    //m_player->SetTeamDungeonBuyTimes(recvMsg->prop_info.team_dungeon_buytimes);
    //m_player->SetStoneTurnGoldTimes(recvMsg->prop_info.stone_turn_gold_times);
    //m_player->SetVoideFiedlTime(recvMsg->prop_info.void_field_time);
    //m_player->SetVoidFieldReftime(recvMsg->prop_info.void_field_reftime);
    //m_player->SetBattlePower(recvMsg->prop_info.battle_power);

    m_player->SetGamePoint(recvMsg->prop_info.game_point);
    m_player->SetMoneyPoint(recvMsg->prop_info.money_point);
    m_player->SetVigorValue(recvMsg->prop_info.vigor);
    m_player->SetVigorReftime(recvMsg->prop_info.vigor_reftime);
    //m_player->SetBindMoneyPoint(recvMsg->prop_info.bind_money_point);
    //m_player->SetSoulPoint(recvMsg->prop_info.soul_point);
    //m_player->SetHonorPoint(recvMsg->prop_info.honor_point);
    //m_player->SetGuildPoint(recvMsg->prop_info.guild_point);
    //m_player->SetIncarnationPoint(recvMsg->prop_info.incarnation_point);
    //m_player->SetBattleSoulPoint(recvMsg->prop_info.battle_soul_point);
    //m_player->SetGrowthPoint(recvMsg->prop_info.vip_info.growth_point_recharge);
    //m_player->SetGrowthPointSpendplus(recvMsg->prop_info.spend_plus);

    m_player->SetHP(recvMsg->prop_info.hp);
    m_player->SetMP(recvMsg->prop_info.mp);
    m_player->SetGuildJoinTime(recvMsg->prop_info.guild_join_time);
    m_player->SetChangeMapCount(recvMsg->prop_info.change_dungeon_count);
    m_player->SetUnionMapID(recvMsg->prop_info.unionmap_id);
    m_player->SetX(recvMsg->prop_info.x);
    m_player->SetY(recvMsg->prop_info.y);
    m_player->SetCityMapID(recvMsg->prop_info.city_id);
    m_player->SetCityX(recvMsg->prop_info.city_x);
    m_player->SetCityY(recvMsg->prop_info.city_y);
    //m_player->SetFieldMapID(recvMsg->prop_info.field_map_id);
    //m_player->SetFieldX(recvMsg->prop_info.field_map_x);
    //m_player->SetFieldY(recvMsg->prop_info.field_map_y);
    //m_player->SetMapType(recvMsg->prop_info.map_type);
    //m_player->SetFieldHookX(recvMsg->prop_info.field_hook_x);
    //m_player->SetFieldHookY(recvMsg->prop_info.field_hook_y);
    m_player->SetOnLineTime(recvMsg->prop_info.online_time);
    //m_player->SetWelcomeReward(recvMsg->prop_info.getwelcome_reward);
    m_player->SetAngle(recvMsg->prop_info.angle);
    m_player->SetRightFlag(recvMsg->prop_info.right_flag);

    //m_player->SetVipInfo(recvMsg->prop_info.vip_info);
    //m_player->SetFcmInfo(recvMsg->prop_info.fcm_info);
    //m_player->SetVigorInfo(recvMsg->prop_info.vigor_info);

    //m_player->SetOffLineVigorInfo(recvMsg->prop_info.off_line_vigor_info);  //离线信息

    // 设置角色的副本信息
    m_player->SetPlayerDungeonInfo(recvMsg->player_dungeon_info);

    // 设置角色的状态信息
    m_player->SetStateInfo(recvMsg->state_info);

    // 设置开启系统信息
    m_player->SetOpenSystemInfo(recvMsg->open_system_info);

    // 设置公会福利信息
    //m_player->SetGuildWelfareInfo(recvMsg->prop_info.guild_welfare_info);

    // 设置城市争夺战信息
    //m_player->SetCityFightInfo(recvMsg->prop_info.city_fight_info);

    // 设置星宫图
    m_player->SetHoroscopeInfo(recvMsg->horoscope_info);

    // 复制装备信息
    m_player->SetEquipInfo(recvMsg->equip_info);

    // 设置药剂信息
    m_player->SetPotionInfo(recvMsg->potion_info);

//     // 复制装备信息
//     m_player->SetFashionInfo(recvMsg->fashion_info);

    // 复制宠物信息
    m_player->SetPetInfo(recvMsg->pet_info);

    //复制宠物上阵信息
    m_player->SetPetFormation(recvMsg->pet_formation_info);

    //复制骑宠信息
    m_player->SetRidePetInfo(recvMsg->ride_info);

    // 复制包裹信息
    m_player->SetItemInfo(recvMsg->item_info);

    // 复制仓库信息
    m_player->SetStoreInfo(recvMsg->store_info);   

    // 复制抽奖信息
    m_player->SetRandomPackageInfo(recvMsg->random_package_info);

    // 复制隐藏包裹信息
    //m_player->SetHideBagInfo(recvMsg->hide_bag_info);

    // 复制技能信息
    m_player->SetSkillInfo(recvMsg->skill_component_info);

    // 复制任务信息
    m_player->SetTaskInfo(recvMsg->task_info);

    // 复制引导信息
    m_player->SetGuideInfo(recvMsg->guide_info);

    // 复制善恶信息
    m_player->SetEnemyInfo(recvMsg->enemy_info);

    // 复制怪物之门数据
    m_player->SetMonsterDoorInfo(recvMsg->monster_door_info);

    // 复制勇者争霸数据
    m_player->SetPlayerDoorInfo(recvMsg->player_door_info);

    // 复制离线数据
    m_player->SetOfflinePlayerInfo(recvMsg->offline_info);

    // 复制奖励池数据
    m_player->SetRewardInfo(recvMsg->reward_info);

    // 复制通天塔数据
    m_player->SetBabelInfo(recvMsg->babel_info);

    // 充值积分抽奖
    m_player->SetPayPointLotteryInfo(recvMsg->pay_point_lottery_info);

    // VIP独享副本
    m_player->SetVIPOnlyDungeonInfo(recvMsg->vip_only_dungeon_info);

    // 复制邮件系统信息
    m_player->SetMailSystemInfo(recvMsg->mail_system_info);

    // 复制签到信息
    m_player->SetSignInfo(recvMsg->sign_info);

    // 复制在线奖励信息
    m_player->SetOnlineRewardInfo(recvMsg->online_reward_info);

    // 复制赛马信息
    m_player->SetRacingInfo(recvMsg->racing_info);

    // 复制采矿信息
    m_player->SetMiningInfo(recvMsg->mining_info);

    // 复制宝库信息
    m_player->SetTreasuryInfo(recvMsg->treasury_info);

    // 复制金像信息
    m_player->SetGoldtreeInfo(recvMsg->goldtree_info);

    // 复制大冒险信息
    m_player->SetAdventureInfo(recvMsg->adventure_info);

    // 复制答题信息
    m_player->SetAnswerInfo(recvMsg->answer_info);

    // 复制翅膀信息
    m_player->SetWingeInfo(recvMsg->wing_info);

    // 复制修罗场信息
    m_player->SetAshuraInfo(recvMsg->ashura_info);

    // 复制商城信息
    m_player->SetMallInfo(recvMsg->restriction_info);

    // 复制被动技能信息
    m_player->SetPassiveSkill(recvMsg->passive_skill_info);

    // 复制称号信息
    m_player->SetTitleInfo(recvMsg->title_info);

    // 复制日常投资信息
    m_player->SetDayInvestmentInfo(recvMsg->day_investment_info);

    // 复制等级投资信息
    m_player->SetLevelInvestmentInfo(recvMsg->level_investment_info);

    // 复制等级奖励信息
    m_player->SetLevelActivityInfo(recvMsg->level_activity_info);

    // 复制自动战斗设置信息
    m_player->SetAutoFightSettingInfo(recvMsg->auto_fight_setting_info);

    // 复制系统设置信息
    m_player->SetSystemSettingInfo(recvMsg->system_setting_info);

    // 复制玩家设置信息
    m_player->SetPlayerSettingInfo(recvMsg->player_setting_info);

    // 复制自动购买设置信息
    m_player->SetAutobuySettingInfo(recvMsg->autobuy_setting_info);

    // 复制虚空挂机设置信息
    m_player->SetVoidFieldSettingInfo(recvMsg->void_field_setting_info);

    // 复制活跃度信息
    m_player->SetLivenessInfo(recvMsg->liveness_info);

    // 复制守卫地宫信息
    m_player->SetDungeonTimesInfo(recvMsg->dungeon_times_list);

    //复制限购信息
    m_player->SetLimitBuyInfo(recvMsg->limit_buy_list);

    //复制单人答题信息
    m_player->SetSingleAnswerInfo(recvMsg->single_answer);

    // 复制圣剑信息
    m_player->SetSwordInfo(recvMsg->sword_info);

    //复制玩家僵尸生存信息
    m_player->SetZombieInfo(recvMsg->zombie_info);

    // 复制勇敢的心
    m_player->SetBhInfo(recvMsg->bh_info);

    // 复制礼品码信息
    m_player->SetGiftCodeInfo(recvMsg->gift_code_info);

    // 复制公会科技信息
    m_player->SetGuildScienceInfo(recvMsg->guild_science_info);

    // 复制游戏收藏信息
    m_player->SetShorcutInfo(recvMsg->shorcut_info);

    // 复制跨服PK
    m_player->SetMultiserverBattleInfo(recvMsg->multiserver_battle_info);

    // 复制投资计划
    m_player->SetInvestmentPlanInfo(recvMsg->investment_plan_info);

    // 复制捐献信息
    m_player->SetDonateInfo(recvMsg->donate_info);

    // 复制砸金蛋信息
    m_player->SetEggInfo(recvMsg->egg_info);

    // 复制越买越便宜信息
    m_player->SetBuyCheapInfo(recvMsg->buy_cheap);

    // 复制个性图标
    m_player->SetHeadPortraitInfo(recvMsg->head_portrait);
}

void PlayerNetHandler::RecvAutoSavePlayerInfoReq(WRITEBACK_PLAYER_INFO_REQ* recvMsg)
{
    if(m_player->GetState() != Player::kStateInGame)
    {
        CnWarn("gs auto save player %s failed, player not in game\n", m_player->GetName().c_str());
        return;
    }

    CnDbg("gs auto save player %s\n", m_player->GetName().c_str());
    
    int32 oldUnionMapID = m_player->GetUnionMapID();       // 不要改变unionMapID，会引起跳副本时unionMapID记录错误

    FillPlayerInfo(recvMsg);

    m_player->SetUnionMapID(oldUnionMapID);
}

void PlayerNetHandler::RecvWriteBackPlayerInfoReq( WRITEBACK_PLAYER_INFO_REQ* recvMsg )
{
    // 更新角色信息
    FillPlayerInfo(recvMsg);

    // 发送确认信息
    GameHandler::Instance()->SendWriteBackPlayerInfoAck(m_player->GetCentGameID(), m_player->GetPlayerID());

    switch(m_player->GetState())
    {
    case Player::kStateChangeDungeonSyncData:
        {
            uint32 dungeonID = m_player->GetChangeDungeonData().dungeon_id;
            uint32 centGameID = GameHandler::Instance()->AskJumpCentGameServerID(m_player, dungeonID);

            if(!GameHandler::Instance()->IsValidGame(centGameID))
            {   // 要跳转的GS不存在或者封闭,进入logout状态,并kickout玩家
                CnDbg("no gs kick out %s\n", m_player->GetName().c_str());
                NoGSKickOut();
                return;
            }

            if(centGameID == m_player->GetCentGameID())
            {
                CnDbg("player jump same server\n");         // 在跨服GS安全退出时会发生
            }

            m_player->SetCentGameID(centGameID);

            GameHandler::Instance()->SendDeliverPlayerInfoReq(centGameID, m_player);
        }
        break;
    case Player::kStateDelivingData:
    case Player::kStateLogoutSyncData:
        {
            TeamManager::Instance()->LeaveTeam(m_player->GetPlayerID());    // 玩家下线离队

            CrossTeamManager::Instance()->LeaveTeam(m_player->GetPlayerID());   // 玩家下线离队

            // 和 DB 间同步数据
            PlayerJob::Instance()->SavePlayerDataAndDestory(m_player);
        }
        break;
    default:
        WARN("Unknow m_player state:%d\n", m_player->GetState());
        CnAssert(false);
        break;
    }
}

void PlayerNetHandler::RecvChangeDungeon( CHANGE_DUNGEON_REQ* recvMsg )
{
    if(!recvMsg) return;

    uint32 centerID = CalCenterIDByRegionID(m_player->GetCentGateID());
    CnAssert(centerID);

    // 检查是否存在该副本
    if(!DungeonManager::Instance()->HasDungeonId(recvMsg->dungeon_id))
    {
        CnInfo("dungeon %d not exist\n", recvMsg->dungeon_id);
        GateHandler::Instance()->SendChangeDungeonAck(ERR_CHANGE_DUNGEON_NO_DUNGEON, m_player->GetCentGateID(), m_player->GetPlayerID());
        return; // 无此副本
    }

    uint32 centGameID = GameHandler::Instance()->AskJumpCentGameServerID(m_player, recvMsg->dungeon_id);
    if(CalGameIDByRegionID(centGameID) == 0)
    {
        CnWarn("dungeon %d can't find GameServer\n", recvMsg->dungeon_id);
        GateHandler::Instance()->SendChangeDungeonAck(ERR_CHANGE_DUNGEON_NO_DUNGEON, m_player->GetCentGateID(), m_player->GetPlayerID());
        return;
    }

    if(!GameHandler::Instance()->IsValidGame(centGameID))
    {
        // 不存在跳转的副本
        CnWarn("dungeon %d in gameserver %d not exist\n", recvMsg->dungeon_id, CalGameIDByRegionID(centGameID));
        GateHandler::Instance()->SendChangeDungeonAck(ERR_CHANGE_DUNGEON_NO_DUNGEON, m_player->GetCentGateID(), m_player->GetPlayerID());
        return;
    }
    
    switch(m_player->GetState())
    {
    case Player::kStateInit:                    // 初始状态
    case Player::kStateLoginReq:                // 登陆请求
    case Player::kStateVerifyOk:                // 验证OK
    case Player::kStateLoadingData:             // 正在加载数据
    case Player::kStateDataLoaded:              // 加载数据结束
        {
            CnAssert(false);
        }
    case Player::kStateDelivingData:            // 正在跟GameServer同步数据
    case Player::kStateChangeDungeonSyncData:   // 切换地图同步数据（这个臭屌丝连着点了几次按钮）
        {
            char idstr[30];
            Crown::SDUtoa64<30>(idstr, m_player->GetLoginID());
            CnWarn("player %s can't jump dungeon %d by state\n", idstr, recvMsg->dungeon_id);
            GateHandler::Instance()->SendChangeDungeonAck(ERR_CHANGE_DUNGEON_IN_CHANGE, m_player->GetCentGateID(), m_player->GetPlayerID());
        }
        break;
    case Player::kStateInGame:                  // 在游戏中
        {
            uint32 errCode = DungeonManager::Instance()->RequestJumpDungeonId(recvMsg->dungeon_id, centGameID, m_player);
            if(errCode != ERR_SUCCEED)
            {   // 请求跳转的副本失败
                GateHandler::Instance()->SendChangeDungeonAck(errCode, m_player->GetCentGateID(), m_player->GetPlayerID());
                return;
            }

            uint32 unionId = UNION_ID(recvMsg->channel_id, recvMsg->dungeon_id, recvMsg->scene_id);

            // 生成跳副本数据
            ENTER_DUNGEON_REQ enterDungeonReq;
            enterDungeonReq.jump_type = recvMsg->jump_type;
            enterDungeonReq.dungeon_id = recvMsg->dungeon_id;
            enterDungeonReq.scene_id = recvMsg->scene_id;
            enterDungeonReq.channel_id = recvMsg->channel_id;
            enterDungeonReq.object_id = recvMsg->object_id;
            enterDungeonReq.pos_x = recvMsg->pos_x;
            enterDungeonReq.pos_y = recvMsg->pos_y;
            enterDungeonReq.fly_player_id = recvMsg->fly_player_id;

            if(m_player->GetCentGameID() == centGameID)
            {
                enterDungeonReq.is_sameserver = true;

                EnterDungeonReq(m_player->GetCentGameID(), enterDungeonReq);
            }
            else
            {
                CnAssert(m_player->GetChangeDungeonData().dungeon_id == 0);

                // 设置角色状态为切换地图状态
                m_player->SetState(Player::kStateChangeDungeonSyncData);

                enterDungeonReq.is_sameserver = false;
                m_player->SetChangeDungeonData(enterDungeonReq);

                ExitDungeonReq();
            }
        }
        break;
    case Player::kStateLogoutSyncData:          // 都下线了还发切换地图（Gate上应该已经不会再转发这种数据了）
        {
            char idstr[30];
            Crown::SDUtoa64<30>(idstr, m_player->GetPlayerID());
            WARN("Player %s has been logout", idstr);  // 这种情况是有可能的
            //CnAssert(false);
        }
        break;
    default:
        CnAssert(false);
        break;
    }
}

void PlayerNetHandler::ExitDungeonReq()
{
    // 进入退出流程，只可能在这两个状态中
    if(m_player->GetState() == Player::kStateChangeDungeonSyncData || m_player->GetState() == Player::kStateLogoutSyncData)
    {
        if(m_player->m_InDungeonCheck)
        {
            DungeonManager::Instance()->DecreasePlayer(DUNGEON_ID(m_player->GetUnionMapID()), m_player);
            CnAssert(m_isEnterDungeon);
            m_isEnterDungeon = false;
        }
        else
            CnAssert(!m_isEnterDungeon);
    }
    else
        CnAssert(false);

    if(GameHandler::Instance()->IsConnectGame(m_player->GetCentGameID()))
        GameHandler::Instance()->SendExitDungeonReq(m_player->GetCentGameID(), m_player->GetPlayerID());
    else
    {   // 玩家下线,所在的GS不存在了。
        CnWarn("player %s exit dungeon no GS\n", m_player->GetName().c_str());
        NoGSKickOut();
    }
}
// GS不可用时的踢人流程
void PlayerNetHandler::NoGSKickOut()
{
    CnDbg("no gs kickout %s\n", m_player->GetName().c_str());
    m_player->SetCentGameID(0);
    m_player->SetState(Player::kStateLogoutSyncData);
    KickOut(ERR_SERVER_NO_READY);
}

void PlayerNetHandler::EnterDungeonReq(uint32 centGameID, const ENTER_DUNGEON_REQ& enterDungeonData)
{
    if(!enterDungeonData.is_sameserver)
    {   // 不同GameServer时，必先执行ExitDungeonReq()
        CnAssert(!m_isEnterDungeon);
        m_isEnterDungeon = true;
    }

    GameHandler::Instance()->SendEnterDungeonReq(centGameID, m_player->GetPlayerID(), enterDungeonData);

    uint32 unionmapID = UNION_ID(enterDungeonData.channel_id, enterDungeonData.dungeon_id, enterDungeonData.scene_id);

    if(enterDungeonData.is_sameserver)
    {
        DungeonManager::Instance()->DecreasePlayer(DUNGEON_ID(m_player->GetUnionMapID()), m_player);
        DungeonManager::Instance()->IncreasePlayer(DUNGEON_ID(unionmapID), m_player);
    }
    else
    {
        CnAssert(m_player->GetChangeMapCount() == 0 || m_player->GetUnionMapID() != unionmapID);       // 跳地图，则unionmapID必不同
        DungeonManager::Instance()->IncreasePlayer(DUNGEON_ID(unionmapID), m_player);
    }

    m_player->SetUnionMapID(unionmapID);

    m_player->NotifyFriendMyInfo();   // 告知好友unionmapID改变

    TeamManager::Instance()->OnPlayerChangeDungeon(m_player);   // 玩家跳转副本通知队友

    OfflineEventManager::Instance()->OnPlayerChangeDungeon(m_player);    // 玩家进入副本发送离线事件

    CnAssert( PlayerManager::Instance()->GetPlayerNum() >= DungeonManager::Instance()->GetPlayerNum() );    // 有玩家已经退出副本，但数据仍然在(回写数据库中)
}

bool PlayerNetHandler::HasGSPlayer() 
{
    return m_player->GetState() == Player::kStateDelivingData 
        || m_player->GetState() == Player::kStateInGame
        || m_player->GetState() == Player::kStateChangeDungeonSyncData;
}




