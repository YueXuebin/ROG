#include "gameholder_pch.h"
#include "task_manage_agent.h"
#include "task_define.h"
#include "player.h"
#include "task_manager.h"
#include "task_target.h"
#include "task_data.h"
#include "event_define.h"
#include "ride_manager.h"
#include "scene.h"



TaskManageAgent::TaskManageAgent(TaskManager* pTaskManager) : 
m_TaskManager(pTaskManager)
{

}

TaskManageAgent::~TaskManageAgent()
{

}

void TaskManageAgent::Clear()
{

}

// ==================================================
// 接收任务事件
// ==================================================
void TaskManageAgent::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    uint32 funcID = 0;
    uint32 taskType = 0;

    ///////////////////////////////////////////////////////////////////////
    // 普通事件处理
    switch(eventId)
    {
    case EVENT_CHANGE_VIP:
        m_TaskManager->NotifyGameTaskNum();
        return;
    }

    int32 Arg1 = arg1;
    ///////////////////////////////////////////////////////////////////////
    // 任务事件处理
    if(TASK_EVENT_BEING < eventId && eventId < TASK_EVENT_END)
    {
        taskType = eventId % TASK_EVENT_BEING;
    }
    else if(eventId == OFF_EVENT_RECHARGE_ONE)
    {
        taskType = eventId % OFF_EVENT_RECHARGE_BEGIN % TASK_EVENT_BEING;
        Arg1 = arg3;
    }
    else
        return;

    switch(eventId)
    {
    case TASK_EVENT_KILL_MONSTER:
        {
            uint32 num = arg2 >> 24;                // 前 8  位为击杀数量
            uint32 mapID = arg2 & 0xFFFFFF;         // 后 24 位为副本ID
            OnKillMonster(Arg1, num, mapID);
        }
        break;
    case TASK_EVENT_RADIS_KILL_MONSTER:             // 扫荡杀怪
        {
            uint32 num = arg2 >> 24;                // 前 8  位为击杀数量
            uint32 mapID = arg2 & 0xFFFFFF;         // 后 24 位为副本ID
            OnKillMonster(Arg1, num, mapID, true);
        }
        break;
        //case TASK_EVENT_ACTIVE_TRIGGER:                 // 激活Trigger
        //    {
        //        OnActiveTrigger(arg1);
        //    }
        //    break;
        //case TASK_EVENT_RADIS_DUNGEON:                  // 扫荡副本
        //    {
        //        OnRadisDungeon(arg1);
        //    }
        //    break;
        //case TASK_EVENT_ASHURA_TIMES:                   // 阿修罗通关次数
        //    {
        //        OnAshuraTimes();
        //    }
        //    break;
        //case TASK_EVENT_ASHURA_KILL:                    // 阿修罗杀人次数
        //    {
        //        OnAshuraKill(arg1);
        //    }
        //    break;
    case TASK_EVENT_TALK_NPC:                       // 跟NPC对话
    case TASK_EVENT_CHANGE_DUNGEON:                 // 切换副本
    case TASK_EVENT_UP_EQUIP_STAR:                  // 提升装备星
    case TASK_EVENT_UP_EQUIP_LEVEL:                 // 提升装备等级
    case TASK_EVENT_UP_EQUIP_QUALITY:               // 提升装备品质
    case TASK_EVENT_INLAID_EQUIP_JEWEL:             // 镶嵌装备宝石
    case TASK_EVENT_SYNTHETIZE_JEWEL:               // 合成宝石
    case TASK_EVENT_ACTIVE_PET:                     // 宠物激发
    case TASK_EVENT_SET_RIDE_PET:                   // 设置骑宠
    case TASK_EVENT_ADD_FRIEND:                     // 添加好友
    case TASK_EVENT_UP_HOROSCOPE:                   // 开启星点
    case TASK_EVENT_JOIN_ACTIVITY:                  // 参加活动
    case TASK_EVENT_UP_WING:                        // 翅膀升阶
    case TASK_EVENT_NPC_BUY:                        // NPC购买物品
    case TASK_EVENT_PLAYER_DOOR:                    // 勇者争霸
    case TASK_EVNET_CMPL_ONE_ROUND_DAILY:           // 日常一轮
    case TASK_EVENT_CRUSADE_TASK:                   // 讨伐令
    case TASK_EVENT_GOLD_TREE:                      // 砸金像
    case TASK_EVENT_TREASURY:                       // 进入宝库
    case TASK_EVENT_ONLINE_PK:                      // 切磋 改为组队一次
    case TASK_EVENT_GUARD:                          // 护卫
    case TASK_EVNET_SUMMON_RIDE:                    // 召唤坐骑
    case TASK_EVENT_EQUIP_LEVEL_STAT_NUM:           // 装备升星次数
    case TASK_EVENT_USE_HOROSCOPE:                  // 使用星点
    case TASK_EVENT_USE_WING:                       // 翅膀提升
    case TASK_EVENT_UP_RUNE_LEVEL:                  // 符文升级
    case TASK_EVENT_JOIN_GUILD:                     // 加入公会
    case TASK_EVENT_DONATE_GUILD_POINT:             // 捐赠公会点
    case TASK_EVENT_DAILY_CONSUME:                  // 每日消费
    case TASK_EVENT_RECHARGE_TOTAL:                 // 充值累计
    case TASK_EVENT_RECHARGE_SINGLE:                // 每日充值单笔最大值
    case TASK_EVENT_RECHARGE_DAILY:                 // 充值每日0
    case TASK_EVENT_UP_PLAYER_LEVEL:                // 角色等级
    case TASK_EVENT_UP_SKILL_RUNE:                  // 符文升级
    case TASK_EVENT_RIDE_NUM_BY_QUALITY:            // 获得坐骑品质数量
    case TASK_EVENT_RECHARGE_OPENSVR:               // 充值累计（开服）
    case TASK_EVENT_CONSUME:                        // 消费
    case TASK_EVENT_INLAID_EQUIP_JEWEL_LEVEL:       // 镶嵌XX级宝石
    case TASK_EVENT_INLAID_JEWEL_LEVEL:             // 镶嵌装备宝石
    case TASK_EVENT_RIDE_LEVEL:                     // 提升坐骑指定等级
    case TASK_EVENT_PASSIVE_SKILL_LEVEL:            // 提升被动技能指定等级
    case TASK_EVENT_ACTIVE_PROTECT_SOUL:            // 宠物激活护魂
    case TASK_EVENT_UP_PET_LEVEL:                   // 提升宠物等级
    case TASK_EVENT_EQUIP_JEWEL_NUM:                // 镶嵌特定宝石数
    case TASK_EVENT_PLAYER_DOOR_REWARD:             // 领取勇者争霸奖励xx次数
    case TASK_EVENT_ONLINE_GROW_REWARD:             // 领取多少等级成长礼包
    case TASK_EVENT_SYNTHETIZE_JEWEL_NUM_LEVEL:     // 合成xx颗xx级宝石
    case TASK_EVENT_PROTECT_SOUL_LEVEUP:            // XX的护魂升级到xx级
    case TASK_EVENT_GUID_POINT:                     // 获得xx点公会贡献点
    case TASK_EVENT_FASHION:                        // 激活xx个xx化身
    case TASK_EVENT_FASHION_BUY_BOX:                // 化身系统购买xx宝箱xx次
    case TASK_EVENT_ZOMBIE_REWARD:                  // 领取多xx次僵尸生存礼包
    case TASK_EVENT_GUILD_TASK:                     // 完成多少次公会任务
    case TASK_EVENT_PET_ONCE_ACTIVE:                // 多少次宠物一件激发
    case TASK_EVENT_LEVE_UP_SWORD:                  // 圣剑升星多少次
    case TASK_EVENT_LEVE_QUALITY_EQUIP_TIMES:       // 装备升品（次数）
    case TASK_EVENT_LEVE_UP_EQUIP_TIMES:            // 装备升级（次数）
    case TASK_EVENT_ONLIE_TIME:                     // 累计在线时长
    case TASK_EVENT_LOGIN_DAY:                      // 每日登陆
    case TASK_EVENT_FASHION_BUY_TOTAL_NUM:          // 所有使用宝藏卡和魔石的抽取行为抽XX次
    case TASK_EVENT_FASHION_BUY_PLATINUM_NUM:       // 铂金抽取XX次
    case TASK_EVENT_FASHION_BUY_EXTREME_NUM:
    case TASK_EVENT_VIP_STATISTICS_REFRESH:
    case TASK_EVENT_VIP_CHANGE:
    case TASK_EVENT_VOID_FIELD_BOSS_PAR:
    case TASK_EVENT_VOID_FIELD_MOONLESS_PAR:
    case TASK_EVENT_VOID_FIELD_BOSS_KILL:
    case TASK_EVENT_TOP_UP_RANK_TOPN:
    case TASK_EVENT_TOP_UP_RANK_ENTER:
    case TASK_EVENT_TOP_UP_RANK_WING:
    case TASK_EVENT_TOP_UP_RANK_PET:
    case TASK_EVENT_TOP_UP_RANK_RIDE:
    case TASK_EVENT_TOP_UP_RANK_BATTLE:
    case TASK_EVENT_TOP_UP_RANK_LEVEL:
    case TASK_EVENT_LOGIN_DAY_RESET:
    case TASK_EVENT_EGG_SMASH_NUM:
    case TASK_EVENT_TOTAL_CONSUMPTION:
    case TASK_EVENT_CUMULATIVE_RECHARGE:
    case TASK_EVENT_NEW_UP_WING:
    case TASK_EVENT_NEW_LEVE_UP_SWORD:
    case TASK_EVENT_TOP_UP_RANK_FASHION:
    case TASK_EVENT_TOP_UP_RANK_OFFLINE_PLAYER:

    case TASK_EVENT_BATTLE_POWER_FASHION:
    case TASK_EVENT_BATTLE_POWER_PET:
    case TASK_EVENT_BATTLE_POWER_WING:
    case TASK_EVENT_BATTLE_POWER_MOUNT:
    case TASK_EVENT_BATTLE_POWER_TOTAL:
    case TASK_EVENT_FASHION_LEVEL:
    case TASK_EVENT_FASHION_NUM_STAR_LEVEL:
        {
            OnTriggerTarget(taskType, Arg1, arg2);
        }
        break;
    case TASK_EVENT_ASHURA_KILL:
        {
            // 暂时不需要的任务事件
        }
        break;
    case OFF_EVENT_RECHARGE_ONE:                    // 单笔充值
        {
            OnTriggerTarget(taskType, Arg1, arg2);
            OnTriggerTarget(TASK_TARGET_111, Arg1, arg2);
        }
        break;
    default:
        CnAssert(false);
        CnError("Unknow taskType:%u by event:%d", taskType, eventId);
        break;
    }
}


// ==================================================
// 杀死怪物
// ==================================================
void TaskManageAgent::OnKillMonster(uint32 monsterID, int32 num, uint32 mapID, bool isRadis)
{
    bool calflag = false;
    bool effect = false;
    int32 rand_num = 0;
    RoleTaskList list;

    RoleTaskList& ptasks = m_TaskManager->m_pTasks;

    for(RoleTaskList::iterator taskIt = ptasks.begin(); taskIt != ptasks.end(); ++taskIt)
    {
        RoleTask* pTask = *taskIt;

        effect = pTask->OnKillMonster(monsterID, num, mapID, isRadis);

        if(effect)
        {
            list.push_back(*taskIt);
        }
    }

    m_TaskManager->NotifyTaskUpdate(list);
}

// ==================================================
// 结算副本
// ==================================================
void TaskManageAgent::OnSettleDungeon(uint32 dungeonID, bool success, bool raids)
{
    if (success == false)
        return;

    OnTriggerTarget(TASK_TARGET_4, dungeonID, raids ? 1 : 0);
}

// 微端登陆触发任务
void TaskManageAgent::OnTaskMicroClient()
{
    OnTriggerTarget(TASK_TARGET_99, 0, 1);        // 微端登陆任务
}
// 获得坐骑触发任务
void TaskManageAgent::CheckRideQualityState()
{
    RideManager * ride_mgr = m_TaskManager->m_player->GetRideManager();
    if(ride_mgr)
    {
        int32 greenum = ride_mgr->GetRideQualityNum(QUALITY_GREEN);
        int32 bluenum = ride_mgr->GetRideQualityNum(QUALITY_BLUE);
        int32 purlenum = ride_mgr->GetRideQualityNum(QUALITY_PURPLE);
        int32 goldnum = ride_mgr->GetRideQualityNum(QUALITY_GOLD);

        OnTriggerTarget(TASK_TARGET_78, QUALITY_GREEN, greenum);
        OnTriggerTarget(TASK_TARGET_78, QUALITY_BLUE, bluenum);
        OnTriggerTarget(TASK_TARGET_78, QUALITY_PURPLE, purlenum);
        OnTriggerTarget(TASK_TARGET_78, QUALITY_GOLD, goldnum);
    }
}
// 兑换物品触发任务
void TaskManageAgent::OnExchangeItem(uint32 itemID)
{
    bool change = false;

    RoleTaskList list;

    RoleTaskList& ptasks = m_TaskManager->m_pTasks;
    for(RoleTaskList::iterator taskIt = ptasks.begin(); taskIt != ptasks.end(); ++taskIt)
    {
        RoleTask* pTask = *taskIt;
        bool change = pTask->OnExchangeItem(itemID);

        if(change)
        {
            list.push_back(pTask);
        }
    }

    m_TaskManager->NotifyTaskUpdate(list);
}

// 触发任务目标
void TaskManageAgent::OnTriggerTarget(uint32 targetType, uint32 arg1, uint32 arg2)
{
    RoleTaskList list;

    Player* m_player = m_TaskManager->m_player;

    RoleTaskList& taskList = m_TaskManager->m_pTasks;

    for(RoleTaskList::iterator taskIt = taskList.begin(); taskIt != taskList.end(); ++taskIt)
    {
        RoleTask* pTask = *taskIt;

        // 检查任务是否需要触发
        if(!pTask->NeedTriggerTaskTarget())
            continue;

        // 检查任务是否在有效时间内
        if(!pTask->IsInValidTime())
            continue;

        bool effect = pTask->OnTriggerTaskTarget(targetType, arg1, arg2);
        if(effect)
        {
            list.push_back(*taskIt);
        }
    }

    m_TaskManager->NotifyTaskUpdate(list);
}

// ==================================================
// 发送任务事件
// ==================================================
void TaskManageAgent::SendTaskEvent(uint32 event_id, int32 arg1, int32 arg2, int32 arg3)
{
    int32 task_target = 0;
    uint32 func_id = 0;

    task_target = event_id % TASK_EVENT_BEING;

    switch(event_id)
    {
    case TASK_EVENT_KILL_MONSTER:
        {
            arg2 <<= 24;                // 前 8  位为击杀数量
            arg2 |= DUNGEON_ID(arg3);   // 后 24 位为副本ID
        }
        break;
    case TASK_EVENT_RADIS_KILL_MONSTER:
        {
            arg2 <<= 24;                // 前 8  位为击杀数量
            arg2 |= DUNGEON_ID(arg3);   // 后 24 位为副本ID
        }
        break;
    default:
        break;
    }

    Player* player = m_TaskManager->m_player;

    if(!player->GetScene())
        return;

    player->GetScene()->SendEvent(player->GetId(), event_id, arg1, arg2, 0);  // 发送任务事件
}


