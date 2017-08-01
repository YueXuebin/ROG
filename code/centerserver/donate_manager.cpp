#include "centerserver_pch.h"
#include "donate_manager.h"
#include "gate_handler.h"
#include "special_activity_job.h"
#include "player.h"
#include "player_manager.h"
#include "festival_manager.h"


IMPLEMENT_SINGLETON(DonateManager)

DonateManager::DonateManager()
{
    m_donate_info.Clear();
    m_configInfo.Clear();
    m_minute = 0;
    m_do_once = false;
}

DonateManager::~DonateManager()
{
}

void DonateManager::Init()
{
    SpecialActivityJob::Instance()->GetDonateAllInfo(m_donate_info);
}

void DonateManager::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_DONATE_INFO_REQ:
        SendDonateInfo(playerID);
        break;
    case MSG_DONATE_ADD_BINDMONEY_NTF:
        OnAddBindmoneyNtf(data);
        break;
    default:
        break;
    }
}

void DonateManager::Update()
{
    m_minute += g_ServerUpdateTime;
    if(m_minute < 60000)
        return;

    m_minute = 0;

    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
    const TASK_FESTIVAL_TEMPLATE* taskTmpl = FestivalManager::Instance()->GetFestivalTemplate(FESTIVAL_DONATE_ID);
    if(!taskTmpl)
        return;

    if(taskTmpl->begin_time <= nowTime && nowTime <= taskTmpl->end_time)
    {
        if(!m_do_once && (nowTime - taskTmpl->begin_time) > m_configInfo.after_minute * 60)
        {
            m_donate_info.money_point += m_configInfo.add_value;
            m_do_once = true;
        }
    }

    if(nowTime > taskTmpl->end_time && m_donate_info.money_point > 0)
    {
        SendReward();
    }
}

void DonateManager::SetConfig(const std::string& configJson)
{
    GAME_SETTING_DONATE_CONFIG info;
    if(!JsonToStruct(info, configJson))
        return;

    m_configInfo = info;

    m_do_once = false;
}

void DonateManager::SaveDB()
{
    SpecialActivityJob::Instance()->SaveDonateAllInfo(m_donate_info);
}

void DonateManager::OnAddBindmoneyNtf(void* data)
{
    DONATE_ADD_BINDMONEY_NTF* recvMsg = (DONATE_ADD_BINDMONEY_NTF*)data;

    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();

    const TASK_FESTIVAL_TEMPLATE* taskTmpl = FestivalManager::Instance()->GetFestivalTemplate(FESTIVAL_DONATE_ID);
    if(!taskTmpl)
        return;

    if(nowTime < taskTmpl->begin_time || nowTime > taskTmpl->end_time)
        return;

    CnAssert(recvMsg->money_point > 0);
    m_donate_info.money_point += recvMsg->money_point;
    
    SendDonateInfo(BROADCAST_PLAYER_ID);
}

void DonateManager::SendDonateInfo(uint64 playerID)
{
    DONATE_ALL_BINDMONEY_NTF sendMsg;
    sendMsg.money_point = m_donate_info.money_point;
    GateHandler::Instance()->SendToAll(MSG_DONATE_ALL_BINDMONEY_NTF, &sendMsg, playerID);
}

void DonateManager::SendReward()
{
    std::map<uint64, Player*> playerMap = PlayerManager::Instance()->GetPlayerMap();
    int32 playerNum = playerMap.size();
    if(playerNum <= 0)
        return;

    int32 oneMoneypoint = m_donate_info.money_point / playerNum;

    DONATE_REWARD_MONEYPOINT sendMsg;
    sendMsg.money_point = oneMoneypoint;
    for(std::map<uint64, Player*>::iterator playerIt = playerMap.begin(); playerIt != playerMap.end(); ++playerIt)
    {
        playerIt->second->SendToGame(MSG_DONATE_REWARD_MONEYPOINT, &sendMsg);
    }

    m_donate_info.money_point = 0;
}

