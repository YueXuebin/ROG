#ifndef ai_controller_h__
#define ai_controller_h__

#include "ai_define.h"
#include "path_data.h"
#include "ai_skill_data.h"
#include "ai_container.h"

class AIStateBase;
class AIIdle;
class AIAttack;
class BattleUnit;
class AIFactory;
class AttackSkillData;
class Checkpoint;
class Player;
class AIState;
class AIGroup;

/*
 *	AI控制器
 */
class AIController : public AIContainer
{
    friend class AIFactory;
public:
    typedef std::vector<AISkillData>    AISkillList;

    AIController(BattleUnit* owner);
    virtual ~AIController();

    virtual bool LoadAI(const std::string& aiName);
    virtual void ClearAI();

    virtual void Update(uint32 delta_time);

    virtual AIController* GetController() {return this;}

    void RecordAIName(const std::string& aiName);           // 记录包含的AI文件(防止递归)

    BattleUnit* GetOwner() {return m_Owner;}

    void    SetBasePosition(int32 x, int32 y);          // 出身点
    int32   GetBaseX() {return m_BaseX;}
    int32   GetBaseY() {return m_BaseY;}

    void    SetPath(const PathData& pathData);          // 设置路径
    const   PathData& GetPath() const {return m_PathData;}
    void    SetPathLoop(bool isLoop);
    bool    GetPathLoop() {return m_PathLoop;}
    bool    IsPathingEnd();                             // 是否需要寻路
    void    NextPathIndex();
    Checkpoint* GetCurrentCheckPoint();

    // 设置要使用的技能
    void    SetSelectSkill(uint32 skillId) {m_SelectSkillId = skillId;}
    uint32  GetSelectSkill(bool bTrue = true);
    
    uint32  GetCurrentPathIndex() {return m_CurrentPathIndex;}
    void    SetCurrentPathIndex(uint32 pathIndex) {m_CurrentPathIndex = pathIndex;}

    uint32  GetTargetUnitId();
    void    SetTargetUnitId(uint32 targetId);

    void    ReceiveEvent( uint32 eventId, int32 arg1, int32 arg2 );
    void    MasterOnHit( uint32 objectId );
    void    MasterAttack( uint32 objectId );

    AIGroup*    GetAIGroup() {return m_AIGroup;}
    void        AddToAIGroup(uint32 AIGroupId);
private:
    //void    RegisterGroup(AIGroup* pAIGroup);           // 注册到AI组中(仅由AIGroup调用)
    //void    UnRegisterGroup();

private:
    BattleUnit*     m_Owner;
    AIGroup*        m_AIGroup;          // 所属的AI组

    int32           m_AIUpdateTimer;    // AI逻辑计时器

    // AI用数据
    uint32          m_SelectSkillId;    // 要使用的技能

    PathData        m_PathData;         // 路径
    bool            m_PathLoop;         // 路径循环
    uint32          m_CurrentPathIndex; // 当前所在路径点

    uint32          m_TargetUnitId;     // 目标对象Id
    
    // 属性
    int32           m_BaseX;            // 怪物初始点
    int32           m_BaseY;            // 怪物初始点

    std::vector<std::string>        m_UsedAIName;       // 使用过的AI文件
};

#endif // ai_controller_h__
