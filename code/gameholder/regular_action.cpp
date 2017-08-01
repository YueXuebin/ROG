#include "gameholder_pch.h"
#include "regular_action.h"
#include "regular_action_create_monster.h"
#include "regular_action_create_boss.h"
#include "regular_action_stop_summon.h"
#include "regular_action_clear_all_monsters.h"
#include "regular_action_resurgence.h"
#include "regular_action_sendDead.h"
#include "regular_action_resurgence_and_send_dead.h"
#include "regular_action_attackable.h"

RegularAction* ParseRegularAction( TiXmlElement* ActionElement, Regular* pRegular )
{
    uint32 actionId = 0;
    ActionElement->QueryUnsignedAttribute("id", &actionId);

    RegularAction* pRegularAction = NULL;

    switch(actionId)
    {
    case REGULAR_ACTION_CREATE_MONSTER:
        {
            pRegularAction = CnNew RegularAction_CreateMonster(pRegular);
        }
        break;
    case REGULAR_ACTION_CREATE_BOSS:
        {
            pRegularAction = CnNew RegularAction_CreateBoss(pRegular);
        }
        break;
    case REGULAR_ACTION_STOP_SUMMON:
        {
            pRegularAction = CnNew RegularAction_StopSummon(pRegular);
        }
        break;
    case REGULAR_ACTION_CLEAR_ALL_MONSTERS:
        {
            pRegularAction = CnNew RegularAction_ClearAllMosters(pRegular);
        }
        break;
    case REGULAR_ACTION_RESURGENCE:
        {
            pRegularAction = CnNew RegularAction_Resurgence(pRegular);
        }
        break;
    case REGULAR_ACTION_SENDDEAD:
        {
            pRegularAction = CnNew RegularAction_SendDead(pRegular);
        }
        break;
    case REGULAR_ACTION_RESURGENCE_AND_SEND_DEAD:
        {
            pRegularAction = CnNew RegularActionResurgenceAndSendDead(pRegular);
        }
        break;
    case REGULAR_ACTION_ATTACKABLE:
        {
            pRegularAction = CnNew RegularAction_Attackable(pRegular);
        }
        break;
    default:
        CnAssert(false);        // 未定义的关卡行为
        break;
    }

    if(pRegularAction)
    {
        // 解析行为参数
        pRegularAction->ParseActionXml(ActionElement);
    }

    return pRegularAction;
}


RegularAction::RegularAction( Regular* pRegular ) :
m_pRegular(pRegular)
{
    m_targetID = 0;
}

RegularAction::~RegularAction()
{

}

