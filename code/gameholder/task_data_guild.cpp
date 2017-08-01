#include "gameholder_pch.h"
#include "task_data_guild.h"
#include "guild_manager.h"
#include "player.h"
#include "guild_task_config.h"
#include "game_util.h"
#include "task_manager.h"
#include "task_game_manager.h"
#include "event_define.h"


CnImplementRTTI(GuildTask, GameTask);

GuildTask::GuildTask( const TASK_TEMPLATE* task_template, Player* player ) : 
GameTask(task_template, player)
{

}

GuildTask::~GuildTask()
{

}

// ==================================================
// 奖励公会任务
// ==================================================
void GuildTask::OnRewards()
{
    m_player->SendTaskEvent(TASK_EVENT_GUILD_TASK, 1 , 0);

    REWARD_TABLE rewardList;

    rewardList = m_template->rewards;

    GUILD_INFO*  guildInfo = GuildManager::Instance()->GetGuildInfo(m_player->GetGuildID());
    if(!guildInfo)
    {
        CnAssert(false);
        return;
    }
    REWARD_TABLE addtionTable = GuildTaskConfig::Instance()->GetRewardAddtion(guildInfo->level);

    rewardList.items.insert(rewardList.items.end(), addtionTable.items.begin(), addtionTable.items.end());
    FuseSameReward(rewardList);
    m_player->TakeInRewardTable(&rewardList);


    TaskGameManager* pTaskGameManager = m_TaskManager->GetTaskGameManager();
    pTaskGameManager->IncGuildTaskCount();
}

void GuildTask::OnAdd()
{
    m_state = TASK_STATE_CANTAKE;

    //// 随机品质
    //ItemProList pro = GuildTaskConfig::Instance()->GetQualityPro();
    //int32 quality = ItemRandom(pro);
    //CnAssert(quality != -1);
    //task.m_quality = quality;
}
