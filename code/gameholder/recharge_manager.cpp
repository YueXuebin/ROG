#include "gameholder_pch.h"
#include "recharge_manager.h"
#include "task_manager.h"
#include "player.h"
#include "task_festive_config.h"
#include "task_data.h"


// ==================================================
// 构造函数
// ==================================================
RechargeManager::RechargeManager(Player* player):
m_player(player)
{
   b_exe = false; 
}

// ==================================================
// 析构函数
// ==================================================
RechargeManager::~RechargeManager()
{
    
}

// ==================================================
// 加载充值信息
// ==================================================
void RechargeManager::LoadRechargeInfo(const PLAYER_RECHARGE_INFO& info)
{
    m_recharge_info = info;
    b_exe = true;
}

// ==================================================
// 获得充值总数
// ==================================================
int32 RechargeManager::GetTotalRecharge(RoleTask* pTask)
{
    int32 money_point = 0;

    if(pTask && pTask->GetFestivalID())
    {
        // 如果未找到节日模板，则返回NULL
        const TASK_FESTIVAL_TEMPLATE* festivalTmpl = TaskFestiveConfig::Instance()->GetFestivalTmpl(m_player->GetCenterSvrID(), pTask->GetFestivalID());
        if(!festivalTmpl)
        {
            return money_point;
        }

        bool b_exe = false;
        if((!festivalTmpl->begin_time) && (!festivalTmpl->end_time))
        {
            return m_recharge_info.recharge_total.money_point;
        }
        
        Crown::CSDDateTime startDay;
        startDay.SetTimeValue(festivalTmpl->begin_time);
        startDay.SetTime(0, 0, 0);
        uint32 startDayTimer = (uint32)startDay.GetTimeValue();

        Crown::CSDDateTime stopDay;
        stopDay.SetTimeValue(festivalTmpl->end_time);
        stopDay.IncDay();
        stopDay.SetTime(0, 0, 0);
        uint32 stopDayTimer = (uint32)stopDay.GetTimeValue();

        for(std::vector<PLAYER_RECHARGE_RECORDER>::iterator it = m_recharge_info.recharge_daily_list.begin();
            it != m_recharge_info.recharge_daily_list.end(); ++it)
        {
            if(it->update_time >= startDayTimer && it->update_time <= stopDayTimer)
            {
                money_point += it->money_point;
            }
        }

        return money_point;
    }
    else
    {
        // 如果不是节日活动则返回从开服到现在的总值
        return m_recharge_info.recharge_total.money_point;
    }
}
// ==================================================
// 获得当日充值数
// ==================================================
int32 RechargeManager::GetTodayRecharge(bool isSingle)
{
    Crown::CSDDateTime dateTime = Crown::SDNow();
    uint32 todayTime = 0;

    dateTime.SetTime(0, 0, 0);
    todayTime = (uint32)dateTime.GetTimeValue();

    for(std::vector<PLAYER_RECHARGE_RECORDER>::iterator it = m_recharge_info.recharge_daily_list.begin();
        it != m_recharge_info.recharge_daily_list.end(); ++it)
    {
        if(todayTime != it->update_time) continue;

        if(isSingle)
        {
            return it->max_single;
        }
        else
        {
            return it->money_point;
        }
    }

    return 0;
}

void RechargeManager::SetRechargeCount(int32 money)
{
    m_recharge_info.recharge_total.money_point = money;
}

int32 RechargeManager::GetRechargeCount()
{
    return m_recharge_info.recharge_total.money_point;
}

uint32 RechargeManager::GetTotalRechargeTime()
{
    return m_recharge_info.recharge_total.update_time;
}
