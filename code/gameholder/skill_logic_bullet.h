#ifndef skill_shoot_node_h__
#define skill_shoot_node_h__

#include "skill_data.h"
#include "skill_logic_base.h"
#include "skill_bullet_trajectory_direct.h"
#include "skill_bullet_trajectory_collision.h"

#define     STEP_SHOOT  1
#define     STEP_HIT    2

class SkillObject;
class BattleUnit;


class SkillLogicBullet : public SkillLogicBase
{
    friend class SkillBulletTrajectory_Direct;
    friend class SkillBulletTrajectory_Collision;

public:
    SkillLogicBullet(const SHOOT_TARGET& bulletTarget, const BulletArgs& args);
    virtual ~SkillLogicBullet();
    virtual uint32  GetLogicType() {return SKILL_LOGIC_BULLET;}

    virtual void Init(SkillObject* pSkill, uint32 bulletIndex);

    virtual void Update();

    virtual void OnStateChange( uint32 state ) {}

    uint32 GetBulletIndex() {return m_BulletIndex;}

    const CoVec2&  GetPosition();
    int32   GetAngle();
    int32   GetRadius() {return m_BulletData->m_CollisionRadius;}
    

private:
    void    OnBulletHit(const SHOOT_TARGET& hitTarget, const HitArgs& args);
    void    OnBulletEnd();

    void HitOnce(const SHOOT_TARGET& target);

    // 弹射逻辑
    void    OnBulletChain(const SHOOT_TARGET& hitTarget);
    BattleUnit* FindChainTarget(const SHOOT_TARGET& hitTarget);


private:
    BulletArgs  m_BulletArgs;       // 子弹系数
    uint32      m_BulletIndex;      // 子弹序号

    const SkillBulletData* m_BulletData;

    SHOOT_TARGET    m_BulletTarget;      // 弹道的目标

    SkillBulletTrajectory_Direct    m_TrajectoryDirect;     // 指向型轨迹
    SkillBulletTrajectory_Collision m_TrajectoryCollision;  // 碰撞型轨迹

};


#endif // skill_shoot_node_h__
