#include "transfer_pch.h"
#ifdef BOOST_NET

#include "protocol_define.h"
#include "boost_session.h"
#include "boost_session_manager.h"
#include "boost_transfer_server.h"
#include "shared_resouces.h"


using namespace boost::asio;
using boost::asio::ip::tcp;
using boost::bind;

BoostSessionManager::BoostSessionManager(short port, BoostTransferServer* proxy, byte threadCount) :
    m_LastSessionIndex(0),
	m_Port(port),
    m_ThreadCount(threadCount),
	m_Proxy(proxy),
	m_Service(SharedResouces::GetInstance().IOService()),
    m_Acceptor(m_Service) // TODO: 确认acceptor配置正确
{
    tcp::endpoint endpoint(tcp::v4(), port);
    m_Acceptor.open(endpoint.protocol());
    m_Acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    try
    {
        m_Acceptor.bind(endpoint);
        m_Acceptor.listen();
    }
    catch (const boost::system::system_error& e)
    {
        LOG_E << e.what() << std::endl;
        throw;
    }

    // If the players more than MAX_PLAYER_NUM, we crash, lalala.
    m_Sessions.resize(MAX_PLAYER_NUM);

    if (threadCount)
        m_ServiceReceiver.reset(CnNew boost::asio::io_service);
}

BoostSessionManager::~BoostSessionManager()
{
    CnDelete SharedResouces::GetInstancePtr();

    if (m_ThreadCount)
        m_Service.stop();

    for (std::size_t i = 0; i < m_Threads.size(); ++i)
        m_Threads[i]->join();
}

void BoostSessionManager::StartAccept()
{
    if (!m_Threads.size() && m_ThreadCount)
        CreateThreads();

    while (m_Sessions[m_LastSessionIndex].get())
    {
        ++m_LastSessionIndex;
        if (m_LastSessionIndex >= m_Sessions.size())
            m_LastSessionIndex = 0;
    }

    // FIXME: 无法处理超过1000人的情况
    BoostSession::DisconnectedCallback dcb = boost::bind(&BoostSessionManager::Delete, this, m_LastSessionIndex);
    BoostSession::ReceiveCallback rcb = boost::bind(&BoostSessionManager::OnRecv, this, m_LastSessionIndex, _1, _2);
    auto newSession = boost::make_shared<BoostSession>(m_Service, dcb, rcb);

    m_Sessions[m_LastSessionIndex] = newSession;
    m_Acceptor.async_accept(newSession->Socket(),
                bind(&BoostSessionManager::OnAccecpted, this, m_LastSessionIndex, placeholders::error));

}

void BoostSessionManager::OnAccecpted(uint32 sessionId, const boost::system::error_code& error)
{
	if (error)
	{
        Delete(sessionId);
        return;
	}

    m_Sessions[sessionId]->Start();
    StartAccept();
}

void BoostSessionManager::CreateThreads()
{
    for (std::size_t i = 0; i < m_ThreadCount; ++i)
    {
      Crown::shared_ptr<boost::thread> thread(CnNew boost::thread(
                                                  boost::bind(&boost::asio::io_service::run, &m_Service)));
      m_Threads.push_back(thread);
    }
}

void BoostSessionManager::OnRecv(uint32 sessionId, uint32 msgId, void *pData)
{
    if (m_ThreadCount)
        m_ServiceReceiver->post(boost::bind(&BoostTransferServer::OnRecv, m_Proxy, sessionId, msgId, pData));
    else
        m_Proxy->OnRecv(sessionId, msgId, pData);
}

void BoostSessionManager::Update()
{
    if (m_ThreadCount)
        m_ServiceReceiver->poll();
    else
    {
        BT_PROFILE("poll");
        m_Service.poll();
    }

#ifdef DEBUG
    static uint32 max = 32;
    uint32 total = 0;
    for (auto itor = m_Sessions.begin(); itor != m_Sessions.end(); ++itor)
    {
        if (*itor)
            total += (*itor)->GetBuffCount();
    }
    if (total > max)
    {
        max *= 2;
        DBG("Buff Count %u", total);
    }
#endif
}

void BoostSessionManager::Send(uint32 sessionId, uint32 msgId, void *pData)
{
	if (m_Sessions[sessionId])
        m_Sessions[sessionId]->Send(static_cast<EN_BnFGameProto_MessageID>(msgId), pData);
}

void BoostSessionManager::Delete(uint32 sessionId)
{
    Crown::shared_ptr<BoostSession>& session = m_Sessions[sessionId];
    if (session)
    {
        session->Disconnect();
        session.reset();
    }
}

bool BoostSessionManager::HasSession(uint32 sessionId) const
{
    return m_Sessions[sessionId];
}


#endif // BOOST_NET
