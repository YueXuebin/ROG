#include "net_manager_pch.h"
#include "reactor.h"
#include "event_handler.h"

list_head Reactor::HandlerTable[HANDLER_TABLE_SIZE];

static inline uint32_t hashFn(uint32_t nKey)
{
    return nKey % HANDLER_TABLE_SIZE;
}

int Reactor::m_HandlerCount = 0;
// class Reactor
Reactor::~Reactor()
{

}

void Reactor::initHandlerTable()
{
    for (int nIndex = 0; nIndex < HANDLER_TABLE_SIZE; nIndex++)
    {
        INIT_LIST_HEAD(&HandlerTable[nIndex]);
    }
    m_HandlerCount = 0;
}

Event_Handler* Reactor::getHandler(uint32_t id)
{
    int npos = hashFn(id);
    assert(npos < HANDLER_TABLE_SIZE && npos >= 0);

    list_head* head = &HandlerTable[npos];
    list_head* pos;
    Event_Handler* pEventHandler = NULL;
    list_for_each(pos, head)
    {
        pEventHandler = list_entry(pos, Event_Handler, hashitem);
        if (pEventHandler->get_id() == id)
        {
            break;
        }
        pEventHandler = NULL;
    }
    return pEventHandler;
}


void Reactor::putHandler(Event_Handler *pHandler, uint32_t id)
{
    assert(pHandler != NULL);
    int npos = hashFn(id);
    assert(npos < HANDLER_TABLE_SIZE && npos >= 0);
    list_head* head = &HandlerTable[npos];
    list_add_tail(&pHandler->hashitem, head);
    m_HandlerCount++;
}

void Reactor::removeHandler(uint32_t id)
{
    int npos = hashFn(id);
    assert(npos < HANDLER_TABLE_SIZE && npos >= 0);
    list_head* head = &HandlerTable[npos];
    list_head* pos, *n;

    Event_Handler* pEventHandler = NULL;
    list_for_each_safe(pos, n, head)
    {
        pEventHandler = list_entry(pos, Event_Handler, hashitem);
        if (pEventHandler->get_id() == id)
        {
            list_del(pos);
            m_HandlerCount--;
            break;
        }
    }
}

void Reactor::removeHandler(Event_Handler* pHandler)
{
    list_del(&pHandler->hashitem);
    m_HandlerCount--;
}

void Reactor::finitHandlerTable()
{
    list_head *pos, *n;	
    list_head* head;
    for (int nIndex = 0; nIndex < HANDLER_TABLE_SIZE; nIndex++)
    {
        head = &HandlerTable[nIndex];
        list_for_each_safe(pos, n, head) 
        {
            Event_Handler* pEventHandler = list_entry(pos, Event_Handler, hashitem);
            list_del(pos);
            CnDelete pEventHandler;
        }
    }
    m_HandlerCount = 0;
}

int Reactor::end_reactor_event_loop()
{
    list_head* pos;
    list_head* head;
    for (int nIndex = 0; nIndex < HANDLER_TABLE_SIZE; nIndex++)
    {
        head = &HandlerTable[nIndex];
        list_for_each(pos, head)
        {
            Event_Handler *event_handler = list_entry(pos, Event_Handler, hashitem);
            event_handler->handle_close();
        }
    }
    return 0;
}




