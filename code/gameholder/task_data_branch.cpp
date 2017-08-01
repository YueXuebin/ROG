#include "gameholder_pch.h"
#include "task_data_branch.h"
#include "task_game_manager.h"
#include "task_manager.h"
#include "player.h"
#include "event_define.h"

CnImplementRTTI(BranchTask, GameTask);

BranchTask::BranchTask( const TASK_TEMPLATE* task_template, Player* player ) : 
GameTask(task_template, player)
{

}

BranchTask::~BranchTask()
{

}

void BranchTask::OnRewards()
{
    // 设置分支标记，表示这个分支任务已完成
    TaskGameManager* pTaskGameManager = m_TaskManager->GetTaskGameManager();
    pTaskGameManager->SetBranchTaskFlag(GetTaskID());

    REWARD_TABLE fcmRewardList;

    m_player->GetFcmRewardList(m_template->rewards.items, fcmRewardList.items);
    m_player->TakeInRewardTable(&fcmRewardList);

    m_player->SendMyselfEvent(EVENT_ACIIVATION_PET, GetTaskID(), ACTIVATION_TASK, 0);
}
