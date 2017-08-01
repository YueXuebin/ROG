/*
 *	离线玩家死亡事件
 */
#ifndef regular_event_offline_player_dead_h__
#define regular_event_offline_player_dead_h__

#include "regular_event.h"

class RegularEvent_OfflinePlayerDead : public RegularEvent
{
public:
    RegularEvent_OfflinePlayerDead(Regular* pRegular);
    virtual ~RegularEvent_OfflinePlayerDead();

    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, const std::string& arg3);
};

#endif // regular_event_offline_player_dead_h__
