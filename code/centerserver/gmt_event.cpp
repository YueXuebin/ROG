#include "centerserver_pch.h"
#include "gmt_event.h"


IMPLEMENT_ALLOCATOR(GmtEvent)

void GmtEvent::Clear()
{
    m_Counter = 0;
    m_cmdjson.clear();
    m_HttpResult = HTTP_OK;
}