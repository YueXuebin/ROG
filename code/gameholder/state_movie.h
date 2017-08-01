#ifndef state_movie_h__
#define state_movie_h__

#include "state.h"

/*
 *	演绎状态
 */
class StateMovie : public State
{
public:
    StateMovie(BattleUnit* owner);
    virtual ~StateMovie();

    virtual bool CanChangeTo(uint32 state);

    virtual void onEnterState();
    virtual void onLeaveState();

    void SetMovieName(const std::string& MovieName) {m_MoiveName = MovieName;}
    const std::string& GetMovieName() {return m_MoiveName;}

    void SetAttackable(bool b);
    bool GetAttackable() {return m_AttackAble;}

private:
    std::string m_MoiveName;
    bool        m_AttackAble;
};


#endif // state_movie_h__
