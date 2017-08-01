/*
 *	TW的Http处理线程
 */
#ifndef tw_http_async_h__
#define tw_http_async_h__

class TwExecutor;



#define ERR_HTTP_SUCCESS "0"                        // 成功
#define ERR_HTTP_ARGE "-1"                          // 参数不全
#define ERR_HTTP_VERIFICATION "-2"                  // 验证失败
#define ERR_HTTP_TIMEOUT "-3"                       // 发起请求超时（前后5分钟有效）
#define ERR_HTTP_POST_TIMEOUT "-4"                  // 接口过期
#define ERR_HTTP_POST "-5"                          // 接口异常
#define ERR_HTTP_NO_PLAYER "-6"                     // 目标玩家不存在

// ==================================================
// TW异步查询操作
// ==================================================
class TwHttpAsync : public Crown::CSDThread
{
public:
    TwHttpAsync();
    virtual         ~TwHttpAsync();

    bool            Init(const char* addrIP, uint32 addrPort);

    void            UnInit();

    void            SetExecutor(TwExecutor* executor);     ///< 设置数据库执行器

    void            Run();                                  ///< 线程执行函数

    virtual void SDAPI      ThrdProc()
    {
        Run();
    }

private:
    TwHttpAsync( const TwHttpAsync& );
    TwHttpAsync& operator=( const TwHttpAsync& );
    TwHttpAsync* operator&();
    const TwHttpAsync* operator&() const;

    typedef std::map<std::string, std::string> UrlParamsMap;

    static void     HttpdHandle(struct evhttp_request *req, void *arg);

    Crown::CSDNamedSemaphore*   m_waitForHandle;

    TwExecutor*                m_executor;
    uint32                      m_counter;

    struct event_base*          m_base;
    struct evhttp*              m_http;
    struct evhttp_bound_socket* m_http_handle;
};

#endif // tw_http_async_h__
