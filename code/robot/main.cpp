#include "robot_pch.h"
#include "robot.h"
#include "robot_config.h"
#include "net_manager.h"
#include "main.h"
#include "script.h"
#include "chat_config.h"

bool mainLoop = true;
bool login = false;

int32 updateTime    = 100;          // 机器人心跳
uint32 g_GameTime   = 0;
char g_Account[100] = "robot";

std::list<Robot*> robot_list;

void SignalHandler(int sig)
{
#ifdef WIN
    if (sig == SIGINT || sig == SIGBREAK)
    {
        if(sig == SIGINT)
            CnDbg("SIGINT received\n");
        else if(sig == SIGBREAK)
            CnDbg("SIGBREAK received\n");
#else
    if (sig == SIGINT)
    {
        CnDbg("SIGINT received\n");
#endif
        mainLoop = false;
    }
}

void Help()
{
        printf(
        "Help: \n"
        "   -h              help\n"
        "   -i              ip, default \"127.0.0.1\"\n"
        "   -p              port, default \"0\"\n"
        "   -n              name, default \"robot_\"\n"
        "   -u              number, default \"1\"\n"
        "   -f              lua file name, default \"test.lua\"\n"
        );
}

bool ParseCmd(int argc, char* argv[])
{
    while(1)
    {
        int c = getopt(argc, argv, "-hi:p:n:u:f:");
        if(c == -1)
            break;

        switch(c) 
        {
            case 'i':   // ip
                {
                    std::string ip = optarg;
                    RobotConfig::Instance()->SetIp(ip);
                }
                break;
            case 'p':   // port
                {
                    int32 port = arg_to_int(optarg, 1, "b");
                    RobotConfig::Instance()->SetPort(port);
                }
                break;
            case 'n':   // name名字
                {
                    std::string name = optarg;
                    RobotConfig::Instance()->SetPrefixName(name);
                }
                break;
            case 'u':   // 数量
                {
                    int32 num = arg_to_int(optarg, 1, "u");
                    RobotConfig::Instance()->SetNumber(num);
                }
                break;            
            case 'f':   // 用例文件
                {
                    std::string filename = optarg;
                    RobotConfig::Instance()->SetCaseFileName(filename);
                }
                break;
            case 'h':
            default:
                Help();
                return false;
                break;
                
        }
    }

    return true;
}

void showInfo()
{
    // 统计进入游戏的机器人数量
    CnAssert(robot_list.size() == RobotConfig::Instance()->GetNumber());

    std::map<uint32, uint32> StateMapNumber;
    uint32 liveRobotCount = 0;
    for(std::list<Robot*>::iterator iter = robot_list.begin(); iter != robot_list.end(); iter++)
    {
        Robot* pRobot = *iter;
        if(pRobot->GetState() == Robot::kStateInGame)
            liveRobotCount++;
        else
        {
            if(StateMapNumber.find(pRobot->GetState()) == StateMapNumber.end())
                StateMapNumber[pRobot->GetState()] = 1;
            else
                StateMapNumber[pRobot->GetState()] = StateMapNumber[pRobot->GetState()]+1;
        }
    }

    // 显示
    stringstream ss;
    ss<<"live robot num "<<liveRobotCount;
     
    for(std::map<uint32, uint32>::iterator sIter = StateMapNumber.begin(); sIter != StateMapNumber.end(); sIter++)
    {
        ss<<" "<<sIter->first<<":"<<sIter->second;
    }
    std::string s = ss.str();

    CnInfo("%s\n", s.c_str());
}

void ShowRunning()
{
    static uint32 showCount = 0;
    showCount++;

    uint32 p = showCount % 3;
    if(p==0)
        printf(".  \b\b\b");
    else if(p==1)
        printf(".. \b\b\b");
    else
        printf("...\b\b\b");
}

int main(int argc, char* argv[])
{
    // 创建配置
    RobotConfig::CreateInstance();
    // 解析命令行
    if(!ParseCmd(argc, argv))
        return 0;

    // 机器人前缀名
    std::string prefix_name = RobotConfig::Instance()->GetPrefixName();

    // 初始化日志
    std::string logFileName = prefix_name;
    Crown::ColorLog::CreateInstance();
    Crown::ColorLog::Instance()->Init("log", logFileName.c_str());
    Crown::ColorLog::Instance()->SetLogTime(true);

    SignalCatcher(SignalHandler);

    // 设置随机数种子
    Crown::SDSetSeed(Crown::SDGetCurrentProcessId() + (uint32)time(nullptr));

    // 创建网络
    Net_Manager::CreateInstance();

    // 初始化脚本引擎
    CScriptEngine::CreateInstance();
    CScriptEngine::Instance()->Init();

    uint32 tickcount = 0;
    uint32 lastcount = 0;

    // 服务器IP和端口号
    RobotConfig::Instance()->LoadConfig();

    // 加载聊天文本
    ChatConfig::CreateInstance();
    if(!ChatConfig::Instance()->LoadConfig())
    {
        return 0;
    }

    std::string svr_ip = RobotConfig::Instance()->GetSvrIP().c_str();
    int32 svr_port = RobotConfig::Instance()->GetSvrPort();
    bool RandPort = (svr_port == 0);

    int32 robot_num = RobotConfig::Instance()->GetNumber();
    for(int32 i=0; i<robot_num; ++i)
    {
        Robot* pRobot = CnNew Robot();

        int32 robot_num = RobotConfig::Instance()->GetNumber();
        if(robot_num > 1)
            sprintf(g_Account, "%s%04d", prefix_name.c_str(), i);
        else
            sprintf(g_Account, "%s", prefix_name.c_str());

        pRobot->SetAccount(g_Account);
        pRobot->SetSvrIP(svr_ip);

        if(RandPort)
        {
            uint32 r = Crown::SDRandom(2);
            if(r < 1)
                svr_port = RobotConfig::Instance()->m_PORT1;
            else
                svr_port = RobotConfig::Instance()->m_PORT2;
#ifdef DEBUG
            svr_port = RobotConfig::Instance()->m_PORT1;
#endif
        }
        pRobot->SetSvrPort(svr_port);

        robot_list.push_back(pRobot);
    }

    g_GameTime = 0;
    uint32 StartGameTime = Crown::SDGetTickCount();
    uint32 ShowInfoTime = 0;
    uint32 frameCount = 0;
    // 主循环
    while(mainLoop)
    {
        lastcount = tickcount;
        tickcount = Crown::SDGetTickCount();     //取得启动时间
        g_GameTime = tickcount - StartGameTime; // 游戏时间

        uint32 deltacount = tickcount - lastcount;

        Crown::ColorLog::Instance()->Update();

        TransferUnpdate();

        bool hasOneRobot = false;
        for(std::list<Robot*>::iterator robotIt = robot_list.begin(); robotIt != robot_list.end(); ++robotIt)
        {
            Robot* pRobot = *robotIt;
            (*robotIt)->Update(tickcount, deltacount);
            if(!pRobot->IsClose())
                hasOneRobot = true;
        }

        if(!hasOneRobot)
            mainLoop = false;

        ShowInfoTime += deltacount;
        if(ShowInfoTime > 5000)
        {
            showInfo();
            ShowInfoTime = 0;
        }

        ShowRunning();

        // sleep
        uint32 tickcount2 = Crown::SDGetTickCount();

        int32 sleepTime = updateTime - (tickcount2 - tickcount);
        if(sleepTime < 0)
            sleepTime = 0;

        Crown::SDSleep(sleepTime);
    }

    CnDbg("main loop end\n");

    // 销毁机器人
    for(std::list<Robot*>::iterator iter = robot_list.begin(); iter != robot_list.end(); iter++)
    {
        CnDelete *iter;
    }
    robot_list.clear();

    // 结束脚本引擎
    CScriptEngine::Instance()->Uninit();
    CScriptEngine::DestroyInstance();

    // 销毁网络
    Net_Manager::DestroyInstance();

    // 销毁配置
    RobotConfig::DestroyInstance();

    // 销毁聊天
    ChatConfig::DestroyInstance();

    // 销毁日志
    Crown::ColorLog::DestroyInstance();

    return 0;
}

