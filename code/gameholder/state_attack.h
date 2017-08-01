#ifndef state_attack_h__
#define state_attack_h__

#include "state.h"

/*
 *	施法状态
 */
class StateAttack : public State
{
public:
    StateAttack(BattleUnit* owner);
    virtual ~StateAttack();

    virtual bool CanChangeTo(uint32 state);
    virtual void Update();

    virtual void onEnterState();

    void    SetCanBreak(bool canBreak) {m_CanBreak = canBreak;}
    bool    GetCanBreak() {return m_CanBreak;}

private:
    bool    m_CanBreak;             // 是否可以主动中断
};

#endif // state_attack_h__
