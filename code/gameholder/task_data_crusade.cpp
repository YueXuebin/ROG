#include "gameholder_pch.h"
#include "task_data_crusade.h"
#include "task_game_config.h"
#include "event_define.h"
#include "player.h"
#include "task_manager.h"
#include "system_config_manager.h"



CnImplementRTTI(CrusadeTask, GameTask);

CrusadeTask::CrusadeTask( const TASK_TEMPLATE* task_template, Player* player ) : 
GameTask(task_template, player)
{

}

CrusadeTask::~CrusadeTask()
{

}

// ==================================================
// 奖励讨伐令任务
// ==================================================
void CrusadeTask::OnRewards()
{
    REWARD_TABLE rewardList;

    m_player->TakeInRewardTable(&m_template->rewards);
}

void CrusadeTask::OnAdd()
{
    m_state = TASK_STATE_CANTAKE;

    RoleTaskList tasks = m_TaskManager->GetRoleTasks();

    // 已经被使用的位置
    std::vector<uint32> existPoition;
    for (RoleTaskList::const_iterator cIter = tasks.begin();cIter != tasks.end(); cIter++)
    {
        if((*cIter)->GetType() == TASK_TYPE_CRUSADE)
        {
            existPoition.push_back((*cIter)->GetPosition());
        }
    }

    // 最大位置数
    uint32 maxPosNum = SystemConfigManager::Instance()->GetGameConfigValue("task_crusade_max_num");

    // 设置任务位置
    for (uint32 i=1; i <= maxPosNum ; i++)
    {
        std::vector<uint32>::iterator cIter = std::find(existPoition.begin(), existPoition.end(), i);
        if (cIter == existPoition.end())
        {
            SetPosition(i);
            break;
        }
    }
}
