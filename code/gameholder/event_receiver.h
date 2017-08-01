/*
 *	事件机制
 */
#ifndef event_receiver_h__
#define event_receiver_h__

#include "object_with_id.h"

class EventReceiver : public ObjectWithId
{
public:
	EventReceiver(uint32 objType);
	virtual ~EventReceiver();

    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3) {}
};


#endif // event_receiver_h__
