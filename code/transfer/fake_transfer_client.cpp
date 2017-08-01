#include "transfer_pch.h"
#include "fake_transfer_client.h"
#include "fake_transfer_server.h"
#include "fake_network_pipe.h"

FakeNetWorkPipe*	g_C2SPipe = 0;

FakeTransferClient*	g_FakeTransferClient = 0;


FakeTransferClient::FakeTransferClient() :
m_pRecvFunc(0),
m_pUserData(0),
bFirstUpdate(false)
{
    CnVerify(!g_C2SPipe);
    g_C2SPipe = CnNew FakeNetWorkPipe;
}

FakeTransferClient::~FakeTransferClient()
{
    CnDelete g_C2SPipe;
    g_C2SPipe = nullptr;
}

void FakeTransferClient::SetRecvFunc( CliRecvFunc pRecvFunc, void* pUserData )
{
    m_pRecvFunc = pRecvFunc;
    m_pUserData = pUserData;
}

bool FakeTransferClient::Send( uint32 MsgId, void* pData )
{
    if( NETPACK_DELAY_MAX == 0 )	// 无延时
    {
        g_FakeTransferServer->OnFakeRecv(0, MsgId, pData);
        return true;
    }

    // 数据打包
    //char pBuf[MAX_PACK_LEN];	
    //char* pBodyData = pBuf + sizeof(STNetMsgHeader);
    //int32 nBodyLen = MAX_PACK_LEN - sizeof(STNetMsgHeader);
    //int32 nEncodeLen = g_BnFGameProto.Encode( MsgId, pData, pBodyData, nBodyLen);

    // 整理包头
    //STNetMsgHeader* pTempHeader = reinterpret_cast<STNetMsgHeader*>(pBuf);
    //int32 nTotalLen = nEncodeLen + sizeof(STNetMsgHeader);
    //pTempHeader->dwMsgID = MsgId;
    //pTempHeader->dwDataLen = nEncodeLen;

    //g_C2SPipe->Send(pBuf, nTotalLen);

    return true;
}

void FakeTransferClient::Update()
{
    if(!bFirstUpdate)
    {	// 伪造 Server连接上的通知
        OnFakeRecv(0, 0);
        bFirstUpdate = true;
    }

    char	pData[MAX_PACK_LEN];
    uint32	len = 0;
    while( g_S2CPipe && ( (len = g_S2CPipe->Recv(pData, MAX_PACK_LEN)) > 0 )  )
    { 
        // 解包头
        const STNetMsgHeader* pstHeader = reinterpret_cast<STNetMsgHeader*>(const_cast<char *>(pData));
        uint32 uMsgID = pstHeader->dwMsgID;
        uint32 dwDataLen = pstHeader->dwDataLen;

        // Decode data.
        char* pBodyData = const_cast<char*>(pData + sizeof(STNetMsgHeader));
        int32 nBodyLen = dwDataLen + sizeof(STNetMsgHeader);

        //char pData[MAX_PACK_LEN];
        //int32 nDecodeLen = g_BnFGameProto.Decode( wMsgID, pBodyData, nBodyLen, pData, MAX_PACK_LEN);
        //CnVerify(nDecodeLen > 0);

        //if(m_pRecvFunc)
        //{
        //	(*m_pRecvFunc)( wMsgID, pData, m_pUserData );
        //}
        //else
        //	CnVerify(false);
    }
}

void FakeTransferClient::OnFakeRecv( uint32 MsgId, void* pData )
{
    if(m_pRecvFunc)
    {
        (*m_pRecvFunc)( MsgId, pData, m_pUserData );
    }
    else
        CnVerify(false);
}
