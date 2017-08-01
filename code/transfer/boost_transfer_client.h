/*
*	真实传输层客户端
*/

#ifndef BoostTransferClient_h__
#define BoostTransferClient_h__


#include <boost/asio.hpp>
#include "transfer.h"
#include "boost_session.h"

class BoostSession;

class BoostTransferClient : public TransferClientProxy
{
    friend class TransferSocket;
public:
    BoostTransferClient();
    virtual ~BoostTransferClient();

    bool    Init(const char* host, int32 port);

    virtual bool Send(uint32 MsgId, void* pData);
    virtual void SetRecvFunc(CliRecvFunc pRecvFunc, void* pUserData);

    virtual	void Update();
private:
    void    OnConnected(const boost::system::error_code& ec);
    void    OnReceived(uint32 MsgId, void* pData);
    void    OnDisconnected();

    std::string             m_Host;
    int32                   m_Port;

    CliRecvFunc             m_pRecvFunc;
    void*                   m_pUserDate;
    boost::asio::io_service m_Service;
    boost::asio::ip::tcp::resolver m_Resolver;

    Crown::shared_ptr<BoostSession> m_Session;
};

#endif // BoostTransferClient_h__
