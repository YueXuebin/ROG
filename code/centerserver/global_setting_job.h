#ifndef global_setting_job_h__
#define global_setting_job_h__

class DBExecutor;
class DBRecorder;


class GlobalSettingJob
{
public:
    GlobalSettingJob(){m_dbExecutor = NULL;}
    ~GlobalSettingJob(){}

    DECLARE_SINGLETON(GlobalSettingJob)

    // 设置SQL执行
    void SetDBExecutor(DBExecutor* val) {m_dbExecutor = val;}

    void LoadGlobalSettingInfo();

    void SaveGlobalSettingInfo();

    void LoadProclamationInfo();

private:
    GlobalSettingJob( const GlobalSettingJob& );
    GlobalSettingJob& operator=( const GlobalSettingJob& );
    GlobalSettingJob* operator&();
    const GlobalSettingJob* operator&() const;

private:
    DBExecutor* m_dbExecutor;

};

#endif