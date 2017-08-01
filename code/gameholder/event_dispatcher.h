/*
 *	事件分发器(场景范围内)
 */
#ifndef event_dispatcher_h__
#define event_dispatcher_h__


class Scene;
class Event;
class EventWithCallback;
class EventReceiver;

class EventDispatcher
{
public:
    EventDispatcher(Scene* scene);
    virtual ~EventDispatcher();

    bool Init();
    void UnInit();

    bool Create(uint32 recieverId, uint32 eventId, int32 arg1, int32 arg2, int32 arg3, uint32 delayTime = 0);
    bool Create(EventReceiver* receiver, uint32 eventId, int32 arg1, int32 arg2, int32 arg3, uint32 delayTime = 0);

    void Update();

    EventReceiver* GetReceiverById(uint32 id);

    uint32 EventsCount() const;
    Scene* GetScene() const { return m_Scene; }

private:
    Scene* m_Scene;

    typedef std::multimap<uint32, Event*> EventMap;       // <帧数, 事件对象>
    EventMap m_EventContainer;

    uint32 m_FrameCount;
    CnVector<Event*>	m_FrameEvents;

    // 池
    Crown::CSDObjectPool<Event>	m_EventPool;

};


#endif // event_dispatcher_h__
