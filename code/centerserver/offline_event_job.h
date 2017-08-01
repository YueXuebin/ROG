/*
 *	离线事件数据库保存
 */
#ifndef offline_event_job_h__
#define offline_event_job_h__

class DBExecutor;
class DBRecorder;

typedef struct offlineEventData
{
    uint32 db_id;
    uint32 event_id;
    std::string eventInfo;
}offlineEventData;


class OfflineEventJob
{
public:
    OfflineEventJob() {m_dbExecutor = 0;}
    ~OfflineEventJob() {}

    DECLARE_SINGLETON(OfflineEventJob)

    // 设置SQL执行
    void SetDBExecutor(DBExecutor* val) {m_dbExecutor = val;}

    void LoadOfflineEvent(uint64 playerID, std::vector<offlineEventData>& offlineEventList);

    bool SaveOfflineEvent(uint64 playerID, uint32 eventID, const std::string& eventInfo);
    bool SaveOfflineEvent(const std::string& account_name, const std::string& server_name, uint32 eventID, const std::string& eventInfo);

    bool DelOfflineEvent(uint64 playerID);
private:
    OfflineEventJob( const OfflineEventJob& );
    OfflineEventJob& operator=( const OfflineEventJob& );
    OfflineEventJob* operator&();
    const OfflineEventJob* operator&() const;

    DBExecutor* m_dbExecutor;
};

#endif // offline_event_job_h__
