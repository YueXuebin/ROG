/*
 *	新技能模式
 *  以时间和状态为节点驱动的
 */
#ifndef skill_mode_h__
#define skill_mode_h__

#include "skill_data.h"


class BattleUnit;

class SkillLogicBase;
class SkillLogicCast;
class ShootData;
class SkillLogicLaunch;
class SkillLogicBullet;
class SkillLogicHit;
class SkillLogicStrike;

class SkillObject
{
    friend class SkillLogicBase;
    friend class SkillLogicCast;
    friend class SkillLogicLaunch;
    friend class SkillLogicBullet;
    friend class SkillLogicHit;
    friend class SkillLogicStrike;

public:
    typedef std::vector<SkillLogicBase*>        SkillLogicList;

    SkillObject(const SkillData* pSkillData, BattleUnit* owner, const SkillArgumentData& args);
    virtual ~SkillObject();

    void Update();
    void OnStateChange(uint32 state);

    bool Init(const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skill_slot);

    uint32  GetSkillId() {return m_pSkillData->m_SkillId;}
    uint32  GetSkillSlot() {return m_SkillSlot;}
    BattleUnit* GetOwner() {return m_Owner;}

    void SetSkillObjectID(uint32 objectID) {m_ObjectID = objectID;}
    uint32  GetSkillObjectID() {return m_ObjectID;}

    bool    isEnd() {return m_dead;}
    void    End();

    const SkillData* GetSkillData() {return m_pSkillData;}

    uint32  GetSkillMode() {return m_SkillArgs.m_SkillMode;}
    const SkillArgumentData& GetSkillArgument() {return m_SkillArgs;}

    void SetCalcShootData(const ShootData* shootData) {m_CalcShootData = shootData;}

    const SHOOT_TARGET& GetTarget() {return m_Target;}

    bool CanCast();
    void EnterCast();
    void LeaveCast();

    const SkillLogicList& GetAllLogics() {return m_AllLogics;}

private:
    void BuildCastLogic();               // 构建时间节点

    uint32  GenerateNodeId() { return m_NodeIdCounter++; }

    void BreakOff();                    // 打断技能施法

    bool MakeTarget();
    bool OwnerConsume();

    void NotifyBulletEnd(SkillLogicBullet* bullet);

    SkillLogicCast*     CreateLogicCast();
    SkillLogicLaunch*   CreateLogicLaunch();
    SkillLogicBullet*   CreateLogicBullet(const SHOOT_TARGET& bulletTarget, const BulletArgs& args);
    SkillLogicHit*      CreateLogicHit(const SHOOT_TARGET& hitTarget, const HitArgs& args);
    SkillLogicStrike*   CreateLogicStrike(const SHOOT_TARGET& strikeTarget, const StrikeArgs& args);

    void DestorySkillNode(SkillLogicBase* pSkillLogic);

private:
    BattleUnit* m_Owner;            // 技能的主人
    bool    m_dead;                 // 是否技能结束

    uint32  m_ObjectID;             // 技能id(每角色唯一),从１开始
    uint32  m_SkillSlot;            // 技能的SlotId

    SHOOT_TARGET m_Target;

    int32   m_CastTimeSum;              // 总施法时间

    bool    m_RunOnceUpdate;            // 是否执行过一次Update
    uint32  m_NodeIdCounter;            // node id计数器
    uint32  m_BulletIndexCounter;       // 子弹序号计数器

    SkillLogicList      m_AllLogics;            // 所有的技能逻辑
    
    const SkillData*    m_pSkillData;           // 技能的配置(引用外部数据)
    SkillArgumentData   m_SkillArgs;            // 该技能的参数

    const ShootData*    m_CalcShootData;      // 暂存弹道数据，计算伤害用

    uint32    m_TriggerFlags;               // 技能触发记录
};


#endif // skill_mode_h__
