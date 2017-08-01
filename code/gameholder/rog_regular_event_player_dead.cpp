#include "gameholder_pch.h"
#include "rog_regular_event_player_dead.h"
#include "event_define.h"
#include "regular_action.h"
#include "battle_unit.h"
#include "scene.h"
#include "player.h"
#include "regular_dungeon.h"

ROGRegularEvent_PlayerDead::ROGRegularEvent_PlayerDead(RegularDungeon* pRegular, int32 deadNumActive, bool startActive) :
ROGRegularEvent(pRegular, ROG_REGULAR_EVENT_PLAYER_DEAD)
{
    m_active = m_startActive = startActive;
    // 关卡总命数
    m_param1 = pRegular->GetLifeCount();
    CnAssert(m_param1 >= 0);

    // 触发命数
    if(deadNumActive < 0)
        m_param2 = pRegular->GetLifeCount();
    else
        m_param2 = deadNumActive;

    // 已经用掉的命数
    m_param3 = 0;
}

ROGRegularEvent_PlayerDead::~ROGRegularEvent_PlayerDead()
{

}

void ROGRegularEvent_PlayerDead::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    ROGRegularEvent::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    switch(eventId)
    {
    case BATTLE_EVENT_PLAYER_DEAD:
        {
            if(GetActive() == m_startActive)
            {
                m_param3++;
                GetRegular()->SetConditionChanged();
                // 判断是否达成
                if(GetAlreadyDeadCount() > GetActiveDeadCount())
                {
                    SetActive(!m_startActive);
                }
            }
        }
        break;
    }
}
