#include "transfer_pch.h"
#ifdef BOOST_NET
#include "transfer.h"
#include "boost_session.h"
#include <stdexcept>
#include <iostream>
#include <fstream>

using std::ofstream;
using std::ios;

Crown::CSDObjectPool<BoostSession::Buff, Crown::CSDNonMutex, Crown::PoolAllocator<BoostSession::Buff> > g_SessionBuffPool(500, 100);

using namespace boost::asio;

BoostSession::BoostSession(boost::asio::io_service& io_service, DisconnectedCallback dcb, ReceiveCallback rcb) :
    m_Disconnected(false), m_Socket(io_service), m_Dcb(dcb), m_Rcb(rcb), m_Sending(false)
{
}

BoostSession::~BoostSession()
{
    while(!m_Buffs.empty())
    {
        g_SessionBuffPool.Free(m_Buffs.front());
        m_Buffs.pop();
    }
}

void BoostSession::Start()
{
    boost::system::error_code e;
    // Turn off the Nagle algorithm.
    m_Socket.set_option(ip::tcp::no_delay(true), e);
    if (e)
    {
        LOG_D << e.message() << std::endl;
        Disconnect();
        return;
    }

    PostData(LOGIN, 0);

    StartRead();
}

void BoostSession::Send(EN_BnFGameProto_MessageID msgId, void *pData)
{
    BT_PROFILE_FUNC;
    Buff* buff;
    {
        BT_PROFILE("new buff");
        
        //buff = CnNew Buff;
        buff = g_SessionBuffPool.Alloc();
    }
    

    char* pBody = buff->data() + sizeof(STNetMsgHeader);
	uint32 bodyMaxLen = MAX_PACK_LEN - sizeof(STNetMsgHeader);
	int encodedLen = g_MessageEncoder.Encode(msgId, pData, pBody, bodyMaxLen);
	if(encodedLen <= 0)
		throw std::runtime_error("Encode error.");

	int32 totalLen = encodedLen + sizeof(STNetMsgHeader);
    STNetMsgHeader* pTempHeader = reinterpret_cast<STNetMsgHeader*>(buff->data());
    pTempHeader->dwMsgID = htonl(msgId);
    pTempHeader->dwDataLen = htonl(encodedLen);
    buff->LenUsed = totalLen;

    if (!(totalLen <= MAX_PACK_LEN))
    {   // 收到数据不对，断开连接
        Disconnect();
        return;
    }

    if (!g_ProtoRecord.empty())
    {
        ofstream myFile;

        myFile.open(g_ProtoRecord, ios::app | ios::binary);
        myFile.write((const char*)pData, totalLen);
        myFile.close();
    }

    // 保证同一时间只有一个异步写操作.
    if (m_Sending)
    {
        m_Buffs.push(buff);
    }
    else
    {
        BT_PROFILE("async_write");

        m_Sending = true;

        async_write(m_Socket, buffer(buff, buff->LenUsed),
                boost::bind(&BoostSession::OnWritten, shared_from_this(), buff, placeholders::error));
    }
}

void BoostSession::OnWritten(Buff* buff, const boost::system::error_code& error)
{
    BT_PROFILE_FUNC;
	g_SessionBuffPool.Free(buff);

	if (error)
	{
		LOG_D << error.message() << std::endl;
		Disconnect();
		return;
	}

	if (m_Buffs.empty())
    {
        m_Sending = false;
        return;
    }

    m_Sending = true;
	buff = m_Buffs.front();
	m_Buffs.pop();
	CnAssert(buff->LenUsed <= MAX_PACK_LEN);

    while (!m_Buffs.empty())
    {
        Buff* anotherBuff = m_Buffs.front();

        if (anotherBuff->LenUsed + buff->LenUsed >= MAX_PACK_LEN)
            break;

        memcpy(buff->data() + buff->LenUsed, anotherBuff->data(), anotherBuff->LenUsed);
        buff->LenUsed += anotherBuff->LenUsed;
        m_Buffs.pop();
        g_SessionBuffPool.Free(anotherBuff);
    }

    {
        BT_PROFILE("OnWritten::async_write");
        async_write(m_Socket, buffer(buff, buff->LenUsed),
            boost::bind(&BoostSession::OnWritten, shared_from_this(), buff, placeholders::error));
    }
}

void BoostSession::StartRead()
{
    async_read(m_Socket, buffer(m_BufferIn, sizeof(STNetMsgHeader)),
            boost::bind(&BoostSession::ReadHeader, shared_from_this(),
                    placeholders::error,
                    placeholders::bytes_transferred));
}

void BoostSession::ReadHeader(const boost::system::error_code& error, size_t /*bytesTransferred*/)
{
    BT_PROFILE_FUNC;

    if (error)
	{
        LOG_D << error.message() << std::endl;
        Disconnect();
		return;
	}

    const STNetMsgHeader* pstHeader = reinterpret_cast<STNetMsgHeader*>(m_BufferIn.data());
    uint32 msgId = ntohl(pstHeader->dwMsgID);
    uint32 bodyLen = ntohl(pstHeader->dwDataLen);
    //CnAssert(bodyLen <= MAX_PACK_LEN);
    if(!(bodyLen <= MAX_PACK_LEN))
    {
        Disconnect();
        return;
    }

	async_read(m_Socket, buffer(m_BufferIn, bodyLen),
			boost::bind(&BoostSession::ReadBody, shared_from_this(),
				    static_cast<EN_BnFGameProto_MessageID>(msgId), bodyLen,
				    placeholders::error,
				    placeholders::bytes_transferred));
}

void BoostSession::ReadBody(EN_BnFGameProto_MessageID msgId, uint32 bodyLen,
				const boost::system::error_code& error, size_t bytesTransferred)
{
    BT_PROFILE_FUNC;

	if (error)
	{
        LOG_D << error.message() << std::endl;
        Disconnect();
		return;
	}

    if (m_Disconnected)
        return;

    CnVerify(bytesTransferred == bodyLen);
	char pData[MAX_PACK_LEN];
    int decodedLen = g_MessageEncoder.Decode(msgId, m_BufferIn.data(), bodyLen, pData, MAX_PACK_LEN);
	if (decodedLen <= 0)
	{
        WARN("Protocol error.");
        Disconnect();
		return;
	}

    if (!g_ProtoRecord.empty())
    {
        ofstream myFile;

        myFile.open(g_ProtoRecord, ios::app | ios::binary);
        myFile.write(m_BufferIn.data(), bodyLen + sizeof(STNetMsgHeader));
        myFile.close();
    }

    PostData(msgId, pData);
    StartRead();
}

void BoostSession::Disconnect()
{
    if(m_Disconnected)
        return;
    m_Disconnected = true;

    PostData(LOGOUT, 0);

	// shutdown() would change the `error` if an error occurred, instead of throwing an exception.
	boost::system::error_code error;
	m_Socket.shutdown(ip::tcp::socket::shutdown_both, error);
	m_Socket.close();

    m_Dcb();
}

void BoostSession::PostData(uint32 msgId, void *pData)
{
    m_Rcb(msgId, pData);
}

#endif // BOOST_NET
