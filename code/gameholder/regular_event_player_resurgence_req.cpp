#include "gameholder_pch.h"
#include "regular_event_player_resurgence_req.h"
#include "event_define.h"
#include "scene.h"
#include "regular.h"
#include "regular_action.h"
#include "player_manager.h"

RegularEvent_PlayerResurgence::RegularEvent_PlayerResurgence(Regular* pRegular) :
RegularEvent(pRegular, REGULAR_EVENT_PLAYER_RESURGENCE_REQ)
{

}

RegularEvent_PlayerResurgence::~RegularEvent_PlayerResurgence()
{

}

void RegularEvent_PlayerResurgence::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, const std::string& arg3)
{
    switch(eventId)
    {
    case EVENT_PLAYER_RESURGENCE_REQ:
        {
            // 设置复活目标
            RegularActionList::iterator actionIt = m_ActionList.begin();
            for(; actionIt != m_ActionList.end(); actionIt++)
            {
                (*actionIt)->SetTargetID(arg1);
            }

            SetActive(true);

            Scene* pScene = m_pRegular->GetScene();
            if(pScene)
            {
                Player* player = pScene->FindPlayer(arg1);
                if(player && (m_pRegular->GetType() == REGULAR_TYPE_TEAM))
                {
                    m_pRegular->SendDungeonEvent(player, TEAM_DUNGEON_EVNET_BUYRELIVE);
                }
            }
        }
        break;
    }
}
