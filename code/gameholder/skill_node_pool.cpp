#include "gameholder_pch.h"
#include "skill_node_pool.h"


SkillNodePool::SkillNodePool()
{
    m_CastPool = CnNew Crown::CSDObjectPool<SkillLogicCast>(200, 200);
    m_LaunchPool = CnNew Crown::CSDObjectPool<SkillLogicLaunch>(200, 200);
    m_BulletPool = CnNew Crown::CSDObjectPool<SkillLogicBullet>(200, 200);
    m_HitPool = CnNew Crown::CSDObjectPool<SkillLogicHit>(200, 200);
    m_StrikePool = CnNew Crown::CSDObjectPool<SkillLogicStrike>(200, 200);

}

SkillNodePool::~SkillNodePool()
{
    CnDelete m_CastPool;
    CnDelete m_LaunchPool;
    CnDelete m_BulletPool;
    CnDelete m_HitPool;
    CnDelete m_StrikePool;
}


SkillLogicCast* SkillNodePool::CreateLogicCastNode()
{
    SkillLogicCast* pLogicCast = m_CastPool->Alloc();
    return pLogicCast;
}

SkillLogicLaunch* SkillNodePool::CreateLogicLaunchNode()
{
    SkillLogicLaunch* pLogicLaunch = m_LaunchPool->Alloc();
    return pLogicLaunch;
}

SkillLogicBullet* SkillNodePool::CreateLogicBulletNode(const SHOOT_TARGET& bulletTarget, const BulletArgs& args)
{
    SkillLogicBullet* pLogicBullet = m_BulletPool->Alloc(bulletTarget, args);
    return pLogicBullet;
}

SkillLogicHit* SkillNodePool::CreateLogicHitNode(const SHOOT_TARGET& hitTarget, const HitArgs& args)
{
    SkillLogicHit* pLogicHit = m_HitPool->Alloc(hitTarget, args);
    return pLogicHit;
}

SkillLogicStrike* SkillNodePool::CreateLogicStrikeNode(const SHOOT_TARGET& strikeTarget, const StrikeArgs& args)
{
    SkillLogicStrike* pLogicStrike = m_StrikePool->Alloc(strikeTarget, args);
    return pLogicStrike;
}

void SkillNodePool::DestorySkillNode(SkillLogicBase* skillNode)
{
    switch(skillNode->GetLogicType())
    {
    case SKILL_LOGIC_CAST:
        m_CastPool->Free((SkillLogicCast*)skillNode);
        break;
    case SKILL_LOGIC_LAUNCH:
        m_LaunchPool->Free((SkillLogicLaunch*)skillNode);
        break;
    case SKILL_LOGIC_BULLET:
        m_BulletPool->Free((SkillLogicBullet*)skillNode);
        break;
    case SKILL_LOGIC_HIT:
        m_HitPool->Free((SkillLogicHit*)skillNode);
        break;
    case SKILL_LOGIC_STRIKE:
        m_StrikePool->Free((SkillLogicStrike*)skillNode);
        break;
    default:
        CnAssert(false);
        break;
    }
}