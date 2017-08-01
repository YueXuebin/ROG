#include "gameholder_pch.h"
#include "ai_condition_find_enemy.h"
#include "ai_controller.h"
#include "battle_unit.h" 
#include "scene.h"
#include "game_util.h"
#include "creature.h"
#include "ai_group_register.h"
#include "ai_group.h"
#include "regular.h"

AIConditionFindEnemy::AIConditionFindEnemy() :
AIConditionBase(AI_CONDITION_FIND_ENEMY)
{
    m_Range = 0;
    m_group = ALL_GROUP;
    m_creatureList.clear();
}

AIConditionFindEnemy::~AIConditionFindEnemy()
{

}

void AIConditionFindEnemy::ParseXML( TiXmlElement* pDoc )
{
    AIConditionBase::ParseXML(pDoc);

    pDoc->QueryIntAttribute("range", &m_Range);
    pDoc->QueryIntAttribute("group", &m_group);

    std::string creatureListStr = "";
    creatureListStr = pDoc->Attribute("creature_list");

    std::vector<std::string> strList = Crown::SDSplitStrings(creatureListStr, ' ');
    for(std::vector<std::string>::iterator strIt = strList.begin(); strIt != strList.end(); ++strIt)
    {
        m_creatureList.push_back(Crown::SDAtou(strIt->c_str()));
    }
}

bool AIConditionFindEnemy::IsSatisfy()
{
    BattleUnit* pOwner = m_pController->GetOwner();

    BattleUnit* pTarget = NULL;

    // 搜索范围内的敌人
    Scene::BattleUnitMap uintsMap;
    pOwner->GetScene()->FindBattleUnitInCircle(pOwner->GetPosition(), m_Range + pOwner->GetRadius(), uintsMap, pOwner->GetId());

    // 指定creatureID的敌对生物
    Scene::BattleUnitMap targetMap;
    for(Scene::BattleUnitMap::iterator unitIt = uintsMap.begin(); unitIt != uintsMap.end(); ++unitIt)
    {
        if(unitIt->first == pOwner->GetId())
            continue;

        if(!unitIt->second->IsKindOf(&Creature::ms_RTTI))
            continue;

        for(std::list<uint32>::iterator creatureIt = m_creatureList.begin(); creatureIt != m_creatureList.end(); ++creatureIt)
        {
            Creature* pCreature = (Creature*)unitIt->second;
            if(pCreature->GetCreatureId() == *creatureIt && pOwner->IsEnemy(pCreature) && (pCreature->GetGroup() == m_group || m_group == ALL_GROUP))
                targetMap[unitIt->first] = unitIt->second;
        }
    }

    real minDistance = 9999999.f;
    for(Scene::BattleUnitMap::iterator targetIt = targetMap.begin(); targetIt != targetMap.end(); ++targetIt)
    {
        real dis = DistanceBySceneObject(pOwner, targetIt->second);
        if(dis < minDistance)
        {
            minDistance = dis;
            pTarget = targetIt->second;
        }
    }

    if (!pTarget)
    {
        // 设置群目标
        if(m_pController->GetAIGroup() && m_pController->GetAIGroup()->GetTargetUnitId())
        {
            m_pController->SetTargetUnitId( m_pController->GetAIGroup()->GetTargetUnitId() );
            return true;
        }
    }

    if(!pTarget)
        return false;

    m_pController->SetTargetUnitId(pTarget->GetId());

    return true;
}



