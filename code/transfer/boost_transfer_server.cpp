#include "transfer_pch.h"

#ifdef BOOST_NET
#include "boost_transfer_server.h"
#include "boost_session_manager.h"

BoostTransferServer::BoostTransferServer() :
	m_pRecvFunc(nullptr),
	m_pUserData(nullptr)
{

}

BoostTransferServer::~BoostTransferServer()
{
}

bool BoostTransferServer::Init(short port)
{
    m_pSessionManager.reset(CnNew BoostSessionManager(port, this, 0));
	return true;
}

bool BoostTransferServer::Send(uint32 sessionId, uint32 msgId, void *pData)
{
	try
	{
		m_pSessionManager->Send(sessionId, msgId, pData);
		return true;
	}
	catch (std::runtime_error& e)
	{
		CRITICAL(e.what());
		return false;
	}
}

void BoostTransferServer::SetRecvFunc(SrvRecvFunc func, void* pUserData)
{
	m_pRecvFunc = func;
	m_pUserData = pUserData;

    // 初始建立n个Accept
    for(int i=0;i<20;i++)
        m_pSessionManager->StartAccept();
}

bool BoostTransferServer::HasSession(uint32 sessionId) const
{
    return m_pSessionManager->HasSession(sessionId);
}

void BoostTransferServer::DeleteSession(uint32 sessionId)
{
    m_pSessionManager->Delete(sessionId);
}

void BoostTransferServer::Update()
{
    BT_PROFILE_FUNC;
	m_pSessionManager->Update();
}

void BoostTransferServer::OnRecv(uint32 sessionId, uint32 msgId, void *pData)
{
	CnVerify(m_pRecvFunc);
	m_pRecvFunc(sessionId, msgId, pData, m_pUserData);
}



#endif // BOOST_NET
