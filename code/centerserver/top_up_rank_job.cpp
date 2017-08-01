#include "centerserver_pch.h"
#include "top_up_rank_job.h"
#include "dbdriver.h"

IMPLEMENT_SINGLETON(TopUpRankJob)

TopUpRankJob::TopUpRankJob() : m_dbExecutor(NULL)
{
}

TopUpRankJob::~TopUpRankJob()
{
}

void TopUpRankJob::QueryRechargeRank(TOP_UP_RANK_QUERY_CREATE_REQ &req, TOP_UP_RANK_INFO_ACK &ack)
{
    DBRecorder res;
    char beginTime[MAX_SQLTXT_LEN] = {0};
    char endTime[MAX_SQLTXT_LEN] = {0};

    std::string queryBeginTime = Crown::CSDDateTime(req.begin_time).ToString();
    std::string queryTriggerTime = Crown::CSDDateTime(req.end_time).ToString();

    m_dbExecutor->EscapeString(beginTime, queryBeginTime.c_str(), queryBeginTime.length());
    m_dbExecutor->EscapeString(endTime, queryTriggerTime.c_str(), queryTriggerTime.length());

    res = m_dbExecutor->Query("SELECT player_id, player_name, SUM(coins) AS total_money "
        "FROM (SELECT player.player_id, player.player_name, kunlun_pay_order.coins, kunlun_pay_order.dtime "
        "       FROM player, kunlun_pay_order "
        "       WHERE player.region = kunlun_pay_order.serverid "
        "       AND player.platform_uid = kunlun_pay_order.uid "
        "       AND (kunlun_pay_order.dtime BETWEEN '%s' AND '%s') "
        "       ORDER BY player.player_name, kunlun_pay_order.dtime DESC) AS t "
        "GROUP BY player_id "
        "HAVING SUM(coins) >= %u "
        "ORDER BY total_money DESC, dtime",
        beginTime, endTime, req.total_recharge_line);

    if (!res.HasExecut())
    {
        CnWarn("Top up recharge rank query failed\n");
        return;
    }

    if (res.GetRowNum() > 0)
    {
        TOP_UP_RANK_INFO rankInfo;
        uint64 playerID;
        double value;

        for (uint32 i = 0; i < res.GetRowNum(); ++i)
        {
            res.GetRecord(playerID);
            rankInfo.player_id.id_h = U64ID_HIGH(playerID);
            rankInfo.player_id.id_l = U64ID_LOW(playerID);
            res.GetRecord(rankInfo.name);
            res.GetRecord(value);
            rankInfo.value = (uint32)value;
            ack.rank_list.push_back(rankInfo);
        }
    }

    ack.update_time = (uint32)Crown::SDNow().GetTimeValue();
}

void TopUpRankJob::QueryWingRank( TOP_UP_RANK_QUERY_CREATE_REQ &req, TOP_UP_RANK_INFO_ACK &ack )
{
    DBRecorder res;

    res = m_dbExecutor->Query("SELECT `player_id`, `rank`, `sex`, `career`, `player_name` FROM `ranking_wing` ORDER BY `rank` limit %d", req.total_recharge_line);

    if (!res.HasExecut())
    {
        CnWarn("Top up wing rank query failed\n");
        return;
    }

    if (res.GetRowNum() > 0)
    {
        TOP_UP_RANK_INFO rankInfo;
        uint64 playerID;
        int32 rank;

        for (uint32 i = 0; i < res.GetRowNum(); ++i)
        {
            res.GetRecord(playerID);
            rankInfo.player_id.id_h = U64ID_HIGH(playerID);
            rankInfo.player_id.id_l = U64ID_LOW(playerID);
            res.GetRecord(rank);
            rankInfo.value = (int32)rank;
            res.GetRecord(rankInfo.sex);
            res.GetRecord(rankInfo.career);
            res.GetRecord(rankInfo.name);

            ack.rank_list.push_back(rankInfo);
        }
    }
    ack.update_time = (uint32)Crown::SDNow().GetTimeValue();
}

void TopUpRankJob::QueryPetRank( TOP_UP_RANK_QUERY_CREATE_REQ &req, TOP_UP_RANK_INFO_ACK &ack )
{
    DBRecorder res;

    res = m_dbExecutor->Query("SELECT `player_id`, `rank`, `sex`, `career`, `player_name` FROM `ranking_pet` ORDER BY `rank` limit %d", req.total_recharge_line);

    if (!res.HasExecut())
    {
        CnWarn("Top up pet rank query failed\n");
        return;
    }

    if (res.GetRowNum() > 0)
    {
        TOP_UP_RANK_INFO rankInfo;
        uint64 playerID;
        int32 rank;

        for (uint32 i = 0; i < res.GetRowNum(); ++i)
        {
            res.GetRecord(playerID);
            rankInfo.player_id.id_h = U64ID_HIGH(playerID);
            rankInfo.player_id.id_l = U64ID_LOW(playerID);
            res.GetRecord(rank);
            rankInfo.value = (int32)rank;
            res.GetRecord(rankInfo.sex);
            res.GetRecord(rankInfo.career);
            res.GetRecord(rankInfo.name);
            ack.rank_list.push_back(rankInfo);
        }
    }
    ack.update_time = (uint32)Crown::SDNow().GetTimeValue();
}

void TopUpRankJob::QueryRideRank( TOP_UP_RANK_QUERY_CREATE_REQ &req, TOP_UP_RANK_INFO_ACK &ack )
{
    DBRecorder res;

    res = m_dbExecutor->Query("SELECT `player_id`, `rank`, `sex`, `career`, `player_name` FROM `ranking_ride` ORDER BY `rank` limit %d", req.total_recharge_line);

    if (!res.HasExecut())
    {
        CnWarn("Top up ride rank query failed\n");
        return;
    }

    if (res.GetRowNum() > 0)
    {
        TOP_UP_RANK_INFO rankInfo;
        uint64 playerID;
        int32 rank;

        for (uint32 i = 0; i < res.GetRowNum(); ++i)
        {
            res.GetRecord(playerID);
            rankInfo.player_id.id_h = U64ID_HIGH(playerID);
            rankInfo.player_id.id_l = U64ID_LOW(playerID);
            res.GetRecord(rank);
            rankInfo.value = (int32)rank;
            res.GetRecord(rankInfo.sex);
            res.GetRecord(rankInfo.career);
            res.GetRecord(rankInfo.name);
            ack.rank_list.push_back(rankInfo);
        }
    }
    ack.update_time = (uint32)Crown::SDNow().GetTimeValue();
}

void TopUpRankJob::QueryBattleRank( TOP_UP_RANK_QUERY_CREATE_REQ &req, TOP_UP_RANK_INFO_ACK &ack )
{
    DBRecorder res;

    res = m_dbExecutor->Query("SELECT `player_id`, `rank`, `sex`, `career`, `player_name` FROM `ranking_battle_power` ORDER BY `rank` limit %d", req.total_recharge_line);

    if (!res.HasExecut())
    {
        CnWarn("Top up battle rank query failed\n");
        return;
    }

    if (res.GetRowNum() > 0)
    {
        TOP_UP_RANK_INFO rankInfo;
        uint64 playerID;
        int32 rank;

        for (uint32 i = 0; i < res.GetRowNum(); ++i)
        {
            res.GetRecord(playerID);
            rankInfo.player_id.id_h = U64ID_HIGH(playerID);
            rankInfo.player_id.id_l = U64ID_LOW(playerID);
            res.GetRecord(rank);
            rankInfo.value = (int32)rank;
            res.GetRecord(rankInfo.sex);
            res.GetRecord(rankInfo.career);
            res.GetRecord(rankInfo.name);
            ack.rank_list.push_back(rankInfo);
        }
    }
    ack.update_time = (uint32)Crown::SDNow().GetTimeValue();
}

void TopUpRankJob::QueryLevelRank( TOP_UP_RANK_QUERY_CREATE_REQ &req, TOP_UP_RANK_INFO_ACK &ack )
{
    DBRecorder res;

    res = m_dbExecutor->Query("SELECT `player_id`, `rank`, `sex`, `career`, `player_name` FROM `ranking_player_level` ORDER BY `rank` limit %d", req.total_recharge_line);

    if (!res.HasExecut())
    {
        CnWarn("Top up level rank query failed\n");
        return;
    }

    if (res.GetRowNum() > 0)
    {
        TOP_UP_RANK_INFO rankInfo;
        uint64 playerID;
        int32 rank;

        for (uint32 i = 0; i < res.GetRowNum(); ++i)
        {
            res.GetRecord(playerID);
            rankInfo.player_id.id_h = U64ID_HIGH(playerID);
            rankInfo.player_id.id_l = U64ID_LOW(playerID);
            res.GetRecord(rank);
            rankInfo.value = (int32)rank;
            res.GetRecord(rankInfo.sex);
            res.GetRecord(rankInfo.career);
            res.GetRecord(rankInfo.name);
            ack.rank_list.push_back(rankInfo);
        }
    }
    ack.update_time = (uint32)Crown::SDNow().GetTimeValue();
}

void TopUpRankJob::QueryFashionRank( TOP_UP_RANK_QUERY_CREATE_REQ &req, TOP_UP_RANK_INFO_ACK &ack )
{
    DBRecorder res;

    res = m_dbExecutor->Query("SELECT `player_id`, `rank`, `sex`, `career`, `player_name` FROM `ranking_fashion` ORDER BY `rank` limit %d", req.total_recharge_line);

    if (!res.HasExecut())
    {
        CnWarn("Top up level rank query failed\n");
        return;
    }

    if (res.GetRowNum() > 0)
    {
        TOP_UP_RANK_INFO rankInfo;
        uint64 playerID;
        int32 rank;

        for (uint32 i = 0; i < res.GetRowNum(); ++i)
        {
            res.GetRecord(playerID);
            rankInfo.player_id.id_h = U64ID_HIGH(playerID);
            rankInfo.player_id.id_l = U64ID_LOW(playerID);
            res.GetRecord(rank);
            rankInfo.value = (int32)rank;
            res.GetRecord(rankInfo.sex);
            res.GetRecord(rankInfo.career);
            res.GetRecord(rankInfo.name);
            ack.rank_list.push_back(rankInfo);
        }
    }
    ack.update_time = (uint32)Crown::SDNow().GetTimeValue();
}

void TopUpRankJob::QueryOfflinePlayerRank( TOP_UP_RANK_QUERY_CREATE_REQ &req, TOP_UP_RANK_INFO_ACK &ack )
{
    DBRecorder res;

    res = m_dbExecutor->Query("SELECT `player_id`, `rank`, `sex`, `career`, `player_name` FROM `ranking_offline_player` ORDER BY `rank` limit %d", req.total_recharge_line);

    if (!res.HasExecut())
    {
        CnWarn("Top up level rank query failed\n");
        return;
    }

    if (res.GetRowNum() > 0)
    {
        TOP_UP_RANK_INFO rankInfo;
        uint64 playerID;
        int32 rank;

        for (uint32 i = 0; i < res.GetRowNum(); ++i)
        {
            res.GetRecord(playerID);
            rankInfo.player_id.id_h = U64ID_HIGH(playerID);
            rankInfo.player_id.id_l = U64ID_LOW(playerID);
            res.GetRecord(rank);
            rankInfo.value = (int32)rank;
            res.GetRecord(rankInfo.sex);
            res.GetRecord(rankInfo.career);
            res.GetRecord(rankInfo.name);
            ack.rank_list.push_back(rankInfo);
        }
    }
    ack.update_time = (uint32)Crown::SDNow().GetTimeValue();
}
