#ifndef dungeon_job_h__
#define dungeon_job_h__

class DBExecutor;
class DBRecorder;

class DungeonJob
{
public:
    DungeonJob() {m_dbExecutor = NULL;}
    ~DungeonJob() {}

    DECLARE_SINGLETON(DungeonJob)

    // 设置SQL执行
    void SetDBExecutor(DBExecutor* val) {m_dbExecutor = val;}

    void LoadPlayerDungeonInfo(uint64 playerID);        ///< 加载角色副本信息

    void LoadDungeonScoreInfo(std::map<uint32, DUNGEON_SCORE_INFO>& dungeonScoreMap);

    void SaveDungeonScoreInfo(DUNGEON_SCORE_INFO& dungeonScoreInfo);

private:
    DungeonJob( const DungeonJob& );
    DungeonJob& operator=( const DungeonJob& );
    DungeonJob* operator&();
    const DungeonJob* operator&() const;

private:
    DBExecutor* m_dbExecutor;
};

#endif // dungeon_job_h__
