/*
 *	TW��Http�����߳�
 */
#ifndef tw_http_async_h__
#define tw_http_async_h__

class TwExecutor;



#define ERR_HTTP_SUCCESS "0"                        // �ɹ�
#define ERR_HTTP_ARGE "-1"                          // ������ȫ
#define ERR_HTTP_VERIFICATION "-2"                  // ��֤ʧ��
#define ERR_HTTP_TIMEOUT "-3"                       // ��������ʱ��ǰ��5������Ч��
#define ERR_HTTP_POST_TIMEOUT "-4"                  // �ӿڹ���
#define ERR_HTTP_POST "-5"                          // �ӿ��쳣
#define ERR_HTTP_NO_PLAYER "-6"                     // Ŀ����Ҳ�����

// ==================================================
// TW�첽��ѯ����
// ==================================================
class TwHttpAsync : public Crown::CSDThread
{
public:
    TwHttpAsync();
    virtual         ~TwHttpAsync();

    bool            Init(const char* addrIP, uint32 addrPort);

    void            UnInit();

    void            SetExecutor(TwExecutor* executor);     ///< �������ݿ�ִ����

    void            Run();                                  ///< �߳�ִ�к���

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
