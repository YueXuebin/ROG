#include "transfer_pch.h"
#include "fake_transfer_client.h"
#include "fake_transfer_server.h"
#include "fake_network_pipe.h"

FakeNetWorkPipe*	g_S2CPipe = 0;

FakeTransferServer*	g_FakeTransferServer = 0;

FakeTransferServer::FakeTransferServer() :
m_pRecvFunc(0),
m_pUserData(0),
bFirstUpdate(false)
{
	if (!g_S2CPipe)
		g_S2CPipe = CnNew FakeNetWorkPipe;
}

FakeTransferServer::~FakeTransferServer()
{
	CnDelete g_S2CPipe;
	g_S2CPipe = nullptr;
}

void FakeTransferServer::SetRecvFunc( SrvRecvFunc pRecvFunc, void* pUserData)
{
	m_pRecvFunc = pRecvFunc;
	m_pUserData = pUserData;
}

bool FakeTransferServer::Send( uint32 SessionId, uint32 MsgId, void* pData )
{
	//if( NETPACK_DELAY_MAX == 0 )	// 无延时
	//{
	//	if(g_FakeTransferClient)
	//		g_FakeTransferClient->OnFakeRecv(MsgId, pData);
	//	else
	//		CnVerify(false);
	//	return true;
	//}

	// 数据打包
 	//char pBuf[MAX_PACK_LEN];	
 	//char* pBodyData = pBuf + sizeof(STNetMsgHeader);
 	//int32 nBodyLen = MAX_PACK_LEN - sizeof(STNetMsgHeader);
 	//int32 nEncodeLen = g_BnFGameProto.Encode( MsgId, pData, pBodyData, nBodyLen);
 
 	// 整理包头
 	//STNetMsgHeader* pTempHeader = reinterpret_cast<STNetMsgHeader*>(pBuf);
 	//int32 nTotalLen = nEncodeLen + sizeof(STNetMsgHeader);
  //  pTempHeader->dwMsgID = MsgId;
  //  pTempHeader->dwDataLen = nEncodeLen;
 
 	//g_S2CPipe->Send(pBuf, nTotalLen);

	return true;
}


void FakeTransferServer::Update()
{
	if(!bFirstUpdate)
	{	// 伪造 Client连接上的通知
		OnFakeRecv(0, 0, 0);	// sessionId = 0
		bFirstUpdate = true;
	}

	char	pData[MAX_PACK_LEN];
	uint32	len = 0;
	while( g_C2SPipe && ( (len = g_C2SPipe->Recv(pData, MAX_PACK_LEN)) > 0 )  )
	{ 
		// 解包头
		const STNetMsgHeader* pstHeader = reinterpret_cast<STNetMsgHeader*>(const_cast<char *>(pData));
        uint32 wMsgID = pstHeader->dwMsgID;
        uint32 dwDataLen = pstHeader->dwDataLen;

		// Decode data.
		char* pBodyData = const_cast<char*>(pData + sizeof(STNetMsgHeader));
		int32 nBodyLen = dwDataLen + sizeof(STNetMsgHeader);

		//char pData[MAX_PACK_LEN];
		//int32 nDecodeLen = g_BnFGameProto.Decode( wMsgID, pBodyData, nBodyLen, pData, MAX_PACK_LEN);
		//CnVerify(nDecodeLen>0);

		//if(m_pRecvFunc)
		//{
		//	(*m_pRecvFunc)( 0, wMsgID, pData, m_pUserData );
		//}
		//else
		//	CnVerify(false);
	}
}

void FakeTransferServer::OnFakeRecv( uint32 SessionId, uint32 MsgId, void* pData )
{
	if(m_pRecvFunc)
	{
		(*m_pRecvFunc)(SessionId, MsgId, pData, m_pUserData);
	}
	else
		CnVerify(false);
}
