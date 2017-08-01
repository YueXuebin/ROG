#include "centerserver_pch.h"
#include "dungeon_job.h"
#include "dbdriver.h"
#include "json_helper.h"

IMPLEMENT_SINGLETON(DungeonJob)

void DungeonJob::LoadDungeonScoreInfo(std::map<uint32, DUNGEON_SCORE_INFO>& dungeonScoreMap)
{
    Json::Reader reader;
    Json::Value dungeon_score_info_json;
    std::string dungeon_score_info_str;
    DUNGEON_SCORE_INFO dungeonScoreInfo;
    DBRecorder res;

    res = m_dbExecutor->Query("SELECT `score_info` FROM `dungeon_score`");
    if(!res.HasExecut())
    {
        CnError("Load dungeon score info failed\n");
        return;
    }

    if(res.GetRowNum() <= 0)
    {
        return;
    }

    for(uint32 i=0; i<res.GetRowNum(); ++i)
    {
        res.GetRecord(dungeon_score_info_str);

        if(!reader.parse(dungeon_score_info_str, dungeon_score_info_json, false))
        {
            CnError("Parse dungeon score info faild\n");
            return;
        }

        dungeonScoreInfo.Clear();

        read_DUNGEON_SCORE_INFO(dungeonScoreInfo, dungeon_score_info_json);

        dungeonScoreMap[dungeonScoreInfo.dungeon_id] = dungeonScoreInfo;
    }
}

void DungeonJob::SaveDungeonScoreInfo(DUNGEON_SCORE_INFO& dungeonScoreInfo)
{
    DBRecorder recorder;
    Json::FastWriter writer;
    Json::Value dungeon_score_info_json;
    std::string dungeon_score_info_str;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};
    char dungeon_score[1024] = {0};

    write_DUNGEON_SCORE_INFO(dungeonScoreInfo, dungeon_score_info_json);

    dungeon_score_info_str = writer.write(dungeon_score_info_json);

    m_dbExecutor->EscapeString(dungeon_score, dungeon_score_info_str.c_str(), dungeon_score_info_str.length());

    // 更新冒险信息
    m_dbExecutor->Query(
        "INSERT INTO `dungeon_score` VALUES('%u', '%s') "
        "ON DUPLICATE KEY UPDATE `dungeon_id`='%u', `score_info`='%s'",
        dungeonScoreInfo.dungeon_id, dungeon_score, 
        dungeonScoreInfo.dungeon_id, dungeon_score);
}
