#ifndef state_die_h__
#define state_die_h__

#include "state.h"

/*
 *	死亡状态
 */
class StateDie : public State
{
public:
    StateDie(BattleUnit* owner);
    virtual ~StateDie();

    virtual bool CanChangeTo(uint32 state);

    virtual void Update();

    virtual bool isEnd();
    virtual void onEnterState();
    virtual void onLeaveState();

    void    SetDeadTime(int32 time) {m_DeadTime = time;}
    int32   GetDeadTime() {return m_DeadTime;}
private:
    int32   m_DeadTime;
    bool    m_isEnd;
};


#endif // state_die_h__
