#include "centerserver_pch.h"
#include "proclamation_manager.h"
#include "global_setting_job.h"
#include "game_handler.h"

IMPLEMENT_SINGLETON(ProclamationManager)

ProclamationManager::ProclamationManager(void)
{
}

ProclamationManager::~ProclamationManager(void)
{
}

void ProclamationManager::OnRecv( uint64 player, uint32 sessionID, uint32 msgID, void* data )
{
    switch(msgID)
    {
        case MSG_GLOBAL_SETTING_PROCLAMATION_REQ:
            SendProclamationList(GameHandler::Instance()->GetRegionIDBySessionID(sessionID));
            break;
        default:
            CnAssert(false);
            break;
    }
}


void ProclamationManager::ReloadProclamationList()
{
    GlobalSettingJob::Instance()->LoadProclamationInfo();
    SendProclamationList();
}


void ProclamationManager::Init()
{
    GlobalSettingJob::Instance()->LoadProclamationInfo();
}

void ProclamationManager::SendProclamationList(uint32 gameserverID)
{
    GLOBAL_PROCLAMATION_BAG sendMsg;
    sendMsg.Clear();
    sendMsg = m_proclamationList;

    if(gameserverID == 0)
    {
        GameHandler::Instance()->SendToAll(MSG_GLOBAL_SETTING_PROCLAMATION_ACK, &sendMsg);
    }
    else
    {
        GameHandler::Instance()->SendByID(gameserverID, MSG_GLOBAL_SETTING_PROCLAMATION_ACK, &sendMsg);
    }
}