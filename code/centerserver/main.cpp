#include "centerserver_pch.h"
#include "centerserver.h"
#include "dbdriver.h"

void help()
{
    printf(
        "Usage: centerserver [OPTION]\n"
        "   -h              help\n"        
        "example: gateserver\n"
        );
}


void ParseCmd(int argc, char* argv[])
{
    while(1) 
    {
        int c = getopt(argc, argv, "-h");
        if(c == -1) break;
        switch(c) 
        {
        case 'h':
            help();
            break;
        }
    }
}

#ifdef WIN
void Title(int argc, char* argv[])
{
    std::string title = "centerserver";
    for(int32 i=1; i<argc; i++)
    {
        title += " ";
        title += argv[i];
    }

    SetConsoleTitle(title.c_str());
}
#endif

Crown::CSDNamedSemaphoreObject g_SemObj;        // CenterServer信号量(标识唯一CenterServer用)

void SignalHandler(int sig)
{
#ifdef LINUX
    if (sig == SIGINT)
    {
        CnDbg("SIGINT received\n");
        CenterServer::Instance()->SetRun(false);
    }
#else
    if (sig == SIGINT || sig == SIGBREAK)
    {
        if(sig == SIGINT)
            CnDbg("SIGINT received\n");
        else if(sig == SIGBREAK)
            CnDbg("SIGBREAK received\n");
        CenterServer::Instance()->SetRun(false);
    }
#endif

    g_SemObj.Release();
}

int32 guard_main(int argc, char* argv[])
{
    ParseCmd(argc, argv);

    Crown::SDSetSeed((uint32)time(nullptr));

    // 初始化日志
    Crown::ColorLog::CreateInstance();
    Crown::ColorLog::Instance()->Init("log", "centersvr");

    CenterServer::PrintLogo();
    Crown::ColorLog::Instance()->SetLogTime(true);

    SignalCatcher(SignalHandler);

    // 检查进程信号量
    g_SemObj = CheckNamedSemaphore(argc, argv);
    if(!g_SemObj.IsValid())
    {
        CnError("server already exist\n");
        return EXIT_FAILURE;
    }

    CnInfo("version:%d.%d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_PROTOCOL);

    CnInfo("timezone=%d\n", Crown::SDGetTimeZone());

    CenterServer::CreateInstance();

    if(!CenterServer::Instance()->Init())
    {
        CnDbg("center server init failed, exit now!\n");
        return EXIT_FAILURE;
    }

    CenterServer::Instance()->Run();

    CenterServer::Instance()->onEnd();
    CenterServer::Instance()->UnInit();

    CenterServer::DestroyInstance();        // call UnInit
    CnInfo("CenterServer End\n");

    SignalCatcher(NULL);
    // 析构日志
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
