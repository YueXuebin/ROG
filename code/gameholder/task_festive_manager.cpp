#include "gameholder_pch.h"
#include "task_festive_manager.h"
#include "task_festive_config.h"
#include "task_manager.h"
#include "player.h"
#include "task_data_factory.h"
#include "task_data_festive.h"

// ==================================================
// 构造函数
// ==================================================
TaskFestiveManager::TaskFestiveManager(Player* player):
m_player(player)
{

}

// ==================================================
// 析构函数
// ==================================================
TaskFestiveManager::~TaskFestiveManager()
{
}

void TaskFestiveManager::Clear()
{
    // 删除所有任务对象
    m_tasks.clear();
    m_festivals.clear();
}

// ==================================================
// 加载任务信息
// ==================================================
void TaskFestiveManager::LoadTaskFestivalInfo(const TASK_FESTIVAL_INFO& info)
{
    for(std::vector<FESTIVAL_RECORD>::const_iterator festivalRecord = info.festival_list.begin(); festivalRecord != info.festival_list.end(); ++festivalRecord)
    {
        m_festivals.push_back(*festivalRecord);
    }

    std::vector<TASK_RECORD> list;
    for(std::vector<TASK_RECORD>::const_iterator recordIt = info.task_list.begin(); recordIt != info.task_list.end(); ++recordIt)
    {
        list.push_back(*recordIt);
    }

    m_task_mgr->LoadRoleTask(list);
}

// ==================================================
// 填充任务信息
// ==================================================
void TaskFestiveManager::FillTaskFestivalInfo(TASK_FESTIVAL_INFO& info) const
{
    info.Clear();

    for(FestivalRecordList::const_iterator festivalRecord = m_festivals.begin(); festivalRecord != m_festivals.end(); ++festivalRecord)
    {
        info.festival_list.push_back(*festivalRecord);
    }

    m_task_mgr->FillTaskRecordList(m_tasks, info.task_list);
}

// ==================================================
// 添加可以接受的活动任务
// ==================================================
void TaskFestiveManager::AddCanTakeTaskFestival(RoleTaskList* add_list)
{
    TaskTemplateList      tmplList;
    std::set<uint32>    taskIDSet;

    // 将已有的活动任务加入到集合中
    for(RoleTaskList::iterator taskIt = m_tasks.begin(); taskIt != m_tasks.end(); ++taskIt)
    {
        taskIDSet.insert((*taskIt)->GetTaskID());
    }

    // 获得所有可接受的活动
    TaskFestiveConfig::Instance()->GetCanTakeList(m_player->GetCenterSvrID(), taskIDSet, tmplList);

    for(TaskTemplateList::iterator tmplIt = tmplList.begin(); tmplIt != tmplList.end(); ++tmplIt)
    {
        m_task_mgr->AddTask(*tmplIt, add_list);
    }
}

// ==================================================
// 添加任务
// ==================================================
void TaskFestiveManager::AddTask(RoleTask* pTask)
{
    CnAssert( pTask->IsKindOf(&FestiveTask::ms_RTTI) );
    m_tasks.push_back(pTask);
}

// ==================================================
// 删除任务
// ==================================================
void TaskFestiveManager::DelTask(RoleTask* pTask)
{
    CnAssert( pTask->IsKindOf(&FestiveTask::ms_RTTI) );
    for(RoleTaskList::iterator taskIt = m_tasks.begin(); taskIt != m_tasks.end(); ++taskIt)
    {
        if((*taskIt) != pTask)
            continue;
        m_tasks.erase(taskIt);
        break;
    }
}

// ==================================================
// 请求节日奖励
// ==================================================
void TaskFestiveManager::OnFestivalRewardReq(void* data)
{
    TASK_FESTIVAL_REWARD_REQ*   recvMsg = (TASK_FESTIVAL_REWARD_REQ*)data;
    const TASK_FESTIVAL_TEMPLATE*     festiveTmpl = NULL;
    FESTIVAL_RECORD*            pRecord = 0;
    FESTIVAL_RECORD             festiveRecord;
    uint16 errcode = ERR_FAILED;

    if(!recvMsg) return;

    festiveRecord.festival_id = recvMsg->festival_id;

    // 获得节日模板
    festiveTmpl = TaskFestiveConfig::Instance()->GetFestivalTmpl(m_player->GetCenterSvrID(), recvMsg->festival_id);
    if(!festiveTmpl || (festiveTmpl->rewards.items.size() < 1))
    {
        goto _END;
    }

    // 获得节日奖励模板
    if(GetFestivalRecord(recvMsg->festival_id))
    {
        goto _END;
    }

    errcode = ERR_SUCCEED;

    festiveRecord.state = TASK_STATE_REWARD;

    m_player->TakeInRewardTable(&festiveTmpl->rewards);

    m_festivals.push_back(festiveRecord);

_END:
    SendFestivalRewardAck(errcode, festiveRecord);
}

// ==================================================
// 添加活动任务
// ==================================================
void TaskFestiveManager::AddFestivalTask(RoleTaskList* add_list)
{
    const TaskFestiveMap* dbFestivalMap = TaskFestiveConfig::Instance()->GetFestivalMap(m_player->GetCenterSvrID());
    if(!dbFestivalMap)
    {
        CnAssert(false);
        return;
    }
    // 遍历支线任务（这里的前提是map中的first是任务编号，而任务的顺序一定是由小到大的）
    for(TaskFestiveMap::const_iterator activityIt = dbFestivalMap->begin(); activityIt != dbFestivalMap->end(); ++activityIt)
    {
        for(std::vector<TASK_TEMPLATE>::const_iterator taskIt = activityIt->second.tasks.begin(); taskIt != activityIt->second.tasks.end(); ++taskIt)
        {
            if(GetFestivalTask(taskIt->task_id))
            {
                continue;
            }
            m_task_mgr->AddTask(&(*taskIt), add_list);
        }
    }
}

// ==================================================
// 获得活动任务
// ==================================================
RoleTask* TaskFestiveManager::GetFestivalTask(uint32 taskID)
{
    for(RoleTaskList::iterator taskIt = m_tasks.begin(); taskIt != m_tasks.end(); ++taskIt)
    {
        RoleTask* pTask = *taskIt;
        if(taskID == pTask->GetTaskID())
        {
            return pTask;
        }
    }
    return NULL;
}

// ==================================================
// 获得节日记录
// ==================================================
FESTIVAL_RECORD* TaskFestiveManager::GetFestivalRecord(uint32 festiveID)
{
    for(FestivalRecordList::iterator recordIt = m_festivals.begin(); recordIt != m_festivals.end(); ++recordIt)
    {
        if(recordIt->festival_id == festiveID)
        {
            return &(*recordIt);
        }
    }
    return NULL;
}

// ==================================================
// 发送节日奖励应答
// ==================================================
void TaskFestiveManager::SendFestivalRewardAck(uint16 errcode, const FESTIVAL_RECORD& festivalRecord)
{
    TASK_FESTIVAL_REWARD_ACK sendMsg;

    sendMsg.errcode = errcode;
    sendMsg.festival_record = festivalRecord;

    m_player->SendToGate(MSG_TASK_FESTIVAL_REWARD_ACK, &sendMsg);
}

void TaskFestiveManager::ResetTask( RoleTask* pTask )
{
    CnAssert( pTask->IsKindOf(&FestiveTask::ms_RTTI) );
    for(RoleTaskList::iterator taskIt = m_tasks.begin(); taskIt != m_tasks.end(); ++taskIt)
    {
        if((*taskIt) != pTask)
            continue;
        (*taskIt)->ResetTask();
        break;
    }
}


