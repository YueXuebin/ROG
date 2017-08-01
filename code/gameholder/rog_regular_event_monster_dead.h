#ifndef rog_regular_event_monster_dead_h__
#define rog_regular_event_monster_dead_h__

#include "rog_regular_event.h"

/*
 *	指定怪物死亡
 *  param1: 怪物ID
 *  param2: 杀死总数量
 *  param3: 已经杀死数
 */
class ROGRegularEvent_MonsterDead : public ROGRegularEvent
{
    CnDeclareRTTI
public:
    ROGRegularEvent_MonsterDead(RegularDungeon* pRegular);
    virtual ~ROGRegularEvent_MonsterDead();
    
    void SetMonster(int32 monsterId, int32 count);
    int32 GetMonsterId() { return m_param1; }
    int32 GetMonsterCount() { return m_param2; }
    int32 GetCurrentMonsterCount() { return m_param3; }

    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

private:
    void OnRecvCreatureDead(uint32 creatureId);

    void SetCurrentMonsterCount(int32 val) { m_param3 = val; }
};

#endif // rog_regular_event_monster_dead_h__
