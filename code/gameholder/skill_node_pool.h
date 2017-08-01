/*
 *	使用池来创建技能逻辑节点
 */
#ifndef skill_node_pool_h__
#define skill_node_pool_h__


#include "skill_logic_cast.h"
#include "skill_logic_launch.h"
#include "skill_logic_bullet.h"
#include "skill_logic_hit.h"
#include "skill_logic_strike.h"


class SkillNodePool
{
public:
    SkillNodePool();
    ~SkillNodePool();

    SkillLogicCast* CreateLogicCastNode();
    SkillLogicLaunch* CreateLogicLaunchNode();
    SkillLogicBullet* CreateLogicBulletNode(const SHOOT_TARGET& bulletTarget, const BulletArgs& args);
    SkillLogicHit* CreateLogicHitNode(const SHOOT_TARGET& hitTarget, const HitArgs& args);
    SkillLogicStrike* CreateLogicStrikeNode(const SHOOT_TARGET& strikeTarget, const StrikeArgs& args);

    void DestorySkillNode(SkillLogicBase* skillNode);

private:
    Crown::CSDObjectPool<SkillLogicCast>*           m_CastPool;
    Crown::CSDObjectPool<SkillLogicLaunch>*         m_LaunchPool;
    Crown::CSDObjectPool<SkillLogicBullet>*         m_BulletPool;
    Crown::CSDObjectPool<SkillLogicHit>*            m_HitPool;
    Crown::CSDObjectPool<SkillLogicStrike>*         m_StrikePool;
};

#endif // skill_node_pool_h__
