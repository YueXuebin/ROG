#include "centerserver_pch.h"
#include "player_save_job.h"
#include "dbdriver.h"
#include "player.h"
#include "guild_manager.h"
#include "json_helper.h"
#include "config_loader.h"
#include "global_db_operation.h"

IMPLEMENT_SINGLETON(PlayerSaveJob)

char PlayerSaveJob::m_vip_buff[MAX_SQLTXT_LEN] = {0};
char PlayerSaveJob::m_open_system_buff[MAX_SQLTXT_LEN] = {0};
char PlayerSaveJob::m_guild_welfare_buff[MAX_SQLTXT_LEN] = {0};
char PlayerSaveJob::m_city_fight_buff[MAX_SQLTXT_LEN] = {0};
char PlayerSaveJob::m_fcm_buff[MAX_SQLTXT_LEN] = {0};
char PlayerSaveJob::m_vigor_buff[MAX_SQLTXT_LEN] = {0};

PlayerSaveJob::~PlayerSaveJob(void)
{
}
// ==================================================
// 保持角色数据到DB
// ==================================================
bool PlayerSaveJob::SavePlayerDataToDB(Player* player)
{
    if(!player)
    {
        CnWarn("SavePlayerDataToDB player is null\n");
        return false;
    }


    // 存储过程开始
    m_dbExecutor->StartTransaction();

    // 保存角色信息
    if(!SavePlayerInfo(player))
    {
        goto _ERROR;
    }

    // 保存角色的装备信息
    if(!SaveEquipData(player))
    {
        goto _ERROR;
    }

    // 保存角色的药剂信息
    if(!SavePotionData(player))
    {
        goto _ERROR;
    }

    // 保存角色的物品信息
    if(!SaveItemData(player))
    {
        goto _ERROR;
    }

    // 保存角色的仓库信息
    if(!SaveStoreData(player))
    {
        goto _ERROR;
    }
    

    // 保存角色的副本信息
    if(!SavePlayerDungeonInfo(player))
    {
        goto _ERROR;
    }

    //// 保存星宫图
    //if(!SaveHoroscope(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存普通活动信息
    //if (!SaveDailylActivityInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存在线活动信息
    if(!SaveOnlineActivityInfo(player))
    {
        goto _ERROR;
    }

    // 保存技能
    if(!SaveSkillData(player))
    {
        goto _ERROR;
    }

	//// 保存天赋
	if(!SavePassiveSkillInfo(player))
	{
		goto _ERROR;
	}

    //// 保存装备信息
    //if(!SaveEquipInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存装备信息
    //if(!SaveFashionInfo(player))
    //{
    //    goto _ERROR;
    //}

    //保存宠物信息
    if(!SavePetData(player))
    {
        goto _ERROR;
    }

    ////保护骑宠信息
    //if (!SaveRideInfo(player))
    //{
    //    goto _ERROR;
    //}

    // 保存抽奖包信息
    if(!SaveRandomPackageInfo(player))
    {
        goto _ERROR;
    }

    //// 保存隐藏物品
    //if(!SaveHideItemInfo(player))
    //{
    //    goto _ERROR;
    //}

    // 保存任务信息
    if(!SaveTaskInfo(player))
    {
        goto _ERROR;
    }

    // 保持引导信息
    if(!SaveGuideAndOpenSysInfo(player))
    {
        goto _ERROR;
    }

    // 保存离线角色信息
    if(!SaveOfflinePlayerInfo(player))
    {
        goto _ERROR;
    }

    //// 保存善恶信息
    //if(!SaveEnemyInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存排行战斗信息
    //if(!SaveRankBattleInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 更新离线角色排行榜信息
    //if(!SaveOfflinePlayerRankingInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存奖励池
    //if(!SaveRewardPool(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存通天塔信息
    //if (!SaveBabelInfo(player))
    //{
    //    goto _ERROR;
    //}

    ////保存充值积分抽奖信息
    //if (!SavePayPointLotteryInfo(player))
    //{
    //    goto _ERROR;
    //}

    ////保存VIP独享副本信息
    //if (!SaveVIPOnlyDungeonInfo(player))
    //{
    //    goto _ERROR;
    //}

    ////保存圣剑信息
    //if (!SaveSwordInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存翅膀信息
    if(!SaveWingInfo(player))
    {
        goto _ERROR;
    }

    //if(!SaveAshuraInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存商城信息
    if(!SaveMallInfo(player))
    {
        goto _ERROR;
    }

    ////保存称号信息
    //if(!SaveTitleInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存日常投资信息
    //if (!SaveDayInvestmentInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存等级投资信息
    //if (!SaveLevelInvestmentInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存等级活动信息
    //if (!SaveLevelActivityInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存设置信息
    //if (!SaveSettingInfo(player))
    //{
    //    goto _ERROR;
    //}

    //if(!SaveDanceInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存活跃度信息
    //if(!SaveLivenessInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存守卫地宫信息
    //if(!SaveDungeonTimesInfo(player))
    //{
    //    goto _ERROR;
    //}
    ////保存限购
    //if (!SaveLimitBuyInfo(player))
    //{
    //    goto _ERROR;
    //}

    ////保存单人答题
    ////if (!SaveSingleAnswerInfo(player))
    ////{
    ////    goto _ERROR;
    ////}

    //// 保存资源挽回
    //if(!SaveGainbackResourceInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存勇敢的心信息
    //if (!SaveBhInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存僵尸生存信息
    //if (!SaveZombieInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存礼品卡信息
    //if (!SaveGiftCodeInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存公会信息
    //if (!SaveGuildScienceInfo(player))
    //{
    //    goto _ERROR;
    //}


    //// 保存跨服战斗信息
    //if(!SaveMultiserverBattleInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存游戏收藏信息
    //if(!SaveShorcutInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存宠物阵型信息
    //if(!SavePetFormation(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存宠物阵型信息
    //if(!SaveInvestmentPlan(player))
    //{
    //    goto _ERROR;
    //}

    ////保存捐献
    //if (!SaveDonate(player))
    //{
    //    goto _ERROR;
    //}

    ////保存砸金蛋
    //if (!SaveEgg(player))
    //{
    //    goto _ERROR;
    //}

    ////保存角色状态
    //if (!SavePlayerStateInfo(player))
    //{
    //    goto _ERROR;
    //}

    //// 保存个性头像
    if(!SaveHeadPortraitInfo(player))
    {
        goto _ERROR;
    }


    // 提交
    if (!m_dbExecutor->Commit())
    {
        goto _ERROR;
    }

    // 存储过程结束
    m_dbExecutor->EndTransaction();

    return true;            // 保存到数据库成功
_ERROR:
    // 存储过程结束
    m_dbExecutor->Rollback();
    m_dbExecutor->EndTransaction(); 

    CnWarn("SavePlayerDataToDB rooback player=%s\n", player->GetName().c_str());

    return false;           // 保存到数据库失败
}

// ==================================================
// 保存角色属性
// ==================================================
bool PlayerSaveJob::SavePlayerInfo(Player* player)
{
    DBRecorder recorder;
    Json::FastWriter writer;
    Json::Value json_val;
    Json::Value item;

    std::string guild_welfare_str;

    PROPERTY_INFO& prop_info = player->GetPropInfo();

    // 写 VIP 信息Buff
    //std::string vip_str;
    //write_VIP_INFO_BUFF(prop_info.vip_info, vip_str);
    //m_dbExecutor->EscapeString(m_vip_buff, vip_str.c_str(), vip_str.length());


    // 写 开启系统 信息Buff
    //OPEN_SYSTEM_INFO& open_system_info = player->GetOpenSystemInfo();
    //std::string open_system_str;
    //write_OPEN_SYSTEM_INFO(open_system_info, open_system_str);
    //m_dbExecutor->EscapeString(m_open_system_buff, open_system_str.c_str(), open_system_str.length());

    // 公会福利信息
    //GUILD_WELFARE_INFO& guild_welfare_info = player->GetGuildWelfareInfo();
    //write_GUILD_WELFARE_INFO(guild_welfare_info, guild_welfare_str);
    //m_dbExecutor->EscapeString(m_guild_welfare_buff, guild_welfare_str.c_str(), guild_welfare_str.length());

    // 城市争夺战信息
    //CITY_FIGHT_PLAYER_INFO& city_fight_info = player->GetCityFightInfo();
    //std::string info_txt;
    //write_CITY_FIGHT_INFO(city_fight_info, info_txt);
    //m_dbExecutor->EscapeString(m_city_fight_buff, info_txt.c_str(), info_txt.length());

    // 防沉迷信息
    //FCM_INFO& fcm_info = player->GetFcmInfo();
    //std::string fcm_str;
    //write_FCM_INFO(fcm_info, fcm_str);
    //m_dbExecutor->EscapeString(m_fcm_buff, fcm_str.c_str(), fcm_str.length());

    // 写 体力信息 buff
    //std::string vigor_str;
    //write_VIGOR_INFO(prop_info.vigor_info, vigor_str);
    //m_dbExecutor->EscapeString(m_vigor_buff, vigor_str.c_str(), vigor_str.length());

    //char offline_vigor_txt[2048] = {0};
    //std::string offline_vigor_str; //离线玩家
    //write_OFFLINE_VIGOR_INFO(prop_info.off_line_vigor_info,offline_vigor_str);
    //m_dbExecutor->EscapeString(offline_vigor_txt, offline_vigor_str.c_str(), offline_vigor_str.length());

    recorder = m_dbExecutor->Query("UPDATE `player` SET "
        "`level`=%d, `exp`=%d, `career`=%d, "
        "`money_point`=%d, `game_point`=%d, "
        "`vigor`=%d, `vigor_reftime`=%d, "
        "`unionmap_id`=%d, `x`=%d, `y`=%d, "
        "`logout_time`= from_unixtime(%d), `daily_reftime`= from_unixtime(%d) "
        " WHERE `player_id` = %llu", 
        player->GetLevel(), player->GetExp(), player->GetCareer(),
        player->GetMoneyPoint(),player->GetGamePoint(),
        player->GetVigorValue(),player->GetVigorReftime(),
        player->GetCityMapID(), player->GetCityX(), player->GetCityY(),
        player->GetLogoutTime(),
        player->GetDailyReftime(),
        player->GetPlayerID()
        );

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 保存角色的副本信息
// ==================================================
bool PlayerSaveJob::SavePlayerDungeonInfo(Player* player)
{
    char open_dungeon_txt[10000] = {0};         // 地图开启信息
    char plot_dungeon_txt[10000] = {0};         // 冒险地图
    char soul_dungeon_txt[10000] = {0};         // 异界地图
    char daily_dungeon_text[10000] = {0};       // 日常地图
    char dungeon_record_txt[10000] = {0};       // 地图记录

    PLAYER_DUNGEON_INFO& player_dungeon_info = player->GetPlayerDungeonInfo();

    
    // 写地图开启信息
    std::string open_dungeon_str;
    open_dungeon_str = StructToJson(player_dungeon_info.open_dungeon_info);
    m_dbExecutor->EscapeString(open_dungeon_txt, open_dungeon_str.c_str(), open_dungeon_str.length());

    // 写冒险地图信息
    std::string plot_dungeon_str; 
    plot_dungeon_str = StructToJson(player_dungeon_info.plot_dungeon_info);
    m_dbExecutor->EscapeString(plot_dungeon_txt, plot_dungeon_str.c_str(), plot_dungeon_str.length());

    // 写异界地图信息
    std::string soul_dungeon_str;
    soul_dungeon_str = StructToJson(player_dungeon_info.soul_dungeon_info);
    m_dbExecutor->EscapeString(soul_dungeon_txt, soul_dungeon_str.c_str(), soul_dungeon_str.length());

    // 写日常地图信息
    std::string daily_dungeon_str;
    daily_dungeon_str = StructToJson(player_dungeon_info.daily_dungeon_info);
    m_dbExecutor->EscapeString(daily_dungeon_text, daily_dungeon_str.c_str(), daily_dungeon_str.length());

    // 写地图记录
    std::string dungeon_record_str;
    dungeon_record_str = StructToJson(player_dungeon_info.dungeon_record);
    m_dbExecutor->EscapeString(dungeon_record_txt, dungeon_record_str.c_str(), dungeon_record_str.length());

    // 写数据库
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("INSERT INTO `player_dungeon_info` VALUES(%llu, '%s', '%s', '%s', '%s', '%s') "
        "ON DUPLICATE KEY UPDATE `open_dungeon_info`='%s', `plot_dungeon_info`='%s', `soul_dungeon_info`='%s', `daily_dungeon_info`='%s', `dungeon_record`='%s' ", 
        player->GetPlayerID(), open_dungeon_txt, plot_dungeon_txt, soul_dungeon_txt, daily_dungeon_text, dungeon_record_txt,
        open_dungeon_txt, plot_dungeon_txt, soul_dungeon_txt, daily_dungeon_text, dungeon_record_txt
        );

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}
// ==================================================
// 保存星宫图信息
// ==================================================
bool PlayerSaveJob::SaveHoroscope(Player* player)
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value horoscope_info;
    std::string horoscope_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    HOROSCOPE_INFO& info = player->GetHoroscopeInfo();
    info.levelup_time = Crown::SDMax(info.levelup_time, (uint32)1);

    horoscope_info["version"] = 1;
    horoscope_info["levelup_id"] = info.levelup_id;
    horoscope_info["levelup_time"] = info.levelup_time;
    horoscope_txt = writer.write(horoscope_info);

    m_dbExecutor->EscapeString(sqlTxt, horoscope_txt.c_str(), horoscope_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `horoscope` VALUES(%llu, %d, '%s', FROM_UNIXTIME(%u)) "
        "ON DUPLICATE KEY UPDATE `horoscope_level`=%d, `horoscope_info`='%s', `levelup_time`=FROM_UNIXTIME(%u)", 
        player->GetPlayerID(), info.horoscope_level, sqlTxt, info.levelup_time, info.horoscope_level, sqlTxt, info.levelup_time);
    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 保存活动信息
// ==================================================
bool PlayerSaveJob::SaveDailylActivityInfo(Player* player)
{
    DBRecorder recorder;
    Json::FastWriter writer;

    if(!player) return false;

    std::string racing_txt;
    std::string mining_txt;
    std::string treasury_txt;
    std::string goldtree_txt;
    std::string adventure_txt;
    std::string answer_txt;
    std::string single_answer_txt;

    // racing
    RACING_INFO& racing_info = player->GetRacingInfo();
    Json::Value racing_json;
    racing_json.clear();
    racing_json["refresh"]      = racing_info.refresh;
    racing_json["count"]        = racing_info.count;
    racing_txt = writer.write(racing_json);

    // mining
    MINING_INFO& mining_info = player->GetMiningInfo();
    Json::Value mining_json;
    mining_json.clear();
    mining_json["index"]        = mining_info.index;
    mining_json["state"]        = mining_info.state;
    mining_json["time"]         = mining_info.time;
    mining_json["type"]         = mining_info.type;
    mining_txt = writer.write(mining_json);

    // treasury
    TREASURY_INFO& treasury_info = player->GetTreasuryInfo();
    Json::Value treasury_json;
    treasury_json.clear();
    treasury_json["count"]      = treasury_info.count;
    treasury_json["daily_time"] = treasury_info.daily_time;
    treasury_json["enter_time"] = treasury_info.enter_time;
    treasury_json["buy_count"]  = treasury_info.buy_count;
    treasury_txt = writer.write(treasury_json);

    // goldtree
    GOLDTREE_INFO& goldtree_info = player->GetGoldtreeInfo();
    Json::Value goldtree_json;
    goldtree_json.clear();
    goldtree_json["count"]      = goldtree_info.count;
    goldtree_json["daily_time"] = goldtree_info.daily_time;
    goldtree_json["enter_time"] = goldtree_info.enter_time;
    goldtree_json["buy_count"]  = goldtree_info.buy_count;
    goldtree_txt = writer.write(goldtree_json);

    // adventure
    ADVENTURE_INFO& adventure_info = player->GetAdventureInfo();
    Json::Value adventure_json;
    adventure_json.clear();
    adventure_json["map_id"]          = adventure_info.map_id;
    adventure_json["adventure_times"] = adventure_info.adventure_times;
    adventure_json["refresh_time"]    = adventure_info.refresh_time;
    adventure_json["end_time"]        = adventure_info.end_time;
    adventure_txt = writer.write(adventure_json);

    // answer
    ANSWER_INFO& answer_info = player->GetAnswerInfo();
    Json::Value answer_json;
    answer_json.clear();
    answer_json["answer_version"] = answer_info.answer_version;
    answer_json["reward_version"] = answer_info.reward_version;
    answer_json["question_id"] = answer_info.question_id;
    answer_json["result_id"] = answer_info.result_id;
    answer_json["score"] = answer_info.score;
    answer_json["continuation"] = answer_info.continuation;
    answer_json["use_double_flag"] = answer_info.use_double_flag;
    answer_json["buy_double_num"] = answer_info.buy_double_num;
    answer_json["sys_double_num"] = answer_info.sys_double_num;
    answer_txt = writer.write(answer_json);
    // TOOD:

    SINGLE_ANSWER_INFO& single_answerinfo = player->GetSingleAnswerInfo();
    Json::Value single_answer_json;
    single_answer_json.clear();
    single_answer_json["cd_start_time"] = single_answerinfo.cd_start_time;
    single_answer_json["into_answer"] = single_answerinfo.into_answer;
    single_answer_json["change_num"] = single_answerinfo.change_num;
    single_answer_json["num"] = single_answerinfo.num;
    single_answer_txt = writer.write(single_answer_json);


    recorder = m_dbExecutor->Query("UPDATE `daily_activity` SET `racing_info`='%s', `mining_info`='%s', "
        "`treasury_info`='%s', `goldtree_info`='%s', `adventure_info`='%s', `answer_info`='%s' ,`single_answer_info`='%s'  WHERE `player_id`=%llu", 
        racing_txt.c_str(), mining_txt.c_str(), treasury_txt.c_str(), goldtree_txt.c_str(), adventure_txt.c_str(), answer_txt.c_str(), single_answer_txt.c_str(),player->GetPlayerID());
    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 保存上线活动信息
// ==================================================
bool PlayerSaveJob::SaveOnlineActivityInfo(Player* player)
{
    DBRecorder recorder;
    Json::FastWriter writer;
    std::string sign_txt;
    std::string online_reward_txt;

    if(!player) return false;

    // sign
    SIGN_INFO& sign_info = player->GetSignInfo();
    Json::Value sign_json;
    sign_json.clear();
    sign_json["sign_gain_sheet_idx"] = sign_info.sign_gain_sheet_idx;
    sign_json["last_sign_time"] = sign_info.last_sign_time;
    sign_json["days"] = sign_info.days;

    sign_txt = writer.write(sign_json);

    //// online_reward
    //ONLINE_REWARD_INFO& online_reward_info = player->GetOnlineRewardInfo();
    //Json::Value online_reward_json;
    //online_reward_json.clear();
    //Json::Value online_growreward_json;
    //online_growreward_json.clear();

    //online_reward_json["online_reward_time"]    = online_reward_info.online_reward_time;
    //online_reward_json["online_reward_day"]     = online_reward_info.online_reward_day;
    //online_reward_json["online_reward_flag"]    = online_reward_info.online_reward_flag;
    //online_reward_json["level"]                 = online_reward_info.level;
    //online_reward_json["day"]                   = online_reward_info.day;
    //online_reward_json["grow_reward_num"]       = online_reward_info.grow_reward_num;
    //online_reward_json["fixed_reward_num"]      = online_reward_info.fixed_reward_num;
    //online_reward_json["cirtnum"]               = online_reward_info.cirtnum;
    //online_reward_json["open_time"]             = online_reward_info.open_time;
    //online_reward_json["interval_time"]           = online_reward_info.interval_time;
    //online_reward_json["online_level_reward_flag"] = online_reward_info.online_level_reward_flag;
    //std::vector<REWARD_ITEM>::iterator itervec;
    //for(itervec = online_reward_info.grow_list.begin(); itervec!=  online_reward_info.grow_list.end(); itervec++)
    //{
    //    online_growreward_json.clear();
    //    write_REWARD_ITEM(*itervec,online_growreward_json);
    //    online_reward_json["growlist"].append(online_growreward_json);
    //}
    //online_reward_txt = writer.write(online_reward_json);

    //recorder = m_dbExecutor->Query("UPDATE `online_activity` SET `sign_info`='%s', `online_reward_info`='%s' WHERE `player_id`=%llu", 
    //    sign_txt.c_str(), online_reward_txt.c_str(), player->GetPlayerID());

	recorder = m_dbExecutor->Query(
		"INSERT INTO `online_activity` VALUES(%llu, '%s') "
		"ON DUPLICATE KEY UPDATE `sign_info`='%s'", 
	 player->GetPlayerID() ,sign_txt.c_str(),sign_txt.c_str());
    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 保存技能数据
// ==================================================
bool PlayerSaveJob::SaveSkillData(Player* player)
{
    char skill_txt[MAX_SQLTXT_LEN] = {0};
    std::string skill_str("");
    DBRecorder recorder;

    SKILL_COMPONENT_INFO& skill = player->GetSkillInfo();
    skill_str = StructToJson(skill);
    m_dbExecutor->EscapeString(skill_txt, skill_str.c_str(), skill_str.length());

    recorder = m_dbExecutor->Query(
        "INSERT INTO `skill` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `skill_info`='%s';", 
        player->GetPlayerID(), skill_txt, skill_txt
        );

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 保存天赋数据
// ==================================================
bool PlayerSaveJob::SavePassiveSkillInfo(Player* player)
{
	char sqlTxt[MAX_SQLTXT_LEN] = {0};
	std::string passive_str("");
	DBRecorder recorder;

	PASSIVE_SKILL_LIST& passiveSkillList = player->GetPassiveSkillInfo();

	passive_str = StructToJson(passiveSkillList);
	m_dbExecutor->EscapeString(sqlTxt, passive_str.c_str(), passive_str.length());

	recorder = m_dbExecutor->Query(
		"INSERT INTO `passive_skill` VALUES(%llu, %lu, '%s') "
		"ON DUPLICATE KEY UPDATE `available_points`=%lu, `passive_skill_info`='%s'",
		player->GetPlayerID(), passiveSkillList.available_points, sqlTxt, passiveSkillList.available_points, sqlTxt);
	if(!recorder.HasExecut())
	{
		return false;
	}
	return true;
}

// ==================================================
// 保存装备数据
// ==================================================
bool PlayerSaveJob::SaveEquipData(Player* player)
{
    char equip_txt[MAX_SQLTXT_LEN] = {0};
    std::string equip_str("");
    DBRecorder recorder;

    EQUIP_INFO& equip = player->GetEquipInfo();

    equip_str = StructToJson(equip);
    m_dbExecutor->EscapeString(equip_txt, equip_str.c_str(), equip_str.length());

    recorder = m_dbExecutor->Query(
        "INSERT INTO `equip` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `equip_info`='%s';", 
        player->GetPlayerID(), equip_txt, equip_txt
        );

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 保存药剂数据
// ==================================================
bool PlayerSaveJob::SavePotionData(Player* player)
{
    char potion_txt[MAX_SQLTXT_LEN] = {0};
    std::string potion_str("");
    DBRecorder recorder;

    POTION_INFO& potion = player->GetPotionInfo();

    potion_str = StructToJson(potion);
    m_dbExecutor->EscapeString(potion_txt, potion_str.c_str(), potion_str.length());

    recorder = m_dbExecutor->Query(
        "INSERT INTO `potion` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `potion_info`='%s';", 
        player->GetPlayerID(), potion_txt, potion_txt
        );

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 保存时装数据
// ==================================================
bool PlayerSaveJob::SaveFashionInfo( Player* player )
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value fashion_info;
    Json::Value fashion;
    Json::Value val;
    std::string equip_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    fashion_info["version"] = 1;
    FASHION_BAG& fashion_bag = player->GetFashionInfo();

    fashion_info["fashion_id"] = fashion_bag.fashion_id;
    fashion_info["fashion_skill_id"] = fashion_bag.fashion_skill_id;
    fashion_info["last_finish_time"] = fashion_bag.last_finish_time;

    for(std::vector<FASHION_INFO>::iterator it = fashion_bag.fashion_list.begin(); it != fashion_bag.fashion_list.end(); ++it)
    {
        fashion.clear();
        fashion["fashion_id"] = it->fashion_id;
        fashion["fashion_level"] = it->fashion_level;
        fashion_info["fashion"].append(fashion);
    }

    for(std::vector<uint32>::const_iterator flagIt = fashion_bag.random_list.begin(); 
        flagIt != fashion_bag.random_list.end(); ++flagIt)
    {
        fashion_info["random_list"].append(*flagIt);
    }

    for (std::vector<PACKAGE_COUNT_INFO>::iterator counti = fashion_bag.package_count.begin(); counti != fashion_bag.package_count.end(); ++counti)
    {
        fashion.clear();
        fashion["package_id"] = counti->package_id;
        fashion["package_count"] = counti->package_count;
        fashion_info["package_count_info"].append(fashion);
    }

    for (std::vector<PACKAGE_TIME_INFO>::iterator timei = fashion_bag.package_time.begin(); timei != fashion_bag.package_time.end(); ++timei)
    {
        fashion.clear();
        fashion["package_id"] = timei->package_id;
        fashion["item_id"] = timei->item_id;
        fashion["item_time"] = timei->item_time;
        fashion_info["package_time_info"].append(fashion);
    }
    equip_txt = writer.write(fashion_info);

    m_dbExecutor->EscapeString(sqlTxt, equip_txt.c_str(), equip_txt.length());
    recorder = m_dbExecutor->Query("UPDATE `equip` SET `fashion_info`='%s',`fashion_battle_power`='%d' WHERE `player_id`=%llu", sqlTxt, fashion_bag.fashion_battle_power, player->GetPlayerID());
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool PlayerSaveJob::SavePetData(Player* player)
{
    DBRecorder recorder;

    std::string soldier_str("");
    char soldier_sql[MAX_SQLTXT_LEN] = {0};

    PET_INFO& pet_info = player->GetPetInfo();

    soldier_str = StructToJson(pet_info.soldier_info);
    m_dbExecutor->EscapeString(soldier_sql, soldier_str.c_str(), soldier_str.length());

    recorder = m_dbExecutor->Query("INSERT INTO `pet` VALUES(%llu, '%s') ON DUPLICATE KEY UPDATE "
        " `mercenary_info`='%s';",
        player->GetPlayerID(), soldier_sql,
        soldier_sql);

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}


bool PlayerSaveJob::SaveRideInfo(Player* player)
{
    DBRecorder recorder;
    std::string db_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    RIDE_INFO& ride_info = player->GetRideInfo();

    db_txt = StructToJson(ride_info);

    m_dbExecutor->EscapeString(sqlTxt, db_txt.c_str(), db_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `ride` VALUES(%llu, %d, '%s') "
        "ON DUPLICATE KEY UPDATE `ride_battle_power`=%d, `ride_info`='%s'", player->GetPlayerID(), ride_info.ride_battle_power, sqlTxt, ride_info.ride_battle_power, sqlTxt);
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

// ==================================================
// 保存物品信息
// ==================================================
bool PlayerSaveJob::SaveItemData(Player* player)
{
    char item_txt[MAX_SQLTXT_LEN] = {0};
    std::string item_str("");
    DBRecorder recorder;

    ITEM_INFO& item = player->GetItemInfo();

    item_str = StructToJson(item);
    m_dbExecutor->EscapeString(item_txt, item_str.c_str(), item_str.length());

    recorder = m_dbExecutor->Query(
        "INSERT INTO `item` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `item_info`='%s';", 
         player->GetPlayerID(), item_txt, item_txt
        );

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 保存仓库信息
// ==================================================
bool PlayerSaveJob::SaveStoreData(Player* player)
{
    char item_txt[MAX_SQLTXT_LEN] = {0};
    std::string item_str("");
    DBRecorder recorder;

    ITEM_INFO& item = player->GetStoreInfo();

    item_str = StructToJson(item);
    m_dbExecutor->EscapeString(item_txt, item_str.c_str(), item_str.length());

    recorder = m_dbExecutor->Query(
        "INSERT INTO `store` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `item_info`='%s';", 
        player->GetPlayerID(), item_txt, item_txt
        );

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 保存抽奖信息
// ==================================================
bool PlayerSaveJob::SaveRandomPackageInfo(Player* player)
{
    DBRecorder recorder;

    std::string item_txt("");
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    item_txt = StructToJson(player->GetRandomPackageInfo());

    m_dbExecutor->EscapeString(sqlTxt, item_txt.c_str(), item_txt.length());

    recorder = m_dbExecutor->Query("INSERT INTO `player_package` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `package_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);
    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

bool PlayerSaveJob::SaveHideItemInfo(Player* player)
{
    //DBRecorder recorder;

    //Json::FastWriter writer;
    //Json::Value js_hide_item_info;
    //Json::Value js_item;
    //std::string item_txt;
    //char sqlTxt[MAX_SQLTXT_LEN] = {0};

    //js_hide_item_info["version"] = 1;

    //HIDE_BAG_INFO& hide_bag_info = player->GetHideBagInfo();
    //for(uint32 i=0; i<hide_bag_info.list.size(); i++)
    //{
    //    const ITEM_INFO& item_info = hide_bag_info.list[i];
    //    js_item.clear();
    //    write_ITEM_INFO(item_info, js_item);
    //    js_hide_item_info["item"].append(js_item);
    //}

    //item_txt = writer.write(js_hide_item_info);

    //m_dbExecutor->EscapeString(sqlTxt, item_txt.c_str(), item_txt.length());
    //recorder = m_dbExecutor->Query("INSERT INTO `item` VALUES(%llu, '{}', '%s', '{}') "
    //    "ON DUPLICATE KEY UPDATE `hide_item_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);
    //if(!recorder.HasExecut())
    //{
    //    return false;
    //}
    return true;
}

// ==================================================
// 保存任务数据
// ==================================================
bool PlayerSaveJob::SaveTaskInfo(Player* player)
{
    DBRecorder recorder;
    Json::FastWriter writer;
    Json::Value task_game_json;
    Json::Value task_activity_json;
    Json::Value task_guild_json;
    std::string task_game_str;
    std::string task_activity_str;
    std::string task_guild_str;
    static char task_game_txt[MAX_SQLTXT_SAVE_LEN] = {0};
    static char task_activity_txt[MAX_SQLTXT_SAVE_LEN] = {0};
    static char task_guild_txt[MAX_SQLTXT_SAVE_LEN] = {0};

    TASK_INFO& task_info = player->GetTaskInfo();
    TASK_GAME_INFO& task_game_info = task_info.task_game_info;
    TASK_FESTIVAL_INFO& task_festival_info = task_info.task_festival_info;
    TASK_GUILD_INFO& guild_info = task_game_info.guild_task;

    write_TaskGameInfo(task_game_info, task_game_json);
    write_TaskFestivalInfo(task_festival_info, task_activity_json);
    write_TaskGuildlInfo(guild_info, task_guild_json);

    task_game_str = writer.write(task_game_json);
    task_activity_str = writer.write(task_activity_json);
    task_guild_str = writer.write(task_guild_json);

    m_dbExecutor->EscapeString(task_game_txt, task_game_str.c_str(), task_game_str.length());
    m_dbExecutor->EscapeString(task_activity_txt, task_activity_str.c_str(), task_activity_str.length());
    m_dbExecutor->EscapeString(task_guild_txt, task_guild_str.c_str(), task_guild_str.length());

    if(task_info.db_flag & DB_OPT_INSERT)
    {
        recorder = m_dbExecutor->Query("INSERT INTO `task` VALUES(%llu, '%s', '{}', '{}') ", player->GetPlayerID(), task_game_txt);
        if(!recorder.HasExecut())
        {
            return false;
        }

        task_info.db_flag &= ~DB_OPT_INSERT;

        recorder = m_dbExecutor->Query("UPDATE `task` SET `festival_info`='%s' WHERE player_id=%llu", task_activity_txt, player->GetPlayerID());
        if(!recorder.HasExecut())
        {
            return false;
        }

        recorder = m_dbExecutor->Query("UPDATE `task` SET `guild_info`='%s' WHERE player_id=%llu", task_guild_txt, player->GetPlayerID());
        if(!recorder.HasExecut())
        {
            return false;
        }
    }
    else
    {
        recorder = m_dbExecutor->Query("UPDATE `task` SET `game_info`='%s' WHERE player_id=%llu", task_game_txt, player->GetPlayerID());
        if(!recorder.HasExecut())
        {
            return false;
        }

        recorder = m_dbExecutor->Query("UPDATE `task` SET `festival_info`='%s' WHERE player_id=%llu", task_activity_txt, player->GetPlayerID());
        if(!recorder.HasExecut())
        {
            return false;
        }

        recorder = m_dbExecutor->Query("UPDATE `task` SET `guild_info`='%s' WHERE player_id=%llu", task_guild_txt, player->GetPlayerID());
        if(!recorder.HasExecut())
        {
            return false;
        }
    }

    return true;
}

bool PlayerSaveJob::SaveGuideAndOpenSysInfo(Player* player)
{
    DBRecorder recorder;

    // 引导
    GUIDE_INFO& guide_info = player->GetGuideInfo();
    
    std::string guide_info_str = StructToJson(guide_info);

    char guide_info_txt[MAX_SQLTXT_LEN] = {0};
    m_dbExecutor->EscapeString(guide_info_txt, guide_info_str.c_str(), guide_info_str.length());

    // 功能开启
    OPEN_SYSTEM_INFO& open_system_info = player->GetOpenSystemInfo();

    std::string open_system_info_str = StructToJson(open_system_info);

    char opensystem_info_txt[MAX_SQLTXT_LEN] = {0};
    m_dbExecutor->EscapeString(opensystem_info_txt, open_system_info_str.c_str(), open_system_info_str.length());

    // 写数据库
    recorder = m_dbExecutor->Query(
        "INSERT INTO `guide` VALUES(%llu, '%s', '%s') "
        "ON DUPLICATE KEY UPDATE `guide_info`='%s', `open_system_info`='%s' ;", 
        player->GetPlayerID(), guide_info_txt, opensystem_info_txt, guide_info_txt, opensystem_info_txt
        );

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 保存善恶信息
// ==================================================
bool PlayerSaveJob::SaveEnemyInfo(Player* player)
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value json_enemy;
    std::string enemy_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    ENEMY_INFO& enemy_info = player->GetEnemyInfo();
    write_ENEMY_INFO(enemy_info, json_enemy);
    enemy_txt = writer.write(json_enemy);

    m_dbExecutor->EscapeString(sqlTxt, enemy_txt.c_str(), enemy_txt.length());

    if(enemy_info.db_flag & DB_OPT_INSERT)
    {
        enemy_info.db_flag &= ~DB_OPT_INSERT;

        recorder = m_dbExecutor->Query("INSERT INTO `enemy` VALUES(%llu, '%s')", player->GetPlayerID(), sqlTxt);
    }
    else
    {
        recorder = m_dbExecutor->Query("UPDATE `enemy` SET `enemy_info`='%s' WHERE player_id=%llu", sqlTxt, player->GetPlayerID());
    }

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 保存离线战斗数据
// ==================================================
bool PlayerSaveJob::SaveRankBattleInfo(Player* player)
{
    DBRecorder recorder;
    Json::FastWriter writer;
    Json::Value battle_player;

    // 勇者争霸
    Json::Value player_door_json;
    std::string player_door_txt;
    char player_door_sqlTxt[MAX_SQLTXT_LEN] = {0};

    PLAYER_DOOR_INFO& player_door_info = player->GetPlayerDoorInfo();
    player_door_json["version"] = 1;
    player_door_json["buy_challenge_count"] = player_door_info.buy_challenge_count;
    player_door_json["challenge_count"] = player_door_info.challenge_count;
    player_door_json["winning_steak"] = player_door_info.winning_steak;
    player_door_json["cd_timestamp"] = player_door_info.cd_timestamp;
    player_door_json["rewarded_today"] = player_door_info.rewarded_info.rewarded_today;
    player_door_json["is_init"] = player_door_info.is_init;
    player_door_txt = writer.write(player_door_json);

    m_dbExecutor->EscapeString(player_door_sqlTxt, player_door_txt.c_str(), player_door_txt.length());

    // 怪物之门
    Json::Value monster_door_json;
    std::string monster_door_txt;
    char monster_door_sqlTxt[MAX_SQLTXT_LEN] = {0};

    MONSTER_DOOR_INFO& monster_door_info = player->GetMonsterDoorInfo();

    monster_door_json["version"] = 1;
    monster_door_json["battle_boss_flag"] = monster_door_info.battle_boss_flag;
    monster_door_json["battle_count"] = monster_door_info.battle_count;
    monster_door_json["combat_count"] = monster_door_info.combat_count;
    monster_door_json["combat_flag"] = monster_door_info.combat_flag;
    monster_door_json["reset_count"] = monster_door_info.reset_count;
    monster_door_json["combat_history_flag"] = monster_door_info.combat_history_flag;
    monster_door_json["auto_cd_timestamp"] = monster_door_info.auto_cd_timestamp;
    monster_door_json["auto_index"] = monster_door_info.auto_index;
    monster_door_txt = writer.write(monster_door_json);

    m_dbExecutor->EscapeString(monster_door_sqlTxt, monster_door_txt.c_str(), monster_door_txt.length());

    recorder = m_dbExecutor->Query("INSERT INTO `offline_battle` VALUES(%llu, '%s', '%s', '%s') "
        "ON DUPLICATE KEY UPDATE `player_door`='%s', `monster_door`='%s', `battle_info`='%s'", 
        player->GetPlayerID(), player_door_sqlTxt, monster_door_sqlTxt, player_door_sqlTxt,
        player_door_sqlTxt, monster_door_sqlTxt, player_door_sqlTxt);
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

// ==================================================
// 保存角色离线数据
// ==================================================
bool PlayerSaveJob::SaveOfflinePlayerInfo(Player* player)
{
    DBRecorder recorder;
    Json::FastWriter writer;
    Json::Value equip_json;
    Json::Value equip;
    Json::Value target;
    Json::Value battle_power_json;
    std::string battle_power_txt;
    std::string equip_txt;
    uint32 task_num = 0;
    uint32 target_num = 0;
    char name[PLAYER_NAME_LEN] = {0};
    char equipTxt[MAX_SQLTXT_LEN] = {0};
    char guildNameTxt[GUILD_NAME_LEN] = {0};
    char battlePowerTxt[MAX_SQLTXT_LEN] = {0};

    OFFLINE_PLAYER_INFO& info = player->GetOfflinePlayerInfo();

    // TODO
    if(!info.player_id_l && !info.player_id_h)
    {
        return true;
    }

    equip_json["version"] = 1;

    for(std::vector<OFFLINE_EQUIP_INFO>::iterator equipIt = info.equip_list.begin(); 
        equipIt != info.equip_list.end(); ++equipIt)
    {
        write_OFFLINE_EQUIP_INFO(*equipIt, equip);       
        equip_json["equip"].append(equip);
    }
    equip_txt = writer.write(equip_json);

    write_BATTLE_POWER_INFO(info.battle_power_info, battle_power_json);
    battle_power_txt = writer.write(battle_power_json);

    m_dbExecutor->EscapeString(name, player->GetName().c_str(), player->GetName().length());
    m_dbExecutor->EscapeString(equipTxt, equip_txt.c_str(), equip_txt.length());
    m_dbExecutor->EscapeString(guildNameTxt, info.guild_name.c_str(), info.guild_name.length());
    m_dbExecutor->EscapeString(battlePowerTxt, battle_power_txt.c_str(), battle_power_txt.length());

    recorder = m_dbExecutor->Query(
        "INSERT INTO `offline_player` SET "
        "`player_name`='%s', "
        "`sex`=%d, `career`=%d, `vip_level`=%d, `battle_power`=%d, "
        "`level`=%d, `exp`=%d, `max_exp`=%d, `guild_name`='%s', "
        "`equip_info`='%s', `wing`=%d, `battle_power_info`='%s', "
        "`achive_count`=%d, `score`=%d, `player_id`=%llu "
        "ON DUPLICATE KEY UPDATE `player_name`='%s', `sex`=%d, `career`=%d, `vip_level`=%d, `battle_power`=%d, `level`=%d, `exp`=%d, `max_exp`=%d, `guild_name`='%s', `equip_info`='%s', `wing`=%d, `battle_power_info`='%s', `achive_count`=%d, `score`=%d",
        name,
        info.sex, info.career, info.vip, info.battle_power, 
        info.level, info.exp, info.max_exp, "0",
        "{}", info.wing, "{}", 
        info.achive_count,info.score,player->GetPlayerID(),
        name, info.sex, info.career, info.vip, info.battle_power, info.level, info.exp, info.max_exp, "0", "{}", info.wing, "{}",  info.achive_count,info.score
        );

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}


bool PlayerSaveJob::SaveOfflinePlayerRankingInfo(Player* player)
{
    DBRecorder recorder;

    //std::string goildName = GuildManager::Instance()->GetGuildName(player->GetGuildID());

    //recorder = m_dbExecutor->Query(
    //    "UPDATE `ranking_offline_player` SET `sex`=%d,`level`=%d,`career`=%d,`guild_name`='%s',`battle_power`=%d,`player_name`='%s' WHERE player_id=%llu", player->GetSex(), player->GetLevel(), player->GetCareer(), goildName.c_str(), player->GetBattlePower(), player->GetName().c_str(), player->GetPlayerID()
    //    );

    //if(!recorder.HasExecut())
    //{
    //    return false;
    //}
    return true;
}

// ==================================================
// 保存角色奖励池
// ==================================================
bool PlayerSaveJob::SaveRewardPool(Player* player)
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value js_reward_info;
    Json::Value js_reward_table;
    Json::Value js_item;
    std::string reward_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    REWARD_INFO info = player->GetRewardInfo();

    uint32 maxTableSize = info.tables.size();
    if( maxTableSize > 30 )
    {   // 最多保存30条奖励,一条奖励约(500byte)
        CnInfo("player %s reward table > 30\n", player->GetAccount().c_str());
        maxTableSize = 30;
    }

    for (uint32 i = 0; i < maxTableSize; i++)
    {
        REWARD_TABLE& table = info.tables[i];
        js_reward_table.clear();
        write_REWARD_TABLE(table, js_reward_table);
        js_reward_info["tables"].append(js_reward_table);
    }

    if(js_reward_info.isNull())
        reward_txt = "{}";
    else
        reward_txt = writer.write(js_reward_info);

    m_dbExecutor->EscapeString(sqlTxt, reward_txt.c_str(), reward_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `reward_store` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `reward_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

// ==================================================
// 保存通天塔
// ==================================================
bool PlayerSaveJob::SaveBabelInfo(Player* player)
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value babel_info;
    std::string babel_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    BABEL_INFO info = player->GetBabelInfo();

    babel_info["last_refresh_time"] = info.last_refresh_time;
    babel_info["left_reset_times"] = info.left_reset_times;
    babel_info["babel_tier"] = info.babel_tier;
    babel_info["babel_stage"] = info.babel_stage;
    babel_info["pass_tier"] = info.pass_tier;
    babel_info["pass_stage"] = info.pass_stage;

    babel_txt = writer.write(babel_info);

    m_dbExecutor->EscapeString(sqlTxt, babel_txt.c_str(), babel_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `babel` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `babel_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

//==================================================
// 保存充值积分抽奖信息
//==================================================
bool PlayerSaveJob::SavePayPointLotteryInfo(Player *player)
{
    DBRecorder recorder;

    std::string db_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    PAY_POINT_LOTTERY_INFO &info = player->GetPayPointLotteryInfo();
    info.record_time = (uint32)Crown::SDNow().GetTimeValue();

    db_txt = StructToJson(info);
    m_dbExecutor->EscapeString(sqlTxt, db_txt.c_str(), db_txt.length());

    recorder = m_dbExecutor->Query("INSERT INTO `pay_point_lottery` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `pay_point_lottery_info`='%s'",
        player->GetPlayerID(), sqlTxt, sqlTxt);

    if (!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 保存充值积分抽奖信息
// ==================================================
bool PlayerSaveJob::SaveVIPOnlyDungeonInfo(Player *player)
{
    DBRecorder recorder;

    std::string db_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    VIP_ONLY_DUNGEON_INFO &info = player->GetVIPOnlyDungeonInfo();
    info.record_time = (uint32)Crown::SDNow().GetTimeValue();

    db_txt = StructToJson(info);
    m_dbExecutor->EscapeString(sqlTxt, db_txt.c_str(), db_txt.length());

    recorder = m_dbExecutor->Query("INSERT INTO `vip_only_dungeon` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `vip_only_dungeon_info`='%s'",
        player->GetPlayerID(), sqlTxt, sqlTxt);

    if (!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 保存圣剑信息
// ==================================================
bool PlayerSaveJob::SaveSwordInfo(Player* player)
{
    DBRecorder recorder;

    std::string sword_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    SWORD_NEW_INFO& info = player->GetSwordInfo();

    sword_txt = StructToJson(info);
    int32 sword_version = 1;

    m_dbExecutor->EscapeString(sqlTxt, sword_txt.c_str(), sword_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `sword` VALUES(%llu, '%s', %d) "
        "ON DUPLICATE KEY UPDATE `sword_info`='%s', `sword_version`=%d", player->GetPlayerID(), sqlTxt, sword_version, sqlTxt, sword_version);

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}


// ==================================================
// 保存翅膀信息
// ==================================================
bool PlayerSaveJob::SaveWingInfo(Player* player)
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value wing_info;
    Json::Value list_info;
    std::string wing_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    WING_INFO& info = player->GetWingInfo();

    wing_txt = StructToJson(info);

    int32 version = 1;

    m_dbExecutor->EscapeString(sqlTxt, wing_txt.c_str(), wing_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `wing` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `wing_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}
// ==================================================
// 保存修罗场信息
// ==================================================
bool PlayerSaveJob::SaveAshuraInfo(Player* player)
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value ashura_info;
    Json::Value list_info;
    std::string ashura_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    ASHURA_INFO& info = player->GetAshuraInfo();

    ashura_info["kill_count"] = info.kill_count;
    ashura_info["dead_count"] = info.dead_count;
    ashura_info["refresh_time"] = info.refresh_time;
    ashura_info["delay_count"] = info.delay_count;

    ashura_txt = writer.write(ashura_info);

    m_dbExecutor->EscapeString(sqlTxt, ashura_txt.c_str(), ashura_txt.length());
    // recorder = m_dbExecutor->Query("INSERT INTO `ashura` VALUES(%llu, '%s', %d) "
    //"ON DUPLICATE KEY UPDATE `ashura_info`='%s',`kill_count`=`kill_count`+'%d'", player->GetPlayerID(), sqlTxt, info.all_kill_count, sqlTxt, info.all_kill_count);
    recorder = m_dbExecutor->Query("INSERT INTO `ashura` VALUES(%llu, '%s', %d) "
        "ON DUPLICATE KEY UPDATE `ashura_info`='%s',`kill_count`='%d'", player->GetPlayerID(), sqlTxt, info.all_kill_count, sqlTxt, info.all_kill_count);
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}


// ==================================================
// 保存商城信息
// ==================================================
bool PlayerSaveJob::SaveMallInfo(Player* player)
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value mall_json;
    Json::Value mall_info;
    std::string mall_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    RESTRICTION_LIST& restrictionList = player->GetMallInfo();

    for(std::vector<RESTRICTION>::iterator restrictionIt = restrictionList.restriction_list.begin(); restrictionIt != restrictionList.restriction_list.end(); ++restrictionIt)
    {
        mall_json["product_id"] = restrictionIt->product_id;
        mall_json["num"] = restrictionIt->num; 
        mall_info["restriction"].append(mall_json);
    }
    mall_txt = writer.write(mall_info);

    m_dbExecutor->EscapeString(sqlTxt, mall_txt.c_str(), mall_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `player_mall` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `mall_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

// ==================================================
// 保存称号信息
// ==================================================
bool PlayerSaveJob::SaveTitleInfo(Player* player)
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value tile_json;
    Json::Value list_info;
    std::string title_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    TITLE_LIST infoList = player->GetTitleInfo();

    std::vector<TITLE_INFO>::iterator titleInfo;
    for(titleInfo= infoList.list.begin(); titleInfo!= infoList.list.end(); titleInfo++)
    {
        tile_json["titlid"] = titleInfo->titlid;
        tile_json["starttime"] = titleInfo->starttime;
        tile_json["b_state"] = titleInfo->b_state;
        tile_json["time"] = titleInfo->time;
        tile_json["num"] = titleInfo->num;
        list_info["list"].append(tile_json);
    }

    title_txt = writer.write(list_info);

    m_dbExecutor->EscapeString(sqlTxt, title_txt.c_str(), title_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `title` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `title_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}
// ==================================================
// 保存日常投资信息
// ==================================================
bool PlayerSaveJob::SaveDayInvestmentInfo( Player* player )
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value info_json;
    Json::Value list_info;
    std::string info_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    DAY_INVESTMENT_INFO& info = player->GetDayInvestmentInfo();
    std::vector<ONE_DAY_INVESTMENT_INFO>::iterator iter;
    for(iter= info.info_list.begin(); iter!= info.info_list.end(); iter++)
    {
        info_json["day"] = iter->day;
        info_json["status"] = iter->status;
        list_info["list"].append(info_json);
    }
    list_info["invest_begin_time"] = info.invest_begin_time;
    list_info["recharge_num"] = info.recharge_num;
    info_txt = writer.write(list_info);

    m_dbExecutor->EscapeString(sqlTxt, info_txt.c_str(), info_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `day_investment` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `day_investment_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);

    if(!recorder.HasExecut())
        return false;

    return true;
}
// ==================================================
// 保存等级投资信息
// ==================================================
bool PlayerSaveJob::SaveLevelInvestmentInfo( Player* player )
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value info_json;
    Json::Value list_info;
    std::string info_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    LEVEL_INVESTMENT_INFO& info = player->GetLevelInvestmentInfo();
    std::vector<ONE_LEVEL_INVESTMENT_INFO>::iterator iter;
    for(iter= info.info_list.begin(); iter!= info.info_list.end(); iter++)
    {
        info_json["level"] = iter->level;
        info_json["status"] = iter->status;
        list_info["list"].append(info_json);
    }
    list_info["invested_money"] = info.invested_money;
    info_txt = writer.write(list_info);

    m_dbExecutor->EscapeString(sqlTxt, info_txt.c_str(), info_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `level_investment` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `level_investment_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);

    if(!recorder.HasExecut())
        return false;

    return true;
}
// ==================================================
// 保存等级活动信息
// ==================================================
bool PlayerSaveJob::SaveLevelActivityInfo( Player* player )
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value info_json;
    std::string info_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    LEVEL_ACTIVITY_INFO& info = player->GetLevelActivityInfo();
    info_json["accept_flag"] = info.accept_flag;
    info_txt = writer.write(info_json);

    m_dbExecutor->EscapeString(sqlTxt, info_txt.c_str(), info_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `level_activity` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `level_activity_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);

    if(!recorder.HasExecut())
        return false;

    return true;
}

// ==================================================
// 保存设置信息
// ==================================================
bool PlayerSaveJob::SaveSettingInfo(Player* player)
{
    DBRecorder recorder;

    Json::FastWriter writer;

    Json::Value auto_setting;
    Json::Value system_setting;
    Json::Value player_setting;
    Json::Value autobuy_setting;
    Json::Value voidfield_setting;
    std::string txt;
    char auto_fight_sqlTxt[MAX_SQLTXT_LEN] = {0};
    char system_sqlTxt[MAX_SQLTXT_LEN] = {0};
    char player_sqlTxt[MAX_SQLTXT_LEN] = {0};
    char autobuy_sqlTxt[MAX_SQLTXT_LEN] = {0};
    char voidfield_sqlTxt[MAX_SQLTXT_LEN] = {0};

    // auto_fight_setting
    AUTO_FIGHT_SETTING_INFO auto_fight_info = player->GetAutoFightSettingInfo();

    auto_setting["hp"]            = auto_fight_info.hp;
    auto_setting["hp_order"]      = auto_fight_info.hp_order;
    auto_setting["mp"]            = auto_fight_info.mp;
    auto_setting["mp_order"]      = auto_fight_info.mp_order;
    auto_setting["skills"]        = auto_fight_info.skills;
    auto_setting["auto_revival"]  = auto_fight_info.auto_revival;
    auto_setting["auto_fight"]    = auto_fight_info.auto_fight;
    auto_setting["auto_buy_hp_drug"]        = auto_fight_info.auto_buy_hp_drug;
    auto_setting["auto_buy_mp_drug"]        = auto_fight_info.auto_buy_mp_drug;
    auto_setting["auto_buy_attack_drug"]    = auto_fight_info.auto_buy_attack_drug;
    auto_setting["auto_buy_defence_drug"]   = auto_fight_info.auto_buy_defence_drug;
    auto_setting["auto_buy_speed_drug"]     = auto_fight_info.auto_buy_speed_drug;
    auto_setting["auto_use_attack_drug"]    = auto_fight_info.auto_use_attack_drug;
    auto_setting["auto_use_defence_drug"]   = auto_fight_info.auto_use_defence_drug;
    auto_setting["auto_use_speed_drug"]     = auto_fight_info.auto_use_speed_drug;

    txt = writer.write(auto_setting);

    m_dbExecutor->EscapeString(auto_fight_sqlTxt, txt.c_str(), txt.length());

    // system_setting
    SYSTEM_SETTING_INFO system_info = player->GetSystemSettingInfo();

    write_SYSTEM_SETTING_INFO(system_info, system_setting);

    txt = writer.write(system_setting);

    m_dbExecutor->EscapeString(system_sqlTxt, txt.c_str(), txt.length());

    // player_setting
    PLAYER_SETTING_INFO player_info = player->GetPlayerSettingInfo();

    player_setting["hp"] = player_info.hp;
    player_setting["mp"] = player_info.mp;

    txt = writer.write(player_setting);

    m_dbExecutor->EscapeString(player_sqlTxt, txt.c_str(), txt.length());

    // autobuy_setting
    AUTOBUY_SETTING_INFO autobuy_info = player->GetAutobuySettingInfo();
    autobuy_setting.clear();
    for(std::vector<int32>::iterator flagIt = autobuy_info.autobuyList.begin(); 
        flagIt != autobuy_info.autobuyList.end(); ++flagIt)
    {
        autobuy_setting["autobuyList"].append(*flagIt);
    }

    txt = writer.write(autobuy_setting);

    m_dbExecutor->EscapeString(autobuy_sqlTxt, txt.c_str(), txt.length());

    // voidfield_setting
    VOID_FIELD_SETTING_INFO voidfield_setting_info = player->GetVoidFieldSettingInfo();
    voidfield_setting["auto_use_revival"]   = voidfield_setting_info.auto_use_revival;
    voidfield_setting["auto_buy_revival"]   = voidfield_setting_info.auto_buy_revival;
    voidfield_setting["current_revival"]    = voidfield_setting_info.current_revival;
    voidfield_setting["reconnect_on_loss"]  = voidfield_setting_info.reconnect_on_loss;

    txt = writer.write(voidfield_setting);

    m_dbExecutor->EscapeString(voidfield_sqlTxt, txt.c_str(), txt.length());

    // write db
    recorder = m_dbExecutor->Query("INSERT INTO `setting` VALUES(%llu, '%s', '%s', '%s', '%s', '%s') "
        "ON DUPLICATE KEY UPDATE `auto_fight_setting`='%s',`system_setting`='%s',`player_setting`='%s',`autobuy_setting`='%s',`void_field_setting`='%s'", player->GetPlayerID(), auto_fight_sqlTxt, system_sqlTxt, player_sqlTxt, autobuy_sqlTxt, voidfield_sqlTxt, auto_fight_sqlTxt, system_sqlTxt, player_sqlTxt, autobuy_sqlTxt, voidfield_sqlTxt);

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 保存活跃度信息
// ==================================================
bool PlayerSaveJob::SaveLivenessInfo( Player* player )
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value info_json;
    Json::Value temp_info;
    std::string info_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    LIVENESS_INFO& info = player->GetLivenessInfo();
    for (uint32 i = 0; i < info.info_list.size(); i++)
    {
        temp_info["target"] = info.info_list[i].target;
        temp_info["cur_count"] = info.info_list[i].cur_count;
        temp_info["finish"] = info.info_list[i].finish;
        info_json["info_list"].append(temp_info);
    }

    temp_info.clear();
    for (uint32 i = 0; i < info.reward_list.size(); i++)
    {
        temp_info["flag"] = info.reward_list[i];
        info_json["reward_list"].append(temp_info);
    }

    info_txt = writer.write(info_json);
    m_dbExecutor->EscapeString(sqlTxt, info_txt.c_str(), info_txt.length());

    recorder = m_dbExecutor->Query(
        "INSERT INTO `liveness` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `liveness_info`='%s'"
        , player->GetPlayerID(), sqlTxt, sqlTxt);

    if(!recorder.HasExecut())
        return false;

    return true;
}
// ==================================================
// 保存守卫地宫信息
// ==================================================
bool PlayerSaveJob::SaveDungeonTimesInfo(Player* player)
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value info_json;
    Json::Value list_json;
    std::string list_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    DUNGEON_TIMES_LIST& dungeonTimesList = player->GetDungeonTimesInfo();

    for(std::vector<DUNGEON_TIMES_INFO>::iterator dunIt = dungeonTimesList.list.begin(); dunIt != dungeonTimesList.list.end(); ++dunIt)
    {
        info_json["enter_count"] = dunIt->enter_count;
        info_json["refresh_time"] = dunIt->refresh_time;
        info_json["dungeon_id"] = dunIt->dungeon_id;
        list_json["list"].append(info_json);
    }
    list_txt = writer.write(list_json);
    m_dbExecutor->EscapeString(sqlTxt, list_txt.c_str(), list_txt.length());

    recorder = m_dbExecutor->Query("INSERT INTO `dungeon_times` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `dungeon_times_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);

    if(!recorder.HasExecut())
        return false;

    return true;
}

// ==================================================
// 保存限购信息
// ==================================================
bool PlayerSaveJob::SaveLimitBuyInfo(Player* player)
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value limit_json;
    Json::Value limit_info;
    std::string title_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};


    LIMIT_BUY_LIST infoList = player->GetLimitBuyInfo();

    std::vector<LIMIT_BUY>::iterator mallbuylimt;
    for(mallbuylimt= infoList.mall_buy_list.begin(); mallbuylimt!= infoList.mall_buy_list.end(); mallbuylimt++)
    {
        limit_json["num"] = mallbuylimt->num;
        limit_json["product_id"] = mallbuylimt->product_id;
        limit_json["type"] = mallbuylimt->type;
        limit_info["malllist"].append(limit_json);
    }


    std::vector<LIMIT_BUY>::iterator bindmoneylimt;
    for(bindmoneylimt= infoList.bind_money_list.begin(); bindmoneylimt!= infoList.bind_money_list.end(); bindmoneylimt++)
    {
        limit_json["num"] = bindmoneylimt->num;
        limit_json["product_id"] = bindmoneylimt->product_id;
        limit_json["type"] = bindmoneylimt->type;
        limit_info["bindmoneylist"].append(limit_json);
    }


    title_txt = writer.write(limit_info);

    m_dbExecutor->EscapeString(sqlTxt, title_txt.c_str(), title_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `limitbuy` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `limit_buy_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);
    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 保存资源挽回信息
// ==================================================
bool PlayerSaveJob::SaveGainbackResourceInfo(Player* player)
{
    //DBRecorder recorder;
    //Json::FastWriter writer;
    //std::string gb_txt;

    //if(!player)
    //    return false;

    //GAINBACK_RESOURCE_INFO& gb_info = player->GetGainbackResourceInfo();
    //Json::Value gb_json;
    //gb_json.clear();
    //gb_json["last_time"]                     = gb_info.last_time;
    //Json::Value yester_json;
    //for(std::vector<YESTERDAY_COUNT>::iterator yesterdayIt = gb_info.yesterday_list.begin(); yesterdayIt != gb_info.yesterday_list.end(); ++ yesterdayIt)
    //{
    //    yester_json.clear();
    //    yester_json["id"]           = yesterdayIt->id;
    //    yester_json["count"]        = yesterdayIt->count;
    //    yester_json["state"]        = yesterdayIt->state;
    //    gb_json["yesterday_list"].append(yester_json);
    //}

    //Json::Value activityJson;
    //for(std::vector<ACTIVITY_ENTER>::iterator acIt = gb_info.activity_enter_list.begin(); acIt != gb_info.activity_enter_list.end(); ++acIt)
    //{
    //    activityJson.clear();
    //    activityJson["id"]          = acIt->id;
    //    activityJson["is_enter"]    = acIt->is_enter;
    //    gb_json["activity_enter_list"].append(activityJson);
    //}

    //gb_txt = writer.write(gb_json);

    //recorder = m_dbExecutor->Query("INSERT INTO `gainback_resource` VALUES(%llu, '%s') ON DUPLICATE KEY UPDATE `gainback_info`='%s'", 
    //    player->GetPlayerID(), gb_txt.c_str(), gb_txt.c_str());
    //if(!recorder.HasExecut())
    //{
    //    return false;
    //}

    return true;
}

// ==================================================
// 保存勇敢的心数据
// ==================================================
bool PlayerSaveJob::SaveBhInfo(Player* player)
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value bh_info;
    std::string bh_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    BH_INFO info = player->GetBhInfo();

    bh_info["today_count"] = info.today_count;
    bh_info["is_selected"] = info.is_selected;
    bh_info["cur_type"] = info.cur_type;
    bh_info["pass_type"] = info.pass_type;
    bh_info["pass_stage"] = info.pass_stage;
    bh_info["heart_point"] = info.heart_point;
    bh_info["heart_point_sum"] = info.heart_point_sum;
    // attributes
    Json::Value attributes_json;
    attributes_json.clear();
    for (std::vector<BH_ADDITION>::iterator add = info.addition_list.begin(); add != info.addition_list.end(); add++)
    {
        Json::Value attr_json;
        attr_json.clear();
        attr_json["attr"] = add->add_prop;
        attr_json["val"] = add->add_value;
        attributes_json.append(attr_json);
    }
    bh_info["attributes"] = attributes_json;

    bh_txt = writer.write(bh_info);

    m_dbExecutor->EscapeString(sqlTxt, bh_txt.c_str(), bh_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `brave_heart` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `bh_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

// ==================================================
// 保存僵尸生存信息
// ==================================================
bool PlayerSaveJob::SaveZombieInfo(Player* player)
{
    DBRecorder recorder;
    Json::FastWriter writer;
    Json::Value dungeonInfo;
    Json::Value zombieValue;

    const ZOMBIE_INFO& zombieInfo = player->GetZombieInfo();

    zombieValue["enter_count"] = zombieInfo.enter_count;
    zombieValue["cumulative_time"] = zombieInfo.cumulative_total_time;

    for (std::vector<unsigned int>::const_iterator iter = zombieInfo.finish_zombie_dungeons.begin(); 
        iter != zombieInfo.finish_zombie_dungeons.end(); ++iter)
    {
        dungeonInfo["dungeonId"] = *iter;
        zombieValue["finish_dungeon"].append(dungeonInfo);
    }

    std::string zombieStr = writer.write(zombieValue);

    recorder = m_dbExecutor->Query(
        "INSERT INTO `zombie` VALUES(%llu, '%s') ON DUPLICATE KEY UPDATE `zombie_info`='%s'",
        player->GetPlayerID(), zombieStr.c_str(), zombieStr.c_str());

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}
// ==================================================
// 保存礼品卡信息
// ==================================================
bool PlayerSaveJob::SaveGiftCodeInfo(Player* player)
{
    DBRecorder recorder;
    Json::FastWriter writer;
    Json::Value info;
    Json::Value giftCodesValue;

    const GIFT_CODE_INFO& giftCodeInfo = player->GetGiftCodeInfo();

    for (std::vector<char>::const_iterator iter = giftCodeInfo.use_info.begin(); 
        iter != giftCodeInfo.use_info.end(); ++iter)
    {
        info["giftcode_type"] = *iter;
        giftCodesValue["giftcodes"].append(info);
    }

    std::string giftCodesStr = writer.write(giftCodesValue);

    recorder = m_dbExecutor->Query(
        "INSERT INTO `gift_code_info` VALUES(%llu, '%s') ON DUPLICATE KEY UPDATE `gift_code_state`='%s'",
        player->GetPlayerID(), giftCodesStr.c_str(), giftCodesStr.c_str());

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

// ==================================================
//  保存公会信息
// ==================================================
bool PlayerSaveJob::SaveGuildScienceInfo(Player* player)
{
    DBRecorder recorder;
    Json::FastWriter writer;
    Json::Value info;
    Json::Value addition;

    const GUILD_PLAYER_SCIENCE_INFO& guildScienceInfo = player->GetGuildPlayerScienceInfo();

    for (std::vector<GUILD_PLAYER_ADDITION>::const_iterator scienceIt = guildScienceInfo.guild_player_addition_list.begin(); 
        scienceIt != guildScienceInfo.guild_player_addition_list.end(); ++scienceIt)
    {
        addition.clear();
        addition["id"] = scienceIt->id;
        addition["promote_time_gamepoint"] = scienceIt->promote_time_gamepoint;
        addition["promote_time_moneypoint"] = scienceIt->promote_time_moneypoint;
        info["guild_player_addition_list"].append(addition);
    }

    std::string infoStr = writer.write(info);

    recorder = m_dbExecutor->Query(
        "INSERT INTO `guild_science` VALUES(%llu, '%s') ON DUPLICATE KEY UPDATE `science_info`='%s'",
        player->GetPlayerID(), infoStr.c_str(), infoStr.c_str());

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}
// ==================================================
// 保存跨服战斗信息
// ==================================================
bool PlayerSaveJob::SaveMultiserverBattleInfo( Player* player )
{
    DBRecorder recorder;

    Json::FastWriter writer;

    Json::Value item_info;
    Json::Value item_info_json;
    Json::Value item;
    std::string item_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    item_info["version"] = 1;

    MULTISERVER_BATTLE_BAG& bag_info = player->GetMultiserverBattleInfo();
    item_info["integral"] = bag_info.integral;
    item_info["win_count"] = bag_info.win_count;
    item_info["lose_count"] = bag_info.lose_count;
    item_info["exploit_point"] = bag_info.exploit_point;
    item_info["alliance_point"] = bag_info.alliance_point;
    item_info["season_point"] = bag_info.season_point;
    item_info["exploit_time"] = bag_info.exploit_time;
    item_info["alliance_time"] = bag_info.alliance_time;
    item_info["yesterday_count"] = bag_info.yesterday_count;
    item_info["yesterday_3v3_count"] = bag_info.yesterday_3v3_count;
    item_info["yesterday_integral"] = bag_info.yesterday_integral;
    item_info["enter_num"] = bag_info.enter_num;
    item_info["yesterday_num"] = bag_info.yesterday_num;
    item_info["season_time"] = bag_info.season_time;

    for(std::vector<MULTISERVER_BATTLE_ITEM_INFO>::iterator it = bag_info.list.begin(); it != bag_info.list.end(); ++it)
    {
        item_info_json.clear();
        item_info_json["type"] = it->type;
        item_info_json["today_count"] = it->today_count;
        item_info_json["today_exploit"] = it->today_exploit;
        item_info_json["last_time"] = it->last_time;
        item_info["battle_info"].append(item_info_json);
    }

    for(std::vector<LIMIT_BUY>::iterator item_it = bag_info.item_list.begin(); item_it != bag_info.item_list.end(); ++item_it)
    {
        item_info_json.clear();
        item_info_json["product_id"] = item_it->product_id;
        item_info_json["num"] = item_it->num;
        item_info["item_info"].append(item_info_json);
    }

    item_txt = writer.write(item_info);

    int32 winPro = 0;
    if(bag_info.win_count + bag_info.lose_count > 0)
    {
        winPro = (int32)((real)bag_info.win_count / ((real)bag_info.win_count + (real)bag_info.lose_count) * BASE_RATE_NUM);

        CnAssert(0 <= winPro && winPro <= BASE_RATE_NUM);
        GlobalDbOperation::Instance()->SaveRankingAlliance(player, winPro);
    }

    m_dbExecutor->EscapeString(sqlTxt, item_txt.c_str(), item_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `multiserver_battle_info` VALUES(%llu, '%s', %d, %d, %d) "
        "ON DUPLICATE KEY UPDATE `battle_info`='%s',`win_pro`=%d,`integral`=%d,`season_time`=%d", 
        player->GetPlayerID(), sqlTxt, winPro, bag_info.integral,  bag_info.season_time, sqlTxt, winPro, bag_info.integral,  bag_info.season_time);

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 保存游戏收藏信息
// ==================================================
bool PlayerSaveJob::SaveShorcutInfo( Player* player )
{
    DBRecorder recorder;

    Json::FastWriter writer;

    Json::Value shorcut_info;
    std::string shorcut_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    shorcut_info["version"] = 1;
    SHORCUT_BAG& shorcut_bag = player->GetShorcutInfo();

    shorcut_info["shorcut_info"] = shorcut_bag.shorcut_flag;
    shorcut_info["taste_vip"] = shorcut_bag.taste_vip;
    shorcut_info["like_flag"] = shorcut_bag.like_flag;
    shorcut_info["dftime"] = shorcut_bag.dftime;

    shorcut_txt = writer.write(shorcut_info);

    m_dbExecutor->EscapeString(sqlTxt, shorcut_txt.c_str(), shorcut_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `shorcut` SET `player_id` = '%llu', `shorcut_info`='%s'"
        "ON DUPLICATE KEY UPDATE `shorcut_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

// ==================================================
// 保存宠物阵型信息
// ==================================================
bool PlayerSaveJob::SavePetFormation(Player* player)
{
    DBRecorder recorder;

    Json::FastWriter writer;
    std::string db_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    PET_FORMATION_INFO& info = player->GetPetFormation();

    db_txt = StructToJson(info);
    m_dbExecutor->EscapeString(sqlTxt, db_txt.c_str(), db_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `pet_formation` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `pet_formation_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

// ==================================================
// 保存投资计划信息
// ==================================================
bool PlayerSaveJob::SaveInvestmentPlan(Player* player)
{
    DBRecorder recorder;

    std::string db_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    INVESTMENT_PLAN_INFO& info = player->GetInvestmentPlanInfo();

    db_txt = StructToJson(info);
    m_dbExecutor->EscapeString(sqlTxt, db_txt.c_str(), db_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `investment_plan` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `investment_plan_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool PlayerSaveJob::SaveDonate( Player* player )
{
    DBRecorder recorder;

    std::string db_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    DONATE_INFO& info = player->GetDonateInfo();

    db_txt = StructToJson(info);
    m_dbExecutor->EscapeString(sqlTxt, db_txt.c_str(), db_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `donate` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `donate_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool PlayerSaveJob::SaveEgg( Player* player )
{
    DBRecorder recorder;

    std::string db_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    EGG_ACTIVITY_INFO& info = player->GetEggInfo();

    db_txt = StructToJson(info);
    m_dbExecutor->EscapeString(sqlTxt, db_txt.c_str(), db_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `activity` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `egg_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool PlayerSaveJob::SavePlayerStateInfo( Player* player )
{
    DBRecorder recorder;

    std::string db_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    PLAYER_STATE_INFO& info = player->GetStateInfo();

    db_txt = StructToJson(info);
    m_dbExecutor->EscapeString(sqlTxt, db_txt.c_str(), db_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `player_state_info` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `state_info`='%s'", player->GetPlayerID(), sqlTxt, sqlTxt);

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool PlayerSaveJob::SaveHeadPortraitInfo(Player* player)
{
    char sqlTxt[MAX_SQLTXT_LEN] = {0};
    std::string head_str("");
    DBRecorder recorder;

    HEAD_PORTRAIT_INFO& headPortrait = player->GetHeadPortraitInfo();

    head_str = StructToJson(headPortrait);
    m_dbExecutor->EscapeString(sqlTxt, head_str.c_str(), head_str.length());

    recorder = m_dbExecutor->Query(
        "INSERT INTO `head_portrait` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `head_portrait_open`='%s'",
        player->GetPlayerID(), sqlTxt, sqlTxt);
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}
