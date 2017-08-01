#ifndef kunlun_pay_http_async_h__
#define kunlun_pay_http_async_h__

class KlPayExecutor;
class KlPayEvent;

// ==================================================
// 昆仑付款异步查询操作
// ==================================================
class KlPayAsync : public Crown::CSDThread
{
public:
    KlPayAsync();
    virtual         ~KlPayAsync();

    bool            Init(const char* addrIP, uint32 addrPort);
    void            UnInit();

    void            SetSemaphore(Crown::CSDNamedSemaphore* pSema);

    void            SetExecutor(KlPayExecutor* executor);   ///< 设置数据库执行器

    void            Run();                                  ///< 线程执行函数

    void SDAPI      ThrdProc()
    {
        Run();
    }

    void            PushPayOrder(KlPayEvent* payEvent);

private:
    KlPayAsync( const KlPayAsync& );
    KlPayAsync& operator=( const KlPayAsync& );
    KlPayAsync* operator&();
    const KlPayAsync* operator&() const;

    typedef std::map<std::string, std::string> UrlParamsMap;

    static void     HttpdHandle(struct evhttp_request *req, void *arg);

    //bool            CheckOrderForm(UrlParamsMap& params_map);               ///< 检查订单

    Crown::CSDNamedSemaphore*   m_waitForHandle;

    KlPayExecutor*              m_executor;

    struct event_base*          m_base;
    struct evhttp*              m_http;
    struct evhttp_bound_socket* m_http_handle;
};



#endif // kunlun_pay_http_async_h__
