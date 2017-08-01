/*
 *	行为基类
 */
#ifndef action_base_h__
#define action_base_h__

class Robot;

class ActionBase
{
public:
    ActionBase(Robot* pRobot);
    virtual ~ActionBase();

    virtual const char* name() {return m_Name.c_str();}

    virtual void OnRecv(uint32 msgID, void* data);
    virtual void Update(uint32 tickcount, uint32 deltacount);

    virtual void OnStart();
    virtual void OnEnd();

    bool IsEnd() {return m_end;}
    void End() {m_end = true;}

    uint32  GetID() {return m_ActionId;}

    void SetImmediatelyEnd(bool end) {m_immediatelyEnd = end;}
protected:
    Robot*  m_Robot;
    bool    m_end;
    uint32  m_ActionId;
    std::string m_Name;

    int32   m_MilliSecondTimer;

    // 暂时没用 改动多 还没决定怎么做
    bool    m_immediatelyEnd;
};

#endif // action_base_h__
