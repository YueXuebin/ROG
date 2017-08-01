#pragma once

/*
 *	协议解析器
 */
#ifndef PacketParser_h__
#define PacketParser_h__

class PacketParser: public Crown::ISDPacketParser
{
	static PacketParser*	m_Instance;
public:
	PacketParser();
	virtual ~PacketParser();

	static PacketParser*	Instance() {return m_Instance;}

	virtual int32 SDAPI ParsePacket(const char* pBuf, uint32 dwLen);		//Interface ISDPacketParser

	uint32				GetMsgIDFromNetHeader(const char* pBuf);
	int16				GetMsgIDFromPipeHeader(const char* pBuf);
	uint32				GetTransIDFromPipeHeader(const char* pBuf);

	void				BuildNetHeader(const char* pBuf, uint16 wMsgID, uint32 dwLen);
	void				BuildPipeHeader(const char* pBuf, uint16 wMsgID, uint32 dwTransID);

	uint32				GetGateSvrIDfromTransID(uint32 dwTransID, uint32 dwSvrID);

};



#endif // YnCliPacketParser_h__
