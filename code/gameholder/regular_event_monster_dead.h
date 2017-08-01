/*
 *	怪物死亡
 */
#ifndef regular_event_monster_dead_h__
#define regular_event_monster_dead_h__


#include "regular_event.h"

class RegularEvent_MonsterDead : public RegularEvent
{
public:
    RegularEvent_MonsterDead(Regular* pRegular);
    virtual ~RegularEvent_MonsterDead();

    virtual void ParseEventXml(TiXmlElement* EventElement);
    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, const std::string& arg3);

private:
    void OnRecvCreatureDead(uint32 creatureId, int32 group);

private:
    uint32  m_CreatureId;           // 怪物id
    int32   m_creatureGroup;        // 所有组
    uint32  m_CreatureNum;          // 怪物数量

    uint32  m_CreatureIndex;       // 死亡的怪物计数
};

#endif // regular_event_monster_dead_h__
