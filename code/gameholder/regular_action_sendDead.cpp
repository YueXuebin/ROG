#include "gameholder_pch.h"
#include "regular_action.h"
#include "regular_action_sendDead.h"
#include "scene.h"
#include "regular.h"
#include "player.h"
#include "formula_manager.h"

RegularAction_SendDead::RegularAction_SendDead(Regular* pRegular) :
RegularAction(pRegular)
{
    m_CountDown         = 0;
    m_DeadAction        = 0;
    m_resurgenceCount   = 0;
}

RegularAction_SendDead::~RegularAction_SendDead()
{

}


void RegularAction_SendDead::ParseActionXml(TiXmlElement* ActionElement)
{
    ActionElement->QueryIntAttribute("count_down", &m_CountDown);
    m_CountDown = FormulaManager::Instance()->CalcRevivalTime(m_CountDown);
    ActionElement->QueryIntAttribute("dead_action", &m_DeadAction);
    ActionElement->QueryIntAttribute("resurgence_count", &m_resurgenceCount);
}

void RegularAction_SendDead::Do()
{
    Scene::PlayerMap playerMap = m_pRegular->GetScene()->GetPlayers();
    Scene::PlayerMap::iterator it = playerMap.find(GetTargetID());
    if(it == playerMap.end())
    {
        return;
    }

    Player* pPlayer = it->second;

    if(!pPlayer)
    {
        return;
    }

    if(pPlayer->IsDead())
    {
        REGULAR_PLAYER_DEAD sendMsg;
        sendMsg.regular_type = m_pRegular->GetType();
        sendMsg.dead_action = m_DeadAction;
        sendMsg.count_down = m_CountDown;
        pPlayer->SendToGate(MSG_PLAYER_DEAD, &sendMsg);
    }
}