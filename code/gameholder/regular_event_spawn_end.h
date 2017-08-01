// 刷怪点刷怪结束

#ifndef regular_event_spawn_end_h__
#define regular_event_spawn_end_h__

#include "regular_event.h"

class RegularEvent_SpawnEnd : public RegularEvent
{
public:
    RegularEvent_SpawnEnd(Regular* pRegular);
    ~RegularEvent_SpawnEnd();

    virtual void ParseEventXml(TiXmlElement* EventElement);

    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, const std::string& arg3);
    void OnRecvSpawnAllDead(uint32 spawnID);
private:
    uint32 m_spawnID;
};

#endif // regular_event_spawn_end_h__