#pragma once

/*
*	伪传输层
*/

#include "transfer.h"

class FakeNetWorkPipe;
extern FakeNetWorkPipe*	g_S2CPipe;
class FakeTransferServer;
extern FakeTransferServer*	g_FakeTransferServer;

class FakeTransferClient : public TransferClientProxy
{
public:
	FakeTransferClient();
	virtual ~FakeTransferClient();

	virtual	bool Send(uint32 MsgId, void* pData);
	virtual void SetRecvFunc(CliRecvFunc pRecvFunc, void* pUserData );

	virtual	void Update();

	void OnFakeRecv( uint32 MsgId, void* pData );
private:
	CliRecvFunc	m_pRecvFunc;
	void*	m_pUserData;
	bool bFirstUpdate;

};
