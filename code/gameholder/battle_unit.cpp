#include "gameholder_pch.h"
#include "battle_unit.h"
#include "state_controller.h"
#include "skill_controller.h"
#include "buff_controller.h"
#include "skill_object.h"
#include "scene.h"
#include "action_factory.h"
#include "action_controller.h"
#include "ai_controller.h"
#include "prop_notifier.h"
#include "state_define.h"
#include "event_define.h"
#include "pet_controller.h"
#include "skill_data.h"
#include "relife_helper.h"
#include "remana_helper.h"
#include "pet.h"
#include "player.h"
#include "world_prop_communicator.h"
#include "trigger.h"
#include "formula_manager.h"
#include "touch_controller.h"
#include "performance_analysis.h"
#include "game_util.h"
#include "regular.h"
#include "resurgence_helper.h"
#include "reenergyshield_helper.h"
#include "ai_group_register.h"
#include "property_set_calculater.h"
#include "monster.h"
#include "buff_define.h"
#include "battle_recorder.h"


CnImplementRTTI(BattleUnit, SceneObject);

BattleUnit::BattleUnit( uint32 objType ) :
SceneObject(objType),
m_group(FRIEND_GROUP)
{
    m_LastFrameCounter = 0;

    m_TouchController   = CnNew TouchController(this);
    m_PropNotifier      = CnNew PropNotifier(this);
    m_StateController   = CnNew StateController(this);
    m_SkillController   = CnNew SkillController(this);
    m_BuffController    = CnNew BuffController(this);
    m_ActionController  = CreateAction(this);
    m_PetController     = CnNew PetController(this);
    m_AIController      = CnNew AIController(this);
    
    m_ResurgenceHelper = CnNew ResurgenceHelper(this);
    m_ReEnergyShieldHelper = CnNew ReEnergyShieldHelper(this);
    m_ReMpHelper = CnNew ReMpHelper(this);
    m_ReHpHelper = CnNew ReHpHelper(this);
    m_PropertySetCalculater = CnNew PropertySetCalculater(this);

    BattleUnitClear();

    m_NoDamage = false;
    m_Attackable = true;
    m_FinalAttackable = true;
    m_NoCollisionWithObject = false;
    m_ShowFlag = 0;

    m_communicator = NULL;
}

BattleUnit::~BattleUnit()
{
    CnAssert(!m_communicator);
    
    CnDelete m_PropertySetCalculater;
    CnDelete m_ReHpHelper;
    CnDelete m_ReMpHelper;
    CnDelete m_ReEnergyShieldHelper;
    CnDelete m_ResurgenceHelper;

    CnDelete m_AIController;
    CnDelete m_PetController;
    CnDelete m_ActionController;
    CnDelete m_BuffController;
    CnDelete m_SkillController;
    CnDelete m_StateController;
    CnDelete m_PropNotifier;
    CnDelete m_TouchController;
}

void BattleUnit::BattleUnitClear()
{
    m_level = 1;                        // 等级
    m_group = 0;                        // 组
    m_TeamID_L = 0;                     // 队伍
    m_TeamID_H = 0;

    ClearAttack();

    // 基础属性
    ClearHP();
    ClearMP();
    ClearEnergyShield();
    ClearMaxHp();
    ClearMaxMp();
    ClearMaxEnergyShield();
    ClearMoveSpeed();
    ClearHpRecovery();
    ClearMpRecovery();
    ClearArmor();
    ClearDodge();
    ClearHit();
    ClearCrit();
    ClearCritDamage();
    ClearAttackSpeed();
    ClearCastSpeed();
    ClearFireResistance();
    ClearIceResistance();
    ClearLightningResistance();
    ClearAttackBlock();
    ClearMagicBlock();

    // 高级属性
    ClearMaxFireResistance();
    ClearMaxIceResistance();
    ClearMaxLightningResistance();
    ClearMaxAttackBlock();
    ClearMaxMagicBlock();

}

uint32 BattleUnit::GetAIState()
{
    return m_AIController->GetAIState();
}

bool BattleUnit::IsDead()
{
    return GetState() == STATE_DIE;
}

uint32 BattleUnit::GetState()
{
    return m_StateController->GetState();
}

bool BattleUnit::CanChangeTo(uint32 state)
{
    return m_StateController->CanChangeTo(state);
}

bool BattleUnit::CanChangePostion()
{
    return m_StateController->CanChangePostion();
}

bool BattleUnit::CanUseSkill()
{
    return m_StateController->CanUseSkill();
}

void BattleUnit::ChangeState( uint32 state )
{
    m_StateController->ChangeState(state);
}

void BattleUnit::OnStateChange(uint32 oldState, uint32 newState)
{
    m_SkillController->OnStateChange(newState);
    m_ActionController->OnStateChange(newState);
    m_TouchController->OnStateChange(oldState, newState);

    UpdateB2PhysicsCategoryAndMask();
}

void BattleUnit::UpdateB2PhysicsCategoryAndMask()
{
    if(!GetB2Object())
        return;

    b2Fixture* pFixture = GetB2Object()->GetFixtureList();
    if(!pFixture)
        return;

    uint32 state = m_StateController->GetState();

    // mask
    uint32 finalMask = 0;
    finalMask = m_B2Mask;

    // category
    uint32 finalCategory = 0;
    if(state == STATE_DIE)
        finalCategory = 0;                  // 死亡后无碰撞
    else
        finalCategory = m_B2Category;

    // 设置
    const b2Filter& filterData = pFixture->GetFilterData();
    if(filterData.categoryBits == finalCategory && filterData.maskBits == finalMask)
        return;

    b2Filter newFilter;
    newFilter.categoryBits = finalCategory;
    newFilter.maskBits = finalMask;
    pFixture->SetFilterData(newFilter);
}

void BattleUnit::onEnterScene( Scene* pScene )
{
    SceneObject::onEnterScene(pScene);

    m_PetController->onEnterScene(pScene);

    m_ActionController->OnEnterScene(pScene);
}

void BattleUnit::onLeaveScene( Scene* pScene )
{
    SceneObject::onLeaveScene(pScene);

    m_PetController->onLeaveScene(pScene);

    m_ActionController->OnLeaveScene(pScene);
}

void BattleUnit::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 )
{
    m_AIController->ReceiveEvent(eventId, arg1, arg2);
    m_PetController->ReceiveEvent(eventId, arg1, arg2);

    // 复活
    switch(eventId)
    {
    case BATTLE_EVENT_RESURGENCE:
        m_ResurgenceHelper->EventResurgence(arg1, arg2);        // 复活事件 (复活Trigger点, 延时)
        break;
    default:
        break;
    }
}

void BattleUnit::Update(uint32 frame_count)
{
    if (!GetScene())
    {
        return;
    }

    uint32 delta_Frame = 1;
    if(m_LastFrameCounter != 0)
    {
        delta_Frame = frame_count - m_LastFrameCounter;
    }
    m_LastFrameCounter = frame_count;
    uint32 delta_time = delta_Frame*g_ServerUpdateTime;
    uint32 curr_time = frame_count*g_ServerUpdateTime;

    SceneObject::Update(frame_count);

    PERFORM_NAME_BEGIN("BattleUnit::Update11");
    // 对象控制器
    m_ActionController->Update();
    PERFORM_NAME_END("BattleUnit::Update11");

    PERFORM_NAME_BEGIN("BattleUnit::Update12");
    // 技能控制器
    m_SkillController->Update(delta_time);
    PERFORM_NAME_END("BattleUnit::Update12");

    PERFORM_NAME_BEGIN("BattleUnit::Update13");
    // buff控制器
    m_BuffController->Update(delta_time);

    // 状态机
    m_StateController->Update();
    PERFORM_NAME_END("BattleUnit::Update13");

    PERFORM_NAME_BEGIN("BattleUnit::Update14");
    // 关卡是否使用AI
    if(GetScene() && GetScene()->IsCanUseAI() && IsAIRun())
    {
        m_AIController->Update(delta_time);           // AI
    }
    PERFORM_NAME_END("BattleUnit::Update14");


    PERFORM_NAME_BEGIN("BattleUnit::Update15");
    // 属性通知器
    m_PropNotifier->Update();

    // HP恢复,MP恢复,能量盾恢复
    m_ReHpHelper->Update();
    m_ReMpHelper->Update();
    m_ReEnergyShieldHelper->Update();

    // 复活
    m_ResurgenceHelper->Update(curr_time);

    PERFORM_NAME_END("BattleUnit::Update15");
}

void BattleUnit::AddBuff(uint32 buff_id, uint32 buff_time, BattleUnit* pCaster, int32 buff_val1, int32 buff_val2)
{
    if(GetCantBuff())
        return;

    m_BuffController->AddBuff(buff_id, buff_time, pCaster, buff_val1, buff_val2);
}

void BattleUnit::DelBuff(uint32 buff_id)
{
    if(GetCantBuff())
        return;

    m_BuffController->DelBuff(buff_id);
}

bool BattleUnit::HasBuff(uint32 buff_id)
{
    return m_BuffController->GetBuff(buff_id);
}

void BattleUnit::ClearAllBuff()
{
    if(GetCantBuff())
        return;

    m_BuffController->ClearAllBuff();
}

bool BattleUnit::UseSkill(uint32 skill_id, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skill_obj_id, uint32 skill_slot)
{
    // 使用技能的状态检测
    if(args.m_SkillMode == SKILL_MODE_SELF)
    {
        if(!CanUseSkill())
        {
            //CnDbg("can't use skill by state %d\n", GetState());
            return false;
        }
    }

    SkillObject* pSkill = m_SkillController->UseSkill(skill_id, target, args, skill_obj_id, skill_slot);
    if (!pSkill)
        return false;

    return true;
}

bool BattleUnit::IsEnemy( BattleUnit* enemy )
{
    Scene* scene = 0;
    Regular* regular = 0;

    if((scene = GetScene()) && (regular = scene->GetRegular()))
    {
        CnAssert(scene);
        CnAssert(regular);
        return regular->IsEnemy(this, enemy);
    }

    CnAssert(false);
    return false;
}

bool BattleUnit::CanSeeing(BattleUnit* SomeOne)
{
    // 如果是敌人，同时又隐身，就无法看见
    if(IsEnemy(SomeOne) && SomeOne->GetHide())
        return false;

    return true;
}

bool BattleUnit::IsImmortal()
{
    // buff无敌判断
    if(m_BuffController->IsImmortal())
        return true;

    return false;
}

bool BattleUnit::CanAttack(BattleUnit* enemy)
{
    if(!enemy->GetAttackable())      // 敌人不能被攻击
        return false;

    if(!IsEnemy(enemy))     // 非敌人不能攻击
        return false;

    if(enemy->IsDead())     // 敌人死了不能攻击
        return false;

    if(!CanSeeing(enemy))    // 如果看不见这个敌人，也不能攻击
        return false;

    return true;
}

bool BattleUnit::CanBuff(BattleUnit* SomeOne)
{
    if(SomeOne->IsDead())     // 敌人死了不能上buff
        return false;

    if(SomeOne->GetObjectType() == OBJ_TOWER)  // NPC和塔不能上buff
        return false;

    return true;
}

bool BattleUnit::CanDelBuff(BattleUnit* SomeOne)
{
    if(SomeOne->IsDead())
        return false;

    return true;
}

void BattleUnit::Damage( const DamageResult& damage, BattleUnit* pAttacker, SkillObject* pSkill )
{
    if(IsDead() || isDestory())
    {
        return;
    }

    DamageResult damageResult = damage;

    if(m_NoDamage)
    {
        damageResult.sum_damage = 0;
        damageResult.poison_damage = 0;
    }

    // 伤害通知
    TAKE_DAMAGE_NTF ntf;
    if(pAttacker)
    {
        if(pAttacker->GetObjectType() == OBJ_PET)
        {
            BattleUnit* master = ((Pet*)pAttacker)->GetMaster();
            if(master)
                ntf.attacker_id = master->GetId();
            else
                ntf.attacker_id = 0;
        }
        else
            ntf.attacker_id = pAttacker->GetId();
    }
    else
        ntf.attacker_id = 0;

    ntf.object_id = GetId();

    // step:伤害附加效果
    if(damage.damage_type != DAMAGE_TYPE_BUFF)
    {
        // 眩晕判定
        int32 stunDamage = damageResult.sum_damage + damageResult.poison_damage;
        int32 originMapHp = GetMaxHp();     // GetPropertySetCalculater()->GetBaseProp().GetValue(PROP_MAXHP);  通过当前最大生命做击晕判定
        CnAssert(originMapHp > 0);
        int32 stun_probability = Crown::SDMin(100, 200 * stunDamage / originMapHp);        // 眩晕几率(通过基础生命值判定)
        if(stun_probability > 10 && (int32)Crown::SDRandomRound(100) < stun_probability)
        {   // 触发眩晕
            AddBuff(BUFF_ID_STUN, 500, pAttacker, 0, 0);        // 0.5秒
        }

        // 点燃判定(火焰伤害暴击时触发)
        bool ignite = (damageResult.damageValue.fire_value >= int32(originMapHp*0.05f)) && (damageResult.damage_type == DAMAGE_TYPE_CRITICAL);
        if(ignite)
        {
            AddBuff(BUFF_ID_IGNITE, 4000, pAttacker, int32(damageResult.damageValue.fire_value * 0.2f), 0);     // 点燃4秒
        }

        // 冰缓和冰冻判定
        bool chill = (damageResult.damageValue.ice_value >= int32(originMapHp*0.05f));
        if(chill)
        {
            int32 buffTime = 6000*damageResult.damageValue.ice_value/originMapHp;
            if(buffTime >= 300)         // >=300ms才触发
            {
                if(damageResult.damage_type == DAMAGE_TYPE_CRITICAL)
                {   // 冰冻
                    AddBuff(BUFF_ID_FREEZE, buffTime, pAttacker, 0, 0);
                }
                else
                {   // 冰缓
                    AddBuff(BUFF_ID_CHILL, buffTime, pAttacker, -3000, 0);
                }
            }
        }
        // 感电
        bool shock = (damageResult.damageValue.lightning_value >= int32(originMapHp*0.05f)) && (damageResult.damage_type == DAMAGE_TYPE_CRITICAL);
        if(shock)
        {
            int32 buffTime = 6000*damageResult.damageValue.lightning_value/originMapHp;
            if(buffTime >= 300)         // >=300ms才触发
            {
                AddBuff(BUFF_ID_SHOCK, buffTime, pAttacker, 0, 0);
            }
        }
        // 流血
        if(pSkill)
        {
            uint32 bleedPro = pSkill->GetSkillArgument().m_SkillProps.GetValue(PROP_SKILL_BLEED);
            if(bleedPro > 0 && Crown::SDRandom(BASE_RATE_NUM) <= bleedPro)
            {
                AddBuff(BUFF_ID_BLEED, 5000, pAttacker, damageResult.damageValue.physics_value, 0);     // 5秒
            }
        }
    }


    // 格挡清除伤害
    if (DAMAGE_TYPE_BLOCK == damageResult.damage_type)
    {
        damageResult.sum_damage = 0;
        damageResult.poison_damage = 0;
    }

    int32 allDamage = damageResult.sum_damage + damageResult.poison_damage;

    // 受到伤害通知
    if(allDamage > 0 ||
       DAMAGE_TYPE_MISS == damageResult.damage_type ||
       DAMAGE_TYPE_BLOCK == damageResult.damage_type ||
       DAMAGE_TYPE_IMMORTAL == damageResult.damage_type)
    {
        ntf.damage_info.type = damageResult.damage_type;
        ntf.damage_info.damage = allDamage;
        GetScene()->NotifyMessage(this, MSG_TAKE_DAMAGE_NTF, &ntf);
    }

    // 伤害发送事件
    if(m_communicator)
    {
        m_communicator->OnDamage(pAttacker, allDamage);
    }

    GetScene()->SendEvent(0, BATTLE_EVENT_DAMAGE, (pAttacker ? pAttacker->GetId() : 0), allDamage, 0);
    GetScene()->SendEvent(GetId(), BATTLE_EVENT_SELF_HURT, (pAttacker ? pAttacker->GetId() : 0), allDamage, 0);

    // 攻击者可能是空
    if (pAttacker)
    {
        // 伤害输出记录
        if(pAttacker->IsKindOf(&Player::ms_RTTI))
        {
            Player* playerAttacker = CnDynamicCast(Player, pAttacker);
            playerAttacker->GetBattleRecorder()->DoDamage(this, allDamage);
        }
        else if(pAttacker->IsKindOf(&Pet::ms_RTTI))
        {
            Pet* pet = CnDynamicCast(Pet, pAttacker);
            BattleUnit* master = pet->GetMaster();
            if(master && master->IsKindOf(&Player::ms_RTTI))
            {
                Player* playerMaster = CnDynamicCast(Player, master);
                playerMaster->GetBattleRecorder()->DoDamage(this, allDamage);
            }
        }
    }

    // 伤害中断触摸读条
    if(GetState() == STATE_TOUCH)
    {
        GetActionController()->Stop();
    }

    m_ReEnergyShieldHelper->OnDamage();

    if(allDamage <= 0)
    {
        return;
    }

    // step:生命和能量盾扣除


    // 先从能量盾扣除
    if(damageResult.sum_damage >= GetEnergyShield())
    {
        damageResult.sum_damage -= GetEnergyShield();
        SetEnergyShield(0);
    }
    else
    {
        SetEnergyShield(GetEnergyShield() - damageResult.sum_damage);
        damageResult.sum_damage = 0;
    }

    allDamage = damageResult.sum_damage + damageResult.poison_damage;
    if(allDamage <= 0)
    {
        return;
    }

    // 再从生命值扣除
    if(allDamage > GetHP())
    {
        allDamage = GetHP();
    }

    // 死亡判断
    int32 hp = GetHP() - allDamage;
    if(hp <= 0)
    {   // 死亡
        SetHP(0);

        // 死亡前处理
        if(BeforeDead())
        {
            // 死亡处理
            if(GetBuffController()->CanDead())      // buff系统对死亡的处理(复活buff等)
            {
                Dead(pAttacker, pSkill);
                if(m_communicator)
                {
                    CnDbg("BattleUnit::Damage, communicator attacker:%u\n", pAttacker);
                    m_communicator->OnDead(pAttacker);
                }
            }
        }
        return;
    }
  
    SetHP(hp);

    // 伤害处理
    OnDamage(pAttacker, allDamage);

}

void BattleUnit::Dead(BattleUnit* pAttacker, SkillObject* pSkill)
{
    // 死亡处理回调
    OnDead(pAttacker);

    // 死亡通知
    DEAD_NTF ntf;
    ntf.object_id = GetId();
    ntf.killer_id = (pAttacker ? pAttacker->GetId() : 0);
    ntf.skill_id = (pSkill ? pSkill->GetSkillId() : 0);
    ntf.skill_obj_id = (pSkill ? pSkill->GetSkillObjectID() : 0);
    GetScene()->NotifyMessage(this, MSG_DEAD_NTF, &ntf);
}

void BattleUnit::Kill(BattleUnit* killer)
{
    DamageResult damageResult;
    damageResult.damage_type = 0;
    damageResult.poison_damage = GetHP();
    Damage(damageResult, killer);
}

void BattleUnit::Heal(int32 heal, BattleUnit* pAttacker)
{
    if(GetCantHeal())
        return;

    int32 hp = 0;
    hp = GetHP() + heal;
    // 治疗通知
    TAKE_HEAL_NTF ntf;
    ntf.caster_id = (pAttacker ? pAttacker->GetId() : 0);
    ntf.object_id = GetId();
    ntf.heal = heal;
    GetScene()->NotifyMessage(this, MSG_TAKE_HEAL_NTF, &ntf);
    SetHP(hp);
}

void BattleUnit::Mana(int32 mana, BattleUnit* pAttacker)
{
    int32 mp = 0;
    mp = GetMP() + mana;
    // 魔法回复通知
    TAKE_HEAL_NTF ntf;
    ntf.caster_id = (pAttacker ? pAttacker->GetId() : 0);
    ntf.object_id = GetId();
    ntf.heal = mana;
    GetScene()->NotifyMessage(this, MSG_TAKE_MANA_NTF, &ntf);
    SetMP(mp);
}

void BattleUnit::OnDamage(BattleUnit* pAttacker, int32 damage)
{
}

void BattleUnit::OnKill(BattleUnit* sufferer)
{
    if (!sufferer)
    {
        return;
    }

    Monster* pMonster = CnDynamicCast(Monster, sufferer);
    if (!pMonster)
    {
        return;
    }

    // 木桶不触发
    if (IsBarrel(pMonster->GetCreatureId()))
    {
        return;
    }

    const PropertySet& prop = GetPropertySetCalculater()->GetFinalProp();

    int32 kill_hp_rec = prop.GetValue(PROP_KILL_HP_REC);
    if (kill_hp_rec > 0)
    {
        Heal(kill_hp_rec, NULL);
    }

    int32 kill_mp_rec = prop.GetValue(PROP_KILL_MP_REC);
    if (kill_mp_rec > 0)
    {
        Mana(kill_mp_rec, NULL);
    }
}

// 死亡处理
void BattleUnit::OnDead(BattleUnit* killer)
{
    // 进入死亡状态
    GetStateController()->ChangeState(STATE_DIE);

    // 杀死处理回调
    if(killer)
    {
        killer->OnKill(this);
    }

    // 清除所有buff
    GetBuffController()->ClearAllBuff();

    // 清除所有技能CD
    GetSkillController()->ClearCD();
    GetSkillController()->StopAllSkill();

    // 通知宠物们，主人死亡
    GetPetController()->OnDead(killer);

    // 关卡逻辑，死亡处理
    GetScene()->GetRegular()->OnDead(killer, this);

    // 发出死亡事件
    GetScene()->SendEvent(0, SCENE_EVENT_DEAD, GetId(), (killer ? killer->GetId() : 0), 0 );
}
// 销毁处理
void BattleUnit::OnDestory()
{
    GetPetController()->OnDestory();   // 告诉宠物们，主人不在了
}

void BattleUnit::OnResurgence()
{

}

bool BattleUnit::CheckCollsion(int32 x, int32 y, CoDynamicObject** exclude, int32 excludeLen)
{
    CoWorld* pCoWorld = GetScene()->GetCoWorld();

    real fixRadius = GetRadius() * 0.5f;

    if(pCoWorld->TestCircleWithTriangles(CoVec2(x,y), fixRadius))
        return false;

    if(!m_NoCollisionWithObject)
    {
        if(pCoWorld->TestCircleWithObjects(CoVec2(x,y), (real)GetRadius(), exclude, excludeLen))
            return false;
    }

    return true;
}

bool BattleUnit::CheckPosition(int32 x, int32 y)
{
    CoWorld* pCoWorld = GetScene()->GetCoWorld();

    real fixRadius = GetRadius() * 0.5f;

    if(pCoWorld->TestCircleWithTriangles(CoVec2(x,y), fixRadius))
        return false;

    return true;
}

bool BattleUnit::CheckPostionNoRadius(int32 x, int32 y)
{
    CoWorld* pCoWorld = GetScene()->GetCoWorld();
    if(pCoWorld->TestCircleWithTriangles(CoVec2(x,y), 0))
        return false;
    return true;
}

void BattleUnit::SetGroup(int32 group)
{
    m_group = group;
    m_PropNotifier->NotifyProperty(PROP_GROUP, m_group);
}

void BattleUnit::SetTeamID(uint64 id)
{
    m_TeamID_H = U64ID_HIGH(id);
    m_TeamID_L = U64ID_LOW(id);

    m_PropNotifier->NotifyProperty(PROP_TEAM_ID_H, m_TeamID_H);
    m_PropNotifier->NotifyProperty(PROP_TEAM_ID_L, m_TeamID_L);

    // 宠物的队伍ID和主人队伍ID一致
    m_PetController->SetTeamID(id);
}

uint64  BattleUnit::GetTeamID()
{
    return U64ID(m_TeamID_H, m_TeamID_L);
}

void BattleUnit::SetShowFlag(uint32 showFlag)
{
    m_ShowFlag = showFlag;
    m_PropNotifier->NotifyProperty(PROP_SHOW_FLAG, m_ShowFlag);
}

void BattleUnit::SetAttackable(bool attackable)
{
    m_Attackable = attackable;
    UpdateAttackable();
}

bool BattleUnit::GetAttackable()
{
    return m_FinalAttackable;
}

void BattleUnit::UpdateAttackable()
{
    bool finalAttackable = m_Attackable && m_StateController->GetAttackable();
    if(finalAttackable == m_FinalAttackable)
        return;
    
    m_FinalAttackable = finalAttackable;
    m_PropNotifier->NotifyProperty(PROP_ATTACKABLE, m_FinalAttackable);
}

void BattleUnit::SetCommunicator(BattleUnitCommunicator* pCommunicator)
{
    if(pCommunicator)
        CnAssert(!m_communicator);
    else
        CnAssert(m_communicator);

    m_communicator = pCommunicator;
}

int32 BattleUnit::GetProp(uint32 type)
{
    switch(type)
    {
        // 基础伤害
    case PROP_PHYSICS_DAMAGE:
        return GetBaseDamage();
        // 最大生命
    case PROP_MAXHP:
        return GetMaxHp();
        // 最大魔法
    case PROP_MAXMP:
        return GetMaxMp();
        // 最大能量盾
    case PROP_MAX_ENERGY_SHIELD:
        return GetMaxEnergyShield();
        // 生命值恢复
    case PROP_HP_REC_RATE:
        return GetHpRecovery();
        // 魔法值恢复
    case PROP_MP_REC_RATE:
        return GetMpRecovery();
        // 护甲
    case PROP_ARMOR:
        return GetArmor();
        // 闪避
    case PROP_DODGE:
        return GetDodge();
        // 命中
    case PROP_HIT_RATE:
        return GetHit();
        // 暴击
    case PROP_CRITICAL_ATTACK:
        return GetCrit();
        // 暴击伤害
    case PROP_CRITICAL_DAMAGE_ADDITION:
        return GetCritDamage();
        // 攻击速度
    case PROP_ATTACK_PER_SEC:
        return GetAttackSpeed();
        // 施法速度
    case PROP_CAST_PER_SEC:
        return GetCastSpeed();
        // 移动速度
    case PROP_MOVE_SPEED:
        return GetMoveSpeed();
        // 火炕
    case PROP_FIRE_DEFENCE:
        return GetFireResistance();
        // 冰抗
    case PROP_ICE_DEFENCE:
        return GetIceResistance();
        // 雷抗
    case PROP_THUNDER_DEFENCE:
        return GetLightningResistance();
        // 攻击格挡
    case PROP_BLOCK:
        return GetAttackBlock();
        // 法术格挡
    case PROP_MAGIC_BLOCK:
        return GetMagicBlock();
        // 最大火抗
    case PROP_MAX_FIRE_DEFENCE:
        return GetMaxFireResistance();
        // 最大冰抗
    case PROP_MAX_ICE_DEFENCE:
        return GetMaxIceResistance();
        // 最大雷抗
    case PROP_MAX_LIGHTNING_DEFENCE:
        return GetMaxLightningResistance();
        // 最大攻击格挡
    case PROP_MAX_ATTACK_BLOCK:
        return GetMaxAttackBlock();
        // 最大法术格挡
    case PROP_MAX_MAGIC_BLOCK:
        return GetMaxMagicBlock();
    default:
        CnAssert(false);
        break;
    }

    return 0;
}

void BattleUnit::SetProp(uint32 type, int32 val)
{
    switch(type)
    {
    case PROP_PHYSICS_DAMAGE:
        SetAttack(val);
        break;
        // 最大生命
    case PROP_MAXHP:
        SetMaxHp(val);
        break;
        // 最大魔法
    case PROP_MAXMP:
        SetMaxMp(val);
        break;
        // 最大能量盾
    case PROP_MAX_ENERGY_SHIELD:
        SetMaxEnergyShield(val);
        break;
        // 生命值恢复
    case PROP_HP_REC_RATE:
        SetHpRecovery(val);
        break;
        // 魔法值恢复
    case PROP_MP_REC_RATE:
        SetMpRecovery(val);
        break;
        // 护甲
    case PROP_ARMOR:
        SetArmor(val);
        break;
        // 闪避
    case PROP_DODGE:
        SetDodge(val);
        break;
        // 命中
    case PROP_HIT_RATE:
        SetHit(val);
        break;
        // 暴击
    case PROP_CRITICAL_ATTACK:
        SetCrit(val);
        break;
        // 暴击伤害
    case PROP_CRITICAL_DAMAGE_ADDITION:
        SetCritDamage(val);
        break;
        // 攻击速度
    case PROP_ATTACK_PER_SEC:
        SetAttackSpeed(val);
        break;
        // 施法速度
    case PROP_CAST_PER_SEC:
        SetCastSpeed(val);
        break;
        // 移动速度
    case PROP_MOVE_SPEED:
        SetMoveSpeed(val);
        break;
        // 火抗
    case PROP_FIRE_DEFENCE:
        SetFireResistance(val);
        break;
        // 冰抗
    case PROP_ICE_DEFENCE:
        SetIceResistance(val);
        break;
        // 雷抗
    case PROP_THUNDER_DEFENCE:
        SetLightningResistance(val);
        break;
        // 攻击格挡
    case PROP_BLOCK:
        SetAttackBlock(val);
        break;
        // 法术格挡
    case PROP_MAGIC_BLOCK:
        SetMagicBlock(val);
        break;
        // 最大火抗
    case PROP_MAX_FIRE_DEFENCE:
        SetMaxFireResistance(val);
        break;
        // 最大冰抗
    case PROP_MAX_ICE_DEFENCE:
        SetMaxIceResistance(val);
        break;
        // 最大雷抗
    case PROP_MAX_LIGHTNING_DEFENCE:
        SetMaxLightningResistance(val);
        break;
        // 最大攻击格挡
    case PROP_MAX_ATTACK_BLOCK:
        SetMaxAttackBlock(val);
        break;
        // 最大法术格挡
    case PROP_MAX_MAGIC_BLOCK:
        SetMaxMagicBlock(val);
        break;
    default:
        break;
    }
}

void BattleUnit::CalcProperty()
{
    // 计算并设置buff提供的属性
    m_BuffController->CalcProperty();

    // 计算地图提供的属性

    int32 oldHp = GetHP();
    int32 oldMp = GetMP();
    int32 oldEnergyShield = GetEnergyShield();

    // 清空属性值
    ClearProperty();
    // 角色总属性汇总
    GetPropertySetCalculater()->CalcFinalProp();


    // 属性更新后处理
    if(oldHp > GetMaxHp())
        SetHP(GetMaxHp());
    else
        SetHP(oldHp);

    if(oldMp > GetMaxMp())
        SetMP(GetMaxMp());
    else
        SetMP(oldMp);

    if(oldEnergyShield > GetMaxEnergyShield())
        SetEnergyShield(GetMaxEnergyShield());
    else
        SetEnergyShield(oldEnergyShield);
    
    if(GetFireResistance() > GetMaxFireResistance())
        SetFireResistance(GetMaxFireResistance());
    if(GetIceResistance() > GetMaxIceResistance())
        SetIceResistance(GetMaxIceResistance());
    if(GetLightningResistance() > GetMaxLightningResistance())
        SetLightningResistance(GetMaxLightningResistance());
    if(GetAttackBlock() > GetMaxAttackBlock())
        SetAttackBlock(GetMaxAttackBlock());
    if(GetMagicBlock() > GetMaxMagicBlock())
        SetMagicBlock(GetMaxMagicBlock());

}

void BattleUnit::ClearProperty()
{
    ClearHP();
    ClearMP();
    ClearEnergyShield();

    ClearAttack();

    // 基础属性
    ClearMaxHp();
    ClearMaxMp();
    ClearMaxEnergyShield();
    ClearHpRecovery();
    ClearMpRecovery();
    ClearArmor();
    ClearDodge();
    ClearHit();
    ClearCrit();
    ClearCritDamage();
    ClearAttackSpeed();
    ClearCastSpeed();
    ClearMoveSpeed();
    ClearFireResistance();
    ClearIceResistance();
    ClearLightningResistance();
    ClearAttackBlock();
    ClearMagicBlock();

    // 高级属性
    ClearMaxFireResistance();
    ClearMaxIceResistance();
    ClearMaxLightningResistance();
    ClearMaxAttackBlock();
    ClearMaxMagicBlock();
}

int32 BattleUnit::GetHP()
{
    if(m_communicator)
    {
        return m_communicator->GetHp();
    }

    return m_Hp;
}

void BattleUnit::SetHP(int32 hp)
{
    hp = Crown::SDRange(0, hp, GetMaxHp());

    if(m_communicator)
    {
        m_communicator->SetHp(hp);
    }

    m_Hp = hp;

    m_PropNotifier->NotifyProperty(PROP_HP, GetHP());
}

void BattleUnit::AddHP(int32 val)
{
    if(m_communicator)
    {
        m_communicator->SetHp(GetHP() + val);
    }

    SetHP(GetHP() + val);
}

void BattleUnit::SubHP(int32 val)
{
    if(m_communicator)
    {
        m_communicator->SetHp(GetHP() - val);
    }

    SetHP(GetHP() - val);
}

void BattleUnit::SetMP(int32 mp)
{
    mp = Crown::SDRange(0, mp, GetMaxMp());

    m_Mp = mp;

    m_PropNotifier->NotifyProperty(PROP_MP, GetMP());
}

void BattleUnit::SetEnergyShield(int32 val)
{
    val = Crown::SDRange(0, val, GetMaxEnergyShield());

    m_EnergyShield = val;

    m_PropNotifier->NotifyProperty(PROP_ENERGY_SHIELD, GetEnergyShield());
}

int32 BattleUnit::GetMaxHp()
{
    if(m_communicator)
    {
        return m_communicator->GetMaxHp();
    }

    return m_MaxHp;
}

void BattleUnit::SetMaxHp(int32 val)
{
    if(m_communicator)
    {
        m_communicator->SetMaxHp(val);
        return;
    }

    m_MaxHp = val;
    m_PropNotifier->NotifyProperty(PROP_MAXHP, GetMaxHp());
}

void BattleUnit::SetMaxMp(int32 val)
{
    m_MaxMp = val;
    m_PropNotifier->NotifyProperty(PROP_MAXMP, GetMaxMp());
}

void BattleUnit::SetMaxEnergyShield(int32 val)
{
    m_MaxEnergyShield = val;
    m_PropNotifier->NotifyProperty(PROP_MAX_ENERGY_SHIELD, GetMaxEnergyShield());
}

void BattleUnit::SetMoveSpeed(int32 val)
{
    m_MoveSpeed = val;
    m_PropNotifier->NotifyProperty(PROP_MOVE_SPEED, GetMoveSpeed());
}

bool BattleUnit::inCD( uint32 skill )
{
    return m_SkillController->inCD(skill);
}
