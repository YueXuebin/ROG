#include "centerserver_pch.h"
#include "dongyi_log.h"

#include <memory.h>
#include <stdarg.h>
#include <string.h>
#include "config_loader.h"
#include "player_manager.h"

#ifdef WIN32
#include <direct.h>
#include <io.h>
#else
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifdef WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#else
#define ACCESS access
#define MKDIR(a) mkdir((a), 0755)
#endif

IMPLEMENT_SINGLETON(DoingGameLog)

DoingGameLog::LogTypeNameMap DoingGameLog::m_log_type_name_map[] =
{
    {0, "none", false},
    {LOG_CLIENT_UDP, "client_udp", true},             // 客户端 UDP 消息
    {LOG_CLIENT_TCP, "client_tcp", true},             // 客户端 TCP 消息
    {LOG_LOGIN, "login", true},                       // 登陆
    {LOG_TRADE, "trade", true},                       // 交易
    {LOG_TASK, "task", true},                         // 任务
    {LOG_GAMEPOINT, "gamepoint", true},               // 游戏币
    {LOG_BINDMONEYPOINT, "bind_moneypoint", true},    // 绑定魔石
    {LOG_ACTIVITY, "activity", true},                 // 活动
    {LOG_DUNGEON, "dungeon", true},                   // 副本
    {LOG_PLAYER_STATE, "player_state", true},         // 角色状态
    {LOG_SOUL_POINT, "soulpoint", true},              // 精魂
    {LOG_ITEM, "item", true},                         // 物品
    {LOG_BEHAVIOR, "behavior", true},                 // 行为
    {LOG_PAY, "pay", true},                           // 充值
    {LOG_EXP, "exp", true},                           // 获得经验
    {LOG_SERVICE, "service", true},                   // 使用服务
    {LOG_CREATE_PLAYER, "create_player", true},       // 创建角色
    {LOG_PCU, "pcu", true},                           // PCU
    {LOG_DEBUG, "debug", true},                       // 调试
    {LOG_PlAY_SYSTEM, "play_system", true},           // 玩法系统
    {LOG_GUILD, "guild", true},                       // 公会
    {LOG_WEALTH, "wealth", true},                     // 其他(非货币和物品)财产日志
};

DoingGameLog::DoingGameLog()
{
    m_dir_split = ' ';
    m_log_dir = "";

    memset(m_ym, 0, sizeof(m_ym));
    memset(m_date, 0, sizeof(m_date));

    m_recode_time = 0;
    m_pcu_time = 0;
}

DoingGameLog::~DoingGameLog()
{
    Uninit();
}

bool DoingGameLog::Init()
{
    uint32 len = 0;

#ifdef WIN32
    m_dir_split = '\\';
    m_log_dir = ConfigLoader::Instance()->GetDongyiWinLogDir();
#else
    m_dir_split = '/';
    m_log_dir = ConfigLoader::Instance()->GetDongyiLinuxLogDir();
#endif

    len = sizeof(m_log_type_name_map)/sizeof(m_log_type_name_map[0]);

    // 初始化 Log 内容列表
    m_file_list.resize(len);
    for(size_t i = 0; i < len; ++i)
    {
        m_file_list[i].m_type = m_log_type_name_map[i].type;
        m_file_list[i].m_log_name = m_log_type_name_map[i].name;
        m_file_list[i].m_is_open = m_log_type_name_map[i].is_open;
    }

    // 创建文件
    for(size_t i = LOG_CLIENT_UDP; i < len; ++i)
    {
        CreateLog(m_log_type_name_map[i].type);   // 创建 log 文件
    }

    return true;
}

void DoingGameLog::Uninit()
{
    for(std::vector<LogFile>::iterator it = m_file_list.begin(); it != m_file_list.end(); it++)
    {
        if(NULL != it->m_log_file)
        {
            fclose(it->m_log_file);
            it->m_log_file = NULL;
        }
    }

    m_file_list.clear();
};

bool DoingGameLog::IsFolderExist(const char* path)
{
    if(0 != ACCESS(path,0)) 
        return false;
    return true;
}

bool DoingGameLog::CreateFolder(const char* path)
{
    if(0 != MKDIR(path))
        return false;
    return true;
}

uint32 DoingGameLog::GetLogDate()
{
    tm *local;
    time_t t;

    t = Crown::SDNow().GetTimeValue();
    local = localtime(&t);

    strftime(m_ym, LOG_FILE_NAME_LEN, "%Y%m", local);
    strftime(m_date, LOG_FILE_NAME_LEN, "%Y-%m-%d %H-%M-%S", local);

    return (uint32)t;
}

std::string DoingGameLog::GenLogDir()
{
    std::string logPath;
    std::string logDir;

    logPath = m_log_dir + m_dir_split + m_ym;
    std::vector<std::string> dirList = Crown::SDSplitStrings(logPath, m_dir_split);
    for(uint32 i=0; i<dirList.size(); ++i)
    {
        logDir = "";

        for(uint32 j=0; j<=i; ++j)
        {
            if(j == 0)
#ifdef WIN32
                logDir = dirList[j];
#else
                logDir = dirList[j];
#endif
            else
                logDir += m_dir_split + dirList[j];

            if(!IsFolderExist(logDir.c_str()))
            {
                if(!CreateFolder(logDir.c_str()))
                {
                    printf("Create folder \"%s\" failed.", logDir.c_str());
                    return "";
                }
            }
        }
    }

    return logPath;
}

LogFile* DoingGameLog::CreateLog(uint32 type)
{
    LogFile* logFile;
    std::string logDir = "";

    // 关闭文件
    logFile = CloseFile(type);
    if(logFile == NULL)
    {
        return NULL;
    }

    // 更新文件日期
    logFile->m_log_time = GetLogDate();
    logDir = GenLogDir();

    //游戏名-区号-功能-log版本号-创建时间
    SDSnprintf( logFile->m_log_path, LOG_FILE_NAME_LEN, 
                    "%s%c%s_%d_%s_%d[%s].log", 
                    logDir.c_str(),
                    m_dir_split,
                    ConfigLoader::Instance()->GetGameName().c_str(),
                    ConfigLoader::Instance()->GetRegionID(),
                    logFile->m_log_name.c_str(),
                    ConfigLoader::Instance()->GetDongyiLogVesion(),
                    m_date);

    // 打开文件
    if(0 == (logFile->m_log_file = fopen(logFile->m_log_path, "at+")))
    {
        printf("Create log file \"%s\" failed\n", logFile->m_log_path);
        return NULL;
    }

    ResetFileRecordID(logFile);

    return logFile;
}

void DoingGameLog::ResetFileRecordID(LogFile* logFile)
{
    std::string line = "";
    int32 fileLen = 0;
    int32 chNum = 0;
    char ch = 0;

    // 查找到文件末尾
    fseek(logFile->m_log_file, 0L, SEEK_END);
    fileLen = ftell(logFile->m_log_file);
    if(fileLen < 1)
    {
        return;
    }

    // 倒序查找文件
    for(int32 i=1; i<=fileLen; i++)
    {
        fseek(logFile->m_log_file, -i, SEEK_END);
        ch = getc(logFile->m_log_file);

        switch(ch)
        {
        case '\n':
            {   // 每次读到换行，重置读取的空字符串
                if(chNum == 0)
                    line = "";
            }
            break;
        case '\r': case '\t': case ' ':
            line = ch + line;
            break;
        default:
            ++chNum;
            line = ch + line;
            break;
        }

        // 读到换行且有内容，则退出
        if((ch =='\n') && (chNum > 0))
        {
            break;
        }
    }

    // 如果之前存在行，则读取最后一行的 record_id
    if(chNum > 0)
    {
        std::vector<std::string> values = Crown::SDSplitStrings(line, '\t');
        if(values.size() > 0)
        {
            logFile->m_record_id = Crown::SDAtou(values[0].c_str());
        }
    }

    // 定位到文件末
    fseek(logFile->m_log_file, 0, SEEK_END);
    logFile->m_log_len = ftell(logFile->m_log_file);
}

LogFile* DoingGameLog::CloseFile(uint32 type)
{
    if(type >= m_file_list.size())
    {
        return NULL;
    }

    if(m_file_list[type].m_log_file)
    {
        fclose(m_file_list[type].m_log_file);

        // 清空残留数据
        m_file_list[type].m_log_time = 0;
        m_file_list[type].m_log_file = NULL;
        m_file_list[type].m_log_len = 0;
        memset(m_file_list[type].m_log_path, 0, sizeof(m_file_list[type].m_log_path));
        m_file_list[type].m_record_id = 0;
    }

    return &m_file_list[type];
}

void DoingGameLog::Update()
{
    m_recode_time += g_ServerUpdateTime;
    m_pcu_time += g_ServerUpdateTime;

    //n 秒跟新一次
    if(m_recode_time >= ConfigLoader::Instance()->GetDongyiLogUpdateTime())
    {
        for(std::vector<LogFile>::iterator iterFile = m_file_list.begin(); iterFile != m_file_list.end(); iterFile++)
        {
            if(NULL == iterFile->m_log_file )
            {
                continue;
            }

            // 判断文件长度
            if(iterFile->m_log_len >= ConfigLoader::Instance()->GetDongyiLogMaxLen())
                CreateLog(iterFile->m_type);
        }

        m_recode_time = 0;
    }

    // pcu 时间
    if(m_pcu_time >= ConfigLoader::Instance()->GetDongyiLogPcuTime())
    {
        uint32 playerNum = PlayerManager::Instance()->GetPlayerNum();
        std::string log = WriterToLogPcu(playerNum);
        DoingGameLog::Instance()->WriteCenterLog(LOG_PCU, 0, log);
        m_pcu_time = 0;
    }
}

LogFile* DoingGameLog::GetLogFile(uint32 type)
{
    if(type < m_file_list.size())
    {
        return &m_file_list[type];
    }
    return NULL;
}

void DoingGameLog::WriteLog(uint32 type, uint64 player_id, const std::string& log)
{
    char msg[LOG_MESSAGE_LEN]={0};
    uint32 len = 0;
    LogFile* logFile = NULL;
    uint32 timeStamp = 0;

    timeStamp = (uint32)Crown::SDNow().GetTimeValue();       // 时间戳

    logFile = GetLogFile(type);
    if(!logFile)
    {
        CnAssert(false);        // 文件不存在
        return;
    }

    // 如果日期变更则需要关闭之前的文件，创建新文件
    if(!IsSameDay(logFile->m_log_time))
    {
        logFile = CreateLog(logFile->m_type);
    }

    // 检查文件指针的有效性
    if (!logFile || !logFile->m_log_file)
    {
        CnAssert(false);
        return;
    }

    // 将记录数加 1
    ++logFile->m_record_id;

    // 根据类型决定LOG格式
    switch(type)
    {
    case LOG_CLIENT_UDP:
    case LOG_PCU:
    case LOG_DEBUG:
        SDSnprintf(msg, LOG_MESSAGE_LEN-1, "%d\t%d\t%s\n", logFile->m_record_id, timeStamp, log.c_str());
        break;
    case LOG_CLIENT_TCP:
    case LOG_LOGIN:
    case LOG_TRADE:
    case LOG_TASK:
    case LOG_GAMEPOINT:
    case LOG_BINDMONEYPOINT:
    case LOG_ACTIVITY:
    case LOG_DUNGEON:
    case LOG_PLAYER_STATE:
    case LOG_SOUL_POINT:
    case LOG_ITEM:
    case LOG_BEHAVIOR:
    case LOG_PAY:
    case LOG_EXP:
    case LOG_SERVICE:
    case LOG_CREATE_PLAYER:
    case LOG_PlAY_SYSTEM:
    case LOG_GUILD:
    case LOG_WEALTH:
        // 写文件
        SDSnprintf(msg, LOG_MESSAGE_LEN-1, "%d\t%d\t%llu\t%s\n", logFile->m_record_id, timeStamp, player_id, log.c_str());
        break;
    default:
        CnWarn("Unknown log type:%u\n", type);
        return;
        break;
    }

    if(logFile->m_is_open)
    {
        len = (unsigned int)strlen(msg);
        logFile->m_log_len += fwrite(msg, sizeof(char), len, logFile->m_log_file);

        // 文件刷新
        fflush(logFile->m_log_file);
    }
}

void DoingGameLog::WriteCenterLog(uint32 id, uint64 player_id ,const char * format, ...)
{
    char str[LOG_STR_MAX_LEN];
    memset(str, 0, LOG_STR_MAX_LEN);
    va_list arglist;
    va_start(arglist, format);
    vsnprintf(str, LOG_STR_MAX_LEN, format, arglist);
    va_end(arglist);
    WriteLog(id, player_id, str);
}

void DoingGameLog::WriteCenterLog(uint32 id, uint64 player_id, std::string logTxt)
{
    WriteLog(id, player_id, logTxt);
}
