#include "centerserver_pch.h"
#include "kunlun_pay_http_async.h"
#include "kunlun_pay_event.h"
#include "kunlun_pay.h"


#ifdef LINUX
#define SSIZE_T ssize_t
#endif

#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/http.h"


// ==============================
// KlPayAsync 构造
// ==============================
KlPayAsync::KlPayAsync()
{
    m_waitForHandle = NULL;

    m_base = NULL;
    m_http = NULL;
    m_http_handle = NULL;
}

// ==============================
// KlPayAsync 析构
// ==============================
KlPayAsync::~KlPayAsync()
{
    UnInit();
}

bool KlPayAsync::Init(const char* addrIP, uint32 addrPort)
{
    m_base = event_base_new();
    if (!m_base) 
    {
        CnError("KlPayAsync couldn't create an event_base: exiting\n");
        return 1;
    }

    m_http = evhttp_new(m_base);
    if (!m_http) 
    {
        CnError("KlPayAsync couldn't create evhttp. Exiting.\n");
        return 1;
    }

    evhttp_set_gencb(m_http, HttpdHandle, this);

    m_http_handle = evhttp_bind_socket_with_handle(m_http, addrIP, addrPort);
    if (!m_http_handle) 
    {
        CnError("KlPayAsync couldn't bind to ip %s port %d. Exiting.\n", addrIP, addrPort);
        return 1;
    }

    CnDbg("KlPayAsync Init %s:%d ok\n", addrIP, addrPort);

    return true;
}

void KlPayAsync::UnInit()
{
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

void    KlPayAsync::SetSemaphore(Crown::CSDNamedSemaphore* pSema)
{
    m_waitForHandle = pSema;
}

void    KlPayAsync::SetExecutor(KlPayExecutor* executor)
{
    m_executor = executor;
}

void    KlPayAsync::Run()
{
    if(m_bstart)
    {
        //循环处理events
        event_base_dispatch(m_base);
        CnDbg("KlPayAsync end\n");
    }
}

//处理模块
void KlPayAsync::HttpdHandle(struct evhttp_request *req, void *arg)
{
    KlPayAsync*         payAsync = (KlPayAsync *)arg;
    KlPayEvent*         payEvent = 0;
    struct evbuffer*    buf = 0;
    struct evbuffer*    evb = 0;
    char                buffer[512];
    std::string         payorder = "";
    int                 len = 0;
    uint16              errcode  = ERR_FAILED;
    Json::Reader        reader;
    Json::Value         payjson;

    CnDbg("KlPayAsync HttpdHandle\n");

    buf = evhttp_request_get_input_buffer(req);
    while(evbuffer_get_length(buf))
    {
        len = evbuffer_remove(buf, buffer, sizeof(buf)-1);
        buffer[len] = '\0';
        if (len > 0)
        {
            payorder += buffer;
        }
    }

    if(!payAsync)
    {
        CnError("payAsync is null'\n");
        evb = evbuffer_new();
        evbuffer_add_printf(evb, "{\"retcode\":109,\"retmsg\":\"internal_erro\"}");
        goto _END;
    }

    // 解析付费充值的 Json 格式
    if(!reader.parse(payorder, payjson, false))
    {
        CnError("Parse payorder failed:'%s'\n", payorder.c_str());
        evb = evbuffer_new();
        evbuffer_add_printf(evb, "{\"retcode\":107,\"retmsg\":\"internal_erro\"}");
        goto _END;
    }

    if(!payjson.isObject())
    {
        CnError("payorder is not json object:'%s'\n", payorder.c_str());
        evb = evbuffer_new();
        evbuffer_add_printf(evb, "{\"retcode\":108,\"retmsg\":\"internal_erro\"}");
        goto _END;
    }

    payEvent = new KlPayEvent;

    // 充值订单赋值
    payEvent->m_oid     = toString(payjson["oid"]);
    payEvent->m_uid     = toString(payjson["uid"]);
    payEvent->m_amount  = toString(payjson["amount"]);
    payEvent->m_coins   = toInt32(payjson["coins"]);
    payEvent->m_dtime   = toUint32(payjson["dtime"]);
    payEvent->m_sign    = toString(payjson["sign"]);
    payEvent->m_ext     = toString(payjson["ext"]);
    payEvent->m_partner = toString(payjson["partner"]);
    payEvent->m_serverid = toString(payjson["serverid"]);
    payEvent->m_paytype = toInt32(payjson["pay_type"]);

    payAsync->PushPayOrder(payEvent);

    evb = evbuffer_new();
    evbuffer_add_printf(evb, "{\"retcode\":0,\"retmsg\":\"ok\"}");

    errcode = ERR_SUCCEED;

_END:
    if(errcode == ERR_SUCCEED)
        evhttp_send_reply(req, HTTP_OK, "OK", evb);
    else
        evhttp_send_reply(req, HTTP_INTERNAL, "recharge failed", evb);
}

void KlPayAsync::PushPayOrder(KlPayEvent* payEvent)
{
    if(m_executor)
    {
        m_executor->CallbackPushBack(payEvent);
    }
}