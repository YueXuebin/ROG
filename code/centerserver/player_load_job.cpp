#include "centerserver_pch.h"
#include "player_load_job.h"
#include "player_info_parser.h"
#include "dbdriver.h"
#include "player_manager.h"
#include "player.h"
#include "player_net_handler.h"
#include "centerserver.h"
#include "gate_handler.h"
#include "player_load_parser.h"
#include "guild_manager.h"


IMPLEMENT_SINGLETON(PlayerLoadJob)

void PlayerLoadJob::LoadData(uint32 gateSvrID,  uint64 loginID, const std::string& account, const std::string& login_server_name, uint32 addrIP, uint16 addrPort)  // 加载角色数据
{
    Player* player = NULL;
    // 存储过程开始
    m_dbExecutor->StartTransaction();

    uint16 errCode = ERR_FAILED;

    errCode = LoadPlayerData(account, loginID, login_server_name);
    if(errCode!= ERR_SUCCEED)
    {
        goto _ERROR;
    }

    player = PlayerManager::Instance()->GetPlayerByLoginID(loginID);
    if(!player)
    {
        CnInfo("no loginID player\n");
        errCode = ERR_UNFOUND_PLAYER;
        goto _ERROR; 
    }

    // 读取角色ID和角色名称后，将角色加入角色ID和名称映射表
    PlayerManager::Instance()->AddPlayerForLoaded(player);

    player->GetNetHandler()->OnLoadStart();

    CenterServer::Instance()->AddPlayerToCenter(gateSvrID, loginID, false);

    //加载各个功能模块里面的数据
    errCode = LoadModuleData(player);
    if(errCode != ERR_SUCCEED)
        goto _ERROR;

    // 存储过程结束
    m_dbExecutor->EndTransaction();

    player->OnCreate();

    return;
_ERROR:
    // 存储过程结束
    m_dbExecutor->Rollback();
    m_dbExecutor->EndTransaction();

    if(errCode == ERR_LOAD_DB_LOGIN)
    {
        if (player)
            GateHandler::Instance()->SendLoadPlayerAck(ERR_LOAD_DB_LOGIN, player->GetCentGateID(), player->GetLoginID());
        // 删除角色
        PlayerManager::Instance()->DelPlayer(player->GetPlayerID());
    }
    else if(errCode == ERR_UNFOUND_PLAYER)
    {
        // 未找到账户信息（说明此账号未创建过角色，但此时不要删掉已经通过验证的Player）
        GateHandler::Instance()->SendLoadPlayerAck(errCode, gateSvrID, loginID);
    }
    else
    {   // 解析信息失败
        GateHandler::Instance()->SendLoadPlayerAck(errCode, gateSvrID, loginID);

        // 通过 LoginID 删除角色
        PlayerManager::Instance()->DelPlayerByLoginID(loginID);
    }
}

//加载各个功能模块里面的数据
uint16 PlayerLoadJob::LoadModuleData(Player* player)
{
    if(!player)  return ERR_FAILED;

    uint64 playerid = player->GetPlayerID();

    // 物品
    if(!LoadItemData(playerid))
        return ERR_LOAD_DB_LOGIN;

    // 仓库
    if(!LoadStoreData(playerid))
        return  ERR_LOAD_DB_LOGIN;

    // 装备
    if(!LoadEquipData(playerid))
        return ERR_LOAD_DB_LOGIN;

    // 药剂
    if(!LoadPotionData(playerid))
        return ERR_LOAD_DB_LOGIN;

    //// 如果有公会则加载公会信息
    //if(player->GetGuildID() && !GuildManager::Instance()->GetGuildInfo(player->GetGuildID()))
    //{
    //    if(!LoadGuildInfoForMgr(player->GetGuildID()))
    //        return ERR_LOAD_DB_LOGIN;
    //}

    //// 星空图
    //if(!LoadHoroscope(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //加载技能
    if(!LoadSkillData(playerid))
        return ERR_LOAD_DB_LOGIN;

	////加载天赋
	if(!LoadPassiveSkillData(playerid))
		return ERR_LOAD_DB_LOGIN;

    //加载宠物
    if(!LoadPetData(playerid))
        return ERR_LOAD_DB_LOGIN;

    ////加载骑宠
    //if(!LoadRideData(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    ////加载时装数据
    //if(!LoadFashionData(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    // 加载奖数据
    if(!LoadRandomPackageData(playerid))
        return  ERR_LOAD_DB_LOGIN;

    //// 加载隐藏物品
    //if(!LoadHideItemInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    // 加载好友
    if(!LoadFrined(playerid))
        return ERR_LOAD_DB_LOGIN;

    // 加载任务数据
    if(!LoadTaskData(playerid))
        return  ERR_LOAD_DB_LOGIN;

    // 加载引导和功能开启数据
    if(!LoadGuideAndOpenSysData(playerid))
        return ERR_LOAD_DB_LOGIN;

    //// 加载善恶信息
    //if(!LoadEnemyInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 加载角色奖励池
    //if(!LoadRewardPool(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 加载通天塔数据
    //if(!LoadBabelInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    ////加载充值积分数据
    //if (!LoadPayPointLotteryInfo(playerid))
    //{
    //    return ERR_LOAD_DB_LOGIN;
    //}

    ////加载VIP独享副本
    //if (!LoadVIPOnlyDungeonInfo(playerid))
    //{
    //    return ERR_LOAD_DB_LOGIN;
    //}

    //// 加载勇敢的心数据
    //if(!LoadBhInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 加载日常活动信息
    //if(!LoadDailyActivityInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 加载上线活动信息
    if(!LoadOnlineActivityInfo(playerid))
        return  ERR_LOAD_DB_LOGIN;

    //// 加载个人邮件数据
    if(!LoadPlayerMailData(playerid))
        return  ERR_LOAD_DB_LOGIN;

    //// 加载系统邮件数据
    if(!LoadSystemMailData(playerid))
        return  ERR_LOAD_DB_LOGIN;

    //// 加载组邮件数据
    //if(!LoadGroupMailData(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 获得怪物之门信息
    //if(!LoadMonsterDoorInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    ////加载勇者争霸数据
    //if(!LoadPlayerDoorInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 加载翅膀信息
    if(!LoadWingInfo(playerid))
        return  ERR_LOAD_DB_LOGIN;

    //// 加载修罗场信息
    //if(!LoadAshuraInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 加载充值信息
    //if(!LoadRechargeInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 加载商城信息
    if(!LoadMallInfo(playerid))
        return  ERR_LOAD_DB_LOGIN;

    //// 加载称号信息
    //if(!LoadTitleInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 加载日常投资理财信息
    //if(!LoadDayInvestmentInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 加载等级投资理财信息
    //if(!LoadLevelInvestmentInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 获得设置信息
    //if(!LoadSettingInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    ////加载活跃度信息
    //if(!LoadLivenessInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 加载副本次数信息
    //if(!LoadDungeonTimesInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    // 加载角色副本信息
    if(!LoadPlayerDungeonInfo(playerid))
        return  ERR_LOAD_DB_LOGIN;

    ////加载限购信息
    //if(!LoadLimitBuyInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 加载资源挽回信息
    //if(!LoadGainbackResourceInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 加载圣剑信息
    //if(!LoadSwordInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 加载僵尸生存信息
    //if(!LoadZombieInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 加载礼品卡信息
    //if(!LoadGiftCodeInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 加载公会科技信息
    //if(!LoadGuildScienceInfo(playerid))
    //    return  ERR_LOAD_DB_LOGIN;

    //// 加载游戏收藏信息
    //if(!LoadShorcutData(playerid))
    //    return ERR_LOAD_DB_LOGIN;

    //// 加载跨服战斗信息
    //if(!LoadMultiserverBattleData(playerid))
    //    return ERR_LOAD_DB_LOGIN;

    //// 加载宠物列阵信息
    //if(!LoadPetFormation(playerid))
    //    return ERR_LOAD_DB_LOGIN;

    //// 加载投资理财信息
    //if(!LoadInvestmentPlan(playerid))
    //    return ERR_LOAD_DB_LOGIN;

    ////加载捐献信息
    //if (!LoadDonate(playerid))
    //    return ERR_LOAD_DB_LOGIN;

    ////加载砸金蛋
    //if (!LoadEgg(playerid))
    //    return ERR_LOAD_DB_LOGIN;

    ////加载角色状态信息
    //if (!LoadPlayerStateInfo(playerid))
    //    return ERR_LOAD_DB_LOGIN;

    // 加载角色副本信息
    if(!LoadHeadPortraitInfo(playerid))
        return  ERR_LOAD_DB_LOGIN;

    return ERR_SUCCEED;
}

uint16  PlayerLoadJob::LoadPlayerData(const std::string& account, uint64 loginID, const std::string& platform_server_name)
{
    char account_str[PLAYER_TOKEN_LEN*2+1] = {0};
    m_dbExecutor->EscapeString(account_str, account.c_str(), account.length());

    char platform_server_name_str[PLAYER_TOKEN_LEN*2+1] = {0};
    m_dbExecutor->EscapeString(platform_server_name_str, platform_server_name.c_str(), platform_server_name.length());

    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `account_name`,"
        "`player_name`,"
        "`player_id`,"
        "`platform_server_name`,"
        "`region`,"
        "`sex`,"
        "`career`,"
        "`level`,"
        "`exp`,"
        "`money_point`,"
        "`game_point`,"
        "`vigor`,"
        "`vigor_reftime`,"
        "`hp`,"
        "`mp`,"
        "`unionmap_id`,"
        "`x`,"
        "`y`,"
        "`angle`,"
        "`right_flag`,"
        "unix_timestamp(`daily_reftime`)"
        "FROM player where `account_name`='%s' AND `platform_server_name`='%s'", account_str, platform_server_name_str);

    // sql执行错误
    if(!recorder.HasExecut())
    {
        return ERR_LOAD_DB_LOGIN;
    }

    // 没有此角色
    if(!recorder.HasRecord())
    {
        // 未找到账户信息（说明此账号未创建过角色，但此时不要删掉已经通过验证的Playe
        CnInfo("server_name=%s account=%s unfound player\n", platform_server_name.c_str(), account.c_str());
        return ERR_UNFOUND_PLAYER;
    }

    return PlayerInfoParser::Instance()->ParsePlayerInfo(recorder, loginID);
}

//加载星空图
bool PlayerLoadJob::LoadHoroscope(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `horoscope_level`, `horoscope_info` FROM horoscope WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound horoscope\n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseHoroscope(recorder, playerID);
}

//加载技能数据
bool PlayerLoadJob::LoadSkillData(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `skill_info` FROM skill WHERE player_id=%llu", playerID);
    if(!recorder.HasExecut())
    {
        CnInfo("unfound skilldata \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseSkillInfo(recorder, playerID);
}
//加载天赋数据
bool PlayerLoadJob::LoadPassiveSkillData(uint64 playerID)
{
	DBRecorder recorder;
	recorder = m_dbExecutor->Query(
		"SELECT `available_points`,`passive_skill_info` FROM passive_skill WHERE player_id=%llu", playerID);

	if (!recorder.HasExecut())
	{
		CnInfo("unfound passiveskilldata \n");
		return false;
	}

	return PlayerLoadParser::Instance()->ParsePassiveSkillInfo(recorder, playerID);
}
//加载宠物
bool PlayerLoadJob::LoadPetData(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `mercenary_info` FROM pet WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound loadpetdata \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParsePetInfo(recorder, playerID);
}

//加载骑宠
bool PlayerLoadJob::LoadRideData(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `ride_info` FROM ride WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound loadridepetdata \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseRideInfo(recorder, playerID);
}


// 加载装备
bool PlayerLoadJob::LoadEquipData(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `equip_info` FROM `equip` WHERE player_id=%llu;", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound equip!\n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseEquipInfo(recorder, playerID);
}

// 加载药剂
bool PlayerLoadJob::LoadPotionData(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `potion_info` FROM `potion` WHERE player_id=%llu;", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound potion!\n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParsePotionInfo(recorder, playerID);
}

// ==================================================
// 加载时装数据
// ==================================================
bool PlayerLoadJob::LoadFashionData(uint64 playerID)
{

    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `fashion_info` FROM equip WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadFashionData \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseFashionInfo(recorder, playerID);
}

// ==================================================
// 加载物品数据
// ==================================================
bool PlayerLoadJob::LoadItemData(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `item_info` FROM `item` WHERE player_id=%llu;", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound item!\n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseItemInfo(recorder, playerID);
}

// ==================================================
// 记载仓库数据
// ==================================================
bool PlayerLoadJob::LoadStoreData(const uint64 & playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `item_info` FROM `store` WHERE player_id=%llu;", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound item!\n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseStoreInfo(recorder, playerID);
}

// ==================================================
// 加抽奖数据
// ==================================================
bool PlayerLoadJob::LoadRandomPackageData(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `package_info` FROM `player_package` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadRandomPackageData \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseRandomPackageInfo(recorder, playerID);
}

// ==================================================
// 加载隐藏物品
// ==================================================
bool PlayerLoadJob::LoadHideItemInfo(uint64 playerID)
{

    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `hide_item_info` FROM item WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadHideItemInfo \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseHideItemInfo(recorder, playerID);
}
// ==================================================
// 加载好友
// ==================================================
bool PlayerLoadJob::LoadFrined(uint64 playerID)
{
    bool ret1, ret2;
    DBRecorder recorder;

    recorder = m_dbExecutor->Query(
        "SELECT T1.player_id, `player_name`, `sex`, `level`, UNIX_TIMESTAMP(`logout_time`), head_portrait.head_portrait_open AS head_portrait "
        "FROM player T1 INNER JOIN friend T2 ON T1.player_id = T2.player_id_b "
        "LEFT JOIN head_portrait ON head_portrait.player_id = T1.player_id "
        "WHERE T2.player_id_a=%llu ",
        playerID, playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadFrined \n");
        return false;
    }

    // 加载我有哪些好友
    ret1 = PlayerLoadParser::Instance()->ParseFriend(recorder, playerID);


    recorder = m_dbExecutor->Query(
        "SELECT T3.player_id, `player_name`, `sex`, `level`, UNIX_TIMESTAMP(`logout_time`) "
        "FROM player T3 INNER JOIN friend T4 ON T3.player_id=T4.player_id_a "
        "WHERE T4.player_id_b=%llu",
        playerID, playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadFrined \n");
        return false;
    }
    // 加载哪些好友有我
    ret2 = PlayerLoadParser::Instance()->ParseFriendHasMe(recorder, playerID);///////////////////
    
    return (ret1 && ret2);
}

// 加载任务数据
bool PlayerLoadJob::LoadTaskData(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `game_info`, `festival_info`, `guild_info` FROM `task` WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadTaskData \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseTaskInfo(recorder, playerID);
}

// 加载引导数据
bool PlayerLoadJob::LoadGuideAndOpenSysData(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `guide_info`, `open_system_info` FROM `guide` WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadGuideAndOpenSysData\n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseGuideAndOpenSysInfo(recorder, playerID);
}

// 加载善恶信息
bool PlayerLoadJob::LoadEnemyInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `enemy_info` FROM `enemy` WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadEnemyInfo \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParsetEnemyInfo(recorder, playerID);
}

// ==================================================
// 加载角色奖励池
// ==================================================
bool PlayerLoadJob::LoadRewardPool(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query( 
        "SELECT `reward_info` FROM reward_store WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadRewardPool \n");
        return false;
    }
    return PlayerLoadParser::Instance()->ParseRewardPool(recorder, playerID);////////////////////
}
// ==================================================
// 加载通天塔数据
// ==================================================
bool PlayerLoadJob::LoadBabelInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query( 
        "SELECT `babel_info` FROM babel WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadBabelInfo \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseBabelInfo(recorder, playerID);/////////////
}

// ==================================================
// 加载充值积分抽奖数据
// ==================================================
bool PlayerLoadJob::LoadPayPointLotteryInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `pay_point_lottery_info` FROM pay_point_lottery WHERE player_id=%llu", playerID);

    if (!recorder.HasExecut())
    {
        CnInfo("unfound LoadPayPointLotteryInfo \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParsePayPointLotteryInfo(recorder, playerID);
}

// ==================================================
// 加载充值积分抽奖数据
// ==================================================
bool PlayerLoadJob::LoadVIPOnlyDungeonInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `vip_only_dungeon_info` FROM vip_only_dungeon WHERE player_id=%llu", playerID);

    if (!recorder.HasExecut())
    {
        CnInfo("unfound LoadVIPOnlyDungeonInfo \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseVIPOnlyDungeonInfo(recorder, playerID);
}

// ==================================================
// 加载勇敢的心数据
// ==================================================
bool PlayerLoadJob::LoadBhInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `bh_info` FROM brave_heart WHERE player_id=%llu", playerID);
    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadBhInfo \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseBhInfo(recorder, playerID);/////////////////
}

// ==================================================
// 加载日常活动信息
// ==================================================
bool PlayerLoadJob::LoadDailyActivityInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `racing_info`, `mining_info`, `treasury_info`, `goldtree_info`, `adventure_info`, `answer_info` ,`single_answer_info` FROM `daily_activity` WHERE `player_id`=%llu", playerID);
    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadDailyActivityInfo \n");
        return false;
    }
    PlayerLoadParser::Instance()->ParseDailylActivityInfo(recorder, playerID);/////////////////

    return true;
}

// ==================================================
// 加载上线活动信息
// ==================================================
bool PlayerLoadJob::LoadOnlineActivityInfo(uint64 playerID)
{
    //m_dbExecutor->Query(this, &PlayerJob::CBLoadDailylActivityInfo, playerID, 
    // "SELECT `racing_info`, `mining_info`, `treasury_info`, `goldtree_info`, `adventure_info`, `answer_info` ,`single_answer_info` FROM `daily_activity` WHERE `player_id`=%llu", playerID);
    DBRecorder recorder;
    recorder = m_dbExecutor->Query( 
        "SELECT `sign_info` FROM `online_activity` WHERE `player_id`=%llu", playerID);
    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadOnlineActivityInfo \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseOnlineActivityInfo(recorder, playerID);

    return true;
}

// ==================================================
// 加载邮件数据
// ==================================================
bool PlayerLoadJob::LoadPlayerMailData(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query( 
        "SELECT `mail_id`, `receiver_id`, `sender_id`, `sender_name`, UNIX_TIMESTAMP(`send_time`), "
        "`mail_type`, `mail_flag`, `title`, `message`, `attachment_flag`, `attachment` "
        "FROM `player_mail` WHERE `receiver_id`=%llu AND `mail_flag`!=%u ORDER BY `send_time` DESC LIMIT %u", 
        playerID, MAIL_FLAG_DELETED, MAX_PLAYER_MAIL_NUM);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadPlayerMailData \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParsePlayerMailData(recorder, playerID);

    return true;
}

// ==================================================
// 加载系统邮件数据
// ==================================================
bool PlayerLoadJob::LoadSystemMailData(uint64 playerID)
{
    DBRecorder recorder;
    recorder= m_dbExecutor->Query(
        "SELECT T3.`mail_id`, UNIX_TIMESTAMP(T3.`send_time`), T3.`mail_type`, IFNULL(T3.`mail_flag`, 0), "
        "T3.`title`, T3.`message`, IFNULL(T3.`attachment_flag`, 0), T3.`attachment` "
        "FROM (SELECT T1.`mail_id`, T1.`send_time`, T1.`mail_type`, IFNULL(T2.`mail_flag`, 0) AS mail_flag, "
        "T1.`title`, T1.`message`, IFNULL(T2.`attachment_flag`, 0) AS attachment_flag, T1.`attachment` FROM `system_mail` AS T1 LEFT JOIN `system_mail_flag` AS T2 "
        "ON T1.`mail_id` = T2.`mail_id` AND T2.`player_id`=%llu) AS T3 WHERE T3.`mail_flag` != %d",
        playerID, MAIL_FLAG_DELETED);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadSystemMailData \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseSystemMailData(recorder, playerID);

    return true;
}

// ==================================================
// 加载组邮件数据
// ==================================================
bool PlayerLoadJob::LoadGroupMailData(uint64 playerID)
{
    DBRecorder recorder;
    recorder= m_dbExecutor->Query(
        "SELECT T1.`mail_id`, UNIX_TIMESTAMP(T1.`send_time`), T1.`mail_type`, T2.`mail_flag`, "
        "T1.`title`, T1.`message`, T2.`attachment_flag`, T1.`attachment` FROM `group_mail` AS T1 INNER JOIN `group_mail_flag` AS T2 "
        "ON T1.`mail_id` = T2.`mail_id` AND T2.`player_id`=%llu AND T2.`mail_flag` != %d",
        playerID, MAIL_FLAG_DELETED);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadGroupMailData \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseGroupMailData(recorder, playerID);

    return true;
}

// ==================================================
// 获得怪物之门信息
// ==================================================
bool PlayerLoadJob::LoadMonsterDoorInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `monster_door` FROM offline_battle WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadMonsterDoorInfo \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseMonsterDoorInfo(recorder, playerID);
}
// ==================================================
//加载勇者争霸数据
// ==================================================
bool PlayerLoadJob::LoadPlayerDoorInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `player_door` FROM offline_battle WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadPlayerDoorInfo \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParsePlayerDoorInfo(recorder, playerID);//////////////////////

    return true;
}

// ==================================================
// 加载翅膀信息//
// ==================================================
bool PlayerLoadJob::LoadWingInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query( 
        "SELECT `wing_info` FROM `wing` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadWingInfo \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseWingInfo(recorder, playerID);
}

// ==================================================
// 加载修罗场信息
// ==================================================
bool PlayerLoadJob::LoadAshuraInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query( 
        "SELECT `ashura_info`, `kill_count` FROM `ashura` WHERE `player_id`=%llu", playerID);
    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadAshuraInfo \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseAshuraInfo(recorder, playerID);
}

// ==================================================
// 加载充值信息
// ==================================================
bool PlayerLoadJob::LoadRechargeInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT %d,`money_point`, `money_point`, UNIX_TIMESTAMP(`update_time`) FROM `recharge_total` WHERE `player_id`=%llu UNION "
        "SELECT %d,`money_point`, `max_single`, UNIX_TIMESTAMP(`update_time`) FROM `recharge_daily` WHERE `player_id`=%llu",
        RECHARGE_ID_TOTAL_RECHARGE, playerID,
        RECHARGE_ID_DAILY_RECHARGE, playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadRechargeInfo \n");
        return false;
    }
    PlayerLoadParser::Instance()->ParseRechargeInfo(recorder, playerID);

    return true;
}

// ==================================================
// 加载商城信息
// ==================================================
bool PlayerLoadJob::LoadMallInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `mall_info` FROM `player_mall` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadMallInfo \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseMallInfo(recorder, playerID);
}

// ==================================================
// 加载称号信息
// ==================================================
bool PlayerLoadJob::LoadTitleInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `title_info` FROM `title` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadTitleInfo \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseTitleInfo(recorder, playerID);
}
// ==================================================
// 加载日常投资理财信息
// ==================================================
bool PlayerLoadJob::LoadDayInvestmentInfo( uint64 playerID )
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `day_investment_info` FROM `day_investment` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadDayInvestmentInfo \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseDayInvestmentInfo(recorder, playerID);
}
// ==================================================
// 加载等级投资理财信息
// ==================================================
bool PlayerLoadJob::LoadLevelInvestmentInfo( uint64 playerID )
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `level_investment_info` FROM `level_investment` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadLevelInvestmentInfo \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseLevelInvestmentInfo(recorder, playerID);
}
// ==================================================
// 获得等级活动
// ==================================================
bool PlayerLoadJob::LoadLevelActivityInfo( uint64 playerID )
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `level_activity_info` FROM `level_activity` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadLevelActivityInfo \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseLevelActivityInfo(recorder, playerID);
}
// ==================================================
// 获得设置信息
// ==================================================
bool PlayerLoadJob::LoadSettingInfo( uint64 playerID )
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `auto_fight_setting`,`system_setting`,`player_setting`,`autobuy_setting`,`void_field_setting` FROM setting WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadSettingInfo \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseSettingInfo(recorder, playerID);

    return true;
}
// ==================================================
//加载活跃度信息
// ==================================================
bool PlayerLoadJob::LoadLivenessInfo( uint64 playerID )
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `liveness_info` FROM `liveness` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadLivenessInfo \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseLivenessInfo(recorder, playerID);
}
// ==================================================
// 加载副本次数信息
// ==================================================
bool PlayerLoadJob::LoadDungeonTimesInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `dungeon_times_info` FROM `dungeon_times` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadDungeonTimesInfo \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseDungeonTimesInfo(recorder, playerID);
    return true;
}
// ==================================================
// 加载角色副本信息
// ==================================================
bool PlayerLoadJob::LoadPlayerDungeonInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `open_dungeon_info`, `plot_dungeon_info`, `soul_dungeon_info`, `daily_dungeon_info`, `dungeon_record` FROM `player_dungeon_info` WHERE `player_id`=%llu", playerID);
    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadPlayerDungeonInfo \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParsePlayerDungeonInfo(recorder, playerID);

    return true;
}
// ==================================================
//加载限购信息
// ==================================================
bool PlayerLoadJob::LoadLimitBuyInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query( 
        "SELECT `limit_buy_info` FROM `limitbuy` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadLimitBuyInfo \n");
        return false;
    }

    return PlayerLoadParser::Instance()->ParseLimitBuyInfo(recorder, playerID);
}
// ==================================================
// 加载资源挽回信息
// ==================================================
bool PlayerLoadJob::LoadGainbackResourceInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `gainback_info` FROM `gainback_resource` WHERE `player_id`=%llu", playerID);
    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadGainbackResourceInfo \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseGainbackResourceInfo(recorder, playerID);

    return true;
}

// ==================================================
// 加载圣剑信息
// ==================================================
bool PlayerLoadJob::LoadSwordInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `sword_info`,`sword_version` FROM `sword` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadSwordInfo \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseSwordInfo(recorder, playerID);

    return true;

}
// ==================================================
//加载僵尸生存信息
// ==================================================
bool PlayerLoadJob::LoadZombieInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `zombie_info` FROM zombie WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadZombieInfo \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseZombieInfo(recorder, playerID);

    return true;
}
// ==================================================
//加载礼品卡信息
// ==================================================
bool PlayerLoadJob::LoadGiftCodeInfo(uint64 playerID)
{  
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `gift_code_state` FROM gift_code_info WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadGiftCodeInfo \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseGiftCodeInfo(recorder, playerID);

    return true;
}
// ==================================================
//加载公会科技信息
// ==================================================
bool PlayerLoadJob::LoadGuildScienceInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `science_info` FROM guild_science WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadGuildScienceInfo \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseGuildScienceInfo(recorder, playerID);

    return true;
}

// ==================================================
// 加载游戏收藏信息
// ==================================================
bool PlayerLoadJob::LoadShorcutData( uint64 playerID )
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `shorcut_info` FROM `shorcut` WHERE `player_id`='%llu'", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadShorcutData \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseShorcutInfo(recorder, playerID);

    return true;
}



// ==================================================
// 加载公会信息
// ==================================================
bool PlayerLoadJob::LoadGuildInfoForMgr(uint64 guildID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query(
        "SELECT `guild_id`, `guild_name`, `level`, `exp`, `additions`, `flag`, `battle_power`, "
        "`bulletin`, `guild_boss_level`, `guild_boss_exp`, "
        "`guild_boss_star_time`, `guild_boss_create_time`, `guild_boss_dead_time` "
        "FROM `guild_info` WHERE `guild_id`=%llu", guildID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadGuildInfoForMgr \n");
        return false;
    }

    GUILD_INFO guildInfo;
    PlayerLoadParser::Instance()->ParseGuildInfoForMgr(recorder, guildInfo);

    if(!LoadGuildMemberForMgr(guildID, guildInfo))                          //加载公会成员
        return false;

    return true;
}

// ==================================================
// 加载公会成员信息
// ==================================================
bool PlayerLoadJob::LoadGuildMemberForMgr(uint64 guildID, GUILD_INFO& guildInfo)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query( 
        "SELECT T1.player_id, `player_name`, `sex`, `career`, `level`, `battle_power`, UNIX_TIMESTAMP(`logout_time`), `vip_info`, `growth_point`, T2.office_id, T2.contribute_value "
        "FROM player T1 INNER JOIN guild_member T2 ON T1.player_id=T2.player_id "
        "WHERE T2.guild_id=%llu ", guildID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadGuildMemberForMgr \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseGuildMemberForMgr(recorder, guildInfo);

    GuildManager::Instance()->LoadGuildInfo(&guildInfo);

    return true;
}

// 跨服战斗信息
bool PlayerLoadJob::LoadMultiserverBattleData(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `battle_info` FROM multiserver_battle_info WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadMultiserverBattleData \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseMultiserverBattleInfo(recorder, playerID);

    return true;
}

// 宠物列阵信息
bool PlayerLoadJob::LoadPetFormation( uint64 playerID )
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `pet_formation_info` FROM `pet_formation` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadPetFormation \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParsePetFormation(recorder, playerID);

    return true;
}

// 投资计划信息
bool PlayerLoadJob::LoadInvestmentPlan( uint64 playerID )
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `investment_plan_info` FROM `investment_plan` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadPetFormation \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseInvestmentPlan(recorder, playerID);

    return true;
}

bool PlayerLoadJob::LoadDonate( uint64 playerID )
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `donate_info` FROM `donate` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadDonate \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseDonate(recorder, playerID);

    return true;
}

bool PlayerLoadJob::LoadEgg( uint64 playerID )
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `egg_info` FROM `activity` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound LoadDonate \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseEgg(recorder, playerID);

    return true;
}

bool PlayerLoadJob::LoadBuyCheap(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `buy_cheap_info` FROM `buy_cheap` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("no found buy_cheap \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseBuyCheap(recorder, playerID);

    return true;
}

bool PlayerLoadJob::LoadPlayerStateInfo( uint64 playerID )
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `state_info` FROM `player_state_info` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("no found player_state_info \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParsePlayerStateInfo(recorder, playerID);

    return true;
}

bool PlayerLoadJob::LoadHeadPortraitInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `head_portrait_open` FROM `head_portrait` WHERE `player_id`=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("no found head_portrait \n");
        return false;
    }

    PlayerLoadParser::Instance()->ParseHeadPortraitInfo(recorder, playerID);

    return true;
}
