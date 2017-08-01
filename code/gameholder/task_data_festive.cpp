#include "gameholder_pch.h"
#include "task_data_festive.h"
#include "task_festive_config.h"
#include "player.h"
#include "event_define.h"
#include "game_util.h"
#include "config_manager.h"


CnImplementRTTI(FestiveTask, TargetTask);

FestiveTask::FestiveTask(const TASK_TEMPLATE* task_template, Player* player) :
TargetTask(task_template, player)
{

}

FestiveTask::~FestiveTask()
{

}

void FestiveTask::OnAdd()
{
    m_state = TASK_STATE_HAVETAKE;
}

bool FestiveTask::IsInValidTime()
{
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
    // 如果未找到节日模板，则返回 false
    const TASK_FESTIVAL_TEMPLATE* festivalTmpl = TaskFestiveConfig::Instance()->GetFestivalTmpl(m_player->GetCenterSvrID(), m_template->festival_id);
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

bool FestiveTask::IsNeedOnlineTime()
{
    const TASK_FESTIVAL_TEMPLATE* festivalTmpl= TaskFestiveConfig::Instance()->GetFestivalTmpl(m_player->GetCenterSvrID(), m_template->festival_id);
    if(!festivalTmpl)
        return false;

    bool b_isStart = false;
    if(festivalTmpl->state == 1) //上架
    {
        b_isStart = true;
    }
    else if(festivalTmpl->state == 2) //自动
    {
        if(IsInValidTime()) //是否在有效时间内
        {
            b_isStart = true;
        }
    }

    if(!b_isStart) 
        return false; 

    for (RoleTaskTargetList::iterator targetIt = m_targets.begin(); targetIt != m_targets.end(); ++targetIt)
    {
        if(!targetIt->GetTargetTmpl())
            continue;

        // 检查任务类型
        if(targetIt->GetTargetType() == TASK_TARGET_97)      // 有目标97的活动任务，才需要记录在线时长
        {
            return true;
        }
    }
    return false;
}

void FestiveTask::OnRewards()
{
    if(GetFestivalID() == FESTIVAL_FIRST_RECHARGE_ID)
    {   // 首冲奖励
        SERVER_MARQUEE marquee;
        marquee.id = MARQUEE_FIRST_RECHARGE;
        marquee.parame_list.push_back( m_player->GetName());
        m_player->OnMarQuee(marquee);
    }
    else if(GetFestivalID() == FESTIVAL_ADD_RECHARGE_ID && !ConfigManager::Instance()->IsTW())
    {
        uint32 totalMoney = GetTaskTargetNum(0);

        SERVER_MARQUEE marquee;
        marquee.id = MARQUEE_ADD_RECHARGE;
        marquee.parame_list.push_back(MarQueePlayerName(m_player->GetVipLevel(), m_player->GetName()));
        marquee.parame_list.push_back(MarQueeLoginReceive(MARQUEE_ADD_RECHARGE, totalMoney));
        m_player->OnMarQuee(marquee);
    }
    else if(GetFestivalID() >= FESTIVAL_DAILY_RECHARGE_BEGIN_ID && GetFestivalID() <= FESTIVAL_DAILY_RECHARGE_END_ID && !ConfigManager::Instance()->IsTW())
    {
        uint32 money = GetTaskTargetNum(0);

        SERVER_MARQUEE marquee;
        marquee.id = MARQUEE_DAY_RECHARGE;
        marquee.parame_list.push_back(MarQueePlayerName(m_player->GetVipLevel(), m_player->GetName()));
        marquee.parame_list.push_back(MarQueeLoginReceive(MARQUEE_DAY_RECHARGE, money));
        m_player->OnMarQuee(marquee);
    }
    else if(GetFestivalID() == FESTIVAL_MICRO_CLIENT_ID)
    {   // 微端奖励
        m_player->SendPlaySystemLog(PLAYSYSTEM_WD_REWARD);
    }


    if(m_targets.size() == 1 && HasTargetByType(TASK_TARGET_103))       // 对于只有103目标的活动任务的特殊处理(充值领取幸运草)
    {
        RoleTaskTargetList::iterator targetIt = m_targets.begin();
        
        uint32 counter = 0;
        while( targetIt->IsEnough() && (m_loop_num < m_template->loop_num) )
        {
            if(counter > 0)
                m_loop_num++;           // 在任务状态变为已领取时，m_loop_num算过一次

            counter++;
            targetIt->Refrash();
        }
        
        REWARD_TABLE rewardList = m_template->rewards;
        for (std::vector<REWARD_ITEM>::iterator rewardIt = rewardList.items.begin(); rewardIt != rewardList.items.end(); ++rewardIt)
        {   
            rewardIt->num = int32(rewardIt->num * counter);
        }

        m_player->TakeInRewardTable(&rewardList);
    }
    else
    {
        REWARD_TABLE fcmRewardList;

        m_player->GetFcmRewardList(m_template->rewards.items, fcmRewardList.items);
        m_player->TakeInRewardTable(&fcmRewardList);

        m_player->SendMyselfEvent(EVENT_ACIIVATION_PET, GetTaskID(), ACTIVATION_TASK, 0);
    }
}
