#pragma once

#ifndef FakeTransferServer_h__
#define FakeTransferServer_h__

#include "transfer.h"

class FakeNetWorkPipe;
extern FakeNetWorkPipe*	g_C2SPipe;
class FakeTransferClient;
extern FakeTransferClient*	g_FakeTransferClient;

class FakeTransferServer : public TransferServerProxy
{
public:
	FakeTransferServer();
	virtual ~FakeTransferServer();

	virtual	bool Send(uint32 SessionId, uint32 MsgId, void* pData);
	virtual void SetRecvFunc(SrvRecvFunc pRecvFunc, void* pUserData);
    virtual void DeleteSession(uint32 sessionId) {}
    virtual bool HasSession(uint32 sessionId) const {/*CnAssert(false);*/ return true;}
	virtual	void Update();   

	void OnFakeRecv( uint32 SessionId, uint32 MsgId, void* pData );
private:
	SrvRecvFunc	m_pRecvFunc;
	void*		m_pUserData;

	bool bFirstUpdate;
};

#endif // FakeTransferServer_h__
