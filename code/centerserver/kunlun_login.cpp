#include "centerserver_pch.h"
#include "kunlun_login.h"
#include "curl.h"
#include "config_loader.h"
#include "centerserver.h"
#include "gate_handler.h"
#include "player_manager.h"
#include "player_job.h"

IMPLEMENT_ALLOCATOR(KlLoginEvent)

IMPLEMENT_SINGLETON(KlLoginExecutor)

// ==============================
// KlLoginAsync
// ==============================
KlLoginAsync::KlLoginAsync()
{
    m_waitForHandle = NULL;
}

KlLoginAsync::~KlLoginAsync()
{

}

void    KlLoginAsync::SetSemaphore(Crown::CSDNamedSemaphore* pSema)
{
    m_waitForHandle = pSema;
}

void    KlLoginAsync::SetExecutor(KlLoginExecutor* executor)
{
    m_executor = executor;
}

void    KlLoginAsync::Run()
{
    KlLoginEvent* loginEvent = NULL;
    for(;;)
    {
        m_waitForHandle->Wait();

        if(NULL != (loginEvent = m_executor->QueryPopFront()))
        {
            HttpVerify(loginEvent);
            m_executor->CallbackPushBack(loginEvent);
        }
    }
}

size_t KlLoginAsync::CBKlLoginAsyncWB(void *ptr, size_t size, size_t nmemb, void *userp)
{
    Json::Reader reader;
    Json::Value json_val;
    char buf[1024] = {0};
    char idstr[30] = {0};
    uint32  len = 0;
    uint32  json_uid = 0;
    KlLoginEvent* loginEvent = 0;
    int32   retCode = 0;
    std::string uid = "";
    std::string uname = "";

    len = size * nmemb;

    loginEvent = (KlLoginEvent*)userp;

    if(!loginEvent)
    {
        CnAssert(false);
        return len;
    }

    loginEvent->m_errCode = ERR_FAILED;

    if(len > 1023) len = 1023;

    memcpy(buf, ptr, len);
    buf[len] = '\0';

    json_val.clear();
    if(!reader.parse(buf, &buf[len], json_val, false))
    {
        goto _END;
    }

    //CnDbg("CBKlLoginAsyncWB=%s\n", buf);

    retCode = json_val["retcode"].asInt();
    if(retCode == 0)    // SUCCEED
    {
        json_uid = json_val["data"]["uid"].asUInt();
        Crown::SDUtoa64<30>(idstr, json_uid);
        uid = idstr;
        uname = json_val["data"]["uname"].asString();
        loginEvent->m_uid = uid;
        loginEvent->m_uname = uname;
        loginEvent->m_errCode = ERR_SUCCEED;
    }

_END:

    //m_executor->CallbackPushBack(loginEvent);

    return len;                          /* no more data left to deliver */
}

void    KlLoginAsync::HttpVerify(KlLoginEvent* loginEvent)
{
    CURL *curl_handle = 0;
    CURLcode curl_code;

    if(!loginEvent) return;

    loginEvent->m_errCode = ERR_FAILED;

    curl_handle = curl_easy_init();

    if(!curl_handle) return;

    curl_easy_setopt(curl_handle, CURLOPT_URL, ConfigLoader::Instance()->GetKunlunApiUrl().c_str());
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CBKlLoginAsyncWB);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, loginEvent);
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, loginEvent->m_token.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, loginEvent->m_token.size());
    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, FALSE);

    //CnDbg("%s HttpVerify: %s\n", Crown::GetIpStr(loginEvent->m_addrIP).c_str(), loginEvent->m_token.c_str());

    // 就是当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出
    curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1);     // CURL多线程的严重错误  http://curl.haxx.se/libcurl/c/curl_easy_setopt.html

    curl_code = curl_easy_perform(curl_handle);
    curl_easy_cleanup(curl_handle);

    if(curl_code != CURLE_OK)
    {
        CnWarn("curl_easy_perform() failed: %s\n", curl_easy_strerror(curl_code));
    }
}

// ==============================
// KlLoginExecutor
// ==============================
KlLoginExecutor::KlLoginExecutor()
{
    std::string SemaName = "KunlunLogin_" + Crown::ToString(Crown::SDGetCurrentProcessId());
    m_KunlunLoginSema = Crown::CreateNamedSemaphore(SemaName.c_str(), 0);
    CnAssert(m_KunlunLoginSema);
    CnInfo("create semaphore %s\n", SemaName.c_str());

    m_loginAsyncList = 0;
}

KlLoginExecutor::~KlLoginExecutor()
{
    Unint();
}

bool KlLoginExecutor::Init(int32 threadNum)
{
    curl_global_init(CURL_GLOBAL_ALL);

    if(m_loginAsyncList)
    {
        CnDelete[] m_loginAsyncList;
        m_loginAsyncList = 0;
    }

    m_loginAsyncList = CnNew KlLoginAsync[threadNum];

    // 初始化异步执行
    for (uint8 i=0; i<threadNum; ++i)
    {
        m_loginAsyncList[i].SetSemaphore(m_KunlunLoginSema);

        m_loginAsyncList[i].SetExecutor(this);
    }

    // 开始异步线程
    for (uint8 i=0; i<threadNum; ++i)
    {
        m_loginAsyncList[i].Start();
    }

    return true;
}

void KlLoginExecutor::Unint()
{
    if(m_loginAsyncList) 
        CnDelete[] m_loginAsyncList;

    curl_global_cleanup();
}

bool KlLoginExecutor::Login(uint32 gateSvrID, uint64 loginID, std::string& token, uint32 addrIP, uint16 addrPort)
{
    KlLoginEvent* loginEvent = 0;

    loginEvent = new KlLoginEvent;

    if(!loginEvent)
    {
        // 登陆失败
        GateHandler::Instance()->SendLoginGameAck(ERR_KUNLUN_EVENT_LOGIN, gateSvrID, loginID);
        return false;
    }

    // 昆仑用户登陆信息
    loginEvent->m_gateSvrID = gateSvrID;
    loginEvent->m_loginID = loginID;
    loginEvent->m_token = token;
    loginEvent->m_addrIP = addrIP;
    loginEvent->m_addrPort = addrPort;
    QueryPushBack(loginEvent);

    return true;
}

// ==================================================
// 验证昆仑模式登录
// ==================================================
void KlLoginExecutor::OnVerifyKunlunModeLogin(uint32 gateSvrID, uint64 loginID, std::string& uid, std::string& uname, uint32 addrIP, uint16 addrPort)
{
    CnAssert(false);            // 暂时不用了
    Player* player = 0;

    if(!PlayerManager::Instance()->AddVerifyPlayer(gateSvrID, loginID, uid, 
        addrIP, addrPort, uid, uname, ConfigLoader::Instance()->GetPlatformName(), "S1"))
    {
        GateHandler::Instance()->SendLoginGameAck(ERR_FAILED, gateSvrID, loginID);
        return;
    }
}

// ==================================================
// 昆仑激活码登陆模式
// ==================================================
void KlLoginExecutor::OnVerifyKunlunCdkeyModeLogin(uint32 gateSvrID,  uint64 loginID, std::string& uid, std::string& uname, uint32 addrIP, uint16 addrPort)
{
    // Kunlun Cdkey 登陆模式
    PlayerJob::Instance()->LoginLoadKLAccount(gateSvrID, loginID, uid, uname, addrIP, addrPort);
}

void KlLoginExecutor::OnVerifyKunlunCdkeyModeLoginCB( uint32 gateSvrID, uint64 loginID, const std::string& account, uint32 addrIP, uint16 addrPort, const std::string& uid, const std::string& uname )
{
    CnAssert(false);            // 暂时不用了
    if(!PlayerManager::Instance()->AddVerifyPlayer(gateSvrID, loginID, account, addrIP, addrPort, uid, uname, ConfigLoader::Instance()->GetPlatformName(), "S1"))
    {
        GateHandler::Instance()->SendLoginGameAck(ERR_ACTIVE_NOT_ACTIVE_LOGIN, gateSvrID, loginID);
        return;
    }
}
