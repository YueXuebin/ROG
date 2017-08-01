#include "transfer_pch.h"
#include "packet_parser.h"
#include "protocol_define.h"

PacketParser	g_PacketParser;

PacketParser*	PacketParser::m_Instance = NULL;

PacketParser::PacketParser()
{
	CnVerify(!m_Instance);
	m_Instance = this;
}

PacketParser::~PacketParser()
{
	m_Instance = NULL;
}

int32 PacketParser::ParsePacket(const char *pBuf, uint32 dwLen)
{
	// 根据包头拆包
	if (dwLen < sizeof(STNetMsgHeader))
		return 0;
	
	STNetMsgHeader* pNetMsgHeader = (STNetMsgHeader*)pBuf;

	uint32 nPkgLen = ntohl( pNetMsgHeader->dwDataLen) + sizeof(STNetMsgHeader);
	if (nPkgLen < 0)
	{
		return -1;
	}

	if (nPkgLen <= dwLen)
	{
		return nPkgLen;
	}

	return 0;
}

uint32 PacketParser::GetMsgIDFromNetHeader(const char *pBuf)
{
	STNetMsgHeader* pNetMsgHeader = (STNetMsgHeader*)pBuf;
	return ntohl(pNetMsgHeader->dwMsgID);
}

int16 PacketParser::GetMsgIDFromPipeHeader(const char *pBuf)
{
	STPipeMsgHeader* pPipeMsgHeader = (STPipeMsgHeader*)pBuf;
	return ntohs(pPipeMsgHeader->wMsgID);
}

uint32 PacketParser::GetTransIDFromPipeHeader(const char* pBuf)
{
	STPipeMsgHeader* pPipeMsgHeader = (STPipeMsgHeader*)pBuf;
	return ntohl(pPipeMsgHeader->dwTransID);
}

void PacketParser::BuildNetHeader(const char *pBuf, uint16 wMsgID, uint32 dwLen)
{
	STNetMsgHeader* pNetMsgHeader = (STNetMsgHeader*)pBuf;
	pNetMsgHeader->dwDataLen = htonl(dwLen);
	pNetMsgHeader->dwMsgID = htons(wMsgID);
}

void PacketParser::BuildPipeHeader(const char *pBuf, uint16 wMsgID, uint32 dwTransID)
{
	STPipeMsgHeader* pPipeMsgHeader = (STPipeMsgHeader*)pBuf;
	pPipeMsgHeader->dwTransID = htonl(dwTransID);
	pPipeMsgHeader->wMsgID = htons(wMsgID);
}

uint32 PacketParser::GetGateSvrIDfromTransID(uint32 dwTransID, uint32 dwSvrID)
{
	CnVerify(false);
	return 0;
// 	uint32 dwIndex = dwTransID >> 24;
// 	dwIndex = dwIndex << 24;
// 	uint32 dwType = TYPE_GATE_SERVER;
// 	dwType = dwType << 16;
// 
// 	uint32 dwGroupRegion = dwSvrID << 16;
// 	dwGroupRegion = dwGroupRegion >> 16;
// 
// 	uint32 dwGateSvrID = dwIndex + dwType + dwGroupRegion;
// 
// 	return dwGateSvrID;
}
