#ifndef state_h__
#define state_h__

class BattleUnit;

/*
 *	状态基类
 */
class State
{
public:
    State(uint32 type, BattleUnit* owner);
    virtual ~State();

    uint32 GetType() {return m_type;}

    virtual bool isEnd();              // 该状态是否结束
    void SetEndTime(int32 time) {m_EndTime = time; m_UpdateTimer = 0;}
    int32 GetEndTime() {return m_EndTime;}

    int32 GetRemainTime() {return Crown::SDMax(m_EndTime - m_UpdateTimer, 0);}

    virtual bool CanChangeTo(uint32 state) = 0;     // 是否可以切换到某状态

    virtual void Update();

    virtual void onEnterState();
    virtual void onLeaveState();

    void    SetNextState(uint32 state) {m_NextState = state;}
    uint32  GetNextState() {return m_NextState;}

protected:
    BattleUnit* m_Owner;
    int32   m_UpdateTimer;
    int32   m_EndTime;

private:
    uint32  m_type;
    uint32  m_NextState;
    
};


#endif // state_h__