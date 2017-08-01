#include "gameholder_pch.h"
#include "task_data_daily.h"
#include "task_game_config.h"
#include "player.h"
#include "event_define.h"
#include "task_manager.h"
#include "task_game_manager.h"


CnImplementRTTI(DailyTask, GameTask);

DailyTask::DailyTask( const TASK_TEMPLATE* task_template, Player* player ) : 
GameTask(task_template, player)
{

}

DailyTask::~DailyTask()
{

}
// 日常任务奖励
void DailyTask::OnRewards()
{
    //REWARD_TABLE rewardList;
    //REWARD_TABLE fcmRewardList;

    //ItemProList& itemProList = TaskGameConfig::Instance()->GetQualityProList(TASK_TYPE_DAILY);
    //uint32 multiple = itemProList[m_quality].m_multiple;

    //LevelRewardsList* levelRewards = &TaskGameConfig::Instance()->GetLevelRewardsList(TASK_TYPE_DAILY);

    //if (!RewardMultiple(multiple, rewardList, levelRewards))
    //{
    //    CnWarn("Error: daily rewards error");
    //    return;
    //}

    //m_player->GetFcmRewardList(rewardList.items, fcmRewardList.items);
    //m_player->TakeInRewardTable(&fcmRewardList);

    //// 通关成功日常副本称号
    //if(!m_player->FinishAchive(ACHIVE_DAILY_DUNGE))
    //{
    //    m_player->SendMyselfEvent(EVENT_ACHIVE_DAILY_DUNGE,0,0,0);
    //}

    //m_player->SendMyselfEvent(EVENT_ACIIVATION_PET, GetTaskID(), ACTIVATION_TASK, 0);


    //TaskGameManager* pTaskGameManager = m_TaskManager->GetTaskGameManager();
    //pTaskGameManager->IncDailyTaskCount();

    //if(pTaskGameManager->GetDailyTaskCount() == MAX_DAILY_TASK_NUM)
    //{
    //    // 完成一轮日常任务
    //    m_player->SendTaskEvent(TASK_EVNET_CMPL_ONE_ROUND_DAILY, 0, 0);

    //    // 活跃度
    //    m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_FINISH_ALL_DAILY_TASK, 1, 0, 0);
    //}
}

void DailyTask::OnAdd()
{
    //m_state = TASK_STATE_HAVETAKE;

    //// 日常任务随机品质
    //ItemProList pro = TaskGameConfig::Instance()->GetQualityProList(GetType());
    //int32 quality = ItemRandom(pro);
    //CnAssert(quality != -1);
    //m_quality = quality;
}
