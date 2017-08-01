/*
 *	技能数据结构
 */
#ifndef skill_data_h__
#define skill_data_h__

#include "buff_data.h"
#include "skill_define.h"

// 技能产生buff效果的数据
const uint32    MAX_BUFF_DATA = 3;      // 一个技能产生的最大buff数


/*
 *  技能数据结构描述
 *  一个技能逻辑流程由5段业务逻辑构成
 *  施法(cast)->发射(launch)->弹道(bullet)->命中(hit)->打击(strike)
 *  施法cast: 描述角色自身的状态，使用技能的整个状态过程，这个过程可能会被打断
 *  发射launch: 描述一次攻击行为，发射产生后不会因为角色状态而被打断
 *  弹道bullet: 一个弹道，可能是碰撞性、或指向型
 *  命中hit：弹道到达或击中目标的逻辑，可能是对个体的，也可能是对范围的。可以持续。
 *  打击strike:对每个对象产生的具体效果，如伤害、buff等。
 *  
 */

// 移动数据
class SkillMoveData
{
public:
    int32   m_StartTime;
    // 自身移动行为数据
    int32   m_moveMode;         // 移动行为模式
    int32   m_moveTime;         // 移动时间
    int32   m_moveDistance;     // 移动距离
    int32   m_BounceTime;       // 弹跳时间
    int32   m_BounceHeight;     // 弹跳高度

    SkillMoveData()
    {
        m_StartTime = 0;
        m_moveMode = 0;
        m_moveTime = 0;
        m_moveDistance = 0;
        m_BounceTime = 0;
        m_BounceHeight = 0;
    }

    bool HasMove() const { return m_moveTime != 0 || m_moveDistance != 0 || m_BounceTime != 0 || m_BounceHeight != 0; }
};

// 技能使用数据
class SkillUseData
{
public:
    uint32  m_SkillCD;          // 技能CD(毫秒)
    int32   m_CousumeMp;        // 魔法消耗

    SkillUseData()
    {
        m_SkillCD = 0;
        m_CousumeMp = 0;
    }
};

// 技能目标数据
class SkillTargetData
{
public:
    int32   m_TargetType;       // 目标类型(1 对角色, 2 对位置, 3 对自己)
    int32   m_Range;            // 使用射程

    SkillTargetData()
    {
        m_TargetType = 0;
        m_Range = 0;
    }
};

// 施法数据
class SkillCastData
{
public:
    int32       m_CastTime;         // 施法状态时间
    bool        m_CanBreak;         // 是否可以主动打断
    SkillMoveData    m_MoveData;    // 施法者位移
};

// 发射数据
class SkillLaunchData
{
public:
    uint32  StartTime;              // 发射的开始时间
};

// 弹道数据
class SkillBulletData
{
public:
    // 弹道模式
    int32   m_BulletMode;        // 弹道模式
    SkillTargetData m_BulletTargetData; // 弹道目标类型

    // 弹道参数
    int32   m_ShootTime;        // 弹道时间
    int32   m_ShootSpeed;       // 弹道速度(时间为0时，使用)

    // 碰撞型弹道参数
    int32   m_CollisionTime;    // 再碰撞时间间隔
    int32   m_CollisionCount;   // 碰撞次数
    int32   m_CollisionRadius;  // 碰撞半径
    int32   m_OffsetAngle;      // 弹道偏角
    bool    m_CollisionEndHit;           // 弹道完成时,是否触发一次击中

    SkillBulletData()
    {
        m_BulletMode = 0;
        m_ShootTime = m_ShootSpeed = 0;
        m_CollisionTime = m_CollisionCount = m_CollisionRadius = m_OffsetAngle = 0;
        m_CollisionEndHit = false;

    }
};

// 命中数据
class SkillHitData
{
public:
    int32   m_HitTime;          // 命中效果的持续时间

    bool    m_IsRect;           // 是否为矩形范围
    int32   m_HitAngle;         // 范围角度,宽
    int32   m_HitRange;         // 范围半径,长
    int32   m_HitRate;          // 每多少ms，击中一次(为0则无持续伤害)

    bool    m_canSelf;          // 可以对自己作用
    bool    m_canFriend;        // 可以对盟友作用
    bool    m_canEnemy;         // 可以对敌人作用

    // 打击ID
    uint32  StrikeDataID;
};

// 打击数据
class SkillStrikeData
{
public:
    // 击中效果
    bool    m_hasDamage;        // 是否有伤害
    bool    m_hasHeal;          // 是否有治疗
    bool    m_hasJump;          // 是否自身移动
    bool    m_hasPushEnemy;     // 是否推动敌人
    bool    m_hasSummon;        // 是否召唤
    bool    m_hasTaunt;         // 是否嘲讽

    // 伤害行为数据
    int32    m_Damage;       // 技能攻击力

    // 治疗行为数据
    int32    m_Heal;         // 治疗量

    // 推动敌人行为数据
    int32   m_PushEnemyMode;            // 推动敌人的行为模式
    int32   m_PushEnemyTime;            // 推动敌人时间
    int32   m_PushEnemyDistance;        // 推动敌人距离
    int32   m_PushEnemyBounceTime;      // 推动敌人弹跳时间
    int32   m_PushEnemyBounceHeight;    // 推动敌人弹跳高度

    // 召唤行为数据
    uint32  m_SummonType;               // 召唤的是(0=战宠, 1=怪物)
    uint32  m_SummonMonsterId;          // 召唤怪物id
    uint32  m_SummonMonsterNumber;      // 召唤的数量
    uint32  m_SummonRadius;             // 出召唤怪的半径
    bool    m_SummonMonsterDeadByMaster;    // 召唤的怪随主人死亡
    std::string m_SummonAIName;         // 召唤怪的AI

    // Buff数据
    BuffData    m_Buff[MAX_BUFF_DATA];      // buff数据
    int32    m_BuffTime[MAX_BUFF_DATA];  // buff时间
    int32    m_BuffVal[MAX_BUFF_DATA];   // buff参数
};

class SkillData
{
public:
    uint32  m_SkillId;          // 技能Id
    uint32  m_SkillVersion;     // 技能版本号
    uint32  m_Usage;            // 技能功能类型(0近战型, 1远程型, 2突进型, 3大招型, 4恢复型, 5自身位移, 6推开敌人, 7普攻, 8召唤型)

    SkillUseData    m_UseData;               // 
    SkillTargetData m_TargetData;           // 目标

    SkillCastData      m_CastData;    // 施法数据
    SkillLaunchData    m_LaunchData;  // 发射数据
    SkillBulletData    m_BulletData;  // 弹道数据
    SkillHitData       m_HitData;     // 命中数据
    SkillStrikeData    m_StrikeData;  // 打击数据


    SkillData()
    {
        m_SkillId = 0;
        m_SkillVersion = 0;
        m_Usage = 0;
    }
};






//
//
//class CastShootData
//{
//public:
//    uint32  m_ShoodId;          // 要触发的弹道ID
//    int32   m_DelayTime;        // 延时触发的时间(再生弹道无效)
//};
//
//
//class ShootData
//{
//public:
//    // 弹道ID
//    int32   m_ShootId;          // 弹道ID
//
//    // 弹道模式
//    int32   m_ShootMode;        // 弹道模式
//    int32   m_ShootTargetType;  // 弹道目标类型
//
//    // 碰撞型弹道参数
//    int32   m_CollisionTime;    // 再碰撞时间间隔
//    int32   m_CollisionCount;   // 碰撞次数
//    int32   m_CollisionRadius;  // 碰撞半径
//    int32   m_OffsetAngle;      // 弹道偏角
//    bool    m_CollisionEndHit;           // 弹道完成时,是否触发一次击中
//
//    // 弹道参数
//    int32   m_ShootTime;        // 弹道时间
//    int32   m_ShootSpeed;       // 弹道速度(时间为0时，使用)
//
//    // 击中参数
//    int32   m_HitTime;          // 击中时间(持续施法用)
//
//    bool    m_IsRect;           // 是否为矩形范围
//    int32   m_HitAngle;         // 范围角度,宽
//    int32   m_HitRange;         // 范围半径,长
//    int32   m_HitRate;          // 每多少ms，击中一次(为0则无持续伤害)
//
//    // 击中效果
//    bool    m_hasDamage;        // 是否有伤害
//    bool    m_hasHeal;          // 是否有治疗
//    bool    m_hasJump;          // 是否自身移动
//    bool    m_hasPushEnemy;     // 是否推动敌人
//    bool    m_hasSummon;        // 是否召唤
//    bool    m_hasTaunt;         // 是否嘲讽
//
//    // 再生弹道
//    std::vector<CastShootData>  m_NextShoot;
//
//    // 伤害行为数据
//    LvlValue    m_Damage;       // 技能攻击力
//
//    // 治疗行为数据
//    LvlValue    m_Heal;         // 治疗量
//
//    // 推动敌人行为数据
//    int32   m_PushEnemyMode;            // 推动敌人的行为模式
//    int32   m_PushEnemyTime;            // 推动敌人时间
//    int32   m_PushEnemyDistance;        // 推动敌人距离
//    int32   m_PushEnemyBounceTime;      // 推动敌人弹跳时间
//    int32   m_PushEnemyBounceHeight;    // 推动敌人弹跳高度
//
//    // 召唤行为数据
//    uint32  m_SummonType;               // 召唤的是(0=战宠, 1=怪物)
//    uint32  m_SummonMonsterId;          // 召唤怪物id
//    uint32  m_SummonMonsterNumber;      // 召唤的数量
//    uint32  m_SummonRadius;             // 出召唤怪的半径
//    bool    m_SummonMonsterDeadByMaster;    // 召唤的怪随主人死亡
//    std::string m_SummonAIName;         // 召唤怪的AI
//
//    // Buff数据
//    BuffData    m_Buff[MAX_BUFF_DATA];      // buff数据
//    LvlValue    m_BuffTime[MAX_BUFF_DATA];  // buff时间
//    LvlValue    m_BuffVal[MAX_BUFF_DATA];   // buff参数
//
//    ShootData()
//    {
//        m_CollisionTime = 0;
//        m_CollisionCount = 0;
//        m_CollisionRadius = 0;
//        m_OffsetAngle = 0;
//
//        m_ShootTime = 0;
//        m_ShootSpeed = 0;
//        m_HitTime = 0;
//
//        m_IsRect = false;
//        m_HitAngle = 0;
//        m_HitRange = 0;
//        m_HitRate = 0;
//
//        m_hasDamage = false;
//        m_hasHeal = false;
//        m_hasJump = false;
//        m_hasPushEnemy = false;
//        m_hasSummon = false;
//
//        m_Damage.Clear();
//        m_Heal.Clear();
//
//        for(int32 i=0; i<MAX_BUFF_DATA;i++)
//        {
//            m_BuffTime[i].Clear();
//            m_BuffVal[i].Clear();
//        }
//
//        m_SummonMonsterId = 0;
//        m_SummonMonsterNumber = 0;
//        m_SummonRadius = 0;
//        m_SummonMonsterDeadByMaster = false;
//
//        m_PushEnemyBounceTime = 0;
//        m_PushEnemyBounceHeight = 0;
//
//    }
//};
//
//
//
//
//class AttackSkillData : public SkillData
//{
//public:
//    // 效果对象
//    bool    m_canSelf;          // 可以对自己用
//    bool    m_canFriend;        // 可以对盟友用
//    bool    m_canEnemy;         // 可以对敌人用
//
//    // 三状态
//    int32   m_SingingTime;      // 吟唱时间
//    int32   m_CastTime;         // 施法时间
//    int32   m_HoldingTime;      // 通道时间
//
//    // 三状态位移
//    MoveData    m_SingingMove;  // 吟唱时位移
//    MoveData    m_CastMove;     // 施法时位移
//    MoveData    m_HoldingMove;  // 通道时位移
//
//    // 中断设置
//    bool    m_canHoldingBreak;  // 通道是否可以中断
//    bool    m_canSingingBreak;
//
//    // 三状态的弹道触发
//    std::vector<CastShootData>  m_SingingShoot;
//    std::vector<CastShootData>  m_CastShoot;
//    std::vector<CastShootData>  m_HoldingShoot;
//
//    // 弹道数据
//    std::vector<ShootData>      m_ShootDatas;
//
//    // 死亡推动敌人行为
//    int32   m_DeadPushEnemyMode;            // 死亡推动敌人的行为模式
//    int32   m_DeadPushEnemyTime;            // 死亡推动敌人时间
//    int32   m_DeadPushEnemyDistance;        // 死亡推动敌人距离
//    int32   m_DeadPushEnemyBounceTime;      // 死亡推动敌人弹跳时间
//    int32   m_DeadPushEnemyBounceHeight;    // 死亡推动敌人弹跳高度
//
//    AttackSkillData()
//    {
//        m_canSelf = false;
//        m_canFriend = false;
//        m_canEnemy = false;
//
//        m_NextRange = 0;
//
//        m_canHoldingBreak = false;
//        m_canSingingBreak = false;
//
//        m_SingingShoot.clear();
//        m_CastShoot.clear();
//        m_HoldingShoot.clear();
//
//        m_DeadPushEnemyMode = 0;
//        m_DeadPushEnemyTime = 0;
//        m_DeadPushEnemyDistance = 0;
//        m_DeadPushEnemyBounceTime = 0;
//        m_DeadPushEnemyBounceHeight = 0;
//    }
//};

#endif // skill_data_h__
