#include "gameholder_pch.h"
#include "task_data_factory.h"
#include "task_data_festive.h"
#include "task_data_game.h"
#include "task_data_main.h"
#include "task_data_branch.h"
#include "task_data_daily.h"
#include "task_data_crusade.h"
#include "task_data_achive.h"
#include "task_data_guard.h"
#include "task_data_guild.h"



// 任务对象池
Crown::CSDObjectPool<MainTask>	    m_MainTaskPool;
Crown::CSDObjectPool<BranchTask>	m_BranchTaskPool;
Crown::CSDObjectPool<DailyTask>	    m_DailyTaskPool;
Crown::CSDObjectPool<CrusadeTask>	m_CrusadeTaskPool;
Crown::CSDObjectPool<GuardTask>	    m_GuardTaskPool;
Crown::CSDObjectPool<GuildTask>	    m_GuildTaskPool;
Crown::CSDObjectPool<FestiveTask>	m_FestiveTaskPool;
Crown::CSDObjectPool<AchiveTask>	m_AchiveTaskPool; // 成就任务


int32       m_TaskObjectNum = 0;            // 任务对象数, 用来检查内存泄漏

RoleTask* CreateTaskDataObject( const TASK_TEMPLATE* task_template, Player* player )
{
    m_TaskObjectNum++;

    RoleTask* pNewTask = NULL;

    switch(task_template->task_type)
    {
    case TASK_TYPE_FESTIVAL:
        pNewTask = m_FestiveTaskPool.Alloc(task_template, player);
        break;
    case TASK_TYPE_MAIN:
        pNewTask = m_MainTaskPool.Alloc(task_template, player);
        break;
    case TASK_TYPE_BRANCH:
        pNewTask = m_BranchTaskPool.Alloc(task_template, player);
        break;
    case TASK_TYPE_DAILY:
        pNewTask = m_DailyTaskPool.Alloc(task_template, player);
        break;
    case TASK_TYPE_CRUSADE:
        pNewTask = m_CrusadeTaskPool.Alloc(task_template, player);
        break;
    case TASK_TYPE_GUARD:
        pNewTask = m_GuardTaskPool.Alloc(task_template, player);
        break;
    case TASK_TYPE_GUILD:
        pNewTask = m_GuildTaskPool.Alloc(task_template, player);
        break;
    case TASK_TYPE_ACHIVE:
        pNewTask = m_AchiveTaskPool.Alloc(task_template, player);
        break;
    default:
        pNewTask = m_FestiveTaskPool.Alloc(task_template, player);      // 空任务按活动类任务处理
        break;
    }

    if(!pNewTask)
    {
        CnAssert(false);            // 不存在的任务类型
        return NULL;
    }

    return pNewTask;
}

void DestoryTaskDataObject( RoleTask* pTask )
{
    m_TaskObjectNum--;
    if(pTask->IsKindOf(&FestiveTask::ms_RTTI))
        m_FestiveTaskPool.Free(CnDynamicCast(FestiveTask, pTask));
    else if(pTask->IsKindOf(&MainTask::ms_RTTI))
        m_MainTaskPool.Free(CnDynamicCast(MainTask, pTask));
    else if(pTask->IsKindOf(&BranchTask::ms_RTTI))
        m_BranchTaskPool.Free(CnDynamicCast(BranchTask, pTask));
    else if(pTask->IsKindOf(&DailyTask::ms_RTTI))
        m_DailyTaskPool.Free(CnDynamicCast(DailyTask, pTask));
    else if(pTask->IsKindOf(&CrusadeTask::ms_RTTI))
        m_CrusadeTaskPool.Free(CnDynamicCast(CrusadeTask, pTask));
    else if(pTask->IsKindOf(&GuardTask::ms_RTTI))
        m_GuardTaskPool.Free(CnDynamicCast(GuardTask, pTask));
    else if(pTask->IsKindOf(&GuildTask::ms_RTTI))
        m_GuildTaskPool.Free(CnDynamicCast(GuildTask, pTask));
    else if(pTask->IsKindOf(&AchiveTask::ms_RTTI))
        m_AchiveTaskPool.Free(CnDynamicCast(AchiveTask, pTask));
    else
    {
        CnAssert(false);
        CnDelete pTask;
    }
}

uint32  GetTaskObjectNum()
{
    return m_TaskObjectNum;
}