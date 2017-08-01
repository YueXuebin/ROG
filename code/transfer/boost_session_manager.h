#pragma once

#ifndef BOOSTSESSIONMANAGER_H_
#define BOOSTSESSIONMANAGER_H_

#include <sdcontainers.h>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

class BoostSession;
class BoostTransferServer;

class BoostSessionManager
{
public:
    BoostSessionManager(short port, BoostTransferServer* proxy, byte threadCount = 0);
	virtual ~BoostSessionManager();

	void StartAccept();
	void Update();

	void Send(uint32 sessionId, uint32 msgId, void *pData);
    void Delete(uint32 sessionId);
    bool HasSession(uint32 sessionId) const;

private:
	void OnAccecpted(uint32 sessionId, const boost::system::error_code& error);
    void CreateThreads();

    void OnRecv(uint32 sessionId, uint32 msgId, void *pData);
private:
    size_t m_LastSessionIndex;
	int m_Port;
    byte m_ThreadCount;
    std::vector<Crown::shared_ptr<boost::thread> > m_Threads;

	BoostTransferServer* m_Proxy;
	boost::asio::io_service& m_Service;
    std::unique_ptr<boost::asio::io_service> m_ServiceReceiver; // 多线程时用于接收数据
	boost::asio::ip::tcp::acceptor m_Acceptor;

	CnVector<boost::shared_ptr<BoostSession> > m_Sessions;
};

#endif /* BOOSTSESSIONMANAGER_H_ */
