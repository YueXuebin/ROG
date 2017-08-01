/*
 *	状态机
 */
#ifndef state_controller_h__
#define state_controller_h__

class BattleUnit;
class State;

class StateController
{
public:
    StateController(BattleUnit* owner);
    ~StateController();

    void Update();

    void FillStateInfo(STATE_INFO& info) const;
    void LoadStateInfo(const STATE_INFO& info);

    uint32  GetState() const;     // 获取当前状态

    bool    CanChangeTo(uint32 state);      // 是否可以改变至某状态
    void    ChangeState(uint32 state);      // 改变状态

    void    Resurgence();                   // 复活

    bool    CanChangePostion();             // 当前状态是否可以移动
    bool    CanUseSkill();
    bool    GetAttackable();                // 当前状态是否可以被攻击

    void    SetCanCastBreak(bool canBreak);
    bool    GetCanCastBreak();              // 释放状态是否可以主动打断
    
    void    SetMovieName(const std::string& movieName);        // 设置演绎状态的演绎名
    const std::string& GetMovieName();
    void    SetMovieAttackAble(bool attackable);

    void    SetStateToState(uint32 state, uint32 next_state);    // 设置状态的下一个状态

    void    SetSingingTime(int32 time);     // 设置吟唱状态的时长
    void    SetAttackTime(int32 time);      // 设置攻击状态的时长
    void    SetMovieTime(int32 time);       // 设置状态的时长
    void    SetStunTime(int32 time);        // 设置眩晕状态的时长
    void    SetDeadTime(int32 time);        // 设置死亡状态的时长
    void    SetSkill(uint32 skill_id, uint32 object_id, int32 x, int32 y);

    int32   GetRemainTime();

private:
    void    _ChangeState(uint32 state);

private:
    BattleUnit* m_Owner;

    State*  m_CurState;     // 当前状态

    typedef std::map<uint32, State*> StateMap;
    StateMap m_States;       // 所有状态

};

#endif // state_controller_h__
