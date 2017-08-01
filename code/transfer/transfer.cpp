#include "transfer_pch.h"
#include "transfer.h"
#include "fake_transfer_client.h"
#include "fake_transfer_server.h"
#include "net_event.h"
#include "net_manager.h"
#include "message_encoder.h"

uint32 g_transfer_id = 0;
std::map<uint32, TransferProxy*> g_transfer_map;

#ifdef BOOST_NET
#include "boost_transfer_client.h"
#include "boost_transfer_server.h"
#endif

#ifdef REAL_NET
#include "real_transfer_client.h"
#include "real_transfer_server.h"
#endif

#include "af_transfer_client.h"
#include "af_transfer_server.h"

#include "protocol_define.h"


// flash 验证线程相关
bool IsThreadRunning = true;
Crown::SDTHREADID flashThreadId = 0;
void InitFlashCheckThread();
SDTHREAD_DECLARE(AuthenticateFlashThread)(void *);

std::string g_ProtoRecord = "";

std::vector<TransferProxy*> m_AllProxy;

TransferProxy::TransferProxy()
{
    m_AllProxy.push_back(this);
}

TransferProxy::~TransferProxy()
{
    for(std::vector<TransferProxy*>::iterator iter = m_AllProxy.begin(); iter != m_AllProxy.end(); iter++)
    {
        if(*iter == this)
        {
            m_AllProxy.erase(iter);
            break;
        }
    }
}

TransferClientProxy* CreateTransferClientProxy(const char* ip, int32 port, TransferRecvFunc func, void* userdata, bool useBigQueue)
{
    TransferClientProxy* proxy = NULL;
    proxy = CnNew AFTransferClientProxy(ip, port, func, userdata, useBigQueue);
    if(!proxy->IsValid())
    {
        CnDelete proxy;
        proxy = NULL;
    }
    return proxy;
}

TransferServerProxy* CreateTransferServerProxy(const char* ip, int32 port, TransferRecvFunc func, void* userdata)
{
    TransferServerProxy* proxy = NULL;
    proxy = CnNew AFTransferServerProxy(ip, port, func, userdata);
    if(!proxy->IsValid())
    {
        CnDelete proxy;
        proxy = NULL;
    }
    return proxy;
}

void DestoryTransferClientProxy(TransferClientProxy* pProxy)
{
    if(pProxy)
        CnDelete pProxy;
}

void DestoryTransferServerProxy(TransferServerProxy* pProxy)
{
    if(pProxy)
        CnDelete pProxy;
}

void TransferUnpdate()
{
    CMessageEncoder::m_TimeCounter = Crown::SDGetTickCount();

    Net_Event* event = nullptr;

    if(!Net_Manager::Instance())
    {
        return;
    }

    // 处理网络事件
    while(event = Net_Manager::Instance()->get_event())
    {
        switch(event->net_event_type)
        {
        //TYPE_NULL,              //!< 空事件
        //TYPE_DATA,              //!< 数据到达
        //TYPE_CONNECT,           //!< tcp连接成功
        //TYPE_ACCEPT,            //!< tcp接受连接
        //TYPE_CLOSE,             //!< tcp连接关闭
        //TYPE_LISTEN_CLOSE,      //!<侦听socket关闭
        //TYPE_EXCEPTION,         //!< 异常
        //TYPE_LISTEN_EXCEPTION,  //!<侦听socket异常
        //TYPE_TIMEOUT,           //!< 超时
        //TYPE_SYS_ERROR          //!< 系统异常
        case TYPE_DATA:
            {
                if(event->UserData)
                {
                    ((TransferProxy*)event->UserData)->BeforeDecode(&event->packet);

                    if(CMessageEncoder::Instance()->Decode(event->id, &event->packet, (TransferProxy*)event->UserData) < 0)
                    {
                        // Decode error close net
                        //((TransferProxy*)event->UserData)->Recv(0, event->id, MSG_NET_CLOSE, 0);      // 连接的是GateServer，不要断开连接
                    }
                }
                //MessageEncoder::
                //net_event->packet.remote_addr = net_event->remote_addr;
                //OnData(net_event->id, &net_event->packet);
            }
            break;
        case TYPE_ACCEPT:
            {
                if(event->UserData)
                {
                    ((TransferProxy*)event->UserData)->Recv(0, event->new_id, MSG_NET_ACCEPT, 0);
                }
            }
            break;
        case TYPE_CONNECT:
            {
                if(event->UserData)
                {
                    ((TransferProxy*)event->UserData)->Recv(0, event->id, MSG_NET_CONNECT, 0);
                }
            }
            break;
        case TYPE_CLOSE:
        case TYPE_LISTEN_CLOSE:
            {
                //m_fd = 0;
                //m_gatewayReconnectTime = (int)time(NULL) + GATEWAY_HANDLE_CONNECT_TIME;
                if(event->UserData)
                {
                    ((TransferProxy*)event->UserData)->Recv(0, event->id, MSG_NET_CLOSE, 0);
                }
            }
            break;
        case TYPE_EXCEPTION:
        case TYPE_LISTEN_EXCEPTION:
        case TYPE_TIMEOUT:
        case TYPE_SYS_ERROR:
            {
                if(event->UserData)
                {
                    ((TransferProxy*)event->UserData)->Recv(0, event->id, MSG_NET_ERROR, 0);
                }
            }
            break;
        case TYPE_NULL:
            {
                
            }
            break;
        default:
            break;
        }
        delete event;
        event = nullptr;
    }

    // update all proxy
    for(std::vector<TransferProxy*>::iterator iter = m_AllProxy.begin(); iter != m_AllProxy.end(); iter++)
    {
        (*iter)->Update();
    }
}

int32 TransferSend(uint32 sessionID, uint32 msgID, void* data, uint64 playerID, TransferProxy* pProxy)
{
    Net_Packet* packet = new Net_Packet;
    if(CMessageEncoder::Instance()->Encode(playerID, sessionID, msgID, data, packet))
    {
        delete packet;
        return -1;
    }

    if(pProxy)
        pProxy->AfterEncode(packet);

    if(Net_Manager::Instance()->send_packet(sessionID, packet))
    {
        delete packet;
        return -1;
    }

    return 0;
}

bool TransferSendPacket(uint32 sessionID, Net_Packet* packet, TransferProxy* pProxy)
{
    if(pProxy)
        pProxy->AfterEncode(packet);

    if(Net_Manager::Instance()->send_packet(sessionID, packet))
    {
        return false;
    }

    return true;
}

SDTHREAD_DECLARE(AuthenticateFlashThread)(void *)
{
    int ListenSocket; 
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket < 0)
    {
#ifdef WIN
        DBG("flash thread socket failed err=%d", WSAGetLastError());
#else
        int32 err=errno;
        DBG("flash thread socket failed err=%d", err);
        if(err == EACCES)
        {
            DBG("may be you need use sudo");
        }
#endif
        SDTHREAD_RETURN(0);
    }

    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("0.0.0.0");
    service.sin_port = htons(843);

    if (::bind( ListenSocket, (sockaddr*) &service, sizeof(service)) < 0)
    {
#ifdef WIN
        DBG("flash thread bind failed 2");
        closesocket(ListenSocket);
#else
        DBG("flash thread bind failed err=%d", errno);
        close(ListenSocket);
#endif
        SDTHREAD_RETURN(0);
    }

    if (listen( ListenSocket, SOMAXCONN ) < 0)
    {
#ifdef WIN
        DBG("flash thread listen failed 3");
#else
        DBG("flash thread listen failed err=%d", errno);
#endif
        close(ListenSocket);
        SDTHREAD_RETURN(0);
    }

    INFO("flash thread listen success!");
    while(IsThreadRunning)
    {
        // Create a SOCKET for accepting incoming requests.
        int AcceptSocket = 0;

        // Accept the connection.
        AcceptSocket = accept( ListenSocket, NULL, NULL );	// block here
        if (AcceptSocket < 0)
        {
#ifdef WIN
            DBG("flash thread accept failed 4");
#else
            DBG("flash thread accept failed err=%d", errno);
#endif
            close(ListenSocket);
            SDTHREAD_RETURN(0);
        }

        const int buflen = 512;
        char recvbuf[buflen];
        int recvbuflen = buflen;
        const char *sendbuf = "<cross-domain-policy><allow-access-from domain=\"*\" to-ports=\"*\" /></cross-domain-policy>";

        int iResult = recv(AcceptSocket, recvbuf, recvbuflen, 0);
        if ( iResult >= 0 )
        {
            iResult = send( AcceptSocket, sendbuf, (int)strlen(sendbuf), 0 );
        }

#ifdef WIN
        closesocket(AcceptSocket);
#else
        close(AcceptSocket);
#endif
    }
    DBG("flash thread listen end!");
    SDTHREAD_RETURN(0);
}

void InitFlashCheckThread()
{
    Crown::SDCreateThread(NULL, AuthenticateFlashThread, NULL, &flashThreadId);
}
