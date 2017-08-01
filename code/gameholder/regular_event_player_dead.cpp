#include "gameholder_pch.h"
#include "regular_event_player_dead.h"
#include "event_define.h"
#include "regular_action.h"
#include "battle_unit.h"
#include "scene.h"
#include "player.h"


RegularEvent_PlayerDead::RegularEvent_PlayerDead(Regular* pRegular) :
RegularEvent(pRegular, REGULAR_EVENT_PLAYER_DEAD)
{

}

RegularEvent_PlayerDead::~RegularEvent_PlayerDead()
{

}

void RegularEvent_PlayerDead::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, const std::string& arg3 )
{
    switch(eventId)
    {
    case BATTLE_EVENT_PLAYER_DEAD:
        RegularActionList::iterator actionIt = m_ActionList.begin();
        for(; actionIt != m_ActionList.end(); actionIt++)
        {
            (*actionIt)->SetTargetID(arg1);
        }
        SetActive(true);
        break;
    }
}

void RegularEvent_PlayerDead::OneSecondUpdate(int32 deltatime)
{
    if(!GetActive())
    {
        bool isAnyOneDead = false;
        Scene::PlayerMap playerList = GetScene()->GetPlayers();
        for(Scene::PlayerMap::iterator playerIt = playerList.begin(); playerIt != playerList.end(); playerIt++)
        {
            Player* player = playerIt->second;
            CnAssert(player);
            
            if(player->IsDead())
            {
                isAnyOneDead = true;
                break;
            }
        }

        if(isAnyOneDead)
            SetActive(true);
    }
}
