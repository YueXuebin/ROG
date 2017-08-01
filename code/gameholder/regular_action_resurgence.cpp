#include "gameholder_pch.h"
#include "regular_action_resurgence.h"
#include "battle_unit.h"
#include "regular_action.h"
#include "regular.h"
#include "scene.h"
#include "player.h"
#include "state_define.h"
#include "item_manager.h"
#include "trigger.h"
#include "formula_manager.h"

RegularAction_Resurgence::RegularAction_Resurgence(Regular* pRegular) :
RegularAction(pRegular)
{
    m_resurgenceType    = 0;
    m_triggerID         = 0;
    m_delayTime         = 0;
    m_resurgenceCount   = -1;
    m_useGroup          = FALSE;
}

RegularAction_Resurgence::~RegularAction_Resurgence()
{

}

void RegularAction_Resurgence::ParseActionXml(TiXmlElement* ActionElement)
{
    ActionElement->QueryIntAttribute("resurgence_type", &m_resurgenceType);
    ActionElement->QueryIntAttribute("trigger_id", &m_triggerID);
    ActionElement->QueryIntAttribute("delay_time", &m_delayTime);
    m_delayTime = FormulaManager::Instance()->CalcRevivalTime(m_delayTime);
    m_delayTime *= 1000;
    ActionElement->QueryIntAttribute("resurgence_count", &m_resurgenceCount);
    int32 use_gruop = 0;
    ActionElement->QueryIntAttribute("use_group", &use_gruop);
    m_useGroup = (use_gruop == 1) ? TRUE : FALSE;
}

void RegularAction_Resurgence::Do()
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

        SEND_RESURGENCE sendMsg;
        sendMsg.resurgence_count = 0;
        sendMsg.delay_time = 0;
        pPlayer->SendToGate(MSG_SEND_RESURGENCE, &sendMsg);
        return;
    }

    switch(m_resurgenceType)
    {
    case RESURGENCE_TYPE_DIRECT:
        DirectResurgence(battleIt);
        break;
    case RESURGENCE_TYPE_ITEM:
        ItemResurgence(battleIt);
        break;
    default:
        CnError("RegularAction_Resurgence resurgence type: %d is not exist\n", m_resurgenceType);
        break;
    }
}

void RegularAction_Resurgence::DirectResurgence(BattleUnit* battleUnit)
{
    if(!battleUnit)
    {
        return;
    }

    Resurgence(battleUnit);
}

void RegularAction_Resurgence::ItemResurgence(BattleUnit* battleUnit)
{
    if(!battleUnit)
    {
        return;
    }

    if(battleUnit->GetObjectType() != OBJ_PLAYER)
    {
        return;
    }

    Player* pPlayer = (Player*)battleUnit;

    uint16 err = pPlayer->SubItemOne(REWARD_REASON_RESURGENCE, PROP_ITEM, 1, RESURGENCE_ITEM);
    if(err != ERR_SUCCEED)
    {
        pPlayer->NotifyErrorCode(err);
        return;
    }

    Resurgence(pPlayer);
}

void RegularAction_Resurgence::Resurgence(BattleUnit* battleUnit)
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

    m_pRegular->Resurgence(battleUnit, tirggerId, m_delayTime);       // 复活

    if(m_resurgenceCount >= 0)
    {
        m_resurgenceCount--;
    }

    if(battleUnit->GetObjectType() != OBJ_PLAYER)
    {
        return;
    }
    Player* pPlayer = (Player*)battleUnit;

    SEND_RESURGENCE sendMsg;
    sendMsg.resurgence_count = m_resurgenceCount;
    sendMsg.delay_time = m_delayTime / 1000;
    pPlayer->SendToGate(MSG_SEND_RESURGENCE, &sendMsg);
}
