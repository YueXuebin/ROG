#include "centerserver_pch.h"
#include "pusher_360_async.h"
#include "pusher_360.h"
#include "curl.h"
#include "config_loader.h"


// ==============================
// Pusher360Async
// ==============================
Pusher360Async::Pusher360Async()
{
    m_waitForHandle = NULL;
}

Pusher360Async::~Pusher360Async()
{

}

void    Pusher360Async::SetSemaphore(Crown::CSDNamedSemaphore* sema)
{
    m_waitForHandle = sema;
}

void    Pusher360Async::SetExecutor(Pusher360Executor* executor)
{
    m_executor = executor;
}

void    Pusher360Async::Run()
{
    Pusher360Event* pushEvent = NULL;
    for(;;)
    {
        m_waitForHandle->Wait();

        if(NULL != (pushEvent = m_executor->QueryPopFront()))
        {
            PushMsg(pushEvent);
            delete pushEvent;
        }
    }
}

size_t Pusher360Async::CBPush360AsyncWB(void *ptr, size_t size, size_t nmemb, void *userp)
{
    char buf[1024] = {0};
    char idstr[30] = {0};
    uint32  len = 0;
    Pusher360Event* p360Event = NULL;

    len = size * nmemb;

    Pusher360Async* pPusher = (Pusher360Async*)userp;

    if(len > 1023)
        len = 1023;

    memcpy(buf, ptr, len);
    buf[len] = '\0';

    // 执行结果返回给主线程
    p360Event = new Pusher360Event;
    p360Event->m_index = pPusher->m_Index;
    p360Event->m_type = pPusher->m_EventType;
    p360Event->m_url = pPusher->m_Url;
    p360Event->m_msg = buf;
    pPusher->m_executor->CallbackPushBack(p360Event);

    return len;
}

void    Pusher360Async::PushMsg(Pusher360Event* pushEvent)
{
    CURL *curl_handle = 0;
    CURLcode curl_code;
    std::string url = "";

    if(!pushEvent) 
        return;

    curl_handle = curl_easy_init();

    if(!curl_handle)
        return;

    m_Index = pushEvent->m_index;
    m_EventType = pushEvent->m_type;
    m_Url = pushEvent->m_url;

    curl_easy_setopt(curl_handle, CURLOPT_URL, pushEvent->m_url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 5L);
    if(pushEvent->m_IsGet)
    {
        curl_easy_setopt(curl_handle, CURLOPT_HTTPGET , 1L);
    }
    else
    {
        curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, pushEvent->m_msg.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, pushEvent->m_msg.size());
    }
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CBPush360AsyncWB);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, FALSE);


    // 就是当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出
    curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1);     // CURL多线程的严重错误  http://curl.haxx.se/libcurl/c/curl_easy_setopt.html

    curl_code = curl_easy_perform(curl_handle);
    curl_easy_cleanup(curl_handle);

    if(curl_code != CURLE_OK)
    {   // 执行错误返回
        Pusher360Event* p360Event = new Pusher360Event;
        p360Event->m_type = pushEvent->m_type;
        p360Event->m_url = pushEvent->m_url;
        p360Event->m_msg = "360 push failed:";
        p360Event->m_msg += curl_easy_strerror(curl_code);
        m_executor->CallbackPushBack(p360Event);
    }
}
