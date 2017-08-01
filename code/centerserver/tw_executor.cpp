#include "centerserver_pch.h"
#include "tw_executor.h"
#include "config_loader.h"
#include "tw_http_async.h"

IMPLEMENT_SINGLETON(TwExecutor)

// ==============================
// 构造函数
// ==============================
TwExecutor::TwExecutor()
{
    uint32 pid = Crown::SDGetCurrentProcessId();
    std::string SemaName = "TwExecutor_" + Crown::ToString(pid);

    CnInfo("create semaphore %s\n", SemaName.c_str());

    m_gmtAsync = NULL;
}

// ==============================
// 析构函数
// ==============================
TwExecutor::~TwExecutor()
{
    //Unint();
}

// ==============================
// 初始化
// ==============================
bool TwExecutor::Init(const char* addrIP, uint32 addrPort)
{
    if(m_gmtAsync)
    {
        CnDelete m_gmtAsync;
        m_gmtAsync = 0;
    }

    m_gmtAsync = CnNew TwHttpAsync;

    m_gmtAsync->SetExecutor(this);

    m_gmtAsync->Init(addrIP, addrPort);

    m_gmtAsync->Start();

    return true;
}

// ==============================
// 销毁
// ==============================
void TwExecutor::Unint()
{
    if(m_gmtAsync)
    {
        CnDelete m_gmtAsync;
        m_gmtAsync = NULL;
    }
}
