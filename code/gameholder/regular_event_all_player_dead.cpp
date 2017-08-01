#include "gameholder_pch.h"
#include "regular_event_all_player_dead.h"
#include "event_define.h"
#include "regular_action.h"
#include "scene.h"
#include "battle_unit.h"
#include "player.h"

RegularEvent_AllPlayerDead::RegularEvent_AllPlayerDead(Regular* pRegular) :
RegularEvent(pRegular, REGULAR_EVENT_ALL_PLAYER_DEAD)
{

}

RegularEvent_AllPlayerDead::~RegularEvent_AllPlayerDead()
{

}

void RegularEvent_AllPlayerDead::OneSecondUpdate(int32 deltatime)
{
    Scene::PlayerMap playerList = GetScene()->GetPlayers();
    for(Scene::PlayerMap::iterator playerIt = playerList.begin(); playerIt != playerList.end(); playerIt++)
    {
        if(playerIt->second == NULL)
        {
            return;
        }

        // Íæ¼ÒÃ»ËÀ
        if(playerIt->second->IsDead() == false)
        {
            return;
        }
    }

    SetActive(true);
}
