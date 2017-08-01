#ifndef task_config_manager_h__
#define task_config_manager_h__

#include "task_game_config.h"

/*
 *	任务配置管理器
 */
class TaskConfigManager
{
    TaskConfigManager();
    ~TaskConfigManager();
    DECLARE_SINGLETON(TaskConfigManager)

public:
    bool LoadConfig(const char* path);



};

#endif // task_config_manager_h__
