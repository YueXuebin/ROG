#ifndef PUSHER_360_H
#define PUSHER_360_H

#include "../net_manager/pool_allocator.h"
#include "gate_handler.h"
#include "centerserver.h"


typedef void CURL;

class Pusher360Executor;
class Pusher360Async;

#define PUSHER_360_EVENT_POOL_COUNT 50000

// ==================================================
// 登陆事件
// ==================================================
class Pusher360Event
{
public:
    Pusher360Event() { Clear(); }
    ~Pusher360Event() { }

    DECLARE_ALLOCATOR(Pusher360Event, PUSHER_360_EVENT_POOL_COUNT)

    void Clear()
    {
        m_IsGet     = false;
        m_type      = 0;
        m_url       = "";
        m_msg       = "";
    }

    uint32          m_index;        // 执行序号

    bool            m_IsGet;
    int32           m_type;
    std::string     m_url;
    std::string     m_msg;
};

// ==================================================
// 平台回调执行器
// ==================================================
class Pusher360Executor
{
public:
    Pusher360Executor();
    ~Pusher360Executor();

    DECLARE_SINGLETON(Pusher360Executor)

    bool Init(int32 threadNum);
    void Unint();

    void Update();

    bool PushMsg(const std::string& url, int type, const std::string& msg, bool isGet = false);

    Pusher360Event*     QueryPopFront();
    void                QueryPushBack(Pusher360Event* pEvent);

    Pusher360Event*     CallbackPopFront();
    void                CallbackPushBack(Pusher360Event* pEvent);

private:
    void Push360Chat(Player* sender, int type, const std::string& toqid, const std::string& toname, const std::string& content);

    // 查询列表锁
    void            QueryLock() { m_queryLock.Lock(); }
    void            QueryUnLock() { m_queryLock.Unlock(); }

    // 结果列表锁
    void            CallbackLock() { m_callbackLock.Lock(); }
    void            CallbackUnLock() { m_callbackLock.Unlock(); }

    void OnPushMsg(Pusher360Event* p360Event);

    enum
    {
        MAX_SEMAPHORE_NUM = 65535
    };

    Crown::CSDMutex                 m_queryLock;
    Crown::CSDMutex                 m_callbackLock;

    Crown::CSDNamedSemaphore*       m_Pusher360Sema;

    Pusher360Async*                 m_pushAsyncList;

    std::list<Pusher360Event*>      m_queryList;
    std::list<Pusher360Event*>      m_callbackList;
};

#endif
