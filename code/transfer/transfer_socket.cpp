#include "transfer_pch.h"
#include "transfer_socket.h"
#include "packet_parser.h"
#include "protocol_define.h"
#include "real_transfer_client.h"
#include "message_encoder.h"

#ifdef WIN32 // 客户端只Windows

TransferSocket::TransferSocket(RealTransferClient* pClientProxy) :
m_bCanSend(false),
m_pClientProxy(pClientProxy)
{
    ZeroMemory(m_szNetBuf, sizeof(m_szNetBuf));
    m_nDataLen = 0;
}

TransferSocket::~TransferSocket()
{

}


void TransferSocket::OnConnect( int nErrorCode )
{
    if( nErrorCode != 0 )
    {
        OnClose(GetLastError());		// 连接不上
        return;
    }

    //m_pClientProxy->OnReceive( MSG_NET_CONNECTION, 0 );
}

void TransferSocket::OnClose( int nErrorCode )
{
    //m_pClientProxy->OnReceive( MSG_NET_TERMINATE, 0 );
}

void TransferSocket::_OnReceive( int nErrorCode )
{
    int nRecvLen = Receive(m_szNetBuf + m_nDataLen , NET_MSG_BUFFER_LEN - m_nDataLen);

    switch (nRecvLen)
    {
    case 0:
        OnClose(SDASYNC_SOCK_ERR_NORMAL);		// 服务器主动断开
        break;
    case SOCKET_ERROR:
        if (GetLastError() != WSAEWOULDBLOCK) 
        {
            OnClose(GetLastError());			// 被断开了
        }
        break;
    default:
        {
            m_nDataLen += nRecvLen;
            while(m_nDataLen > 0)
            {

                int32 nUsed = PacketParser::Instance()->ParsePacket(m_szNetBuf, m_nDataLen);	// nUsed为该包的长度

                if(nUsed > 0)
                {
                    CnVerify(nUsed <= m_nDataLen);

                    // 解包头
                    const STNetMsgHeader* pstHeader = reinterpret_cast<STNetMsgHeader*>(const_cast<char *>(m_szNetBuf));
                    uint32 dwMsgID = ntohl(pstHeader->dwMsgID);
                    uint32 dwDataLen = ntohl(pstHeader->dwDataLen);
                    CnVerify( (dwDataLen + sizeof(STNetMsgHeader)) == nUsed );

                    // Decode data.
                    char* pBodyData = const_cast<char*>(m_szNetBuf + sizeof(STNetMsgHeader));
                    int32 nBodyLen = nUsed - sizeof(STNetMsgHeader);
                    CnVerify(nBodyLen == (int32)dwDataLen);

                    CnAssert(false);
                     //m_pEncoder->Decode(0, dwMsgID, pBodyData, nBodyLen);
                    //char pData[MAX_PACK_LEN];
                    //int32 nDecodeLen = g_BnFGameProto.Decode( dwMsgID, pBodyData, nBodyLen, pData, MAX_PACK_LEN);
                    //if( nDecodeLen < 0 )
                    //	CnVerify(false);
                    //else
                    //{	// 协议包，接受到了，上传给应用层
                    //	m_pClientProxy->OnReceive( dwMsgID, pData );
                    //}

                    m_nDataLen -= nUsed;
                    memmove_s(m_szNetBuf, NET_MSG_BUFFER_LEN, m_szNetBuf + nUsed, m_nDataLen);
                }
                else if( nUsed == 0)
                {
                    if( m_nDataLen > NET_MSG_BUFFER_LEN )
                    {
                        Close(SDASYNC_SOCK_ERR_PACKET);			// 数据包太长，超过了buffer大小，没法收，主动断开连接
                    }
                    break;
                }
                else
                {   // 包头数据不合法
                    Close(SDASYNC_SOCK_ERR_PACKET);
                    break;
                }
            } // while
        }	// default:
        break;
    }

}

void TransferSocket::OnSend( int nErrorCode )
{
    CnVerify(!m_bCanSend);
    m_bCanSend = true;
    //m_pClientProxy->OnReceive( MSG_NET_ESTABLISH, 0 );
}

void TransferSocket::OnOutOfBandData( int nErrorCode )
{

}

void TransferSocket::OnAccept( int nErrorCode )
{
    CnVerify(false);			// 我只做是客户端用，只发起连接
}

//bool TransferSocket::SendMsg( uint32 dwMsgID, void* pData )
//{
//    CnVerify(m_bCanSend);			// 还未连接上，就发消息了
//
//    STNetMsgHeader stHeader;
//    stHeader.dwMsgID = dwMsgID;
//    return SendDataWithHead( &stHeader, pData );
//}
//
//bool TransferSocket::SendDataWithHead( const STNetMsgHeader* pstHeader, void* pData )
//{
//    return false;
//
//    //char pBuf[MAX_PACK_LEN];
//
//    //// 数据打包
//    //char* pBodyData = pBuf + sizeof(STNetMsgHeader);
//    //int32 nBodyLen = MAX_PACK_LEN - sizeof(STNetMsgHeader);
//    //int32 nEncodeLen = g_BnFGameProto.Encode( pstHeader->dwMsgID, pData);
//
//    //if(nEncodeLen <= 0)
//    //	return false;
//
//    //// 整理包头
//    //STNetMsgHeader* pTempHeader = reinterpret_cast<STNetMsgHeader*>(pBuf);
//    //int32 nTotalLen = nEncodeLen + sizeof(STNetMsgHeader);
//    //pTempHeader->dwMsgID = htonl(pstHeader->dwMsgID);
//    //pTempHeader->dwDataLen = htonl(nEncodeLen);
//
//    //return SendRawData(pBuf, nTotalLen);
//}

//bool TransferSocket::SendRawData(const char* pData, uint32 nLen)
//{
//    CnVerify(pData);
//    int len = Send( pData, nLen );
//    if( len == SOCKET_ERROR )
//    {
//        int error = WSAGetLastError();
//        if( WSAEWOULDBLOCK == error )
//        {
//            CnVerify(m_bCanSend);
//            m_bCanSend = false;
//        }
//        else if(WSAENOTSOCK == error)
//        {
//        }
//        else
//        {
//            CnVerify(false);
//        }
//        return false;
//    }
//    else
//        return true;
//}

#endif // WIN32
