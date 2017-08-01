#include "gameholder_pch.h"
#include "ai_factory.h"
#include "ai_controller.h"
#include "battle_unit.h"
#include "npc.h"
#include "pet.h"
#include "skill_manager.h"
#include "ai_manager.h"
#include "ai_state.h"
#include "ai_action_base.h"
#include "ai_action_move_random.h"
#include "ai_action_moveto.h"
#include "ai_action_pathing.h"
#include "ai_action_seek_object.h"
#include "ai_action_select_skill.h"
#include "ai_action_skill_to.h"
#include "ai_action_teleport.h"
#include "ai_condition_base.h"
#include "ai_condition_onhit.h"
#include "ai_condition_satisfy.h"
#include "ai_condition_timer.h"
#include "ai_condition_enemy_in_range.h"
#include "ai_condition_in_skill_range.h"
#include "ai_condition_onstate.h"
#include "ai_condition_lost_target.h"
#include "ai_condition_enemy_exist.h"
#include "ai_condition_finish_path.h"
#include "ai_condition_cd.h"
#include "ai_condition_master_onhit.h"
#include "ai_condition_master_attack.h"
#include "ai_condition_master_in_range.h"
#include "ai_condition_lack_hp.h"
#include "ai_condition_lack_mp.h"
#include "ai_condition_call_pet_count.h"
#include "ai_action_flee.h"
#include "ai_action_stop.h"
#include "ai_action_container.h"
#include "ai_condition_pet_life.h"
#include "ai_condition_find_enemy.h"

AIFactory::AIFactory()
{

}

AIFactory::~AIFactory()
{

}

void AIFactory::CreateDefaultAI(AIController* controller)
{
    LoadAIStates("ai/sample1.aix", controller);
}

bool AIFactory::LoadAIStates(const std::string& aiName, AIContainer* container)
{
    CnAssert(container);
    CnAssert(container->m_AllStateMap.size() == 0);

    TiXmlDocument* pAIDoc = AIManager::Instance()->GetAIDoc(aiName);
    if(!pAIDoc) return false;

    TiXmlElement* pRoot = pAIDoc->RootElement();
    CnAssert(pRoot);

    uint32 stateId(0);
    AIState* pAIState(NULL);
    for(TiXmlElement* stateEle = pRoot->FirstChildElement("state"); stateEle; stateEle = stateEle->NextSiblingElement("state"))
    {   // 解析各状态
        stateId = 0;
        stateEle->QueryUnsignedAttribute("id", &stateId);

        // 状态是否存在
        if( container->m_AllStateMap.find(stateId) != container->m_AllStateMap.end() )
        {
            CnAssert(false);
            CnFatal("state id %d already exist\n", stateId);
            continue;
        }
        // 创建AI状态实例
        pAIState = CnNew AIState(stateId, container);
        CnAssert(pAIState);

        // 设置Controller
        if(container->GetClassType() == AICONTAINER_TYPE_CONTROLLER)
        {
            pAIState->m_pController = (AIController*)container;
        }
        else if(container->GetClassType() == AICONTAINER_TYPE_ACTION)
        {
            pAIState->m_pController = ((AIActionContainer*)container)->GetController();
        }
        else
            CnAssert(false);

        // 解析AI状态数据
        ParseState(pAIState, stateEle);

        // 把所有AI状态映射到AI控制器表中
        container->m_AllStateMap[stateId] = pAIState;

        // 0状态为开始状态
        if(stateId == 0)
        {
            container->m_pAIState = pAIState;
        }
    }

    CnAssert(container->m_pAIState);        // 没有0状态

    return true;
}

void AIFactory::ParseState( AIState* pAIState, TiXmlElement* stateXml)
{
    CnAssert(pAIState);
    CnAssert(stateXml);

    // 解析状态变迁条件
    for(TiXmlElement* conditionEle = stateXml->FirstChildElement("condition"); conditionEle; conditionEle = conditionEle->NextSiblingElement("condition"))
    {
        uint32 conditionId = 0;
        conditionEle->QueryUnsignedAttribute("id", &conditionId);
        if(conditionId == 0)
        {
            CnFatal("condition id %d not exist", conditionId);
            continue;
        }

        AIConditionBase* pCondition = NULL;
        switch(conditionId)
        {
        case AI_CONDITION_SATISFY: // 永远满足
            {
                pCondition = CnNew AIConditionSatisfy;
            }
            break;
        case AI_CONDITION_TIMER: // 计时器
            {
                pCondition = CnNew AIConditionTimer;
            }
            break;
        case AI_CONDITION_ON_HIT: // 被攻击
            {
                pCondition = CnNew AIConditionOnHit;
            }
            break;
        case AI_CONDITION_ENEMY_IN_RANGE:
            {
                pCondition = CnNew AIConditionEnemyInRange;
            }
            break;
        case AI_CONDITION_IN_SKILL_RANGE:
            {
                pCondition = CnNew AIConditionInSkillRange;
            }
            break;
        case AI_CONDITION_ON_STATE:
            {
                pCondition = CnNew AIConditionOnState;
            }
            break;
        case AI_CONDITION_LOST_TARGET:
            {
                pCondition = CnNew AIConditionLostTarget;
            }
            break;
        case AI_CONDITION_FINISH_PATH:
            {
                pCondition = CnNew AIConditionFinishPath;
            }
            break;
        case AI_CONDITION_ENEMY_EXIST:
            {
                pCondition = CnNew AIConditionEnemyExist;
            }
            break;
        case AI_CONDITION_SKILL_IN_CD:
            {
                pCondition = CnNew AIConditionCD;
            }
            break;
        case AI_CONDITION_MASTER_ON_HIT:
            {
                pCondition = CnNew AIConditionMasterOnhit;
            }
            break;
        case AI_CONDITION_MASTER_ATTACK:
            {
                pCondition = CnNew AIConditionMasterAttack;
            }
            break;
        case AI_CONDITION_MASTER_IN_RANGE:
            {
                pCondition = CnNew AIConditionMasterInRange;
            }
            break;
        case AI_CONDITION_LACK_HP:
            {
                pCondition = CnNew AIConditionLackHP;
            }
            break;
        case AI_CONDITION_LACK_MP:
            {
                pCondition = CnNew AIConditionLackMP;
            }
            break;
        case AI_CONDITION_SUMMON_PET_NUM:
            {
                pCondition = CnNew AIConditionCallPetCount;
            }
            break;
        case AI_CONDITION_BATTLE_PET_ALIVE:
            {
                pCondition = CnNew AIConditionPetLife;
            }
            break;
        case AI_CONDITION_FIND_ENEMY:
            {
                pCondition = CnNew AIConditionFindEnemy;
            }
            break;
        default:
            CnAssert(false);
            break;
        }
        
        if(pCondition)
        {
            CnAssert(conditionId == pCondition->GetID());
            pCondition->m_pController = pAIState->GetController();
            pCondition->m_pAIState = pAIState;
            pAIState->m_AllCondition.push_back(pCondition);
            ParsetCondition(pCondition, conditionEle);
        }
    }

    // 解析行为
    TiXmlElement* actionEle = stateXml->FirstChildElement("action");
    uint32 actionId = 0;
    actionEle->QueryUnsignedAttribute("id", &actionId);

    AIActionBase* pActionBase(NULL);

    switch(actionId)
    {
    case 0: // 无行为
        break;
    case AI_ACTION_MOVE_RANDOM:
        {
            pActionBase = CnNew AIActionMoveRandom;
        }
        break;
    case AI_ACTION_MOVE_TO:
        {
            pActionBase = CnNew AIActionMoveTo;
        }
        break;
    case AI_ACTION_PATHING:
        {
            pActionBase = CnNew AIActionPathing;
        }
        break;
    case AI_ACTION_SEEK_OBJECT:
        {
            pActionBase = CnNew AIActionSeekObject;
        }
        break;
    case AI_ACTION_CHOOSE_SKILL:
        {
            pActionBase = CnNew AIActionSelectSkill;
        }
        break;
    case AI_ACTION_SKILL_TO:
        {
            pActionBase = CnNew AIActionSkillTo;
        }
        break;
    case AI_ACTION_TELEPORT:
        {
            pActionBase = CnNew AIActionTeleport;
        }
        break;
    case AI_ACTION_FLEE:
        {
            pActionBase = CnNew AIActionFlee;
        }
        break;
    case AI_ACTION_STOP:
        {
            pActionBase = CnNew AIActionStop;
        }
        break;
    case AI_ACTION_SET_AI:
        {
            pActionBase = CnNew AIActionContainer;
        }
        break;
    default:
        CnAssert(false);
        break;
    }
    
    if(pActionBase)
    {
        CnAssert(!pAIState->m_pAction);
        CnAssert(actionId == pActionBase->GetID());
        pActionBase->m_pController = pAIState->GetController();
        pAIState->m_pAction = pActionBase;
        pActionBase->ParseXML(actionEle);
    }
}

void AIFactory::ParsetCondition(AIConditionBase* pCondition, TiXmlElement* conditionXml)
{
    uint32 nextStateId = 0;
    conditionXml->QueryUnsignedAttribute("stateid", &nextStateId);          //要跳转的下一状态
    pCondition->SetNextStateId(nextStateId);
    pCondition->ParseXML(conditionXml);
}
