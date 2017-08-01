#pragma once

#ifndef BOOSTSESSION_H_
#define BOOSTSESSION_H_

#include <queue>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "protocol_define.h"


class BoostSession : public boost::enable_shared_from_this<BoostSession>
{
public:
    struct Buff : public boost::array<char, MAX_PACK_LEN>
    {
        size_t LenUsed;
    };


    typedef std::function<void()> DisconnectedCallback;
    typedef std::function<void(uint32 msgId, void* pData)> ReceiveCallback;

    BoostSession(boost::asio::io_service &io_service, DisconnectedCallback dcb, ReceiveCallback rcb);
	virtual ~BoostSession();

    boost::asio::ip::tcp::socket& Socket()
	{
		return m_Socket;
	}

    void Start();
    void Send(EN_BnFGameProto_MessageID msgId, void* pData);
    void Disconnect();
    size_t GetBuffCount() const { return m_Buffs.size(); }

private:
    void StartRead();
    void ReadHeader(const boost::system::error_code& error, size_t bytesTransferred);
	void ReadBody(EN_BnFGameProto_MessageID msgId, uint32 bodyLen,
			const boost::system::error_code& error, size_t bytesTransferred);

    void OnWritten(Buff *buff, const boost::system::error_code &error);
    void PostData(uint32 msgId, void* pData);

    bool m_Disconnected;
    std::queue<Buff*> m_Buffs;

    DisconnectedCallback m_Dcb;
    ReceiveCallback m_Rcb;

	boost::asio::ip::tcp::socket m_Socket;

    Buff m_BufferIn;
    bool m_Sending;
};

#endif /* BOOSTSESSION_H_ */
