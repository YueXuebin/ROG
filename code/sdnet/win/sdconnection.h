#pragma once

#ifndef SDCONNECTION_H
#define SDCONNECTION_H

#include "sdnetconfig.h"
#include "sdnet.h"


using namespace Crown;

struct CConnData;

/**
* @brief 代表一个网络连接的类
*/
class CSDConnection : public ISDConnection
{
	enum EConnStat
	{
		CONN_NONE = 0,
		CONN_OK,
		CONN_ASSOCIATE,
		CONN_CLOSING,
	};
public:
	CSDConnection();
	virtual ~CSDConnection();

	/**
	* @brief 此连接是否为连接的
	* @return 返回true代表连接的,返回false代表断开的
	*/
	virtual bool SDAPI IsConnected(void);

	/**
	* @brief 使用此连接发送数据
	* @param pBuf : 指向发送的数据的指针
	* @param dwLen : 发送数据的长度
	*/
	virtual void SDAPI Send(const char* pBuf, uint32 dwLen);

	/**
	* @brief 设置此连接的参数
	* @param dwType : 参数类型
	* @param pOpt : 参数数据
	* @return void
	*/
	virtual void SDAPI SetOpt(uint32 dwType, void* pOpt);

	/**
	* @brief 断开连接
	* @return void
	*/
	virtual void SDAPI Disconnect(void);

	/**
	* @brief 获取此连接的远程IP地址的数字型
	* @return 此连接的远程IP地址的数字型
	*/
	virtual const uint32 SDAPI GetRemoteIP(void)	{ return m_dwRemoteIP;	}

	/**
	* @brief 获取此连接的远程IP地址的字符串类型
	*/
	virtual const char* SDAPI GetRemoteIPStr(void)	{ return inet_ntoa( (in_addr&)m_dwRemoteIP );	}

	/**
	* @brief 获取此连接的远程端口号
	* @return 此连接的远程端口号
	*/
	virtual uint16  SDAPI GetRemotePort(void)		{ return m_wRemotePort; }

	/**
	* @brief 获取此连接的本地IP地址的数字型
	* @return 此连接的本地IP地址的数字型
	*/
	virtual const uint32 SDAPI GetLocalIP(void)		{ return m_dwLocalIP;	}

	/**
	* @brief 获取此连接的本地IP地址的字符串型
	* @return 此连接的本地IP地址的字符串型
	*/
	virtual const char* SDAPI GetLocalIPStr(void)	{ return inet_ntoa( (in_addr&)m_dwLocalIP );	}

	/**
	* @brief 获取此连接的本地端口号
	* @return 此连接的本地端口号
	*/
	virtual uint16 SDAPI GetLocalPort(void)			{ return m_wLocalPort;	}

	/**
	* @brief 返回未使用的发送缓存
	*/
	virtual uint32 SDAPI GetSendBufFree(void);

	/**
	* @brief 重置此连接,清理此连接的信息
	*/
	void Reset();

	/**
	* @brief 设置此连接的ID号
	*/
	inline void SetID(uint32 dwID) { m_dwID = dwID; }

	/**
	* @brief 获取此连接的ID号
	*/
	uint32 GetID() { return m_dwID; }

	/**
	* @brief 设置此连接是否为主动连接,抑或被动连接
	* @param bAccept : false为主动连接,true为被动连接
	* @return void
	*/
	inline void SetAccept(bool bAccept)	{ m_bAccept = bAccept;	}

	/**
	* @brief 查看此连接是否为被动连接
	* @return true为被动连接,false为主动连接
	*/
	bool IsAccept()					{ return m_bAccept;		}

	inline void SetParentID(uint32 dwParentID)	{ m_dwParentID = dwParentID;	}
	uint32 GetParentID()				{ return m_dwParentID;			}

	/**
	* @brief 设置此连接的远程IP地址
	* @param dwIP : 数字型的远程IP地址
	* @return void
	*/
	void SetRemoteIP(uint32 dwIP);

	/**
	* @brief 设置此连接的远程端口号
	* @param dwIP : 远程端口号
	* @return void
	*/
	inline void SetRemotePort(uint16 wPort)	{ m_wRemotePort = wPort;	}

	/**
	* @brief 设置此连接的本地IP地址
	* @param dwIP : 本地IP地址
	* @return void
	*/
	void SetLocalIP(uint32 dwIP);

	/**
	* @brief 设置此连接的本地端口号
	* @param wPort : 本地端口号
	* @return void
	*/
	inline void SetLocalPort(uint16 wPort)		{ m_wLocalPort = wPort;		}

	/**
	* @brief 设置此连接的连接数据
	* @param pConnData : 连接数据
	* @return void
	*/
	inline void SetConnData(CConnData* pConnData)		{ m_pConnData = pConnData; }

	/**
	* @brief 设置此连接的会话
	* @param poSession : 连接会话
	*/
	inline void SetSession(ISDSession* poSession) { m_poSession = poSession; }

	/**
	* @brief 当此连接连接成功时,调用此函数
	*/
	void OnConnect();

	/**
	* @brief 当此连接发送数据时,调用此函数
	* @param pBuf : 指向发送的数据的指针
	* @param dwLen : 发送数据的长度
	*/
	void OnSend(const char * pBuf, uint32 dwLen);

	/**
	* @brief 当此连接连接成功时,调用此类
	*/
	void OnAssociate();

	/**
	* @brief 当此连接关闭时,调用此函数
	*/
	void OnClose();

	/**
	* @brief 当此连接接收到消息时,调用此函数
	*/
	inline void OnRecv(const char* pData, int32 nLen)
	{
		if(m_nConnStat != CONN_ASSOCIATE)
		{
			return;
		}

		CnVerify(m_poSession != NULL);
		m_poSession->OnRecv(pData, nLen);
	}

	/**
	* @brief 当此连接发生错误时,调用此函数
	*/
	inline void OnError(int32 nSDError, int32 nSysError)
	{
		if(m_nConnStat != CONN_ASSOCIATE)
		{
			return;
		}

		CnVerify(m_poSession != NULL);
		m_poSession->OnError(nSDError, nSysError);
	}

protected:
	uint32		m_dwID;
	uint32		m_dwParentID;
	bool		m_bAccept;
	EConnStat	m_nConnStat;
	uint32		m_dwLocalIP;
	uint32		m_dwRemoteIP;
	uint16		m_wLocalPort;
	uint16		m_wRemotePort;
	//char		m_szLocalIP[16];
	//char		m_szRemoteIP[16]; //decrease the memory footprint
	CConnData *	m_pConnData;
	ISDSession* m_poSession;
};

#endif
