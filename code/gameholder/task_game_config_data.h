#ifndef task_game_config_data_h__
#define task_game_config_data_h__

#include "task_define.h"

struct TASK_GAME_ROOT_TASK_STRUCT;

/*
 *	标准配置任务数据
 */
class TaskGameConfigData
{
public:
    TaskGameConfigData();
    ~TaskGameConfigData();

    DECLARE_SINGLETON(TaskGameConfigData)

    bool InitConfigData();

    const TASK_TEMPLATE* GetMainTaskByPreID(uint32 pre_id);     // 获取下一个主线任务

    const TASK_TEMPLATE* GetTaskTemplate(uint32 taskID);

    const TASK_TEMPLATE* GetCrusadeTemplate(uint32 taskID);     // 根据id获取讨伐任务

    const TASK_TEMPLATE* GetEmptyTaskTemplate() {return &m_EmptyTemplate;}      // 空任务模板(当GameServer上无对应的任务模板时用)
    const TaskTemplateMap* GetCrusadeTask(){return &m_crusade_task;} // 获取讨伐任务模板
    const TaskTemplateMap* GetAchiveTask(){return &m_achive_task;} // 获取成就任务模板

private:
    void ReadTaskConfigData(TASK_TEMPLATE* pTaskTmpl, const TASK_GAME_ROOT_TASK_STRUCT* pTaskConfig);
    
    void ParsePreCondition(TASK_TEMPLATE* pTaskTmpl, const std::string& preCondtionStr);
    void ParseTaskReward(TASK_TEMPLATE* pTaskTmpl, const std::string& taskRewardStr);
    void ParseTaskRewardList(REWARD_TABLE* pReward, const std::string& taskRewardStr);
    void ParseTaskTarget(TASK_TEMPLATE* pTaskTmpl, const std::string& taskTargetStr);

    // 获取任务的前置任务ID
    const TASK_CONDITION*  GetTaskPreCondition(TASK_TEMPLATE* pTaskTmpl, uint32 conditionType);

private:
    TaskTemplateMap         m_premain_task;     // 主线任务依赖表<主线任务Id, 下一个主线任务模板>

    TaskTemplateMap         m_main_task;        // 主线任务<任务Id, 任务模板>
    TaskTemplateMap         m_daily_task;       // 日常任务<任务Id, 任务模板>
    TaskTemplateMap         m_crusade_task;     // 讨伐任务<任务Id, 任务模板>
    TaskTemplateMap         m_achive_task;      // 成就任务<任务Id, 任务模板>

    TaskTemplateMap         m_all_task;         // 所有任务<任务Id, 任务模板>

    TaskTemplateList        m_tasklist;         // 所有任务列表

    TASK_TEMPLATE           m_EmptyTemplate;    // 一个空模板
};


#endif // task_game_config_data_h__
