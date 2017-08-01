#ifndef tw_executor_h__
#define tw_executor_h__

class TwHttpAsync;

// ==================================================
// 台服 视讯聊天
// ==================================================
class TwExecutor
{
public:
    TwExecutor();
    ~TwExecutor();

    DECLARE_SINGLETON(TwExecutor)

    bool            Init(const char* addrIP, uint32 addrPort);
    void            Unint();

private:
    TwHttpAsync*                   m_gmtAsync;
};

#endif // tw_executor_h__
