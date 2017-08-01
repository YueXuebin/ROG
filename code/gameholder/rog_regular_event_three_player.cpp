#include "gameholder_pch.h"
#include "rog_regular_event_three_player.h"
#include "scene.h"
#include "regular_dungeon.h"



CnImplementRTTI(ROGRegularEvent_ThreePlayer, ROGRegularEvent);

ROGRegularEvent_ThreePlayer::ROGRegularEvent_ThreePlayer(RegularDungeon* pRegular) :
ROGRegularEvent(pRegular, ROG_REGULAR_EVENT_THREE_PLAYER)
{
    m_MaxPlayerNum = 0;
    m_NoOneLeave = true;
}

ROGRegularEvent_ThreePlayer::~ROGRegularEvent_ThreePlayer()
{

}

void ROGRegularEvent_ThreePlayer::OneSecondUpdate(int32 deltatime)
{
    ROGRegularEvent::OneSecondUpdate(deltatime);

    int32 playerNum = GetScene()->GetPlayerCount();
    if(playerNum > m_MaxPlayerNum)
        m_MaxPlayerNum = playerNum;

    if(playerNum < m_MaxPlayerNum)
        m_NoOneLeave = false;

    bool isActive = ((m_MaxPlayerNum>=3) && m_NoOneLeave);

    if(GetActive() != isActive)
    {
        GetRegular()->SetConditionChanged();
        SetActive(isActive);
    }
}
