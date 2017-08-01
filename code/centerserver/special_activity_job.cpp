#include "centerserver_pch.h"
#include "dbdriver.h"
#include "special_activity_job.h"
#include "special_activity_manager_gc.h"
#include "city_fight_manager_gc.h"
#include "game_handler.h"

IMPLEMENT_SINGLETON(SpecialActivityJob)
SpecialActivityJob::SpecialActivityJob()
{
    m_dbExecutor = 0;
}

SpecialActivityJob::~SpecialActivityJob()
{

}

bool SpecialActivityJob::SaveBattlePowerActivityInfo( void* data )
{
    SPECIAL_ACTIVITY_BATTLE_POWER_ACTIVITY_INFO* info = (SPECIAL_ACTIVITY_BATTLE_POWER_ACTIVITY_INFO*) data;
    if (!info)
        return false;

    DBRecorder recorder;
    Json::FastWriter writer;
    Json::Value info_json;
    std::string info_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    info_json["time"] = info->time;
    info_txt = writer.write(info_json);

    m_dbExecutor->EscapeString(sqlTxt, info_txt.c_str(), info_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `special_activity` SET `activity_key` = '%s', `activity_info`='%s'"
        "ON DUPLICATE KEY UPDATE `activity_info`='%s'", "battle_power_activity_info", sqlTxt, sqlTxt);

    if (recorder.GetAffNum() < 1)
        return false;

    return true;
}

void SpecialActivityJob::GetBattlePowerActivityInfo( uint32 msgId )
{
    m_dbExecutor->Query(this, &SpecialActivityJob::CBGetBattlePowerActivityInfo, msgId, 
        "SELECT `activity_info` FROM `special_activity` WHERE `activity_key`='%s'", "battle_power_activity_info");
}

void SpecialActivityJob::CBGetBattlePowerActivityInfo( DBRecorder& res, uint32 msgId )
{
    Json::Reader reader;
    Json::Value info_json;
    std::string info_str;
    SPECIAL_ACTIVITY_BATTLE_POWER_ACTIVITY_INFO info ;
    info.Clear();
    info.time = 0;

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(info_str);

        if(!reader.parse(info_str, info_json, false))
        {
            CnError("CBGetBattlePowerActivityInfo failed: %s \n", info_str.c_str());
            return;
        }

        info.time = info_json["time"].asUInt();
    }

    switch (msgId)
    {
    case MSG_SPECIAL_ACTIVITY_BATTLE_POWER_ACTIVITY_REQ:
        {
            SpecialActivityManager::Instance()->OnRecv(0, 0, MSG_SPECIAL_ACTIVITY_BATTLE_POWER_ACTIVITY_ACK, &info);
            break;
        }
    default:
        CnAssert(false);
        break;
    }
}

bool SpecialActivityJob::SaveFirstTimeRechargeActivityInfo( uint64 playerID, int32 status )
{
    DBRecorder recorder;
    Json::FastWriter writer;
    Json::Value info_json;
    std::string info_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    info_json["status"] = status;
    info_txt = writer.write(info_json);

    m_dbExecutor->EscapeString(sqlTxt, info_txt.c_str(), info_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `first_time_recharge_activity` SET `player_id` = '%llu', `activity_info`='%s'"
        "ON DUPLICATE KEY UPDATE `activity_info`='%s'", playerID, sqlTxt, sqlTxt);

    if (recorder.GetAffNum() < 1)
        return false;

    return true;
}

void SpecialActivityJob::GetFirstTimeRechargeActivityInfo( uint64 playerID, std::string type )
{
    m_dbExecutor->Query(this, &SpecialActivityJob::CBGetFirstTimeRechargeActivityInfo, playerID, type,
        "SELECT `activity_info` FROM `first_time_recharge_activity` WHERE `player_id`='%llu'", playerID);
}

void SpecialActivityJob::CBGetFirstTimeRechargeActivityInfo( DBRecorder& res, uint64 playerID, std::string type )
{
    Json::Reader reader;
    Json::Value info_json;
    std::string info_str;
    int32 status = NO_REWARD;

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(info_str);

        if(!reader.parse(info_str, info_json, false))
        {
            CnError("CBGetFirstTimeRechargeActivityInfo failed: %s \n", info_str.c_str());
            return;
        }

        status = info_json["status"].asInt();
    }

    SpecialActivityManager::Instance()->CBGetFirstTimeRechargeActivityInfo(playerID, type, status);
}

bool SpecialActivityJob::SaveDailyRechargeActivityInfo( uint64 playerID, int32 status, uint32 time )
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("UPDATE `daily_recharge_activity` SET `reward_flag`='%d' WHERE `player_id`='%llu' AND UNIX_TIMESTAMP(`update_time`)='%llu'", status, playerID, (int64)time);
    
    // 保证有改变数据,才返回true
    if(recorder.GetAffNum() < 1)
        return false;

    return true;
}

void SpecialActivityJob::GetCurDailyRechargeActivityInfo( uint64 playerID, int32 param, std::string type)
{
    Crown::CSDDateTime time = Crown::SDNow();
    time.SetTime(0, 0, 0);
    m_dbExecutor->Query(this, &SpecialActivityJob::CBGetCurDailyRechargeActivityInfo, playerID, param, type,
        "SELECT `money_point`, `reward_flag`, UNIX_TIMESTAMP(`update_time`) FROM `daily_recharge_activity` WHERE `player_id`='%llu' AND UNIX_TIMESTAMP(`update_time`)='%llu'", playerID, time.GetTimeValue());
}

void SpecialActivityJob::CBGetCurDailyRechargeActivityInfo( DBRecorder& res, uint64 playerID, int32 param, std::string type )
{
    int32 money = 0;
    int32 status = 0;
    int64 time = 0;
    if(res.GetRowNum() > 0)
    {
        res.GetRecord(money);
        res.GetRecord(status);
        res.GetRecord(time);
    }

    SpecialActivityManager::Instance()->CBGetCurDailyRechargeActivityInfo(playerID, param, type, money, status, (uint32)time);
}

bool SpecialActivityJob::DeleteOldDailyRechargeActivityInfo( uint64 playerID, uint32 time )
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("DELETE FROM `daily_recharge_activity` WHERE `player_id`='%llu' AND UNIX_TIMESTAMP(`update_time`)<'%u'", playerID, time);

    // 保证有改变数据,才返回true
    if(recorder.GetAffNum() < 1)
        return false;

    return true;
}

void SpecialActivityJob::GetOldDailyRechargeActivityInfo( uint64 playerID)
{
    Crown::CSDDateTime time = Crown::SDNow();
    time.SetTime(0, 0, 0);
    int64 curTime = time.GetTimeValue();
    m_dbExecutor->Query(this, &SpecialActivityJob::CBGetOldDailyRechargeActivityInfo, playerID, (int32)curTime,
        "SELECT `money_point`, `reward_flag` FROM `daily_recharge_activity` WHERE `player_id`='%llu' AND UNIX_TIMESTAMP(`update_time`)<'%llu'", playerID, curTime);

}

void SpecialActivityJob::CBGetOldDailyRechargeActivityInfo( DBRecorder& res, uint64 playerID, int32 time )
{
    uint32 count = (uint32)res.GetRowNum();
    if(count > 0)
    {
        DailyRechargeActivityInfoList infoList;
        infoList.clear();

        DailyRechargeActivityInfo info;
        for(uint32 i = 0; i < count; ++i)
        {
            info.Clear();
            res.GetRecord(info.money_point);
            res.GetRecord(info.status);
            infoList.push_back(info);
        }
        
        // 当有老数据时,才调用回调
        SpecialActivityManager::Instance()->CBGetOldDailyRechargeActivityInfo(playerID, infoList, time);
    }
}

bool SpecialActivityJob::SaveUpgradeWingActivityInfo( uint64 playerID, int32 status )
{
    DBRecorder recorder;
    Json::FastWriter writer;
    Json::Value info_json;
    std::string info_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    info_json["status"] = status;
    info_txt = writer.write(info_json);

    m_dbExecutor->EscapeString(sqlTxt, info_txt.c_str(), info_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `upgrade_wing_acitivity` SET `player_id` = '%llu', `activity_info`='%s'"
        "ON DUPLICATE KEY UPDATE `activity_info`='%s'", playerID, sqlTxt, sqlTxt);

    if (recorder.GetAffNum() < 1)
        return false;

    return true;
}

void SpecialActivityJob::GetUpgradeWingActivityInfo( uint64 playerID, std::string type, int32 param )
{
    m_dbExecutor->Query(this, &SpecialActivityJob::CBGetUpgradeWingActivityInfo, playerID, type, param,
        "SELECT `activity_info` FROM `upgrade_wing_acitivity` WHERE `player_id`='%llu'", playerID);
}

void SpecialActivityJob::CBGetUpgradeWingActivityInfo( DBRecorder& res, uint64 playerID, std::string type, int32 param )
{
    Json::Reader reader;
    Json::Value info_json;
    std::string info_str;
    int32 status = NO_REWARD;

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(info_str);

        if(!reader.parse(info_str, info_json, false))
        {
            CnError("CBGetUpgradeWingActivityInfo failed: %s \n", info_str.c_str());
            return;
        }

        status = info_json["status"].asInt();
    }

    SpecialActivityManager::Instance()->CBGetUpgradeWingActivityInfo(playerID, type, param, status);
}

bool SpecialActivityJob::SaveMicroClientActivityInfo( uint64 playerID, int32 status )
{
    DBRecorder recorder;
    Json::FastWriter writer;
    Json::Value info_json;
    std::string info_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    info_json["status"] = status;
    info_txt = writer.write(info_json);

    m_dbExecutor->EscapeString(sqlTxt, info_txt.c_str(), info_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `micro_client_activity` SET `player_id` = '%llu', `activity_info`='%s'"
        "ON DUPLICATE KEY UPDATE `activity_info`='%s'", playerID, sqlTxt, sqlTxt);

    if (recorder.GetAffNum() < 1)
        return false;

    return true;
}

void SpecialActivityJob::GetMicroClientActivityInfo( uint64 playerID )
{
    m_dbExecutor->Query(this, &SpecialActivityJob::CBGetMicroClientActivityInfo, playerID,
        "SELECT `activity_info` FROM `micro_client_activity` WHERE `player_id`='%llu'", playerID);
}

void SpecialActivityJob::CBGetMicroClientActivityInfo( DBRecorder& res, uint64 playerID )
{
    Json::Reader reader;
    Json::Value info_json;
    std::string info_str;
    int32 status = NO_REWARD;

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(info_str);

        if(!reader.parse(info_str, info_json, false))
        {
            CnError("CBGetMicroClientActivityInfo failed: %s \n", info_str.c_str());
            return;
        }

        status = info_json["status"].asInt();
    }

    SpecialActivityManager::Instance()->CBGetMicroClientActivityInfo(playerID, status);
}

bool SpecialActivityJob::SaveCityFightActivityInfo( CITY_FIGHT_INFO_NTF* info )
{
    if (!info)
        return false;

    DBRecorder recorder;
    Json::FastWriter writer;
    Json::Value info_json;
    Json::Value info_list_json;
    std::string info_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    for (uint32 i = 0; i < info->city_list.size(); ++i)
    {
        info_json["city_id"] = info->city_list[i].city_id;
        info_json["flag"] = info->city_list[i].flag;
        info_json["guild_name"] = info->city_list[i].guild_name;
        info_json["timestamp"] = info->city_list[i].timestamp;
        info_list_json.append(info_json);
    }

    info_txt = writer.write(info_list_json);
    m_dbExecutor->EscapeString(sqlTxt, info_txt.c_str(), info_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `special_activity` SET `activity_key` = '%s', `activity_info`='%s'"
        "ON DUPLICATE KEY UPDATE `activity_info`='%s'", "city_fight_activity_info", sqlTxt, sqlTxt);

    if (recorder.GetAffNum() < 1)
        return false;

    return true;
}

void SpecialActivityJob::GetCityFightActivityInfo()
{
    m_dbExecutor->Query(this, &SpecialActivityJob::CBCityFightActivityInfo,
        "SELECT `activity_info` FROM `special_activity` WHERE `activity_key`='%s'", "city_fight_activity_info");
}

void SpecialActivityJob::CBCityFightActivityInfo( DBRecorder& res )
{
    if (res.GetRowNum() > 0)
    {
        Json::Reader reader;
        Json::Value info_json;
        std::string info_str;
        CITY_FIGHT_ONE_CITY_NTF cityInfo ;

        res.GetRecord(info_str);
        if (!reader.parse(info_str, info_json, false))
        {
            CnError("CBCityFightActivityInfo :%s failed\n", info_str.c_str());
            return;
        }

        CITY_FIGHT_INFO_NTF& saveInfo = CityFightManager::Instance()->GetCityFightSaveInfo();

        uint32 saveInfo_city_num = info_json.size();
        for (uint32 i = 0; i < saveInfo_city_num; i++)
        {
            cityInfo.Clear();

            Json::Value& val = info_json[i];
            cityInfo.city_id = val["city_id"].asUInt();
            cityInfo.flag = val["flag"].asInt();
            cityInfo.guild_name = val["guild_name"].asString();
            cityInfo.timestamp = val["timestamp"].asUInt();

            saveInfo.city_list.push_back(cityInfo);
        }
    }

    CityFightManager::Instance()->InitFinish();
}

bool SpecialActivityJob::SaveVipStatisticsInfo(VIP_STATISTICS_AUTO_GROWTH_LIST& info)
{
    DBRecorder recorder;
    std::string info_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    info_txt = StructToJson(info);

    m_dbExecutor->EscapeString(sqlTxt, info_txt.c_str(), info_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `special_activity` SET `activity_key` = '%s', `activity_info`='%s'"
        "ON DUPLICATE KEY UPDATE `activity_info`='%s'", "vip_statistics", sqlTxt, sqlTxt);

    if (recorder.GetAffNum() < 1)
        return false;

    return true;
}

void SpecialActivityJob::GetVipStatisticsInfo(VIP_STATISTICS_AUTO_GROWTH_LIST& auto_growth_list, std::map<uint64, int8>& playerVipMap)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `activity_info` FROM `special_activity` WHERE `activity_key`='%s'", "vip_statistics");

    if(!recorder.HasExecut())
    {
        CnInfo("unfound vip_statistics \n");
        return;
    }

    if (recorder.GetRowNum() > 0)
    {
        std::string info_str;
        VIP_STATISTICS_AUTO_GROWTH_LIST vipInfo;

        recorder.GetRecord(info_str);

        if(!JsonToStruct(vipInfo, info_str))
        {
            CnInfo("unfound vip_statistics \n");
            return;
        }

        auto_growth_list = vipInfo;
    }

    recorder = m_dbExecutor->Query("SELECT player_id,vip_level FROM offline_player WHERE vip_level>0");

    if(!recorder.HasExecut())
    {
        CnInfo("unfound vip_statistics \n");
        return;
    }

    uint64 playerId;
    int32 vipLevel;
    for(uint64 i = 0; i < recorder.GetRowNum(); ++i)
    {
        recorder.GetRecord(playerId);
        recorder.GetRecord(vipLevel);

        CnAssert(playerVipMap.find(playerId) == playerVipMap.end());
        playerVipMap[playerId] = vipLevel;
    }
}

bool SpecialActivityJob::SaveDonateAllInfo(DONATE_ALL_INFO& info)
{
    DBRecorder recorder;
    std::string info_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    info_txt = StructToJson(info);

    m_dbExecutor->EscapeString(sqlTxt, info_txt.c_str(), info_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `special_activity` SET `activity_key` = '%s', `activity_info`='%s'"
        "ON DUPLICATE KEY UPDATE `activity_info`='%s'", "donate_all", sqlTxt, sqlTxt);

    if (recorder.GetAffNum() < 1)
        return false;

    return true;
}

void SpecialActivityJob::GetDonateAllInfo(DONATE_ALL_INFO& info)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `activity_info` FROM `special_activity` WHERE `activity_key`='%s'", "donate_all");

    if(!recorder.HasExecut())
    {
        CnInfo("unfound donate_all \n");
        return;
    }

    if (recorder.GetRowNum() > 0)
    {
        std::string info_str;
        DONATE_ALL_INFO donateInfo;

        recorder.GetRecord(info_str);

        if(!JsonToStruct(donateInfo, info_str))
        {
            CnInfo("unfound donate_all \n");
            return;
        }

        info = donateInfo;
    }
}

bool SpecialActivityJob::SaveSeasonInfo(SEASON_INFO& info)
{
    DBRecorder recorder;
    std::string info_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    info_txt = StructToJson(info);

    m_dbExecutor->EscapeString(sqlTxt, info_txt.c_str(), info_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `special_activity` SET `activity_key` = '%s', `activity_info`='%s'"
        "ON DUPLICATE KEY UPDATE `activity_info`='%s'", "season_info", sqlTxt, sqlTxt);

    if (recorder.GetAffNum() < 1)
        return false;

    return true;
}

void SpecialActivityJob::GetSeasonInfo(SEASON_INFO& info)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `activity_info` FROM `special_activity` WHERE `activity_key`='%s'", "season_info");

    if(!recorder.HasExecut())
    {
        CnInfo("unfound season_info \n");
        return;
    }

    if (recorder.GetRowNum() > 0)
    {
        std::string info_str;

        recorder.GetRecord(info_str);

        if(!JsonToStruct(info, info_str))
        {
            CnInfo("unfound season_info \n");
            return;
        }
    }
}

bool SpecialActivityJob::SaveRankListInfo( uint32 key, TOP_UP_RANK_INFO_ACK& info )
{
    DBRecorder recorder;
    std::string info_txt;
    //std::string key_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};
    //char keyTxt[MAX_SQLTXT_LEN] = {0};

    info_txt = StructToJson(info);
    //key_txt = "rank_" + key;

    m_dbExecutor->EscapeString(sqlTxt, info_txt.c_str(), info_txt.length());
    //m_dbExecutor->EscapeString(keyTxt, key_txt.c_str(), key_txt.length());

    recorder = m_dbExecutor->Query("INSERT INTO `special_activity` SET `activity_key` = '%d', `activity_info`='%s'"
        "ON DUPLICATE KEY UPDATE `activity_info`='%s'", key, sqlTxt, sqlTxt);

    if (recorder.GetAffNum() < 1)
        return false;

    return true;
}

void SpecialActivityJob::GetRankListInfo( uint32 key, TOP_UP_RANK_INFO_ACK& info )
{
    //std::string key_txt = "rank_" + key ;
    //char keyTxt[MAX_SQLTXT_LEN] = {0};
    //m_dbExecutor->EscapeString(keyTxt, key_txt.c_str(), key_txt.length());

    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `activity_info` FROM `special_activity` WHERE `activity_key`='%d'", key);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound donate_all \n");
        return;
    }

    if (recorder.GetRowNum() > 0)
    {
        std::string info_str;
        TOP_UP_RANK_INFO_ACK rankInfo;

        recorder.GetRecord(info_str);

        if(!JsonToStruct(rankInfo, info_str))
        {
            CnInfo("unfound TOP_UP_RANK_INFO_ACK \n");
            return;
        }

        info = rankInfo;
    }
}

bool SpecialActivityJob::SaveDailyFashionAllInfo( DAILY_DRAW_FASHION_INFO& info )
{
    DBRecorder recorder;
    std::string info_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    info_txt = StructToJson(info);

    m_dbExecutor->EscapeString(sqlTxt, info_txt.c_str(), info_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `special_activity` SET `activity_key` = '%s', `activity_info`='%s'"
        "ON DUPLICATE KEY UPDATE `activity_info`='%s'", "daily_fashion", sqlTxt, sqlTxt);

    if (recorder.GetAffNum() < 1)
        return false;

    return true;
}

void SpecialActivityJob::GetDailyFashionAllInfo( DAILY_DRAW_FASHION_INFO& info )
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("SELECT `activity_info` FROM `special_activity` WHERE `activity_key`='%s'", "daily_fashion");

    if(!recorder.HasExecut())
    {
        CnInfo("unfound donate_all \n");
        return;
    }

    if (recorder.GetRowNum() > 0)
    {
        std::string info_str;
        DAILY_DRAW_FASHION_INFO donateInfo;

        recorder.GetRecord(info_str);

        if(!JsonToStruct(donateInfo, info_str))
        {
            CnInfo("unfound donate_all \n");
            return;
        }

        info = donateInfo;
    }
}
