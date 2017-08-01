#include "centerserver_pch.h"
#include "festival_manager.h"
#include "dbjob.h"
#include "gate_handler.h"
#include "game_handler.h"

IMPLEMENT_SINGLETON(FestivalManager)

FestivalManager::FestivalManager()
{

}

FestivalManager::~FestivalManager()
{

}

// ==================================================
// ���û����
// ==================================================
void FestivalManager::SetFestivalList(const TASK_FESTIVAL_TEMPLATE_LIST& list)
{
    m_festivalList = list;
}

// ==================================================
// ��ûģ��
// ==================================================
const TASK_FESTIVAL_TEMPLATE* FestivalManager::GetFestivalTemplate(uint32 id)
{
    for(std::vector<TASK_FESTIVAL_TEMPLATE>::const_iterator festivalIt = m_festivalList.list.begin();
            festivalIt != m_festivalList.list.end(); ++festivalIt)
    {
        if(festivalIt->festival_id == id)
        {
            return &(*festivalIt);
        }
    }

    return NULL;
}

// ==================================================
// ֪ͨ����Gate���Ϣ
// ==================================================
void FestivalManager::SendToAllGateSvrFestivalList()
{
    GateHandler::Instance()->SendToAll(MSG_TASK_FESTIVAL_TMPL_LIST_ACK, &m_festivalList, BROADCAST_PLAYER_ID);
}

// ==================================================
// ֪ͨ����Game���Ϣ
// ==================================================
void FestivalManager::SendToAllGameSvrFestivalList()
{
    GameHandler::Instance()->SendToAll(MSG_TASK_FESTIVAL_TMPL_LIST_ACK, &m_festivalList);
}

// ==================================================
// ֪ͨ��Ϸ���������л����
// ==================================================
void FestivalManager::SendFestivalListToGameSvr(uint32 sessionID)
{
    uint32 regionID = GameHandler::Instance()->GetRegionIDBySessionID(sessionID);

    GameHandler::Instance()->SendByID(regionID, MSG_TASK_FESTIVAL_TMPL_LIST_ACK, &m_festivalList);
}
