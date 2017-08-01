#ifndef regular_event_player_resurgence_req_h__
#define regular_event_player_resurgence_req_h__

#include "regular_event.h"

class RegularEvent_PlayerResurgence : public RegularEvent
{
public:
    RegularEvent_PlayerResurgence(Regular* pRegular);
    virtual ~RegularEvent_PlayerResurgence();

    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, const std::string& arg3);
private:
};


#endif // regular_event_player_resurgence_req_h__
