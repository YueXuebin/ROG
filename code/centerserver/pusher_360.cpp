#include "centerserver_pch.h"
#include "pusher_360.h"
#include "pusher_360_async.h"
#include "curl.h"
#include "centerserver.h"
#include "gate_handler.h"


IMPLEMENT_ALLOCATOR(Pusher360Event)

IMPLEMENT_SINGLETON(Pusher360Executor)


// ==============================
// Pusher360Executor
// ==============================
Pusher360Executor::Pusher360Executor()
{
    uint32 pid = Crown::SDGetCurrentProcessId();
    std::string SemaName = "Pusher360Executor_" + Crown::ToString(pid);

    m_Pusher360Sema = Crown::CreateNamedSemaphore(SemaName.c_str(), 0);
    CnAssert(m_Pusher360Sema);
    CnInfo("create semaphore %s\n", SemaName.c_str());

    m_pushAsyncList = 0;
}

Pusher360Executor::~Pusher360Executor()
{
    Unint();
}

bool Pusher360Executor::Init(int32 threadNum)
{
    curl_global_init(CURL_GLOBAL_ALL);

    if(m_pushAsyncList)
    {
        CnDelete[] m_pushAsyncList;
        m_pushAsyncList = 0;
    }

    m_pushAsyncList = CnNew Pusher360Async[threadNum];

    // 初始化异步执行
    for (int32 i=0; i<threadNum; ++i)
    {
        m_pushAsyncList[i].SetSemaphore(m_Pusher360Sema);

        m_pushAsyncList[i].SetExecutor(this);
    }

    // 开始异步线程
    for (int32 i=0; i<threadNum; ++i)
    {
        m_pushAsyncList[i].Start();
    }

    return true;
}

void Pusher360Executor::Unint()
{
    Crown::DestoryNamedSemaphore(m_Pusher360Sema);
    m_Pusher360Sema = NULL;

    if(m_pushAsyncList) 
        CnDelete[] m_pushAsyncList;

    curl_global_cleanup();
}

void Pusher360Executor::Update()
{
    for(Pusher360Event* p360Event = CallbackPopFront(); p360Event; p360Event = CallbackPopFront())
    {
        if(!p360Event)
        {
            CnAssert(false);
            continue;
        }

        OnPushMsg(p360Event);

        delete p360Event;
    }
}

void Pusher360Executor::OnPushMsg(Pusher360Event* p360Event)
{
    switch(p360Event->m_type)
    {
    case PUSH_360_CHAT:
    case PUSH_360_LEVEL:
        {
            CnDbg("%d:push 360:%s\n", p360Event->m_index, p360Event->m_msg.c_str());

            int32   errcode = 0;
            std::string errmsg;
            Json::Reader reader;
            Json::Value json_val;
            json_val.clear();
            if(reader.parse(p360Event->m_msg.c_str(), json_val, false))
            {
                if(json_val.isObject() && json_val.isMember("errno"))
                    errcode = toInt32(json_val["errno"]);
                if(json_val.isObject() && json_val.isMember("errmsg"))
                    errmsg = toString(json_val["errmsg"]);
            }
            
            if(p360Event->m_type == PUSH_360_LEVEL)
                CnDbg("%d:push 360 level:%d %s\n", p360Event->m_index, errcode, errmsg.c_str());
            else
                CnDbg("%d:push 360 chat:%d %s\n", p360Event->m_index, errcode, errmsg.c_str());
        }
        break;
    case PUSH_KUGOU_CREATE_CHARACTER:
        CnDbg("%d:kugou create character:%s\n", p360Event->m_index, p360Event->m_msg.c_str());
        break;
    case PUSH_TW_CHAT:
        CnDbg("%d:push tw:%s\n", p360Event->m_index, p360Event->m_msg.c_str());
        break;
    default:
        CnAssert(false);
        break;
    }
}

bool Pusher360Executor::PushMsg(const std::string& url, int type, const std::string& msg, bool isGet)
{
    Pusher360Event* pushEvent = 0;

    static uint32 s_index = 0;
    pushEvent = new Pusher360Event;

    // 360 推送消息
    pushEvent->m_index = s_index++;
    pushEvent->m_type = type;
    pushEvent->m_url = url;
    pushEvent->m_msg = msg;
    pushEvent->m_IsGet = isGet;

    CnDbg("%d:type=%d msg=%s\n", pushEvent->m_index, type, msg.c_str());

    QueryPushBack(pushEvent);

    return true;
}

Pusher360Event* Pusher360Executor::QueryPopFront()
{
    Pusher360Event* loginEvent = NULL;
    QueryLock();
    if(!m_queryList.empty())
    {
        loginEvent = m_queryList.front();
        m_queryList.pop_front();
    }
    QueryUnLock();
    return loginEvent;
}

void Pusher360Executor::QueryPushBack(Pusher360Event* pEvent)
{
    QueryLock();
    m_queryList.push_back(pEvent);
    m_Pusher360Sema->Post();
    QueryUnLock();
}

Pusher360Event* Pusher360Executor::CallbackPopFront()
{
    Pusher360Event* pEvent = NULL;
    CallbackLock();
    if(!m_callbackList.empty())
    {
        pEvent = m_callbackList.front();
        m_callbackList.pop_front();
    }
    CallbackUnLock();
    return pEvent;
}

void Pusher360Executor::CallbackPushBack(Pusher360Event* pEvent)
{
    CallbackLock();
    m_callbackList.push_back(pEvent);
    CallbackUnLock();
}
