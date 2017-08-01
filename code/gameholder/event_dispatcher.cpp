#include "gameholder_pch.h"
#include "event_dispatcher.h"
#include "event_receiver.h"
#include "event.h"
#include "scene.h"
#include "game_define.h"

EventDispatcher::EventDispatcher(Scene* scene) :
m_Scene(scene),
m_FrameCount(0)
{
}

EventDispatcher::~EventDispatcher()
{
    for(EventMap::iterator iter = m_EventContainer.begin(); iter != m_EventContainer.end(); iter++)
    {
        Event* delEvent = iter->second;
        m_EventPool.Free(delEvent);
    }
}

bool EventDispatcher::Init()
{
    return true;
}

void EventDispatcher::UnInit()
{   
    // 等待所有事件完成
    while(m_EventContainer.size() > 0)
    {        
        Update();
    }
}

bool EventDispatcher::Create(EventReceiver* receiver, uint32 eventId, int32 arg1, int32 arg2, int32 arg3, uint32 delayTime)
{
    uint32 receiverId = receiver->GetId();
    return Create(receiverId, eventId, arg1, arg2, arg3, delayTime);
}

bool EventDispatcher::Create(uint32 receiverId, uint32 eventId, int32 arg1, int32 arg2, int32 arg3, uint32 delayTime)
{
    if(delayTime < g_ServerUpdateTime) 
        delayTime = g_ServerUpdateTime;     // 必延迟一帧响应

    uint32 targetFrame = m_FrameCount + (delayTime/g_ServerUpdateTime);
    CnAssert(targetFrame > m_FrameCount);
    CnAssert((targetFrame - m_FrameCount) < 1000);         // 延迟帧数过多,一定是bug 


    Event* event = m_EventPool.Alloc(this, receiverId, eventId, arg1, arg2, arg3);
    m_EventContainer.insert(EventMap::value_type(targetFrame, event));

    return true;
}


void EventDispatcher::Update()
{
    uint32 count = m_EventContainer.count(m_FrameCount);

    std::pair<EventMap::iterator, EventMap::iterator> range = m_EventContainer.equal_range(m_FrameCount);
    EventMap::iterator itor = range.first;

    if(m_FrameEvents.size() < count)
        m_FrameEvents.resize(count);
    
    for(uint32 i = 0; i < count; ++i)
    {
        m_FrameEvents[i] = itor->second;
        ++itor;
    }

    // call event
    for(uint32 i = 0; i < count; ++i)
    {
        //CnDbg("Trigger event id=%d\n", m_FrameEvents[i].GetEvent());
        m_FrameEvents[i]->Trigger(m_FrameCount);
        m_EventPool.Free(m_FrameEvents[i]);
    }

    m_EventContainer.erase(m_FrameCount);

    m_FrameCount++;
}

uint32 EventDispatcher::EventsCount() const
{
    return m_EventContainer.size();
}

EventReceiver * EventDispatcher::GetReceiverById(uint32 id)
{
    EventReceiver* obj = (EventReceiver*)m_Scene->GetSceneObject(id);
    return obj;
}