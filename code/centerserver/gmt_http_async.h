/*
 *	GMT的Http处理线程
 */
#ifndef gmt_http_async_h__
#define gmt_http_async_h__


class GmtExecutor;
class GmtEvent;


// ==================================================
// GMT异步查询操作
// ==================================================
class GmtHttpAsync : public Crown::CSDThread
{
public:
    GmtHttpAsync();
    virtual         ~GmtHttpAsync();

    bool            Init(const char* addrIP, uint32 addrPort);

    void            UnInit();

    void            SetSemaphore(Crown::CSDNamedSemaphore* pSema);

    void            SetExecutor(GmtExecutor* executor);     ///< 设置数据库执行器

    void            Run();                                  ///< 线程执行函数

    virtual void SDAPI      ThrdProc()
    {
        Run();
    }

private:
    GmtHttpAsync( const GmtHttpAsync& );
    GmtHttpAsync& operator=( const GmtHttpAsync& );
    GmtHttpAsync* operator&();
    const GmtHttpAsync* operator&() const;

    typedef std::map<std::string, std::string> UrlParamsMap;

    static void     HttpdHandle(struct evhttp_request *req, void *arg);

    void            PushCommand(std::string& cmdjson);      /// 命令 JSON
    GmtEvent*       PopCommand();

    Crown::CSDNamedSemaphore*   m_waitForHandle;

    GmtExecutor*                m_executor;
    uint32                      m_counter;

    struct event_base*          m_base;
    struct evhttp*              m_http;
    struct evhttp_bound_socket* m_http_handle;
};

#endif // gmt_http_async_h__
