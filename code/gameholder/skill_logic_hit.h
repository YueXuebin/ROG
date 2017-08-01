/*
 *	击中的逻辑节点
 */
#ifndef skill_hit_node_h__
#define skill_hit_node_h__

#include "skill_logic_base.h"

const uint32 SHOOT_MAX_NEXT_TARGET = 10;         // 最大再生弹道记录

class SkillObject;
class ShootData;
class SkillShootNode;
class BattleUnit;

class SkillLogicHit : public SkillLogicBase
{
public:
    SkillLogicHit(const SHOOT_TARGET& hitTarget, const HitArgs& args);
    virtual ~SkillLogicHit();
    virtual uint32  GetLogicType() {return SKILL_LOGIC_HIT;}

    virtual void Init(SkillObject* pSkill);

    virtual void Update();
    virtual void OnStateChange( uint32 state ) {}

    int32   GetHitAngle() {return m_HitArgs.m_HitAngle;}
    CoVec2  GetHitPosition() {return m_HitPosition;}

    bool    m_DebugShow;            // 图形化显示用

private:
    void HitOnce();
    void HitOne(BattleUnit* target);

    void CalcHitPosition();

private:
    SHOOT_TARGET    m_HitTarget;      // 命中的目标
    HitArgs         m_HitArgs;

    const SkillHitData* m_HitData;    // 命中配置数据



    //const SkillShootNode*   m_ShootNode;        // 产生此击中的弹道
    //const ShootData*        m_ShootData;

    //uint32  m_ShootIndex;
    //int32   m_ShootLevel;
    //const SHOOT_TARGET      m_HitTarget;

    //int32   m_PrePassTime;      // 上一帧经过的时间(ms)
    int32   m_PassTime;         // 已经经过的时间(ms)

    int32   m_HitTime;          // 击中时间
    int32   m_HitRateTimer;     // 持续伤害计时

    CoVec2  m_HitPosition;      // 击中位置
    CoVec2  m_HitDir;           // 击中范围方向

private:
    ///////////////////////////////////////////////////////////////////////
    // 次级弹道

    void UpdateSubShoot();

    void CastRemainShoot();

    void CastSubShoot(uint32 shootId);

    void AddNextTargetId(uint32 object_id);
    bool HasNextTargetId(uint32 object_id);

    uint32  m_NextTargetIDS[SHOOT_MAX_NEXT_TARGET];
    int32   m_NextTargetCount;
};


#endif // skill_hit_node_h__
