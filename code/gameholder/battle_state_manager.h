#ifndef battle_state_manager_h__
#define battle_state_manager_h__

class Player;

/*
 *	战斗状态管理器
 */
class BattleStateManager
{
public:
    BattleStateManager(Player* player);
    ~BattleStateManager();

    void ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 );

    void OnUseSkill(uint32 skill_id);

    void Update();

    void OnStateChange(uint32 oldState, uint32 newState);

    bool IsInBattle() {return m_InBattle;}

private:
    void EnterBattleState();
    void LeaveBattleState();

    void OnEnterBattleState();
    void OnLeaveBattleState();

private:
    Player* m_player;

    bool    m_InBattle;         // 是否在战斗状态中
    int32   m_InBattleTimer;    // 战斗状态计时
};


#endif // battle_state_manager_h__
