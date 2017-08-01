#include "centerserver_pch.h"
#include "offline_event_job.h"
#include "dbdriver.h"

IMPLEMENT_SINGLETON(OfflineEventJob)

bool OfflineEventJob::SaveOfflineEvent(uint64 playerID, uint32 eventID, const std::string& eventInfo)
{
    DBRecorder recorder;
    char sqlText[MAX_SQLTXT_LEN] = {0};

    m_dbExecutor->EscapeString(sqlText, eventInfo.c_str(), eventInfo.length());

    recorder = m_dbExecutor->Query("INSERT INTO `offline_event` VALUES(NULL, %llu, %d, '%s', 0)", playerID, eventID, sqlText);
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool OfflineEventJob::SaveOfflineEvent(const std::string& account_name, const std::string& server_name, uint32 eventID, const std::string& eventInfo)
{
    DBRecorder recorder;
    char sqlText[MAX_SQLTXT_LEN] = {0};

    m_dbExecutor->EscapeString(sqlText, eventInfo.c_str(), eventInfo.length());

    recorder = m_dbExecutor->Query("INSERT INTO `offline_event` SELECT NULL, player_id, '%d', '%s', 0  FROM player WHERE account_name='%s' AND platform_server_name='%s'", eventID, sqlText, account_name.c_str(), server_name.c_str());
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

void OfflineEventJob::LoadOfflineEvent(uint64 playerID, std::vector<offlineEventData>& offlineEventList)
{
    DBRecorder recorder;
    offlineEventData offlineEventNtf;

    recorder = m_dbExecutor->Query("SELECT id, event_id, event_str FROM offline_event WHERE player_id=%llu", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound loadridepetdata \n");
        return;
    }

    for(uint32 i = 0; i < recorder.GetRowNum(); ++i)
    {
        recorder.GetRecord(offlineEventNtf.db_id);
        recorder.GetRecord(offlineEventNtf.event_id);
        recorder.GetRecord(offlineEventNtf.eventInfo);
        offlineEventList.push_back(offlineEventNtf);
    }
}

bool OfflineEventJob::DelOfflineEvent(uint64 playerID)
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM offline_event WHERE player_id = '%llu'", playerID);

    if(!recorder.HasExecut())
    {
        CnInfo("unfound loadridepetdata \n");
        return false;
    }

    return true;
}
