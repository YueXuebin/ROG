#include "gameholder_pch.h"
#include "egg_manager.h"
#include "event_define.h"
#include "player.h"
#include "task_festive_config.h"
#include "egg_config.h"
#include "item_manager.h"
#include "package_manager.h"
#include "game_util.h"
#include "task_manager.h"


EggManager::EggManager(Player *player) : 
m_player(player)
{

}

EggManager::~EggManager(void)
{

}

void EggManager::LoadEggInfo( EGG_ACTIVITY_INFO &info )
{
    m_eggListInfo = info;
    InitConfig();
}

void EggManager::FillEggInfo( EGG_ACTIVITY_INFO &info )
{
    info = m_eggListInfo;
}

void EggManager::OnRecv( uint32 msgID, void* data )
{
    switch (msgID)
    {
    case MSG_EGG_INFO_REQ:
        {
            OnEggInfReq(msgID, data);
        }
        break;
    case MSG_EGG_SMASH_REQ:
        {
            OnSmashEggInfReq(msgID, data);
        }
        break;
    }
}

void EggManager::OnEggInfReq( uint32 msgID, void *data )
{
    SendEggListNotify();
}

void EggManager::OnSmashEggInfReq( uint32 msgID, void *data )
{
    ERROR_CODE *recvMsg = (ERROR_CODE*)data;
    if(!recvMsg)
    {
        m_player->NotifyErrorCode(ERR_FAILED);
        return;
    }
    
    if(recvMsg->errcode == EGG_ONE_KEY_GET)
    {
        OnOneKeyEggHandle(recvMsg->errcode); 
    }
    else
    {
        const EGG_INFO_CONFI* config = EggConfig::Instance()->GetEggItemByType(recvMsg->errcode);
        if(config == NULL)
        {
            SendSmashEggNotify(ERR_UNFOUND_CONFIG);
            return;
        }

        EGG_INFO* info = GetEggInfo(recvMsg->errcode);
        if(info == NULL)
        {
            SendSmashEggNotify(ERR_UNFOUND_CONFIG);
            return;
        }
        
       if(info->num >= config->need_num)
       {
            REWARD_TABLE rewardTable;
            FASHION_GET_TREASURE_ACK sendMsg;
            uint16 error = ERR_FAILED;

            RandomRewardHandle(config->type, config->package_id, &rewardTable.items);

            //// 检查包裹
            //if(!m_player->GetItemManager()->CanAddRewardTable(rewardTable))
            //{
            //    SendSmashEggNotify(ERR_BAG_FULL);
            //    return;
            //}

            ReduceEggNum(recvMsg->errcode, config->need_num);

            for (std::vector<REWARD_ITEM>::iterator re = rewardTable.items.begin(); re != rewardTable.items.end(); re++)
            {
                //m_player->GetBag()->AddItemByNum(re->id, re->num, REWARD_REASON_EGG_REWARD);
                if(re->flag > 0)
                {
                    EggMarquee( int32(re->flag / 10), re->id, 1);
                }
            }
            if(rewardTable.items.size() <= 0)
            {
                error = ERR_FAILED;
            }
            else
            {
                error = ERR_SUCCEED;
            }

            m_player->SendTaskEvent(TASK_EVENT_EGG_SMASH_NUM, recvMsg->errcode , 1);

            m_eggListInfo.last_time = (uint32)Crown::SDNow().GetTimeValue();
            sendMsg.errcode = error;
            sendMsg.type = recvMsg->errcode;
            sendMsg.item_list = rewardTable.items;
            sendMsg.last_finish_time = m_eggListInfo.last_time;
            m_player->SendToGate(MSG_EGG_SMASH_ACK, &sendMsg);
            SendEggListNotify();
       }
       else
       {
           SendSmashEggNotify(ERR_FAILED);
       }
    }
}

void EggManager::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 )
{
    switch (eventId)
    {
    case EVENT_TASK_NUM_DAILY_LOGIN:
        {
            //每日登陆奖励计数
           bool status= TaskFestiveConfig::Instance()->IsInValidTime(m_player->GetCenterSvrID(), FESTIVAL_ACTIVITY_EGG_ID);
           if(status)
           {
                AddEggNum(EGG_TYPE_NORMAL, arg2 * 3);
                m_eggListInfo.day += 1;
           }
        }
        break;
    case EVENT_TASK_NUM_TOTAL_CONSUMPTION:
        {
            //每日登陆奖励计数
            CnDbg(" EGG EVENT_TASK_NUM_TOTAL_CONSUMPTION event %d\n", arg2);
            bool status= TaskFestiveConfig::Instance()->IsInValidTime(m_player->GetCenterSvrID(), FESTIVAL_ACTIVITY_EGG_ID);
            if(status)
            {
                AddEggNum(EGG_TYPE_SILVER, arg2);
                CnDbg(" EGG EVENT_TASK_NUM_TOTAL_CONSUMPTION add %d\n", arg2);
            }
        }
        break;
    case EVENT_TASK_NUM_CUMULATIVE_RECHARGE:
        {
            //每日登陆奖励计数
            CnDbg(" EGG EVENT_TASK_NUM_CUMULATIVE_RECHARGE event %d\n", arg2);
            bool status= TaskFestiveConfig::Instance()->IsInValidTime(m_player->GetCenterSvrID(), FESTIVAL_ACTIVITY_EGG_ID);
            if(status)
            {
                AddEggNum(EGG_TYPE_GLOD, arg2);
                CnDbg(" EGG EVENT_TASK_NUM_CUMULATIVE_RECHARGE add %d\n", arg2);
            }
        }
        break;
    }
}

void EggManager::SendEggListNotify()
{
   m_player->SendToGate(MSG_EGG_INFO_ACK,&m_eggListInfo);
}

void EggManager::InitConfig()
{
    bool status= TaskFestiveConfig::Instance()->IsInValidTime(m_player->GetCenterSvrID(), FESTIVAL_ACTIVITY_EGG_ID);
    if(!status)
    {
        return;
    }

    if(m_eggListInfo.last_time  == 0)
    {
        // 初始化时间
        m_eggListInfo.last_time = (uint32)Crown::SDNow().GetTimeValue();
    }

    const TASK_FESTIVAL_TEMPLATE* festivalTmpl = TaskFestiveConfig::Instance()->GetFestivalTmpl(m_player->GetCenterSvrID(), FESTIVAL_ACTIVITY_EGG_ID);

    // 如果在活动时间内
    if(((uint32)m_eggListInfo.last_time < festivalTmpl->begin_time) || ((uint32)m_eggListInfo.last_time > festivalTmpl->end_time))
    {
        // 上次使用时间不在当前活动时间内 重置活动信息
        m_eggListInfo.Clear();
        m_eggListInfo.day = 0;
        m_eggListInfo.last_time = (uint32)Crown::SDNow().GetTimeValue();
        ResetTaskFestival(m_player->GetCenterSvrID(), FESTIVAL_ACTIVITY_EGG_ID);
    }
    
    if(m_eggListInfo.egg_list.size() <= 0)
    {
        EGG_ITEM_LIST config = EggConfig::Instance()->GetEggConfigList();
        for(EGG_ITEM_LIST::const_iterator el = config.begin(); el != config.end(); el++)
        {
            bool flag = true;
            for(std::vector<EGG_INFO>::iterator gl = m_eggListInfo.egg_list.begin(); gl != m_eggListInfo.egg_list.end(); gl++)
            {
                if(gl->type == el->type)
                    flag = false;
            }

            if(flag)
            {
                EGG_INFO info;
                info.type = el->type;
                info.num = 0;
                info.total = 0;
                m_eggListInfo.egg_list.push_back(info);
            }
        }
    }
}

void EggManager::AddEggNum( uint32 type, uint32 num )
{
    for(std::vector<EGG_INFO>::iterator gl = m_eggListInfo.egg_list.begin(); gl != m_eggListInfo.egg_list.end(); gl++)
    {
        if(gl->type == type)
        {
            gl->num += num;
            gl->total += num;
            SendEggListNotify();
            break;
        }
    }
}

void EggManager::ReduceEggNum( uint32 type, uint32 num )
{
    for(std::vector<EGG_INFO>::iterator gl = m_eggListInfo.egg_list.begin(); gl != m_eggListInfo.egg_list.end(); gl++)
    {
        if(gl->type == type)
        {
            gl->num -= num;
            break;
        }
    }
}

void EggManager::RandomRewardHandle( int type, int goodf, std::vector<REWARD_ITEM>* itemList )
{
    if(!itemList)
        return;
    //根据包裹抽物品
    const TREASURE_PACKAGE_ITEM* packageItem = m_player->GetPackageManager()->GetPackageItem(goodf, PACKAGE_TYPE_INCARNATION);
    if(!packageItem)
    {
        m_player->NotifyErrorCode(ERR_FAILED);
        return;
    }

    REWARD_ITEM rewardItem;
    int32 num = 0;
    if(packageItem->count_max > packageItem->count_min)
    {
        //随机物品数量
        num = packageItem->count_min + Crown::SDRandom(packageItem->count_max - packageItem->count_min);
    }
    else
    {
        num =  packageItem->count_min;
    }
    //添加抽到的物品
    rewardItem.id = packageItem->item_id;
    rewardItem.num = num;

    // 公告
    if(packageItem->notice > 0)
        rewardItem.flag = type * 10 + packageItem->notice;
    
    itemList->push_back(rewardItem);
}

EGG_INFO* EggManager::GetEggInfo( uint32 type )
{
    for(std::vector<EGG_INFO>::iterator gl = m_eggListInfo.egg_list.begin(); gl != m_eggListInfo.egg_list.end(); gl++)
    {
        if(gl->type == type)
        {
             return &(*gl);
        }
    }
    return NULL;
}

void EggManager::OnOneKeyEggHandle( uint32 type )
{

    REWARD_TABLE rewardTable;
    FASHION_GET_TREASURE_ACK sendMsg;
    uint16 error = ERR_FAILED;

    for(std::vector<EGG_INFO>::iterator gl = m_eggListInfo.egg_list.begin(); gl != m_eggListInfo.egg_list.end(); gl++)
    {
        const EGG_INFO_CONFI* config = EggConfig::Instance()->GetEggItemByType(gl->type);
        if(config == NULL)
        {
            SendSmashEggNotify(ERR_UNFOUND_CONFIG);
            return;
        }

        int32 cnum = int32(gl->num / config->need_num);
        if(cnum <= 0)
            continue;

        for (int32 i = 0; i < cnum; i++)
        {
            RandomRewardHandle(config->type, config->package_id, &rewardTable.items);
        }
    }

    
    //if(!m_player->GetItemManager()->CanAddRewardTable(rewardTable))
    //{
    //    // 检查包裹 失败
    //    SendSmashEggNotify(ERR_BAG_FULL);
    //    return;
    //}

    for(std::vector<EGG_INFO>::iterator gl = m_eggListInfo.egg_list.begin(); gl != m_eggListInfo.egg_list.end(); gl++)
    {
        const EGG_INFO_CONFI* config = EggConfig::Instance()->GetEggItemByType(gl->type);
        if(config == NULL)
        {
            SendSmashEggNotify(ERR_UNFOUND_CONFIG);
            return;
        }

        int32 cnum = int32(gl->num / config->need_num);
        if(cnum <= 0)
            continue;

        ReduceEggNum(config->type, cnum * config->need_num);

        m_player->SendTaskEvent(TASK_EVENT_EGG_SMASH_NUM, config->type , cnum);

    }

    for (std::vector<REWARD_ITEM>::iterator re = rewardTable.items.begin(); re != rewardTable.items.end(); re++)
    {
        //m_player->GetBag()->AddItemByNum(re->id, re->num, REWARD_REASON_EGG_REWARD);
        if(re->flag > 0)
        {
            EggMarquee(int32(re->flag / 10), re->id, 1);
        }
    }

    if(rewardTable.items.size() <= 0)
    {
        error = ERR_FAILED;
    }
    else
    {
        error = ERR_SUCCEED;
    }

    m_eggListInfo.last_time = (uint32)Crown::SDNow().GetTimeValue();
    sendMsg.errcode = error;
    sendMsg.type = type;
    sendMsg.item_list = rewardTable.items;
    sendMsg.last_finish_time = m_eggListInfo.last_time;
    m_player->SendToGate(MSG_EGG_SMASH_ACK, &sendMsg);
    SendEggListNotify();
}

void EggManager::SendSmashEggNotify( uint32 err )
{
    FASHION_GET_TREASURE_ACK sendMsg;
    sendMsg.errcode = err;
    m_player->SendToGate(MSG_EGG_SMASH_ACK, &sendMsg);
}

void EggManager::GmAddEgg( uint32 type, uint32 num )
{
    if(type == EGG_ONE_KEY_GET)
    {
        m_eggListInfo.Clear();
        m_eggListInfo.day = 0;
        m_eggListInfo.last_time = (uint32)Crown::SDNow().GetTimeValue();
        ResetTaskFestival(m_player->GetCenterSvrID(), FESTIVAL_ACTIVITY_EGG_ID);
        InitConfig();
        SendEggListNotify();
        return;
    }

    AddEggNum(type, num);
}

void EggManager::EggMarquee( int32 type, int32 id, int32 num )
{
    SERVER_MARQUEE marquee;
    marquee.id = MARQUEE_EGG;
    marquee.parame_list.push_back(MarQueePlayerName(m_player->GetVipLevel(), m_player->GetName()));
    marquee.parame_list.push_back(MarEgg(type, id, num));
    m_player->OnMarQuee(marquee);
}

bool EggManager::ResetTaskFestival( uint32 centerID, uint32 festivalID )
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
    m_player->GetTaskManager()->ResetTask(idList);
    return true;
}

