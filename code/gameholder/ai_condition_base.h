#ifndef ai_condition_base_h__
#define ai_condition_base_h__

class BattleUnit;
class AIController;
class AIState;

/*
 *	AI条件基类
 */
class AIConditionBase
{
    friend class AIFactory;
public:
    AIConditionBase(uint32 id);
    virtual ~AIConditionBase();

    virtual void onEnterState();
    virtual void onLeaveState() {}
    virtual void onHit(BattleUnit* pAttacker) {}
    virtual void onMasterHit(BattleUnit* pAttacker) {}
    virtual void onMasterAttack(BattleUnit* pDefender) {}

    void    SetNextStateId(uint32 stateId) {m_nextStateId = stateId;}
    uint32  GetNextStateId() {return m_nextStateId;}

    virtual void ParseXML(TiXmlElement* pDoc);

    virtual bool IsSatisfy() = 0;       // 是否满足条件
    virtual void Update(int32 deltaTime);

    uint32  GetID() {return m_id;}
    bool IntervalAndCount();    // 间隔时间和次数是否满足

    AIState*    GetAIState() {return m_pAIState;}

private:

protected:
    uint32  m_id;
    uint32  m_nextStateId;

    int32   m_interval;                     // 间隔时间
    int32   m_count;                        // 执行次数

    int32   m_intervalTimer;
    int32   m_countTimer;

    AIState*    m_pAIState;                // 所属的AI状态

    AIController*   m_pController;

};


#endif // ai_condition_base_h__
