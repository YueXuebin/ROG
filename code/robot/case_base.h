/*
 *	行为用例基类
 */
#ifndef case_base_h__
#define case_base_h__

class Robot;
class ActionBase;

class CaseBase
{
public:
    CaseBase(Robot* pRobot);
    virtual ~CaseBase();

    virtual void OnRecv(uint32 msgID, void* data);
    virtual void Update(uint32 tickcount, uint32 deltacount);
    virtual void OnEnterGame();

    void PushAction(ActionBase* action);
    virtual void OnActionStart(ActionBase* action);
    virtual void OnActionEnd(ActionBase* action);
    virtual void OnAllActionEnd();

    Robot*  GetRobot() {return m_Robot;}

protected:
    Robot*  m_Robot;

private:
    std::vector<ActionBase*>    m_ActionList;
    ActionBase*     m_CurrentAction;

    bool m_AllEnd;
};


#endif // case_base_h__
