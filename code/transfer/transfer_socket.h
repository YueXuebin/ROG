#pragma once

/*
 *	客户端的异步Socket对象
 */
#ifndef TransferSocket_h__
#define TransferSocket_h__

#ifdef WIN32 // 客户端只Windows

// 接受数据的Buffer
//#define  ASYNC_SOCK_BUF_SIZE	40960

#include "message_encoder.h"

struct STNetMsgHeader;
class RealTransferClient;
class CMessageEncoder;

class TransferSocket : public Crown::CSDAsyncSocket
{
public:
	TransferSocket(RealTransferClient* pClientProxy);
	virtual ~TransferSocket();

	bool SendMsg(uint32 dwMsgID, void* pData);		// 发送协议包
    bool SendRawData(const char* pData, uint32 nLen);

    CMessageEncoder*    m_pEncoder;
protected:
	virtual void OnConnect(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnClose(int nErrorCode);

	virtual void OnOutOfBandData(int nErrorCode);
	virtual void OnAccept(int nErrorCode);

private:	
	virtual void _OnReceive(int nErrorCode);

	//bool SendDataWithHead(const STNetMsgHeader* pstHeader, void* pData);		// call SendRawData
	//bool SendRawData(const char* pData, uint32 nLen);
	

private:
	RealTransferClient*	m_pClientProxy;

	char m_szNetBuf[NET_MSG_BUFFER_LEN];
	int  m_nDataLen;

	bool m_bCanSend;
};

#endif // WIN32

#endif // TransferSocket_h__
