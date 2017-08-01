// 游戏服务器
#include "loginserver.h"
#include "transfer.h"
#include "real_transfer_server.h"

IMPLEMENT_SINGLETON(LoginServer)

LoginServer::LoginServer()
{
    m_encoder = nullptr;
    m_transfer = nullptr;
}

LoginServer::~LoginServer()
{
    UnInit();
}

bool LoginServer::Init()
{
    m_encoder = CnNew CMessageEncoder();

    // 创建传输层服务端
    m_transfer = static_cast<RealTransferServer*>(CreateTransferServerProxy(true, 9527));

    m_transfer->SetRecvFunc(ProcMsg);
    m_transfer->SetDecoder(m_encoder);

    // 设置发送
    m_encoder->SetTransfer(m_transfer);

    return true;
}

void LoginServer::UnInit()
{
    // 释放传输层服务端
    if(m_transfer)
    {
        CnDelete m_transfer;
        m_transfer = nullptr;
    }

    if(m_encoder)
    {
        CnDelete m_encoder;
        m_encoder = nullptr;
    }
}

void LoginServer::Run()
{
    // 主循环
    while (true)
    {
        Crown::SDSleep(100);
        m_transfer->Update();
    }
}

void LoginServer::ProcMsg(uint32 sessionID, uint32 msgID, void* msg)
{
    DBG("recv %d", msgID);
    if(msgID == MSG_TEST1)
    {
        TEST_DTAT3 testData;
        testData.val1.val1 = 1;
        testData.val1.val2 = 2;
        testData.val1.val3 = -1024;
        testData.val1.val4 = 65535;
        testData.val1.val5 = 5;
        testData.val1.val6 = 6;
        testData.val1.val7 = 7.7f;
        testData.val1.val8 = "Fuck fu!";

        testData.val2.val1.push_back(1);
        testData.val2.val2.push_back(2);
        testData.val2.val3.push_back(-1024);
        testData.val2.val4.push_back(65535);
        testData.val2.val5.push_back(5);
        testData.val2.val6.push_back(6);
        testData.val2.val7.push_back(7.7f);
        testData.val2.val8.push_back("Fcuk fu again!");

        LoginServer::Instance()->Send(sessionID, MSG_TEST3, &testData);
    }
}

int main(int argc, char* argv[])
{
    LoginServer::CreateInstance();

    if(!LoginServer::Instance()->Init())
    {
        return -1;
    }

    LoginServer::Instance()->Run();

    LoginServer::DestroyInstance();

    return 0;
}

