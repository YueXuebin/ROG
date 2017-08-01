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
// 设置活动数据
// ==================================================
void FestivalManager::SetFestivalList(const TASK_FESTIVAL_TEMPLATE_LIST& list)
{
    m_festivalList = list;
}

// ==================================================
// 获得活动模板
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
// 通知所有Gate活动信息
// ==================================================
void FestivalManager::SendToAllGateSvrFestivalList()
{
    GateHandler::Instance()->SendToAll(MSG_TASK_FESTIVAL_TMPL_LIST_ACK, &m_festivalList, BROADCAST_PLAYER_ID);
}

// ==================================================
// 通知所有Game活动信息
// ==================================================
void FestivalManager::SendToAllGameSvrFestivalList()
{
    GameHandler::Instance()->SendToAll(MSG_TASK_FESTIVAL_TMPL_LIST_ACK, &m_festivalList);
}

// ==================================================
// 通知游戏服务器所有活动数据
// ==================================================
void FestivalManager::SendFestivalListToGameSvr(uint32 sessionID)
{
    uint32 regionID = GameHandler::Instance()->GetRegionIDBySessionID(sessionID);

    GameHandler::Instance()->SendByID(regionID, MSG_TASK_FESTIVAL_TMPL_LIST_ACK, &m_festivalList);
}
