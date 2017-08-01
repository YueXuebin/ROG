#include "stdafx.h"
#include "gameserver.h"
#include "mfc_gameserver.h"
#include "MainFrm.h"
#include "mfc_gameserverDoc.h"
#include "mfc_gameserverView.h"

GameServer g_GameServer;
DWORD gServerID = 1;
uint32  g_FrameCount = 0;
uint32  g_FrameUsedTime = 0;

GameServer::GameServer() :
m_Init(false)
{
    m_lastTime = 0;
}

GameServer::~GameServer()
{

}

void ParseCmd(int argc, char* argv[])
{
    while(1) 
    {
        int c = getopt(argc, argv, "-hi:");
        if(c == -1) break;
        switch(c) 
        {
        case 'i': 
            gServerID = arg_to_int(optarg, 1, "i");
            break;
        }
    }
}

bool GameServer::Init()
{
    _ASSERT(!m_Init);
    m_Init = true;

    // 解析命令行参数
    ParseCmd(__argc, __argv);

    char logname[20];
    SDSprintf(logname, "mfcgamesvr-%02u", gServerID);
    Crown::ColorLog::CreateInstance();
    Crown::ColorLog::Instance()->Init("log", logname);
    Crown::ColorLog::Instance()->SetLogTime(true);

    m_gameholder = CnNew GameHolder;

    if(!m_gameholder->Init(gServerID))
    {	// 初始化错误
        CnAssert(false);
        return false;
    }

    return true;
}

void GameServer::Uninit()
{
    m_gameholder->Uninit();
    CnDelete m_gameholder;

    Crown::ColorLog::DestroyInstance();
}

void GameServer::Tick()
{
    uint32 currentTime = GetTickCount();
    if(m_lastTime == 0) 
        m_lastTime = currentTime;       // first tick

    while((currentTime - m_lastTime) >= g_ServerUpdateTime)
    {
        Crown::ColorLog::Instance()->Update();

        m_gameholder->Update(g_FrameCount);
        m_lastTime += g_ServerUpdateTime;

        g_FrameCount++;

        //_RPT2(_CRT_WARN, "%d %d\n", m_lastTime, currentTime);

        g_FrameUsedTime = GetTickCount() - currentTime;
        //_RPT1(_CRT_WARN, "frame use time:%d\n", g_FrameUsedTime);
    }

    // 渲染
    CMainFrame*   pFrame=(CMainFrame*)AfxGetMainWnd();
    Cmfc_gameserverView* view = (Cmfc_gameserverView*)pFrame->GetActiveView();
    view->Invalidate();

}
