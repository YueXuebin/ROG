#ifndef task_config_manager_h__
#define task_config_manager_h__

#include "task_game_config.h"

/*
 *	�������ù�����
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
