#include "gameholder_pch.h"
#include "offline_vigor_manager.h"
#include "player.h"
#include "off_line_vigor_config.h"
#include "trade_manager.h"
#include "game_util.h"



OfflineVigorManager::OfflineVigorManager(Player* player):
m_player(player)
{
    m_off_line_vigor_info.b_get = 1;
    m_off_line_vigor_info.count_time = 0;
    m_off_line_vigor_info.start_time = 0;
    m_off_line_vigor_info.off_voigor_point = 0;
}

OfflineVigorManager::~OfflineVigorManager(void)
{
}

void OfflineVigorManager::OnRecv(uint32 msgID, void* data)
{
    switch (msgID)
    {
    case MSG_OFF_LINE_VIGOR_REQ:
        Init(data);
        break;
    case MSG_GET_OFF_LINE_VIGOR_REQ:                            //领取离线体力
        GetOffLineVigor(data);
        break;
    default:
        break;
    }
}

void OfflineVigorManager::LoadInfo(const OFF_LINE_VIGOR_INFO& info)
{
    m_off_line_vigor_info = info;
}

void OfflineVigorManager::FillInfo(OFF_LINE_VIGOR_INFO& info)
{
    info = m_off_line_vigor_info;
}

void OfflineVigorManager::Init(void* data)
{
    SendOfflineVigorAck();
}


void OfflineVigorManager::GetOffLineVigor(void* data)
{
    GET_OFF_LINE_VIGOR_REQ* recvMsg = (GET_OFF_LINE_VIGOR_REQ*)data;
    if(!recvMsg) return;

    int32 point = 0;
    const OffLineVigor*  offline = NULL;

    GET_OFF_LINE_VIGOR_ACK sendMsg;
    sendMsg.errcode = ERR_FAILED;

    if(m_off_line_vigor_info.b_get || m_off_line_vigor_info.off_voigor_point <= 0) 
    {
        goto _END;
    }

    offline = OffLineVigorConfig::Instance()->FindOffLineVigorById(recvMsg->id);
    if(offline == NULL)
    {
        goto _END;
    }

    if (m_player->GetVipLevel() < offline->vip_level)
    {
        sendMsg.errcode = ERR_NO_ENOUGH_VIP;
        goto _END;
    }

    //if(m_player->GetMoneyPoint() < offline->money_point)
    //{
    //    sendMsg.errcode = ERR_NO_ENOUGH_MONEYPOINT;
    //    goto _END;
    //}

    point =  (int32)(m_off_line_vigor_info.off_voigor_point* ToPercent(offline->recover_percent));

    ////魔石购买
    //TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_OFF_LINE_VIGOR_BUY,
    //    PROP_OFF_LINE_VIGOR, 0,point, offline->money_point);

    BuyOffLineVigor(point);   //改称不花魔石

    return;

_END:
    m_player->SendToGate(MSG_GET_OFF_LINE_VIGOR_ACK, &sendMsg);

}
// 花费魔石购买的离线体力
void OfflineVigorManager::BuyOffLineVigor(int32 point)
{
    GET_OFF_LINE_VIGOR_ACK sendMsg;
    sendMsg.errcode = ERR_SUCCEED;
    m_player->AddBuyVigor(point);

    m_off_line_vigor_info.off_voigor_point = 0;
    m_off_line_vigor_info.b_get = 1;
    m_off_line_vigor_info.start_time = 0;

    sendMsg.point =  m_off_line_vigor_info.off_voigor_point;
    sendMsg.b_get = m_off_line_vigor_info.b_get;
    m_player->SendToGate(MSG_GET_OFF_LINE_VIGOR_ACK, &sendMsg);
}

void OfflineVigorManager::SendOfflineVigorAck()
{
    OFF_LINE_VIGOR_ACK sendMsg;
    sendMsg.b_get = m_off_line_vigor_info.b_get;
    sendMsg.point =  m_off_line_vigor_info.off_voigor_point;
    m_player->SendToGate(MSG_OFF_LINE_VIGOR_ACK, &sendMsg);
}

void OfflineVigorManager::CalDailyRefresh()
{
    CalOffLineVigorTime();

    CaloffVigorByTime();
}

//计算下线累计时间
void OfflineVigorManager::CalOffLineVigorTime()
{
    if(m_off_line_vigor_info.start_time)
    {
        time_t curTime = Crown::SDNow().GetTimeValue();
        uint32 time =  (uint32)curTime - m_off_line_vigor_info.start_time;
        m_off_line_vigor_info.count_time =  m_off_line_vigor_info.count_time + time;
        m_off_line_vigor_info.start_time = 0;
    }
}

//根据累计时间计算体力点
void OfflineVigorManager ::CaloffVigorByTime()
{
    //if(m_off_line_vigor_info.count_time > 0)
    //{
    //    int32 topoint = 0;
    //    topoint = GameParamConfig::Instance()->GetOffLineVigorTimePoint();
    //    int32  point = ( m_off_line_vigor_info.count_time) /topoint; //        //30分换1点
    //    if(point > 0)
    //    {
    //        uint32 maxpoint = DAY_SECONDS / topoint; //最大的体力点

    //        m_off_line_vigor_info.off_voigor_point = point;

    //        if( m_off_line_vigor_info.off_voigor_point >= (uint32)maxpoint)
    //            m_off_line_vigor_info.off_voigor_point = maxpoint;

    //    }

    //    m_off_line_vigor_info.b_get = 0;
    //    m_off_line_vigor_info.start_time = 0;
    //    m_off_line_vigor_info.count_time = 0;

    //    SendOfflineVigorAck();
    //}
    //else
    //{
    //    //在线的时候更新，那么前一天的离线时间为0
    //    m_off_line_vigor_info.b_get = 1;
    //    m_off_line_vigor_info.start_time = 0;
    //    m_off_line_vigor_info.count_time = 0;
    //    m_off_line_vigor_info.off_voigor_point =  0;

    //    SendOfflineVigorAck();
    //}



}
