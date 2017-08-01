#include "gameholder_pch.h"
#include "task_data_guard.h"
#include "event_define.h"
#include "player.h"
#include "task_game_config.h"
#include "game_util.h"
#include "task_manager.h"
#include "task_game_manager.h"


CnImplementRTTI(GuardTask, GameTask);

GuardTask::GuardTask( const TASK_TEMPLATE* task_template, Player* player ) : 
GameTask(task_template, player)
{

}

GuardTask::~GuardTask()
{

}

// ==================================================
// 奖励护卫任务
// ==================================================
void GuardTask::OnRewards()
{
    //m_player->SendTaskEvent(TASK_EVENT_GUARD, GetTaskID(), 0);

    //REWARD_TABLE rewardList;
    //REWARD_TABLE fcmRewardList;

    //ItemProList& itemProList = TaskGameConfig::Instance()->GetQualityProList(TASK_TYPE_GUARD);
    //uint32 multiple = itemProList[m_template->task_id].m_multiple;

    //LevelRewardsList* levelRewards = &TaskGameConfig::Instance()->GetLevelRewardsList(TASK_TYPE_GUARD);

    //if (!RewardMultiple(multiple, rewardList, levelRewards))
    //{
    //    CnWarn("Error: guard rewards error\n");
    //    return;
    //}

    //// 护卫完成失败奖励减半
    //for (std::vector<REWARD_ITEM>::iterator rewardIt = rewardList.items.begin(); rewardIt != rewardList.items.end(); ++rewardIt)
    //{
    //    rewardIt->num = int32(rewardIt->num * ToPercent(m_rate_of_complete));
    //}

    //// 护卫完成时 护卫任务模板中的奖励也给  没完成不给
    //if(m_rate_of_complete >= BASE_RATE_NUM)
    //{
    //    rewardList.items.insert(rewardList.items.end(), m_template->rewards.items.begin(), m_template->rewards.items.end());
    //}

    //m_player->GetFcmRewardList(rewardList.items, fcmRewardList.items);
    //m_player->TakeInRewardTable(&fcmRewardList);

    //uint32 npcid = m_template->take_npc_id;
    //SendGuardAchive(npcid, m_player);

    //m_player->SendMyselfEvent(EVENT_ACIIVATION_PET, GetTaskID(), ACTIVATION_TASK, 0);


    //TaskGameManager* pTaskGameManager = m_TaskManager->GetTaskGameManager();
    //if(pTaskGameManager->GetGuardFreeNum() >=  1)
    //    pTaskGameManager->DecGuardFreeNum();
    //else if (pTaskGameManager->GetGuradBuyNum() >= 1)
    //    pTaskGameManager->DecGuardBuyNum();

    ////CnWarn("getreward, freenum , buynum , finishnum : %u,%u,%u\n", m_guard_free_count, m_guard_buy_count,m_guard_task_count);
    //// 活跃度
    //m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_GUARD, 1, 0, 0);
}

void GuardTask::SendGuardAchive(uint32 npcid, Player* player)
{
    //if(!player->FinishAchive(ACHIVE_GUARD))
    //{
    //    bool b_hurt = false;
    //    //b_hurt = player->GetScene()->NpcHurt(npcid);
    //    if(b_hurt)
    //    {
    //        player->SendMyselfEvent(EVENT_ACHIVE_GUARD, b_hurt, 0, 0);
    //    }
    //}

}

void GuardTask::OnAdd()
{
    m_state = TASK_STATE_HAVETAKE;
}
