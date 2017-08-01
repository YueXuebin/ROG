#ifndef pusher_360_async_h__
#define pusher_360_async_h__

class Pusher360Event;
class Pusher360Executor;

// ==================================================
// 昆仑登陆异步查询操作
// ==================================================
class Pusher360Async : public Crown::CSDThread
{
public:
    Pusher360Async();
    virtual         ~Pusher360Async();

    void            SetSemaphore(Crown::CSDNamedSemaphore* pSema);

    void            SetExecutor(Pusher360Executor* executor);   ///< 设置数据库执行器

    void            Run();                                      ///< 线程执行函数

    void SDAPI ThrdProc()
    {
        Run();
    }

private:
    Pusher360Async( const Pusher360Async& );
    Pusher360Async& operator=( const Pusher360Async& );
    Pusher360Async* operator&();
    const Pusher360Async* operator&() const;

    static size_t   CBPush360AsyncWB(void *ptr, size_t size, size_t nmemb, void *userp);

    void            PushMsg(Pusher360Event* pushEvent);

    Crown::CSDNamedSemaphore*   m_waitForHandle;

    Pusher360Executor*  m_executor;

    uint32  m_Index;
    int32   m_EventType;
    std::string m_Url;
};


#endif // pusher_360_async_h__
