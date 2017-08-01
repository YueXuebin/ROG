#include "gameholder_pch.h"
#include "ai_action_select_skill.h"
#include "ai_controller.h"
#include "battle_unit.h"
#include "boss.h"
#include "monster.h"
#include "pet.h"
#include "monster_config_manager.h"
#include "soldier.h"
#include "skill_info_manager.h"

AIActionSelectSkill::AIActionSelectSkill() :
AIActionBase(AI_ACTION_CHOOSE_SKILL)
{

}

AIActionSelectSkill::~AIActionSelectSkill()
{

}

void AIActionSelectSkill::ParseXML(TiXmlElement* pDoc)
{
    AIActionBase::ParseXML(pDoc);

    int32 skillId = 0;
    int32 skillPro = 0;
    int32 skillCount = 0;
    std::vector<uint32> skill_list;

    BattleUnit* pOwner = m_pController->GetOwner();

    // 查找生物ID
    uint32 creature_id = 0;
    if (pOwner->GetObjectType() == OBJ_BOSS)
    {
        creature_id = ((Boss*)pOwner)->GetCreatureId();
    }
    else if (pOwner->GetObjectType() == OBJ_MONSTER)
    {
        creature_id = ((Monster*)pOwner)->GetCreatureId();
        // 查找怪物对应的技能
        const MONSTER_ATTRIBUTE_REVISE_ROOT_MONSTER_STRUCT* monsterStruct = MonsterConfigManager::Instance()->GetMonsterAttributeReviseById(creature_id);
        skill_list.push_back(monsterStruct->skill_1);
        skill_list.push_back(monsterStruct->skill_2);
    }
    else if (pOwner->GetObjectType() == OBJ_PET)
    {
        creature_id = ((Pet*)pOwner)->GetCreatureId(); 

        skill_list.push_back(1);
        skill_list.push_back(2);
    }

    skillCount = skill_list.size();

    // 技能1
    skillId = skillPro = 0;
    pDoc->QueryIntAttribute("skill1_id", &skillId);
    pDoc->QueryIntAttribute("skill1_pro", &skillPro);

    if (skillId < 10 && skillId != 0)     // skillId<1000表达第几个技能
    {
        if (skillId <= skillCount)
        {
            skillId = skill_list[skillId - 1];
        }
        else
        {
            skillId = 0;
            skillPro = 0;
        }
    }
    if(skillId != 0 && skillPro != 0)
        AddSkill(skillId, skillPro);

    // 技能2
    skillId = skillPro = 0;
    pDoc->QueryIntAttribute("skill2_id", &skillId);
    pDoc->QueryIntAttribute("skill2_pro", &skillPro);

    if (skillId < 10 && skillId != 0)
    {
        if (skillId <= skillCount)
        {
            skillId = skill_list[skillId - 1];
        }
        else
        {
            skillId = 0;
            skillPro = 0;
        }
    }
    if(skillId != 0 && skillPro != 0)
        AddSkill(skillId, skillPro);

    // 技能3
    skillId = skillPro = 0;
    pDoc->QueryIntAttribute("skill3_id", &skillId);
    pDoc->QueryIntAttribute("skill3_pro", &skillPro);

    if (skillId < 10 && skillId != 0)
    {
        if (skillId <= skillCount)
        {
            skillId = skill_list[skillId - 1];
        }
        else
        {
            skillId = 0;
            skillPro = 0;
        }
    }
    if(skillId != 0 && skillPro != 0)
        AddSkill(skillId, skillPro);

    // 技能4
    skillId = skillPro = 0;
    pDoc->QueryIntAttribute("skill4_id", &skillId);
    pDoc->QueryIntAttribute("skill4_pro", &skillPro);
    if (skillId < 10 && skillId != 0)
    {
        if (skillId <= skillCount)
        {
            skillId = skill_list[skillId - 1];
        }
        else
        {
            skillId = 0;
            skillPro = 0;
        }
    }
    if(skillId != 0 && skillPro != 0)
        AddSkill(skillId, skillPro);

}

void AIActionSelectSkill::Do()
{
    AIController::AISkillList skillList = GetSkillList();
    BattleUnit* pUint = m_pController->GetOwner();

    // 根据概率选择技能
    int32 p = Crown::SDRandom(100);
    int32 pro = 0;
    uint32 skillId = 0;

    for(AIController::AISkillList::iterator iter = skillList.begin(); iter != skillList.end(); iter++)
    {
        const AISkillData& skillData = *iter;
        pro += skillData.probability;
        if(p < pro)
        {
            skillId = skillData.skillId;
            break;
        }
    }

    if(!skillId)
    {
        CnWarn("AIActionSelectSkill 0\n");
    }
    
    m_pController->SetSelectSkill(skillId);
}
