#include "centerserver_pch.h"
#include "config_loader.h"
#include "platform_callbacker.h"
#include "platform_kugou_callbacker.h"
#include "platform_360_callbacker.h"
#include "platform_tw_callbacker.h"

PlatformCallbacker* m_pCallbacker;

PlatformCallbacker* GetPlatformCallbacker()
{
    CnAssert(m_pCallbacker);
    return m_pCallbacker;
}

void SetPlatformCallbacker(PlatformCallbacker* pCallbacker)
{
    CnAssert(m_pCallbacker);
    CnAssert(pCallbacker);

    if(m_pCallbacker)
    {
        CnDelete m_pCallbacker;
    }
    m_pCallbacker = pCallbacker;
}

bool InitPlatformCallbacker()
{
    CnAssert(!m_pCallbacker);

    int32 loginMode = ConfigLoader::Instance()->GetLoginMode();
    switch(loginMode)
    {
    case LOGIN_MODE_TEST:
        m_pCallbacker = CnNew PlatformCallbacker;
        break;
    case LOGIN_MODE_KUGOU:
        m_pCallbacker = CnNew PlatformKugouCallbacker;
        break;
    case LOGIN_MODE_360:
        m_pCallbacker = CnNew Platform360Callbacker;
        break;
    case LOGIN_MODE_TW:
        m_pCallbacker = CnNew PlatformTwCallbacker;
        break;
    default:
        m_pCallbacker = CnNew PlatformCallbacker;
        break;
    }

    return true;
}

void UninitPlatformCallbacker()
{
    CnAssert(m_pCallbacker);
    CnDelete m_pCallbacker;
    m_pCallbacker = NULL;
}
