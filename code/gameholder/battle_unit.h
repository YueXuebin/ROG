/*
 *	战斗单位(虚类)
 */
#ifndef battle_unit_h__
#define battle_unit_h__

#include "sceneobject.h"
#include "formula_object.h"
#include "skill_define.h"
#include "battle_define.h"

class StateController;
class SkillController;
class BuffController;
class AuraController;
class ActionController;
class AIController;
class PropNotifier;
class PetController;
class SkillObject;
class ReHpHelper;
class ReMpHelper;
class ReEnergyShieldHelper;
class BattleUnitCommunicator;
class TouchController;
class ResurgenceHelper;
class PropertySetCalculater;
class Pet;

typedef std::vector<Pet*> PetList;

class BattleUnit : public SceneObject, public FormulaObject
{
    CnDeclareRTTI
public:
    BattleUnit(uint32 objType);
    virtual ~BattleUnit();

    void    BattleUnitClear();

    void    SetGroup(int32 group);
    uint32  GetGroup() {return m_group;}

    void    SetShowFlag(uint32 showFlag);
    uint32  GetShowFlag() {return m_ShowFlag;}

    uint64  GetTeamID();
    void    SetTeamID(uint64 id);

    void    SetLevel(int32 val){ m_level = val; }
    int32   GetLevel() const { return m_level; }

    uint32  GetState();

    bool    CanChangePostion();
    bool    CanUseSkill();
    bool    CanChangeTo(uint32 state);
    void    ChangeState(uint32 state);
    
    virtual void OnStateChange(uint32 oldState, uint32 newState);        // 状态改变回调

    virtual void UpdateB2PhysicsCategoryAndMask();

    uint32  GetAIState();

    //--------------------------------------
    ActionController*   GetActionController() {return m_ActionController;}
    AIController*       GetAIController() {return m_AIController;}
    StateController*    GetStateController() {return m_StateController;}
    BuffController*     GetBuffController() {return m_BuffController;}
    PetController*      GetPetController() {return m_PetController;}
    SkillController*    GetSkillController() {return m_SkillController;}
    TouchController*    GetTouchController() {return m_TouchController;}
    PropertySetCalculater*  GetPropertySetCalculater() {return m_PropertySetCalculater;}  // 属性集合计算器
    //--------------------------------------
    PropNotifier*       GetPropNotifier() {return m_PropNotifier;}

    ResurgenceHelper*   GetResurgenceHelper() {return m_ResurgenceHelper;}

    virtual void onEnterScene(Scene* pScene);
    virtual void onLeaveScene(Scene* pScene);

    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

    virtual void Update(uint32 frame_count);

    virtual bool UseSkill(uint32 skill_id, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skill_obj_id, uint32 skill_slot);

    virtual bool IsAIRun() {return true;}       // 是否执行AI

    virtual PetList* GetPetList() {return NULL;} 

    void    AddBuff(uint32 buff_id, uint32 buff_time, BattleUnit* pCaster, int32 buff_val1, int32 buff_val2);     // 添加buff
    void    DelBuff(uint32 buff_id);                                       // 删除buff
    bool    HasBuff(uint32 buff_id);
    void    ClearAllBuff();                                   // 清除所有BUFF

    bool    CheckPosition(int32 x, int32 y);            // 检查当前位置是否合法(不在墙里面)
    bool    CheckPostionNoRadius(int32 x, int32 y);     // 检查当前位置是否合法(不半径)
    bool    CheckCollsion(int32 x, int32 y, CoDynamicObject** exclude, int32 excludeLen);

    virtual void Damage(const DamageResult& damage, BattleUnit* pAttacker, SkillObject* pSkill = NULL);           // 受到伤害
    void Heal(int32 heal, BattleUnit* pAttacker);                                           // 受到治疗
    void Mana(int32 mana, BattleUnit* pAttacker);                                           // 受到魔法回复
    void Dead(BattleUnit* pAttacker, SkillObject* pSkill);

    virtual bool BeforeDead() {return true;}                        // 死亡之前的特殊处理

    virtual void OnDamage(BattleUnit* pAttacker, int32 damage);     // 伤害处理
    virtual void OnKill(BattleUnit* sufferer);                      // 杀死处理(我杀死了谁)
    virtual void OnDead(BattleUnit* killer);                        // 死亡处理(谁杀死了它)
    virtual void OnDestory();                                       // 销毁处理
    virtual void OnResurgence();                                    // 复活处理
    void    Kill(BattleUnit* killer);                               // 杀死它

    bool    IsDead();
    bool    IsEnemy(BattleUnit* enemy);             // 是否是敌人
    bool    IsImmortal();                           // 是否无敌
    bool    CanSeeing(BattleUnit* SomeOne);         // 是否能看到
    bool    CanAttack(BattleUnit* enemy);           // 是否能攻击
    bool    CanBuff(BattleUnit* SomeOne);           // 是否能上Buff
    bool    CanDelBuff(BattleUnit* SomeOne);        // 是否能删除Buff
    void    SetNoDamage(bool noDamage) {m_NoDamage = noDamage;}
    void    SetAttackable(bool attackable);
    bool    GetAttackable();
    void    UpdateAttackable();
    void    SetNoCollisionObject(bool c) {m_NoCollisionWithObject = c;}     // 设置对象无碰撞
    bool    GetNoCollisionObject() {return m_NoCollisionWithObject;}

    void    SetCommunicator(BattleUnitCommunicator* pCommunicator);
    BattleUnitCommunicator* GetCommunicator() {return m_communicator;}

    //------------------------------------------------------------------------------
    // 战斗单位属性
    //------------------------------------------------------------------------------
    virtual int32   GetProp(uint32 type);                                       // 获得属性
    virtual void    SetProp(uint32 type, int32 val);                            // 设置属性

    virtual void    CalcProperty();
    virtual void    ClearProperty();

    // ------------------------------------------------------------------------------
    // 基础属性
    // 全局通知属性(生命,最大生命,魔法,最大魔法,能量盾,最大能量盾,移动速度)
    // ------------------------------------------------------------------------------

    // 生命值(全局通知)
    int32   GetHP();
    virtual void    SetHP(int32 val);
    void    AddHP(int32 val);
    void    SubHP(int32 val);
    void    ClearHP() {m_Hp = 0;}

    // 魔法值(全局通知)
    int32   GetMP() {return m_Mp;}
    virtual void    SetMP(int32 val);
    void    AddMP(int32 val) {SetMP(GetMP() + val);}
    void    SubMP(int32 val) {SetMP(GetMP() - val);}
    void    ClearMP() {m_Mp = 0;}

    // 能量盾(全局通知)
    int32   GetEnergyShield() {return m_EnergyShield;}
    virtual void    SetEnergyShield(int32 val);
    void    AddEnergyShield(int32 val) {SetEnergyShield(GetEnergyShield() + val);}
    void    SubEnergyShield(int32 val) {SetEnergyShield(GetEnergyShield() - val);}
    void    ClearEnergyShield() {m_EnergyShield = 0;}

    // 最大生命值(全局通知)
    int32   GetMaxHp();
    virtual void    SetMaxHp(int32 val);
    void    ClearMaxHp() {m_MaxHp = 0;}

    // 最大魔法值(全局通知)
    int32   GetMaxMp() {return m_MaxMp;}
    virtual void    SetMaxMp(int32 val);
    void    ClearMaxMp() {m_MaxMp = 0;}

    // 最大能量盾(全局通知)
    int32   GetMaxEnergyShield() {return m_MaxEnergyShield;}
    virtual void    SetMaxEnergyShield(int32 val);
    void    ClearMaxEnergyShield() {m_MaxEnergyShield = 0;}

    // 移动速度(全局通知)
    uint32  GetMoveSpeed() {return m_MoveSpeed;}
    virtual void    SetMoveSpeed(int32 val);
    void    ClearMoveSpeed() {m_MoveSpeed = 0;}

    // 生命恢复
    int32   GetHpRecovery() {return m_HpRecovery;}
    void    SetHpRecovery(int32 val) {m_HpRecovery = val;}
    void    ClearHpRecovery() {m_HpRecovery = 0;}

    // 魔法恢复
    int32   GetMpRecovery() {return m_MpRecovery;}
    void    SetMpRecovery(int32 val) {m_MpRecovery = val;}
    void    ClearMpRecovery() {m_MpRecovery = 0;}

    // 基础伤害
    int32   GetBaseDamage() {return m_BaseDamage;}
    void    SetAttack(int32 val) {m_BaseDamage = val;}
    void    ClearAttack() { m_BaseDamage = 0; }

    // 护甲
    int32   GetArmor() {return m_Armor;}
    void    SetArmor(int32 val) {m_Armor = val;}
    void    ClearArmor() {m_Armor = 0;}

    // 闪避
    int32   GetDodge() { return m_Dodge; }
    void    SetDodge(int32 val) { m_Dodge = val; }
    void    ClearDodge() { m_Dodge = 0; }

    // 命中
    int32   GetHit() { return m_Hit; }
    void    SetHit(int32 val) { m_Hit = val; }
    void    ClearHit() { m_Hit = 0; }

    // 暴击
    int32   GetCrit() { return m_Crit; }
    void    SetCrit(int32 val) { m_Crit = val; }
    void    ClearCrit() { m_Crit = 0; }

    // 暴伤
    int32   GetCritDamage() { return m_CritDamage; }
    void    SetCritDamage(int32 val) { m_CritDamage =  val; }
    void    ClearCritDamage() { m_CritDamage = 0; }

    // 攻击速度属性相关
    int32   GetAttackSpeed() {return m_AttackSpeed;}
    void    SetAttackSpeed(int32 val) {m_AttackSpeed = val;}
    void    ClearAttackSpeed() { m_AttackSpeed = 0; }

    // 施法速度属性相关
    int32   GetCastSpeed() {return m_CastSpeed;}
    void    SetCastSpeed(int32 val) {m_CastSpeed = val;}
    void    ClearCastSpeed() { m_CastSpeed = 0; }

    // 火焰抗性
    int32   GetFireResistance() {return m_FireResistance;}
    void    SetFireResistance(int32 val) {m_FireResistance = val;}
    void    ClearFireResistance() { m_FireResistance = 0; }

    // 冰霜抗性
    int32   GetIceResistance() {return m_IceResistance;}
    void    SetIceResistance(int32 val) {m_IceResistance = val;}
    void    ClearIceResistance() { m_IceResistance = 0; }

    // 闪电抗性
    int32   GetLightningResistance() {return m_LightningResistance;}
    void    SetLightningResistance(int32 val) {m_LightningResistance = val;}
    void    ClearLightningResistance() { m_LightningResistance = 0; }

    // 攻击格挡
    int32   GetAttackBlock() {return m_AttackBlock;}
    void    SetAttackBlock(int32 val) {m_AttackBlock = val;}
    void    ClearAttackBlock() { m_AttackBlock = 0; }

    // 法术格挡
    int32   GetMagicBlock() {return m_MagicBlock;}
    void    SetMagicBlock(int32 val) {m_MagicBlock = val;}
    void    ClearMagicBlock() { m_MagicBlock = 0; }

    // ------------------------------------------------------------------------------

    // 最大火焰抗性
    int32   GetMaxFireResistance() {return m_MaxFireResistance;}
    void    SetMaxFireResistance(int32 val) {m_MaxFireResistance = val;}
    void    ClearMaxFireResistance() {m_MaxFireResistance = 0;}

    // 最大冰霜抗性
    int32   GetMaxIceResistance() {return m_MaxIceResistance;}
    void    SetMaxIceResistance(int32 val) {m_MaxIceResistance = val;}
    void    ClearMaxIceResistance() {m_MaxIceResistance = 0;}

    // 最大闪电抗性
    int32   GetMaxLightningResistance() {return m_MaxLightningResistance;}
    void    SetMaxLightningResistance(int32 val) {m_MaxLightningResistance = val;}
    void    ClearMaxLightningResistance() {m_MaxLightningResistance = 0;}

    // 最大攻击格挡
    int32   GetMaxAttackBlock() {return m_MaxAttackBlock;}
    void    SetMaxAttackBlock(int32 val) {m_MaxAttackBlock = val;}
    void    ClearMaxAttackBlock() {m_MaxAttackBlock = 0;}

    // 最大攻击格挡
    int32   GetMaxMagicBlock() {return m_MaxMagicBlock;}
    void    SetMaxMagicBlock(int32 val) {m_MaxMagicBlock = val;}
    void    ClearMaxMagicBlock() {m_MaxMagicBlock = 0;}

    // ------------------------------------------------------------------------------
    bool    inCD( uint32 skill );                           // 能否使用技能

protected:
    uint32              m_LastFrameCounter;

    StateController*    m_StateController;          // 状态机
    SkillController*    m_SkillController;          // 技能控制器
    BuffController*     m_BuffController;           // Buff控制器
    AIController*       m_AIController;             // AI控制器(并非所有对象都有)
    ActionController*   m_ActionController;         // 对象控制器
    PropNotifier*       m_PropNotifier;             // 属性通知器
    PetController*      m_PetController;            // 宠物控制器
    TouchController*    m_TouchController;          // 触摸控制器

    BattleUnitCommunicator* m_communicator;         // 属性共享器(世界BOSS)

    PropertySetCalculater*  m_PropertySetCalculater;  // 属性集合计算器

    bool        m_NoDamage;                         // 是否能够受到伤害
    bool        m_NoCollisionWithObject;            // 不和其他对象碰撞

    int32       m_level;                            // 等级

    //////////////////////////////////////////////////////////////////////////////////////
    // 全局通知属性

    uint32      m_group;                            // 组 PROP_GROUP
    uint32      m_ShowFlag;                         // 显示标记 PROP_SHOW_FLAG
    uint32      m_TeamID_L;                         // 队伍ID
    uint32      m_TeamID_H;
    bool        m_Attackable;                       // 是否能够被攻击标记
    bool        m_FinalAttackable;                  // 最终是否能够被攻击 PROP_ATTACKABLE

    int32       m_BaseDamage;                       // 基础伤害 PROP_PHYSICS_DAMAGE
    //-18条基础属性-----------------------------------------------------------------------------------
    int32       m_Hp;                               // 生命 PROP_HP
    int32       m_Mp;                               // 魔法 PROP_MP
    int32       m_EnergyShield;                     // 能量盾 PROP_ENERGY_SHIELD

    int32       m_MaxHp;                            // 最大生命 PROP_MAXHP
    int32       m_MaxMp;                            // 最大魔法 PROP_MAXMP
    int32       m_MaxEnergyShield;                  // 最大能量盾 PROP_MAX_ENERGY_SHIELD
    int32       m_MoveSpeed;                        // 移动速度 PROP_MOVE_SPEED

    int32       m_HpRecovery;                       // 生命回复 PROP_HP_REC_RATE
    int32       m_MpRecovery;                       // 魔法回复 PROP_MP_REC_RATE

    int32       m_Armor;                            // 护甲 PROP_DEFENCE
    int32       m_Dodge;                            // 闪避 PROP_DODGE
    int32       m_Hit;                              // 命中 PROP_HIT_RATE
    int32       m_Crit;                             // 暴击 PROP_CRITICAL_ATTACK
    int32       m_CritDamage;                       // 暴击伤害 PROP_CRITICAL_DAMAGE_ADDITION

    int32       m_AttackSpeed;                      // 攻击速度(每秒攻击次数) PROP_ATTACK_PER_SEC
    int32       m_CastSpeed;                        // 施法速度(每秒施法次数) PROP_CAST_PER_SEC

    int32       m_FireResistance;                   // 火抗 PROP_FIRE_DEFENCE
    int32       m_IceResistance;                  // 冰抗 PROP_ICE_DEFENCE
    int32       m_LightningResistance;              // 雷抗 PROP_THUNDER_DEFENCE

    int32       m_AttackBlock;                      // 攻击格挡 PROP_BLOCK
    int32       m_MagicBlock;                       // 法术格挡 PROP_MAGIC_BLOCK
    //-高级属性----------------------------------------------------------------------------------------
    int32       m_MaxFireResistance;                // 最大火抗
    int32       m_MaxIceResistance;                 // 最大冰抗
    int32       m_MaxLightningResistance;           // 最大雷抗
    int32       m_MaxAttackBlock;                   // 最大攻击格挡
    int32       m_MaxMagicBlock;                    // 最大法术格挡
    



    //-------------------------------------------------------------------------------------------------

    ReHpHelper* m_ReHpHelper;                       // hp回复器
    ReMpHelper* m_ReMpHelper;                       // mp回复器
    ReEnergyShieldHelper*   m_ReEnergyShieldHelper; // 能量盾回复器

    ResurgenceHelper*   m_ResurgenceHelper;         // 复活逻辑
};

#endif // battle_unit_h__
