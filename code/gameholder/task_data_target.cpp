#include "gameholder_pch.h"
#include "task_data_target.h"
#include "item_manager.h"
#include "player.h"
#include "task_manager.h"


CnImplementRTTI(TargetTask, RoleTask);

TargetTask::TargetTask(const TASK_TEMPLATE* task_template, Player* player) : 
RoleTask(task_template, player)
{

}

TargetTask::~TargetTask()
{

}

void TargetTask::OnPreRewards()
{
    // 获得角色背包
    ItemManager* itemMgr = m_player->GetItemManager();

    // 删除交换类任务的物品
    for(std::vector<TASK_TARGET>::const_iterator it = m_template->targets.begin(); it != m_template->targets.end(); ++it)
    {
        if((it->type == TASK_TARGET_89))
        {
            //if(itemMgr)
            //    itemMgr->DelItemByID(it->value, it->num);
        }
    }
}

bool TargetTask::InitTaskLinkage()
{
    bool targetChange = false;
    for (RoleTaskTargetList::iterator targetIt = m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        // 只对状态类的目标做初始化
        if(!targetIt->IsLevelTriggerType())
            continue;

        if(targetIt->UpdateTargetState())            // 状态类目标，与触发参数无关
            targetChange = true;
    }

    bool taskChange = CheckTaskComplete();
    return targetChange || taskChange;
}

// 触发某任务的目标
bool TargetTask::OnTriggerTaskTarget(uint32 targetType, uint32 arg1, uint32 arg2)
{
    bool targetChange = false;
    for (RoleTaskTargetList::iterator targetIt = m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        if(targetIt->GetTargetType() != targetType)
            continue;

        switch(targetType)
        {
        case TASK_TARGET_4:
            bool b = (arg2 == 1 ? IsRaid() : true);         // 副本ID一致 且扫荡时任务目标也为扫荡
            arg2 = b?1:0;
            break;
        }

        if(targetIt->OnTriggerTaskTarget(arg1, arg2))          // 触发任务目标
            targetChange = true;
    }

    bool taskChange = CheckTaskComplete();
    return targetChange || taskChange;
}


