class DBExecutor;
class DBRecorder;

class Player;

#ifndef activity_job_h__
#define activity_job_h__

class ActivityJob
{
    ActivityJob() { m_dbExecutor = 0; }
    ~ActivityJob() { }

    DECLARE_SINGLETON(ActivityJob)

    /// 设置SQL执行
    void SetDBExecutor(DBExecutor* val) { m_dbExecutor = val; }

    /// 获得冒险者列表
    void GetAdventurerList(uint64 playerID);

    /// 获得冒险者资源
    void RobAdventurer(uint64 playerID, uint64 roberID);

    /// 减少掠夺次数
    void SubRobTimes(uint64 playerID, uint64 roberID);

    /// 更新冒险信息
    void UpdateAdventure(uint64 playerID, ADVENTURE_ADD_REQ adventureAdd);

private:
    ActivityJob(const ActivityJob&);
    ActivityJob& operator=(const ActivityJob&);
    ActivityJob* operator&();
    const ActivityJob* operator&() const;

    /// 获得冒险列表回调
    void CBGetAdventurerList(DBRecorder& res, uint64 playerID);

    /// 获得冒险者资源回调
    void CBRobAdventurer(DBRecorder& res, uint64 playerID, uint64 roberID);

    /// 减少掠夺次数回调
    void CBSubRobTimes(DBRecorder& res, uint64 playerID, uint64 roberID, int32 gamepoint);

    /// 更新冒险信息回调
    void CBUpdateAdventure(DBRecorder& res, uint64 playerID);

    DBExecutor*         m_dbExecutor;                                                                   ///< SQL执行器
};

#endif
