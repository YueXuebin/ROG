#include "gameholder_pch.h"
#include "task_data_game.h"
#include "player.h"
#include "game_util.h"


CnImplementRTTI(GameTask, TargetTask);

GameTask::GameTask(const TASK_TEMPLATE* task_template, Player* player) : 
TargetTask(task_template, player)
{

}

GameTask::~GameTask()
{

}

bool GameTask::RewardMultiple(uint32 multiple, REWARD_TABLE& rewardList, LevelRewardsList* taskList)
{
    if(!taskList) return false;

    rewardList = (*taskList)[m_player->GetLevel()];

    for (std::vector<REWARD_ITEM>::iterator rewardIt = rewardList.items.begin(); rewardIt != rewardList.items.end(); ++rewardIt)
    {
        // 品质公式
        rewardIt->num = int32(rewardIt->num * ToPercent(multiple));
    }

    if (rewardList.items.size() == 0)
    {
        return false;
    }
    return true;
}