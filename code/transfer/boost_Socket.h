#ifndef BOOSTSOCKET_H
#define BOOSTSOCKET_H


#include <boost/asio.hpp>
#include <protocol_define.h>

class BoostSocket
{
public:
    BoostSocket(DisconnectedCallback dcb, ReceiveCallback rcb);

    boost::asio::ip::tcp::socket& GetSocket() { return m_Socket; }

    void Start();
    void Send(EN_BnFGameProto_MessageID msgId, void* pData);
    void Disconnect();


private:
    DisconnectedCallback m_Dcb;
    ReceiveCallback m_Rcb;

    bool m_Disconnected;
    std::queue<Buff*> m_Buffs;

    boost::asio::ip::tcp::socket m_Socket;

    Buff m_BufferIn;
};

#endif // BOOSTSOCKET_H
