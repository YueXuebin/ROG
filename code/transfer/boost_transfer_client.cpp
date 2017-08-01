#include "transfer_pch.h"
#ifdef BOOST_NET
#include "boost_transfer_client.h"
#include "transfer_socket.h"
#include "protocol_define.h"

int32 g_BoostClientCount = 0;	// client connect count

using namespace boost::asio;

BoostTransferClient::BoostTransferClient() :
    m_pRecvFunc(NULL),
    m_pUserDate(NULL),
    m_Resolver(m_Service),
    m_Session(CnNew BoostSession(m_Service, boost::bind(&BoostTransferClient::OnDisconnected, this),
              boost::bind(&BoostTransferClient::OnReceived, this, _1, _2)))
{
	g_BoostClientCount++;
}

BoostTransferClient::~BoostTransferClient()
{
    g_BoostClientCount--;
}

void BoostTransferClient::SetRecvFunc( CliRecvFunc pRecvFunc, void* pUserData )
{
	CnVerify(!m_pRecvFunc);
	m_pRecvFunc = pRecvFunc;
	m_pUserDate = pUserData;

    ip::tcp::resolver::query q(m_Host, Crown::ToString(m_Port));
    ip::tcp::resolver::iterator itor = m_Resolver.resolve(q);

    async_connect(m_Session->Socket(), itor, boost::bind(&BoostTransferClient::OnConnected, this, placeholders::error));
}

bool BoostTransferClient::Init( const char* host, int32 port )
{
    m_Host = host;
    m_Port = port;

    return true;
}

void BoostTransferClient::Update()
{
    m_Service.poll();
}

void BoostTransferClient::OnConnected(const boost::system::error_code& ec)
{
	m_Session->Start();
}

bool BoostTransferClient::Send( uint32 msgId, void* pData )
{
    m_Session->Send(static_cast<EN_BnFGameProto_MessageID>(msgId), pData);
    return true;
}

void BoostTransferClient::OnReceived( uint32 MsgId, void* pData )
{
	if( m_pRecvFunc )
		m_pRecvFunc( MsgId, pData, m_pUserDate );
	else
        CnVerify(false);
}

void BoostTransferClient::OnDisconnected()
{
    OnReceived(-1, 0);
}


#endif // BOOST_NET
