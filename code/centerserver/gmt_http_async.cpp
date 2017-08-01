#include "centerserver_pch.h"
#include "gmt_http_async.h"
#include "gmt_executor.h"
#include "gmt_event.h"
#include "gm_config.h"


// ==============================
// GmtAsync 构造
// ==============================
GmtHttpAsync::GmtHttpAsync()
{
    m_waitForHandle = NULL;

    m_base = NULL;
    m_http = NULL;
    m_http_handle = NULL;

    m_counter = 1;
}

// ==============================
// GmtAsync 析构
// ==============================
GmtHttpAsync::~GmtHttpAsync()
{
    UnInit();
}

bool GmtHttpAsync::Init(const char* addrIP, uint32 addrPort)
{
    m_base = event_base_new();
    if (!m_base) 
    {
        CnError("GmtAsync couldn't create an event_base: exiting\n");
        return 1;
    }

    m_http = evhttp_new(m_base);
    if (!m_http) 
    {
        CnError("GmtAsync couldn't create evhttp. Exiting.\n");
        return 1;
    }

    evhttp_set_gencb(m_http, HttpdHandle, this);

    m_http_handle = evhttp_bind_socket_with_handle(m_http, addrIP, addrPort);
    if (!m_http_handle) 
    {
        CnError("GmtAsync couldn't bind to ip %s port %d. Exiting.\n", addrIP, addrPort);
        return 1;
    }

    CnDbg("GmtAsync Init %s:%d ok\n", addrIP, addrPort);

    return true;
}

void    GmtHttpAsync::SetSemaphore(Crown::CSDNamedSemaphore* pSema)
{
    m_waitForHandle = pSema;
}

void    GmtHttpAsync::UnInit()
{
    m_waitForHandle = NULL;

    if(m_base)
    {
        event_base_loopexit(m_base, NULL);
    }

    if(m_http_handle)
    {
        evhttp_del_accept_socket(m_http, m_http_handle);
        m_http_handle = NULL;
    }

    if(m_http)
    {
        evhttp_free(m_http);
        m_http = NULL;
    }
#ifdef WIN
    Wait();
#endif

    if(m_base)
    {
        event_base_free(m_base);
        m_base = NULL;
    }
}

void    GmtHttpAsync::SetExecutor(GmtExecutor* executor)
{
    m_executor = executor;
}

void    GmtHttpAsync::Run()
{
    if(m_bstart)
    {
        //循环处理events
        int32 ret = event_base_dispatch(m_base);
        CnDbg("GmtAsync end\n");
    }
}

//处理发送过来的模块
void GmtHttpAsync::HttpdHandle(struct evhttp_request *req, void *arg)
{
    GmtHttpAsync*       gmtAsync = (GmtHttpAsync *)arg;
    struct evbuffer*    buf = 0;
    char                buffer[128];
    std::string         gmcmd = "";
    int                 len = 0;
    Json::Reader        reader;
    Json::Value         cmdjson;
    std::string         cmd;
    uint32              playerNum = 0;
    struct evbuffer*    evb = 0;

    CnDbg("GmtAsync HttpdHandle\n");

    // 获取请求方的IP和Port
    char *clientIp;
    uint16 port;
    evhttp_connection_get_peer(evhttp_request_get_connection(req), &clientIp, &port);
    // 白名单功能关闭
    if (false && !GMConfig::Instance()->IsPermitAddr(clientIp))
    {
        CnError("Address %s not allow access.\n", clientIp);
        evhttp_send_reply(req, HTTP_INTERNAL, "Access Forbidden", evb);
        return;
    }

    // 获取请求内容
    buf = evhttp_request_get_input_buffer(req);
    while (evbuffer_get_length(buf))
    {
        len = evbuffer_remove(buf, buffer, sizeof(buf)-1);
        buffer[len] = '\0';
        if (len > 0)
        {
            gmcmd += buffer;
        }
    }

    if(!reader.parse(gmcmd, cmdjson, false))
    {
        CnError("Parse gmt command failed:'%s'\n", gmcmd.c_str());
        evhttp_send_reply(req, HTTP_INTERNAL, "Parser failed", evb);
        CnAssert(false);
        return;
    }

    if(!gmtAsync)
    {
        CnError("gmt mgr failed:'%s'\n", gmcmd.c_str());
        evhttp_send_reply(req, HTTP_INTERNAL, "gmt failed", evb);
        CnAssert(false);
        return;
    }

    // 将命令传递给主线程处理
    gmtAsync->PushCommand(gmcmd);

    // 等待主线程处理结果(使用信号量同步)
    int32 ret = gmtAsync->m_waitForHandle->Wait(300*1000);      // n分钟超时
    if(ret != SEMA_OK)
        CnError("gmt http wait time out\n");

    GmtEvent* pResultEvent = gmtAsync->PopCommand();
    while(pResultEvent)
    {
        // 返回结果
        evb = evbuffer_new();
    
        evbuffer_add_printf(evb, pResultEvent->m_cmdjson.c_str());
        if(pResultEvent->m_HttpResult == HTTP_OK)
        {
            evhttp_send_reply(req, HTTP_OK, "OK", evb);
        }
        else
        {
            evhttp_send_reply(req, pResultEvent->m_HttpResult, "ERROR", evb);
        }
   
        evbuffer_free(evb);

        // 释放事件对象
        if(pResultEvent)
        {
            delete pResultEvent;
            pResultEvent = NULL;
        }

        pResultEvent = gmtAsync->PopCommand();
    }
}
// ==============================
//处理模块
// ==============================
void GmtHttpAsync::PushCommand(std::string& cmdjson)
{
    GmtEvent* gmtEvent = new GmtEvent;

    gmtEvent->m_Counter = m_counter++;

    gmtEvent->m_cmdjson = cmdjson;

    m_executor->CallbackPushBack(gmtEvent);
}

GmtEvent* GmtHttpAsync::PopCommand()
{
    return m_executor->QueryPopFront();
}