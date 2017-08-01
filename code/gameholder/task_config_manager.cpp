#include "gameholder_pch.h"
#include "task_config_manager.h"
#include "task_game_config.h"
#include "task_game_config_data.h"


IMPLEMENT_SINGLETON(TaskConfigManager)

TaskConfigManager::TaskConfigManager()
{
    TASK_GAME_ENTRY::CreateInstance();
    TaskGameConfigData::CreateInstance();
}

TaskConfigManager::~TaskConfigManager()
{
    TaskGameConfigData::DestroyInstance();
    TASK_GAME_ENTRY::DestroyInstance();
}

bool TaskConfigManager::LoadConfig( const char* path )
{
    if(TASK_GAME_ENTRY::Instance()->LoadConfig(path))
    {
        
    }
    else
        return false;

    if(TaskGameConfigData::Instance()->InitConfigData())
        return true;
    else
        return false;
}