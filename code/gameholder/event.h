/*
 *	事件对象
 */
#ifndef event_h__
#define event_h__

class EventDispatcher;

class Event
{
public:
    Event(EventDispatcher* dispatcher, uint32 recieverId, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);
    virtual ~Event();

    uint32  GetEvent() {return m_EventId;}

    virtual void Trigger(uint32 frameCount);

    EventDispatcher* Dispatcher() const { return m_Dispatcher; }
private:

    EventDispatcher*    m_Dispatcher;
    uint32              m_RecieverId;
    uint32              m_EventId;
    int32               m_Arg1, m_Arg2, m_Arg3;
};


#endif // event_h__
