#include "transfer_pch.h"
#include "real_transfer_server.h"
#include "session_manager.h"
#include "packet_parser.h"

SessionManager*	SessionManager::m_Instance = NULL;

SessionManager::SessionManager() :
m_pCliSessionPool(NULL),
m_pNetModule(NULL),
m_pListener(NULL),
m_pServerProxy(NULL)
{
    memset(m_AllSessions, 0, sizeof(m_AllSessions));
}

SessionManager::~SessionManager()
{

}

bool SessionManager::Init()
{
    CnVerify(!m_Instance);
    m_Instance = CnNew SessionManager;

    return m_Instance->_Init();	
}

void SessionManager::Release()
{
    CnVerify(m_Instance);

    if( m_Instance )
    {
        m_Instance->_Release();
        CnDelete m_Instance;
        m_Instance = NULL;
    }
}

bool SessionManager::_Init()
{
    m_pNetModule = SDNetGetModule(&Crown::SDNET_VERSION);       // 创建网络模块
    CnVerify(m_pNetModule);

    CnVerify(!m_pCliSessionPool);
    m_pCliSessionPool = CnNew Crown::CSDObjectPool<Session>(1000, 1000);        // 创建SessionPool

    return true;
}

void SessionManager::_Release()
{
    for( uint32 id = 0; id < MAX_PLAYER_NUM; id++ )
    {
        if(m_AllSessions[id])
        {
            m_AllSessions[id]->GetConnection()->Disconnect();		// 服务端退出时，主动断开连接
            m_pCliSessionPool->Free(m_AllSessions[id]);				// 删除
            m_AllSessions[id] = 0;
        }
    }

    if (m_pCliSessionPool)
    {
        CnDelete m_pCliSessionPool ;
        m_pCliSessionPool = NULL;
    }

    if(m_pNetModule)
    {
        m_pNetModule->Release();
        m_pNetModule = NULL;
    }
#ifdef WIN	// linux模块会自己删除
    if(m_pListener)
    {
        CnDelete m_pListener;
        m_pListener = NULL;
    }
#endif
}

Crown::ISDSession* SessionManager::CreateSession( Crown::ISDConnection* poConnection )	// iocp线程执行
{
    Crown::CSDLock<Crown::CSDMutex> lock(m_mutex);

    Session* poSession = m_pCliSessionPool->Alloc();

    if (NULL == poSession)
    {
        CnVerify(false);
        return NULL;
    }
    // 记录该Session，并分配一个id
    uint32 id = 0;
    for( ;id < MAX_PLAYER_NUM; id++ )
    {
        if(m_AllSessions[id] == 0)
            break;
    }

    poSession->SetID(id);
    poSession->SetDecoder(m_pServerProxy->GetDecoder());
    poSession->SetTransfer(m_pServerProxy);
    m_AllSessions[id] = poSession;

    return poSession;
}

void SessionManager::ReleaseSession(uint32 SessionId)
{
    if(m_AllSessions[SessionId] != 0)
        ReleaseSession(m_AllSessions[SessionId]);
}

void SessionManager::ReleaseSession(Session* poSession)		// 主线程执行
{
    Crown::CSDLock<Crown::CSDMutex> lock(m_mutex);

    uint32 id = 0;
    for( ;id < MAX_PLAYER_NUM; id++ )
    {
        if(m_AllSessions[id] == poSession)
        {
            poSession->SetID(-1);
            m_AllSessions[id]->SetDecoder(nullptr);
            m_AllSessions[id] = nullptr;
            break;
        }
    }
    CnVerify(id < MAX_PLAYER_NUM);

    m_pCliSessionPool->Free(poSession);
}

bool SessionManager::StartListen( const char * ip, uint32 port )
{
    CnVerify(!m_pListener);
#ifdef WIN32
    m_pListener = m_pNetModule->CreateListener(Crown::NETIO_COMPLETIONPORT);	// 创建一个Listener
#else
    m_pListener = m_pNetModule->CreateListener(Crown::NETIO_EPOLL);
#endif
    CnVerify(m_pListener);
    m_pListener->SetSessionFactory(SessionManager::GetInstance());		// 设置SessionFactory
    m_pListener->SetPacketParser(PacketParser::Instance());				// 设置包解析器

    bool ret = false;
    if(ip)
        ret = m_pListener->Start(ip, port);
    else
        ret = m_pListener->Start("0", port);

    CnVerify( ret );
    return ret;
}

void SessionManager::Update()
{
    CnVerify(m_pNetModule);
    m_pNetModule->Run();
}

bool SessionManager::Send(uint32 sessionID, char* data, uint32 dataLen)
{
    CnVerify(m_AllSessions[sessionID]);
    if((sessionID >= MAX_PLAYER_NUM) || (m_AllSessions[sessionID] == NULL))
        return false;
    return m_AllSessions[sessionID]->Send(data, dataLen);
}

bool SessionManager::HasSession(uint32 sessionID)
{
    if(sessionID >= MAX_PLAYER_NUM) return false;
    return m_AllSessions[sessionID] != 0;
}

Session* SessionManager::GetSession(uint32 sessionID)
{
    if(sessionID >= MAX_PLAYER_NUM) return nullptr;
    return m_AllSessions[sessionID];
}
