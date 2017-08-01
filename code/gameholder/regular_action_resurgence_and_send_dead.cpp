#include "gameholder_pch.h"
#include "regular_action_resurgence_and_send_dead.h"
#include "regular.h"
#include "scene.h"
#include "player.h"
#include "scene.h"
#include "trigger.h"
#include "formula_manager.h"

RegularActionResurgenceAndSendDead::RegularActionResurgenceAndSendDead(Regular* pRegular) :
RegularAction(pRegular)
{
    m_triggerID         = 0;
    m_delayTime         = 0;
    m_resurgenceCount   = -1;
    m_useGroup          = FALSE;
}

RegularActionResurgenceAndSendDead::~RegularActionResurgenceAndSendDead()
{

}

void RegularActionResurgenceAndSendDead::ParseActionXml(TiXmlElement* ActionElement)
{
    ActionElement->QueryIntAttribute("trigger_id", &m_triggerID);
    ActionElement->QueryIntAttribute("delay_time", &m_delayTime);
    m_delayTime = FormulaManager::Instance()->CalcRevivalTime(m_delayTime);
    m_delayTime *= 1000;
    ActionElement->QueryIntAttribute("resurgence_count", &m_resurgenceCount);
    int32 use_group = 0;
    ActionElement->QueryIntAttribute("use_group", &use_group);
    m_useGroup = (use_group == 1) ? TRUE : FALSE;
}

void RegularActionResurgenceAndSendDead::Do()
{
    BattleUnit* battleIt = m_pRegular->GetScene()->GetBattleUnit(GetTargetID());
    if(battleIt == NULL)
    {
        return;
    }

    if(!battleIt->IsDead())
    {
        CnWarn("%s:player is no dead\n", __FUNCTION__);
        return;
    }

    if(m_resurgenceCount == 0)
    {
        if(battleIt->GetObjectType() != OBJ_PLAYER)
        {
            return;
        }
        Player* pPlayer = (Player*)battleIt;

        RESURGENCE_AND_SEND_DEAD sendMsg;
        sendMsg.trigger_id = 0;
        sendMsg.resurgence_count = 0;
        sendMsg.delay_time = 0;
        pPlayer->SendToGate(MSG_RESURGENCE_AND_SEND_DEAD, &sendMsg);
        return;
    }

    Resurgence(battleIt);
}

void RegularActionResurgenceAndSendDead::Resurgence(BattleUnit* battleUnit)
{
    if(!battleUnit)
    {
        return;
    }

    int32 tirggerId = 0;
    // 判断trigger是否存在
    if(m_triggerID != 0)
    {
        if(!m_pRegular->GetScene()->GetTriggerByID(m_triggerID))
        {
            CnError("triggerID: %d is not exist\n", m_triggerID);
            tirggerId = 0;
        }
        else
        {
            tirggerId = m_triggerID;
        }
    }

    // 根据队伍ID复活到对应的triggerID
    if(m_useGroup)
    {
        tirggerId = m_pRegular->GetReliveTriggerID(battleUnit->GetGroup());

        if(!m_pRegular->GetScene()->GetTriggerByID(tirggerId))
        {
            CnError("GetReliveTriggerID: %d is not exist\n", m_triggerID);
            tirggerId = 0;
        }
    }

    m_pRegular->Resurgence(battleUnit, tirggerId, m_delayTime);

    if(m_resurgenceCount >= 0)
    {
        m_resurgenceCount--;
    }

    if(battleUnit->GetObjectType() != OBJ_PLAYER)
    {
        return;
    }
    Player* pPlayer = (Player*)battleUnit;

    RESURGENCE_AND_SEND_DEAD sendMsg;
    sendMsg.trigger_id = tirggerId;
    sendMsg.resurgence_count = m_resurgenceCount;
    sendMsg.delay_time = m_delayTime / 1000;
    pPlayer->SendToGate(MSG_RESURGENCE_AND_SEND_DEAD, &sendMsg);
}