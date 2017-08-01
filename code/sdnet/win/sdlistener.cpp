#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "sdlistener.h"

CSDListener::CSDListener()
{
	m_poPacketParser	= NULL;
	m_poSessionFactory	= NULL;
	m_dwRecvBufSize		= DEFAULT_RECVBUF_SIZE;
	m_dwSendBufSize		= DEFAULT_SENDBUF_SIZE;
	m_poCpListener		= NULL;
	m_bStart			= false;
}

CSDListener::~CSDListener()
{
	if(m_poCpListener)
		CnDelete m_poCpListener;
}

void CSDListener::SetPacketParser(ISDPacketParser* poPacketParser)
{
	m_poPacketParser = poPacketParser;
}

void CSDListener::SetSessionFactory(ISDSessionFactory* poSessionFactory)
{
	m_poSessionFactory = poSessionFactory;
}

void CSDListener::SetBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize)
{
	m_dwRecvBufSize	= dwRecvBufSize;
	m_dwSendBufSize	= dwSendBufSize;
}

void CSDListener::SetOpt(uint32 dwType, void* pOpt)
{

}

bool CSDListener::Start(const char* pszIP, uint16 wPort, bool bReUseAddr)
{
	if(m_bStart)
	{
		return true;
	}

    WARN("CSDListener::Start Listen %s:%d, ReuseAddr %d", pszIP, wPort, bReUseAddr);

	if(NULL == m_poSessionFactory)
	{
		return false;
	}

    m_poCpListener = CnNew CCpListener;
	if(NULL == m_poCpListener)
	{
		CRITICAL("CSDListener::Start, new CCpListener failed");
		return false;
	}

	m_poCpListener->SetID(0);
	m_poCpListener->SetBufferSize(m_dwRecvBufSize, m_dwSendBufSize);
	m_poCpListener->SetPacketParser(m_poPacketParser);
	m_poCpListener->SetSessionFactory(m_poSessionFactory);
	if(false == m_poCpListener->Start(pszIP, wPort, bReUseAddr))
	{
		return false;
	}

	m_bStart = true;

	return true;
}

bool CSDListener::Stop(void)
{
	if(false == m_bStart)
	{
		return true;
	}

    WARN("CSDListener::Stop");

	if(m_poCpListener != NULL)
	{
		m_poCpListener->Stop();
		//Sleep(500);
		CnDelete m_poCpListener;
		m_poCpListener = NULL;
	}

	m_bStart = false;
	return true;
}

void CSDListener::Release(void)
{
	CnDelete this;
}
