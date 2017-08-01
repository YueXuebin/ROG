#ifndef skill_logic_launch_h__
#define skill_logic_launch_h__

#include "skill_define.h"
#include "skill_logic_base.h"

class SkillLogicLaunch : public SkillLogicBase
{
public:
    SkillLogicLaunch();
    virtual ~SkillLogicLaunch();
    virtual uint32  GetLogicType() {return SKILL_LOGIC_LAUNCH;}

    virtual void Init(SkillObject* pSkill);

    virtual void Update();

    virtual void OnStateChange( uint32 state ) {}


private:
    SHOOT_TARGET CalcBulletTarget();        // 计算弹道目标

private:
    const SkillLaunchData*      m_LaunchData;

    int32   m_BulletRepeatCounter;      // 发射弹道次数
    int32   m_BulletIntervalTimer;      // 弹道间隔计时
};



#endif // skill_logic_launch_h__
