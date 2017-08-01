#pragma once

#ifndef NETREQUEST_H
#define NETREQUEST_H

#include "sdnet.h"
#include <list>

using namespace Crown;

enum ENetReqType
{
	NETREQ_INVALID = 0,
	NETREQ_CONNECT,
	NETREQ_DISCONN,
};

/**
* @brief 连接创建请求结构体
*/
struct SConnReq
{
	uint32				dwConnectorID;
	uint32				dwIP;
	uint16				wPort;
	uint16				wBindPort;
	const char*			pszBindIP;

	ISDPacketParser*	poPacketParser;
	ISDSession*			poSession;
	uint32				dwRecvBufSize;
	uint32				dwSendBufSize;
	bool				bNeedBind;
};

/**
* @brief 断开连接请求结构体
*/
struct SDisconnReq
{
	uint32				dwConnectorID;
};

/**
* @brief 网络请求结构体
*/
struct SNetRequest
{
	ENetReqType		nType;
	union
	{
		SConnReq			stConn;
		SDisconnReq			stDisConn;
	}stUn;
};

typedef std::list<SNetRequest*> CNetReqList;

#endif
