#ifndef rog_regular_event_player_dead_h__
#define rog_regular_event_player_dead_h__

#include "rog_regular_event.h"

/*
 *	玩家死亡事件
 *  param1: 关卡的总命数
 *  param2: 触发命数
 *  param3: 已经用掉的命数
 */
class ROGRegularEvent_PlayerDead : public ROGRegularEvent
{
public:
    ROGRegularEvent_PlayerDead(RegularDungeon* pRegular, int32 deadNumActive = -1, bool startActive = false);     // 死deadNumActive条命达成条件
    virtual ~ROGRegularEvent_PlayerDead();

    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

    int32   GetRegularDeadCount() {return m_param1;}

    void    SetActiveDeadCount(int32 num) {m_param2 = num;}
    int32   GetActiveDeadCount() {return m_param2;}

    int32   GetAlreadyDeadCount() {return m_param3;}

private:
    bool m_startActive;
};

#endif // rog_regular_event_player_dead_h__
