// 游戏服务器
#include <stdlib.h>
#include "sdu.h"
#include "commonlib.h"
#include "transfer.h"
#include "gameholder.h"




void PrintLogo(uint32 serverID)
{
    Crown::ColorLog::Instance()->ColorPrint(0, FC_BLACK, BC_RED, "+=======================================+\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_RED, BC_BLACK, "|   ____      _                         |\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_RED, BC_BLACK, "|  |    \\ ___|_|___ ___ ___ _____ ___   |\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_RED, BC_BLACK, "|  |  |  | . | |   | . | .'|     | -_|  |\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_RED, BC_BLACK, "|  |____/|___|_|_|_|_  |__,|_|_|_|___|  |\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_RED, BC_BLACK, "|                  |___|                |\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_BLACK, BC_RED, "+===================+===================+\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_RED, BC_BLACK, "|  GameServer - %02u  |\n", serverID);
    Crown::ColorLog::Instance()->ColorPrint(0, FC_BLACK, BC_RED, "+===================+\n");
}

bool IsRunning = true;
int32 IsBusy = 0;             // 是否高负载
GameHolder* g_gameholder;

int32 allTime = 0;
int32 sleepTime = 0;

Crown::CSDNamedSemaphoreObject g_SemObj;        // GameServer唯一标识用信号量

void SignalHandler(int sig)
{
#ifdef LINUX
    if (sig == SIGINT)
    {
        CnDbg("SIGINT received\n");
        IsRunning = false;
    }
#else
    if (sig == SIGINT || sig == SIGBREAK)
    {
        if(sig == SIGINT)
            CnDbg("SIGINT received\n");
        else if(sig == SIGBREAK)
            CnDbg("SIGBREAK received\n");
        IsRunning = false;
    }
#endif

    g_SemObj.Release();
}

void ShowRunning(uint32 frameCurrent)
{
    static uint32 frameFlash = 0;
    static uint32 runningCount = 0;

    if(frameFlash > frameCurrent)
        return;

    // 计算系统负载
    real sysLoad = 0;
    if(allTime > 0)
        sysLoad = 1 - Crown::SDRange((real)0, (real)sleepTime / allTime, (real)1);
    int32 sysLoadPer = int32(sysLoad*100);
    g_gameholder->sysLoadPercent = sysLoadPer;
    sleepTime = allTime = 0;

    // console显示
    frameFlash = frameCurrent + 1000;

    uint32 p = runningCount++ % 3;
    if(p==0)
        if(IsBusy)
            printf("%0.2d%%*  \b\b\b\b\b\b", sysLoadPer);
        else
            printf("%0.2d%%.  \b\b\b\b\b\b", sysLoadPer);
    else if(p==1)
        if(IsBusy)
            printf("%0.2d%%** \b\b\b\b\b\b", sysLoadPer);
        else
            printf("%0.2d%%.. \b\b\b\b\b\b", sysLoadPer);
    else
        if(IsBusy)
            printf("%0.2d%%***\b\b\b\b\b\b", sysLoadPer);
        else
            printf("%0.2d%%...\b\b\b\b\b\b", sysLoadPer);

    if(IsBusy)
        CnInfo("InBusy\n");
}

uint32 serverID = 1;

void help()
{
    printf(
        "Usage: gameserver.exe server_id config_path\n"
        "   -h              help\n"
        "   -i              id (default 1)\n"
        "example: gameserver -i 123\n"
        );
}

void ParseCmd(int argc, char* argv[])
{
    while(1) 
    {
        int c = getopt(argc, argv, "-hi:");
        if(c == -1) break;
        switch(c) 
        {
        case 'h':
            help();
            break;
        case 'i': 
            serverID = arg_to_int(optarg, 1, "i");
            break;
        }
    }
}

#ifdef WIN 
void Title(int argc, char* argv[])
{
    std::string title = "gameserver";
    for(int32 i=1; i<argc; i++)
    {
        title += " ";
        title += argv[i];
    }

    SetConsoleTitle(title.c_str());
}
#endif

int32 guard_main(int argc, char* argv[])
{
    ParseCmd(argc, argv);

    // 初始化日志
    char logname[30];
    SDSnprintf(logname, 30, "gamesvr-%02u", serverID);
    Crown::ColorLog::CreateInstance();
    Crown::ColorLog::Instance()->Init("log", logname);

    PrintLogo(serverID);
    Crown::ColorLog::Instance()->SetLogTime(true);

    SignalCatcher(SignalHandler);

    // 初始化
    CnInfo("version:%d.%d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_PROTOCOL);
    CnInfo("Server init pid = %d\n", Crown::SDGetCurrentProcessId());
    CnInfo("timezone=%d\n", Crown::SDGetTimeZone());

    // 检查进程信号量
    g_SemObj = CheckNamedSemaphore(argc, argv);
    if(!g_SemObj.IsValid())
    {
        CnError("server already exist\n");
        return EXIT_FAILURE;
    }

    g_gameholder = CnNew GameHolder;
    if(!g_gameholder->Init(serverID))
    {   // 初始化错误
        CnDbg("Server Init failed, exit after 5 second!\n");
        g_gameholder->Uninit();
        CnDelete g_gameholder;
        Crown::SDSleep(5000);
        return 1;
    }

    // 开始运行
    //CnInfo("GameServer start\n");

    uint32 NextTick = 0;        // 下一帧滴答
    uint32 CurrentTick = 0;     // 当前帧滴答
    uint32 LastTick = 0;
    uint32 FrameCounter = 0;    // 帧数

    NextTick = LastTick = CurrentTick = Crown::SDGetTickCount();
    
    bool needRunning = true;
    while (needRunning)
    {
        Crown::ColorLog::Instance()->Update();

        LastTick = CurrentTick;
        CurrentTick = Crown::SDGetTickCount();

        allTime += (CurrentTick - LastTick);

        ShowRunning(CurrentTick);  // 终端进度条

        int32 GapTick = NextTick - CurrentTick;

        if(NextTick <= CurrentTick)
        {
            g_gameholder->Update(FrameCounter); // 游戏逻辑心跳更新
            FrameCounter++;

            if((NextTick + g_ServerUpdateTimeBusy) >= CurrentTick)
            {
                NextTick += g_ServerUpdateTime;
                if(IsBusy > 0) --IsBusy;
            }
            else
            {
                if((NextTick + g_ServerUpdateTimeThreshold) >= CurrentTick)
                {
                    // 超过阀值太多，不再追赶心跳
                    NextTick = CurrentTick + g_ServerUpdateTime;
                }
                else
                {
                    // 可能是处理的抖动，看看后续心跳是否能追赶上
                    NextTick += g_ServerUpdateTime;
                }

                if(IsBusy < 10) ++IsBusy;   // update超时，进入高负载状态
            }
        }
        else
        {
            sleepTime += (GapTick+1);
            Crown::SDSleep( GapTick+1 );
        }

        if(!IsRunning && !g_gameholder->IsExit())
        {
            g_gameholder->RequestExit();       // 请求安全退出
        }

        needRunning = IsRunning || (!g_gameholder->CanExit());
    }
    
    // 结束处理
    CnInfo("GameServer uninit\n");
    g_gameholder->Uninit();

    CnDelete g_gameholder;
    g_gameholder = NULL;

    CnInfo("GameServer End\n");

    SignalCatcher(NULL);
    Crown::ColorLog::DestroyInstance();

    return 0;
}

int main(int argc, char* argv[])
{
#ifdef WIN 
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);     // check memory leak
    Title(argc, argv);
#endif

    int32 ret = 0;

    //try
    //{
        ret = guard_main(argc, argv);
    //}
    //catch(...)
    //{
    //    ErrorReporter("exception");
    //}

    return ret;
}



// 对player处理协议的异常保护
//try
//{
//    int32 sig;
//    if ((sig = SDSetJmp(g_JmpBuffer)) == 0)
//    {
//        g_InSetJmp = true;
//        player->OnRecv(msgID, data);            // 如果在OnRecv中抛异常，只会让玩家下线不会宕机
//        g_InSetJmp = false;
//    }
//    else
//    {   // signal longjmp回来后执行
//        g_InSetJmp = false;
//        throw(sig);
//    }
//}
//catch (int32 &z)
//{
//    g_InSetJmp = false;
//
//    CnFatal("player %s msg %d signal %d\n", player->GetName().c_str(), msgID, z);
//    // 迫使该玩家下线
//    PlayerManager::Instance()->KickOutPlayer(ERR_SIGNAL_EXCEPTION, player->GetPlayerID());      // 代码执行错误，抛异常迫使玩家下线
//}