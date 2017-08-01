#ifndef ai_group_h__
#define ai_group_h__

class AIController;
class AIGroupRegister;

/*
 *	AI群
 *  用来使一个AI群中的怪物有相同行为。
 *  怪物优先执行自己的目标,当找不到自己的目标时则针对群目标
 */
class AIGroup
{
    friend class AIGroupRegister;
    friend class AIController;

    AIGroup(AIGroupRegister* pManager, uint32 id);     // 仅由AIGroupRegister创建和销毁
    ~AIGroup();
public:
    uint32 GetId() {return m_id;}

    void Update();

    uint32  GetTargetUnitId() {return m_TargetUnitId;}
    void    SetTargetUnitId(uint32 targetId) {m_TargetUnitId = targetId;}

private:
    void AddAIController(AIController* pAIController);
    void RemoveAIController(AIController* pAIController);

    bool IsExist(AIController* pAIController);

private:
    uint32  m_id;

    AIGroupRegister*    m_pManager;

    std::vector<AIController*>  m_AIControllers;

    uint32          m_TargetUnitId;     // AI群的目标对象Id

};


#endif // ai_group_h__
