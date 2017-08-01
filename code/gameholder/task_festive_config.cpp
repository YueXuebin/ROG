#include "gameholder_pch.h"
#include "task_festive_config.h"
#include "task_game_config.h"
#include "task_manager.h"
#include "player_manager.h"
#include "game_util.h"
#include "top_up_rank_manager.h"

IMPLEMENT_SINGLETON(TaskFestiveConfig)

// ================================================================================
// 任务配置
// ================================================================================
TaskFestiveConfig::TaskFestiveConfig()
{
    
}

TaskFestiveConfig::~TaskFestiveConfig()
{

}

// ==================================================
// 接收消息
// ==================================================
void TaskFestiveConfig::OnRecvFromCenter(uint32 centerID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_TASK_FESTIVAL_TMPL_LIST_ACK:
        OnLoadFestivalTmplList(centerID, data);
        break;
    default:
        break;
    }
}

// ==================================================
// 接收节日模板列表
// ==================================================
void TaskFestiveConfig::OnLoadFestivalTmplList(uint32 centerID, void* data)
{
    TASK_FESTIVAL_TEMPLATE_LIST* recvMsg = (TASK_FESTIVAL_TEMPLATE_LIST*)data;

    if(!recvMsg) 
        return;

    if(m_dbCenterFestivalMap.find(centerID) != m_dbCenterFestivalMap.end())
    {
        CnDbg("CenterServer id=%d ReLoad Festival\n", centerID);
    }
    else
    {
        TaskFestiveMap newFestivalMap;
        newFestivalMap.clear();
        m_dbCenterFestivalMap[centerID] = newFestivalMap;
    }

    TaskFestiveMap& dbFestivalMap = m_dbCenterFestivalMap[centerID];
    dbFestivalMap.clear();

    for(std::vector<TASK_FESTIVAL_TEMPLATE>::const_iterator festivalIt = recvMsg->list.begin(); festivalIt != recvMsg->list.end(); ++festivalIt)
    {
        AddDBFestivalInfo(dbFestivalMap, *festivalIt);
    }

    CnInfo("CenterServer id=%d Load Festival template num=%d\n", centerID, dbFestivalMap.size());

    // 建立充值排行查询
    TopUpRankManager::Instance()->Init(centerID);

    // 重新查询任务
    PlayerManager::Instance()->ReloadTask();
}

// ==================================================
// 添加活动信息
// ==================================================
void TaskFestiveConfig::AddDBFestivalInfo(TaskFestiveMap& festivalMap, const TASK_FESTIVAL_TEMPLATE& festivalTmpl)
{
    // 检查是否已有此活动
    if(festivalMap.find(festivalTmpl.festival_id) != festivalMap.end())
    {
        CnError("Duplicate festival ID:%u\n", festivalTmpl.festival_id);
        return;
    }

    // 添加活动数据
    festivalMap[festivalTmpl.festival_id] = festivalTmpl;
}

bool TaskFestiveConfig::HasCenterIDFestive(uint32 centerID)
{
    return m_dbCenterFestivalMap.find(centerID) != m_dbCenterFestivalMap.end();
}

// ==================================================
// 获得可接任务模板列表
// ==================================================
void TaskFestiveConfig::GetCanTakeList(uint32 centerID, const std::set<uint32>& idSet, TaskTemplateList& tmplList)
{
    if(!HasCenterIDFestive(centerID))
    {
        CnAssert(false);            // 没有该玩家的CenterServer上的活动
        return;
    }

    TaskFestiveMap& dbFestivalMap = m_dbCenterFestivalMap[centerID];

    for(TaskFestiveMap::iterator activityIt=dbFestivalMap.begin(); activityIt != dbFestivalMap.end(); ++activityIt)
    {
        TASK_FESTIVAL_TEMPLATE& festivalTemplate = activityIt->second;
        for(std::vector<TASK_TEMPLATE>::iterator taskIt = festivalTemplate.tasks.begin(); taskIt != festivalTemplate.tasks.end(); ++taskIt)
        {
            if(idSet.find(taskIt->task_id) != idSet.end())
                continue;

            tmplList.push_back(&(*taskIt));
        }
    }
}

// ==================================================
// 填充任务活动列表
// ==================================================
void TaskFestiveConfig::FillTaskFestivalList(uint32 centerID, TASK_FESTIVAL_TEMPLATE_LIST& festivalList)
{
    if(!HasCenterIDFestive(centerID))
    {
        CnAssert(false);            // 没有该玩家的CenterServer上的活动
        return;
    }

    TaskFestiveMap& dbFestivalMap = m_dbCenterFestivalMap[centerID];

    for(TaskFestiveMap::iterator taskIt=dbFestivalMap.begin(); taskIt != dbFestivalMap.end(); ++taskIt)
    {
        festivalList.list.push_back(taskIt->second);
    }
}

const TaskFestiveMap* TaskFestiveConfig::GetFestivalMap(uint32 centerID)
{
    if(!HasCenterIDFestive(centerID))
    {
        CnAssert(false);            // 没有该玩家的CenterServer上的活动
        return NULL;
    }

    TaskFestiveMap& dbFestivalMap = m_dbCenterFestivalMap[centerID];
    return &dbFestivalMap;
}
// ==================================================
// 得到活动模板
// ==================================================
const TASK_FESTIVAL_TEMPLATE* TaskFestiveConfig::GetFestivalTmpl(uint32 centerID, uint32 festivalID)
{
    if(!HasCenterIDFestive(centerID))
    {
        CnAssert(false);            // 没有该玩家的CenterServer上的活动
        return NULL;
    }

    TaskFestiveMap& dbFestivalMap = m_dbCenterFestivalMap[centerID];

    TaskFestiveMap::iterator activityIt = dbFestivalMap.find(festivalID);

    if(activityIt != dbFestivalMap.end())
    {
        return &(activityIt->second);
    }

    return NULL;
}
// ==================================================
// 获得任务模板
// ==================================================
const TASK_TEMPLATE* TaskFestiveConfig::GetTaskTmpl(uint32 centerID, uint32 taskID)
{
    if(!HasCenterIDFestive(centerID))
    {
        CnAssert(false);            // 没有该玩家的CenterServer上的活动
        return NULL;
    }

    TaskFestiveMap& dbFestivalMap = m_dbCenterFestivalMap[centerID];

    uint32 festivalID = 0;
    for(TaskFestiveMap::iterator festivalIt = dbFestivalMap.begin(); festivalIt != dbFestivalMap.end(); ++festivalIt)
    {
        festivalID = CalOriginFestivalID(taskID);
        if(festivalID != festivalIt->first)
            continue;

        for(std::vector<TASK_TEMPLATE>::iterator taskIt = festivalIt->second.tasks.begin();
            taskIt != festivalIt->second.tasks.end(); ++taskIt)
        {
            if(taskIt->task_id == taskID)
            {
                return &(*taskIt);
            }
        }
    }

    return NULL;
}

bool TaskFestiveConfig::GetFestivalLoop( uint32 centerID, uint32 festivalID )
{
    if(!HasCenterIDFestive(centerID))
    {
        CnAssert(false);            // 没有该玩家的CenterServer上的活动
        return false;
    }

    TaskFestiveMap& dbFestivalMap = m_dbCenterFestivalMap[centerID];

    TaskFestiveMap::iterator activityIt = dbFestivalMap.find(festivalID);

    if(activityIt != dbFestivalMap.end())
    {
        if(activityIt->second.loop_hours > 0)
            return true;
    }

    return false;
}

bool TaskFestiveConfig::IsInValidTime(uint32 centerID, uint32 festivalID)
{
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
    // 如果未找到节日模板，则返回 false
    const TASK_FESTIVAL_TEMPLATE* festivalTmpl = GetFestivalTmpl(centerID, festivalID);
    if(!festivalTmpl)
    {
        return false;
    }

    // 未限定有效时间，返回 true
    if(!festivalTmpl->begin_time || !festivalTmpl->end_time)
    {
        return true;
    }

    // 如果在活动时间内
    if((festivalTmpl->begin_time <= now) && (now < festivalTmpl->end_time))
    {
        return true;
    }

    return false;
}

bool TaskFestiveConfig::IsInActivityValidTime( uint32 centerID, uint32 festivalID, uint32 time )
{
    // 如果未找到节日模板，则返回 false
    const TASK_FESTIVAL_TEMPLATE* festivalTmpl = GetFestivalTmpl(centerID, festivalID);
    if(!festivalTmpl)
    {
        return false;
    }

    // 未限定有效时间，返回 true
    if(!festivalTmpl->begin_time || !festivalTmpl->end_time)
    {
        return true;
    }

    // 如果在活动时间内
    if((time >= festivalTmpl->begin_time) && (time < festivalTmpl->end_time))
    {
        return true;
    }

    return false;
}

uint32 TaskFestiveConfig::GetActivityBeginTime( uint32 centerID, uint32 festivalID )
{
    // 如果未找到节日模板，则返回 false
    const TASK_FESTIVAL_TEMPLATE* festivalTmpl = GetFestivalTmpl(centerID, festivalID);
    if(!festivalTmpl)
    {
        return 0;
    }

    // 未限定有效时间，返回 true
    if(!festivalTmpl->begin_time || !festivalTmpl->end_time)
    {
        return 0;
    }

    return festivalTmpl->begin_time;
}
