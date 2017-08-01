#ifndef festival_job_h__
#define festival_job_h__


class DBExecutor;
class DBRecorder;

class FestivalJob
{
public:
    FestivalJob(void);
    ~FestivalJob(void);

    DECLARE_SINGLETON(FestivalJob)

    // 设置SQL执行
    void SetDBExecutor(DBExecutor* gamedb, DBExecutor* gmtdb)
    {
        m_gameExecutor = gamedb;
        m_gmtExecutor = gmtdb;
    }

    void LoadLocalFestival();                               // 从本地数据库加载

    void LoadGmtFestival();                                 // 从GMT数据库加载

    void HandleGmtFestivalList();                           //

    void SendToGameFestivalList();                          // 发送给GameServer

    bool InsertGmtSyncRecorder(int32 type, uint32 time);    // 保存活动更新时间

    bool InsertLocalFestival(const TASK_FESTIVAL_TEMPLATE_LIST& festival_list);

    bool DeleteLocalFestival();

private:
    enum
    {
        MAX_FESTIVEAL_JOB_SIZE = 2,
    };

    void CBLoadFestivalInfo(DBRecorder& res, bool b_update, bool b_notify);

    void ReadRewardItems(REWARD_TABLE& table, Json::Value festival_info_json, const std::string& str);
    void ReadTargetsList(TASK_TEMPLATE& taskTmpl,Json::Value val);
    bool ReadTarget(TASK_TARGET& target, const std::string& parma);
    void ReadTaskTmpl(std::string& festival_task_str, TASK_FESTIVAL_TEMPLATE& festivalTmpl);

    bool ReadTarget_1(TASK_TARGET& target, const std::string& param);
    bool ReadTarget_3(TASK_TARGET& target, const std::string& param);
    bool ReadTarget_NpcValueNum(TASK_TARGET& target, const std::string& param);
    bool ReadTarget_6(TASK_TARGET& target, const std::string& param);
    bool ReadTarget_17(TASK_TARGET& target,const std::string& param);
    bool ReadTarget_ValueNum(TASK_TARGET& target, const std::string& param);
    bool ReadTarget_Num(TASK_TARGET& target, const std::string& param);

    void WriteTaskTemplate(char* tasksqlTxt,std::vector<TASK_TEMPLATE> tasklist);
    void WriteReward(char* rewardsqlTxt,REWARD_TABLE table);
    void WriteConditon(char* conditionTxt, const TASK_FESTIVAL_TEMPLATE& festivaltemp);

private:
    DBExecutor*     m_gameExecutor;
    DBExecutor*     m_gmtExecutor;
};
#endif 