#include "gameholder_pch.h"
#include "battle_unit.h"
#include "skill_data.h"
#include "skill_object.h"
#include "scene.h"
#include "battle_manager.h"
#include "action_controller.h"
#include "formula_manager.h"
#include "game_util.h"
#include "skill_logic_strike.h"
#include "battle_unit.h"
#include "skill_utils.h"
#include "skill_config_manager.h"
#include "buff_define.h"
#include "event_define.h"


SkillLogicStrike::SkillLogicStrike(const SHOOT_TARGET& strikeTarget, const StrikeArgs& args) :
m_StrikeTarget(strikeTarget),
m_StrikeArgs(args)
{
    
}

SkillLogicStrike::~SkillLogicStrike()
{

}

void SkillLogicStrike::Init(SkillObject* pSkill)
{
    SkillLogicBase::Init(pSkill);
}

void SkillLogicStrike::Update()
{
    End();

    BattleUnit* Owner = m_Skill->GetOwner();
    BattleUnit* pTarget = CnDynamicCast(BattleUnit, Owner->GetScene()->GetSceneObject(m_StrikeTarget.target_id));
    if(!pTarget)
        return;

    // 伤害逻辑处理
    uint32 damageType = BattleManager::Instance()->Attack(m_Skill->GetOwner(), pTarget, m_Skill);
    bool isHit = (damageType == DAMAGE_TYPE_NORMAL || damageType == DAMAGE_TYPE_CRITICAL);

    const SKILL_ROOT_SKILL_STRUCT* pSkillConfig = SkillConfigManager::Instance()->GetSkillConfigData(m_Skill->GetSkillId());
    if(!pSkillConfig)
    {
        CnAssert(false);
        return;
    }

    // 技能造成buff
    if(Owner == pTarget)
    {
        if(pSkillConfig->to_me)
            CauseBuff(pTarget);
    }
    else
    {
        if(Owner->IsEnemy(pTarget))
        {
            if(pSkillConfig->to_enemy)
                CauseBuff(pTarget);
        }
        else
        {
            if(pSkillConfig->to_friend)
                CauseBuff(pTarget);
        }
    }

    // 击中效果
    if(isHit)
    {
        const SkillArgumentData& skillArgs = m_Skill->GetSkillArgument();
        uint32 knockBackPro = skillArgs.m_SkillProps.GetValue(PROP_SKILL_KNOCK_BACK);
        if(knockBackPro > 0 && Crown::SDRandom(BASE_RATE_NUM) <= knockBackPro)
        {
            if(!skillArgs.HasKnockBackId(pTarget->GetId()))
            {
                CauseMove(pTarget);
                skillArgs.AddKnockBack(pTarget->GetId());
            }
        }
    }

    if(isHit && m_Skill->GetSkillMode() == SKILL_MODE_SELF)
    {
        // 充能球判定
        const int32 chargeBuffTime = 15000;
        const SKILL_ROOT_SKILL_STRUCT* skillConfig = SkillConfigManager::Instance()->GetSkillConfigData(m_Skill->GetSkillId());
        if(!skillConfig)
        {
            CnAssert(false);
            return;
        }

        // 近战击中获得耐力球
        if( (skillConfig->feature & SKILL_FEATURE_MELEE)
            && ((m_Skill->m_TriggerFlags & SKILL_TRIGGER_ENDURANCE_CHARGE) == 0)
            )
        {
            int32 enduranceChargePro = m_Skill->GetSkillArgument().m_SkillProps.GetValue(PROP_SKILL_ENDURANCE_CHARGE);
            if(enduranceChargePro > 0 && (int32)Crown::SDRandom(BASE_RATE_NUM) <= enduranceChargePro)
            {
                if(Owner->GetObjectType() == OBJ_MONSTER)
                    Owner->AddBuff(BUFF_ID_ENDURANCE_CHARGES, chargeBuffTime, Owner, 1500, 0);
                else
                    Owner->AddBuff(BUFF_ID_ENDURANCE_CHARGES, chargeBuffTime, Owner, 500, 0);
                m_Skill->m_TriggerFlags |= SKILL_TRIGGER_ENDURANCE_CHARGE;
            }
        }

        // 攻击击中获得狂怒球
        if( (skillConfig->feature & SKILL_FEATURE_ATTACK)
            && ((m_Skill->m_TriggerFlags & SKILL_TRIGGER_FRENZY_CHARGE) == 0)
            )
        {
            int32 frenzyChargePro = m_Skill->GetSkillArgument().m_SkillProps.GetValue(PROP_SKILL_FRENZY_CHARGE);
            if(frenzyChargePro > 0 && (int32)Crown::SDRandom(BASE_RATE_NUM) <= frenzyChargePro)
            {
                if(Owner->GetObjectType() == OBJ_MONSTER)
                    Owner->AddBuff(BUFF_ID_FRENZY_CHARGES, chargeBuffTime, Owner, 1500, 0);
                else
                    Owner->AddBuff(BUFF_ID_FRENZY_CHARGES, chargeBuffTime, Owner, 500, 0);
                m_Skill->m_TriggerFlags |= SKILL_TRIGGER_FRENZY_CHARGE;
            }
        }

        // 暴击获得暴击球
        if( (damageType == DAMAGE_TYPE_CRITICAL)
            && ((m_Skill->m_TriggerFlags & SKILL_TRIGGER_POWER_CHARGE) == 0)
            )
        {
            int32 powerChargePro = m_Skill->GetSkillArgument().m_SkillProps.GetValue(PROP_SKILL_POWER_CHARGE);
            if(powerChargePro > 0 && (int32)Crown::SDRandom(BASE_RATE_NUM) <= powerChargePro)
            {
                if(Owner->GetObjectType() == OBJ_MONSTER)
                    Owner->AddBuff(BUFF_ID_POWER_CHARGES, chargeBuffTime, Owner, 1500, 0);
                else
                    Owner->AddBuff(BUFF_ID_POWER_CHARGES, chargeBuffTime, Owner, 500, 0);
                m_Skill->m_TriggerFlags |= SKILL_TRIGGER_POWER_CHARGE;
            }
        }

        // 技能暴击事件
        if(damageType == DAMAGE_TYPE_CRITICAL && pTarget && Owner->IsEnemy(pTarget))
        {
            if((m_Skill->m_TriggerFlags & SKILL_TRIGGER_CRITICAL_EVENT) == 0)
            {
                Owner->GetScene()->SendEvent(Owner->GetId(), BATTLE_EVENT_SELF_SKILL_CRITICAL, pTarget->GetId(), 0, 0);
                m_Skill->m_TriggerFlags |= SKILL_TRIGGER_CRITICAL_EVENT;
            }
        }

        // 近战击中事件
        if((skillConfig->feature & SKILL_FEATURE_MELEE)
            && ((m_Skill->m_TriggerFlags & SKILL_TRIGGER_MELEE_EVENT) == 0)
            )
        {
            Owner->GetScene()->SendEvent(Owner->GetId(), BATTLE_EVENT_SELF_SKILL_MELEE, pTarget->GetId(), 0, 0);
            m_Skill->m_TriggerFlags |= SKILL_TRIGGER_MELEE_EVENT;
        }
    }

}

void SkillLogicStrike::CauseBuff(BattleUnit* pTarget)
{
    BattleUnit* Owner = m_Skill->GetOwner();
    uint32 skillID = m_Skill->GetSkillId();

    const SKILL_ROOT_SKILL_STRUCT* pSkillConfig = SkillConfigManager::Instance()->GetSkillConfigData(skillID);
    if(!pSkillConfig)
        return;

    // 技能击中后产生buff效果
    if(pSkillConfig->buff == 0)
        return;
    
    const BUFF_ROOT_BUFF_STRUCT* buffConfig = SkillConfigManager::Instance()->GetBuffConfigData(pSkillConfig->buff);
    if(!buffConfig)
    {
        CnAssert(false);
        return;
    }

    const SkillArgumentData& args = m_Skill->GetSkillArgument();
    int32 skill_grow = SkillConfigManager::Instance()->GetSkillGrowByLevelQuality(args.m_SkillLevel, args.m_SkillQuality);

    int32 buff_val1 = int32(pSkillConfig->buff_val1_base + pSkillConfig->buff_val1_grow * ToPercent(skill_grow));
    int32 buff_val2 = int32(pSkillConfig->buff_val2_base + pSkillConfig->buff_val2_grow * ToPercent(skill_grow));

    if(buffConfig->is_aure)
    {   // 光环的处理
        CnAssert(pTarget == Owner);
        if(pTarget->HasBuff(pSkillConfig->buff))
        {
            pTarget->DelBuff(pSkillConfig->buff);
        }
        else
        {
            pTarget->AddBuff(pSkillConfig->buff, 864000000, Owner, buff_val1, buff_val2);     // 光环给一个很长的时间
        }
    }
    else
    {   // 普通buff
        pTarget->AddBuff(pSkillConfig->buff, pSkillConfig->buff_time, Owner, buff_val1, buff_val2);
    }
}

void SkillLogicStrike::CauseMove(BattleUnit* Target)
{
    BattleUnit* Owner = m_Skill->GetOwner();

    // 怪物不能被推动
    if(Target->GetCantPush())
        return;

    // 推动敌人移动
    if(Owner->CanAttack(Target))
    {
        MoveEnemy(Owner, Target,
            0,
            100, 100, 0, 0);
    }
    // 怪物被打死了(ROG暂无此功能)
    //else if(Target->IsDead() && (pSkillData->m_DeadPushEnemyBounceTime != 0 || pSkillData->m_DeadPushEnemyTime != 0))
    //{
    //    MoveEnemy(Owner, Target, pSkill,
    //        pSkillData->m_DeadPushEnemyMode,
    //        pSkillData->m_DeadPushEnemyTime, pSkillData->m_DeadPushEnemyDistance,
    //        pSkillData->m_DeadPushEnemyBounceTime, pSkillData->m_DeadPushEnemyBounceHeight);
    //}
}

// 移动目标
void SkillLogicStrike::MoveEnemy(BattleUnit* Owner, BattleUnit* Target, int32 PushMode, int32 PushTime, int32 PushDistance, int32 BounceTime, int32 BounceHeight)
{
    // 推动
    int32 angle = 0;
    int32 distance = 0;

    if(PushMode == 0)   // 背离角色方向
    {
        CoVec2 dir = Target->GetPosition() - Owner->GetPosition();
        if(PushDistance > 0)
        {   // 远离角色
            angle = int32(dir.GetAngle() / M_PI * 180);
            distance = PushDistance;
        }
        else
        {   // 靠近角色
            angle = int32(dir.GetAngle() / M_PI * 180 + 180);
            distance = -PushDistance;

            int32 max_push = Crown::SDMax((int32)dir.Length() - Target->GetRadius() - Owner->GetRadius(), 0);
            distance = Crown::SDMin(distance, max_push);
        }
    }
    else if(PushMode == 1)  // 攻击者方向
    {
        angle = Owner->GetAngle();
        distance = PushDistance;
        if(distance < 0)
        {
            angle += 180;
            distance = -distance;
        }
    }
    else if(PushMode == 2)  // 随机朝向
    {
        distance = PushDistance;
        if(distance < 0) distance = -distance;
        angle = (int32)Crown::SDRandomFloat(360.f);
    }

    // 推动目标
    if(PushTime > 0 || distance > 0)
    {
        CoVec2 desPos = Owner->GetScene()->GetCoWorld()->RayTraceFind(Target->GetPosition(), (int32)angle, (real)distance, (real)Target->GetRadius());
        Target->GetActionController()->PushTo((int32)desPos.x, (int32)desPos.y, PushTime, true);
    }

    // 弹跳目标
    if(BounceTime > 0 && BounceHeight > 0)
    {   // 弹跳
        Target->GetActionController()->Bounce(BounceHeight, BounceTime);
    }
}






// 击中后的各种效果
/*
void HitNotify(BattleUnit* Owner, BattleUnit* Target, Skill* pSkill, SkillHitNode* pHit);       // 击中通知
void CauseDamage(BattleUnit* Owner, BattleUnit* Target, Skill* pSkill, SkillHitNode* pHit);     // 造成伤害
void CauseHeal(BattleUnit* Owner, BattleUnit* Target, Skill* pSkill, SkillHitNode* pHit);       // 造成治疗
void CauseBuff(BattleUnit* Owner, BattleUnit* Target, Skill* pSkill, SkillHitNode* pHit);       // 造成buff
void CauseMove(BattleUnit* Owner, BattleUnit* Target, Skill* pSkill, SkillHitNode* pHit);       // 造成移动

void SkillHit::Hit( BattleUnit* Owner, BattleUnit* Target, Skill* pSkill, SkillHitNode* pHit)
{
    const ShootData* shootData = pHit->GetShootData();

    ((SkillObject*)pSkill)->SetCalcShootData(shootData);        // 伤害计算用

    // 攻击有效性判定
    DAMAGE_INFO damageInfo = BattleManager::Instance()->IsHitSucceed(Owner, Target, pSkill);
    if(damageInfo.type != 0)        // 攻击无效
    {
        if(!Target)
        {
            CnAssert(false);
            return;
        }
        // 攻击无效通知
        TAKE_DAMAGE_NTF sendMsg;
        sendMsg.attacker_id = Owner->GetId();
        sendMsg.object_id = Target->GetId();
        sendMsg.damage_info = damageInfo;
        Target->GetScene()->NotifyMessage(Target, MSG_TAKE_DAMAGE_NTF, &sendMsg);
        return;
    }

    // 击中通知
    HitNotify(Owner, Target, pSkill, pHit);

    // 对目标产生伤害    
    CauseDamage(Owner, Target, pSkill, pHit);

    // 对目标产生治疗
    CauseHeal(Owner, Target, pSkill, pHit);

    // 对目标产生buff
    CauseBuff(Owner, Target, pSkill, pHit);

    // 对目标产生移动
    CauseMove(Owner, Target, pSkill, pHit);
}

void HitNotify(BattleUnit* Owner, BattleUnit* Target, Skill* pSkill, SkillHitNode* pHit)
{
    const ShootData* shootData = pHit->GetShootData();
    // 技能打中通知
    SKILL_HIT_NTF hitAck;
    hitAck.caster_id        = Owner->GetId();
    hitAck.target_id        = Target->GetId();
    hitAck.skill_id         = pSkill->GetSkillId();
    hitAck.skill_index      = pSkill->GetSkillIndex();
    hitAck.shoot_id         = shootData->m_ShootId;
    hitAck.shoot_index      = pHit->GetShootIndex();
    hitAck.hit_direction    = pHit->GetHitDirection();

    Owner->GetScene()->NotifyMessage(Owner, MSG_SKILL_HIT_NTF, &hitAck);
}

void CauseDamage(BattleUnit* Owner, BattleUnit* Target, Skill* pSkill, SkillHitNode* pHit)
{
    if(!Owner->CanAttack(Target))
        return;

    SkillData* pSkillData = pSkill->GetSkillData();
    const ShootData* shootData = pHit->GetShootData();

    if(!shootData->m_hasDamage)
        return;

    // 伤害计算
    DAMAGE_INFO damageInfo;
    if(pSkill->GetSkillMode() == SKILL_MODE_SELF)
    {   // 自身使用技能
        BattleManager::Instance()->Attack(Owner, Target, pSkill);       // 计算伤害值
    }
    else if(pSkill->GetSkillMode() == SKILL_MODE_FOLLOW_PET)
    {   // 跟宠使用技能
        BattleManager::Instance()->AttackByFollowPet(Owner, Target, pSkill);
    }
    else if(pSkill->GetSkillMode() == SKILL_MODE_SCENE)
    {   // 场景技能
        BattleManager::Instance()->AttackByScene(Target, pSkill);
    }
    else
        CnAssert(false);
}

void CauseHeal(BattleUnit* Owner, BattleUnit* Target, Skill* pSkill, SkillHitNode* pHit)
{
    SkillData* pSkillData = pSkill->GetSkillData();
    const ShootData* shootData = pHit->GetShootData();

    if(!shootData->m_hasHeal)
    {
        return;
    }

    BattleManager::Instance()->MakeHeal(Owner, Target, pSkill);     // 治疗
}

void _DoBuff(bool isAddBuff, BattleUnit* Owner, BattleUnit* Target, int32 buffId, int32 buffTime, int32 buffVal)
{
    // 加BUFF
    if(isAddBuff && Owner->CanBuff(Target))
    {
        Target->AddBuff(buffId, buffTime, buffVal, Owner);
    }
    // 去BUFF
    else if(!isAddBuff && Owner->CanDelBuff(Target))
    {
        Target->DelBuff(buffId);
    }
}

void CauseBuff(BattleUnit* Owner, BattleUnit* Target, Skill* pSkill, SkillHitNode* pHit)
{
    SkillData* pSkillData = pSkill->GetSkillData();
    const ShootData* shootData = pHit->GetShootData();

    // buff效果
    for(int bi=0; bi<MAX_BUFF_DATA; bi++)
    {
        if(shootData->m_Buff[bi].m_BuffInfo.buff_id == 0)
            continue;

        if(shootData->m_Buff[bi].m_BuffProbability <= (int32)Crown::SDRandom(100))
            continue;

        int32 buffId = shootData->m_Buff[bi].m_BuffInfo.buff_id;

        // 如果是玩家技能, 跟符文等级走
        int32 level = 1;
        if(!IsNormalAttack(pSkill->GetSkillId()))
            level = pSkill->GetLevelBySkillID();

        int32 buffTime = pSkill->GetBuffTime(level, bi);
        int32 buffVal = pSkill->GetBuffVal(level, bi);

        bool addBuff = shootData->m_Buff[bi].m_addBuff;
        bool canSelf = shootData->m_Buff[bi].m_canSelf;
        bool canFriend = shootData->m_Buff[bi].m_canFriend;
        bool canEnemy = shootData->m_Buff[bi].m_canEnemy;

        BattleUnit* BuffTarget = NULL;
    
        if(canSelf)
        {
            _DoBuff(addBuff, Owner, Owner, buffId, buffTime, buffVal);          // 对自己使用Buff
        }

        bool isEnemy = Owner->IsEnemy(Target);
        bool can = false;

        if(isEnemy && canEnemy)
        {
            can = true;
            BuffTarget = Target;        // 对敌人使用Buff
        }
        else if(!isEnemy && canFriend)
        {
            can = true;
            BuffTarget = Target;        // 对盟友使用Buff
        }

        if(can && BuffTarget)
        {
            _DoBuff(addBuff, Owner, BuffTarget, buffId, buffTime, buffVal);
        }
    }
}
*/


