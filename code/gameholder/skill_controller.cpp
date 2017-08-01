#include "gameholder_pch.h"
#include "skill_controller.h"
#include "battle_unit.h"
#include "skill_object.h"
#include "skill_manager.h"
#include "skill_data.h"
#include "scene.h"
#include "regular.h"
#include "game_define.h"
#include "player.h"
#include "performance_analysis.h"
#include "game_util.h"
#include "skill_info_manager.h"
#include "skill_config_manager.h"
#include "soldier.h"


SkillController::SkillController(BattleUnit* owner) :
m_Owner(owner)
{
    m_inUpdate = false;
    m_silenceTime = 0;

    m_skillObjectIDCounter = 0;
}

SkillController::~SkillController()
{
    // 释放未结束的技能
    for(uint32 i=0; i< m_Skills.size(); i++)
    {
        CnDelete m_Skills[i];
    }
}

void SkillController::FillSkillState(STATE_INFO& info)
{
    info.skill_cd_list.clear();
    KEY_VALUE cdInfo;

    for(std::map<uint32, int32>::iterator cdIter = m_SkillCDMap.begin(); cdIter != m_SkillCDMap.end(); cdIter++)
    {
        cdInfo.key = cdIter->first;
        cdInfo.value = cdIter->second;
        info.skill_cd_list.push_back(cdInfo);
    }
}

void SkillController::LoadSkillState(const STATE_INFO& info)
{
    for(std::vector<KEY_VALUE>::const_iterator iter = info.skill_cd_list.begin(); iter != info.skill_cd_list.end(); iter++)
    {
        m_SkillCDMap[iter->key] = iter->value;
    }
}

void SkillController::Update(uint32 delta_time)
{
    if (!m_Owner->GetScene())
    {
        return;
    }

    PERFORM_HELPER;
    m_inUpdate = true;
    int32 ki=0;
    while(ki < (int32)m_Skills.size())
    {
        SkillObject* pSkill = m_Skills[ki];

        if(pSkill->isEnd())
        {
            NotifyEndSkill(pSkill->GetSkillId(), pSkill->GetSkillObjectID());       // 技能结束通知
            m_Skills.erase(m_Skills.begin()+ki);
            CnDelete pSkill;
        }
        else
        {
            pSkill->Update();
            ++ki;
        }
    }
    m_inUpdate = false;

    // 技能CD
    std::map<uint32, int32>::iterator delIter;
    for(std::map<uint32, int32>::iterator cdIter = m_SkillCDMap.begin(); cdIter != m_SkillCDMap.end();)
    {
        cdIter->second -= delta_time;
        if(cdIter->second <= 0)
        {
            delIter = cdIter;
            cdIter++;
            m_SkillCDMap.erase(delIter);
        }
        else
            cdIter++;
    }

    // 沉默
    if(m_silenceTime > 0)
    {
        m_silenceTime -= delta_time;
        if(m_silenceTime < 0)
        {
            m_silenceTime = 0;
        }
    }
}

void SkillController::OnStateChange(uint32 state)
{
    for(std::vector<SkillObject*>::iterator iter = m_Skills.begin(); iter != m_Skills.end(); iter++)
    {
        (*iter)->OnStateChange(state);
    }
}

void SkillController::StopAllSkill()
{
    for(std::vector<SkillObject*>::iterator iter = m_Skills.begin(); iter != m_Skills.end(); iter++)
    {
        (*iter)->End();
    }
}

bool SkillController::CheckCanUseSkill(uint32 skill_id)
{
    if(m_Owner->GetObjectType() == OBJ_PLAYER)
    {
        if(!m_Owner->GetScene())
            return false;
        if(!m_Owner->GetScene()->GetRegular())
            return false;
        if(!m_Owner->GetScene()->GetRegular()->CanUseSkill(skill_id))
            return false;
    }

    return true;
}
// 主动使用技能
SkillObject* SkillController::UseSkill(uint32 skill_id, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skill_obj_id, uint32 skill_slot)
{
    if(!CheckCanUseSkill(skill_id))
        return NULL;

    // 沉默检查
    if(m_silenceTime > 0)
    {
        return NULL;
    }

    // CD检查
    if(inCD(skill_id))
    {
        return NULL;    // 技能CD中
    }
    
    // 魔法值消耗
    if(m_Owner->GetObjectType() == OBJ_PLAYER)
    {
        if( ((Player*)m_Owner)->GetMP() >= args.m_Mp )
        {
            ((Player*)m_Owner)->SetMP( ((Player*)m_Owner)->GetMP() - args.m_Mp );
        }
        else
            return false;       // 魔法值不够
    }

    CnAssert(args.m_SkillMode != 0);

    SkillObject* pSkill = _UseSkill(skill_id, target, args, skill_obj_id, skill_slot);
    if(!pSkill)
        goto fail;

    return pSkill;
fail:
    if(pSkill)
        CnDelete pSkill;

    return NULL;
}
// 被动触发使用技能
SkillObject* SkillController::UseSkillByPassive(uint32 skill_id, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skill_slot)
{
    if(!CheckCanUseSkill(skill_id))
        return NULL;

    CnAssert(args.m_SkillMode == SKILL_MODE_PASSIVE);

    SkillObject* pSkill = _UseSkill(skill_id, target, args, 0, skill_slot);
    if(!pSkill)
        goto fail;

    return pSkill;
fail:
    if(pSkill)
        CnDelete pSkill;

    return NULL;
}
//
//SkillObject* SkillController::UseSkillByFollowPet(uint32 skill_id, const SHOOT_TARGET& target, const SkillArgumentData& args)
//{
//    if(!CheckCanUseSkill(skill_id))
//        return NULL;
//
//    CnAssert(args.m_SkillMode == SKILL_MODE_FOLLOW_PET);
//
//    SkillObject* pSkill = _UseSkill(skill_id, target, args, 0);
//    if(!pSkill)
//        goto fail;
//    
//    return pSkill;
//fail:
//    if(pSkill)
//        CnDelete pSkill;
//
//    return NULL;
//}
//
//SkillObject* SkillController::UseSkillByScene(uint32 skill_id, const SHOOT_TARGET& target, const SkillArgumentData& args)
//{
//    if(!CheckCanUseSkill(skill_id))
//        return NULL;
//
//    // CD检查
//    if(inCD(skill_id))
//    {
//        return NULL;    // 技能CD中
//    }
//
//    CnAssert(args.m_SkillMode == SKILL_MODE_SCENE);
//
//    SkillObject* pSkill = _UseSkill(skill_id, target, args, 0);
//    if(!pSkill)
//        goto fail;
//
//    return pSkill;
//fail:
//    if(pSkill)
//        CnDelete pSkill;
//
//    return NULL;
//}

SkillObject* SkillController::_UseSkill(uint32 skill_id, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skill_obj_id, uint32 skill_slot)
{
    // 技能CD记录
    uint32 cooldown = 0;

    SkillObject* pSkill = SkillManager::Instance()->CreateSkill(skill_id, m_Owner, args);
    if(!pSkill)
        goto fail;

    if(skill_obj_id == 0)       // 服务器角色创建的技能时skill_obj_id==0, 由服务器生成技能对象的唯一ID
    {
        if(m_skillObjectIDCounter >= 0xffff) 
            m_skillObjectIDCounter = 0;
        skill_obj_id = ++m_skillObjectIDCounter;
    }

    pSkill->SetSkillObjectID(skill_obj_id);      // 设置技能对象ID

    if(!pSkill->Init(target, args, skill_slot))
        goto fail;

    // 使用技能成功
    //CnAssert(!m_inUpdate);      // 不应该在技能Update中又产生技能
    m_Skills.push_back(pSkill);

    // CD计算
    if(m_Owner->GetObjectType() == OBJ_PLAYER && !args.m_ConsumeSkill)
    {
        Player* player = CnDynamicCast(Player, m_Owner);
        const SkillInfo* pSkillInfo = player->GetSkillInfoMgr()->GetSkillInfoBySlot(skill_slot);
        if(pSkillInfo && pSkillInfo->GetSkillId() == skill_id)
        {
            cooldown = pSkillInfo->GetCoolDown();
        }
        else
        {
            if(IsNormalAttack(skill_id))
            {
                pSkillInfo = player->GetSkillInfoMgr()->GetNormalAttackSkillInfo(skill_id);
            }
            else
            {
                cooldown = 0;       // 技能被替换掉了
            }
        }

        bool isGMNoCD = ((Player*)m_Owner)->IsGMNoCD();
        if(isGMNoCD)
            cooldown = 500;            // GM功能,技能无CD
    }
    else if(m_Owner->IsKindOf(&Soldier::ms_RTTI))
    {
        Soldier* pSoldier = CnDynamicCast(Soldier, m_Owner);
        const SkillInfo* pSkillInfo = NULL;
        if (pSoldier->DefaultSkillId() == skill_id)
        {
            pSkillInfo = pSoldier->GetDefaultSkillInfo();
        }
        else
        {
            pSkillInfo = pSoldier->GetSkillInfoMgr()->GetSkillInfoBySlot(skill_slot);
        }

        if(pSkillInfo)
        {
            cooldown = pSkillInfo->GetCoolDown();
        }
        else
        {
            CnAssert(false);
            cooldown = 0;
        }

    }

    if(cooldown>0)
        m_SkillCDMap[skill_id] = cooldown;


    // 发送使用技能通知
    NotifyUseSkill(skill_id, cooldown, target, args, skill_obj_id, skill_slot);

    // first update
    pSkill->Update();

    return pSkill;
fail:
    if(pSkill)
        CnDelete pSkill;

    return NULL;
}

int32 SkillController::MakeSkillCD(uint32 skill_id, int32 cdtime)
{
    int32 oldCD = m_SkillCDMap[skill_id];
    if(cdtime > oldCD)
        m_SkillCDMap[skill_id] = cdtime;
    return m_SkillCDMap[skill_id];
}

bool SkillController::inCD( uint32 skill_id )
{
    std::map<uint32, int32>::iterator iter = m_SkillCDMap.find(skill_id);
    if(iter == m_SkillCDMap.end())
        return false;

    int32 cdtime = iter->second;
    if(cdtime <= 0)
        return false;

    return true;
}

void SkillController::ClearCD()
{
    m_SkillCDMap.clear();
}

int32 SkillController::SubSkillCD(uint32 skill_id)
{
    if(m_Owner->GetObjectType() == OBJ_PLAYER)
    {
        Player* player = (Player*)m_Owner;
       
    }
    return 0;
}

void SkillController::NotifyUseSkill(uint32 skill_id, uint32 skill_cd, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skill_obj_id, uint32 skill_slot)
{
    // 技能使用通知
    USE_SKILL_NTF sendMsg;
    sendMsg.owner_id = m_Owner->GetId();
    sendMsg.skill_cd = skill_cd;
    sendMsg.skill.skill_id = skill_id;
    sendMsg.skill.skill_slot = skill_slot;
    sendMsg.skill.skill_obj_id = skill_obj_id;
    sendMsg.skill.target = target;
    sendMsg.skill.skill_args = args.toNetArgument();

    // 使用技能成功
    sendMsg.error = ERR_SUCCEED;
    
    m_Owner->GetScene()->NotifyMessage(m_Owner, MSG_USE_SKILL_NTF, &sendMsg);
}

void SkillController::NotifyEndSkill(uint32 skill_id, uint32 skill_obj_id)
{
    // 技能结束通知
    SKILL_END_NTF ntf;
    ntf.owner_id = m_Owner->GetId();
    ntf.skill_id = skill_id;
    ntf.skill_obj_id = skill_obj_id;
    m_Owner->GetScene()->NotifyMessage(m_Owner, MSG_SKILL_END_NTF, &ntf);
}



