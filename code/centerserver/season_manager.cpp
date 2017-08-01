#include "centerserver_pch.h"
#include "season_manager.h"
#include "special_activity_job.h"
#include "global_db_operation.h"
#include "config_loader.h"
#include "player.h"
#include "game_handler.h"
#include "mail_manager.h"
#include "multserver_config.h"

IMPLEMENT_SINGLETON(SeasonManager)

SeasonManager::SeasonManager()
{
    m_season_info.Clear();
}

SeasonManager::~SeasonManager()
{

}

void SeasonManager::Init()
{
    GlobalDbOperation::Instance()->LoadSeasonInfo(m_season_info);
}

void SeasonManager::SaveDB()
{
    GlobalDbOperation::Instance()->SaveSeasonInfo(m_season_info);
}

void SeasonManager::Update()
{
    m_minute += g_ServerUpdateTime;
    if(m_minute < 60000)
        return;
    m_minute = 0;

    Crown::CSDDateTime nowDateTime = Crown::SDNow();
    if(nowDateTime.GetTimeValue() > m_season_info.season_end_time)
    {
        // 跨服服务器才进行赛季清理
        if(ConfigLoader::Instance()->IsCrossCenterSrv())
        {
            Crown::CSDDateTime nextMonthDateTime = nowDateTime;
#ifdef _DEBUG
            nextMonthDateTime.SetDateTime(nowDateTime.GetYear(), nowDateTime.GetMonth(), nowDateTime.GetDay(), nowDateTime.GetHour(), nowDateTime.GetMinute() >= 30 ? 30 : 0, 0);
#else
            nextMonthDateTime.SetDateTime(nowDateTime.GetYear(), nowDateTime.GetMonth(), 1, 0, 0, 0);
            nextMonthDateTime.IncMonth(1);

#endif // _DEBUG

            m_season_info.season_end_time = (uint32)nextMonthDateTime.GetTimeValue();

            GlobalDbOperation::Instance()->RefreshRank();

            // 清理赛季积分
            GlobalDbOperation::Instance()->ClearSeasonIntegral();
            GlobalDbOperation::Instance()->SaveSeasonInfo(m_season_info);

            // 新赛季通知
            SendNewSeasonNtf();

            // 发奖励
            SendReward();
        }
        else // 不是跨服服务器从跨服数据库取时间        由于没有跨服服务器通知只能这样取数据了
        {
            uint32 oldSeasonTime = m_season_info.season_end_time;
            if(GlobalDbOperation::Instance()->LoadSeasonInfo(m_season_info))
            {
                if(m_season_info.season_end_time > oldSeasonTime)
                {
                    // 新赛季通知
                    SendNewSeasonNtf();

                    // 发奖励
                    SendReward();
                }
            }
        }

        CnInfo("new season!\n");
    }
}

void SeasonManager::SendInfo(uint32 sessionID)
{
    uint32 regionID = 0;
    regionID = GameHandler::Instance()->GetRegionIDBySessionID(sessionID);
    MULTISERVER_BATTLE_SEASON_NTF sendMsg;
    sendMsg.season_time = m_season_info.season_end_time;

    GameHandler::Instance()->SendByID(regionID, MSG_MULTISERVER_BATTLE_SEASON_NTF, &m_season_info);
}

void SeasonManager::SendNewSeasonNtf()
{
    MULTISERVER_BATTLE_SEASON_NTF sendMsg;
    sendMsg.season_time = m_season_info.season_end_time;

    GameHandler::Instance()->SendToAll(MSG_MULTISERVER_BATTLE_SEASON_NTF, &sendMsg);
}

void SeasonManager::SendReward()
{
    std::string platformName = ConfigLoader::Instance()->GetPlatformName();
    int32 regionId = ConfigLoader::Instance()->GetRegionID();
    std::map<int32, std::string> rankMap;
    rankMap.clear();
    if(!GlobalDbOperation::Instance()->GetOldRankListInfo(platformName, regionId, rankMap))
    {
        CnError("season send reward error!\n");
        return;
    }

    REWARD_TABLE rewardTable;
    REWARD_ITEM rewardItem;
    for(std::map<int32, std::string>::iterator rankIt = rankMap.begin(); rankIt != rankMap.end(); ++rankIt)
    {
        rewardTable.Clear();
        int32 season_point = MultserverConfig::Instance()->GetRewardByRank(rankIt->first);
        rewardItem.Clear();
        //rewardItem.type = PROP_SEASON_POINT;
        rewardItem.num = season_point;
        rewardTable.items.push_back(rewardItem);
        MailManager::Instance()->SendMessageMail(0, rankIt->second, rewardTable.items, MAIL_MESSAGE_TYPE_SEASON_REWARD, 1, Crown::ToString(rankIt->first).c_str());
    }
}

void SeasonManager::RefreshRank()
{
    GlobalDbOperation::Instance()->RefreshRank();
}
