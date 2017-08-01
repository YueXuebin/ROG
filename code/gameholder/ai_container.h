#ifndef ai_container_h__
#define ai_container_h__

class AIState;
class AIController;

// 继承AIContainer的子类类型
enum
{
    AICONTAINER_TYPE_CONTROLLER = 1,        // AIController
    AICONTAINER_TYPE_ACTION = 2             // AIActionContainer
};

/*
 *	包含AI状态机的接口类
 */
class AIContainer
{
    friend class AIFactory;
public:
    AIContainer(uint32 ClassType);
    virtual ~AIContainer();

    uint32  GetClassType() {return m_ClassType;}

    const std::string& GetAIName() {return m_AIName;}

    uint32  GetAIState() {return m_AIStateId;}
    void    SetAIState(uint32 state);

    virtual bool LoadAI(const std::string& aiName);         // 加载AI状态机
    virtual void ClearAI();                                 // 清除AI状态机

    virtual void Update(uint32 delta_time);

    virtual AIController* GetController() = 0;

protected:
    bool        HasState(uint32 state);
    AIState*    GetState(uint32 state);


protected:
    uint32          m_ClassType;        // 子类类型

    uint32          m_lastAIStateId;    // 上一个AI状态id
    uint32          m_AIStateId;        // 当前AI状态id
    std::string     m_AIName;           // AI文件名

    // AI状态
    AIState*    m_pAIState;             // 当前AI状态
    typedef std::map<uint32, AIState*> AIStateMap;
    AIStateMap      m_AllStateMap;      // 所有AI状态
};


#endif // ai_container_h__
