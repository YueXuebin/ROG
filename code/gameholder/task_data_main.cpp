#include "gameholder_pch.h"
#include "task_data_main.h"
#include "task_game_manager.h"
#include "task_manager.h"
#include "player.h"
#include "event_define.h"


CnImplementRTTI(MainTask, GameTask);

MainTask::MainTask( const TASK_TEMPLATE* task_template, Player* player ) : 
GameTask(task_template, player)
{

}

MainTask::~MainTask()
{

}

void MainTask::OnRewards()
{
    // 如果是主线任务，则将已完成的主线任务ID保存
    TaskGameManager* pTaskGameManager = m_TaskManager->GetTaskGameManager();
    pTaskGameManager->SetCompleteMainTaskID(GetTaskID());


    REWARD_TABLE fcmRewardList;

    //m_player->GetFcmRewardList(m_template->rewards.items, fcmRewardList.items);

    m_player->GetFcmRewardList(GetReward().items, fcmRewardList.items);
    
    m_player->TakeInRewardTable(&fcmRewardList);

    m_player->SendMyselfEvent(EVENT_ACIIVATION_PET, GetTaskID(), ACTIVATION_TASK, 0);
}
