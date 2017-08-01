#pragma once

#ifndef NETEVENT_H
#define NETEVENT_H
//#include "sdloopbuffer.h"
#include "simpleloopbuffer.h"

enum ENetEvtType
{
	NETEVT_INVALID = 0,
	NETEVT_ESTABLISH,
	NETEVT_ASSOCIATE,
	NETEVT_CONN_ERR,
	NETEVT_ERROR,
	NETEVT_TERMINATE,
	NETEVT_RECV,
	NETEVT_SEND,
	NETEVT_BIND_ERR,
};

struct CConnData;

struct SEstablishEvt
{
	CConnData*		pConnData;
	bool			bAccepted;
	uint32			dwParentID;		//Connector的ID
};

struct SAssociateEvt
{
	CConnData*		pConnData;
	uint32			dwConnectionID;
};

struct SConnErrEvt
{
	int32			nSysErrCode;
	uint32			dwConnectorID;
};

struct SErrorEvt
{
	CConnData*		pConnData;
	uint32			dwConnectionID;
	int32			nSDErrCode;
	int32			nSysErrCode;
};

struct STerminateEvt
{
	CConnData*		pConnData;
	uint32			dwConnectionID;
	bool			bAccepted;
	uint32			dwParentID;		//Connector的ID
};

struct SRecvEvt
{
	CConnData*			pConnData;
	uint32				dwConnectionID;
	int32				nLen;
	CSimpleLoopBuffer*	pLoopBuf; 
};

struct SSendEvt
{
	CConnData*		pConnData;
	const char*		pBuf;
	uint32			dwLen;
};

//// 2009-03-24 cwy add for interface expanding, add bind function
struct SBindErrEvt
{
	uint32			dwConnectorID;
	int32			nSDErrCode;
	int32			nSysErrCode;
};

/**
* @brief 网络事件，nType代表网络事件的类型
*
*/
struct SNetEvent
{
	ENetEvtType		nType;
	union
	{
		SEstablishEvt		stEstablish;
		SAssociateEvt		stAssociate;
		SConnErrEvt			stConnErr;
		SErrorEvt			stError;
		STerminateEvt		stTerminate;
		SRecvEvt			stRecv;
		SSendEvt            stSend; 
		SBindErrEvt			stBindErr;
	}stUn;
};

#endif

