#include "gameholder_pch.h"
#include "task_manager.h"
#include "task_game_config_data.h"
#include "player.h"
#include "event_define.h"
#include "dungeon_manager.h"
#include "sdbit.h"
#include "item_manager.h"
#include "vip_config.h"
#include "game_util.h"
#include "trade_manager.h"
#include "gameplaytimes_notifier.h"
#include "center_handler.h"
#include "task_festive_config.h"
#include "guild_task_config.h"
#include "guild_manager.h"
#include "task_data_factory.h"
#include "task_data.h"
#include "task_manage_agent.h"
#include "system_config_manager.h"


TaskManager::TaskManager(Player* player) :
m_player(player),
m_task_game_mgr(player),
m_task_festive_mgr(player)
{
    m_task_game_mgr.SetTaskManager(this);
    m_task_festive_mgr.SetTaskManager(this);

    m_task_manage_agent = CnNew TaskManageAgent(this);

    Clear();
}

TaskManager::~TaskManager()
{
    Clear();

    CnDelete m_task_manage_agent;
}

// ==================================================
// 清空任务
// ==================================================
void TaskManager::Clear()
{
    m_task_manage_agent->Clear();

    for(RoleTaskList::iterator taskIt = m_pTasks.begin(); taskIt != m_pTasks.end(); ++taskIt)
    {
        DestoryTaskDataObject(*taskIt);
    }

    m_pTasks.clear();
    m_task_game_mgr.Clear();
    m_task_festive_mgr.Clear();
}

// 加载任务信息
void TaskManager::LoadTaskInfo(const TASK_INFO& task_info)
{
    m_task_game_mgr.LoadTaskGameInfo(task_info.task_game_info);                 // 标准任务
    m_task_festive_mgr.LoadTaskFestivalInfo(task_info.task_festival_info);      // 活动任务
}

// 填充任务信息
void TaskManager::FillTaskInfo(TASK_INFO& task_info) const 
{
    m_task_game_mgr.FillTaskGameInfo(task_info.task_game_info);                 // 标准任务
    m_task_festive_mgr.FillTaskFestivalInfo(task_info.task_festival_info);      // 活动任务
}

// 接收消息
void TaskManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_TAKE_TASK_REQ:
        {
            OnTakeTaskReq(data);            // 接任务请求
        }
        break;
    case MSG_TASK_TALK_NPC:
        {
            OnTalkNpc(data);                // 与NPC对话
        }
        break;
    case MSG_TASK_MICRO_CLIENT:
        {
            OnMicroClient(data);            // 微端登陆
        }
        break;
    case MSG_TASK_CHANGE_DUNGEON_REQ:
        {
            OnTaskChangeDungeonReq(data);   // 任务切换副本
        }
        break;
    case MSG_TASK_REWARD_REQ:
        {
            OnTaskRewardReq(data);          // 请求(领取)任务奖励
        }
        break;
    case MSG_TASK_QUALITY_REQ:
        {
            OnTaskQualityReq(data);         // 提高任务品质请求
        }
        break;
    case MSG_TASK_COMPLETE_REQ:
        {
            OnTaskCompleteReq(data);        // 立即完成任务
        }
        break;
    case MSG_TASK_CRUSADE_REFRESH_REQ:
        {
            m_task_game_mgr.OnTaskCrusadeRefreshReq();          // 刷新讨伐令任务
        }
        break;
    case MSG_TASK_CRUSADE_GIVEUP_REQ:
        {
            m_task_game_mgr.OnTaskCrusadeGiveUpReq(data);      // 放弃讨伐令任务
        }
        break;
    case MSG_TASK_RECEIVE_BOX_REQ:
        {
            m_task_game_mgr.OnTaskReceiveBoxReq(data);          // 领取宝箱
        }
        break;
    case MSG_TASK_REFRESH_TIME_REQ:
        {
            m_task_game_mgr.OnTaskRefreshTimeReq();             // 刷新任务时间
        }
        break;
    case MSG_TASK_GUARD_REFRESH_REQ:
        {
            m_task_game_mgr.OnTaskGuardRefreshReq();            // 变更护卫任务
        }
        break;
    case MSG_TASK_DAILY_RESET_REQ:
        {
            m_task_game_mgr.OnTaskDailyResetReq(data);          // 重置日常任务
        }
        break;
    case MSG_TASK_GUARD_BUY_REQ:
        {
            m_task_game_mgr.OnTaskGuardBuyReq(data);            // 护卫任务次数购买请求
        }
        break;
    case MSG_TASK_FESTIVAL_REWARD_REQ:
        {
            m_task_festive_mgr.OnFestivalRewardReq(data);       // 节日任务奖励请求
        }
        break;
    case MSG_TASK_GUILD_RESET_REQ:
        {
            m_task_game_mgr.OnTaskGuildResetReq(data);          // 公会任务重置请求
        }
        break;
    case MSG_TASK_GUILD_BOX_REWARD_REQ:
        {
            m_task_game_mgr.OnTaskGuildBoxRewardReq(data);      // 公会任务领取宝箱奖励
        }
        break;
    case MSG_TASK_MICRO_CLIENT_STATE_REQ:
        {
            OnTaskMicroClientReq(data);      // 微端登陆状态请求
        }
        break;
    default:
        break;
    }
}

// 发送任务信息
void TaskManager::SendTaskInfo()
{
    TASK_INFO task_info;
    FillTaskInfo(task_info);

    m_player->SendToGate(MSG_TASK_INFO_NTF, &task_info);
}

// 发送任务事件
void TaskManager::SendTaskEvent(uint32 event_id, int32 arg1, int32 arg2, int32 arg3)
{
    m_task_manage_agent->SendTaskEvent(event_id, arg1, arg2, arg3);
}

// 接收任务事件
void TaskManager::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    m_task_manage_agent->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);
}

// 刷新每日任务
void TaskManager::CalDailyRefresh()
{
    m_task_game_mgr.CalDailyRefresh();
    //RefreshLoopTaskState();
}

// 重新计算显示Trigger的标记
void TaskManager::CalShowTriggerFlag()
{
    //NOTIFY_OBJ_FLAG_LIST sendMsg;
    //NOTIFY_OBJ_FLAG npcFlag;
    //
    //uint32 triggerID = 0;

    //// 检查已完成的任务中，是否有需要条件显示的Trigger
    //triggerID = TaskGameConfig::Instance()->GetShowTrigger(m_task_game_mgr.GetMainTaskID());
    //if(triggerID)
    //{
    //    npcFlag.Clear();
    //    npcFlag.obj_id = triggerID;
    //    npcFlag.flag |= SHOW_FLAG_DISAPPEAR;
    //    sendMsg.list.push_back(npcFlag);
    //}

    //// 检查现有任务中，是否有需要条件显示的Trigger
    //RolePTaskList::iterator taskIt;
    //for(taskIt = m_ptasks.begin(); taskIt != m_ptasks.end(); ++taskIt)
    //{
    //    if((*taskIt)->GetType() == TASK_TYPE_MAIN)
    //    {
    //        break;      // 找到主线任务
    //    }
    //}

    //if((taskIt != m_ptasks.end()) && ((*taskIt)->GetState() == TASK_STATE_HAVETAKE))
    //{
    //    triggerID = TaskGameConfig::Instance()->GetShowTrigger((*taskIt)->GetTaskID());
    //    if(triggerID)
    //    {
    //        npcFlag.Clear();
    //        npcFlag.obj_id = triggerID;
    //        npcFlag.flag = 0;
    //        sendMsg.list.push_back(npcFlag);
    //    }
    //}

    //if(sendMsg.list.size() < 1)
    //    return;

    //m_player->SendToGate(MSG_NOTIFY_TRIGGER_FLAG, &sendMsg);
}

// 通过TriggerID获得副本ID
uint32 TaskManager::GetDungeonIDByTrigger(uint32 triggerID)
{
    uint32 dungeonID = 0;

    for(RoleTaskList::iterator taskIt = m_pTasks.begin(); taskIt != m_pTasks.end(); ++taskIt)
    {
        RoleTask* pTask = (*taskIt);
        if(!pTask->NeedTriggerTaskTarget()) 
            continue;

        dungeonID = pTask->GetDungeonIDByTrigger(triggerID);
        if(dungeonID > 0)
            return dungeonID;
    }

    return dungeonID;
}

// 检查是否需要隐藏此触发器NPC对话
bool TaskManager::NeedDisappearTrigger(uint32 triggerID)
{
    //uint32 taskID = 0;

    //for (RolePTaskList::iterator taskIt = m_ptasks.begin(); taskIt != m_ptasks.end(); ++taskIt)
    //{
    //    RoleTask* pTask = *taskIt;
    //    if( (pTask->GetType() == TASK_TYPE_MAIN) && (pTask->GetState() == TASK_STATE_HAVETAKE) )
    //    {
    //        taskID = (*taskIt)->GetTaskID();
    //        break;
    //    }
    //}

    //if(TaskGameConfig::Instance()->NeedShowTrigger(triggerID, taskID))
    //{
    //    return false;
    //}

    return true;
}

// 加载任务记录
void TaskManager::LoadRoleTask(const std::vector<TASK_RECORD>& in_list)
{
    for(std::vector<TASK_RECORD>::const_iterator taskIt = in_list.begin(); taskIt != in_list.end(); ++taskIt)
    {
        const TASK_RECORD& taskRecord = *taskIt;
        const TASK_TEMPLATE* taskTemplate = NULL;

        if(taskRecord.task_id == 0)
            continue;

        uint8 task_type = QueryTaskTypeById(taskRecord.task_id);
        CnAssert(taskRecord.type == 0 || taskRecord.type == task_type);

        if(task_type == TASK_TYPE_FESTIVAL)
        {   // 活动任务
            taskTemplate = TaskFestiveConfig::Instance()->GetTaskTmpl(m_player->GetCenterSvrID(), taskRecord.task_id);
        }
        else if(IsNormalTask(task_type))
        {   // 标准任务
            taskTemplate = TaskGameConfigData::Instance()->GetTaskTemplate(taskRecord.task_id);
        }
        else
        {
            CnAssert(false);
            taskTemplate = TaskGameConfigData::Instance()->GetTaskTemplate(taskRecord.task_id);
        }
            
        // 如果找到不到对应的任务模板，则给一个空任务模板
        if(!taskTemplate) 
        {
            taskTemplate = TaskGameConfigData::Instance()->GetEmptyTaskTemplate();
        }

        RoleTask* pTask = CreateTaskDataObject(taskTemplate, m_player);
        pTask->LoadTaskRecord(*taskIt);     // 填充任务

        AddRoleTask(pTask);
    }
 }

// 重新加载节日任务
void TaskManager::ReloadTask()
{
    TASK_INFO task_info;
    FillTaskInfo(task_info);    // 先对当前任务回写
    Clear();                    // 清除任务数据

    LoadTaskInfo(task_info);    // 再次读取当前任务
    SendTaskInfo();
}

// 填充任务记录列表
void TaskManager::FillTaskRecordList(const RoleTaskList& in_list, std::vector<TASK_RECORD>& out_list) const
{
    TASK_RECORD task_record;

    for(RoleTaskList::const_iterator taskIt = in_list.begin(); taskIt != in_list.end(); ++taskIt)
    {
        (*taskIt)->FillTaskRecord(task_record);
        out_list.push_back(task_record);
    }
}

// 通知任务内容更新
void TaskManager::NotifyTaskUpdate(RoleTask* task)
{
    TASK_UPDATE_INFO sendMsg;
    RoleTaskList list;

    if(!task) return;

    list.push_back(task);

    FillTaskRecordList(list, sendMsg.list);

    m_player->SendToGate(MSG_TASK_UPDATE_NTF, &sendMsg);
}

// 通知任务内容更新
void TaskManager::NotifyTaskUpdate(RoleTaskList& list)
{
    TASK_UPDATE_INFO sendMsg;

    if(!list.size()) return;

    FillTaskRecordList(list, sendMsg.list);

    m_player->SendToGate(MSG_TASK_UPDATE_NTF, &sendMsg);
}

// 通知任务添加或删除
void TaskManager::NotifyTaskChange(std::vector<uint32>& del_list, RoleTaskList& add_list)
{
    TASK_CHANGE_INFO sendMsg;

    if(!del_list.size() && !add_list.size()) return;

    sendMsg.del_list = del_list;

    FillTaskRecordList(add_list, sendMsg.add_list);

    m_player->SendToGate(MSG_TASK_CHANGE_NTF, &sendMsg);

    m_task_game_mgr.NotifyGameTaskRefresh();
}

// ==================================================
// 通知任务剩余数量
// ==================================================
void TaskManager::NotifyGameTaskNum()
{
    m_task_game_mgr.NotifyGameTaskNum();
}

//设置任务状态
void TaskManager::SetTaskHaveTakeState(RoleTask* task)
{
    task->SetTaskHaveTakeState();
}

// ==================================================
// 在已有的任务列表中，是否有此任务
// ==================================================
bool TaskManager::HasTask(uint32 taskID)
{
    for(RoleTaskList::iterator taskIt = m_pTasks.begin(); taskIt != m_pTasks.end(); ++taskIt)
    {
        if(taskID == (*taskIt)->GetTaskID())
        {
            return true;
        }
    }

    return false;
}

// ==================================================
// 是否已接好友任务
// ==================================================
bool TaskManager::HasFriendTask()
{
    //return GetTaskByIdAndState(TaskGameConfig::Instance()->GetFriendTaskID(), TASK_STATE_HAVETAKE) > 0;
    return true;
}

//是否需要统计在线总时长
bool TaskManager::IsStartCountOnlineTime()
{
    for (RoleTaskList::iterator taskIt = m_pTasks.begin(); taskIt != m_pTasks.end(); ++taskIt)
    {
        RoleTask* pTask = *taskIt;
        if(pTask->IsNeedOnlineTime())
            return true;
    }

    return false;
}

// 判定任务的前置条件
bool TaskManager::CheckTaskPreCondition(const TASK_TEMPLATE* taskTmpl, uint32* err)
{
    // 判定任务的前置条件
    for(std::vector<TASK_CONDITION>::const_iterator condIt = taskTmpl->pre_conditions.begin(); condIt != taskTmpl->pre_conditions.end(); ++condIt)
    {
        if(TASK_PRECOND_MAIN_TASKID == condIt->type)
        {
            if(condIt->value > m_task_game_mgr.GetCompleteMainTaskID())
            {
                // 前置主线任务没有完成
                return false;
            }
        }
        else if(TASK_PRECOND_LEVEL == condIt->type)
        {
            if(condIt->value > (uint32)m_player->GetLevel())
            {
                // 角色等级不够
                if(err)
                    *err = ERR_NO_ENOUGH_PLAYER_LEVEL;
                return false;
            }
        }
        else if(TASK_PRECOND_BRANCH_TASKID == condIt->type)
        {
            if(!m_task_game_mgr.IsBranchTaskFlag(condIt->value))
            {
                // 前置支线任务没有完成
                return false;
            }
        }
        else if (TASK_PRECOND_LESS_LEVEL == condIt->type)
        {
            if(condIt->value < (uint32)m_player->GetLevel())
            {
                // 角色等级过高
                return false;
            }
        }
    }

    return true;
}

// ==================================================
// 联动可添加任务
// ==================================================
void TaskManager::LinkageCanTakeTask()
{
    // 联动可接任务
    RoleTaskList add_list;
    AddCanTakeTask(&add_list);

    if(add_list.size() > 0)
    {
        std::vector<uint32> del_list;
        NotifyTaskChange(del_list, add_list);
    }
}

// ==================================================
// 添加可以接受的任务（主线和支线任务）
// ==================================================
void TaskManager::AddCanTakeTask(RoleTaskList* add_list)
{
    m_task_game_mgr.AddCanGameTakeTask(add_list);
    m_task_festive_mgr.AddCanTakeTaskFestival(add_list);
}

// ==================================================
// 任务切换副本
// ==================================================
void TaskManager::OnTaskChangeDungeonReq(void* data)
{
    TASK_CHANGE_DUNGEON_REQ* recvMsg = (TASK_CHANGE_DUNGEON_REQ*)data;
    if(!recvMsg) 
        return;

    // 查找是否存在指定任务
    RoleTask* pTask = GetTaskByIdAndState(recvMsg->task_id, TASK_STATE_HAVETAKE);
    if(!pTask)
        return;

    // 查找任务目标
    uint32 dungeonID = pTask->GetChangeDungeonID();
    if(!dungeonID)  // 如果没有副本ID 则 dungeonID为 0
        return;

    uint32 dungeonClass = GetDungeonClass(dungeonID);
    //if(dungeonClass == DUNGEON_CLASS_CITY_ID)
    //    return;     // 大地图上的杀怪任务，不跳转副本
    
    CHANGE_DUNGEON_REQ changeDungeon;
    changeDungeon.dungeon_id = dungeonID;
    m_player->OnRecv(MSG_CHANGE_DUNGEON_REQ, &changeDungeon);
}

RoleTask* TaskManager::GetTaskByIdAndState(uint32 taskId, uint32 state)
{
    RoleTask* pTask = NULL;
    for(RoleTaskList::iterator taskIt = m_pTasks.begin(); taskIt != m_pTasks.end(); ++taskIt)
    {
        if((*taskIt)->GetTaskID() != taskId)
            continue;

        if((*taskIt)->GetState() != state)
            continue;

        pTask = *taskIt;
        break;
    }
    return pTask;
}

RoleTask* TaskManager::GetTaskById(uint32 taskId)
{
    RoleTask* pTask = NULL;
    for(RoleTaskList::iterator taskIt = m_pTasks.begin(); taskIt != m_pTasks.end(); ++taskIt)
    {
        if((*taskIt)->GetTaskID() != taskId)
            continue;

        pTask = *taskIt;
        break;
    }
    return pTask;
}

// ==================================================
// 接任务请求
// ==================================================
void TaskManager::OnTakeTaskReq(void* data)
{
    TAKE_TASK_REQ* recvMsg = (TAKE_TASK_REQ*)data;

    if(!recvMsg) return;

    RoleTask* roleTask = GetTaskByIdAndState(recvMsg->task_id, TASK_STATE_CANTAKE);

    if(!roleTask)
    {
        m_player->NotifyErrorCode(ERR_UNFOUND_TASK);
        return;
    }

    // 检查前置角色等级是否达到
    uint32 preConditionErr = 0;
    if(!roleTask->CheckTaskPreCondition(&preConditionErr))
    {
        m_player->NotifyErrorCode(preConditionErr);
        return;
    }

    // 任务接受个数不能超过每天最大值
    if(!CanTakeTaskByTaskCount(roleTask->GetType()))
        return;

    // 检查包裹是否已满
    if(!CanAddGive(roleTask->GetGiveReward()))
    {
        m_player->NotifyErrorCode(ERR_NO_ENOUGH_BAGPOS);
        return;
    }

    // 检查体力是否够用
    if (!ValidateVigor(roleTask))
    {
        m_player->NotifyErrorCode(ERR_NO_ENOUGH_VIGOR);
        return;
    }

    // 根据任务目标开启副本(剧情副本)
    roleTask->OpenDungeonByTask();

    // 给予物品
    GetGives(roleTask->GetGiveReward());

    // 更改任务状态
    roleTask->SetState(TASK_STATE_HAVETAKE);

    // 任务关联
    roleTask->InitTaskLinkage();

    // 接受讨伐令任务
    if(roleTask->GetType() == TASK_TYPE_CRUSADE)
    {
        RoleTaskList add_list;
        std::vector<uint32> del_list;

        m_task_game_mgr.IncCrusadeTaskCount();

        AddCanTakeTask(&add_list);

        NotifyTaskChange(del_list, add_list);

       
        // 接讨伐令任务消耗体力点
        uint32 cost = SystemConfigManager::Instance()->GetGameConfigValue("task_crusade_take_cost");
        if(cost > 0)
        {
            m_player->SubVigorValue(cost, 0);
        }
        

        // 活跃度
        //m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_ACCEPT_CRUSADE, 1, 0, 0);
    }

    // 重新计算显示Trigger的标记
    CalShowTriggerFlag();

    // 添加任务日志
    OnSendTaskLog(MSG_TAKE_TASK_REQ ,roleTask);

    // 通知任务变更
    NotifyTaskUpdate(roleTask);
}

// ==================================================
// 与NPC对话
// ==================================================
void TaskManager::OnTalkNpc(void* data)
{
    TASK_TALK_NPC* recvMsg = (TASK_TALK_NPC*)data;

    if(!recvMsg) return;

    m_player->SendTaskEvent(TASK_EVENT_TALK_NPC, recvMsg->npc_id, 0);
}

// ==================================================
// 微端登陆
// ==================================================
void TaskManager::OnMicroClient(void* data)
{
    m_player->SendTaskEvent(TASK_EVENT_MICRO_CLIENT, 0, 0);
}

// ==================================================
// 请求任务奖励
// ==================================================
void TaskManager::OnTaskRewardReq(void* data)
{
    TASK_REWARD_REQ* recvMsg = (TASK_REWARD_REQ*)data;
    std::vector<uint32> del_list;
    RoleTaskList       add_list;
    uint16 errcode = ERR_UNFOUND_TASK;

    if(!recvMsg) return;

    RoleTask* pRewardTask = GetTaskById(recvMsg->task_id);
    if(!pRewardTask)
    {
        errcode = ERR_UNFOUND_TASK;
        goto _ERROR;
    }

    if(pRewardTask->GetState() != TASK_STATE_COMPLETE)
    {
        errcode = ERR_TASK_UNFINSH;
        goto _ERROR;
    }

    // 检查任务是否在有效时间内
    if(!pRewardTask->IsInValidTime())
    {
        errcode = ERR_FAILED;
        goto _ERROR;
    }

    // 检查包裹是否已满
    if(!CanAddReward(pRewardTask->GetReward()))
    {
        m_player->NotifyErrorCode(ERR_NO_ENOUGH_BAGPOS);
        return;
    }

    // 任务奖励
    pRewardTask->SetState(TASK_STATE_REWARD);
    GetRewards(pRewardTask);

    // 通知任务状态改变
    NotifyTaskUpdate(pRewardTask);

    OnSendTaskLog(MSG_TASK_REWARD_REQ, pRewardTask);

    // 如果是普通任务，则需要删除此任务
    if(IsNormalTask(pRewardTask->GetType()))
    {   
        // 领取奖励以后，主线任务需要把任务给删掉
        uint8 taskType = pRewardTask->GetType();
        if(taskType == TASK_TYPE_MAIN)
        {
            del_list.push_back(pRewardTask->GetTaskID());
            DelRoleTask(pRewardTask);           // 删除任务
        }
    }
    else
    {
        // 如果环任务次数小于上限，则充值任务状态和任务目标计数
        if(pRewardTask->CanLoop())
        {
            pRewardTask->DoLoop();
        }

        // 通知客户端循环次数变更
        NotifyTaskUpdate(pRewardTask);
    }

    // 添加可以添加的任务
    AddCanTakeTask(&add_list);

    // 通知任务
    NotifyTaskChange(del_list, add_list);

    errcode = ERR_SUCCEED;

_ERROR:
    SendTaskRewardAck(errcode, recvMsg->task_id);
}

// ==================================================
// 提升任务品质请求
// ==================================================
void TaskManager::OnTaskQualityReq( void* data )
{
    //TASK_QUALITY_REQ* recvMsg = (TASK_QUALITY_REQ*) data;
    //if (!recvMsg)   return;

    //int32 maxQuality = TaskGameConfig::Instance()->GetQualityProList(TASK_TYPE_DAILY).size() - 1;

    //// 找到此日常任务
    //RoleTask* roleTask = 0;
    //for(RolePTaskList::iterator taskIt = m_ptasks.begin(); taskIt != m_ptasks.end(); ++taskIt)
    //{
    //    if((*taskIt)->GetTaskID() != recvMsg->task_id) continue;

    //    if((*taskIt)->GetState() == TASK_STATE_CANTAKE) continue;

    //    if ((*taskIt)->GetType() != TASK_TYPE_DAILY) continue;

    //    roleTask = *taskIt;
    //    break;
    //}

    //if (!roleTask)  return;

    //// 是否是最高品质
    //if ( roleTask->GetQuality() >= maxQuality )
    //{
    //    return;
    //}

    ////// 检查升级石
    ////if (!m_player->GetItemManager()->GetItemNum(TASK_UPGRADE_STONE))
    ////{
    ////    SendTaskItemAck(ERR_UNFOUND_ITEM, TASK_UPGRADE_STONE);
    ////    return;
    ////}

    //// 消耗升级石
    ////m_player->GetBag()->DelItemByID(TASK_UPGRADE_STONE, 1);

    //// 品质提高
    //roleTask->IncQuality();

    //// 更新任务信息
    //RolePTaskList pRoleTaskList;
    //pRoleTaskList.clear();
    //pRoleTaskList.push_back(roleTask);
    //NotifyTaskUpdate(pRoleTaskList);
}
// ==================================================
// 初始化任务状态
// ==================================================
void TaskManager::InitTaskState()
{
    RoleTaskList list;
    bool effect = false;
    for(RoleTaskList::iterator taskIt = m_pTasks.begin(); taskIt != m_pTasks.end(); ++taskIt)
    {
        effect = (*taskIt)->InitTaskLinkage();
        if(effect)
        {
            list.push_back(*taskIt);
        }
    }

    NotifyTaskUpdate(list);
}
// ==================================================
// 立即完成任务
// ==================================================
void TaskManager::OnTaskCompleteReq( void* data )
{
    TASK_COMPLETE_REQ* recvMsg = (TASK_COMPLETE_REQ*)data;
    if (!recvMsg)   return;

    // 找到此日常任务
    RoleTask* roleTask = 0;
    for(RoleTaskList::iterator taskIt = m_pTasks.begin(); taskIt != m_pTasks.end(); ++taskIt)
    {
        if((*taskIt)->GetTaskID() != recvMsg->task_id) continue;

        if((*taskIt)->GetState() != TASK_STATE_HAVETAKE) continue;

        roleTask = *taskIt;
        break;
    }

    if (!roleTask)  return;

    if(roleTask->GetType() == TASK_TYPE_GUARD)
    {
        int32 bexe =  VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_5 , m_player->GetVipLevel());
        if(!bexe)
        {
            m_player->NotifyErrorCode(ERR_VIP_OPERATION);
            return;
        }
    }

    if (roleTask->GetType() == TASK_TYPE_DAILY || roleTask->GetType() == TASK_TYPE_CRUSADE || roleTask->GetType() == TASK_TYPE_GUARD)
    {
        //// 检查消耗品
        //if ( !m_player->GetItemManager()->GetItemNum(TASK_COMPLETE_STONE) )
        //{
        //    SendTaskItemAck(ERR_UNFOUND_ITEM, TASK_COMPLETE_STONE);
        //    return;
        //}

        // 消耗消耗品
        //m_player->GetBag()->DelItemByID(TASK_COMPLETE_STONE, 1);

        // 完成任务
        CompleteTask(roleTask);

        RoleTaskList plist;
        for (RoleTaskList::iterator taskIt = m_pTasks.begin(); taskIt != m_pTasks.end(); ++taskIt)
        {
            RoleTask* rotask = *taskIt;
            if ((rotask->GetType() == TASK_TYPE_DAILY) || 
                (rotask->GetType() == TASK_TYPE_CRUSADE) || 
                (rotask->GetType() == TASK_TYPE_GUARD))     // filter by type
            {                
                plist.push_back(rotask);
            }
        }
        // 计算任务标记
        NotifyTaskUpdate(plist);

        OnSendTaskLog(MSG_TASK_COMPLETE_REQ ,roleTask);
    }
}

void TaskManager::OnTaskFestivalTemplateListReq()
{
    TASK_FESTIVAL_TEMPLATE_LIST sendMsg;
    TaskFestiveConfig::Instance()->FillTaskFestivalList(m_player->GetCenterSvrID(), sendMsg);
    m_player->SendToGate(MSG_TASK_FESTIVAL_TMPL_LIST_ACK, &sendMsg);
}

void TaskManager::OnSendTaskLog(uint32 agreementid , RoleTask* roleTask)
{
    if(!roleTask)
        return;

    LOG_REQ msg;
    msg.log = WriterToLogByTask(m_player->GetLevel(), roleTask->GetType(), roleTask->GetTaskID(), roleTask->GetState(), roleTask->GetQuality());
    msg.idflag = LOG_TASK;
    msg.agreementid = agreementid;
    CenterHandler::Instance()->SendByPlayer(m_player, MSG_LOG_REQ, &msg, m_player->GetPlayerID());

    // 记录10级以下的任务行为
    if(m_player->GetLevel() < 10)
        m_player->SendBehaviorLog("task id=%d s=%d", roleTask->GetTaskID(), roleTask->GetState());
}

// 检查是否可以给予物品
bool TaskManager::CanAddGive(const REWARD_TABLE& givesReward)
{
    ItemManager* itemMgr = m_player->GetItemManager();
    CnAssert(itemMgr);

    if(itemMgr->CanAddRewardTable(&givesReward))
        return true;
    else
        return false;
}

// ==================================================
// 检查是否可以添加奖励
// ==================================================
bool TaskManager::CanAddReward(const REWARD_TABLE& taskReward)
{
    ItemManager* ItemMgr = m_player->GetItemManager();
    CnAssert(ItemMgr);

    if(ItemMgr->CanAddRewardTable(&taskReward))
        return true;
    else
        return true;
}

// ==================================================
// 给予物品
// ==================================================
void TaskManager::GetGives(const REWARD_TABLE& givesRewards)
{
    m_player->TakeInRewardTable(&givesRewards);
}

// ==================================================
// 获得奖励
// ==================================================
void TaskManager::GetRewards(RoleTask* roleTask)
{
    roleTask->OnPreRewards();

    roleTask->OnRewards();
}

bool TaskManager::ValidateVigor(RoleTask* taskIt)
{
    if (taskIt->GetType() == TASK_TYPE_CRUSADE)
    {
        // 接讨伐令任务消耗体力点
        int32 cost = SystemConfigManager::Instance()->GetGameConfigValue("task_crusade_take_cost");
        if(m_player->GetVigorValue() < cost){
            return false;
        }
    }

    return true;
}

// ==================================================
// 通过GM命令添加任务
// ==================================================
void TaskManager::GmAddTask(uint32 taskID)
{
    const TASK_TEMPLATE* taskTmpl = TaskGameConfigData::Instance()->GetTaskTemplate(taskID);
    std::vector<uint32> del_list;
    RoleTaskList       add_list;

    if(!taskTmpl) return;

    if(taskTmpl->task_type == TASK_TYPE_MAIN)
    {
        for(RoleTaskList::iterator taskIt = m_pTasks.begin(); taskIt != m_pTasks.end(); ++taskIt)
        {
            if((*taskIt)->GetType() == TASK_TYPE_MAIN)
            {
                del_list.push_back((*taskIt)->GetTaskID());
                DelRoleTask(*taskIt);
                break;
            }
        }

        if(taskID == 10000)
        {
            m_task_game_mgr.SetCompleteMainTaskID(0);
        }
        else
        {
            m_task_game_mgr.SetCompleteMainTaskID(taskID - 1);
        }
    }
    else if(taskTmpl->task_type == TASK_TYPE_BRANCH)
    {
        for(RoleTaskList::iterator taskIt = m_pTasks.begin(); taskIt != m_pTasks.end(); ++taskIt)
        {
            if((*taskIt)->GetTaskID() == taskTmpl->task_id)
            {
                m_task_game_mgr.ResetBranchTaskFlag(taskTmpl->task_id);
                del_list.push_back((*taskIt)->GetTaskID());
                DelRoleTask(*taskIt);
                break;
            }
        }
    }

    // 添加任务
    AddTask(taskTmpl, &add_list);

    // 计算任务标记
    //CalTaskTargetFlag();

    // 通知任务
    NotifyTaskChange(del_list, add_list);
}

// ==================================================
// 强制完成所有任务(仅主线任务)
// ==================================================
void TaskManager::GmCompleteTask()
{
    bool effect = false;
    RoleTaskList list;
    std::list<uint32> taskList;

    // 接可接任务
    // ==================================================
    for(RoleTaskList::iterator taskIt = m_pTasks.begin(); taskIt != m_pTasks.end(); ++taskIt)
    {
        TAKE_TASK_REQ takeReq;
        if((*taskIt)->GetState() == TASK_STATE_CANTAKE && (*taskIt)->GetType() == TASK_TYPE_MAIN)
        {
            takeReq.task_id = (*taskIt)->GetTaskID();
            OnTakeTaskReq(&takeReq);
        }
    }

    // 完成任务
    // ==================================================
    for(RoleTaskList::iterator taskIt = m_pTasks.begin(); taskIt != m_pTasks.end(); ++taskIt)
    {
        effect = true;

        if((*taskIt)->GetType() != TASK_TYPE_MAIN)
            continue;

        CompleteTask(*taskIt);

        if(effect)
        {
            list.push_back(*taskIt);
        }
    }

    NotifyTaskUpdate(list);

    // 接可接任务
    // ==================================================
    for(RoleTaskList::iterator taskIt = m_pTasks.begin(); taskIt != m_pTasks.end(); ++taskIt)
    {
        if((*taskIt)->GetType() == TASK_TYPE_FESTIVAL)
        {
            continue;
        }

        if((*taskIt)->GetState() == TASK_STATE_COMPLETE)
        {
            taskList.push_back((*taskIt)->GetTaskID());
        }
    }

    // 领取奖励
    if(false)
    {
        for(std::list<uint32>::iterator it = taskList.begin(); it != taskList.end(); ++it)
        {
            TASK_REWARD_REQ rewardReq;
            rewardReq.task_id = *it;
            OnTaskRewardReq(&rewardReq);
        }
    }
}

// ==================================================
// 接受任务
// ==================================================
void TaskManager::GmTakeTask( uint32 taskID )
{
    TAKE_TASK_REQ taskReq;    
    taskReq.task_id = taskID;
    OnTakeTaskReq(&taskReq);
}

// ==================================================
// 切换副本
// ==================================================
void TaskManager::GmTaskChangeDugeon( uint32 taskID )
{
    TASK_CHANGE_DUNGEON_REQ taskReq;
    taskReq.Clear();
    taskReq.task_id = taskID;
    OnTaskChangeDungeonReq(&taskReq);
}

// ==================================================
// 请求任务奖励
// ==================================================
void TaskManager::GmTaskReward( uint32 taskID )
{
    TASK_QUALITY_REQ taskReq;
    taskReq.Clear();
    taskReq.task_id = taskID;
    OnTaskRewardReq(&taskReq);
}

// ==================================================
// 提高任务品质
// ==================================================
void TaskManager::GmTaskQualityReq( uint32 taskID )
{
    TASK_COMPLETE_REQ taskReq;
    taskReq.Clear();
    taskReq.task_id = taskID;
    OnTaskQualityReq(&taskReq);
}

// ==================================================
// 立即完成任务
// ==================================================
void TaskManager::GmTaskComplete( uint32 taskID )
{
    TASK_COMPLETE_REQ taskReq;
    taskReq.Clear();
    taskReq.task_id = taskID;
    OnTaskCompleteReq(&taskReq);
}

// ==================================================
// 刷新讨伐令任务
// ==================================================
void TaskManager::GmTaskRefresh()
{
    m_task_game_mgr.OnTaskCrusadeRefreshReq();
}

// ==================================================
// 领取宝箱
// ==================================================
void TaskManager::GmTaskReceiveBox(uint32 boxID)
{
    TASK_RECEIVE_BOX_REQ taskReq;
    taskReq.Clear();
    taskReq.box = boxID;
    m_task_game_mgr.OnTaskReceiveBoxReq(&taskReq);
}

// ==================================================
// 刷新任务时间
// ==================================================
void TaskManager::GmTaskRefreshTimeReq()
{
    m_task_game_mgr.OnTaskRefreshTimeReq();
}

// ==================================================
// 变更护卫任务
// ==================================================
void TaskManager::GmTaskGuardRefresh()
{
    m_task_game_mgr.OnTaskGuardRefreshReq();
}

void TaskManager::RefreshLoopTaskState()
{
    RoleTaskList plist;
    bool b_exe = false;
    for(RoleTaskList::iterator taskIt = m_pTasks.begin(); taskIt != m_pTasks.end(); ++taskIt)
    {
        RoleTask* pTask = (*taskIt);
        if(pTask->RefreshLoopTaskState())
        {
            plist.push_back(*taskIt);
            b_exe = true;
        }
    }

    if(b_exe)
    {
        NotifyTaskUpdate(plist);
    }
}

// ==================================================
// 交换任务
// ==================================================
void TaskManager::OnExchangeItem(uint32 itemID)
{
    m_task_manage_agent->OnExchangeItem(itemID);
}
// ==================================================
// 结算副本
// ==================================================
void TaskManager::OnSettleDungeon(uint32 dungeonID, bool success, bool raids)
{
    m_task_manage_agent->OnSettleDungeon(dungeonID, success, raids);
}

// ==================================================
// 添加任务
// ==================================================
void TaskManager::AddTask(const TASK_TEMPLATE* taskTmpl, RoleTaskList* add_list)
{
    if(!taskTmpl) return;

    RoleTask* pTask = CreateTaskDataObject(taskTmpl, m_player);

    pTask->OnAdd();

    // 添加新的任务到列表中
    AddRoleTask(pTask);

    // 添加到新添加任务列表中
    add_list->push_back(pTask);

    // 初始化任务目标状态
    pTask->InitTaskLinkage();
}
// ==================================================
// 添加角色任务
// ==================================================
void TaskManager::AddRoleTask(RoleTask* pTask)
{
    CnAssert( !pTask->IsDispose() );
    // 任务对象的释放由TaskManager管理
    if(pTask->GetType() == TASK_TYPE_FESTIVAL)
        m_task_festive_mgr.AddTask(pTask);
    else if(IsNormalTask(pTask->GetType()))
        m_task_game_mgr.AddTask(pTask);
    else
        CnAssert(false);

    m_pTasks.push_back(pTask);
}

// ==================================================
// 删除角色任务
// ==================================================
void TaskManager::DelRoleTask(RoleTask* pTask)
{
    CnAssert( !pTask->IsDispose() );
    // 在所有任务列表中删除
    for(RoleTaskList::iterator taskIt = m_pTasks.begin(); taskIt != m_pTasks.end(); ++taskIt)
    {
        CnAssert( !(*taskIt)->IsDispose() );
        if(*taskIt != pTask) 
            continue;
        m_pTasks.erase(taskIt);
        break;
    }

    // 在子管理器中删除
    if(pTask->GetType() == TASK_TYPE_FESTIVAL)
    {
        m_task_festive_mgr.DelTask(pTask);
    }
    else if(IsNormalTask(pTask->GetType()))
    {
        m_task_game_mgr.DelTask(pTask);
    }
    else
        CnAssert(false);

    DestoryTaskDataObject(pTask);       // 删除任务对象
}
// 删除一类任务
void TaskManager::DelTaskByTypeAndState(std::vector<uint32>& del_list, uint8 task_type, uint8 task_state, uint8 task_nostate)
{
    del_list.clear();
    
    std::vector<RoleTask*>  delTasks;
    for(RoleTaskList::iterator it = m_pTasks.begin(); it != m_pTasks.end();  ++it)
    {
        RoleTask* pTask = *it;
        if(pTask->GetType() != task_type)
            continue;

        // 这是给成就的每日任务做处理
        if(pTask->GetType() == TASK_TYPE_ACHIVE && pTask->GetSubType() != 9)
            continue;

        if( (task_state == 0 && pTask->GetState() != task_nostate)
            || (pTask->GetState() == task_state) )
        {
            del_list.push_back(pTask->GetTaskID());
            delTasks.push_back(pTask);
        }
    }
    for(std::vector<RoleTask*>::iterator dIter = delTasks.begin(); dIter != delTasks.end(); dIter++)
    {
        DelRoleTask(*dIter);
    }
}

// ==================================================
// 完成任务 把任务目标设置成最大
// ==================================================
void TaskManager::CompleteTask(RoleTask* task)
{
    if (!task)  
        return;
    task->MakeTaskComplete();
    task->CheckTaskComplete();
}

// ==================================================
// 返回主线任务ID
// ==================================================
uint32 TaskManager::GetMainTaskID()
{
    return m_task_game_mgr.GetCompleteMainTaskID();
}

// ==================================================
// 获得护卫任务次数
// ==================================================
uint32 TaskManager::GetGuardTaskNum()
{
    return m_task_game_mgr.GetGuardTaskNum();
}


uint32 TaskManager::GetAchiveCount()
{
    uint32 count = 0;
    RoleTaskList::const_iterator cIt = m_pTasks.begin();
    for (cIt; cIt != m_pTasks.end(); cIt++)
    {
        if((*cIt)->GetType() == TASK_TYPE_ACHIVE && (*cIt)->GetState() == TASK_STATE_REWARD)
        {
            count += 1;
        }
    }

    return count;
}

// ==================================================
// 任务重置应答
// ==================================================
void TaskManager::CBDailyReset()
{
    m_task_game_mgr.CBDailyReset();
}

// ==================================================
// 护卫任务购买应答
// ==================================================
void TaskManager::CBGuardBuy()
{
    m_task_game_mgr.CBGuardBuy(1);
}

// ==================================================
// 发送任务奖励信息
// ==================================================
void TaskManager::SendTaskRewardAck(uint16 errcode, uint32 taskID)
{
    TASK_REWARD_ACK sendMsg;
    sendMsg.errcode = errcode;
    sendMsg.task_id = taskID;
    m_player->SendToGate(MSG_TASK_REWARD_ACK, &sendMsg);
}

// ==================================================
// 发送任务奖励信息
// ==================================================
void TaskManager::SendTaskItemAck(uint16 errcode, uint32 item_id)
{
    TASK_ITEM_ACK sendMsg;
    sendMsg.errcode = errcode;
    sendMsg.item_id = item_id;
    m_player->SendToGate(MSG_TASK_ITEM_ACK, &sendMsg);
}

// ==================================================
// 可添加不重复任务列表
// ==================================================
TaskTemplateList TaskManager::GetCanAddTaskList(const TaskTemplateMap* taskList)
{
    // 任务模板列表
    TaskTemplateList canAddTasks;
    canAddTasks.clear();

    for (TaskTemplateMap::const_iterator it = taskList->begin(); it != taskList->end(); it++)
    {
        const TASK_TEMPLATE* taskTemplate = it->second;
        // 检查任务的前置条件
        if(!CheckTaskPreCondition(taskTemplate)) 
            continue;

        // 检查是否已有此任务
        if (HasTask(it->first))
            continue;

        canAddTasks.push_back(it->second);
    }

    return canAddTasks;
}

// 是否能接受任务(任务数超过最大值)
bool TaskManager::CanTakeTaskByTaskCount(uint8 type)
{
    if(type == TASK_TYPE_CRUSADE)
    {
        // 用户单次循环时间内，最多同时可以接几个任务
        int32 maxActive = SystemConfigManager::Instance()->GetGameConfigValue("task_crusade_max_active");
        
        int32 haveTake = 0;
        for (RoleTaskList::const_iterator cIter = m_pTasks.begin(); cIter != m_pTasks.end(); cIter++)
        {
            if((*cIter)->GetType() == TASK_TYPE_CRUSADE && (*cIter)->GetState() == TASK_STATE_HAVETAKE)
            {
                haveTake++;
            }
        }

        if(haveTake >= maxActive)
        {
            m_player->NotifyErrorCode(ERR_TASK_REACH_MAX_COUNT);
            return false;
        }

        //// 讨伐令任务接受个数不能超过每天的最大值
        //uint32 maxCrusadeTaskNum = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_10 , m_player->GetVipLevel());
        //if(m_task_game_mgr.GetCrusadeTaskCount() >= maxCrusadeTaskNum)
        //{
        //    m_player->NotifyErrorCode(ERR_TASK_REACH_MAX_COUNT);
        //    return false;
        //}

        //// 检查讨伐令任务列表数
        //if(m_task_game_mgr.GetCrusadeHaveTaskTotal() >= MAX_CRUSADE_TASK_LIST_NUM)
        //    return false;
    }
    else if(type == TASK_TYPE_GUILD)
    {
        // 检查公会任务接受个数是否超过最大公会可接个数
        uint32 maxGuildTaskNum = GuildTaskConfig::Instance()->GetMaxTaskNum();
        if(m_task_game_mgr.GetGuildTaskTakeCount() >= maxGuildTaskNum)
        {
            m_player->NotifyErrorCode(ERR_TASK_REACH_MAX_COUNT);
            return false;
        }

        // 检查任务列表中已接公会任务数
        if(m_task_game_mgr.GetGuildHaveTaskTotal() + m_task_game_mgr.GetGuildCompleteTaskTotal() >= GuildTaskConfig::Instance()->GetCanTaskNum())
        {
            m_player->NotifyErrorCode(ERR_TASK_CAN_TAKE_ONE);
            return false;
        }
    }

    return true;
}

// ==================================================
// 添加公会积分
// ==================================================
void TaskManager::AddGuildScore(int32 score)
{
    m_task_game_mgr.IncGuildScore(score);
}

// 微端登陆通知
void TaskManager::OnTaskMicroClientReq( void* data )
{
    ERROR_CODE* recvMsg = (ERROR_CODE*)data;
    if (!recvMsg)
        return;

    if(recvMsg->errcode != 1)
        return;

    // 微端登陆任务
    m_task_manage_agent->OnTaskMicroClient();

    // 活跃度
    m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_MICRO_CLIENT, 1, 0, 0);

    //设置微端领取过
    m_player->SetWelcomeRewardFlag(FLAG_MICRO_CLIENT, true);

    DO_CHANGE_ACK sendMsg;
    sendMsg.flag = m_player->GetWelcomeReward();
    m_player->SendToGate(MSG_DO_CHANGE_ACK, &sendMsg);
}

void TaskManager::ResetTask( std::vector<uint32>& taskID_list )
{
    RoleTaskList resetList;
    for(std::vector<uint32>::iterator idIter = taskID_list.begin(); idIter != taskID_list.end(); idIter++)
    {
        RoleTask* pRewardTask = GetTaskById(*idIter);
        if(!pRewardTask)
        {
            return;
        }

        // 在子管理器中删除
        if(pRewardTask->GetType() == TASK_TYPE_FESTIVAL)
        {
            pRewardTask->ResetTask();
            m_task_festive_mgr.ResetTask(pRewardTask);
            resetList.push_back(pRewardTask);
        }
    }
   
    NotifyTaskUpdate(resetList);
}

bool TaskManager::ResetTaskFestival( uint32 centerID, uint32 festivalID )
{
    // 重置任务
    const TASK_FESTIVAL_TEMPLATE* festivalTmpl = TaskFestiveConfig::Instance()->GetFestivalTmpl(centerID, festivalID);
    if(!festivalTmpl)
    {
        return false;
    }

    std::vector<uint32> idList;
    for (std::vector<TASK_TEMPLATE>::const_iterator targetIt = festivalTmpl->tasks.begin(); targetIt != festivalTmpl->tasks.end(); ++targetIt)
    {
        idList.push_back(targetIt->task_id);
        
    }

    ResetTask(idList);
    return true;
}
