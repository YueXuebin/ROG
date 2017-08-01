#include "gameholder_pch.h"
#include "task_data_achive.h"
#include "player.h"

CnImplementRTTI(AchiveTask, GameTask);

AchiveTask::AchiveTask( const TASK_TEMPLATE* task_template, Player* player ) : 
GameTask(task_template, player)
{

}

AchiveTask::~AchiveTask()
{

}

void AchiveTask::OnRewards()
{
    REWARD_TABLE rewardList;

    m_player->TakeInRewardTable(&m_template->rewards);
}

void AchiveTask::OnAdd()
{
    // ��ӳɾ�����ģʽ���ѽ�
    m_state = TASK_STATE_HAVETAKE;
}