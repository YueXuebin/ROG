#include "gameholder_pch.h"
#include "event_define.h"
#include "task_data.h"
#include "player.h"
#include "task_manager.h"
#include "item_manager.h"
#include "task_festive_config.h"
#include "task_game_config.h"
#include "dungeon_data.h"
#include "dungeon_manager.h"


CnImplementRootRTTI(RoleTask);

// ==================================================
// 角色的任务数据结构
// ==================================================
RoleTask::RoleTask(const TASK_TEMPLATE* task_template, Player* player) :
m_player(player)
{
    Clear();

    m_TaskManager = player->GetTaskManager();
    m_IsDispose = false;
    m_IsInitTarget = false;

    m_template = task_template;
    m_taskID = task_template->task_id;

    // 通过任务模板，构建任务目标对象
    RoleTaskTarget target(this);
    for(std::vector<TASK_TARGET>::const_iterator targetIt = m_template->targets.begin(); targetIt != m_template->targets.end(); ++targetIt)
    {
        target.InitByTargetTmpl(*targetIt);
        m_targets.push_back(target);
    }
    CnAssert(m_targets.size() > 0 || task_template->task_id == 0);      // 除非是空模板构建的任务对象，否则不应该没有目标
}

RoleTask::~RoleTask()
{
    m_IsDispose = true;
}

void RoleTask::Clear()
{
    m_template              = 0;
    m_state                 = 0;
    m_quality               = 0;
    m_rate_of_complete      = BASE_RATE_NUM;
    m_loop_num              = 0;
    m_targets.clear();
}

bool RoleTask::HasTargetByType(uint32 targetType)
{
    for(RoleTaskTargetList::iterator targetIt=m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        if(targetIt->GetTargetType() == targetType)
            return true;
    }
    return false;
}

bool RoleTask::HasTargetByID(uint32 targetID)
{
    for(RoleTaskTargetList::iterator targetIt=m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        if(targetIt->GetID() == targetID)
            return true;
    }
    return false;
}

void RoleTask::RemoveTarget(uint32 targetID)
{
    for(RoleTaskTargetList::iterator targetIt=m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        if(targetIt->GetID() == targetID)
        {
            m_targets.erase(targetIt);
            break;
        }
    }
}

bool RoleTask::IsEmptyTemplate()
{
    //return m_template == TaskGameConfig::Instance()->GetEmptyTaskTmpl();
    return false;
}

void RoleTask::FillTaskRecord(TASK_RECORD& taskRecord) const
{
    TASK_TARGET_RECORD target;
    taskRecord.Clear();
    taskRecord.task_id  = GetTaskID();
    taskRecord.state    = m_state;
    taskRecord.quality  = m_quality;
    taskRecord.position = m_position;

    if (taskRecord.task_id >= TASK_FESTIVE_BASE_ID)     // 精彩活动GMT刷新后m_template失效
        taskRecord.type = TASK_TYPE_FESTIVAL;
    else
        taskRecord.type     =   m_template->task_type;
    taskRecord.sub_type = m_template->sub_type;
    taskRecord.rate_of_complete = m_rate_of_complete;
    taskRecord.loop_num = m_loop_num;
    for(RoleTaskTargetList::const_iterator targetIt=m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        (*targetIt).FillTaskTargetRecord(target);
        taskRecord.list.push_back(target);
    }
}

void RoleTask::LoadTaskRecord(const TASK_RECORD& taskRecord)
{
    // 构建任务数据
    CnAssert(GetState() == 0);

    CnAssert(m_taskID == 0 || m_taskID == taskRecord.task_id);
    m_taskID    = taskRecord.task_id;
    m_state     = taskRecord.state;
    m_quality   = taskRecord.quality;

    SetPosition(taskRecord.position);
    SetLoopNum(taskRecord.loop_num);

    // 重新构建,任务目标对象<->任务目标模板
    RoleTaskTarget target(this);
    for(std::vector<TASK_TARGET_RECORD>::const_iterator targetIt=taskRecord.list.begin(); targetIt != taskRecord.list.end(); ++targetIt)
    {
        if(HasTargetByID(targetIt->id))
        {
            RemoveTarget(targetIt->id);
        }

        target.LoadTaskTargetRecord(*targetIt, m_template);
        m_targets.push_back(target);
    }
    CnAssert(m_targets.size() > 0 || m_template->task_id == 0);
}

bool RoleTask::CanLoop()
{
    if(m_state < TASK_STATE_REWARD)        // 领完奖的任务才可以循环
    {
        CnAssert(false);
        return false;           // 未完成的任务，不可以循环
    }

    return m_loop_num < m_template->loop_num;
}

void RoleTask::DoLoop()
{
    if(m_targets.size() == 1 && HasTargetByType(TASK_TARGET_103))
    {
        CnAssert(GetTaskTargetNum(0) == 0);
    }
    else
    {
        for(RoleTaskTargetList::iterator targetIt = m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
        {
            targetIt->Refrash();
        }
    }
    
    m_state = TASK_STATE_HAVETAKE;          // 使任务又回到已接状态

    InitTaskLinkage();    // 更新任务状态
}

void RoleTask::Cancel()
{
    // 只有已接任务可以放弃
    if(m_state != TASK_STATE_HAVETAKE){
        CnAssert(false);        //只有已接任务才可以放弃
        return;
    }
    // 重置任务目标对象
    for(RoleTaskTargetList::iterator targetIt = m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {

        targetIt->Refrash();
    }

    m_state = TASK_STATE_CANTAKE;   // 使任务又回到可接状态

    InitTaskLinkage();              // 更新任务状态
}

void RoleTask::SetState( uint8 state )
{
    if(IsEmptyTemplate())
    {
        CnAssert(false);        // 空模板任务，不应该会有逻辑
        return;
    }

    uint8 old_state = m_state;
    if(old_state > state)
    {   
        CnAssert(false);    // 任务状态的反流程设置，是不应该发生的
    }

    if(old_state == TASK_STATE_COMPLETE && state == TASK_STATE_REWARD)
    {
        m_loop_num++;
    }

    m_state = state;

    if(m_state == TASK_STATE_COMPLETE)
    {
        // 任务完成
        CheckTaskAutoReward();
    }

}

void RoleTask::MakeTaskComplete()
{
    if(IsEmptyTemplate())
        return;

    for (RoleTaskTargetList::iterator targetIt = m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        targetIt->Complete();
    }
}

bool RoleTask::CheckTaskComplete()
{
    if(IsEmptyTemplate())
        return false;

    if(m_state != TASK_STATE_HAVETAKE && m_state != TASK_STATE_COMPLETE)
        return false;

    if(m_targets.size() == 0)
        return false;

    bool is_complete = true;
    bool can_revert = false;
    for (RoleTaskTargetList::iterator targetIt = m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        if(targetIt->GetTargetType() == TASK_TARGET_89)     // 物品兑换目标，可以使任务回退
            can_revert = true;

        if(!targetIt->IsEnough())
        {
            is_complete = false;
            break;
        }
    }

    if(m_state == TASK_STATE_HAVETAKE && is_complete)
    {
        m_state = TASK_STATE_COMPLETE;      // 已接->完成
        CheckTaskAutoReward();
        return true;
    }
    else if(can_revert)
    {
        if(m_state == TASK_STATE_COMPLETE && !is_complete)
        {
            m_state = TASK_STATE_HAVETAKE;      // 已经完成的任务->回退到已接
            return true;
        }
    }
    
    return false;
}

bool RoleTask::CheckTaskPreCondition(uint32* err)
{
    return m_TaskManager->CheckTaskPreCondition(m_template, err);
}

void RoleTask::OnAdd()
{
    m_state = TASK_STATE_CANTAKE;       // 默认新添加的任务，状态为可接
}

bool RoleTask::OnExchangeItem( uint32 itemID )
{
    if((m_state != TASK_STATE_HAVETAKE) && (m_state != TASK_STATE_COMPLETE))
        return false;

    // 检查任务是否在有效时间内
    if(!IsInValidTime())
    {
        return false;
    }

    bool targetChange = false;
    for (RoleTaskTargetList::iterator targetIt = m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        if(targetIt->OnExchangeItem(itemID))
            targetChange = true;
    }

    bool taskChange = CheckTaskComplete();
    return targetChange || taskChange;
}

bool RoleTask::OnKillMonster(uint32 monsterID, int32 num, uint32 mapID, bool isRadis)
{
    if(GetState() != TASK_STATE_HAVETAKE) 
        return false;

    // 检查任务类型是否支持扫荡
    if(isRadis && !IsRaid())
    {
        return false;
    }

    bool targetChange = false;
    for (RoleTaskTargetList::iterator targetIt = m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        if(targetIt->OnKillMonster(monsterID, num, mapID))
            targetChange = true;
    }

    bool taskChange = CheckTaskComplete();
    return targetChange || taskChange;
}

bool RoleTask::RefreshLoopTaskState()
{
    // 检查任务是否在有效时间内
    if(!IsInValidTime())
    {
        return false;
    }

    bool b_exe = false;
    for (RoleTaskTargetList::iterator targetIt = m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        if(!targetIt->GetTargetTmpl())
            continue;

        uint32 targetType = targetIt->GetTargetType();
        // 判断重置条件
        if(targetType == TASK_TARGET_123)
        {
            targetIt->UpdateTargetState();
            continue;
        }

        // 要24小时重置的
        if((targetType != TASK_TARGET_71) && (targetType != TASK_TARGET_75) && (targetType != TASK_TARGET_103) &&
            (targetType != TASK_TARGET_106) && (targetType != TASK_TARGET_107) && (targetType != TASK_TARGET_108) &&
            (targetType != TASK_TARGET_117))
        {
            continue;
        }
        targetIt->Refrash();
        SetTaskHaveTakeState();

        SetLoopNum(0);

        b_exe = true;
    }

    return b_exe;
}

void RoleTask::SetTaskHaveTakeState()
{
    for (RoleTaskTargetList::iterator targetIt = m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        if(!targetIt->IsEnough())
        {
            m_state = TASK_STATE_HAVETAKE;
            break;
        }
    }
}

void RoleTask::OpenDungeonByTask()
{
    // 根据任务目标开启副本
    for(std::vector<TASK_TARGET>::const_iterator targetIt = m_template->targets.begin(); targetIt != m_template->targets.end(); ++targetIt)
    {
        const TASK_TARGET& target = *targetIt;

        uint32 dungeonID = 0;
        DungeonTemplate* dungeonTmpl = 0;

        switch(target.type)
        {
        case TASK_TARGET_1:
        case TASK_TARGET_3:
        case TASK_TARGET_4:
        case TASK_TARGET_17:
            {
                dungeonID = target.map_id;
            }
            break;
        default:
            break;
        }

        if(!dungeonID)
            continue;

        dungeonTmpl = DungeonManager::Instance()->GetDungeonTemplate(dungeonID);
        if(!dungeonTmpl)
            continue;

        // 判断副本类型是否为剧情副本
        uint32 dungeonClass = GetDungeonClass(dungeonTmpl->m_dungeon_id);
        if(dungeonClass == DUNGEON_CLASS_PLOT_ID)
        {
            //if(m_player->IsPlayerDungeonOpen(dungeonID)) 
            //    continue;            // 检查副本标记是否已打开

            //m_player->SetPlayerDungeonFlag(dungeonID, DUNGEON_OPEN_FLAG);   // 设置打开副本标记
            //m_player->SendOpenDungeonNtf(dungeonID);                        // 发送开打副本标记通知
        }
    }
}


bool RoleTask::NeedTriggerTaskTarget()
{
    if(IsEmptyTemplate())
        return false;         // 没有模板的任务不允许做任务触发

    bool isSpecial = false;

    for(RoleTaskTargetList::iterator targetIt = m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        if(targetIt->GetTargetType() == TASK_TARGET_89              // 89(物品兑换)
            || targetIt->GetTargetType() == TASK_TARGET_103)        // 103(单笔充值多少魔石，获得道具)目标，是即使状态为完成，也需要接受任务触发
        {
            isSpecial = true;
            break;
        }
    }

    if(isSpecial)
        return GetState() == TASK_STATE_HAVETAKE || GetState() == TASK_STATE_COMPLETE;
    else
        return GetState() == TASK_STATE_HAVETAKE;
}

uint32 RoleTask::GetDungeonIDByTrigger( uint32 triggerID )
{
    for(RoleTaskTargetList::iterator targetIt = m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        if(targetIt->GetTargetType() != TASK_TARGET_17)
            continue;
        if(targetIt->IsEnough()) 
            continue;
        if(targetIt->GetTargetNpcID() == triggerID)
        {
            return targetIt->GetTargetMapID();      // map_id = trigger触发的跳副本id
        }
    }
    return 0;
}

uint32 RoleTask::GetChangeDungeonID()
{
    uint32 dungeonID = 0;
    for (RoleTaskTargetList::iterator targetIt = m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        if(targetIt->IsEnough())
        {
            continue;
        }

        if((targetIt->GetTargetType() == TASK_TARGET_1) 
            || (targetIt->GetTargetType() == TASK_TARGET_3)
            || (targetIt->GetTargetType() == TASK_TARGET_4))
        {
            dungeonID = targetIt->GetTargetMapID();
        }
        else
        {
            continue;
        }

        break;
    }
    return dungeonID;
}

int32 RoleTask::GetTaskTargetNum( uint32 targetIndex )
{
    if(targetIndex >= m_targets.size())
        return 0;

    int32 index = 0;

    for(RoleTaskTargetList::iterator targetIt = m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        if(index == targetIndex)
        {
            return targetIt->GetNum();
        }
        index++;
    }
    return 0;
}

bool RoleTask::CheckTaskAutoReward()
{
    // 自动发奖励
    /**
    if(GetFestivalID())
    {
        if(m_template->icon == "1")
        {
           m_TaskManager->GmTaskReward(m_template->task_id);
        }
        return true;
    }*/
    return false;
}

void RoleTask::ResetTask()
{
    for(RoleTaskTargetList::iterator targetIt = m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        DoLoop();
    }
}

const REWARD_TABLE& RoleTask::GetReward()
{
    if(GetType() == TASK_TYPE_MAIN)
    {
        
        switch(m_player->GetCareer())
        {
            case CAREER_WARRIOR:
                return m_template->warrior_reward;
                break;
            case CAREER_MAGE:
                return m_template->mage_reward;
                break;
            case CAREER_ARCHER:
                return m_template->archer_reward;
                break;
        }
        return m_template->rewards;
    }
   return m_template->rewards;
}