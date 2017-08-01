#ifndef world_boss_job_h__
#define world_boss_job_h__

class DBExecutor;
class DBRecorder;

class WorldBossJob
{
public:
    WorldBossJob() {m_dbExecutor = NULL;}
    ~WorldBossJob() {}

    DECLARE_SINGLETON(WorldBossJob)

    // 设置SQL执行
    void SetDBExecutor(DBExecutor* val) {m_dbExecutor = val;}

    void LoadWorldBossInfo();

    void SaveWorldBossInfo();

private:
    WorldBossJob( const WorldBossJob& );
    WorldBossJob& operator=( const WorldBossJob& );
    WorldBossJob* operator&();
    const WorldBossJob* operator&() const;

private:
    DBExecutor* m_dbExecutor;
};

#endif // world_boss_job_h__
