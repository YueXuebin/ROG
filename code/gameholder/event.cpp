#include "gameholder_pch.h"
#include "event.h"
#include "event_receiver.h"
#include "event_dispatcher.h"
#include "scene.h"
#include "creature.h"


Event::Event(EventDispatcher* dispatcher, uint32 recieverId, uint32 eventId, int32 arg1, int32 arg2, int32 arg3) :
m_Dispatcher(dispatcher),
m_RecieverId(recieverId),
m_EventId(eventId),
m_Arg1(arg1),
m_Arg2(arg2),
m_Arg3(arg3)
{
}

Event::~Event()
{
}

void Event::Trigger(uint32 frameCount)
{
    EventReceiver* obj = nullptr;
    if(m_RecieverId != 0)
    {
        obj = (EventReceiver*)Dispatcher()->GetScene()->GetSceneObject(m_RecieverId);
        if (obj != nullptr)
            obj->ReceiveEvent(frameCount, m_EventId, m_Arg1, m_Arg2, m_Arg3);
        // 对象可能已经不存在了
    }
    else    // RecicverId=0时通知场景中所有对象
    {
        const Scene::BattleUnitMap& allCreatures = Dispatcher()->GetScene()->GetBattleUnits();
        for(Scene::BattleUnitMap::const_iterator iter = allCreatures.begin(); iter != allCreatures.end(); iter++)
        {
            iter->second->ReceiveEvent(frameCount, m_EventId, m_Arg1, m_Arg2, m_Arg3);
        }

        const Scene::SceneObjectMap& allObjects = Dispatcher()->GetScene()->GetObjects();
        for(Scene::SceneObjectMap::const_iterator iter = allObjects.begin(); iter != allObjects.end(); iter++)
        {
            iter->second->ReceiveEvent(frameCount, m_EventId, m_Arg1, m_Arg2, m_Arg3);
        }

        m_Dispatcher->GetScene()->ReceiveEvent(frameCount, m_EventId, m_Arg1, m_Arg2, m_Arg3);

    }
}
