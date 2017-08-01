#pragma once

#ifndef BOOSTTRANSFERSERVER_H_
#define BOOSTTRANSFERSERVER_H_

#include "transfer.h"
#include "protocol_define.h"

class BoostSessionManager;

class BoostTransferServer : public TransferServerProxy
{
public:
	BoostTransferServer();
	virtual ~BoostTransferServer();

	virtual bool Init(short port);
	virtual bool Send(uint32 SessionId, uint32 MsgId, void* pData);
	virtual void SetRecvFunc(SrvRecvFunc, void* );
    virtual bool HasSession(uint32 sessionId) const;
    virtual void DeleteSession(uint32 sessionId);

	virtual	void Update();

	void OnRecv(uint32 sessionId, uint32 msgId, void* pData);
private:
	std::unique_ptr<BoostSessionManager> m_pSessionManager;
	SrvRecvFunc m_pRecvFunc;
	void* m_pUserData;

};

#endif /* BOOSTTRANSFERSERVER_H_ */
