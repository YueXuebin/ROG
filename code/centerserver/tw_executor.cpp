#include "centerserver_pch.h"
#include "tw_executor.h"
#include "config_loader.h"
#include "tw_http_async.h"

IMPLEMENT_SINGLETON(TwExecutor)

// ==============================
// ���캯��
// ==============================
TwExecutor::TwExecutor()
{
    uint32 pid = Crown::SDGetCurrentProcessId();
    std::string SemaName = "TwExecutor_" + Crown::ToString(pid);

    CnInfo("create semaphore %s\n", SemaName.c_str());

    m_gmtAsync = NULL;
}

// ==============================
// ��������
// ==============================
TwExecutor::~TwExecutor()
{
    //Unint();
}

// ==============================
// ��ʼ��
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
// ����
// ==============================
void TwExecutor::Unint()
{
    if(m_gmtAsync)
    {
        CnDelete m_gmtAsync;
        m_gmtAsync = NULL;
    }
}
