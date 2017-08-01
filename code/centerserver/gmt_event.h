#ifndef gmt_event_h__
#define gmt_event_h__

#include "../net_manager/pool_allocator.h"

#define GMT_EVENT_POOL_COUNT 10000

// ==================================================
// GMT事件对象
// ==================================================
class GmtEvent
{
public:
    GmtEvent() { Clear(); }
    ~GmtEvent() { }

    DECLARE_ALLOCATOR(GmtEvent, GMT_EVENT_POOL_COUNT)

    void Clear();

    uint32      m_Counter;              // 请求计数
    int32       m_HttpResult;           // HTTP_OK：返回正确
    std::string m_cmdjson;              // 返回正确：表示结果  返回错误：表示错误原因
};




#endif // gmt_event_h__
