#include "transfer_pch.h"
#include "session.h"
#include "session_manager.h"
#include "protocol_define.h"
#include "real_transfer_server.h"
#include <iostream>
#include <fstream>

using namespace std;

Session::Session() :
m_pServerProxy(NULL),
m_poConnection(NULL),
m_Id(-1),
m_poDecoder(NULL)
{

}

Session::~Session( void )
{

}


void Session::SetID( uint32 id )
{
    CnVerify(m_Id == (uint32)-1 || id == (uint32)-1);		// 该函数应该只会在该Session被创建和释放时，由SessionManager设置
    m_Id = id;
}

void SDAPI Session::SetConnection( Crown::ISDConnection* poConnection )
{
    CnVerify(!m_poConnection);
    m_poConnection = poConnection;
    m_pServerProxy->GetRecvFunc()(0, GetID(), MSG_NET_CONNECT, 0, m_pServerProxy->GetUserData());
}

void SDAPI Session::OnEstablish( void )
{
    CnVerify(m_poConnection);
    m_pServerProxy->GetRecvFunc()(0, GetID(), MSG_NET_CONNECT, 0, m_pServerProxy->GetUserData());
}

void SDAPI Session::OnTerminate( void )
{
    CnVerify(m_poConnection);
    m_poConnection = NULL;
    //m_pServerProxy->GetRecvFunc()(GetID(), MSG_NET_TERMINATE, 0, m_pServerProxy->GetUserData());
}

bool SDAPI Session::OnError( int32 nModuleErr, int32 nSysErr )
{
    CnVerify(m_poConnection);
    m_pServerProxy->GetRecvFunc()(0, GetID(), MSG_NET_ERROR, 0, m_pServerProxy->GetUserData());
    return true;
}

void SDAPI Session::OnRecv( const char* pBuf, uint32 dwLen )
{
    CnVerify(m_poConnection);
    CnVerify(m_pServerProxy);

    if (dwLen < sizeof(STNetMsgHeader))
        return;

    // 解包头
    const STNetMsgHeader* pstHeader = reinterpret_cast<STNetMsgHeader*>(const_cast<char *>(pBuf));
    uint32 dwMsgID = ntohl(pstHeader->dwMsgID);
    uint32 dwDataLen = ntohl(pstHeader->dwDataLen);

    // Decode data.
    char* pBodyData = const_cast<char*>(pBuf + sizeof(STNetMsgHeader));
    int32 nBodyLen = dwLen - sizeof(STNetMsgHeader);
    CnVerify(nBodyLen == (int32)dwDataLen);


    //int32 nDecodeLen = m_poDecoder->Decode(GetID(), dwMsgID, pBodyData, nBodyLen);
    //CnVerify(nDecodeLen > 0);
}

void SDAPI Session::Release( void )
{
    CnVerify(!m_poConnection);
    SessionManager::GetInstance()->ReleaseSession(this);
}

bool Session::Send(char* data, uint32 dataLen)
{
    // 数据打包
    //char pBuf[MAX_PACK_LEN];
    //char* pBodyData = pBuf + sizeof(STNetMsgHeader);
    //int32 nBodyLen = MAX_PACK_LEN - sizeof(STNetMsgHeader);
    //int32 nEncodeLen = g_BnFGameProto.Encode( MsgId, pData );

    //if(nEncodeLen <= 0)
    //    return false;

    // 整理包头
    //STNetMsgHeader* pTempHeader = reinterpret_cast<STNetMsgHeader*>(pBuf);
    //int32 nTotalLen = nEncodeLen + sizeof(STNetMsgHeader);
    //pTempHeader->dwMsgID = htonl(MsgId);
    //pTempHeader->dwDataLen = htonl(nEncodeLen);

    // 发送
    CnVerify(m_poConnection);
    if(!m_poConnection)
        return false;

    m_poConnection->Send(data, dataLen);

    //else
    //{
    //    m_poConnection->Send( pBuf, nTotalLen );
    //    if (!g_ProtoRecord.empty())
    //    {
    //        ofstream myFile;

    //        myFile.open (g_ProtoRecord.c_str(), ios::app | ios::binary);
    //        myFile.write((const char*)pData, nTotalLen);
    //        myFile.close();
    //    }

    //    return true;
    //}

    return true;
}
