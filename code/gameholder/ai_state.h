#ifndef ai_state_h__
#define ai_state_h__

class AIActionBase;
class AIConditionBase;
class AIController;
class AIContainer;
class BattleUnit;


/*
 *	AI状态
 */
class AIState
{
    friend class AIFactory;
public:
    AIState(uint32 id, AIContainer* pContainer);
    ~AIState();

    uint32  GetID() {return m_id;}
    AIController*   GetController() {return m_pController;}

    void Update(int32 deltaTime);

    void onEnterState();
    void onLeaveState();

    void onHit(BattleUnit* pAttacker);
    void onMasterHit(BattleUnit* pAttacker);
    void onMasterAttack(BattleUnit* pDefender);

    int32   GetPassTime() {return m_PassTime;}

private:
    uint32  m_id;                                           // 状态ID
    AIController*   m_pController;                          // AI管理器
    AIContainer*    m_pContainer;                           // 包含此状态的状态机对象


    typedef std::vector<AIConditionBase*>   ConditionList;
    ConditionList   m_AllCondition;                         // 所有跳转条件

    AIActionBase*   m_pAction;                              // 行为

    int32   m_PassTime;                                     // 该状态经过的时间
};


#endif // ai_state_h__
