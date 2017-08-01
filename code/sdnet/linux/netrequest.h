#ifndef NETREQUEST_H
#define NETREQUEST_H

#include "sdnet.h"

using namespace Crown;

enum ENetReqType
{
	NETREQ_INVALID = 0,
	NETREQ_START_LISTEN = 1,
	NETREQ_STOP_LISTEN,
	NETREQ_CONNECT,
	NETREQ_DISCONN,
	NETREQ_SEND,
	NETREQ_SOCKOPT,
};

struct SStartListenReq
{
	uint32				dwListenerID;
	int32				hListenSock;
	ISDPacketParser*	poPacketParser;
	ISDSessionFactory*	poSessionFactory;
	uint32				dwRecvBufSize;
	uint32				dwSendBufSize;
};

struct SStopListenReq
{
	uint32				dwListenerID;
};

struct SConnReq
{
	uint32				dwConnectorID;
	uint32				dwIP;
	uint16				wPort;
	ISDPacketParser*	poPacketParser;
	ISDSession*			poSession;
	uint32				dwRecvBufSize;
	uint32				dwSendBufSize;
	bool					bNeedBind;
	const char*		pszBindIP;
	uint16				wBindPort;
};

struct SDisconnReq
{
	uint32				dwEpollSockID;
};

struct SSendReq
{
	uint32				dwEpollSockID;
	int32				nLen;
	char*				pData;			//只有在临时缓冲队列中才使用这个指针，如果不为空，则可以直接使用此指针指向的数据，使用完后释放
};

struct SSockOptReq
{
	uint32				dwEpollSockID;
	int32				nLevel;
	int32				nOptName;
	char				OptVal[8];
	int32				nOptLen;
};

struct SNetRequest
{
	ENetReqType		nType;
	union
	{
		SStartListenReq		stStartListen;
		SStopListenReq		stStopListen;
		SConnReq			stConn;
		SDisconnReq			stDisConn;
		SSendReq			stSend;
		SSockOptReq			stSockOpt;
	}stUn;
};

#endif
