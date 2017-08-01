#ifndef offline_player_job_h__
#define offline_player_job_h__

class DBExecutor;
class DBRecorder;

class OfflinePlayerJob
{
    OfflinePlayerJob() { m_dbExecutor = 0; }
    ~OfflinePlayerJob() { }

    DECLARE_SINGLETON(OfflinePlayerJob)

    /// …Ë÷√SQL÷¥––
    void SetDBExecutor(DBExecutor* val) { m_dbExecutor = val; }

    void SearchTeamOfflinePlayer(uint64 playerID, TEAM_OFFLINE_MEMBER_ADD_REQ* offline_member_add_req);

private:
    OfflinePlayerJob( const OfflinePlayerJob& );
    OfflinePlayerJob& operator=( const OfflinePlayerJob& );
    OfflinePlayerJob* operator&();
    const OfflinePlayerJob* operator&() const;

    void CBSearchTeamOfflinePlayer(DBRecorder& res, uint64 playerID, TEAM_OFFLINE_MEMBER_ADD_REQ offline_member_add_req);

    DBExecutor*         m_dbExecutor;       ///< SQL÷¥––∆˜
};


#endif // offline_player_job_h__
