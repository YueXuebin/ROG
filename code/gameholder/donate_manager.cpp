#include "gameholder_pch.h"
#include "player.h"
#include "donate_manager.h"
#include "donate_config.h"
#include "trade_manager.h"
#include "game_util.h"
#include "gameplaytimes_notifier.h"
#include "mail_manager.h"
#include "mail_title_config.h"
#include "title_manager.h"
#include "mail_manager.h"
#include "task_festive_config.h"

DonateManager::DonateManager(Player* player) :
m_player(player)
{

}

DonateManager::~DonateManager()
{

}

void DonateManager::LoadDonateInfo( const DONATE_INFO& info )
{
    m_donate_info = info;
}

void DonateManager::FillDonateInfo( DONATE_INFO& info )
{
    info = m_donate_info;
}

void DonateManager::OnRecv( uint32 msgID, void* data )
{
    switch (msgID)
    {
    case MSG_DONATE_INFO_REQ:
        {
            onDonateInfo();
            break;
        }
    case MSG_DONATE_PUT_REQ:
        {
            onDonatePut(data);
            break;
        }
    case MSG_DONATE_RECEIVE_REQ:
        {
            onRewardReq();
            break;
        }
    case MSG_DONATE_REWARD_MONEYPOINT:
        {
            onRewardMoneypoint(data);
            break;
        }
    default:
        break;
    }
}

void DonateManager::onDonateInfo()
{
    const DonateTemplate& donateTemp = DonateConfig::Instance()->GetDonateTemplate();

    m_donate_info.timer = getTimer();

    DONATE_INFO sendMsg;
    sendMsg.Clear();
    sendMsg = m_donate_info;

    NONE centerMsg;
    m_player->SendToCenter(MSG_DONATE_INFO_REQ, &centerMsg);

    m_player->SendToGate(MSG_DONATE_INFO_ACK,&sendMsg);
}

void DonateManager::onDonatePut(void* data)
{
    DONATE_PUT* recvMsg = (DONATE_PUT*)data;

    uint32 err_code = ERR_FAILED;

    const DonateTemplate& donateTemp = DonateConfig::Instance()->GetDonateTemplate();

    int32 tempTimer = getTimer();

    if(tempTimer <= 0)
    {
        m_donate_info.timer = tempTimer;
        goto _END;
    }

    if(m_donate_info.state != NO_REWARD)
    {
        goto _END;
    }

    if (m_player->GetMoneyPoint() >= int32(donateTemp.money_point))
    {
        err_code = ERR_SUCCEED;

        std::string str = "";
        str += Crown::ToString(donateTemp.money_point);
        TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_DONATE, PROP_DONATE, 0, 1, donateTemp.money_point, str);
    }

_END:
    ERROR_CODE sendMsg;
    sendMsg.Clear();
    sendMsg.errcode = err_code;

    m_player->SendToGate(MSG_DONATE_PUT_ACK,&sendMsg);
}

void DonateManager::onRewardReq()
{
    uint32 err_code = ERR_FAILED;
    if (m_donate_info.state == CAN_ACCEPT_REWARD)
    {
        m_donate_info.state = ACCEPTED_REWARD;
        const DonateTemplate& donateTemp = DonateConfig::Instance()->GetDonateTemplate();

        m_player->TakeInRewardTable(PROP_ITEM, donateTemp.title_id, 1, REWARD_DONATE);

        err_code = ERR_SUCCEED;

        SERVER_MARQUEE marquee;
        marquee.id = MARQUEE_DONATE_TITLE;
        marquee.parame_list.push_back(MarQueePlayerName(m_player->GetVipLevel(), m_player->GetName()));
        marquee.parame_list.push_back(MarQueeItem(PROP_ITEM, 1, donateTemp.title_id));
        m_player->OnMarQuee(marquee);
    }

    ERROR_CODE sendMsg;
    sendMsg.Clear();
    sendMsg.errcode = err_code;
    m_player->SendToGate(MSG_DONATE_RECEIVE_ACK,&sendMsg);

    sendDanateInfoAck();
}

void DonateManager::onRewardMoneypoint(void* data)
{
    //DONATE_REWARD_MONEYPOINT* recvMsg = (DONATE_REWARD_MONEYPOINT*)data;

    //REWARD_TABLE rewardTable;
    //REWARD_ITEM rewardItem;
    //rewardItem.type = PROP_BIND_MONEYPOINT;
    //rewardItem.num = recvMsg->money_point * DonateConfig::Instance()->GetDonateTemplate().reward_percent / BASE_RATE_NUM;
    //rewardTable.items.push_back(rewardItem);
    //MailManager::SendMessageMail(m_player->GetCenterSvrID(), 0, m_player->GetName(), rewardTable.items, MAIL_MESSAGE_TYPE_DONATE, 0);
}

void DonateManager::sendDanateInfoAck()
{
    DONATE_INFO sendMsg;
    sendMsg.Clear();
    sendMsg = m_donate_info;

    m_player->SendToGate(MSG_DONATE_INFO_ACK,&sendMsg);
}

void DonateManager::onMoneyPointAck( const std::string& str )
{
    const DonateTemplate& donateTemp = DonateConfig::Instance()->GetDonateTemplate();

    m_donate_info.state = CAN_ACCEPT_REWARD;

    DONATE_ADD_BINDMONEY_NTF sendMsg;
    sendMsg.money_point = donateTemp.money_point;
    m_player->SendToCenter(MSG_DONATE_ADD_BINDMONEY_NTF, &sendMsg);

    sendDanateInfoAck();
}

uint32 DonateManager::getTimer()
{
    int32 timer = 0;

    const TASK_FESTIVAL_TEMPLATE* festivalTmpl = TaskFestiveConfig::Instance()->GetFestivalTmpl(m_player->GetCenterSvrID(), FESTIVAL_DONATE_ID);
    if(!festivalTmpl)
        return 0;

    uint32 cur_time = (uint32)Crown::SDNow().GetTimeValue();

    if (festivalTmpl->begin_time <= cur_time && cur_time < festivalTmpl->end_time)
    {
        return festivalTmpl->end_time - cur_time;
    }

    return 0;
}


