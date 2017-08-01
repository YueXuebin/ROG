#include "gameholder_pch.h"
#include "game_define.h"
#include "gameholder.h"
#include "event_define.h"
#include "ai_controller.h"
#include "ai_define.h"
#include "battle_unit.h"
#include "scene.h"
#include "skill_manager.h"
#include "ai_manager.h"
#include "ai_state.h"
#include "player.h"
#include "performance_analysis.h"
#include "player_manager.h"
#include "ai_group_register.h"
#include "ai_group.h"
#include "regular.h"
#include "soldier.h"
#include "skill_info_manager.h"


bool    g_AIStop = false;       // 停止所有AI(调试用)

AIController::AIController(BattleUnit* owner) :
AIContainer(AICONTAINER_TYPE_CONTROLLER),
m_Owner(owner),
m_AIGroup(NULL)
{
    m_AIUpdateTimer = 0;
    m_lastAIStateId = m_AIStateId = 0;
    m_pAIState = NULL;

    m_SelectSkillId = 0;
    m_PathLoop = false;
    m_TargetUnitId = 0;
    m_CurrentPathIndex = 0;

    m_BaseX = m_BaseY = 0;
}

AIController::~AIController()
{
    if(m_AIGroup)
    {
        AddToAIGroup(0);
    }
}

void AIController::Update(uint32 delta_time)
{
    PERFORM_HELPER;

    if(g_AIStop)
    {
        return;
    }

    if (m_Owner->IsDead())
    {
        return;
    }

    m_AIUpdateTimer += delta_time;
    if(m_AIUpdateTimer < AI_UPDATE_TIME)
        return;
    m_AIUpdateTimer = 0;

    PERFORM_NAME_BEGIN("AIState");
    AIContainer::Update(delta_time);
    PERFORM_NAME_END("AIState");
}

bool AIController::LoadAI(const std::string& aiName)
{
    return AIContainer::LoadAI(aiName);
}

void AIController::ClearAI()
{
    AIContainer::ClearAI();
    m_UsedAIName.clear();
}

void AIController::RecordAIName(const std::string& aiName)
{
    for(uint32 i=0; i<m_UsedAIName.size(); i++)
    {
        if(aiName == m_UsedAIName[i])
        {
            CnAssert(false);
            CnFatal("%s include self!", aiName.c_str());
        }
    }
}

void AIController::SetBasePosition( int32 x, int32 y )
{
    m_BaseX = x;
    m_BaseY = y;
}

void AIController::SetPath( const PathData& pathData )
{
    m_PathData = pathData;
}

void AIController::SetPathLoop( bool isLoop )
{
    m_PathLoop = isLoop;
}

bool AIController::IsPathingEnd()
{
    if(m_CurrentPathIndex < 0 || m_CurrentPathIndex >= MAX_PATH_PIN)
        return true;
    return m_PathData.CheckPointList[m_CurrentPathIndex] == 0;
}

void AIController::NextPathIndex()
{
    m_CurrentPathIndex++;
    if(m_PathLoop && !GetCurrentCheckPoint())
    {
        m_CurrentPathIndex = 0;
    }
}

Checkpoint* AIController::GetCurrentCheckPoint()
{
    if(m_CurrentPathIndex < 0 || m_CurrentPathIndex >= MAX_PATH_PIN)
        return NULL;

    int32 CheckPointId = m_PathData.CheckPointList[m_CurrentPathIndex];
    if(CheckPointId <= 0)
        return NULL;

    Checkpoint* pCP = m_Owner->GetScene()->GetCheckPoint(CheckPointId);
    if(!pCP)
        return NULL;

    return pCP;
}

uint32 AIController::GetSelectSkill(bool bTrue)
{
    if (m_SelectSkillId>0 && bTrue && m_Owner->IsKindOf(&Soldier::ms_RTTI))
    {
        uint32 nSlot = m_SelectSkillId;
        Soldier* pSoldier = CnDynamicCast(Soldier, m_Owner);

        if (2 == nSlot)
        {
            return pSoldier->DefaultSkillId();
        }

        SkillInfo* pSkillInfo = pSoldier->GetSkillInfoMgr()->GetSkillInfoBySlot(nSlot);
        if (!pSkillInfo)
        {
            CnAssert(false);
            return 0;
        }

        return pSkillInfo->GetSkillId();
    }

    return m_SelectSkillId;
}

uint32 AIController::GetTargetUnitId()
{
    return m_TargetUnitId;
}

void AIController::SetTargetUnitId( uint32 targetId )
{
    m_TargetUnitId = targetId;

    // 如果AI群没有目标，则把目标设置给AI群
    if(m_AIGroup && targetId && m_AIGroup->GetTargetUnitId() == 0)
    {
        m_AIGroup->SetTargetUnitId(targetId);
    }
}

void AIController::ReceiveEvent( uint32 eventId, int32 arg1, int32 arg2 )
{
    if(!m_pAIState) return;

    switch(eventId)
    {
    case BATTLE_EVENT_ONHIT:   // 被攻击
        {
            BattleUnit* pAttacker = m_Owner->GetScene()->GetBattleUnit(arg1);
            if(pAttacker && m_Owner->IsEnemy(pAttacker))
            {
                m_pAIState->onHit(pAttacker);
            }
        }
        break;
    }
}

void AIController::MasterOnHit( uint32 objectId )
{
    if (!m_pAIState) return;

    BattleUnit* pAttacker = m_Owner->GetScene()->GetBattleUnit(objectId);
    if (pAttacker && m_Owner->IsEnemy(pAttacker))
    {
        m_pAIState->onMasterHit(pAttacker);
    }
}

void AIController::MasterAttack( uint32 objectId )
{
    if (!m_pAIState) return;

    BattleUnit* pDefender = m_Owner->GetScene()->GetBattleUnit(objectId);
    if (pDefender && m_Owner->IsEnemy(pDefender))
    {
        m_pAIState->onMasterAttack(pDefender);
    }
}

void AIController::AddToAIGroup(uint32 AIGroupId)
{
    if(m_AIGroup)
    {
        m_AIGroup->RemoveAIController(this);
        m_AIGroup = NULL;
    }

    if(AIGroupId == 0)
        return;

    AIGroupRegister* pAIGroupRegister = m_Owner->GetScene()->GetRegular()->GetAIGroupRegister();

    AIGroup* pAIGroup = pAIGroupRegister->GetAIGroupWithCreate(AIGroupId);

    pAIGroup->AddAIController(this);
    m_AIGroup = pAIGroup;

}

//void AIController::RegisterGroup(AIGroup* pAIGroup)
//{
//    if(m_AIGroup)
//    {
//        UnRegisterGroup();          // 从原AI组中移除
//    }
//
//    pAIGroup->AddAIController(this);
//    m_AIGroup = pAIGroup;
//}
//
//void AIController::UnRegisterGroup()
//{
//    if(!m_AIGroup)
//    {
//        m_AIGroup->RemoveAIController(this);
//        m_AIGroup = NULL;
//    }
//}