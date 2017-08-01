#ifndef KUNLUN_LOGIN_H
#define KUNLUN_LOGIN_H

#include "../net_manager/pool_allocator.h"
#include "gate_handler.h"
#include "centerserver.h"
#include "config_loader.h"


typedef void CURL;

class KlLoginExecutor;

#define KUNLUN_LOGIN_EVENT_POOL_COUNT 50000

// ==================================================
// 昆仑登陆事件
// ==================================================
class KlLoginEvent
{
public:
    KlLoginEvent() { }
    ~KlLoginEvent() { }

    DECLARE_ALLOCATOR(KlLoginEvent, KUNLUN_LOGIN_EVENT_POOL_COUNT)

    void Clear()
    {
        m_gateSvrID     = 0;
        m_loginID       = 0;
        m_token         = "";
        m_addrIP        = 0;
        m_addrPort      = 0;
        m_errCode       = ERR_FAILED;
        m_uid           = "";
        m_uname         = "";
    }

    uint32 m_gateSvrID;         ///< 登陆 GateID
    uint64 m_loginID;           ///< 登陆 LoginID
    std::string m_token;        ///< 验证码
    uint32 m_addrIP;            ///< 登陆 IP
    uint32 m_addrPort;          ///< 登陆 Port
    uint16 m_errCode;           ///< 错误码
    // ------------------------------------------
    std::string m_uid;          ///< 昆仑ID
    std::string m_uname;        ///< 昆仑账户名
};

// ==================================================
// 昆仑登陆异步查询操作
// ==================================================
class KlLoginAsync : public Crown::CSDThread
{
public:
    KlLoginAsync();
    virtual         ~KlLoginAsync();

    void            SetSemaphore(Crown::CSDNamedSemaphore* pSema);

    void            SetExecutor(KlLoginExecutor* executor); ///< 设置数据库执行器

    void            Run();                                  ///< 线程执行函数

    void SDAPI ThrdProc()
    {
        Run();
    }

private:
    KlLoginAsync( const KlLoginAsync& );
    KlLoginAsync& operator=( const KlLoginAsync& );
    KlLoginAsync* operator&();
    const KlLoginAsync* operator&() const;

    static size_t   CBKlLoginAsyncWB(void *ptr, size_t size, size_t nmemb, void *userp);

    void            HttpVerify(KlLoginEvent* loginEvent);

    Crown::CSDNamedSemaphore*   m_waitForHandle;

    KlLoginExecutor*    m_executor;
};

// ==================================================
// 昆仑登陆执行器
// ==================================================
class KlLoginExecutor
{
public:
    KlLoginExecutor();
    ~KlLoginExecutor();

    DECLARE_SINGLETON(KlLoginExecutor)

    bool Init(int32 threadNum);
    void Unint();

    void Update()
    {
        for(KlLoginEvent* loginEvent = CallbackPopFront(); loginEvent; loginEvent = CallbackPopFront())
        {
            if(!loginEvent)
            {
                CnAssert(false);
                continue;
            }

            if(loginEvent->m_errCode == ERR_SUCCEED)
            {
                if(ConfigLoader::Instance()->GetLoginMode() == LOGIN_MODE_KUNLUN)
                {
                    // 昆仑验证登陆
                    OnVerifyKunlunModeLogin(loginEvent->m_gateSvrID, 
                        loginEvent->m_loginID, loginEvent->m_uid, loginEvent->m_uname, loginEvent->m_addrIP, loginEvent->m_addrPort);
                }
                else if(ConfigLoader::Instance()->GetLoginMode() == LOGIN_MODE_KUNLUN_CDKEY)
                {
                    // 昆仑验证登陆
                    OnVerifyKunlunCdkeyModeLogin(loginEvent->m_gateSvrID, 
                        loginEvent->m_loginID, loginEvent->m_uid, loginEvent->m_uname, loginEvent->m_addrIP, loginEvent->m_addrPort);
                }
            }
            else
            {
                // 登陆失败
                CnInfo("kunlun name=%s login failed\n", loginEvent->m_uname.c_str());
                GateHandler::Instance()->SendLoginGameAck(ERR_KUNLUN_EVENT_LOGIN, loginEvent->m_gateSvrID, loginEvent->m_loginID);
            }

            delete loginEvent;
        }
    }

    bool Login(uint32 gateSvrID, uint64 loginID, std::string& token, uint32 addrIP, uint16 addrPort);

    KlLoginEvent*   QueryPopFront()
    {
        KlLoginEvent* loginEvent = NULL;
        QueryLock();
        if(!m_queryList.empty())
        {
            loginEvent = m_queryList.front();
            m_queryList.pop_front();
        }
        QueryUnLock();
        return loginEvent;
    }

    void QueryPushBack(KlLoginEvent* loginEvent)
    {
        QueryLock();
        m_queryList.push_back(loginEvent);
        m_KunlunLoginSema->Post();

        QueryUnLock();
    }

    KlLoginEvent* CallbackPopFront()
    {
        KlLoginEvent* loginEvent = NULL;
        CallbackLock();
        if(!m_callbackList.empty())
        {
            loginEvent = m_callbackList.front();
            m_callbackList.pop_front();
        }
        CallbackUnLock();
        return loginEvent;
    }

    void CallbackPushBack(KlLoginEvent* loginEvent)
    {
        CallbackLock();
        m_callbackList.push_back(loginEvent);
        CallbackUnLock();
    }

    // 昆仑激活码登陆返回
    void OnVerifyKunlunCdkeyModeLoginCB(uint32 gateSvrID, uint64 loginID, const std::string& account, uint32 addrIP, uint16 addrPort, const std::string& uid, const std::string& uname);

private:
    // 昆仑登陆模式
    void OnVerifyKunlunModeLogin(uint32 gateSvrID,  uint64 loginID, std::string& uid, std::string& uname, uint32 addrIP, uint16 addrPort);

    // 昆仑激活码登陆模式
    void OnVerifyKunlunCdkeyModeLogin(uint32 gateSvrID,  uint64 loginID, std::string& uid, std::string& token, uint32 addrIP, uint16 addrPort);
    

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

    Crown::CSDNamedSemaphore*       m_KunlunLoginSema;

    KlLoginAsync*                   m_loginAsyncList;

    std::list<KlLoginEvent*>        m_queryList;
    std::list<KlLoginEvent*>        m_callbackList;
};

#endif
