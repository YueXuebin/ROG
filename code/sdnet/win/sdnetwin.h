#pragma once

#ifndef SDNETWIN_H
#define SDNETWIN_H

#include "sdnet.h"
#include "sdsingleton.h"
#include "sdconnector.h"
#include "sdloopbuffer.h"
#include "netevent.h"
#include "sdtimer.h"
#include "cpsock.h"
#include <vector>

using namespace Crown;

struct SendOutTimer;

class CSDNetWin : public ISDNet
{
	CSDNetWin();
	virtual ~CSDNetWin();

	DECLARE_SINGLETON(CSDNetWin)

public:
	//Interface ISDBase
	
	/**
	* @brief 增加对SDNet的引用
	* @return void
	*/
	virtual void SDAPI AddRef(void);

	/**
	* @brief 查询目前引用的数量
	* @return 返回目前引用的数量
	*/
	virtual uint32  SDAPI QueryRef(void);

	/**
	* @brief 减少对SDNet的引用,若引用为0,释放此类
	* @return void
	*/
	virtual void SDAPI Release(void);

	/**
	* @brief 获取此类的版本
	* @return 返回此类的版本号
	*/
	virtual SSGDPVersion SDAPI GetVersion(void);

	/**
	* @brief 返回此类的名称
	* @return 返回此类的名称
	*/
	virtual const char * SDAPI GetModuleName(void);

	//Interface ISDNet

	/**
	* @brief 创建网络连接器
	* @param dwNetIOType : 网络连接的类型,类型只有NETIO_COMPLETIONPORT
	* @return 创建的网络连接器
	*/
	virtual ISDConnector* SDAPI CreateConnector(uint32 dwNetIOType);

	/**
	* @brief 创建网络监听器
	* @param dwNetIOType : 网络监听的类型,
	* 类型有NETIO_COMPLETIONPORT和NETIO_COMPLETIONPORT_GATE
	* @return 创建的网络监听器
	*/
	virtual ISDListener*  SDAPI CreateListener(uint32 dwNetIOType);

	/**
	* @brief 在主线程中运行SDNet的逻辑操作
	* @param nCount : 此次运行的逻辑操作的个数
	* @return bool : 是否运行了nCount个逻辑操作
	*/
	virtual bool SDAPI Run(int32 nCount);

	/**
	* @brief 初始化SDNet
	* @return true为初始化成功,false为初始化失败
	*/
	bool Init();

	/**
	* @brief 清理SDNet
	* @return true为清理成功,false为清理失败
	*/
	void Uninit();

	/**
	* @brief 释放网络连接器
	* @return true为清理成功,false为清理失败
	*/
	void ReleaseConnector(CSDConnector* poConnector);

	/**
	* @brief 根据dwID查找网络连接器
	* @return 返回的网络连接器,查找失败,返回NULL
	*/
	CSDConnector* FindConnector(uint32 dwID);

	typedef std::vector<CCPSock * > ConnectedSockets; 

#ifdef SDNET_HAS_GATHER_SEND 
	ConnectedSockets m_connSockets; 
    void FlushBufferedData()
	{
		for (ConnectedSockets::iterator itr = m_connSockets.begin();
				itr != m_connSockets.end(); ++itr)
		{
			(*itr)->OnSendOut(); 
		}
	}
#endif // 

	/**
	* @brief 处理接收的网络数据
	* @param pConnData : "连接数据"
	* @param pData : 接收的网络数据
	* @param len : 网络数据的长度
	*/
	void ProcRecvData(CConnData * pConnData, const char * pData, int len); 

protected:
	/**
	* @brief 创建SDNet组件
	* @return 是否创建组件成功,true为成功,false为失败
	*/
	bool _CreateComponent();

	/**
	* @brief 初始化SDNet组件
	* @return 是否初始化组件成功,true为成功,false为失败
	*/
	bool _InitComponent();

	/**
	* @brief 清理SDNet组件
	* @return void
	*/
	void _UninitComponent();

	/**
	* @brief 摧毁SDNet组件
	* @return void
	*/
	void _DesroryComponent();

	/**
	* @brief 清理网络连接器
	* @return void
	*/
	void _ClearConnector();

	void _ProcEstablishEvt(SEstablishEvt* pstEvent);
	void _ProcAssociateEvt(SAssociateEvt* pstEvent);
	void _ProcConnErrEvt(SConnErrEvt* pstEvent);
	void _ProcErrorEvt(SErrorEvt* pstEvent);
	void _ProcTerminateEvt(STerminateEvt* pstEvent);
	void _ProcRecvEvt(SRecvEvt* pstEvent);
	void _ProcSendEvt(SSendEvt* pstEvent);
	//// 2009-03-24 cwy add for interface expanding, add bind function
	void _ProcBindErrEvt(SBindErrEvt* pstEvent);

protected:
	uint32			m_dwRef;


	uint32			m_dwNextConnectorID;
	CMapConnector	m_oMapConnector;
	CSDLoopBuffer	m_oRecvQueue;
	//char*			m_pRecvBuf;
    uint32          m_dwLastTick;    
};

#endif
