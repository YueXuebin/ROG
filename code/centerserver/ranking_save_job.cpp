#include "centerserver_pch.h"
#include "ranking_save_job.h"
#include "dbdriver.h"
#include "ranking_manager.h"

IMPLEMENT_SINGLETON(RankingSaveJob);

bool RankingSaveJob::SaveRankUpdateTime(int32 rankType, uint32 rankTime)
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("INSERT INTO ranking_schedule VALUES(%d,FROM_UNIXTIME(%u)) ON DUPLICATE KEY UPDATE execute_time=FROM_UNIXTIME(%u)", rankType, rankTime, rankTime);

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

bool RankingSaveJob::SaveAllFirstOfRanking()
{
    DBRecorder recorder;

    std::string sqlTxt = "INSERT INTO `ranking_all_first` SELECT %d,`player_id`,`sex`,`level`,`career`,`guild_name`,`player_name` FROM %s WHERE `rank`=1";

    recorder = m_dbExecutor->Query(sqlTxt.c_str(), RANKING_TYPE_BATTLE_POWER, "ranking_battle_power");
    if(!recorder.HasExecut())
        return false;

    recorder = m_dbExecutor->Query(sqlTxt.c_str(), RANKING_TYPE_HOROSCOPE, "ranking_horoscope");
    if(!recorder.HasExecut())
        return false;

    //recorder = m_dbExecutor->Query(sqlTxt.c_str(), RANKING_TYPE_GAME_POINT, "gamepoint_ranking");
    //if(!recorder.HasExecut())
    //    return false;

    recorder = m_dbExecutor->Query(sqlTxt.c_str(), RANKING_TYPE_OFFLINE_BATTLE, "ranking_offline_player");
    if(!recorder.HasExecut())
        return false;

    recorder = m_dbExecutor->Query(sqlTxt.c_str(), RANKING_TYPE_ASHURA, "ashura_rank");
    if(!recorder.HasExecut())
        return false;

    //recorder = m_dbExecutor->Query(sqlTxt.c_str(), RANKING_TYPE_PLAYER_LEVEL, "ranking_player_level");
    //if(!recorder.HasExecut())
    //    return false;

    recorder = m_dbExecutor->Query(sqlTxt.c_str(), RANKING_TYPE_WING, "ranking_wing");
    if(!recorder.HasExecut())
        return false;

    recorder = m_dbExecutor->Query(sqlTxt.c_str(), RANKING_TYPE_FOLLOW_PET, "ranking_pet");
    if(!recorder.HasExecut())
        return false;

    recorder = m_dbExecutor->Query(sqlTxt.c_str(), RANKING_TYPE_RIDE_PET, "ranking_ride");
    if(!recorder.HasExecut())
        return false;

    recorder = m_dbExecutor->Query(sqlTxt.c_str(), RANKING_TYPE_FASHION, "ranking_fashion");
    if(!recorder.HasExecut())
        return false;

    return true;
}

bool RankingSaveJob::ClearAllFirstOfRanking()
{
    // 清空表
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM ranking_all_first");

    if(!recorder.HasExecut())
        return false;

    return true;
}

bool RankingSaveJob::SaveBattlePowerRank()
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("INSERT INTO ranking_battle_power SELECT (@new_rank :=@new_rank + 1),T1.player_id,T1.sex,T1.level,T1.career,IFNULL(T2.guild_name,''),T1.battle_power,T1.player_name FROM (SELECT (@new_rank :=0),`player_id`,`player_name`,`battle_power`,`sex`,`level`,`career`,`guild_id` FROM player ORDER BY `battle_power` DESC LIMIT 200) AS T1 LEFT JOIN `guild_info` AS T2 ON T1.guild_id = T2.guild_id");

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool RankingSaveJob::ClearBattlePowerRank()
{
    // 清空表
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM ranking_battle_power");

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

bool RankingSaveJob::SaveHoroscopeRank()
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query(
        "INSERT INTO ranking_horoscope "
        "SELECT (@new_rank :=@new_rank + 1),T4.player_id,T4.sex,T4.level,T4.career,IFNULL(T3.guild_name, ''),T4.horoscope_level,T4.player_name FROM "
        "(SELECT (@new_rank :=0),T2.player_id,T1.horoscope_level,T2.level,T2.player_name,T2.career,T2.sex,T2.guild_id FROM "
        "(SELECT `player_id`,`horoscope_level` FROM horoscope ORDER BY `horoscope_level` DESC, `levelup_time` ASC LIMIT 200) AS T1 "
        "INNER JOIN `player` AS T2 ON T1.player_id=T2.player_id) AS T4 "
        "LEFT JOIN `guild_info` AS T3 ON T3.guild_id=T4.guild_id"
        );

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool RankingSaveJob::ClearHoroscopeRank()
{
    // 清空表
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM ranking_horoscope");

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool RankingSaveJob::SaveGamepointRank()
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("INSERT INTO gamepoint_ranking SELECT (@new_rank :=@new_rank + 1),T1.player_id,T1.sex,T1.level,T1.career,IFNULL(T2.guild_name,''),T1.game_point,T1.player_name FROM (SELECT (@new_rank :=0),`player_id`,`player_name`,`game_point`,`sex`,`level`,`career`,`guild_id` FROM player ORDER BY `game_point` DESC LIMIT 200) AS T1 LEFT JOIN `guild_info` AS T2 ON T1.guild_id = T2.guild_id");

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool RankingSaveJob::ClearGamepointRank()
{
    // 清空表
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM gamepoint_ranking");

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool RankingSaveJob::SavePlayerLevelRank()
{
    DBRecorder recorder;

    //recorder = m_dbExecutor->Query("INSERT INTO ranking_player_level SELECT (@new_rank :=@new_rank + 1),T1.player_id,T1.sex,T1.level,T1.career,IFNULL(T2.guild_name,''),T1.player_name,T1.battle_power FROM (SELECT (@new_rank :=0),`player_id`,`player_name`,`sex`,`level`,`career`,`guild_id`,`battle_power` FROM player ORDER BY `level` DESC,`battle_power` DESC LIMIT 200) AS T1 LEFT JOIN `guild_info` AS T2 ON T1.guild_id = T2.guild_id");
    recorder = m_dbExecutor->Query("INSERT INTO ranking_player_level(rank, player_id, sex, level, career, player_name) (SELECT (@new_rank :=@new_rank + 1),T1.player_id,T1.sex,T1.level,T1.career,T1.player_name FROM (SELECT (@new_rank :=0),`player_id`,`player_name`,`sex`,`level`,`career` FROM player ORDER BY `level` DESC LIMIT 100) AS T1)");

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool RankingSaveJob::ClearPlayerLevelRank()
{
    // 清空表
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM ranking_player_level");

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool RankingSaveJob::SaveGuildRank()
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query(
        "INSERT INTO `guild_ranking` "
        "SELECT (@new_rank :=@new_rank + 1),`guild_id`,`guild_name`,`level`,`member_count`,`battle_power` FROM"
        "(SELECT (@new_rank :=0),`guild_id`,`guild_name`,`level`,`member_count`,`battle_power` FROM `guild_info` ORDER BY `level` DESC,`battle_power` DESC) AS T1"
        );

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool RankingSaveJob::ClearGuildRank()
{
    // 清空表
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM guild_ranking");

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool RankingSaveJob::SaveAshuraRank()
{
    DBRecorder res;

    res = m_dbExecutor->Query(
        "INSERT INTO `ashura_rank` "
        "SELECT (@new_rank :=@new_rank + 1),`player_id`,`kill_count`,0,'',1,0,'',1 FROM"
        "(SELECT (@new_rank :=0),`player_id`,`kill_count` FROM `ashura` ORDER BY `kill_count` DESC) AS T1"
        );

    if(!res.HasExecut())
        return false;

    m_dbExecutor->Query(
        "UPDATE `ashura_rank` AS T1, player AS T2 "
        "SET T1.sex = T2.sex, T1.player_name = T2.player_name, T1.level = T2.level, T1.career = T2.career, T1.guild_id = T2.guild_id "
        "WHERE T1.player_id = T2.player_id"
        );
    if(!res.HasExecut())
        return false;

    return true;
}

bool RankingSaveJob::ClearAshuraRank()
{
    // 清空表
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM `ashura_rank`");

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool RankingSaveJob::SaveOldOfflinePlayerRank()
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query(
        "UPDATE `ranking_offline_player` "
        "SET `old_rank`=`rank`"
        );

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

void RankingSaveJob::GetRankingRefreshTime(uint32& m_quarterHourRefreshTime, uint32& dailyRefreshTime)
{
    DBRecorder recorder;
    int64 time = 0;

    recorder = m_dbExecutor->Query("SELECT UNIX_TIMESTAMP(`execute_time`) FROM `ranking_schedule` WHERE `ranking_type`=%d", RANKING_TYPE_PLAYER_LEVEL);
    if(recorder.HasExecut() && (recorder.GetRowNum() > 0))
    {
        recorder.GetRecord(time);

        // 下次刷新时间
        m_quarterHourRefreshTime = (uint32)time;
        //hourRefreshTime = (uint32)time + 3600;
    }

    recorder = m_dbExecutor->Query("SELECT UNIX_TIMESTAMP(`execute_time`) FROM `ranking_schedule` WHERE `ranking_type`=%d", RANKING_TYPE_DAILY_REFRESH);
    if(recorder.HasExecut() && (recorder.GetRowNum() > 0))
    {
        recorder.GetRecord(time);
        dailyRefreshTime = (uint32)time;
    }
}

bool RankingSaveJob::SaveWingBattlePowerRank()
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query(
        "INSERT INTO ranking_wing "
        "SELECT (@new_rank :=@new_rank + 1),T4.player_id,T4.sex,T4.level,T4.career,IFNULL(T3.guild_name, ''),T4.wing_battle_power,T4.player_name FROM "
        "(SELECT (@new_rank :=0),T2.player_id,T1.wing_battle_power,T2.level,T2.player_name,T2.career,T2.sex,T2.guild_id FROM "
        "(SELECT `player_id`,`wing_battle_power` FROM wing ORDER BY `wing_battle_power` DESC LIMIT 200) AS T1 "
        "INNER JOIN `player` AS T2 ON T1.player_id=T2.player_id) AS T4 "
        "LEFT JOIN `guild_info` AS T3 ON T3.guild_id=T4.guild_id"
        );

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool RankingSaveJob::ClearWingBattlePowerRank()
{
    // 清空表
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM ranking_wing");

    if(!recorder.HasExecut()) 
    {
        return false;
    }

    return true;
}

bool RankingSaveJob::SavePetBattlePowerRank()
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query(
        "INSERT INTO ranking_pet "
        "SELECT (@new_rank :=@new_rank + 1),T4.player_id,T4.sex,T4.level,T4.career,IFNULL(T3.guild_name, ''),T4.pet_battle_power,T4.player_name FROM "
        "(SELECT (@new_rank :=0),T2.player_id,T1.pet_battle_power,T2.level,T2.player_name,T2.career,T2.sex,T2.guild_id FROM "
        "(SELECT `player_id`,`pet_battle_power` FROM pet ORDER BY `pet_battle_power` DESC LIMIT 200) AS T1 "
        "INNER JOIN `player` AS T2 ON T1.player_id=T2.player_id) AS T4 "
        "LEFT JOIN `guild_info` AS T3 ON T3.guild_id=T4.guild_id"
        );

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool RankingSaveJob::ClearPetBattlePowerRank()
{
    // 清空表
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM ranking_pet");

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

bool RankingSaveJob::SaveRideBattlePowerRank()
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query(
        "INSERT INTO ranking_ride "
        "SELECT (@new_rank :=@new_rank + 1),T4.player_id,T4.sex,T4.level,T4.career,IFNULL(T3.guild_name, ''),T4.ride_battle_power,T4.player_name FROM "
        "(SELECT (@new_rank :=0),T2.player_id,T1.ride_battle_power,T2.level,T2.player_name,T2.career,T2.sex,T2.guild_id FROM "
        "(SELECT `player_id`,`ride_battle_power` FROM ride ORDER BY `ride_battle_power` DESC LIMIT 200) AS T1 "
        "INNER JOIN `player` AS T2 ON T1.player_id=T2.player_id) AS T4 "
        "LEFT JOIN `guild_info` AS T3 ON T3.guild_id=T4.guild_id"
        );

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool RankingSaveJob::ClearRideBattlePowerRank()
{
    // 清空表
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM ranking_ride");

    if(!recorder.HasExecut()) 
    {
        return false;
    }

    return true;
}

bool RankingSaveJob::SaveFashionBattlePowerRank()
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query(
        "INSERT INTO ranking_fashion "
        "SELECT (@new_rank :=@new_rank + 1),T4.player_id,T4.sex,T4.level,T4.career,IFNULL(T3.guild_name, ''),T4.fashion_battle_power,T4.player_name FROM "
        "(SELECT (@new_rank :=0),T2.player_id,T1.fashion_battle_power,T2.level,T2.player_name,T2.career,T2.sex,T2.guild_id FROM "
        "(SELECT `player_id`,`fashion_battle_power` FROM equip ORDER BY `fashion_battle_power` DESC LIMIT 200) AS T1 "
        "INNER JOIN `player` AS T2 ON T1.player_id=T2.player_id) AS T4 "
        "LEFT JOIN `guild_info` AS T3 ON T3.guild_id=T4.guild_id"
        );

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool RankingSaveJob::ClearFashionBattlePowerRank()
{
    // 清空表
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM ranking_fashion");

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

bool RankingSaveJob::SavePlotDungonScoreRank()
{
    // 清空表
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM ranking_dungeon_score");

    if(!recorder.HasExecut())
    {
        return false;
    }

    recorder = m_dbExecutor->Query("INSERT INTO ranking_dungeon_score (rank, player_id, sex, `level`, career, player_name,score) (SELECT (@new_rank :=@new_rank + 1), T1.player_id, T1.sex, T1.`level`, T1.career, T1.player_name,T1.score FROM ( SELECT (@new_rank := 0), `player_id`, `player_name`, `sex`, `level`, `career`,`score` FROM offline_player ORDER BY `score` DESC LIMIT 100) AS T1)");

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;



    return true;
}

bool RankingSaveJob::SaveAchiveCountRank()
{

    // 清空表
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM ranking_achive");

    if(!recorder.HasExecut())
    {
        return false;
    }

    recorder = m_dbExecutor->Query("INSERT INTO ranking_achive (rank, player_id, sex, `level`, career,count ,player_name) (SELECT (@new_rank :=@new_rank + 1), T1.player_id, T1.sex, T1.`level`, T1.career, T1.achive_count, T1.player_name FROM ( SELECT (@new_rank := 0), `player_id`, `player_name`, `sex`, `level`, `career`,`achive_count` FROM offline_player ORDER BY `achive_count` DESC LIMIT 100) AS T1)");

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}
