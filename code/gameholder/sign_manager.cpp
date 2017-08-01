// 签到系统
#include "gameholder_pch.h"
#include "sign_manager.h"
#include "bouns_config_manager.h"
#include "player.h"
#include "center_handler.h"
#include "scene.h"
#include "trade_manager.h"
#include "event_define.h"
#include "gameplaytimes_notifier.h"
#include "game_util.h"
#include "game_setting_manager.h"

SignManager::SignManager(Player* player) :
m_player(player)
{
    Clear();
}

SignManager::~SignManager()
{
}

void SignManager::Clear()
{
    m_sign_info.Clear();
}

void SignManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_SIGN_INFO_REQ:
        {
            OnSignInfoReq();
        }
        break;
    case MSG_SIGN_REQ:
        {
            OnSignReq();
        }
        break;

    default:
        break;
    }
}

void SignManager::LoadInfo(SIGN_INFO& info)
{
    m_sign_info = info;
}

void SignManager::CheckSignData()
{
    Crown::CSDDateTime now = Crown::SDNow();

    // 开服时间
    uint32 server_open_time = GameSettingManager::Instance()->GetServerOpenToDayTime();
    //server_open_tim	= 
    uint32 intTimespan =0;
    if(now.GetTimeValue()>server_open_time)
    {
        intTimespan = (uint32)now.GetTimeValue()-server_open_time;
    }

    //uint32 newIndex = intTimespan/(60*60*24*30);
    //test 1 day refresh
    uint32 newIndex = intTimespan/CALENDAR_Refresh;
    m_sign_info.sign_sheet_refresh_time = server_open_time+(newIndex+1)*CALENDAR_Refresh;
    uint32 signGainsSheetLength =BounsConfigManager::Instance()->GetSignGainsMonthLength();
    newIndex=newIndex%signGainsSheetLength;
    //new month
    if (newIndex!=m_sign_info.sign_gain_sheet_idx)
    {
        m_sign_info.sign_gain_sheet_idx =newIndex;
        m_sign_info.days = 0;  //clear
    }

}

void SignManager::FillSignInfo(SIGN_INFO& info)
{
    info = m_sign_info;
}

void SignManager::OnSignInfoReq()
{
    SIGN_INFO sendMsg;
    Crown::CSDDateTime currTime = Crown::SDNow();
    Crown::CSDDateTime lastSignTime = Crown::CSDDateTime(m_sign_info.last_sign_time);
    CheckSignData();

    sendMsg = m_sign_info;

    // 这个字段客户端用来判断当前服务器的时间
    //sendMsg.last_sign_time = (uint32)currTime.GetTimeValue();

 
    bool bSignToday =std::abs((long)currTime.DiffSecond(lastSignTime)) > SIGN_Refresh ? false:true;

   

    if (bSignToday)
    {
        sendMsg.sign_refresh_time =std::abs( (long)((uint32) m_sign_info.last_sign_time + SIGN_Refresh - (uint32)currTime.GetTimeValue()));
    }
    else
    {
        sendMsg.sign_refresh_time = 0;
    }
    m_player->SendToGate(MSG_SIGN_INFO_ACK, &sendMsg);
}

void SignManager::OnSignReq()
{
    // get time
    //time_t t = Crown::SDNow().GetTimeValue();
    Crown::CSDDateTime currTime = Crown::SDNow();
    Crown::CSDDateTime lastSignTime = Crown::CSDDateTime(m_sign_info.last_sign_time);
    bool bSignToday = std::abs((long)currTime.DiffSecond(lastSignTime)) > SIGN_Refresh ? false:true;
    if (bSignToday)
        return;
	if (m_sign_info.days>= BounsConfigManager::Instance()->GetSignGainsDayLength(m_sign_info.sign_gain_sheet_idx))
		return;
    m_sign_info.last_sign_time = (uint32)currTime.GetTimeValue();   // sign timestamp
	m_sign_info.days++;
    //ack
    SIGN_ACK sendMsg;
    sendMsg.days = SIGN_Refresh;
	sendMsg.isOk =1;
    m_player->SendToGate(MSG_SIGN_ACK, &sendMsg);

    m_player->SendPlaySystemLog(PLAYSYSTEM_SIGN_SIGN, sendMsg.days);

	// add rewards
	REWARD_TABLE rewards;
	const SIGN_GAIN_ROOT_MONTH_DAY_STRUCT* signGainDay = BounsConfigManager::Instance()->getSignGainDayStruct(m_sign_info.sign_gain_sheet_idx,  m_sign_info.days);
	std::vector<REWARD_ITEM> inRewards;
	if (signGainDay)
	{
	 
		inRewards.clear();
		REWARD_ITEM tmpRewardItem;
		tmpRewardItem.id=signGainDay->itemId;
		tmpRewardItem.num=signGainDay->itemNum;
		if(signGainDay->vip && m_player->GetVipLevel()>=signGainDay->vip)
		{
			tmpRewardItem.num=tmpRewardItem.num*2;
		}
		inRewards.push_back(tmpRewardItem);
	    m_player->GetFcmRewardList(inRewards, rewards.items);

	}
	m_player->TakeInRewardTable(&rewards);

}





