#include "centerserver_pch.h"
#include "global_db_operation.h"
#include "dbdriver.h"

#include "player.h"
#include "player_manager.h"
#include "player_net_handler.h"

#include "ranking_list_manager.h"
#include "ranking_loader_job.h"
#include "config_loader.h"

IMPLEMENT_SINGLETON(GlobalDbOperation)

GlobalDbOperation::~GlobalDbOperation(void)
{
}

void GlobalDbOperation::SaveChatInfo(const Chat_Info& chatinfo)
{
    DBRecorder recorder;

    char name[PLAYER_NAME_LEN*2+1] = {0};
    char context[CHAT_MAX_LENGTH*2+1] = {0};
    char plaform_name[PLAYER_NAME_LEN*2+1] = {0};

    int32 kk = chatinfo.context.size();

    if(CHAT_MAX_LENGTH < chatinfo.context.length())
    {
        return;
    }

    if(PLAYER_NAME_LEN < chatinfo.player_name.length())
    {
        return;
    }

    if(PLAYER_NAME_LEN < chatinfo.plaform_name.length())
    {
        return;
    }

    m_dbGlobalExecutor->EscapeString(name, chatinfo.player_name.c_str(), chatinfo.player_name.length());
    m_dbGlobalExecutor->EscapeString(context, chatinfo.context.c_str(), chatinfo.context.length());
    m_dbGlobalExecutor->EscapeString(plaform_name, chatinfo.plaform_name.c_str(), chatinfo.plaform_name.length());


    recorder = m_dbGlobalExecutor->Query("insert into `chat_info` set server_id =%d, time=CURRENT_TIMESTAMP, player_id =%llu,"
        "`player_name`='%s',chat_type=%d, content ='%s', plaform_name = '%s'",chatinfo.server_id, chatinfo.player_id,name, chatinfo.chat_type, context, plaform_name
        );

    if(!recorder.HasExecut())
    {
        CnError("save global setting chat info faild\n");
    }
}

void GlobalDbOperation::SaveRankingAlliance(Player* pPlayer, int32 winPro)
{
    //MULTISERVER_BATTLE_BAG& bag_info = pPlayer->GetMultiserverBattleInfo();
    //uint32 region_id = ConfigLoader::Instance()->GetRegionID();
    //std::string platform_name = ConfigLoader::Instance()->GetPlatformName();

    //DBRecorder recorder;

    //recorder = m_dbGlobalExecutor->Query("insert into ranking_alliance VALUES(0,%llu,%d,%d,%d,%d,'%s',%d,'%s',%d,%d,%d,0,%d)"
    //    "ON DUPLICATE KEY UPDATE "
    //    "sex=%d, level=%d, vip_level=%d, career=%d, player_name='%s', server_id=%d, plaform_name='%s', win_pro=%d, integral=%d, season_time=%d, battle_power=%d ",
    //    pPlayer->GetPlayerID(), pPlayer->GetSex(), pPlayer->GetLevel(), pPlayer->GetVipLevelByGrowthPoint(), pPlayer->GetCareer(), pPlayer->GetName().c_str(),
    //    region_id, platform_name.c_str(), winPro, bag_info.integral, bag_info.season_time, pPlayer->GetBattlePower(), 
    //    pPlayer->GetSex(), pPlayer->GetLevel(), pPlayer->GetVipLevelByGrowthPoint(), pPlayer->GetCareer(), pPlayer->GetName().c_str(),
    //    region_id, platform_name.c_str(), winPro, bag_info.integral, bag_info.season_time, pPlayer->GetBattlePower()
    //    );

    //if(!recorder.HasExecut())
    //{
    //    CnError("save global Save RankingAlliance info faild\n");
    //}
}

void GlobalDbOperation::GetRankingAlliances( uint64 playerID, std::vector<int32>& rankList )
{
    // 先获取最近一个排行的时间
    DBRecorder res;
    res = m_dbGlobalExecutor->Query("SELECT season_time FROM ranking_alliance GROUP BY season_time ORDER BY season_time DESC limit 1");
    if(!res.HasExecut())
    {
        CnDbg("global db GetRankingAlliances failed\n");
        return;
    }

    if(!res.HasRecord())
    {
        CnDbg("global db GetRankingAlliances no record\n");
        return;
    }
    
    int32 season_time = 0;
    res.GetRecord(season_time);

    // 查找最近一次排行的排行列表
    std::string txt = "SELECT `rank`, `sex`, `player_name`,`level`,`career`,`win_pro`, `integral` FROM ranking_alliance WHERE season_time >= "+ Crown::ToString(season_time) +" AND rank != 0 AND plaform_name = '"+ConfigLoader::Instance()->GetPlatformName()+"' ORDER BY rank ASC LIMIT 50";

    m_dbGlobalExecutor->Query(this, &GlobalDbOperation::CBLoadPlayerListByAlliancesRank, playerID, txt.c_str());
}

void GlobalDbOperation::CBLoadPlayerListByAlliancesRank( DBRecorder& res, uint64 playerID )
{
    Player* player = NULL;
    int32 pageMax = 0;
    RANKING_LIST playerRankInfo;
    RANKING_PLAYER_INFO playerInfo;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        CnDbg("CBLoadPlayerListByAlliancesRank player id=%llu not exist\n", playerID);
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
        res.GetRecord(playerInfo.battle_power);

        playerRankInfo.list.push_back(playerInfo);
        pageMax += 1;
    }

    RANKING_ACK rankAck;
    rankAck.rank_type = RANKING_TYPE_BATTLEHALL;
    rankAck.rank_list = playerRankInfo;
    rankAck.max_page = pageMax;
    rankAck.refresh_time = RankingLoaderJob::Instance()->m_refreshTime;

    rankAck.my_ranking = GetSeasonRankByPlayerID(playerID);

    RankListManager::Instance()->SendRankInfoToGate(playerID, rankAck);
}

int32 GlobalDbOperation::GetSeasonRankByPlayerID(uint64 playerID)
{
    DBRecorder recorder;

    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
    recorder = m_dbGlobalExecutor->Query("SELECT `rank` FROM `ranking_alliance` WHERE player_id =%llu and rank != 0 and season_time >= %d;", playerID,nowTime);
    if(recorder.GetRowNum() < 1)
        return 0;

    int32 rank = 0;
    recorder.GetRecord(rank);

    return rank;
}

void GlobalDbOperation::Update()
{
    m_minute += g_ServerUpdateTime;
    if(m_minute < 900000)
        return;

    m_minute = 0;

    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();

    RankingLoaderJob::Instance()->OnSaveRefreshTime(RANKING_TYPE_BATTLEHALL, nowTime);

    if(!ConfigLoader::Instance()->IsCrossCenterSrv())   // 不是跨服服务器不进行排序
        return;

    UpdateRankingAllianceRank();
}

bool GlobalDbOperation::UpdateRankingAllianceRank()
{
    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();

    DBRecorder recorder;

    recorder = m_dbGlobalExecutor->Query("SET @var =0;  UPDATE ranking_alliance a, (SELECT player_id,@var:=@var+1 AS PLACE FROM ranking_alliance "
        "WHERE season_time >= %d AND plaform_name = '%s' ORDER BY integral DESC ,win_pro DESC )b SET a.rank = b.PLACE WHERE a.player_id = b.player_id ", nowTime, ConfigLoader::Instance()->GetPlatformName().c_str());

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

bool GlobalDbOperation::ClearSeasonIntegral()
{
    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();

    DBRecorder recorder;

    recorder = m_dbGlobalExecutor->Query("UPDATE `ranking_alliance` SET old_rank=rank,rank=0,integral=0,win_pro=0");
    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

bool GlobalDbOperation::SaveSeasonInfo(const SEASON_INFO& info)
{
    DBRecorder recorder;
    std::string info_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    info_txt = StructToJson(info);

    m_dbGlobalExecutor->EscapeString(sqlTxt, info_txt.c_str(), info_txt.length());
    recorder = m_dbGlobalExecutor->Query("INSERT INTO `global_record` SET `key` = '%s', `info`='%s'"
        "ON DUPLICATE KEY UPDATE `info`='%s'", "season_info", sqlTxt, sqlTxt);

    if (recorder.GetAffNum() < 1)
        return false;

    return true;
}

bool GlobalDbOperation::LoadSeasonInfo(SEASON_INFO& info)
{
    DBRecorder recorder;
    recorder = m_dbGlobalExecutor->Query("SELECT `info` FROM `global_record` WHERE `key`='%s'", "season_info");

    if(!recorder.HasExecut())
    {
        CnInfo("unfound season_info \n");
        return false;
    }

    if (recorder.GetRowNum() > 0)
    {
        std::string info_str;

        recorder.GetRecord(info_str);

        if(!JsonToStruct(info, info_str))
        {
            CnInfo("unfound season_info \n");
            return false;
        }
    }

    return true;
}

bool GlobalDbOperation::GetOldRankListInfo(std::string platform_name, int32 region_id, std::map<int32, std::string>& rankMap)
{
    DBRecorder recorder;
    recorder = m_dbGlobalExecutor->Query("SELECT old_rank,player_name FROM `ranking_alliance` WHERE server_id=%d AND plaform_name='%s' AND old_rank != 0", region_id, platform_name.c_str());

    if(!recorder.HasExecut())
    {
        CnInfo("unfound season_info \n");
        return false;
    }

    int32 old_rank;
    std::string player_name;
    for(int32 i = 0; i < recorder.GetRowNum(); ++i)
    {
        recorder.GetRecord(old_rank);
        recorder.GetRecord(player_name);
        rankMap[old_rank] = player_name;
    }

    return true;
}

void GlobalDbOperation::SendMultiSrvInfoAboutRank(uint64 playerID)
{
    std::string txt = "SELECT player_name,vip_level,battle_power FROM ranking_alliance WHERE old_rank=1";

    m_dbGlobalExecutor->Query(this, &GlobalDbOperation::CBLoadMultiSrvInfoAboutRank, playerID, txt.c_str());
}

void GlobalDbOperation::CBLoadMultiSrvInfoAboutRank(DBRecorder& res, uint64 playerID)
{
    MULTISERVER_BATTLE_ABOUT_RANK_NTF sendMsg;

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(sendMsg.player_name);
        res.GetRecord(sendMsg.vip_level);
        res.GetRecord(sendMsg.battle_power);
    }

    sendMsg.my_rank = GetSeasonRankByPlayerID(playerID);
    Player* pPlayer = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(pPlayer)
    {
        pPlayer->SendToGate(MSG_MULTISERVER_BATTLE_ABOUT_RANK_NTF, &sendMsg);
    }
}

void GlobalDbOperation::RefreshRank()
{
    m_minute = 1000000;
}

