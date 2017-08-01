#include "gameholder_pch.h"
#include "online_reward_manager.h"
#include "online_reward_config.h"
#include "player.h"
#include "item_manager.h"
#include "game_util.h"
#include "event_define.h"


OnlineRewardManager::OnlineRewardManager(Player* player) :
m_player(player)
{
    Clear();
}

OnlineRewardManager::~OnlineRewardManager()
{
}

void OnlineRewardManager::Clear()
{
    m_online_reward_info.Clear();
    m_secondTimer = 0;
}

// ==================================================
// 接收信息
// ==================================================
void OnlineRewardManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_ONLINE_REWARD_REQ:
        RecvOnlineRewardReq(data);
        break;
    case MSG_ONLINE_REWARD_INFO_REQ:
        InitOnlineRewardInfo();
        break;
    case MSG_GROW_LEVEL_GIFT_REQ:
        OnGrowLevelReq(data);
        break;
    case MSG_BESTOW_REWARD_REQ:
        BestowRewardReq(data);
        break;
    default:
        break;
    }
}

void OnlineRewardManager::BestowRewardReq(void* data)
{
    BESTOW_REWARD_REQ* recMsg = (BESTOW_REWARD_REQ*)data;
    if(!recMsg)
        return;

    BESTOW_REWARD_ACK sendMsg;
    sendMsg.errcode = ERR_FAILED;

    // 检查是否领过奖励
    int32 fixedday = OnlineRewardConfig::Instance()->GetOnlineDay(); // 固定天数，读取表
    if(fixedday <= 0)
        CnAssert(false);

    if(recMsg->fixed_reward_num != m_online_reward_info.fixed_reward_num)
        return;

    time_t curTime = Crown::SDNow().GetTimeValue();
    int32 days = m_player->GetDaysBetweenDates(curTime, (time_t)m_online_reward_info.day);  // 距离上次领取的时间间隔
    int32 alltime = m_online_reward_info.day + m_online_reward_info.interval_time;      // 下次可领取周期时间点

    if(days >= fixedday)            // 时间间隔大于领取间隔周期(必然可以领取)
    {
        CalBestowReward(sendMsg, days, (int32)curTime);
    }
    else if(curTime >= alltime)     // 到了再次可领取的周期时间点
    {
        int32 days = m_player->GetDaysBetweenDates(curTime, (time_t)alltime);
        CalBestowReward(sendMsg, days, (int32)curTime);
    }
    else
    {
        if(alltime > curTime)
            sendMsg.time = alltime - (int32)curTime;
        else
            CnAssert(false);
    }

    m_player->SendToGate(MSG_BESTOW_REWARD_ACK, &sendMsg);
}

// 领取福利奖励的天数和奖励分发(days:到上次领取的间隔天数)
void OnlineRewardManager::CalBestowReward(BESTOW_REWARD_ACK& sendMsg, int32 days, int32 curTime)
{
    if(days < 0)
    {
        CnAssert(false);
        return;
    }

    int32 fixedday = OnlineRewardConfig::Instance()->GetOnlineDay(); // 间隔多少天可领取一次恩赐

    int32 curday = days % fixedday;
    int32 surplus = fixedday - curday;
    int32 howtime = surplus * DAY_SECONDS; 
    m_online_reward_info.day = (int32)curTime;
    m_online_reward_info.interval_time = howtime;       // 下次领取恩赐的时间间隔

    // 对应轮数的奖励
    REWARD_TABLE fixedReward;
    OnlineRewardConfig::Instance()->FindFixedReward(fixedReward, m_online_reward_info.fixed_reward_num);
    FindGrowReward(fixedReward);
    m_player->TakeInRewardTable(&fixedReward);

    // 设置领过奖励
    int32 fixedindex = m_online_reward_info.fixed_reward_num;
    m_online_reward_info.fixed_reward_num = OnlineRewardConfig::Instance()->FindAppointNextIndex(ONLINE_FIXED_REWARD, fixedindex);//下一轮;
    int32 growindex = m_online_reward_info.grow_reward_num;
    m_online_reward_info.grow_reward_num = OnlineRewardConfig::Instance()->FindAppointNextIndex(ONLINE_GROW_REWARD, growindex);

    ClearGrowList();

    sendMsg.grownum = m_online_reward_info.grow_reward_num;
    sendMsg.fixednum = m_online_reward_info.fixed_reward_num;
    sendMsg.grow_list = m_online_reward_info.grow_list;
    sendMsg.time = howtime;
    sendMsg.errcode  = ERR_SUCCEED;

    m_player->SendPlaySystemLog(PLAYSYSTEM_ONLINE_REWARD_BESTOW, sendMsg.grownum, sendMsg.fixednum);
}
// ==================================================
// 加载在线奖励信息
// ==================================================
void OnlineRewardManager::LoadInfo(const ONLINE_REWARD_INFO& info)
{
    m_online_reward_info = info;
}
// ==================================================
// 填充在线奖励信息
// ==================================================
void OnlineRewardManager::FillInfo(ONLINE_REWARD_INFO& info)
{
    info = m_online_reward_info;
}
// ==================================================
// 是否在线奖励被标记
// ==================================================
bool OnlineRewardManager::IsOnlineRewardFlag(uint32 id)
{
    uint32 flag = 1 << (id % 32);

    if(id >= 32)
    {
        CnAssert(false);
        return true;
    }

    flag = m_online_reward_info.online_reward_flag & flag;

    if(flag)
        return true;

    return false;
}
// ==================================================
// 设置在线奖励标记
// ==================================================
void OnlineRewardManager::SetOnlineRewardFlag(uint32 id)
{
    uint32 flag = 1 << (id % 32);
    m_online_reward_info.online_reward_flag |= flag;
}

bool OnlineRewardManager::IsLevelRewardFlag(int32 level)
{
    uint32 flag = 1 << level;

    flag = m_online_reward_info.online_level_reward_flag & flag;

    if(flag) return true;

    return false;
}

void OnlineRewardManager::SetLevelRewardFlag(int32 level)
{
    uint32 flag = 1 << level;
    m_online_reward_info.online_level_reward_flag |= flag;
}

void OnlineRewardManager::InitOnlineRewardInfo()
{
    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_ONLINE_REWARD, m_player) != ERR_SUCCEED)
    //{
    //    return;
    //}

    ONLINE_REWARD_INFO_ACK sendmsg;

    uint32 now = (int32)Crown::SDNow().GetTimeValue();

    if(m_online_reward_info.level <= 0)
        m_online_reward_info.level = 0;

    bool b_exe  = false;
    if(m_online_reward_info.day <= 0)
    {
        m_online_reward_info.day = now;
        m_online_reward_info.open_time = now;
        m_online_reward_info.interval_time = OnlineRewardConfig::Instance()->GetOnlineDay()*DAY_SECONDS;    // 下次领取恩赐的时间间隔
    }

    if(m_online_reward_info.fixed_reward_num <= 0)
        m_online_reward_info.fixed_reward_num = 1;

    if(m_online_reward_info.grow_reward_num <= 0)
        m_online_reward_info.grow_reward_num = 1;

    InitOnlineRewardGrowList();

    sendmsg.finish_time = GetOnlineRewardTime();

    sendmsg.online_info = m_online_reward_info;

    //成长恩赐的倒计时
    sendmsg.time = OnGetGrowBestowtime();
    m_player->SendToGate(MSG_ONLINE_REWARD_INFO_ACK, &sendmsg);
}

void OnlineRewardManager::InitOnlineRewardGrowList()
{
    if(m_online_reward_info.grow_list.size() <= 0)
    {
        REWARD_TABLE rewardtable;
        OnlineRewardConfig::Instance()->FindGrowReward(rewardtable, 1);
        std::vector<REWARD_ITEM>::iterator rewarditer;
        for (rewarditer= rewardtable.items.begin(); rewarditer != rewardtable.items.end(); rewarditer++)
        {
            rewarditer->num = 0;
            m_online_reward_info.grow_list.push_back(*rewarditer);
        }
    }
}

// 计算领取成长赐福的时间差
int32 OnlineRewardManager::OnGetGrowBestowtime()
{
    int32 inter_time = 0; // 时间差
    int32 fixedday = OnlineRewardConfig::Instance()->GetOnlineDay();    // 固定天数，读取表
    time_t curTime = Crown::SDNow().GetTimeValue();

    int32 days = m_player->GetDaysBetweenDates(curTime, (time_t)m_online_reward_info.day);
    if(days >= fixedday)
    {
        inter_time = 0;
    }
    else
    {
        int32 alltime = m_online_reward_info.day +  m_online_reward_info.interval_time;    // 间隔时间

        int32 itertime = m_player->GetDaysBetweenDates(alltime, curTime);
        inter_time = itertime* DAY_SECONDS;
    }

    return inter_time;
}

void OnlineRewardManager::OnGrowLevelReq(void* data)
{
    GROW_LEVEL_GIFT_REQ* recMsg = (GROW_LEVEL_GIFT_REQ*)data;
    if(!recMsg)
        return;

    GROW_LEVEL_GIFT_ACK sendMsg;
    sendMsg.errcode = ERR_FAILED;
    if(m_player->GetLevel() < recMsg->level)
    {
        m_player->NotifyErrorCode(ERR_NO_ENOUGH_PLAYER_LEVEL);
        return;
    }

    if(m_online_reward_info.level && m_player->GetLevel() <= (int32)m_online_reward_info.level )
        return;

    if(IsLevelRewardFlag( recMsg->level))
    {
        m_player->NotifyErrorCode(ERR_ONLIEN_REWARD_GET);
        return;
    }

    REWARD_TABLE* rewardtable = OnlineRewardConfig::Instance()->FindIdByLevelReward(recMsg->level);

    if(rewardtable)
    {
        // 已经领取过奖励
        if(recMsg->level < (int32)m_online_reward_info.level)
            sendMsg.errcode = ERR_FAILED;
        else
        {
            m_player->TakeInRewardTable(rewardtable);

            m_online_reward_info.level = recMsg->level; // 当前一等级
            SetLevelRewardFlag( m_online_reward_info.level);
            sendMsg.errcode = ERR_SUCCEED;
            sendMsg.level = m_online_reward_info.level;

            m_player->SendPlaySystemLog(PLAYSYSTEM_ONLINE_REWARD_LEVEL, m_online_reward_info.level);
        }
    }

    m_player->SendToGate(MSG_GROW_LEVEL_GIFT_ACK, &sendMsg);
    if(sendMsg.errcode == ERR_SUCCEED)
    {
        m_player->SendTaskEvent(TASK_EVENT_ONLINE_GROW_REWARD, recMsg->level, 0, 0);
    }
}
// ==================================================
// 刷新在线奖励请求
// ==================================================
void OnlineRewardManager::CalDailyRefresh()
{
    ONLINE_REWARD_ACK sendMsg;
    Crown::CSDDateTime now = Crown::SDNow();
    Crown::CSDDateTime nowDay;

    nowDay = now;
    nowDay.SetTime(0, 0, 0);

    sendMsg.errcode = ERR_SUCCEED;

    m_online_reward_info.cirtnum = 0;                   // 必暴击的次数
    SetOnlineRewardDay((uint32)nowDay.GetTimeValue());  // 重置天

    SetOnlineRewardTime(0);                             // 重置时间
    sendMsg.finish_time = GetOnlineRewardTime();

    m_online_reward_info.online_reward_flag = 0;        // 重置标记
    sendMsg.online_reward_flag = GetOnlineRewardFlag();

    sendMsg.dayfresh = 1;
    sendMsg.time = OnGetGrowBestowtime();

    sendMsg.list = m_online_reward_info.grow_list;
    m_player->SendToGate(MSG_ONLINE_REWARD_ACK, &sendMsg);
}

void OnlineRewardManager::Update()
{
    // 每5秒更新时间
    if(m_secondTimer < 5000)
    {
        m_secondTimer += g_ServerUpdateTime;
        return;
    }
    m_secondTimer = 0;

    //// 在线奖励时间递加
    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_ONLINE_REWARD, m_player) == ERR_SUCCEED)
    //    m_online_reward_info.online_reward_time += 5;
}

// ==================================================
// 接收在线奖励请求
// ==================================================
void OnlineRewardManager::RecvOnlineRewardReq(void* data)
{
    ONLINE_REWARD_REQ* recvMsg = (ONLINE_REWARD_REQ*)data;

    ONLINE_REWARD_ACK sendMsg;
    sendMsg.errcode = ERR_FAILED;

    const OnlineReward* onlineTmp = OnlineRewardConfig::Instance()->GetOnlineReward(recvMsg->id);
    if(!onlineTmp)
        goto _END;

    // 领奖时间未到
    if(GetOnlineRewardTime() < onlineTmp->time)
        goto _END;

    // 已经领过奖励
    if(IsOnlineRewardFlag(onlineTmp->id))
        goto _END;

    // 增加成长值
    if(!RandomGrowValue(onlineTmp->id))
    {
        CnAssert(false);
        goto _END;
    }

    // 设置领取过奖励标记
    SetOnlineRewardFlag(onlineTmp->id);

    // 奖励物品
    m_player->TakeInRewardTable(&onlineTmp->rewardTable);

    sendMsg.errcode = ERR_SUCCEED;

    if(m_onlinechangItem.size() > 0)
        sendMsg.list = m_onlinechangItem;
    else
        sendMsg.list = m_online_reward_info.grow_list;

    m_player->SendPlaySystemLog(PLAYSYSTEM_ONLINE_REWARD, sendMsg.time, sendMsg.finish_time);

_END:
    sendMsg.online_reward_flag = GetOnlineRewardFlag();
    sendMsg.finish_time = GetOnlineRewardTime();
    sendMsg.dayfresh = 0;

    m_player->SendToGate(MSG_ONLINE_REWARD_ACK, &sendMsg);
}

bool OnlineRewardManager::RandomGrowValue(int32 curnum)
{
    m_onlinechangItem.clear();
    // 单个增加概率
    int32 cur_sing_probability = OnlineRewardConfig::Instance()->GetOnlineSingleProbablity();
    int32 sing_probability =  cur_sing_probability;
    int32 crit_probability  = OnlineRewardConfig::Instance()->GetOnlineCirtProblity() ;

    int32 maxvalue = OnlineRewardConfig::Instance()->GetMaxID();//最大次数
    int32 maxcirtenum = OnlineRewardConfig::Instance()->GetCritNum();

    int32 hownum = maxvalue - maxcirtenum;

    if(hownum <= curnum)            // 必现暴击次数
    {
        if(m_online_reward_info.cirtnum < maxcirtenum)
            return AddCritGrowreward();
    }
    int32 randomMode = Crown::SDRandom(sing_probability + crit_probability);

    if(0 <= randomMode &&  randomMode <= sing_probability)
    {
        return RandomSingleGrowValue();
    }
    else if (sing_probability < randomMode && randomMode <= sing_probability+crit_probability)
    {
        return AddCritGrowreward();
    }

    return false;
}

bool OnlineRewardManager::RandomSingleGrowValue()
{
    int32 random = Crown::SDRandom (BASE_RATE_NUM);
    //随机物品根据概率
    REWARD_ITEM* curitem = OnlineRewardConfig::Instance()->RandomSingleProbility(random);
    if(!curitem)
    {
        CnAssert(false);
        return false;
    }
    //第几个物品的百分比
    return CalculationGrowValue(curitem);
}

bool OnlineRewardManager::CalculationGrowValue(REWARD_ITEM* curitem)
{
    if(!curitem)
        return false;

    //查找成长表里的物品的数量
    int32 num = m_online_reward_info.grow_reward_num; // 轮数
    REWARD_ITEM curRewaditem;
    OnlineRewardConfig::Instance()->FindGrowRewardByOrder(curitem->flag, num, curRewaditem);

    int32 itemnum = CalAddItemNum(curRewaditem.num);
    //AddGrowReward(curRewaditem.type, itemnum, curRewaditem.id, SingleType);
    return true;
}

bool OnlineRewardManager::AddCritGrowreward()
{
    OnlineRewardConfig* onlineRewardConfig =  OnlineRewardConfig::Instance();
    if(!onlineRewardConfig)
        return false;

    //根据轮数查找对应的物品的奖励
    REWARD_TABLE rewardtable;
    onlineRewardConfig->FindGrowReward(rewardtable,  m_online_reward_info.grow_reward_num);

    std::vector<REWARD_ITEM>::iterator rewarditer;
    for (rewarditer = rewardtable.items.begin(); rewarditer != rewardtable.items.end(); rewarditer++)
    {
        int32 num = CalAddItemNum(rewarditer->num);//计算增加物品的百分比
        //AddGrowReward(rewarditer->type, num, rewarditer->id, CrityType);
    }

    m_online_reward_info.cirtnum = m_online_reward_info.cirtnum + 1;
    return true;
}
//计算添加物品的数量
int32 OnlineRewardManager::CalAddItemNum(int32 num)
{
    int32 percent = OnlineRewardConfig::Instance()->GetAddGrowPrecent();
    int32 additemnum = int32(num * ToPercent(percent));
    if(additemnum < 0)
        additemnum = 0;
    return additemnum;
}

void OnlineRewardManager::ClearGrowList()
{
    std::vector<REWARD_ITEM>::iterator itergrowreward;
    for(itergrowreward = m_online_reward_info.grow_list.begin(); itergrowreward != m_online_reward_info.grow_list.end(); itergrowreward++)
    {
        itergrowreward->num = 0;
    }
}

void OnlineRewardManager::FindGrowReward(REWARD_TABLE& rewardtable)
{
    std::vector<REWARD_ITEM>::iterator itergrowreward;
    for(itergrowreward = m_online_reward_info.grow_list.begin(); itergrowreward != m_online_reward_info.grow_list.end(); itergrowreward++)
    {
        if(itergrowreward->num >0)
            rewardtable.items.push_back(*itergrowreward);
    }
}

void OnlineRewardManager::AddGrowReward(int32 type, int32 num, int32 id, int32 randomtype)
{
    bool b_find = false;
    std::vector<REWARD_ITEM>::iterator itergrowreward;
    //for(itergrowreward = m_online_reward_info.grow_list.begin(); itergrowreward != m_online_reward_info.grow_list.end(); itergrowreward++)
    //{
    //    if(itergrowreward->type == type && itergrowreward->id == id)
    //    {
    //        itergrowreward->num = itergrowreward->num + num;
    //        b_find = true;
    //        if(randomtype == SingleType)
    //            m_onlinechangItem.push_back(*itergrowreward);
    //        break;
    //    }
    //}

    if(!b_find)
    {
        REWARD_ITEM  growreward;
        growreward.id = id;
        //growreward.type = type;
        growreward.num = num;
        m_online_reward_info.grow_list.push_back(growreward);
        if(randomtype == SingleType)
            m_onlinechangItem.push_back(growreward);
    }

    if(randomtype == CrityType)
        m_onlinechangItem.clear();
}
