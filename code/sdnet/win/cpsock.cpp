#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "cpsock.h"
#include "iocpctrl.h"
#include "sdnet.h"
#include "eventmgr.h"
#include "sdconnection.h"
#include "conndatamgr.h"
#include "sdnetwin.h"

//CCpLock	CCPSock::m_mutex; //Lyan

CCPSock::CCPSock()
{
	m_stPerHandleData.bListen	= false;
	m_stPerHandleData.ptr		= this;	
	m_stRecvIoData.nOp			= IOCP_RECV;
	m_stSendIoData.nOp			= IOCP_SEND;
	m_stCloseIoData.nOp         = IOCP_CLOSE; 

	Reset();
}
//int CCPSock::user_close_count  =0 ;
//int CCPSock::on_close_count =0 ; 
CCPSock::~CCPSock()
{

}

void CCPSock::Reset()
{
	m_hSock					= INVALID_SOCKET;
	m_bConnect				= false;
//	m_dwDelayReleaseStamp	= 0;
	m_pRecvBuf				= NULL;
	m_pSendBuf				= NULL;
	m_dwRecvBufSize			= 0;
	m_dwSendBufSize			= 0;
	m_nDataReceived			= 0;
	m_nDataSend				= 0;
	m_poPacketParser		= NULL;
	m_pConnData             = NULL;

#ifdef SDNET_HAS_GATHER_SEND 
	m_hasCollectSent  = true; 
#endif
    m_dwPostRecv            = 0;
    m_dwPostSend            = 0;
    //m_bIsNeedForceClean     = false; //lcj
 
}

void CCPSock::AttachRecvBuf(char* pRecvBuf, uint32 dwRecvBufSize)
{
	m_pRecvBuf		= pRecvBuf;
	m_dwRecvBufSize	= dwRecvBufSize;
	m_nDataReceived	= 0;
}

uint32 CCPSock::DetachRecvBuf(char** ppRecvBuf)
{
	*ppRecvBuf = m_pRecvBuf;
	uint32 dwRet = m_dwRecvBufSize;

	m_dwRecvBufSize = 0;
	m_nDataReceived	= 0;
	m_pRecvBuf		= NULL;

	return dwRet;
}

void CCPSock::AttachSendBuf(char* pSendBuf, uint32 dwSendBufSize)
{
	m_pSendBuf		= pSendBuf;
	m_dwSendBufSize = dwSendBufSize;
	m_nDataSend		= 0;
}

uint32 CCPSock::DetachSendBuf(char** ppSendBuf)
{
	*ppSendBuf		= m_pSendBuf;
	uint32 dwRet	= m_dwSendBufSize;
	m_dwSendBufSize = 0;
	m_nDataSend		= 0;
	m_pSendBuf		= NULL;

	return dwRet;
}

void CCPSock::SetConnData(CConnData* pConnData)
{
	m_pConnData = pConnData;
	//m_dwConnectionID	= poConnection->GetID();
	DBG("[%s:%d]connID=%u, recvBuf=%p, sendBuf=%p", MSG_MARK, GetConnectionID(), m_pRecvBuf, m_pSendBuf);
}

uint32 CCPSock::GetConnectionID() 
{ 
	if ( m_pConnData!= NULL )
	{
		return m_pConnData->connId;
	}
	return -1;
}

 
void CCPSock::Send(const char* pData, uint32 dwLen)
{
	bool bSend = false;
	if( !IsConnect() )
	{
		return ;
	}
    /// 单次长度超过限制，buffer和当前数据都立即发送 \///
	if (dwLen > this->m_dwSendBufSize)
	{
		SyncSend(pData, dwLen);
		return ;
	}
	m_sendMutex.Lock();
	byte byTimes = 0 ;
	while(dwLen + m_nDataSend > this->m_dwSendBufSize)
	{
#ifdef SDNET_HAS_GATHER_SEND
		m_sendCond.Wait(&m_sendMutex);
#else
		m_sendMutex.Unlock();
		Sleep(1);
		byTimes ++ ;

		m_sendMutex.Lock();
#endif
		if ( byTimes > 5 )
		{
			DoClose() ;
			m_sendMutex.Unlock();
			return ;
		}
		if (!IsConnect())
		{
			m_sendMutex.Unlock();
			return ;
		}
	}

	if (m_nDataSend == 0)
	{
		bSend = true;
	}
	memcpy(m_pSendBuf + m_nDataSend, pData, dwLen);
	m_nDataSend += dwLen;
	m_sendMutex.Unlock();

#ifdef SDNET_HAS_GATHER_SEND
	m_hasCollectSent = bSend;
#else
	if (bSend)
	{
		this->AsyncSend(m_pSendBuf, m_nDataSend);
	}
#endif


//////////////////////////////////////////////////////////////////////
//
//	m_sendMutex.Lock();
//	if (dwLen + m_nDataSend > this->m_dwSendBufSize)
//    {
//		do
//		{
//
//#ifdef SDNET_HAS_GATHER_SEND
//			m_sendCond.Wait(&m_sendMutex);
//#else
//			m_sendMutex.Unlock();
//			Sleep(1);
//			m_sendMutex.Lock();
//#endif
//
//			if (!IsConnect())
//			{
//				m_sendMutex.Unlock();
//				return ;
//			}
//		}while (m_nDataSend + dwLen > this->m_dwSendBufSize);
//
//		if (m_nDataSend == 0)
//		{
//			bSend = true;
//		}
//		memcpy(m_pSendBuf + m_nDataSend, pData, dwLen);
//		m_nDataSend += dwLen;
//		m_sendMutex.Unlock();
//
//#ifdef SDNET_HAS_GATHER_SEND
//		m_hasCollectSent = bSend;
//#else
//		if (bSend)
//		{
//			this->AsyncSend(m_pSendBuf, m_nDataSend);
//		}
//#endif
//
//		return ;
//    }
//    else
//    {
//		if (m_nDataSend == 0)
//		{
//			bSend = true;
//		}
//		// 缓存数据到缓冲区，异步发送会一直发送直到发完缓冲区数据
//		memcpy(m_pSendBuf + m_nDataSend, pData, dwLen);
//		m_nDataSend += dwLen;
//    }
//
//#ifndef SDNET_HAS_GATHER_SEND 
//	if (bSend)
//	{
//		this->AsyncSend(m_pSendBuf, m_nDataSend);
//	}
//#endif
//
//	m_sendMutex.Unlock();
}

#ifdef SDNET_HAS_GATHER_SEND 
void CCPSock::OnSendOut()
{
    /// 单线程操作，不加锁 \///
    //m_oLock.Lock();

	MutexLock lock(m_sendMutex);
	if (m_nDataSend  > 0 && m_hasCollectSent)
	{
		m_hasCollectSent = false;
		this->AsyncSend(m_pSendBuf, m_nDataSend);
	}
    //m_oLock.Unlock();
    /// 单线程操作，不加锁 \///
}
#endif // 

//在缓冲区不够的情况下，使用同步发送。 
int32  CCPSock::SyncSend(const char * pData, uint32 dwLen)
{
 	WSABUF sndBuf;
	sndBuf.buf = (char*)pData;
	sndBuf.len = dwLen;
	DWORD dwNumberOfBytesSent = 0;

	INFO("[%s:%d] SyncSend Buffer overflow  len %d, Socket %d \n", 
		MSG_MARK, dwLen, m_hSock);
 
	if ( 0 != WSASend(m_hSock, &sndBuf, 1, &dwNumberOfBytesSent, 0, NULL, NULL) )
	{
		int32 errNo = WSAGetLastError();
		if(errNo != WSA_IO_PENDING)
		{
			_OnError(NET_SEND_ERROR, WSAGetLastError());
			this->Close(); 
			return -1 ;
		}
	}
	CnVerify (dwLen == dwNumberOfBytesSent); 

#ifdef SDNET_HAS_SEND_REPORT
	CEventMgr::Instance()->PushSendEvt(m_pConnData, pData + dwNumberOfBytesSent, dwLen - dwNumberOfBytesSent);
#endif

	return dwNumberOfBytesSent;
}

int32 CCPSock::AsyncSend(const char* pData, uint32 dwLen)
{
	m_stSendIoData.stWsaBuf.buf = (char*)pData;
	m_stSendIoData.stWsaBuf.len = dwLen;
	
	if(false == _PostSend())
	{
		_OnError(NET_SEND_ERROR, WSAGetLastError());
		Close();
	}
	return 0;
}

void CCPSock::DoClose()
{
	shutdown(m_hSock, SD_BOTH);
	this->SetConnect(false);
	closesocket(m_hSock);
	if (m_hSock != INVALID_SOCKET)
	{
		m_hSock = INVALID_SOCKET;
		CEventMgr::Instance()->PushTerminateEvt(m_pConnData, GetConnectionID());
	}
	//InterlockedIncrement((int32*)&on_close_count);
	//printf("User Close %d , On close %d\n", this->user_close_count,this->on_close_count); 
}

void CCPSock::Close()
{
	// close from user , all pending op should be canceled
	if (this->IsConnect() && m_hSock != INVALID_SOCKET)
	{
		this->SetConnect(false);
		//InterlockedIncrement((int32*)&user_close_count);
		if (this->GetPostSend() > 0)
		{
			bool ret = this->_PostClose(m_hSock);
			if (!ret)
			{
				this->DoClose();
			}
		}
		else // GetPostSend()  == 0 
		{
			if ( this->GetPostRecv() == 0)
			{
				this->DoClose();
			}
			else 
			{
				shutdown(m_hSock, SD_BOTH);
			}
		}
		return ;
	}
}

void CCPSock::OnClose(bool bPassive)
{
	if (m_hSock == INVALID_SOCKET)
	{
		return ;
	}
	if (bPassive)
	{
		//close from client, all pending io op should closed 
		this->SetConnect(false);
		shutdown(m_hSock, SD_BOTH);
		if (this->GetPostSend() > 0 || this->GetPostRecv() > 0)
		{
			CancelIo((HANDLE) m_hSock);
			INFO("[%s:%d]OnClose  Passive  Send Count %d, Recv Count %d, Socket %d\n",
				MSG_MARK, this->GetPostSend(), this->GetPostRecv(), m_hSock);
		}

		closesocket(m_hSock);
		if (m_hSock != INVALID_SOCKET)
		{
			m_hSock = INVALID_SOCKET;
			INFO("[%s:%d]OnClose  Push terminated Evt, Socket :%d\n", MSG_MARK, m_hSock);
			CEventMgr::Instance()->PushTerminateEvt(m_pConnData, GetConnectionID());
		}
	}
	else 
	{
		if (this->GetPostSend() > 0)
		{
			bool ret = this->_PostClose(m_hSock);
			if (!ret)
			{
				this->DoClose();
			}
		}
		else 
		{
			if ( this->GetPostRecv() == 0)
			{
				this->DoClose();
			}
			else 
			{
				shutdown(m_hSock, SD_BOTH);
			}
		}
	}
}

bool CCPSock::AssociateWithIocp()
{
	/*change the order of the event     */ 
	CEventMgr::Instance()->PushAssociateEvt(m_pConnData, GetConnectionID());
	return  CIocpCtrl::Instance()->AssociateWithIocp(m_hSock, &m_stPerHandleData);
}

void CCPSock::OnRecv(DWORD dwBytes)
{
	m_nDataReceived += dwBytes;
	char *p = m_pRecvBuf;

	while(m_nDataReceived > 0)
	{
		int nUsed = m_nDataReceived;
		if (m_poPacketParser != NULL)
		{
			nUsed = m_poPacketParser->ParsePacket(p, m_nDataReceived);
		}

		if(0 == nUsed)
		{
			//接收缓冲区已满，但还没有装下一个完整包，只好断开连接
			if(m_nDataReceived >= (int32)m_dwRecvBufSize || m_nDataReceived >= MAX_PKG_LEN || m_nDataReceived >= RECV_LOOP_BUF_SIZE)
			{
				_OnError(NET_PACKET_ERROR, 0);
				Close();
				return;
			}
			break;
		}
		else if(nUsed > 0)
		{
			if(nUsed > m_nDataReceived)
			{
				_OnError(NET_PACKET_ERROR, 1); //lcj
				Close();
				return;
			}

#ifdef SDENT_HAS_RECV_QUEUE
			CEventMgr::Instance()->PushRecvEvt(m_pConnData, GetConnectionID(), p, nUsed);
#else
			CSDNetWin::Instance()->ProcRecvData(m_pConnData, p, nUsed);
#endif //
			p += nUsed;
			m_nDataReceived -= nUsed;
		}
		else	// < 0
		{
			_OnError(NET_PACKET_ERROR, 2); //lcj
			Close();
			return;
		}
	}

	if (m_nDataReceived > 0 && (p != m_pRecvBuf))
	{
        memmove(m_pRecvBuf, p, m_nDataReceived);
	}

	if(!IsConnect())
	{
		return;
	}

	if(false == PostRecv())
	{
		_OnError(NET_RECV_ERROR, WSAGetLastError());
		Close();
	}
}

void CCPSock::OnSend(DWORD dwBytes)
{
	if ( !IsConnect() )
	{
		return ;
	}

	bool bSend = false;
 
	m_sendMutex.Lock();
	if(dwBytes >= (uint32)m_nDataSend)
	{
		m_nDataSend = 0;
	}
	else
	{	 
		m_nDataSend -= dwBytes;
		memmove(m_pSendBuf, m_pSendBuf+dwBytes, m_nDataSend);
		m_stSendIoData.stWsaBuf.buf = m_pSendBuf;
		m_stSendIoData.stWsaBuf.len = m_nDataSend;
		bSend = true;
	}
#ifdef SDNET_HAS_GATHER_SEND 
	m_sendCond.Signal(); 
	m_hasCollectSent = true; 
#endif
	m_sendMutex.Unlock();

#ifdef SDNET_HAS_SEND_REPORT
	CEventMgr::Instance()->PushSendEvt(m_pConnData, m_pSendBuf, m_nDataSend);
#endif

    if(bSend)
	{
		if(false == _PostSend())
		{
			_OnError(NET_SEND_ERROR, WSAGetLastError());
			Close();
		}
	}
}

bool CCPSock::PostRecv()
{
	if (!IsConnect())
	{
		return true;
	}
	CnVerify((uint32)m_nDataReceived < m_dwRecvBufSize);

	ZeroMemory(&m_stRecvIoData.stOverlapped, sizeof(m_stRecvIoData.stOverlapped));
	m_stRecvIoData.stWsaBuf.buf = m_pRecvBuf + m_nDataReceived;
	m_stRecvIoData.stWsaBuf.len	= m_dwRecvBufSize - (uint32)m_nDataReceived;

	DWORD dwReadLen;
	DWORD dwFlags = 0;

//+lcj
	if (0 != WSARecv(m_hSock, &m_stRecvIoData.stWsaBuf, 1, &dwReadLen, &dwFlags, &m_stRecvIoData.stOverlapped, NULL))
	{
        int errNo = WSAGetLastError();

		if (errNo != WSA_IO_PENDING)
        {
            WARN("[%s:%d]post WSARecv failed, errNo=%d, %p ", MSG_MARK, errNo, m_pRecvBuf);
			return false;
        }
	}
	IncPostRecv();
	return true;
}

bool CCPSock::_PostSend()
{
	if (!IsConnect())
	{
		return true;
	}
	ZeroMemory(&m_stSendIoData.stOverlapped, sizeof(m_stSendIoData.stOverlapped));

	DWORD dwNumberOfBytesSent = 0;
	if (0 != WSASend(m_hSock, &m_stSendIoData.stWsaBuf, 1, &dwNumberOfBytesSent, 0, &m_stSendIoData.stOverlapped, NULL))
	{
		if(WSAGetLastError() != WSA_IO_PENDING)
        {
			return false;
		}
	}
    IncPostSend();//increase only when post event to iocp 
	return true;
}

bool CCPSock::_PostClose(SOCKET hSock)
{
	m_stCloseIoData.stWsaBuf.buf = NULL;
	m_stCloseIoData.stWsaBuf.len = 0;
	
	ZeroMemory(&m_stCloseIoData.stOverlapped, sizeof(m_stCloseIoData.stOverlapped));

	DWORD dwNumberOfBytesSent = 0;
	if (0 != WSASend(m_hSock, &m_stCloseIoData.stWsaBuf, 1, &dwNumberOfBytesSent, 0, &m_stCloseIoData.stOverlapped, NULL))
	{
		int ret = WSAGetLastError(); 
		if(WSAGetLastError() != WSA_IO_PENDING)
        {
			return false;
		}
	}

	/*
	ZeroMemory(&m_stCloseIoData.stOverlapped, sizeof(m_stCloseIoData.stOverlapped));
	m_stCloseIoData.hSock= hSock; 
	return CIocpCtrl::Instance()->PostIocpCompletionStatus(0,(ULONG_PTR)&m_stPerHandleData,&m_stCloseIoData.stOverlapped); 
	*/
	return true;
}

void CCPSock::_OnError(int32 nSDError, int32 nSysError)
{
	CEventMgr::Instance()->PushErrorEvt(m_pConnData, GetConnectionID(), nSDError, nSysError);
}


