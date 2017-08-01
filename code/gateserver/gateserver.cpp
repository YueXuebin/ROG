// 游戏服务器
#include "gateserver_pch.h"
#include "gateserver.h"
#include "net_manager.h"
#include "real_transfer_client.h"
#include "real_transfer_server.h"
#include "config_loader.h"
#include "client_handler.h"
#include "game_handler.h"
#include "center_handler.h"
#include "player_manager.h"
#include "flash_handler.h"
#include "udp_log_handler.h"

extern bool IsRunning;

IMPLEMENT_SINGLETON(GateServer)

void PrintLogo(uint32 serverID)
{
    Crown::ColorLog::Instance()->ColorPrint(0, FC_BLACK, BC_YELLOW, "+=======================================+\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_YELLOW, BC_BLACK, "|   ____      _                         |\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_YELLOW, BC_BLACK, "|  |    \\ ___|_|___ ___ ___ _____ ___   |\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_YELLOW, BC_BLACK, "|  |  |  | . | |   | . | .'|     | -_|  |\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_YELLOW, BC_BLACK, "|  |____/|___|_|_|_|_  |__,|_|_|_|___|  |\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_YELLOW, BC_BLACK, "|                  |___|                |\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_BLACK, BC_YELLOW, "+===================+===================+\n");
    Crown::ColorLog::Instance()->ColorPrint(0, FC_YELLOW, BC_BLACK, "|  GateServer - %02u  |\n", serverID);
    Crown::ColorLog::Instance()->ColorPrint(0, FC_BLACK, BC_YELLOW, "+===================+\n");
}

bool    IsRunning   = true;     // 运行状态
int     IsBusy      = 0;        // 是否高负载
//const uint32 ServerUpdateTime = 5;

Crown::CSDNamedSemaphoreObject g_SemObj;        // GateServer唯一标识用信号量

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
    static uint32 RunningCount = 0;

    if(frameFlash > frameCurrent)
        return;

    frameFlash = frameCurrent + 1000;

    uint32 p = RunningCount++ % 3;
    if(p==0)
        if(IsBusy)
            printf("*  \b\b\b");
        else
            printf(".  \b\b\b");
    else if(p==1)
        if(IsBusy)
            printf("** \b\b\b");
        else
            printf(".. \b\b\b");
    else
        if(IsBusy)
            printf("***\b\b\b");
        else
            printf("...\b\b\b");
}

// ==================================================
// 构造
// ==================================================
GateServer::GateServer()
{
    m_serialNum = 0;
}

// ==================================================
// 析构
// ==================================================
GateServer::~GateServer()
{
    UnInit();
}

bool GateServer::Init(uint32 id, const char* path)
{
    ConfigLoader::CreateInstance();

    // 读取配置文件
    if(!ConfigLoader::Instance()->LoadConfig(path))
    {
        CnDbg("config error %s\n", path);
        return false;
    }
    ConfigLoader::Instance()->SetMyID(id);

    // 创建编码器
    CMessageEncoder::CreateInstance();

    // 创建网络处理器
    Net_Manager::CreateInstance();

    // 创建角色管理
    PlayerManager::CreateInstance();

    // 客户端消息处理
    ClientHandler::CreateInstance();

    // 游戏服务器消息处理
    GameHandler::CreateInstance();

    // 中心服务器消息处理
    CenterHandler::CreateInstance();

    // 中心服务器消息处理
    FlashHandler::CreateInstance();

    // 日志收集处理
    UdpLogHandler::CreateInstance();

    // 读取Flash安全监听端口
    if(id == 1)
    {
        const ServerConfig& falshSecurity = ConfigLoader::Instance()->GetListenFlashSecurity();
        FlashHandler::Instance()->Init(falshSecurity.m_ip.c_str(), falshSecurity.m_port);
    }

    // 初始化udp日志处理
    if(id == 1)
    {
        const ServerConfig& udplog = ConfigLoader::Instance()->GetListenUdpLog();
        UdpLogHandler::Instance()->Init(udplog.m_ip.c_str(), udplog.m_port);
    }

    const ServerConfig& listenGate = ConfigLoader::Instance()->GetListenGate();
    if(!CenterHandler::Instance()->Init(listenGate.m_outer_ip.c_str(), listenGate.m_port))  // 连接Center使用外网地址
    {
        return false;
    }

    return true;
}

void GateServer::UnInit()
{
    // 日志收集处理
    UdpLogHandler::DestroyInstance();

    // 客户端消息处理
    FlashHandler::DestroyInstance();

    Net_Manager::DestroyInstance();
    CMessageEncoder::DestroyInstance();
    CenterHandler::DestroyInstance();
    GameHandler::DestroyInstance();
    ClientHandler::DestroyInstance();
    ConfigLoader::DestroyInstance();
    PlayerManager::DestroyInstance();
}

void GateServer::Run()
{
    uint32 frameNext = 0;      // 下一帧滴答
    uint32 frameCurrent = 0;   // 当前帧滴答

    frameNext = frameCurrent = Crown::SDGetTickCount();

    // 主循环
    while (IsRunning)
    {
        frameCurrent = Crown::SDGetTickCount();

        Crown::ColorLog::Instance()->Update();

        ShowRunning(frameCurrent);  // 终端进度条

        UpdateNet();                            // 处理网络消息

        if(frameNext <= frameCurrent)
        {
            CenterHandler::Instance()->Update();    // 与Center间的断线重连

            FlashHandler::Instance()->Update();

            UdpLogHandler::Instance()->Update();

            PlayerManager::Instance()->Update();


            // 心跳处理
            if((frameNext + g_ServerUpdateTimeBusy) >= frameCurrent)
            {
                frameNext += g_ServerUpdateTime;
                if(IsBusy > 0)
                    IsBusy--;
            }
            else
            {
                if((frameNext + g_ServerUpdateTimeThreshold) >= frameCurrent)
                {
                    // 超过阀值太多，不再追赶心跳
                    frameNext = frameCurrent + g_ServerUpdateTime;
                }
                else
                {
                    // 可能是处理的抖动，看看后续心跳是否能追赶上
                    frameNext += g_ServerUpdateTime;
                }

                if(IsBusy < 10)
                    IsBusy++;   // update超时，进入高负载状态
            }
        }

        Crown::SDSleep( 1 );
    }

    // gateserver正常关闭前的处理
    BeforeCloss();
}

void GateServer::BeforeCloss()
{
    // 通知客户端，服务器正常关闭
    GATESVR_CLOSE_NTF closeNtf;
    closeNtf.reason = ERR_SUCCEED;
    PlayerManager::Instance()->SendToAll(MSG_GATESVR_CLOSE_NTF, &closeNtf);

    // 关闭前等待2秒
    int32 beforeTime = 2000;        
    int32 startTime = Crown::SDGetTickCount();
    while ( ((int32)Crown::SDGetTickCount() - startTime) <= beforeTime )
    {
        BeforeCloseUpdate();
        Crown::SDSleep( 50 );
    }
}

void GateServer::BeforeCloseUpdate()
{

}

void GateServer::UpdateNet()
{
    CMessageEncoder::m_TimeCounter = Crown::SDGetTickCount();

    bool delpacket= true;
    Net_Event* event = nullptr;



    // 处理网络事件
    while(event = Net_Manager::Instance()->get_event())
    {
        switch(event->net_event_type)
        {
            //TYPE_NULL,              //!< 空事件
            //TYPE_DATA,              //!< 数据到达
            //TYPE_CONNECT,           //!< tcp连接成功
            //TYPE_ACCEPT,            //!< tcp接受连接
            //TYPE_CLOSE,             //!< tcp连接关闭
            //TYPE_LISTEN_CLOSE,      //!<侦听socket关闭
            //TYPE_EXCEPTION,         //!< 异常
            //TYPE_LISTEN_EXCEPTION,  //!<侦听socket异常
            //TYPE_TIMEOUT,           //!< 超时
            //TYPE_SYS_ERROR          //!< 系统异常
        case TYPE_DATA:
            {
                if(event->UserData)
                {
                    ((RecvNetPacket)event->UserData)(TYPE_DATA, event->id, &event->packet, delpacket);
                }
            }
            break;
        case TYPE_ACCEPT:
            {
                if(event->UserData)
                {
                    CnDbg("OnAccept ip=%s port=%d\n", Crown::GetIpStr(event->remote_addr.get_addr()).c_str(), event->remote_addr.get_port());
                    ((RecvNetPacket)event->UserData)(TYPE_ACCEPT, event->new_id, &event->packet, delpacket);
                }
            }
            break;
        case TYPE_CONNECT:
            {
                if(event->UserData)
                {
                    ((RecvNetPacket)event->UserData)(TYPE_CONNECT, event->id, &event->packet, delpacket);
                }
            }
            break;
        case TYPE_CLOSE:
        case TYPE_LISTEN_CLOSE:
            {
                if(event->UserData)
                {
                    CnDbg("OnClose ip=%s port=%d\n", Crown::GetIpStr(event->remote_addr.get_addr()).c_str(), event->remote_addr.get_port());
                    ((RecvNetPacket)event->UserData)(TYPE_CLOSE, event->id, &event->packet, delpacket);
                }
            }
            break;
        case TYPE_EXCEPTION:
        case TYPE_LISTEN_EXCEPTION:
        case TYPE_TIMEOUT:
        case TYPE_SYS_ERROR:
            {
                if(event->UserData)
                {
                    CnDbg("OnException ip=%s port=%d type=%d\n", Crown::GetIpStr(event->remote_addr.get_addr()).c_str(), event->remote_addr.get_port(), event->net_event_type);
                    ((RecvNetPacket)event->UserData)(TYPE_EXCEPTION, event->id, &event->packet, delpacket);
                }
            }
            break;
        case TYPE_NULL:
            {

            }
            break;
        default:
            break;
        }

        if(delpacket)
        {
            delete event;
            event = nullptr;
        }
    }
}

// ==================================================
// 生成全局的登陆序列号
// ==================================================
uint64 GateServer::GenLoginUUID()
{
    // 时间戳 32bit, 序列号16bit, 
    uint64 sn = 0;
    sn = (uint32)time(0);
    sn = sn << 32;
    sn |= ++m_serialNum << 16;
    sn |= ConfigLoader::Instance()->GetMyID() & 0xFFFF;
    return sn;
}

// ==================================================
// 连接游戏服务器
// ==================================================
bool GateServer::ConnectToGame(uint32 id, const char* ip, uint32 port)
{
    if(m_gameProxy.find(id) != m_gameProxy.end())
    {
        WARN("Gameserver%u repeat", id);
        return false;
    }

    return true;
}

// ==================================================
// 给中心服务器发送消息
// ==================================================
void GateServer::SendToCenter(uint32 msgID, void* data)
{
    CnDbg("Send center mid=%d\n", msgID);
}

uint32 serverID = 1;

void help()
{
    printf(
        "Usage: gateserver [OPTION] [INPUT]\n"
        "   -h              help\n"
        "   -i              id (default 1)\n"
        "example: gateserver -i 123\n"
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
    std::string title = "gateserver";
    for(int32 i=1; i<argc; i++)
    {
        title += " ";
        title += argv[i];
    }

    SetConsoleTitle(title.c_str());
}
#endif

int guard_main(int argc, char* argv[])
{
    ParseCmd(argc, argv);

    // 初始化日志
    Crown::ColorLog::CreateInstance();
    char logname[30];
    SDSnprintf(logname, 30, "gatesvr-%02u", serverID);
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

    CnInfo("timezone=%d\n", Crown::SDGetTimeZone());

    GateServer::CreateInstance();

    // 初始化
    CnInfo("GateServer init pid = %d\n", Crown::SDGetCurrentProcessId());
    if(!GateServer::Instance()->Init(serverID, RES_CONFIG_PATH""CENTER_CONFIG_FILE))
    {
        return -1;
    }

    GateServer::Instance()->Run();

    GateServer::DestroyInstance();

    CnInfo("GateServer End\n");

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

