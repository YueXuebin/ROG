#ifndef KUNLUN_PAY_H
#define KUNLUN_PAY_H


#include "gate_handler.h"
#include "centerserver.h"
#include "config_loader.h"


class KlPayEvent;
class KlPayAsync;

// ==================================================
// 昆仑登陆执行器
// ==================================================
class KlPayExecutor
{
public:
    KlPayExecutor();
    ~KlPayExecutor();

    DECLARE_SINGLETON(KlPayExecutor)

    bool            Init(const char* addrIP, uint32 addrPort);
    void            UnInit();

    void            Update();

    KlPayEvent*     PayPopFront();
    void            PayPushBack(KlPayEvent* payEvent);

    KlPayEvent*     CallbackPopFront();
    void            CallbackPushBack(KlPayEvent* payEvent);

private:

    // 查询列表锁
    void            QueryLock() { m_queryLock.Lock(); }
    void            QueryUnLock() { m_queryLock.Unlock(); }

    // 结果列表锁
    void            CallbackLock() { m_callbackLock.Lock(); }
    void            CallbackUnLock() { m_callbackLock.Unlock(); }

    enum
    {
        MAX_SEMAPHORE_NUM = 65535
    };

    Crown::CSDMutex                 m_queryLock;
    Crown::CSDMutex                 m_callbackLock;

    Crown::CSDNamedSemaphore*       m_KunlunPaySema;

    KlPayAsync*                     m_payAsync;

    std::list<KlPayEvent*>          m_queryList;
    std::list<KlPayEvent*>          m_callbackList;
};

#endif
