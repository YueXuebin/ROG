#include "gameholder_pch.h"
#include "task_game_manager.h"
#include "task_manager.h"
#include "vip_config.h"
#include "task_game_config_data.h"
#include "sdbit.h"
#include "player.h"
#include "item_manager.h"
#include "gameplaytimes_notifier.h"
#include "trade_manager.h"
#include "guild_task_config.h"
#include "game_util.h"
#include "protect_config.h"
#include "task_data_factory.h"
#include "task_data_game.h"
#include "system_config_manager.h"

// ==================================================
// 构造函数
// ==================================================
TaskGameManager::TaskGameManager(Player* player):
m_player(player)
{
    Clear();
}

// ==================================================
// 析构函数
// ==================================================
TaskGameManager::~TaskGameManager()
{
}

// ==================================================
// 清空任务
// ==================================================
void TaskGameManager::Clear()
{
    //m_guide_id                  = 0;
    m_CompleteMainTaskID              = 0;
    m_daily_info.Clear();
    m_crusade_info.Clear();
    m_guard_info.Clear();
    m_guild_info.Clear();
    m_achive_info.Clear();
    //m_daily_info.daily_task_flash_time     = 0;
    //m_daily_info.daily_task_count          = 0;
    //m_daily_info.daily_task_reset_count    = 0;
    m_crusade_info.crusade_task_flash_time   = 0;
    m_crusade_info.crusade_task_count        = 0;
    //m_guard_info.guard_refresh_count       = 0;
    //m_guard_info.guard_buy_count           = 0;
    //m_guard_info.guard_task_count          = 0;
    m_open_box                  = 0;
    //m_guard_info.guard_buy_day_count    = 0;

    m_tasks.clear();

    memset(m_branch_task_flag, 0, sizeof(m_branch_task_flag));
}

// ==================================================
// 加载任务信息
// ==================================================
void TaskGameManager::LoadTaskGameInfo(const TASK_GAME_INFO& info)
{
    m_CompleteMainTaskID = info.complete_main_task_id;                      // 完成的上一个主线任务ID
    m_daily_info.daily_task_flash_time = info.daily_task_flash_time;        // 每日日常/讨伐令任务刷新时间
    m_daily_info.daily_task_count = info.daily_task_count;                  // 日常任务完成次数
    m_daily_info.daily_task_reset_count = info.daily_task_reset_count;      // 日常任务重置次数
    m_crusade_info.crusade_task_flash_time = info.crusade_task_flash_time;  // 讨伐令任务刷新时间
    m_crusade_info.crusade_task_count = info.crusade_task_count;            // 讨伐令任务接的次数
    m_guard_info.guard_task_count =  info.guard_task_count;                 // 护卫每天的免费次数
    m_guard_info.guard_buy_count = info.guard_buy_count;                    // 护卫购买次数
    m_open_box = info.open_box;                                             // 宝箱是否开过
    m_guard_info.guard_buy_day_count = info.guard_buy_day_count;            // 护卫每天购买的总次数
    m_guild_info = info.guild_task;

    // 支线任务标记
    for(uint32 i = 0; (i < info.branch_task_flag.size()) && (i < TASK_BRANCH_TASK_FLAG_LEN); ++i)
    {
        m_branch_task_flag[i] = info.branch_task_flag[i];
    }

    m_task_mgr->LoadRoleTask(info.list);
}

// ==================================================
// 填充任务信息
// ==================================================
void TaskGameManager::FillTaskGameInfo(TASK_GAME_INFO& info) const
{
    info.Clear();

    info.complete_main_task_id = m_CompleteMainTaskID;
    info.daily_task_flash_time = m_daily_info.daily_task_flash_time;
    info.daily_task_count = m_daily_info.daily_task_count;
    info.daily_task_reset_count = m_daily_info.daily_task_reset_count;


    info.crusade_task_flash_time = m_crusade_info.crusade_task_flash_time;
    info.crusade_task_count = m_crusade_info.crusade_task_count;
    info.open_box = m_open_box;
    info.guard_task_count = m_guard_info.guard_task_count;
    info.guard_buy_count = m_guard_info.guard_buy_count;
    info.guard_buy_day_count = m_guard_info.guard_buy_day_count;
    info.guild_task = m_guild_info;

    // 支线任务标记
    for(uint32 branch_flag_num = 0; branch_flag_num < TASK_BRANCH_TASK_FLAG_LEN; ++branch_flag_num)
    {
        info.branch_task_flag.push_back(m_branch_task_flag[branch_flag_num]);
    }

    m_task_mgr->FillTaskRecordList(m_tasks, info.list);
}

// ==================================================
// 设置引导任务ID
// ==================================================
//void TaskGameManager::SetGuildID(uint32 guildID)
//{
//    m_guide_id = guildID;
//}

uint32 TaskGameManager::GetCompleteMainTaskID()
{
    return m_CompleteMainTaskID;
}

void TaskGameManager::SetCompleteMainTaskID(uint32 taskID)
{
    CnAssert((taskID / TASK_ID_BASE_MOD) == TASK_MAIN_ID_MOD);      // 主线任务ID
    m_CompleteMainTaskID = taskID;
}

// ==================================================
// 每日刷新
// ==================================================
void TaskGameManager::CalDailyRefresh()
{
    // 每日数据重置
    RefreshDailyTask();

    // 刷新任务时间
    OnTaskRefreshTimeReq();
}

// ==================================================
// 24小时重置日常等计数
// ==================================================
void TaskGameManager::RefreshDailyTask()
{
    std::vector<uint32> del_list;
    del_list.clear();
    RoleTaskList add_list;
    add_list.clear();

    // 保存数据
    SaveYesterdayData();

    m_daily_info.daily_task_count           = 0;
    m_daily_info.daily_task_reset_count     = 0;
    m_crusade_info.crusade_task_count       = 0;
    m_open_box                              = 0;

    // 清除所有正在做的日常任务
    m_task_mgr->DelTaskByTypeAndState(del_list, TASK_TYPE_DAILY);

    // 更新护卫时间
    // m_guard_buy_count = 0;
    m_guard_info.guard_task_count = 0;
    m_guard_info.guard_buy_day_count = 0;

    // 公会任务重置
    RefreshGuildTask();

    // 每日成就重置
    RefreshAchiveTask(del_list);

    // 通知任务变更
    m_task_mgr->NotifyTaskChange(del_list, add_list);
}

// ==================================================
// 更新讨伐令任务(系统按时间自动刷新)
// ==================================================
bool TaskGameManager::RefreshCrusadeTask(std::vector<uint32>& del_list)
{
    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue() + 30;     // 当前时间,加30秒，防止误差
    uint32 loopTime = SystemConfigManager::Instance()->GetGameConfigValue("task_crusade_loop_time");

    // 指定秒数刷新一次任务列表
    if(nowTime > m_crusade_info.crusade_task_flash_time)
    {
        // 更新时间
        m_crusade_info.crusade_task_flash_time = nowTime + (loopTime - (nowTime - m_crusade_info.crusade_task_flash_time) % loopTime);

        std::vector<uint32> del_list_can_take;
        std::vector<uint32> del_list_reward;
        // 删除所有可接和已经拿过奖励的讨伐令任务
        m_task_mgr->DelTaskByTypeAndState(del_list_can_take, TASK_TYPE_CRUSADE, TASK_STATE_CANTAKE);
        m_task_mgr->DelTaskByTypeAndState(del_list_reward, TASK_TYPE_CRUSADE, TASK_STATE_REWARD);
        // 合并所有删除任务列表
        for (uint32 i=0; i < del_list_can_take.size(); i++)
        {
            del_list.push_back(del_list_can_take[i]);
        }
        for (uint32 i=0; i < del_list_reward.size(); i++)
        {
            del_list.push_back(del_list_reward[i]);
        }

        return true;
    }

    return false;
}

bool TaskGameManager::RefreshAchiveTask(std::vector<uint32>& del_list)
{
    std::vector<uint32> del_list_have_take;
    std::vector<uint32> del_list_reward;
    // 删除所有已接和已经拿过奖励的任务
    m_task_mgr->DelTaskByTypeAndState(del_list_have_take, TASK_TYPE_ACHIVE, TASK_STATE_HAVETAKE);
    m_task_mgr->DelTaskByTypeAndState(del_list_reward, TASK_TYPE_ACHIVE, TASK_STATE_REWARD);
    // 合并所有删除任务列表
    for (uint32 i=0; i < del_list_have_take.size(); i++)
    {
        del_list.push_back(del_list_have_take[i]);
    }
    for (uint32 i=0; i < del_list_reward.size(); i++)
    {
        del_list.push_back(del_list_reward[i]);
    }

    return true;
}

// ==================================================
// 刷新公会任务
// ==================================================
void TaskGameManager::RefreshGuildTask()
{
    m_guild_info.guild_sroces = 0;
    m_guild_info.guild_task_count = 0;
    m_guild_info.open_box_list.clear();
}

// ==================================================
// 添加可以接受的任务（主线任务、支线任务、日常任务、讨伐令、护卫、公会任务、成就任务）
// ==================================================
void TaskGameManager::AddCanGameTakeTask(RoleTaskList* add_list)
{
    AddMainTask(add_list);
    AddBranchTask(add_list);
    AddDailyTask(add_list);
    AddCrusadeTask(add_list);
    AddGuardTask(add_list);
    AddGuildTask(add_list);
    AddAchiveTask(add_list);
}

// ==================================================
// 添加任务
// ==================================================
void TaskGameManager::AddTask(RoleTask* pTask)
{
    CnAssert( pTask->IsKindOf(&GameTask::ms_RTTI) );
    m_tasks.push_back(pTask);
}

// ==================================================
// 删除任务
// ==================================================
void TaskGameManager::DelTask(RoleTask* pTask)
{
    CnAssert( pTask->IsKindOf(&GameTask::ms_RTTI) );
    for(RoleTaskList::iterator taskIt = m_tasks.begin(); taskIt != m_tasks.end(); ++taskIt)
    {
        if((*taskIt) != pTask) 
            continue;
        m_tasks.erase(taskIt);
        break;
    }
}

// ==================================================
// 分支任务是否被置位
// ==================================================
bool TaskGameManager::IsBranchTaskFlag(uint32 taskID)
{
    uint32 index = 0;
    uint32 bit = 0;
    uint32  flag = 0;
    taskID %= 10000;

    index = taskID / 32;
    bit = taskID % 32;

    if(index >= TASK_BRANCH_TASK_FLAG_LEN)
    {
        return false;
    }

    flag = m_branch_task_flag[index] & (1 << bit);

    if(0 == flag)
    {
        return false;
    }

    return true;
}

// ==================================================
// 分支任务置位
// ==================================================
void TaskGameManager::SetBranchTaskFlag(uint32 taskID)
{
    uint32 index = 0;
    uint32 bit = 0;

    taskID %= 10000;

    index = taskID / 32;
    bit = taskID % 32;

    if(index >= TASK_BRANCH_TASK_FLAG_LEN)
    {
        return;
    }

    m_branch_task_flag[index] |= 1 << bit;
}

// ==================================================
// 分支任务置位
// ==================================================
void TaskGameManager::ResetBranchTaskFlag(uint32 taskID)
{
    uint32 index = 0;
    uint32 bit = 0;

    taskID %= 10000;

    index = taskID / 32;
    bit = taskID % 32;

    if(index >= TASK_BRANCH_TASK_FLAG_LEN)
    {
        return;
    }

    m_branch_task_flag[index] &= ~(1 << bit);
}

// ==================================================
// 日常任务数
// ==================================================
int32 TaskGameManager::GetDailyTaskTotal()
{
    int32 taskNum = 0;
    for (RoleTaskList::iterator it = m_tasks.begin(); it != m_tasks.end(); ++it)
    {
        RoleTask* pTask = *it;
        if(pTask->GetType() == TASK_TYPE_DAILY)
        {
            ++taskNum;
        }
    }
    return taskNum;
}

// ==================================================
// 获得日常任务记数
// ==================================================
uint32 TaskGameManager::GetDailyTaskCount()
{
    return m_daily_info.daily_task_count;
}

// ==================================================
// 添加日常任务记数
// ==================================================
void TaskGameManager::IncDailyTaskCount()
{
    ++m_daily_info.daily_task_count;
}

// ==================================================
// 获取讨伐令任务次数
// ==================================================
uint32 TaskGameManager::GetCrusadeTaskCount()
{
    return m_crusade_info.crusade_task_count;
}

// ==================================================
// 增加讨伐令任务次数
// ==================================================
void TaskGameManager::IncCrusadeTaskCount()
{
    ++m_crusade_info.crusade_task_count;
}

// ==================================================
// 获得护卫任务次数
// ==================================================
uint32 TaskGameManager::GetGuardTaskNum()
{
    uint32 taskNum = m_guard_info.guard_task_count + m_guard_info.guard_buy_count;
    return taskNum;
}
// ==================================================
// 获得护卫免费次数
// ==================================================
uint32 TaskGameManager::GetGuardFreeNum()
{
    return m_guard_info.guard_task_count;
}
// ==================================================
// 减少护卫免费次数
// ==================================================
void TaskGameManager::DecGuardFreeNum()
{
    if(m_guard_info.guard_task_count > 0)
        --m_guard_info.guard_task_count;
}
// ==================================================
// 获得护卫购买次数
// ==================================================
uint32 TaskGameManager::GetGuradBuyNum()
{
    return m_guard_info.guard_buy_count;
}
// ==================================================
// 减少护卫购买次数
// ==================================================
void TaskGameManager::DecGuardBuyNum()
{
    if(m_guard_info.guard_buy_count > 0)
        --m_guard_info.guard_buy_count;
}
// ==================================================
// 得到公会任务接受次数
// ==================================================
uint32 TaskGameManager::GetGuildTaskTakeCount()
{
    return m_guild_info.guild_task_count;
}
// ==================================================
// 添加公会任务次数
// ==================================================
void TaskGameManager::IncGuildTaskCount()
{
    ++m_guild_info.guild_task_count;
}
// ==================================================
// 添加公会积分
// ==================================================
void TaskGameManager::IncGuildScore(int32 score)
{
    if(score <= 0)
        return;

    uint32 guild_sroces = m_guild_info.guild_sroces;
    guild_sroces += score;
    if(guild_sroces > MAX_GUILD_SCORE_NUM)
        return;

    m_guild_info.guild_sroces += score;

    NotifyGameTaskRefresh();
}

// ==================================================
// 讨伐令可接任务数(任务状态为可接的任务数)
// ==================================================
int32 TaskGameManager::GetCrusadeCanTakeTotal()
{
    return GetTaskTotal(TASK_TYPE_CRUSADE, TASK_STATE_CANTAKE);
}

// ==================================================
// 讨伐令已完成数(包含完成任务未领取奖励和已完成已领取奖励的)
// ==================================================
int32 TaskGameManager::GetCrusadeHaveTaskTotal()
{
    return GetTaskTotal(TASK_TYPE_CRUSADE, TASK_STATE_REWARD) + GetTaskTotal(TASK_TYPE_CRUSADE, TASK_STATE_COMPLETE);
}

// ==================================================
// 护卫已接任务数
// ==================================================
int32 TaskGameManager::GetGuardTaskTotal()
{
    return GetTaskTotal(TASK_TYPE_GUARD);
}

// ==================================================
// 得到公会任务数
// ==================================================
int32 TaskGameManager::GetGuildTaskTotal()
{
    return GetTaskTotal(TASK_TYPE_GUILD);
}

// ==================================================
// 得到公会已接任务数
// ==================================================
int32 TaskGameManager::GetGuildHaveTaskTotal()
{
    return GetTaskTotal(TASK_TYPE_GUILD, TASK_STATE_HAVETAKE);
}

// ==================================================
// 得到公会任务已完成数
// ==================================================
int32 TaskGameManager::GetGuildCompleteTaskTotal()
{
    return GetTaskTotal(TASK_TYPE_GUILD, TASK_STATE_COMPLETE);
}

int32 TaskGameManager::GetTaskTotal(uint8 task_type, uint8 task_state)
{
    int32 taskNum = 0;
    for(RoleTaskList::iterator it = m_tasks.begin(); it != m_tasks.end(); ++it)
    {
        RoleTask* pTask = *it;
        if( pTask->GetType() == task_type && (task_state == 0 || pTask->GetState() == task_state) )
        {
            ++taskNum;
        }
    }
    return taskNum;
}

// ==================================================
// 公会宝箱是否被领过
// ==================================================
bool TaskGameManager::IsOpenGuildTaskBox(int32 boxScore)
{
    for(std::vector<int32>::iterator boxIt = m_guild_info.open_box_list.begin(); boxIt != m_guild_info.open_box_list.end(); ++boxIt)
    {
        if(*boxIt == boxScore)
        {
            return true;
        }
    }

    return false;
}

// ==================================================
// 刷新讨伐令
// ==================================================
void TaskGameManager::OnTaskCrusadeRefreshReq()
{
    std::vector<uint32> del_list_can_take;
    std::vector<uint32> del_list_reward;
    std::vector<uint32> del_list_all;
    RoleTaskList       add_list;
    uint32 nowTime = 0;

    int32 cost = SystemConfigManager::Instance()->GetGameConfigValue("task_crusade_refresh_money");

    // 检查钻石数
    if (m_player->GetMoneyPoint() < cost)
    {
        m_player->NotifyErrorCode(ERR_NO_ENOUGH_MONEYPOINT);
        //m_task_mgr->SendTaskItemAck(ERR_NO_ENOUGH_MONEYPOINT, TASK_REFRESH_STONE);
        return;
    }

    // 消耗钻石
    if (cost > 0)
    {
        m_player->SubMoneyPoint(cost, 0);
    }

    // 删除所有可接和已经拿过奖励的讨伐令任务
    m_task_mgr->DelTaskByTypeAndState(del_list_can_take, TASK_TYPE_CRUSADE, TASK_STATE_CANTAKE);
    m_task_mgr->DelTaskByTypeAndState(del_list_reward, TASK_TYPE_CRUSADE, TASK_STATE_REWARD);
    // 合并所有删除任务列表
    for (uint32 i=0; i < del_list_can_take.size(); i++)
    {
        del_list_all.push_back(del_list_can_take[i]);
    }
    for (uint32 i=0; i < del_list_reward.size(); i++)
    {
        del_list_all.push_back(del_list_reward[i]);
    }

    // 更新时间
    nowTime = (uint32)Crown::SDNow().GetTimeValue();
    m_crusade_info.crusade_task_flash_time = nowTime + SystemConfigManager::Instance()->GetGameConfigValue("task_crusade_loop_time");

    // 添加可以添加的任务
    m_task_mgr->AddCanTakeTask(&add_list);

    // 通知任务变更
    m_task_mgr->NotifyTaskChange(del_list_all, add_list);
}

void TaskGameManager::OnTaskCrusadeGiveUpReq(void* data)
{
    TASK_REQ* recvMsg = (TASK_REQ*)data;

    if(!recvMsg) return;

    RoleTask* roleTask = m_task_mgr->GetTaskByIdAndState(recvMsg->task_id, TASK_STATE_HAVETAKE);

    if(!roleTask)
    {
        m_player->NotifyErrorCode(ERR_UNFOUND_TASK);
        return;
    }

    if (roleTask->GetType() != TASK_TYPE_CRUSADE)
    {
        // 不是讨伐令任务
        CnAssert(false);
        return;
    }
    roleTask->Cancel();


    m_task_mgr->NotifyTaskUpdate(roleTask);

}

// ==================================================
// 领取宝箱
// ==================================================
void TaskGameManager::OnTaskReceiveBoxReq( void* data )
{
    TASK_RECEIVE_BOX_REQ* recvMsg = (TASK_RECEIVE_BOX_REQ*) data;

    // 不是VIP 以后都不能领VIP宝箱
    // if(m_player->GetVipLevel() <= 0)
    if(!VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_6 , m_player->GetVipLevel()) )
    {
        m_open_box = Crown::SDBitSaveOne(TASK_DAILY_BOX_VIP, m_open_box);
        //return;
    }

    // 箱子奖励模板
    //BoxRewardList& boxReward =  TaskGameConfig::Instance()->m_boxReward;

    //if (recvMsg->box == TASK_DAILY_BOX_TEN)
    //{
    //    if (m_daily_info.daily_task_count < boxReward[TASK_DAILY_BOX_TEN].m_num)
    //    {
    //        SendTaskBoxAck(ERR_BOX_COMPLETE_NUM_LACK,TASK_DAILY_BOX_TEN);
    //        return;
    //    }

    //    REWARD_TABLE list = boxReward[TASK_DAILY_BOX_TEN].m_rewardlist;
    //    if (!Crown::SDBitRead(TASK_DAILY_BOX_TEN, m_open_box))
    //    {
    //        m_open_box = Crown::SDBitSaveOne(TASK_DAILY_BOX_TEN, m_open_box);
    //        m_player->TakeInRewardTable(&list);
    //    }
    //}
    //else if (recvMsg->box == TASK_DAILY_BOX_VIP)
    //{
    //    // vip等级不足
    //    //if(m_player->GetVipLevel() <= 0)
    //    if(!VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_6 , m_player->GetVipLevel()))
    //    {
    //        SendTaskBoxAck(ERR_NO_ENOUGH_VIP, TASK_DAILY_BOX_VIP);
    //        return;
    //    }

    //    if (m_daily_info.daily_task_count < boxReward[TASK_DAILY_BOX_VIP].m_num)
    //    {
    //        SendTaskBoxAck(ERR_BOX_COMPLETE_NUM_LACK,TASK_DAILY_BOX_VIP);
    //        return;
    //    }

    //    REWARD_TABLE list = boxReward[TASK_DAILY_BOX_VIP].m_rewardlist;
    //    if (!Crown::SDBitRead(TASK_DAILY_BOX_VIP, m_open_box))
    //    {
    //        m_open_box = Crown::SDBitSaveOne(TASK_DAILY_BOX_VIP, m_open_box);
    //        m_player->TakeInRewardTable(&list);
    //    }
    //}
    //else if (recvMsg->box == TASK_CRUSADE_BOX_TWENTY)
    //{
    //    if (m_crusade_info.crusade_task_count < boxReward[TASK_CRUSADE_BOX_TWENTY].m_num)
    //    {
    //        SendTaskBoxAck(ERR_BOX_COMPLETE_NUM_LACK,TASK_CRUSADE_BOX_TWENTY);
    //        return;
    //    }
    //    REWARD_TABLE list = boxReward[TASK_CRUSADE_BOX_TWENTY].m_rewardlist;
    //    if (!Crown::SDBitRead(TASK_CRUSADE_BOX_TWENTY, m_open_box))
    //    {
    //        m_open_box = Crown::SDBitSaveOne(TASK_CRUSADE_BOX_TWENTY, m_open_box);
    //        m_player->TakeInRewardTable(&list);
    //    }
    //}

    NotifyGameTaskRefresh();
}

// ==================================================
// 刷新任务时间
// ==================================================
void TaskGameManager::OnTaskRefreshTimeReq()
{
    std::vector<uint32> del_list;
    RoleTaskList add_list;

    // 刷新讨伐令任务
    RefreshCrusadeTask(del_list);

    // 添加可以接受的任务
    m_task_mgr->AddCanTakeTask(&add_list);

    // 当角色第一次加载的时候不需要发送数据,数据会在角色进入场景后发送给他
    // 通知任务变更
    m_task_mgr->NotifyTaskChange(del_list, add_list);

    // 刷新任务时间
    if((del_list.size() < 1) && (add_list.size() < 1))
    {
        NotifyGameTaskRefresh();
    }
}

// ==================================================
// 变更护卫任务
// ==================================================
void TaskGameManager::OnTaskGuardRefreshReq()
{
    //std::vector<uint32> del_list;
    //RolePTaskList       add_list;

    ////// 检查变更石
    ////if (!m_player->GetItemManager()->GetItemNum(TASK_CHANGE_STONE))
    ////{
    ////    m_task_mgr->SendTaskItemAck(ERR_UNFOUND_ITEM, TASK_CHANGE_STONE);
    ////    return;
    ////}

    //// 删除所有可接的讨伐令任务
    //m_task_mgr->DelTaskByTypeAndState(del_list, TASK_TYPE_GUARD);

    //// 随机任务
    //TaskTemplateMap guardTask = TaskGameConfig::Instance()->m_guard_task;
    //ItemProList guardQualityPro = TaskGameConfig::Instance()->m_qualityPro[TASK_TYPE_GUARD];
    //int32 taskID = ItemRandom(guardQualityPro);
    //TaskTemplateMap::iterator guardIt;

    //// 刷到一定数量变更为最大品质任务
    //if(m_guard_info.guard_refresh_count >= GUARD_MAX_QUALITY_NUM)
    //{
    //    taskID = guardQualityPro.rbegin()->first;
    //}

    //guardIt = guardTask.find((uint32)taskID);
    //if( guardIt == guardTask.end())
    //{
    //    CnFatal("ERROR: guard random fail\n");
    //    return;
    //}

    //// 消耗变更石
    ////m_player->GetBag()->DelItemByID(TASK_CHANGE_STONE, 1);

    //// 刷到最大品质时 固定结点奖励清0
    //if(taskID == guardQualityPro.rbegin()->first)
    //{
    //    // 公告客户端自发 解决跑马灯延迟问题
    //    //SERVER_MARQUEE sendMsg;
    //    //sendMsg.Clear();
    //    //sendMsg.id = MARQUEE_GUARD_MAX_QUALITY;
    //    //sendMsg.parame_list.push_back(MarQueePlayerName(m_player->GetVipLevel(), m_player->GetName()));
    //    //m_player->OnMarQuee(sendMsg);

    //    m_guard_info.guard_refresh_count = 0;
    //}
    //else
    //{
    //    // 护卫任务变更数增加
    //    ++m_guard_info.guard_refresh_count;
    //}

    //m_task_mgr->AddTask(guardIt->second, &add_list);

    //m_task_mgr->NotifyTaskChange(del_list, add_list);
}

// ==================================================
// 日常任务重置请求
// ==================================================
void TaskGameManager::OnTaskDailyResetReq(void* data)
{
    // 判断次数是否最大
    //if(m_daily_info.daily_task_reset_count >=  VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_3 , m_player->GetVipLevel()))
    //{
    //    SendTaskDailyResetAck(ERR_TASK_DAILY_MAX_RESET);
    //    return;
    //}

    //if(GetDailyTaskTotal() > 0)
    //{
    //    SendTaskDailyResetAck(ERR_TASK_DAILY_RESET);
    //    return;
    //}

    //// 判断魔石是否足够
    //int32 needMoney = TaskGameConfig::Instance()->GetDailyTaskBuyMoneyByIndex(m_daily_info.daily_task_reset_count + 1);

    //if (needMoney == 0)
    //{
    //    CBDailyReset();
    //    return;
    //}

    //CnAssert(needMoney > 0);

    //if(m_player->GetMoneyPoint() < needMoney)
    //{
    //    SendTaskDailyResetAck(ERR_NO_ENOUGH_MONEYPOINT);
    //    return;
    //}

    //TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_TASK_DAILY_RESET, PROP_TASK_DAILY_RESET, 0, 1, needMoney);
}

// ==================================================
// 护卫任务次数购买请求
// ==================================================
void TaskGameManager::OnTaskGuardBuyReq(void* data)
{
    // 判断购买次数是否最大
    // uint8 max_buy_count = VipConfig::Instance()->GetBuyGuardCount(m_player->GetVipLevel());
    uint8 max_buy_count = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_4 , m_player->GetVipLevel());
    // if(m_guard_buy_count >= max_buy_count)
    if(m_guard_info.guard_buy_day_count >=  max_buy_count)
    {
        SendTaskGuardBuyAck(ERR_TASK_GUARD_MAX_BUY_COUNT);
        return;
    }

    int32 money = ProtectConfig::Instance()->GetProtectMoney(m_guard_info.guard_buy_day_count);

    // 判断魔石数
    if(m_player->GetMoneyPoint() < money)
    {
        SendTaskGuardBuyAck(ERR_NO_ENOUGH_MONEYPOINT);
        return;
    }

    TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_TASK_GUARD_BUY, PROP_TASK_GUARD_BUY, 0, 1, money);
}

// ==================================================
// 公会任务次数购买请求
// ==================================================
void TaskGameManager::OnTaskGuildResetReq(void* data)
{
    std::vector<uint32> del_list;
    RoleTaskList       add_list;

    // 公会令够不够
    TASK_GUILD_RESET_ACK sendMsg;

    uint16 errCode = m_player->SubItemOne(REWARD_REASON_GUILD_RESET, PROP_ITEM, GuildTaskConfig::Instance()->GetUseTokenNum(), GuildTaskConfig::Instance()->GetTokenId());
    if(errCode != ERR_SUCCEED)
    {
        sendMsg.err_code = errCode;
        m_player->SendToGate(MSG_TASK_GUILD_RESET_ACK, &sendMsg);
        return;
    }

    // 删除所有公会任务
    m_task_mgr->DelTaskByTypeAndState(del_list, TASK_TYPE_GUILD, 0, TASK_STATE_COMPLETE);

    // 可接任务数重置
    //m_guild_info.guild_task_count = 0;

    AddCanGameTakeTask(&add_list);
    m_task_mgr->NotifyTaskChange(del_list, add_list);
}

// ==================================================
// 公会任务领取宝箱
// ==================================================
void TaskGameManager::OnTaskGuildBoxRewardReq(void* data)
{
    TASK_GUILD_BOX_REWARD_REQ* recvMsg = (TASK_GUILD_BOX_REWARD_REQ*)data;

    // 已经领过这个宝箱
    if(IsOpenGuildTaskBox(recvMsg->score))
        return;

    // 积分不够领取这个宝箱
    if((int32)m_guild_info.guild_sroces < recvMsg->score)
        return;

    const REWARD_TABLE* boxTmp = GuildTaskConfig::Instance()->GetBoxReward(recvMsg->score);
    if(!boxTmp)
        return;

    m_guild_info.open_box_list.push_back(recvMsg->score);

    m_player->TakeInRewardTable(boxTmp);
    NotifyGameTaskRefresh();
}

// ==================================================
// 通知任务剩余数量
// ==================================================
void TaskGameManager::NotifyGameTaskNum()
{
    int32 taskNum = 0;

    // 日常
    int32 maxDailyTaskNum = MAX_DAILY_TASK_NUM;
    taskNum = maxDailyTaskNum - m_daily_info.daily_task_count;
    if(taskNum < 0) taskNum = 0;
    m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_DAILY, taskNum, maxDailyTaskNum);

    // 讨伐令
    int32 maxCrusadeTaskNum = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_10 , m_player->GetVipLevel());
    taskNum = maxCrusadeTaskNum - m_crusade_info.crusade_task_count;
    if(taskNum < 0) taskNum = 0;
    m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_CRUSADE, taskNum, maxCrusadeTaskNum);

    // 护卫
    int32 maxGuardTaskNum = 0;
    taskNum = m_guard_info.guard_task_count + m_guard_info.guard_buy_count;
    if(taskNum < 0) taskNum = 0;
    m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_PROTECT, taskNum, maxGuardTaskNum);
}

// ==================================================
// 通知任务刷新时间
// ==================================================
void TaskGameManager::NotifyGameTaskRefresh()
 {
    TASK_REFRESH_ACK sendMsg;
    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();

    sendMsg.crusade_task_flash_time = m_crusade_info.crusade_task_flash_time;
    sendMsg.daily_task_count        = m_daily_info.daily_task_count;
    sendMsg.daily_task_reset_count  = m_daily_info.daily_task_reset_count;
    sendMsg.crusade_task_count      = m_crusade_info.crusade_task_count;
    sendMsg.guard_task_count        = m_guard_info.guard_task_count;
    sendMsg.guard_buy_count         = m_guard_info.guard_buy_count;
    sendMsg.open_box                = m_open_box;
    sendMsg.guard_day_buy_count     = m_guard_info.guard_buy_day_count;

    sendMsg.guild_refresh.task_info = m_guild_info;
    Crown::CSDDateTime tomorrow = Crown::SDNow();
    tomorrow.IncDay(1);
    tomorrow.SetTime(0,0,0);
    
    int32 refrushTime = (int32)tomorrow.GetTimeValue() -  (int32)Crown::SDNow().GetTimeValue();
    sendMsg.guild_refresh.refresh_time = refrushTime > 0 ? refrushTime : 0;

    m_player->SendToGate(MSG_TASK_REFRESH_ACK, &sendMsg);

    NotifyGameTaskNum();
}

// ==================================================
// 添加主线任务
// ==================================================
void TaskGameManager::AddMainTask(RoleTaskList* add_list)
{
    const TASK_TEMPLATE* taskTmpl = NULL;

    // 检查是否已存在主线任务(主线任务只有一个)
    for (RoleTaskList::iterator taskIt = m_tasks.begin(); taskIt != m_tasks.end(); ++taskIt)
    {
        RoleTask* pTask = *taskIt;
        if(pTask->GetType() == TASK_TYPE_MAIN)
        {
            return;
        }
    }

    // 通过已完成的主线任务ID获得下一个主线任务
    taskTmpl = TaskGameConfigData::Instance()->GetMainTaskByPreID(GetCompleteMainTaskID());
    if(!taskTmpl) return;

    // 添加主线任务到角色的任务列表
    m_task_mgr->AddTask(taskTmpl, add_list);
}

// ==================================================
// 添加支线任务
// ==================================================
void  TaskGameManager::AddBranchTask(RoleTaskList* add_list)
{
    //// 获得支线任务的个数
    //int32 branch_task_count = GetTaskTotal(TASK_TYPE_BRANCH);

    //// 检查支线任务数
    //if(branch_task_count >= MAX_BRANCH_TASK_NUM)
    //    return;

    //// 遍历支线任务（这里的前提是map中的first是任务编号，而任务的顺序一定是由小到大的）
    //for(TaskTemplateMap::iterator taskIt = TaskGameConfig::Instance()->m_breanch_task.begin(); 
    //    (taskIt != TaskGameConfig::Instance()->m_breanch_task.end()) && (branch_task_count < MAX_BRANCH_TASK_NUM); 
    //    ++taskIt)
    //{
    //    // 已完成此支线任务
    //    if(IsBranchTaskFlag(taskIt->first)) continue;

    //    // 在已有的任务列表中，是否有此任务
    //    if(m_task_mgr->HasTask(taskIt->first)) continue;

    //    // 检查前置主线任务是否完成
    //    if(!m_task_mgr->CheckTaskPreCond(taskIt->second->pre_conditions))
    //    {
    //        continue;
    //    }

    //    m_task_mgr->AddTask(taskIt->second, add_list);

    //    branch_task_count++;
    //}
}

// ==================================================
// 添加日常任务
// ==================================================
void  TaskGameManager::AddDailyTask(RoleTaskList* add_list)
{
    //const TaskTemplateMap* dailyTaskTemple = &(TaskGameConfig::Instance()->m_daily_task);
    //TaskTemplateList dailyTask = m_task_mgr->GetCanAddTaskList(dailyTaskTemple);
    //uint32 dailyTotal = 0;
    //uint32 randNum = 0;

    //for(int32 dailycount = GetDailyTaskTotal(); (dailycount < MAX_DAILY_TASK_LIST_NUM) && ((m_daily_info.daily_task_count + dailycount) < MAX_DAILY_TASK_NUM);)
    //{
    //    dailyTotal = dailyTask.size();

    //    if (dailyTotal == 0)
    //        break;

    //    randNum = Crown::SDRandom(dailyTotal);

    //    m_task_mgr->AddTask(dailyTask[randNum], add_list);

    //    dailyTask.erase(dailyTask.begin() + randNum);

    //    ++dailycount;
    //}
}

// ==================================================
// 添加讨伐令任务
// ==================================================
void TaskGameManager::AddCrusadeTask(RoleTaskList* add_list)
{
    const TASK_TEMPLATE* taskTmpl = NULL;

    // 讨伐任务在循环时间内加到玩家身上的可接任务最多有几个
    int32 maxNum = SystemConfigManager::Instance()->GetGameConfigValue("task_crusade_max_num");

    // 1.判断玩家身上的讨伐令任务数是不是已经满了
    int32 taskNum = 0;
    for (RoleTaskList::iterator taskIt = m_tasks.begin(); taskIt != m_tasks.end(); ++taskIt)
    {
        RoleTask* pTask = *taskIt;
        if(pTask->GetType() == TASK_TYPE_CRUSADE )
        {
            taskNum++;
        }
    }
    if (taskNum >= maxNum)
    {
        return;
    }

    const TaskTemplateMap* crusadeTaskTemple = TaskGameConfigData::Instance()->GetCrusadeTask();
    TaskTemplateList crusadeTask = m_task_mgr->GetCanAddTaskList(crusadeTaskTemple);
    uint32 crusadeTotal = 0;
    uint32 randNum = 0;

    // 2. 把剩余的任务空缺加入可接任务(最大可接数-当前任务数)
    //for (int32 canTakeNum = GetCrusadeCanTakeTotal(); canTakeNum < loopCantake; )
    for (int32 i = 0; i < (maxNum-taskNum); i++)
    {
        crusadeTotal = crusadeTask.size();
        if (crusadeTotal == 0)
            break;

        randNum = Crown::SDRandom(crusadeTotal);

        m_task_mgr->AddTask(crusadeTask[randNum], add_list);

        crusadeTask.erase(crusadeTask.begin() + randNum);
    }
}

// ==================================================
// 添加护卫任务
// ==================================================
void TaskGameManager::AddGuardTask( RoleTaskList* add_list )
{

    //if(m_guard_info.guard_task_count + m_guard_info.guard_buy_count <= 0)
    //    return;

    //TaskTemplateMap::iterator taskIt = TaskGameConfig::Instance()->m_guard_task.begin();

    //if (taskIt == TaskGameConfig::Instance()->m_guard_task.end())
    //    return;

    //// 将可接任务放入任务列表中
    //if (GetGuardTaskTotal() < MAX_GUARD_TASK_LIST)
    //{
    //    m_task_mgr->AddTask(taskIt->second, add_list);
    //}
}

// ==================================================
// 添加公会任务
// ==================================================
void TaskGameManager::AddGuildTask(RoleTaskList* add_list)
{
    // 没有公会接不了公会任务
    //if(m_player->GetGuildID() == 0)
    //    return;

    //const TaskTemplateMap* taskTemple = &(TaskGameConfig::Instance()->m_guild_task);
    //TaskTemplateList guildTask = m_task_mgr->GetCanAddTaskList(taskTemple);
    //uint32 total = 0;
    //uint32 randNum = 0;

    //// 将可接任务放入任务列表中
    //for (int32 canTakeNum = GetGuildTaskTotal(); canTakeNum < (int32)GuildTaskConfig::Instance()->GetMaxTaskNum() ;)
    //{
    //    total = guildTask.size();

    //    if (total == 0)
    //        break;

    //    randNum = Crown::SDRandom(total);

    //    m_task_mgr->AddTask(guildTask[randNum], add_list);

    //    guildTask.erase(guildTask.begin() + randNum);

    //    ++canTakeNum;
    //}
}

// ==================================================
// 添加成就任务
// ==================================================
void TaskGameManager::AddAchiveTask(RoleTaskList* add_list)
{
    const TaskTemplateMap* achiveTaskTemple = TaskGameConfigData::Instance()->GetAchiveTask();
    TaskTemplateList achiveTask = m_task_mgr->GetCanAddTaskList(achiveTaskTemple);

    for(TaskTemplateList::const_iterator it = achiveTask.begin(); it != achiveTask.end(); it++)
    {
        m_task_mgr->AddTask(*it, add_list);
    }
}

// ==================================================
// 任务重置应答
// ==================================================
void TaskGameManager::CBDailyReset()
{
    m_daily_info.daily_task_reset_count++;
    m_daily_info.daily_task_count = 0;
    m_open_box = Crown::SDBitSaveZero(TASK_DAILY_BOX_TEN, m_open_box);
    m_open_box = Crown::SDBitSaveZero(TASK_DAILY_BOX_VIP, m_open_box);

    std::vector<uint32 > del_list;
    RoleTaskList add_list;

    AddDailyTask(&add_list);

    m_task_mgr->NotifyTaskChange(del_list, add_list);
}

// ==================================================
// 护卫购买成功
// ==================================================
void TaskGameManager::CBGuardBuy( int32 num)
{
    std::vector<uint32> del_list;
    RoleTaskList add_list;

    m_guard_info.guard_buy_count += num;

    m_guard_info.guard_buy_day_count += num;

    AddGuardTask(&add_list);

    m_task_mgr->NotifyTaskChange(del_list, add_list);

    NotifyGameTaskRefresh();
}

// ==================================================
// 保存昨天数据
// ==================================================
void TaskGameManager::SaveYesterdayData()
{
    //bool isSaveMax = m_player->GetGainbackResourceManager()->IsSaveMaxCount();
    //int32 taskNum = 0;

    //// 日常
    //int32 maxDailyTaskNum = MAX_DAILY_TASK_NUM;
    //taskNum = maxDailyTaskNum - m_daily_info.daily_task_count;
    //if(taskNum < 0) taskNum = 0;
    //m_player->GetGainbackResourceManager()->SaveYesterdayCount(OPEN_SYS_DAILY_TASK, isSaveMax? maxDailyTaskNum : taskNum);

    //// 讨伐令
    //int32 maxCrusadeTaskNum = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_10 , m_player->GetVipLevel());
    //taskNum = maxCrusadeTaskNum - m_crusade_info.crusade_task_count;
    //if(taskNum < 0) taskNum = 0;
    //m_player->GetGainbackResourceManager()->SaveYesterdayCount(OPEN_SYS_CRUSADE_TASK, isSaveMax? maxCrusadeTaskNum : taskNum);

    //// 护卫
    //int32 maxGuardTaskNum = 0;
    //taskNum = m_guard_info.guard_task_count + m_guard_info.guard_buy_count;
    //if(taskNum < 0) taskNum = 0;
    //m_player->GetGainbackResourceManager()->SaveYesterdayCount(OPEN_SYS_GUARD_TASK, isSaveMax? maxGuardTaskNum : taskNum);
}

// ==================================================
// 领取宝箱奖励
// ==================================================
void TaskGameManager::SendTaskBoxAck(uint16 errcode, uint32 box_id)
{
    TASK_RECEIVE_BOX_ACK sendMsg;
    sendMsg.errcode = errcode;
    sendMsg.box_id = box_id;
    m_player->SendToGate(MSG_TASK_RECEIVE_BOX_ACK, &sendMsg);
}

// ==================================================
// 发送日常任务重置应答
// ==================================================
void TaskGameManager::SendTaskDailyResetAck(uint16 errcode)
{
    DAILY_RESET_ACK sendMsg;
    sendMsg.err_code = errcode;
    m_player->SendToGate(MSG_TASK_DAILY_RESET_ACK, &sendMsg);
}

// ==================================================
// 护卫购买应答
// ==================================================
void TaskGameManager::SendTaskGuardBuyAck(uint16 errcode)
{
    m_player->NotifyErrorCode(errcode);
}

bool TaskGameManager::CardByTaskGuardNum( int32 num )
{
    uint8 max_buy_count = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_4 , m_player->GetVipLevel());
    if(m_guard_info.guard_buy_day_count + num >  max_buy_count)
    {
        m_player->NotifyErrorCode(ERR_FAILED);
        return false;
    }

    CBGuardBuy(num);
    return true;
}
