#include "centerserver_pch.h"
#include "tw_http_async.h"
#include "tw_executor.h"
#include "gm_config.h"
#include "md5.h"
#include "config_loader.h"
#include "player.h"
#include "player_manager.h"
#include "gate_handler.h"

// ==============================
// TwAsync 构造
// ==============================
TwHttpAsync::TwHttpAsync()
{
    m_base = NULL;
    m_http = NULL;
    m_http_handle = NULL;

    m_counter = 1;
}

// ==============================
// TwAsync 析构
// ==============================
TwHttpAsync::~TwHttpAsync()
{
    UnInit();
}

bool TwHttpAsync::Init(const char* addrIP, uint32 addrPort)
{
    m_base = event_base_new();
    if (!m_base) 
    {
        CnError("TwAsync couldn't create an event_base: exiting\n");
        return 1;
    }

    m_http = evhttp_new(m_base);
    if (!m_http) 
    {
        CnError("TwAsync couldn't create evhttp. Exiting.\n");
        return 1;
    }

    evhttp_set_gencb(m_http, HttpdHandle, this);

    m_http_handle = evhttp_bind_socket_with_handle(m_http, addrIP, addrPort);
    if (!m_http_handle) 
    {
        CnError("TwAsync couldn't bind to ip %s port %d. Exiting.\n", addrIP, addrPort);
        return 1;
    }

    CnDbg("TwAsync Init %s:%d ok\n", addrIP, addrPort);

    return true;
}

void    TwHttpAsync::UnInit()
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

void    TwHttpAsync::SetExecutor(TwExecutor* executor)
{
    m_executor = executor;
}

void    TwHttpAsync::Run()
{
    if(m_bstart)
    {
        //循环处理events
        int32 ret = event_base_dispatch(m_base);
        CnDbg("TwAsync end\n");
    }
}

//处理发送过来的模块
void TwHttpAsync::HttpdHandle(struct evhttp_request *req, void *arg)
{
    TwHttpAsync*        TwAsync = (TwHttpAsync *)arg;
    struct evbuffer*    buf = evbuffer_new();
    char                buffer[128] = "";
    std::string         gmcmd = "";
    int                 len = 0;
    Json::Reader        reader;
    Json::Value         cmdjson;
    std::string         cmd;
    uint32              playerNum = 0;

    CnDbg("TwAsync HttpdHandle\n");

    // 获取请求方的IP和Port
    char *clientIp;
    uint16 port;
    evhttp_connection_get_peer(evhttp_request_get_connection(req), &clientIp, &port);
    // 白名单功能关闭
    if (false && !GMConfig::Instance()->IsPermitAddr(clientIp))
    {
        CnError("Address %s not allow access.\n", clientIp);

        evbuffer_add_printf(buf, ERR_HTTP_VERIFICATION);
        evhttp_send_reply(req, HTTP_OK, "OK", buf);
        return;
    }

    if(!TwAsync)
    {
        CnError("tw mgr failed:'%s'\n", gmcmd.c_str());
        evbuffer_add_printf(buf, ERR_HTTP_VERIFICATION);
        evhttp_send_reply(req, HTTP_OK, "OK", buf);
        CnAssert(false);
        return;
    }

    // 分析请求
    const char *decode_uri = evhttp_request_get_uri(req);
    struct evkeyvalq headers;
    evhttp_parse_query(decode_uri, &headers);

    const char * charValue;
    charValue = evhttp_find_header(&headers, "type");
    std::string typeStr = charValue ? charValue : "";

    charValue = evhttp_find_header(&headers, "server_id");
    std::string serverIDStr = charValue ? charValue : "";

    charValue = evhttp_find_header(&headers, "time");
    std::string timeStr = charValue ? charValue : "";

    charValue = evhttp_find_header(&headers, "senderid");
    std::string senderidStr = charValue ? charValue : "";

    charValue = evhttp_find_header(&headers, "sendername");
    std::string sendernameStr = charValue ? charValue : "";

    charValue = evhttp_find_header(&headers, "targetid");
    std::string targetidStr = charValue ? charValue : "";

    charValue = evhttp_find_header(&headers, "targetname");
    std::string targetnameStr = charValue ? charValue : "";

    charValue = evhttp_find_header(&headers, "content");
    std::string contentStr = charValue ? charValue : "";

    charValue = evhttp_find_header(&headers, "sign");
    std::string signStr = charValue ? charValue : "";

    std::string md5Str = typeStr + serverIDStr + timeStr + contentStr + ConfigLoader::Instance()->GetLogin9377Key();
    MD5 md5;
    md5.update(md5Str);
    std::string signMd5 = md5.toString();
    if(signMd5 != signStr)
    {   // MD5不对
        CnError("tw md5 inconformity!\n");
        evbuffer_add_printf(buf, ERR_HTTP_VERIFICATION);
        evhttp_send_reply(req, HTTP_OK, "OK", buf);
        return;
    }

    // url解码
    sendernameStr = Crown::UrlDecode(sendernameStr);
    std::string playerName = Crown::UrlDecode(targetnameStr);
    contentStr = Crown::UrlDecode(contentStr);

    if(serverIDStr != ConfigLoader::Instance()->GetServerName())
    {   // 服务器ID不对
        evbuffer_add_printf(buf, ERR_HTTP_POST);
        evhttp_send_reply(req, HTTP_OK, "OK", buf);
        return;
    }

    if(typeStr == "" || serverIDStr == "" || timeStr == "")
    {   // 参数不对
        evbuffer_add_printf(buf, ERR_HTTP_ARGE);
        evhttp_send_reply(req, HTTP_OK, "OK", buf);
        return;
    }

    int32 time = atoi(timeStr.c_str());
    time = (int32)Crown::SDNow().GetTimeValue() - time;
    time = abs(time);
    if(time > 5 * 60)
    {   // 超时
        evbuffer_add_printf(buf, ERR_HTTP_TIMEOUT);
        evhttp_send_reply(req, HTTP_OK, "OK", buf);
        return;
    }

    CHAT_MSG chatMsg;
    chatMsg.errcode = ERR_SUCCEED;
    chatMsg.src_name = sendernameStr;
    chatMsg.chat = contentStr;
    if(typeStr == "1")  // 世界频道
    {
        chatMsg.channel = CHAT_CHANNEL_WORLD;
        GateHandler::Instance()->SendToAll(MSG_CHAT_ACK, &chatMsg, BROADCAST_PLAYER_ID);
    }
    else if(typeStr == "2") // 个人频道
    {
        Player* pPlayer = PlayerManager::Instance()->GetPlayerByName(targetnameStr);
        if(!pPlayer)
        {
            evbuffer_add_printf(buf, ERR_HTTP_NO_PLAYER);
            evhttp_send_reply(req, HTTP_INTERNAL, "OK", buf);
            return;
        }

        chatMsg.channel = CHAT_CHANNEL_PRIVATE;
        chatMsg.dst_name = pPlayer->GetName();

        GateHandler::Instance()->SendByID(pPlayer->GetCentGateID(), MSG_CHAT_ACK, &chatMsg, pPlayer->GetPlayerID());
    }

    evbuffer_add_printf(buf, ERR_HTTP_SUCCESS);
    evhttp_send_reply(req, HTTP_OK, "OK", buf);

    evbuffer_free(buf);
}