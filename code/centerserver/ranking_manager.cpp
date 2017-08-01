#include "centerserver_pch.h"
#include "ranking_manager.h"
#include "ranking_save_job.h"
#include "sdtime.h"

IMPLEMENT_SINGLETON(RankManager)

RankManager::RankManager()
{
    Clear();
}

RankManager::~RankManager()
{

}

void RankManager::Init()
{
    Clear();
    RankingSaveJob::Instance()->GetRankingRefreshTime(m_quarterHourRefreshTime, m_dailyRefreshTime);
}

void RankManager::Clear()
{
    m_dailyRefreshTime = 0;
    m_hourRefreshTime = 0;
    m_quarterHourRefreshTime = 0;
}

void RankManager::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_GMRANKING_REFRESH_REQ:
        OnRefreshRanking(msgID, data);
        break;
    case MSG_RANKING_OFFLINE_FIRST_CHANGE:
        SaveAllFirstOfRanking();
        break;
    default:
        break;
    }
}

void RankManager::Update()
{
    Crown::CSDDateTime nowTime;

    uint32 now =  (uint32)Crown::SDNow().GetTimeValue();
    
    // 每15分钟刷新一次
    if(now >= m_quarterHourRefreshTime)
    {
        // 获得当前时间
        nowTime = Crown::SDNow();
        // 将分钟置为15的倍数，秒置零
        nowTime.SetTime(nowTime.GetHour(), (nowTime.GetMinute()/15) * 15, 0);

        // 下次刷新时间
        nowTime.IncMinute(15);
        m_quarterHourRefreshTime = (uint32)nowTime.GetTimeValue();

        // 把当前刷新时间存入数据库
        SaveRanking(m_quarterHourRefreshTime);
    }

    // 每1小时刷新一次
    if(now >= m_hourRefreshTime)
    {
        //// 获得当前时间
        //nowTime = Crown::SDNow();
        //// 将分钟，秒置零
        //nowTime.SetTime(nowTime.GetHour(), 0, 0);

        //// 把当前刷新时间存入数据库
        //SaveRanking((uint32)nowTime.GetTimeValue());

        //// 加 1 小时, 下个小时刷新时间
        //nowTime.IncHour(1);
        //m_hourRefreshTime = (uint32)nowTime.GetTimeValue();
    }

    // 每天刷一次
    if(now >= m_dailyRefreshTime)
    {
        // 保存离线玩家旧数据
       /* if(RankingSaveJob::Instance()->SaveOldOfflinePlayerRank())
        {
            RankingSaveJob::Instance()->SaveRankUpdateTime(RANKING_TYPE_OFFLINE_BATTLE, (uint32)nowTime.GetTimeValue());
        }*/

        // 设置明天的更新时间
        nowTime = Crown::SDNow();
        nowTime.SetTime(0, 30, 0);
        m_dailyRefreshTime = (uint32)nowTime.GetTimeValue() + 86400;

        // 存档下一次刷新时间
        //RankingSaveJob::Instance()->SaveRankUpdateTime(RANKING_TYPE_DAILY_REFRESH, m_dailyRefreshTime);

        NONE sendMsg;
        //CenterHandler::Instance()->Send(MSG_RANKING_DAILY_REFRESH, &sendMsg);
    }
}

void RankManager::SaveRanking(uint32 rankTime)
{
    // 保存所有榜单信息进数据库
    SavePlayerLevelRanking(rankTime);
    SavePlotDungonScoreRanking(rankTime);
    SaveAchiveCountRanking(rankTime);

    // SaveGamepointRanking(rankTime);
    
    // SaveGuildRanking(rankTime);
    // SaveWingRanking(rankTime);

    // 必须在所有其他排行榜排完之后才进行排序
    //SaveAllFirstOfRanking();

    CnInfo("save ranking\n");

    // 通知CenterServer排行榜存档了,同时也表明战力排行榜重排了 [12/26/2013 ZhangHM]
    NONE sendMsg;
    //CenterHandler::Instance()->Send(MSG_RANKING_SAVED_NTF, &sendMsg);
}

void RankManager::OnRefreshRanking(uint32 msgID, void* data)
{
    Crown::CSDDateTime nowTime = Crown::SDNow();
    nowTime.SetTime(nowTime.GetHour(), 0, 0);
    SaveRanking((uint32)nowTime.GetTimeValue());
}

void RankManager::SaveAllFirstOfRanking()
{
    if(RankingSaveJob::Instance()->ClearAllFirstOfRanking())
    {
        RankingSaveJob::Instance()->SaveAllFirstOfRanking();
    }
}

void RankManager::SaveGamepointRanking(uint32 rankTime)
{
    if(RankingSaveJob::Instance()->ClearGamepointRank())
    {
        if(RankingSaveJob::Instance()->SaveGamepointRank())
        {
            RankingSaveJob::Instance()->SaveRankUpdateTime(RANKING_TYPE_GAME_POINT, rankTime);
        }
    }
}

void RankManager::SavePlayerLevelRanking(uint32 rankTime)
{
    if(RankingSaveJob::Instance()->ClearPlayerLevelRank())
    {
        if(RankingSaveJob::Instance()->SavePlayerLevelRank())
        {
            RankingSaveJob::Instance()->SaveRankUpdateTime(RANKING_TYPE_PLAYER_LEVEL, rankTime);
        }
    }
}

void RankManager::SaveGuildRanking(uint32 rankTime)
{
    if(RankingSaveJob::Instance()->ClearGuildRank())
    {
        if(RankingSaveJob::Instance()->SaveGuildRank())
        {
            RankingSaveJob::Instance()->SaveRankUpdateTime(RANKING_TYPE_GUILD, rankTime);
        }
    }
}

void RankManager::SaveWingRanking( uint32 rankTime )
{
    if(RankingSaveJob::Instance()->ClearWingBattlePowerRank())
    {
        if(RankingSaveJob::Instance()->SaveWingBattlePowerRank())
        {
            RankingSaveJob::Instance()->SaveRankUpdateTime(RANKING_TYPE_WING, rankTime);
        }
    }
}

void RankManager::SavePlotDungonScoreRanking(uint32 rankTime)
{
    if(RankingSaveJob::Instance()->SavePlotDungonScoreRank())
    {
        RankingSaveJob::Instance()->SaveRankUpdateTime(RANKING_TYPE_PLOT_DUNGON_SCORE, rankTime);
    }
}

void RankManager::SaveAchiveCountRanking(uint32 rankTime)
{
    if(RankingSaveJob::Instance()->SaveAchiveCountRank())
    {
        RankingSaveJob::Instance()->SaveRankUpdateTime(RANKING_TYPE_ACHIVE_COUNT, rankTime);
    }
}
