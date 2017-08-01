/*
 *	技能定义
 */
#ifndef skill_define_h__
#define skill_define_h__

// 技能模式定义
enum
{
    SKILL_ATTACK3 = 3,          // 新技能模型2
    SKILL_ATTACK4 = 4,          // 新技能模型3(参数化技能系统)
};

// 技能目标定义
enum
{
    SKILL_TARGET_OBJECT = 1,        // 对目标
    SKILL_TARGET_POS,               // 对位置
    SKILL_TARGET_SELF,              // 对自己

};

// 技能模式
enum
{
    SKILL_MODE_SELF = 1,            // 自身使用技能
    //SKILL_MODE_SOLDIER = 2,       // 跟佣兵用技能
    //SKILL_MODE_SCENE = 3,         // 场景技能
    SKILL_MODE_PASSIVE = 4,         // 被动使用技能
};

// 技能逻辑阶段
enum
{
    SKILL_LOGIC_CAST        = 1,    // 施法逻辑
    SKILL_LOGIC_LAUNCH      = 2,    // 发射逻辑
    SKILL_LOGIC_BULLET      = 3,    // 子弹逻辑
    SKILL_LOGIC_HIT         = 4,    // 命中逻辑
    SKILL_LOGIC_STRIKE      = 5     // 打击逻辑
};

// 弹道模式
enum
{
    SKILL_SHOOT_MODE_NONE           = 0,        // 不存在的弹道模式
    SKILL_BULLET_MODE_DIRECT        = 1,        // 指向型
    SKILL_BULLET_MODE_COLLISION     = 2,        // 碰撞型
};

// 技能触发标志位
enum
{
    SKILL_TRIGGER_ENDURANCE_CHARGE      = 0x1,
    SKILL_TRIGGER_FRENZY_CHARGE         = 0x2,
    SKILL_TRIGGER_POWER_CHARGE          = 0x4,
    SKILL_TRIGGER_CRITICAL_EVENT        = 0x8,
    SKILL_TRIGGER_MELEE_EVENT           = 0x10,
    SKILL_TRIGGER_PROJECTILE_EVENT      = 0x20,
};


const uint32 NORMAL_ATTACK_NEAR = 1001;     // 近战普攻
const uint32 NORMAL_ATTACK_FAR = 1002;      // 远程普攻
inline bool IsNormalAttack(uint32 skillId)
{
    return skillId == NORMAL_ATTACK_NEAR || skillId == NORMAL_ATTACK_FAR;
}

const uint32 MAX_TARGET_NUM = 20;       // 最多记录n个被攻击对象


// 技能参数
class SkillArgumentData
{
public:
    uint32  m_SkillMode;            // 自身使用技能、被动触发技能
    int32   m_CastSpeed;            // 施法速度
    int32   m_CastMulti;            // 重复施法次数
    int32   m_BulletMulti;          // 多重弹道数
    int32   m_BulletMultiAngle;     // 多重弹道的间隔角度
    int32   m_BulletRepeat;         // 弹道重复次数
    int32   m_BulletRepeatInterval; // 弹道重复发射的间隔时间(ms)
    int32   m_BulletSpeed;          // 弹道速度
    int32   m_BulletChain;          // 弹射技能次数
    int32   m_BulletChainID;        // 弹射技能ID
    bool    m_HandLeft;             // 左手攻击

    PropertySet m_SkillProps;       // 技能属性条目

    mutable uint32  m_TargetID[MAX_TARGET_NUM];         // 攻击目标记录(技能逻辑会修改此值)
    mutable uint32  m_TargetCount;                      // 攻击目标记录数量(技能逻辑会修改此值)

    mutable uint32  m_KnockBackID[MAX_TARGET_NUM];      // 击退目标记录(技能逻辑会修改此值)
    mutable uint32  m_KnockBackCount;                   // 击退目标记录数量(技能逻辑会修改此值)

    int32   m_Mp;                   // 技能的mp消耗
    bool    m_IsCritical;           // 是否暴击
    uint32  m_SkillLevel;           // 技能等级
    uint32  m_SkillQuality;         // 技能品质

    bool    m_ConsumeSkill;         // 消耗品技能

    SkillArgumentData()
    {
        m_SkillMode = SKILL_MODE_SELF;
        m_CastSpeed = 10000;
        m_CastMulti = 0;
        m_BulletMulti = 0;
        m_BulletMultiAngle = 15;
        m_BulletRepeat = 0;
        m_BulletRepeatInterval = 0;
        m_BulletSpeed = 10000;
        m_BulletChain = 0;
        m_BulletChainID = 0;
        m_HandLeft = false;

        memset(m_TargetID, 0, sizeof(m_TargetID));
        m_TargetCount = 0;
        memset(m_KnockBackID, 0, sizeof(m_KnockBackID));
        m_KnockBackCount = 0;

        m_Mp = 0;
        m_IsCritical = false;
        m_SkillLevel = 1;
        m_SkillQuality = QUALITY_WHITE;

        m_ConsumeSkill = false;
    }

    SKILL_ARGUMETNS toNetArgument() const
    {
        SKILL_ARGUMETNS netArgs;
        netArgs.skill_mode = m_SkillMode;
        netArgs.cast_speed = m_CastSpeed;
        netArgs.cast_multi = m_CastMulti;
        netArgs.bullet_speed = m_BulletSpeed;
        netArgs.bullet_multi = m_BulletMulti;
        netArgs.bullet_multi_angle = m_BulletMultiAngle;
        netArgs.bullet_repeat = m_BulletRepeat;
        netArgs.bullet_repeat_interval = m_BulletRepeatInterval;
        netArgs.bullet_chain = m_BulletChain;
        netArgs.bullet_chain_id = m_BulletChainID;
        netArgs.hand_left = m_HandLeft?1:0;

        return netArgs;
    }

    void fromNetArgument(const SKILL_ARGUMETNS& netArgs)
    {
        m_SkillMode = netArgs.skill_mode;
        m_CastSpeed = netArgs.cast_speed;
        m_CastMulti = netArgs.cast_multi;
        m_BulletSpeed = netArgs.bullet_speed;
        m_BulletMulti = netArgs.bullet_multi;
        m_BulletMultiAngle = netArgs.bullet_multi_angle;
        m_BulletRepeat = netArgs.bullet_repeat;
        m_BulletRepeatInterval = netArgs.bullet_repeat_interval;
        m_BulletChain = netArgs.bullet_chain;
        m_BulletChainID = netArgs.bullet_chain_id;
        m_HandLeft = netArgs.hand_left;
    }

    void AddTargetId(uint32 object_id) const
    {
        if(HasTargetId(object_id))
            return;

        CnAssert(m_TargetID[m_TargetCount] == 0);
        m_TargetID[m_TargetCount] = object_id;
        m_TargetCount++;
    }

    bool HasTargetId(uint32 object_id) const
    {
        if(m_TargetCount == MAX_TARGET_NUM)
            return true;            // 已达最大记录数

        for(uint32 i=0; i<m_TargetCount; i++)
        {
            if(m_TargetID[i] == object_id)
                return true;
        }
        return false;
    }

    void AddKnockBack(uint32 object_id) const
    {
        if(HasKnockBackId(object_id))
            return;

        CnAssert(m_KnockBackID[m_KnockBackCount] == 0);
        m_KnockBackID[m_KnockBackCount] = object_id;
        m_KnockBackCount++;
    }

    bool HasKnockBackId(uint32 object_id) const
    {
        if(m_KnockBackCount == MAX_TARGET_NUM)
            return true;            // 已达最大记录数

        for(uint32 i=0; i<m_KnockBackCount; i++)
        {
            if(m_KnockBackID[i] == object_id)
                return true;
        }
        return false;

    }

};

// 子弹参数
struct BulletArgs
{
    int32   m_StartAngle;       // 弹道偏角
    int32   m_Chain;            // 弹射次数

    BulletArgs() : m_StartAngle(0), m_Chain(0) {}
};

// 命中参数
struct HitArgs
{
    int32   m_HitAngle;         // 命中方向

    HitArgs() : m_HitAngle(0) {}
};

// 打击参数
struct StrikeArgs
{
    int32   m_StrikeAngle;      // 打击方向

    StrikeArgs() : m_StrikeAngle(0) {}
};


#endif // skill_define_h__
