#include "server_info_pch.h"
#include "server_info.h"
#include "net_manager.h"
#include "server_info_config.h"
#include "server_info_handler.h"

IMPLEMENT_SINGLETON(ServerInfo)



bool IsRunning = true;
ServerInfoHandler* g_infoholder;
const uint32 UpdateTime = 100;      //update更新周期(ms)
int32 option;                                               ///< 查看不同内容

void ShowRunning(uint32 frameCurrent)
{
    static uint32 frameFlash = 0;
    static uint32 RunningCount = 0;

    if(frameFlash > frameCurrent)
        return;

    frameFlash = frameCurrent + 1000;

    if(ServerInfo::Instance()->GetOption() == kNumber)
        return;

    uint32 p = RunningCount++ % 3;
    if(p==0)
        printf(".  \b\b\b");
    else if(p==1)
        printf(".. \b\b\b");
    else
        printf("...\b\b\b");
}


void help()
{
    printf(
        "Usage: server_info.exe server_id config_path\n"
        "   -h              help\n"
        "   -n              Number of online gamers\n"
        "   -i              Information for players\n"
        "   -p              Where are player\n"
        "   -d              Information for dungeon\n"
        );
}


bool ParseCmd(int argc, char* argv[])
{
    bool hasOneOption = false;
    while(1) 
    {
        int c = getopt(argc, argv, "-hnipd");
        if(c == -1) 
            break;
        
        hasOneOption = true;

        switch(c) 
        {
        case 'h':
            help();
            break;
        case 'n': 
            ServerInfo::Instance()->SetOption(kNumber);
            break;
        case 'i':
            ServerInfo::Instance()->SetOption(kInformation);
            break;
        case 'p':
            ServerInfo::Instance()->SetOption(kPlayerDungeon);
            break;
        case 'd':
            ServerInfo::Instance()->SetOption(kDungeonInfo);
            break;
        default:
            ServerInfo::Instance()->SetOption(kNumber);
            break;
        }
    }
    
    if(hasOneOption)
        return true;
    else
    {
        help();
        return false;
    }
}



int main(int argc, char* argv[])
{
#ifdef WIN 
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);     // check memory leak
#endif
    // 初始化日志
    char logname[20];
    SDSprintf(logname, "server_info");
    Crown::ColorLog::CreateInstance();
    Crown::ColorLog::Instance()->Init(NULL, NULL);
    Crown::ColorLog::Instance()->SetLogTime(true);

    // 初始化
    CnInfo("Server init\n");
    ServerInfo::CreateInstance();

    if(!ParseCmd(argc, argv))
    {
        ServerInfo::DestroyInstance();
        Crown::ColorLog::DestroyInstance();
        return 0;
    }

    if(!ServerInfo::Instance()->Init(RES_CONFIG_PATH""CENTER_CONFIG_FILE))
    {
        return -1;
    }

    uint32 frameNext = 0;      // 下一帧滴答
    uint32 frameCurrent = 0;   // 当前帧滴答

    frameNext = frameCurrent = Crown::SDGetTickCount();

    while (IsRunning)
    {
        Crown::ColorLog::Instance()->Update();

        frameCurrent = Crown::SDGetTickCount();

        ShowRunning(frameCurrent);  // 终端进度条

        if(frameNext <= frameCurrent)
        {
            g_infoholder->Update();     // 游戏逻辑心跳更新

            if((frameNext + UpdateTime) >= frameCurrent)
            {
                frameNext += UpdateTime;
            }
            else
            {
                if((frameNext + UpdateTime) >= frameCurrent)
                {
                    // 超过阀值太多，不再追赶心跳
                    frameNext = frameCurrent + UpdateTime;
                }
                else
                {
                    // 可能是处理的抖动，看看后续心跳是否能追赶上
                    frameNext += UpdateTime;
                }

            }
        }
        else
        {
            Crown::SDSleep( 50 );
        }
    }
    // 结束处理
    ServerInfo::Instance()->Uninit();
    ServerInfo::DestroyInstance();

    //CnInfo("Server end\n");

    Crown::ColorLog::DestroyInstance();

    return 0;
}

bool ServerInfo::Init(const char* path)                            ///< 初始化
{
    ConfigLoader::CreateInstance();

    //读取配置文件
    if (!ConfigLoader::Instance()->LoadConfig(path))
    {
        CnDbg("config error %s\n",path);
        return false;
    }

    // 创建编码器
    CMessageEncoder::CreateInstance();

    // 创建网络处理器
    Net_Manager::CreateInstance();

    // 中心服务器消息处理
    ServerInfoHandler::CreateInstance();

    ServerConfig& listenInfo = ConfigLoader::Instance()->GetListenInfo();

    if(!ServerInfoHandler::Instance()->Init(listenInfo.m_ip.c_str(), listenInfo.m_port))
    {
        return false;
    }

    return true;
}

void ServerInfo::Uninit()
{
    ServerInfoHandler::DestroyInstance();

    Net_Manager::DestroyInstance();

    CMessageEncoder::DestroyInstance();

    ConfigLoader::DestroyInstance();
}
