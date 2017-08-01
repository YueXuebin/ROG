#include "centerserver_pch.h"
#include "ranking_loader_job.h"
#include "dbdriver.h"
#include "ranking_list_manager.h"
#include "game_handler.h"
#include "player.h"
#include "player_manager.h"
#include "special_activity_manager_gc.h"

IMPLEMENT_SINGLETON(RankingLoaderJob)

void RankingLoaderJob::GetBattlePowerRanking( uint32 count, uint32 msgId )
{
    char buff[100];
    memset(buff, 0, 100);
    Crown::SDItoa(buff, 100, count);
    std::string txt = "SELECT `player_name` FROM `ranking_battle_power` ORDER BY `battle_power` DESC LIMIT ";
    txt += buff;

    m_dbExecutor->Query(this, &RankingLoaderJob::CBGetBattlePowerRanking, msgId, txt.c_str());
}

void RankingLoaderJob::CBGetBattlePowerRanking( DBRecorder& res, uint32 msgId )
{
    switch (msgId)
    {
    case MSG_SPECIAL_ACTIVITY_BATTLE_POWER_RANKING_REQ:
        {
            SPECIAL_ACTIVITY_BATTLE_POWER_RANKING_ACK ack;
            ack.Clear();

            SPECIAL_ACTIVITY_RANKING_INFO info;

            for(uint32 i = 0; i < res.GetRowNum(); ++i)
            {
                info.Clear();
                res.GetRecord(info.name);
                ack.ranking_list.push_back(info);
            }

            SpecialActivityManager::Instance()->OnRecv(0, 0, MSG_SPECIAL_ACTIVITY_BATTLE_POWER_RANKING_ACK, &ack);

            break;
        }
    default:
        break;
    }
}

void RankingLoaderJob::LoadPlayerListByAllFirst(uint64 playerID, std::vector<int32>& rankList)
{
    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadPlayerListByAllFirst, playerID, "SELECT `rank_type`,`sex`,`level`,`career`,`guild_name`,`player_name` FROM `ranking_all_first`");
}

void RankingLoaderJob::CBLoadPlayerListByAllFirst(DBRecorder& res, uint64 playerID)
{
    int32 rank = 0;
    RANKING_LIST playerRankInfo;
    RANKING_PLAYER_INFO playerInfo;

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        playerInfo.Clear();
        res.GetRecord(playerInfo.rank);
        res.GetRecord(playerInfo.sex);
        res.GetRecord(playerInfo.level);
        res.GetRecord(playerInfo.career);
        res.GetRecord(playerInfo.guild_name);
        res.GetRecord(playerInfo.name);
        playerRankInfo.list.push_back(playerInfo);
    }

    RANKING_ACK rankingAck;
    rankingAck.rank_type = RANKING_TYPE_ALL_FIRST;
    rankingAck.rank_list = playerRankInfo;

    LoadMaxPage(playerID, rankingAck, "ranking_all_first");
}

void RankingLoaderJob::LoadPlayerListByBattlePowerRanking(uint64 playerID, std::vector<int32>& rankList)
{
    std::string txt = "SELECT `rank`,`sex`,`level`,`career`,`guild_name`,`battle_power`,`player_name` FROM `ranking_battle_power` WHERE ";
    txt += "rank IN " + GetRankListTxt(rankList);

    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadPlayerListByBattlePowerRanking, playerID, txt.c_str());
}

void RankingLoaderJob::CBLoadPlayerListByBattlePowerRanking(DBRecorder& res, uint64 playerID)
{
    int32 rank = 0;
    RANKING_LIST playerRankInfo;
    RANKING_PLAYER_INFO playerInfo;

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        playerInfo.Clear();
        res.GetRecord(playerInfo.rank);
        res.GetRecord(playerInfo.sex);
        res.GetRecord(playerInfo.level);
        res.GetRecord(playerInfo.career);
        res.GetRecord(playerInfo.guild_name);
        res.GetRecord(playerInfo.num);
        res.GetRecord(playerInfo.name);

        playerRankInfo.list.push_back(playerInfo);
    }

    RANKING_ACK rankingAck;
    rankingAck.rank_type = RANKING_TYPE_BATTLE_POWER;
    rankingAck.rank_list = playerRankInfo;

    LoadMaxPage(playerID, rankingAck, "ranking_battle_power");
}

void RankingLoaderJob::LoadPlayerListByHoroscopeRanking(uint64 playerID, std::vector<int32>& rankList)
{
    std::string txt = "SELECT `rank`,`sex`,`level`,`career`,`guild_name`,`horoscope_level`,`player_name` FROM `ranking_horoscope` WHERE ";
    txt += "rank IN " + GetRankListTxt(rankList);

    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadPlayerListByHoroscopeRanking, playerID, txt.c_str());
}

void RankingLoaderJob::CBLoadPlayerListByHoroscopeRanking(DBRecorder& res, uint64 playerID)
{
    Player* player = 0;
    int32 rank = 0;
    RANKING_LIST playerRankInfo;
    RANKING_PLAYER_INFO playerInfo;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        res.GetRecord(playerInfo.rank);
        res.GetRecord(playerInfo.sex);
        res.GetRecord(playerInfo.level);
        res.GetRecord(playerInfo.career);
        res.GetRecord(playerInfo.guild_name);
        res.GetRecord(playerInfo.num);
        res.GetRecord(playerInfo.name);
        playerRankInfo.list.push_back(playerInfo);
    }

    RANKING_ACK rankingAck;
    rankingAck.rank_type = RANKING_TYPE_HOROSCOPE;
    rankingAck.rank_list = playerRankInfo;

    LoadMaxPage(playerID, rankingAck, "ranking_horoscope");
}

void RankingLoaderJob::LoadPlayerListByGamepointRanking(uint64 playerID, std::vector<int32>& rankList)
{
    std::string txt = "SELECT `rank`,`sex`,`level`,`career`,`guild_name`,`gamepoint`,`player_name` FROM `gamepoint_ranking` WHERE ";
    txt += "rank IN " + GetRankListTxt(rankList);

    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadPlayerListByGamepointRanking, playerID, txt.c_str());
}

void RankingLoaderJob::CBLoadPlayerListByGamepointRanking(DBRecorder& res, uint64 playerID)
{
    Player* player = 0;
    int32 rank = 0;
    RANKING_LIST playerRankInfo;
    RANKING_PLAYER_INFO playerInfo;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        res.GetRecord(playerInfo.rank);
        res.GetRecord(playerInfo.sex);
        res.GetRecord(playerInfo.level);
        res.GetRecord(playerInfo.career);
        res.GetRecord(playerInfo.guild_name);
        res.GetRecord(playerInfo.num);
        res.GetRecord(playerInfo.name);
        playerRankInfo.list.push_back(playerInfo);
    }

    RANKING_ACK rankAck;
    rankAck.rank_type = RANKING_TYPE_GAME_POINT;
    rankAck.rank_list = playerRankInfo;

    LoadMaxPage(playerID, rankAck, "gamepoint_ranking");
}

void RankingLoaderJob::LoadPlayerListByPlayerLevelRank(uint64 playerID)
{
    Player* player = 0;
    int32 rank = 0;
    RANKING_LIST playerRankInfo;
    RANKING_PLAYER_INFO playerInfo;
    RANKING_ACK rankAck;
    // 默认排名是0
    rankAck.my_ranking = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    DBRecorder res;
    res = m_dbExecutor->Query("SELECT rank.player_id,rank.rank,rank.sex,rank.level,rank.career,rank.guild_name,rank.battle_power,rank.player_name,head.head_portrait_open FROM ranking_player_level AS rank LEFT JOIN head_portrait AS head ON head.player_id = rank.player_id ORDER BY rank.rank ASC limit 100");
    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        uint64 playerId = 0;
        std::string head_str;

        res.GetRecord(playerId);
        res.GetRecord(playerInfo.rank);
        res.GetRecord(playerInfo.sex);
        res.GetRecord(playerInfo.level);
        res.GetRecord(playerInfo.career);
        res.GetRecord(playerInfo.guild_name);
        res.GetRecord(playerInfo.battle_power);
        res.GetRecord(playerInfo.name);
        res.GetRecord(head_str);
        
        if (!head_str.empty() && !JsonToStruct(playerInfo.head_portrait, head_str))
        {
            CnError("Parse player:%llu head portrait failed\n", playerID);
        }

        // 查找我的排名
        if(playerId == player->GetPlayerID())
        {
            rankAck.my_ranking = playerInfo.rank;
        }
        
        // 生成排名列表
        playerRankInfo.list.push_back(playerInfo);
    }
    rankAck.rank_type = RANKING_TYPE_PLAYER_LEVEL;
    rankAck.rank_list = playerRankInfo;

    res = m_dbExecutor->Query("SELECT UNIX_TIMESTAMP(`execute_time`) FROM `ranking_schedule` WHERE ranking_type = %d",RANKING_TYPE_PLAYER_LEVEL);
    int64 execute_time = 0;
    if(res.GetRowNum() > 0)
    {
        res.GetRecord(execute_time);
        // 设置排名刷新时间
        rankAck.refresh_time = (uint32)execute_time;
    }

    RankListManager::Instance()->SendRankInfoToGate(playerID, rankAck);


  /*  std::string txt = "SELECT `rank`,`sex`,`level`,`career`,`guild_name`,`battle_power`,`player_name` FROM `ranking_player_level` WHERE ";
    txt += "rank IN " + GetRankListTxt(rankList);

    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadPlayerListByPlayerLevelRanking, playerID, txt.c_str());*/
}

void RankingLoaderJob::LoadPlotDungonScoreRank(uint64 playerID)
{
    Player* player = 0;
    int32 rank = 0;
    RANKING_LIST playerRankInfo;
    RANKING_PLAYER_INFO playerInfo;
    RANKING_ACK rankAck;
    // 默认排名是0
    rankAck.my_ranking = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    DBRecorder res;
    res = m_dbExecutor->Query("SELECT rank.player_id,rank.rank,rank.sex,rank.level,rank.career,rank.guild_name,rank.player_name,rank.score,head.head_portrait_open FROM `ranking_dungeon_score` AS rank LEFT JOIN head_portrait AS head ON head.player_id = rank.player_id ORDER BY rank.rank ASC limit 100");
    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        uint64 playerId = 0;
        std::string head_str;

        res.GetRecord(playerId);
        res.GetRecord(playerInfo.rank);
        res.GetRecord(playerInfo.sex);
        res.GetRecord(playerInfo.level);
        res.GetRecord(playerInfo.career);
        res.GetRecord(playerInfo.guild_name);
        res.GetRecord(playerInfo.name);
        res.GetRecord(playerInfo.num);
        res.GetRecord(head_str);

        if (!JsonToStruct(playerInfo.head_portrait, head_str))
        {
            CnError("Parse player:%llu head portrait failed\n", playerID);
        }

        // 查找我的排名
        if(playerId == player->GetPlayerID())
        {
            rankAck.my_ranking = playerInfo.rank;
        }

        // 生成排名列表
        playerRankInfo.list.push_back(playerInfo);
    }
    rankAck.rank_type = RANKING_TYPE_PLOT_DUNGON_SCORE;
    rankAck.rank_list = playerRankInfo;

    res = m_dbExecutor->Query("SELECT UNIX_TIMESTAMP(`execute_time`) FROM `ranking_schedule` WHERE ranking_type = %d",RANKING_TYPE_PLOT_DUNGON_SCORE);
    int64 execute_time = 0;
    if(res.GetRowNum() > 0)
    {
        res.GetRecord(execute_time);
        // 设置排名刷新时间
        rankAck.refresh_time = (uint32)execute_time;
    }

    RankListManager::Instance()->SendRankInfoToGate(playerID, rankAck);
}

void RankingLoaderJob::LoadAchiveCountRank(uint64 playerID)
{
    Player* player = 0;
    int32 rank = 0;
    RANKING_LIST playerRankInfo;
    RANKING_PLAYER_INFO playerInfo;
    RANKING_ACK rankAck;
    // 默认排名是0
    rankAck.my_ranking = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    DBRecorder res;
    res = m_dbExecutor->Query("SELECT rank.player_id,rank.rank,rank.sex,rank.level,rank.career,rank.guild_name,rank.player_name,rank.count,head.head_portrait_open FROM `ranking_achive` AS rank LEFT JOIN head_portrait AS head ON head.player_id = rank.player_id ORDER BY rank.rank ASC limit 100");
    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        uint64 playerId = 0;
        std::string head_str;

        res.GetRecord(playerId);
        res.GetRecord(playerInfo.rank);
        res.GetRecord(playerInfo.sex);
        res.GetRecord(playerInfo.level);
        res.GetRecord(playerInfo.career);
        res.GetRecord(playerInfo.guild_name);
        res.GetRecord(playerInfo.name);
        res.GetRecord(playerInfo.num);
        res.GetRecord(head_str);

        if (!JsonToStruct(playerInfo.head_portrait, head_str))
        {
            CnError("Parse player:%llu head portrait failed\n", playerID);
        }

        // 查找我的排名
        if(playerId == player->GetPlayerID())
        {
            rankAck.my_ranking = playerInfo.rank;
        }

        // 生成排名列表
        playerRankInfo.list.push_back(playerInfo);
    }
    rankAck.rank_type = RANKING_TYPE_ACHIVE_COUNT;
    rankAck.rank_list = playerRankInfo;

    res = m_dbExecutor->Query("SELECT UNIX_TIMESTAMP(`execute_time`) FROM `ranking_schedule` WHERE ranking_type = %d",RANKING_TYPE_ACHIVE_COUNT);
    int64 execute_time = 0;
    if(res.GetRowNum() > 0)
    {
        res.GetRecord(execute_time);
        // 设置排名刷新时间
        rankAck.refresh_time = (uint32)execute_time;
    }

    RankListManager::Instance()->SendRankInfoToGate(playerID, rankAck);
}

void RankingLoaderJob::CBLoadPlayerListByPlayerLevelRanking(DBRecorder& res, uint64 playerID)
{
  /*  Player* player = 0;
    int32 rank = 0;
    RANKING_LIST playerRankInfo;
    RANKING_PLAYER_INFO playerInfo;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        res.GetRecord(playerInfo.rank);
        res.GetRecord(playerInfo.sex);
        res.GetRecord(playerInfo.level);
        res.GetRecord(playerInfo.career);
        res.GetRecord(playerInfo.guild_name);
        res.GetRecord(playerInfo.battle_power);
        res.GetRecord(playerInfo.name);
        playerRankInfo.list.push_back(playerInfo);
    }

    RANKING_ACK rankAck;
    rankAck.rank_type = RANKING_TYPE_PLAYER_LEVEL;
    rankAck.rank_list = playerRankInfo;

    LoadMaxPage(playerID, rankAck, "ranking_player_level");*/
}

void RankingLoaderJob::LoadPlayerOfflineOldRankingByPlayerID(uint64 playerID, std::vector<uint64>& playerIDList, RANKING_RANK_ACK& rankingAck, const std::string rankTxt)
{
    std::string txt = "SELECT `old_rank`,`player_id` FROM " + rankTxt + " WHERE ";
    txt += "player_id IN " + GetIDListTxt(playerIDList);

    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadPlayerOfflineOldRankingByPlayerID, playerID, rankingAck, rankTxt, txt.c_str());
}

void RankingLoaderJob::CBLoadPlayerOfflineOldRankingByPlayerID(DBRecorder& res, uint64 playerID, RANKING_RANK_ACK rankingAck, const std::string rankTxt)
{
    Json::Reader reader;

    uint64 playerIt = 0;
    PLAYER_RANK_INFO playerRankInfo;

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        res.GetRecord(playerRankInfo.rank);
        res.GetRecord(playerIt);
        playerRankInfo.player_id.id_h = U64ID_HIGH(playerIt);
        playerRankInfo.player_id.id_l = U64ID_LOW(playerIt);
        rankingAck.playerList.push_back(playerRankInfo);
    }
    RankListManager::Instance()->SendRankInfo(playerID, rankingAck);
}

void RankingLoaderJob::LoadPlayerIDByPlayerOfflineRanking(uint64 playerID, uint32 sessionID, std::vector<int32>& rankList, uint8 reason)
{
    std::string txt = "SELECT `rank`,`player_id` FROM ranking_offline_player WHERE ";
    txt += "rank IN " + GetRankListTxt(rankList);

    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadPlayerIDByPlayerOfflineRanking, playerID, sessionID, reason, txt.c_str());
}

void RankingLoaderJob::CBLoadPlayerIDByPlayerOfflineRanking(DBRecorder& res, uint64 playerID, uint32 sessionID, uint8 reason)
{
    Json::Reader reader;

    uint64 playerIt = 0;
    int32 rank = 0;
    std::map<uint64, int32> playerRankInfo;
    playerRankInfo.clear();

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        res.GetRecord(rank);
        res.GetRecord(playerIt);
        playerRankInfo[playerIt] = rank;
    }
    uint8 rank_type = 0;
    rank_type = RANKING_TYPE_OFFLINE_BATTLE;

    RankListManager::Instance()->SendPlayerInfo(playerID, sessionID, playerRankInfo, rank_type, reason);
}

void RankingLoaderJob::ChangeTwoPlayerRanking(uint64 playerID, uint32 sessionID, uint64 player1, uint64 player2, int32 level1, int32 level2)
{
    if(player1 == player2)
    {
        CnAssert(false);
        return;
    }

    m_dbExecutor->Query(this, &RankingLoaderJob::CBChangeTwoPlayerRanking, playerID, sessionID,
        "UPDATE `ranking_offline_player` AS T1 JOIN `ranking_offline_player` AS T2 "
        "ON (T1.player_id=%llu AND T2.player_id=%llu) OR (T1.player_id=%llu AND T2.player_id=%llu) "
        "SET T1.rank=T2.rank, T2.rank=T1.rank, T1.level=%d, T2.level=IF(%d>0,%d,T2.level)"
        , player1, player2, player2, player1, level1, level2, level2);
}

void RankingLoaderJob::CBChangeTwoPlayerRanking(DBRecorder& res, uint64 playerID, uint32 sessionID)
{
    RANKING_CHANGE_ACK sendMsg;
    sendMsg.rank_type = RANKING_TYPE_OFFLINE_BATTLE;

    if(!res.HasExecut())
    {
        sendMsg.err_code.errcode = ERR_FAILED;
    }
    else
    {
        sendMsg.err_code.errcode = ERR_SUCCEED;
    }

    GameHandler::Instance()->SendByID(GameHandler::Instance()->GetRegionIDBySessionID(sessionID), MSG_RANKING_CHANGE_ACK, &sendMsg, playerID);
}

void RankingLoaderJob::LoadPlayerListByOfflineRanking(uint64 playerID, std::vector<int32>& rankList)
{
    std::string txt = "SELECT `rank`,`sex`,`level`,`career`,`guild_name`,`battle_power`,`player_name` FROM ranking_offline_player WHERE ";
    txt += "rank IN " + GetRankListTxt(rankList);
 
    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadPlayerListByPlayerOfflineRanking, playerID, txt.c_str());
}

void RankingLoaderJob::LoadMaxPage(uint64 playerID, RANKING_ACK& rankingAck, const std::string rankTxt)
{
    std::string txt = "SELECT COUNT(player_id) FROM ";
    txt = txt + rankTxt;

    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadMaxPage, playerID, rankingAck, rankTxt, txt.c_str());
}

void RankingLoaderJob::CBLoadMaxPage(DBRecorder& res, uint64 playerID, RANKING_ACK rankingAck, const std::string rankTxt)
{
    int64 max = 0;

    if(res.GetRowNum() < 1)
    {
        return;
    }

    res.GetRecord(max);

    int32 page = (int32)max / RANKING_PAGE_LINE_NUM;
    page++;

    rankingAck.max_page = page;
    LoadMyRanking(playerID, rankingAck, rankTxt);
}

void RankingLoaderJob::LoadMyRanking(uint64 playerID, RANKING_ACK rankingAck, const std::string rankTxt)
{
    std::string txt = "SELECT rank FROM ";

    if(rankingAck.rank_type == RANKING_TYPE_ALL_FIRST)
        txt = "SELECT rank_type FROM ";

    txt = txt + rankTxt + " WHERE player_id=%llu";

    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadMyRanking, playerID, rankingAck, rankTxt, txt.c_str(), playerID);
}

void RankingLoaderJob::CBLoadMyRanking(DBRecorder& res, uint64 playerID, RANKING_ACK rankingAck, const std::string rankTxt)
{
    if(res.GetRowNum() > 0)
    {
        res.GetRecord(rankingAck.my_ranking);
    }

    LoadRankingTime(playerID, rankingAck);
}

void RankingLoaderJob::LoadRankingTime(uint64 playerID, RANKING_ACK rankingAck)
{
    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadRankingTime, playerID, rankingAck,
        "SELECT UNIX_TIMESTAMP(`execute_time`) FROM `ranking_schedule` WHERE `ranking_type`=%d", rankingAck.rank_type);
}

void RankingLoaderJob::CBLoadRankingTime(DBRecorder& res, uint64 playerID, RANKING_ACK rankingAck)
{
    int64 execute_time = 0;
    if(res.GetRowNum() > 0)
    {
        res.GetRecord(execute_time);
        rankingAck.refresh_time = (uint32)execute_time;
    }

    RankListManager::Instance()->SendRankInfoToGate(playerID, rankingAck);
}

void RankingLoaderJob::LoadGuildMaxPage(uint64 playerID, RANKING_ACK& rankingAck, const std::string rankTxt)
{
    std::string txt = "SELECT COUNT(guild_id) FROM ";
    txt = txt + rankTxt;
    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadGuildMaxPage, playerID, rankingAck, rankTxt, txt.c_str());
}

void RankingLoaderJob::CBLoadGuildMaxPage(DBRecorder& res, uint64 playerID, RANKING_ACK rankingAck, const std::string rankTxt)
{
    int64 max = 0;

    if(res.GetRowNum() < 1)
    {
        return;
    }

    res.GetRecord(max);

    int32 page = (int32)max / RANKING_PAGE_LINE_NUM;
    page++;

    rankingAck.max_page = page;
    LoadMyGuildRanking(playerID, rankingAck, rankTxt);
}

void RankingLoaderJob::LoadMyGuildRanking(uint64 playerID, RANKING_ACK& rankingAck, const std::string rankTxt)
{
    Player* pPlayer = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!pPlayer)
    {
        return;
    }

    std::string txt = "SELECT rank FROM ";
    txt = txt + rankTxt + " WHERE guild_id=%llu";

    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadMyGuildRanking, playerID, rankingAck, rankTxt, txt.c_str(), pPlayer->GetGuildID());
}

void RankingLoaderJob::CBLoadMyGuildRanking(DBRecorder& res, uint64 playerID, RANKING_ACK rankingAck, const std::string rankTxt)
{
    if(res.GetRowNum() < 1)
    {
        rankingAck.my_ranking = 0;
    }
    else
    {
        res.GetRecord(rankingAck.my_ranking);
    }

    LoadRankingTime(playerID, rankingAck);
}


void RankingLoaderJob::CBLoadPlayerListByPlayerOfflineRanking(DBRecorder& res, uint64 playerID)
{
    Player* player = 0;
    int32 rank = 0;
    RANKING_LIST playerRankInfo;
    RANKING_PLAYER_INFO playerInfo;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        res.GetRecord(playerInfo.rank);
        res.GetRecord(playerInfo.sex);
        res.GetRecord(playerInfo.level);
        res.GetRecord(playerInfo.career);
        res.GetRecord(playerInfo.guild_name);
        res.GetRecord(playerInfo.num);
        res.GetRecord(playerInfo.name);
        playerRankInfo.list.push_back(playerInfo);
    }

    RANKING_ACK rankAck;
    rankAck.rank_type = RANKING_TYPE_OFFLINE_BATTLE;
    rankAck.rank_list = playerRankInfo;

    LoadMaxPage(playerID, rankAck, "ranking_offline_player");
}

// 删除公会BOSS之战的排行信息
void RankingLoaderJob::DelGuildCrusadeBossRank(GUILD_DAMAGE_RANK_LIST& list)
{
    uint64 guildID = 0;

    guildID = U64ID(list.guild_id.id_h, list.guild_id.id_l);

    m_dbExecutor->Query(this, &RankingLoaderJob::CBDelGuildCrusadeBossRank, list,
        "DELETE FROM crusade_boss_ranking WHERE guild_id=%llu", guildID);
}

// 删除公会暮光之战的排行信息
void RankingLoaderJob::DelGuildTwilightWarRank(GUILD_DAMAGE_RANK_LIST& list)
{
    m_dbExecutor->Query(this, &RankingLoaderJob::CBDelGuildTwilightWarRank, list,"DELETE FROM ranking_twilight_war");
}

// 保存公会BOSS之战的排行信息
void RankingLoaderJob::SaveGuildCrusadeBossRank(GUILD_DAMAGE_RANK_LIST list)
{
    DBRecorder recorder;
    std::stringstream ss;
    char name[256] = {0};
    uint64 guildID = 0;

    guildID = U64ID(list.guild_id.id_h, list.guild_id.id_l);

    if(list.damage_list.size() < 1) 
        return;

    ss << "INSERT INTO `crusade_boss_ranking` ";

    for(uint32 i=0; i<list.damage_list.size(); ++i)
    {
        if(i == 0)
        {
            ss << "VALUES(";
        }
        else
        {
            ss << ",(";
        }

        ss << list.damage_list[i].rank;
        ss << ",";
        ss << guildID;
        ss << ",'";
        m_dbExecutor->EscapeString(name, list.damage_list[i].name.c_str(), list.damage_list[i].name.length());
        ss << name;
        ss << "',";
        ss << list.damage_list[i].damage;
        ss << ",";
        ss << list.damage_list[i].level;
        ss << ")";
    }

    m_dbExecutor->Query(ss.str().c_str());
}

// 保存公会暮光之战的排行信息
void RankingLoaderJob::SaveGuildTwilightWarRank(GUILD_DAMAGE_RANK_LIST& list)
{
    DBRecorder recorder;
    std::stringstream ss;

    char name[256] = {0};

    if(list.damage_list.size() < 1) 
        return;

    ss << "INSERT INTO `ranking_twilight_war` ";

    for(uint32 i=0; i<list.damage_list.size(); ++i)
    {
        if(i == 0)
        {
            ss << "VALUES(";
        }
        else
        {
            ss << ",(";
        }

        ss << list.damage_list[i].rank;
        ss << ",'";
        m_dbExecutor->EscapeString(name, list.damage_list[i].name.c_str(), list.damage_list[i].name.length());
        ss << name;
        ss << "',";
        ss << list.damage_list[i].damage;
        ss << ",";
        ss << list.damage_list[i].level;
        ss << ")";
    }

    m_dbExecutor->Query(ss.str().c_str());
}

// 获取暮光之战排行信息
void RankingLoaderJob::GetGuildTwilightwarRank(uint64 playerID)
{
    m_dbExecutor->Query(this, &RankingLoaderJob::CBGetGuildTwilightWarRank, playerID, 
        "SELECT `rank`, `guild_name`, `damage`, `level` FROM ranking_twilight_war");
}

// 获得公会BOSS排行
void RankingLoaderJob::GetGuildCrusadeBossRank(uint64 playerID)
{
    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    if(!player->GetGuildID())
    {
        return;
    }

    m_dbExecutor->Query(this, &RankingLoaderJob::CBGetGuildCrusadeBossRank, playerID, 
        "SELECT `rank`, `player_name`, `damage`, `level` FROM crusade_boss_ranking WHERE guild_id=%llu", player->GetGuildID());
}

void RankingLoaderJob::GetTopPlayerLevelRank(uint32 count)
{
    char buff[100];
    memset(buff, 0, 100);
    Crown::SDItoa(buff, 100, count);
    std::string txt = "SELECT `rank`, `player_id`, `level` FROM `ranking_player_level` ORDER BY `level` DESC LIMIT ";
    txt += buff;

    m_dbExecutor->Query(this, &RankingLoaderJob::CBGetTopPlayerLevelRank, txt.c_str());
}

void RankingLoaderJob::LoadPlayerRankingByPlayerID(uint64 playerID, std::vector<uint64>& playerIDList, RANKING_RANK_ACK& rankingAck, const std::string rankTxt)
{
    std::string txt = "SELECT `rank`,`player_id` FROM " + rankTxt + " WHERE ";
    txt += "player_id IN " + GetIDListTxt(playerIDList);

    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadPlayerRankingByPlayerID, playerID, rankingAck, rankTxt, txt.c_str());
}

void RankingLoaderJob::LoadGuildRankingByGuildID(uint64 playerID, std::vector<uint64>& guildIDList, RANKING_RANK_ACK& rankingAck, const std::string rankTxt)
{
    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
        return;

    guildIDList.clear();

    guildIDList.push_back( player->GetGuildID());

    std::string txt = "SELECT `rank` FROM " + rankTxt + " WHERE guild_id IN"  + GetIDListTxt(guildIDList) ;
    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadGuildRanking, playerID, rankingAck, txt.c_str());
}

void RankingLoaderJob::LoadPlayerListByAshuraRank(uint64 playerID, std::vector<int32>& rankList)
{
    std::string txt = "SELECT `rank`,`sex`,`player_name`,`level`,`career`,`kill_count`,`guild_name` FROM `ashura_rank` WHERE ";
    txt += "rank IN " + GetRankListTxt(rankList);

    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadPlayerListByAshuraRank, playerID, txt.c_str());
}

void RankingLoaderJob::CBLoadPlayerListByAshuraRank(DBRecorder& res, uint64 playerID)
{
    Player* player = 0;
    int32 rank = 0;
    RANKING_LIST playerRankInfo;
    RANKING_PLAYER_INFO playerInfo;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        res.GetRecord(playerInfo.rank);
        res.GetRecord(playerInfo.sex);
        res.GetRecord(playerInfo.name);
        res.GetRecord(playerInfo.level);
        res.GetRecord(playerInfo.career);
        res.GetRecord(playerInfo.num);
        res.GetRecord(playerInfo.guild_name);
        playerRankInfo.list.push_back(playerInfo);
    }

    RANKING_ACK rankAck;
    rankAck.rank_type = RANKING_TYPE_ASHURA;
    rankAck.rank_list = playerRankInfo;

    LoadMaxPage(playerID, rankAck, "ashura_rank");
}

void RankingLoaderJob::LoadPlayerListByGuildRank(uint64 playerID, std::vector<int32>& rankList)
{
    std::string txt = "SELECT `rank`,`guild_name`,`level`,`number_count`,`battle_power` FROM `guild_ranking` WHERE ";
    txt += "rank IN " + GetRankListTxt(rankList);

    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadPlayerListByGuildRank, playerID, txt.c_str());
}

void RankingLoaderJob::CBLoadPlayerListByGuildRank(DBRecorder& res, uint64 playerID)
{
    Player* player = NULL;
    int32 rank = 0;
    RANKING_LIST playerRankInfo;
    RANKING_PLAYER_INFO playerInfo;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        res.GetRecord(playerInfo.rank);
        res.GetRecord(playerInfo.guild_name);
        res.GetRecord(playerInfo.level);
        res.GetRecord(playerInfo.num);
        res.GetRecord(playerInfo.battle_power);
        playerRankInfo.list.push_back(playerInfo);
    }

    RANKING_ACK rankAck;
    rankAck.rank_type = RANKING_TYPE_GUILD;
    rankAck.rank_list = playerRankInfo;

    LoadGuildMaxPage(playerID, rankAck, "guild_ranking");
}

// 删除公会BOSS之战排行回调
void RankingLoaderJob::CBDelGuildCrusadeBossRank(DBRecorder& res, GUILD_DAMAGE_RANK_LIST list)
{
    SaveGuildCrusadeBossRank(list);
}

// 删除公会暮光之战排行回调
void RankingLoaderJob::CBDelGuildTwilightWarRank(DBRecorder& res, GUILD_DAMAGE_RANK_LIST list)
{
    SaveGuildTwilightWarRank(list);
}

// 获得暮光之战排行信息
void RankingLoaderJob::CBGetGuildTwilightWarRank(DBRecorder& res, uint64 playerID)
{
    GUILD_BOSS_DAMAGE_INFO damageInfo;
    GUILD_DAMAGE_RANK_LIST sendMsg;

    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        res.GetRecord(damageInfo.rank);
        res.GetRecord(damageInfo.name);
        res.GetRecord(damageInfo.damage);
        res.GetRecord(damageInfo.level);
        sendMsg.damage_list.push_back(damageInfo);
    }

    player->SendToGate(MSG_GUILD_TWILIGHT_WAR_RANK_ACK, &sendMsg);
}

// 获得公会BOSS排行信息
void RankingLoaderJob::CBGetGuildCrusadeBossRank(DBRecorder& res, uint64 playerID)
{
    GUILD_BOSS_DAMAGE_INFO damageInfo;
    GUILD_DAMAGE_RANK_LIST sendMsg;

    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        res.GetRecord(damageInfo.rank);
        res.GetRecord(damageInfo.name);
        res.GetRecord(damageInfo.damage);
        res.GetRecord(damageInfo.level);
        sendMsg.damage_list.push_back(damageInfo);
    }

    player->SendToGate(MSG_GUILD_CRUSADE_BOSS_RANK_ACK, &sendMsg);
}

// 由玩家ID得到玩家排名发给GameServer
void RankingLoaderJob::CBLoadPlayerRankingByPlayerID(DBRecorder& res, uint64 playerID, RANKING_RANK_ACK rankingAck, const std::string rankTxt)
{
    Json::Reader reader;

    uint64 playerIt = 0;

    PLAYER_RANK_INFO playerRankInfo;

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        playerRankInfo.Clear();
        res.GetRecord(playerRankInfo.rank);
        res.GetRecord(playerIt);
        playerRankInfo.player_id.id_h = U64ID_HIGH(playerIt);
        playerRankInfo.player_id.id_l = U64ID_LOW(playerIt);
        rankingAck.playerList.push_back(playerRankInfo);
    }

    RankListManager::Instance()->SendRankInfo(playerID, rankingAck);
}


//公会排名
void RankingLoaderJob::CBLoadGuildRanking(DBRecorder& res, uint64 playerID ,RANKING_RANK_ACK rankingAck)
{

    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    PLAYER_RANK_INFO playerRankInfo;
    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        playerRankInfo.Clear();
        res.GetRecord(playerRankInfo.rank);
        playerRankInfo.player_id.id_h = U64ID_HIGH(playerID);
        playerRankInfo.player_id.id_l = U64ID_LOW(playerID);
        rankingAck.playerList.push_back(playerRankInfo);
    }

    RankListManager::Instance()->SendRankInfo(playerID, rankingAck);
}

void RankingLoaderJob::CBGetTopPlayerLevelRank(DBRecorder& res)
{
    RANKING_TOP_RANK_NTF ack;
    ack.Clear();

    uint64 playerId = 0;
    TOP_RANK_INFO info;
    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        info.Clear();
        res.GetRecord(info.rank);
        res.GetRecord(playerId);
        res.GetRecord(info.player_level);
        info.player_id.id_h = U64ID_HIGH(playerId);
        info.player_id.id_l = U64ID_LOW(playerId);
        ack.rankList.push_back(info);
    }

    RankListManager::Instance()->OnQueryTopPlayerLevelRankAck(ack);
}

std::string RankingLoaderJob::GetRankListTxt(const std::vector<int32>& rankList)
{
    std::string rankListTxt = "(";
    for(uint32 i = 0; i < rankList.size(); ++i)
    {
        if(i == 0)
            rankListTxt += Crown::ToString(rankList[i]);
        else
            rankListTxt += ", " + Crown::ToString(rankList[i]);
    }
    return rankListTxt + ")";
}


std::string RankingLoaderJob::GetIDListTxt(const std::vector<uint64>& playerIDList)
{
    std::string rankListTxt = "(";
    for(uint32 i = 0; i < playerIDList.size(); ++i)
    {
        if(i == 0)
            rankListTxt += Crown::ToString(playerIDList[i]);
        else
            rankListTxt += ", " + Crown::ToString(playerIDList[i]);
    }
    return rankListTxt + ")";
}

void RankingLoaderJob::OnGetRefreshTime()
{
    DBRecorder recorder;
    int64 time = 0;

    recorder = m_dbExecutor->Query("SELECT UNIX_TIMESTAMP(`execute_time`) FROM `ranking_schedule` WHERE `ranking_type`=%d", RANKING_TYPE_BATTLEHALL);
    if(recorder.HasExecut() && (recorder.GetRowNum() > 0))
    {
        recorder.GetRecord(time);
        m_refreshTime = (uint32)time;
    }

}

bool RankingLoaderJob::OnSaveRefreshTime(int32 rankType, uint32 rankTime)
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("INSERT INTO ranking_schedule VALUES(%d,FROM_UNIXTIME(%u)) ON DUPLICATE KEY UPDATE execute_time=FROM_UNIXTIME(%u)", rankType, rankTime, rankTime);

    if(!recorder.HasExecut())
    {
        return false;
    }

    m_refreshTime = (uint32)rankTime;

    return true;
}

void RankingLoaderJob::LoadPlayerListByWingRanking( uint64 playerID, std::vector<int32>& rankList )
{
    std::string txt = "SELECT `rank`,`sex`,`level`,`career`,`guild_name`,`wing_battle_power`,`player_name` FROM `ranking_wing` WHERE ";
    txt += "rank IN " + GetRankListTxt(rankList);

    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadPlayerListByWingRanking, playerID, txt.c_str());
}

void RankingLoaderJob::CBLoadPlayerListByWingRanking( DBRecorder& res, uint64 playerID )
{
    Player* player = 0;
    int32 rank = 0;
    RANKING_LIST playerRankInfo;
    RANKING_PLAYER_INFO playerInfo;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        res.GetRecord(playerInfo.rank);
        res.GetRecord(playerInfo.sex);
        res.GetRecord(playerInfo.level);
        res.GetRecord(playerInfo.career);
        res.GetRecord(playerInfo.guild_name);
        res.GetRecord(playerInfo.battle_power);
        res.GetRecord(playerInfo.name);
        playerRankInfo.list.push_back(playerInfo);
    }

    RANKING_ACK rankingAck;
    rankingAck.rank_type = RANKING_TYPE_WING;
    rankingAck.rank_list = playerRankInfo;

    LoadMaxPage(playerID, rankingAck, "ranking_wing");
}

void RankingLoaderJob::LoadPlayerListByPetRanking( uint64 playerID, std::vector<int32>& rankList )
{
    std::string txt = "SELECT `rank`,`sex`,`level`,`career`,`guild_name`,`pet_battle_power`,`player_name` FROM `ranking_pet` WHERE ";
    txt += "rank IN " + GetRankListTxt(rankList);

    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadPlayerListByPetRanking, playerID, txt.c_str());
}

void RankingLoaderJob::CBLoadPlayerListByPetRanking( DBRecorder& res, uint64 playerID )
{
    Player* player = 0;
    int32 rank = 0;
    RANKING_LIST playerRankInfo;
    RANKING_PLAYER_INFO playerInfo;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        res.GetRecord(playerInfo.rank);
        res.GetRecord(playerInfo.sex);
        res.GetRecord(playerInfo.level);
        res.GetRecord(playerInfo.career);
        res.GetRecord(playerInfo.guild_name);
        res.GetRecord(playerInfo.battle_power);
        res.GetRecord(playerInfo.name);
        playerRankInfo.list.push_back(playerInfo);
    }

    RANKING_ACK rankingAck;
    rankingAck.rank_type = RANKING_TYPE_FOLLOW_PET;
    rankingAck.rank_list = playerRankInfo;

    LoadMaxPage(playerID, rankingAck, "ranking_pet");
}

void RankingLoaderJob::LoadPlayerListByRideRanking( uint64 playerID, std::vector<int32>& rankList )
{
    std::string txt = "SELECT `rank`,`sex`,`level`,`career`,`guild_name`,`ride_battle_power`,`player_name` FROM `ranking_ride` WHERE ";
    txt += "rank IN " + GetRankListTxt(rankList);

    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadPlayerListByRideRanking, playerID, txt.c_str());
}

void RankingLoaderJob::CBLoadPlayerListByRideRanking( DBRecorder& res, uint64 playerID )
{
    Player* player = 0;
    int32 rank = 0;
    RANKING_LIST playerRankInfo;
    RANKING_PLAYER_INFO playerInfo;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        res.GetRecord(playerInfo.rank);
        res.GetRecord(playerInfo.sex);
        res.GetRecord(playerInfo.level);
        res.GetRecord(playerInfo.career);
        res.GetRecord(playerInfo.guild_name);
        res.GetRecord(playerInfo.battle_power);
        res.GetRecord(playerInfo.name);
        playerRankInfo.list.push_back(playerInfo);
    }

    RANKING_ACK rankingAck;
    rankingAck.rank_type = RANKING_TYPE_RIDE_PET;
    rankingAck.rank_list = playerRankInfo;

    LoadMaxPage(playerID, rankingAck, "ranking_ride");
}

void RankingLoaderJob::LoadPlayerListByFashionRanking( uint64 playerID, std::vector<int32>& rankList )
{
    std::string txt = "SELECT `rank`,`sex`,`level`,`career`,`guild_name`,`fashion_battle_power`,`player_name` FROM `ranking_fashion` WHERE ";
    txt += "rank IN " + GetRankListTxt(rankList);

    m_dbExecutor->Query(this, &RankingLoaderJob::CBLoadPlayerListByFashionRanking, playerID, txt.c_str());
}

void RankingLoaderJob::CBLoadPlayerListByFashionRanking( DBRecorder& res, uint64 playerID )
{
    Player* player = 0;
    int32 rank = 0;
    RANKING_LIST playerRankInfo;
    RANKING_PLAYER_INFO playerInfo;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        res.GetRecord(playerInfo.rank);
        res.GetRecord(playerInfo.sex);
        res.GetRecord(playerInfo.level);
        res.GetRecord(playerInfo.career);
        res.GetRecord(playerInfo.guild_name);
        res.GetRecord(playerInfo.battle_power);
        res.GetRecord(playerInfo.name);
        playerRankInfo.list.push_back(playerInfo);
    }

    RANKING_ACK rankingAck;
    rankingAck.rank_type = RANKING_TYPE_FASHION;
    rankingAck.rank_list = playerRankInfo;

    LoadMaxPage(playerID, rankingAck, "ranking_ride");
}
