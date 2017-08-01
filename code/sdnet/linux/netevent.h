#ifndef NETEVENT_H
#define NETEVENT_H
#include "sdtype.h"

enum ENetEvtType
{
	NETEVT_INVALID = 0,
	NETEVT_ESTABLISH,
	NETEVT_CONN_ERR,
	NETEVT_ERROR,
	NETEVT_TERMINATE,
	NETEVT_RECV,
	NETEVT_SEND,
	NETEVT_BINDERR,
};

struct SEstablishEvt
{
	uint32	dwSockID;		//EpollSock的ID
	uint32	dwConnID;		//Connection的ID
	bool	bAccepted;
	uint32	dwParentID;		//Connector的ID
};

struct SConnErrEvt
{
	int32	nSysErrCode;
	uint32	dwConnectorID;
};

struct SErrorEvt
{
	uint32	dwConnectionID;
	int32	nSDErrCode;
	int32	nSysErrCode;
};

struct STerminateEvt
{
	uint32	dwConnectionID;
	bool	bAccepted;
	uint32	dwParentID;		//Connector的ID
};

struct SRecvEvt
{
	uint32	dwConnectionID;
	int32	nLen;
	char*	pData;			//只有在临时缓冲队列中才使用这个指针，如果不为空，则可以直接使用此指针指向的数据，使用完后释放
};

struct SSendEvt
{
	uint32	dwConnectionID;
	const char *          pBuf; 
	uint32                dwLen; 
};
struct SBindErrEvt
{
	int32	nSysErrCode;
	uint32	dwConnectorID;
};

struct SNetEvent
{
	ENetEvtType		nType;
	union
	{
		SEstablishEvt		stEstablish;
		SConnErrEvt			stConnErr;
		SErrorEvt			stError;
		STerminateEvt		stTerminate;
		SRecvEvt			stRecv;
		SSendEvt            stSend; 
		SBindErrEvt			stBindErr;
	}stUn;
};

#endif
