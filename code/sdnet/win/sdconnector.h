#pragma once

#ifndef SDCONNECTOR_H
#define SDCONNECTOR_H

#include "sdnet.h"
#include <map>

using namespace Crown;

/**
* @brief 网络连接器,用于本地连接远程服务器
*/
class CSDConnector : public ISDConnector
{
	enum EConnStat
	{
		CONN_NONE = 0,
		CONN_PROC,
		CONN_OK
	};

public:
	CSDConnector();
	virtual ~CSDConnector();

	/**
	* @brief 设置数据包解析器
	* @param poPakcetParser : 设置的数据包解析器
	* @return void
	*/
	virtual void SDAPI SetPacketParser(ISDPacketParser* poPakcetParser);

	/**
	* @brief 设置连接会话
	* @param poSession : 设置的连接会话
	* @return void
	*/
	virtual void SDAPI SetSession(ISDSession* poSession);

	/**
	* @brief 设置缓存大小
	* @param dwRecvBufSize : 设置接收缓存大小
	* @param dwSendBufSize : 设置发送缓存大小
	* @return void
	*/
	virtual void SDAPI SetBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize);

	/**
	* @brief 设置此连接器参数
	* @param dwType : 设置此连接器的类型
	* @param pOpt : 设置此连接器的值
	* @return void
	*/
	virtual void SDAPI SetOpt(uint32 dwType, void* pOpt);

	/**
	* @brief 使用此连接器连接远程服务器
	* @param pszIP : 设置远程服务器IP地址
	* @param wPort : 设置远程服务器的端口号
	* @return 是否连接成功,true为成功,false为失败
	*/
	virtual bool SDAPI Connect(const char* pszIP, uint16 wPort);

	/**
	* @brief 使用此连接器重新连接远程服务器
	* @return 是否重新连接成功,true为成功,false为失败
	* @remark 重新连接的远程服务器的IP和端口为m_dwIP和m_wPort
	*/
	virtual bool SDAPI ReConnect(void);

	/**
	* @brief 释放此连接器
	* @return void
	*/
	virtual void SDAPI Release(void);

	/**
	* @brief 设置此连接器的ID号
	* @param dwID : 设置连接器的ID号
	*/
	void SetID(uint32 dwID) { m_dwID = dwID; }

	/**
	* @brief 获取此连接器的ID号
	* @return 获取的ID号
	*/
	uint32 GetID() { return m_dwID; }

	//// 2009-03-24 cwy add for interface expanding, add bind function
	/**
	* @brief 当绑定本地IP地址失败时,调用此函数
	* @param nSDError : SD Error
	* @param nSysError : Sys Error
	* @return void
	*/
	void OnBindErr(int32 nSDError, int32 nSysError);

	/**
	* @brief 当使用此连接器连接成功时,调用此函数
	* @return void
	*/
	void OnConnect();

	/**
	* @brief 当连接错误时,调用此函数
	* @param nSysError : Sys Error
	* @return void
	*/
	void OnConnectErr(int32 nSysError);

	/**
	* @brief 当连接关闭时,调用此函数
	* @return void
	*/
	void OnClose();

protected:
	uint32				m_dwID;
	ISDPacketParser*	m_poPacketParser;
	ISDSession*			m_poSession;
	uint32				m_dwRecvBufSize;
	uint32				m_dwSendBufSize;
	uint32				m_dwIP;
	uint16				m_wPort;
	EConnStat			m_nConnStat;
	bool					m_bNeedBind;
	const char*		m_pszBindIP;
	uint16				m_wBindPort;
};

typedef std::map<uint32, CSDConnector*> CMapConnector;

#endif
