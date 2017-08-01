#include "centerserver_pch.h"
#include "kunlun_log.h"
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

IMPLEMENT_SINGLETON(KunlunLog)

KunlunLog::KunlunLog()
{
    m_dir_split = ' ';
    m_logDir = "";

    m_update_time = 0;

    memset(m_yyyymm, 0, sizeof(m_yyyymm));
    memset(m_yyyymmdd, 0, sizeof(m_yyyymmdd));
    memset(m_login_name, 0, sizeof(m_login_name));
    memset(m_active_name, 0, sizeof(m_active_name));
    memset(m_online_name, 0, sizeof(m_online_name));
    memset(m_behavior_name, 0, sizeof(m_behavior_name));

    m_login_logtime = 0;
    m_active_logtime = 0;
    m_online_logtime = 0;
    m_behavior_logtime = 0;

    m_login_file = NULL;
    m_active_file = NULL;
    m_online_file = NULL;
    m_behavior_file = NULL;
}

KunlunLog::~KunlunLog()
{
    Uninit();
}

bool KunlunLog::Init()
{
#ifdef WIN32
    m_dir_split = '\\';
    m_logDir = ConfigLoader::Instance()->GetKunlunWinLogDir();
#else
    m_dir_split = '/';
    m_logDir = ConfigLoader::Instance()->GetKunlunLinuxLogDir();
#endif

    CreateLoginLog();

    CreateActiveLog();

    CreateOnlineLog();

    CreateBehaviorLog();

    return true;
}

void KunlunLog::Uninit()
{
    if(NULL != m_login_file)
    {
        fclose(m_login_file);
        m_login_file = NULL;
    }

    if(NULL != m_active_file)
    {
        fclose(m_active_file);
        m_active_file = NULL;
    }

    if(NULL != m_online_file)
    {
        fclose(m_online_file);
        m_online_file = NULL;
    }

    if(NULL != m_behavior_file)
    {
        fclose(m_behavior_file);
        m_behavior_file = NULL;
    }
}

void KunlunLog::Update(uint32 tick)
{
    uint32 num = 0;
    if(m_update_time < tick)
    {
        num = PlayerManager::Instance()->GetAccountNum();
        LogOnline(num, num);
        m_update_time = tick + LOG_RECORD_ONLINE_TICK;     // 10 分钟
    }
}

uint32 KunlunLog::GenLogDate()
{
    tm *local;
    time_t t;

    t = Crown::SDNow().GetTimeValue();
    local = localtime(&t);

    strftime(m_yyyymm, LOG_FILE_NAME_LEN, "%Y%m", local);
    strftime(m_yyyymmdd, LOG_FILE_NAME_LEN, "%Y%m%d", local);

    return (uint32)t;
}

std::string KunlunLog::GenLogDir()
{
    std::string logPath;
    std::string logDir;

    logPath = m_logDir + m_dir_split + m_yyyymm;

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
                logDir = m_dir_split + dirList[j];
#endif
            else
                logDir += m_dir_split + dirList[j];

            if(!IsFolderExist(logDir.c_str()))
            {
                if(!CreateFolder(logDir.c_str()))
                {
                    printf("Create folder \"%s\" failed.", logDir.c_str());
                    return false;
                }
            }
        }
    }

    return logPath;
}

bool KunlunLog::CreateLoginLog()
{
    std::string logFile = "";
    std::string logPath = "";

    m_login_logtime = GenLogDate();

    logPath = GenLogDir();

    if(m_login_file)
    {
        fclose(m_login_file);
        m_login_file = nullptr;
    }

    SDSnprintf(m_login_name, LOG_FILE_NAME_LEN, "login_%u_%u_%s.log", 
        ConfigLoader::Instance()->GetKunlunPid(), ConfigLoader::Instance()->GetKunlunRid(), 
        m_yyyymmdd);

    // 角色登陆日志
    logFile = logPath + m_dir_split + m_login_name;
    if(0 == (m_login_file = fopen(logFile.c_str(),"at")))
    {
        printf("Create log file \"%s\" failed.", logFile.c_str());
        return false;
    }

    return true;
}

bool KunlunLog::CreateActiveLog()
{
    std::string logFile = "";
    std::string logPath = "";

    m_active_logtime = GenLogDate();

    logPath = GenLogDir();

    if(m_active_file)
    {
        fclose(m_active_file);
        m_active_file = nullptr;
    }

    SDSnprintf(m_active_name, LOG_FILE_NAME_LEN, "active_%u_%u_%s.log", 
        ConfigLoader::Instance()->GetKunlunPid(), ConfigLoader::Instance()->GetKunlunRid(), 
        m_yyyymmdd);

    // 角色创建日志
    logFile = logPath + m_dir_split + m_active_name;
    if(0 == (m_active_file = fopen(logFile.c_str(),"at")))
    {
        printf("Create log file \"%s\" failed.", logFile.c_str());
        return false;
    }

    return true;
}

bool KunlunLog::CreateOnlineLog()
{
    std::string logFile = "";
    std::string logPath = "";

    m_online_logtime = GenLogDate();

    logPath = GenLogDir();

    if(m_online_file)
    {
        fclose(m_online_file);
        m_online_file = nullptr;
    }

    SDSnprintf(m_online_name,  LOG_FILE_NAME_LEN, "online_%u_%u_%s.log", 
        ConfigLoader::Instance()->GetKunlunPid(), ConfigLoader::Instance()->GetKunlunRid(), 
        m_yyyymmdd);

    // 角色在线日志
    logFile = logPath + m_dir_split + m_online_name;
    if(0 == (m_online_file = fopen(logFile.c_str(),"at")))
    {
        printf("Create log file \"%s\" failed.", logFile.c_str());
        return false;
    }

    return true;
}

bool KunlunLog::CreateBehaviorLog()
{
    std::string logFile = "";
    std::string logPath = "";

    m_behavior_logtime = GenLogDate();

    logPath = GenLogDir();

    if(m_behavior_file)
    {
        fclose(m_behavior_file);
        m_behavior_file = nullptr;
    }

    SDSnprintf(m_behavior_name, LOG_FILE_NAME_LEN, "behavior_%u_%u_%s.log", 
        ConfigLoader::Instance()->GetKunlunPid(), ConfigLoader::Instance()->GetKunlunRid(), 
        m_yyyymmdd);

    // 角色消费日志
    logFile = logPath + m_dir_split + m_behavior_name;
    if(0 == (m_behavior_file = fopen(logFile.c_str(),"at")))
    {
        printf("Create log file \"%s\" failed.", logFile.c_str());
        return false;
    }

    return true;

}

void KunlunLog::LogLogin(const std::string& user_id, uint64 char_id, const std::string& user_name, uint32 user_ip, uint32 login_time)
{
    char logmsg[LOG_MESSAGE_LEN];
    char ip_str[30] = {0};
    uint32 len = 0;

    if(!IsSameDay(m_login_logtime))
        CreateLoginLog();

    if(!m_login_file) return;

    //SDSnprintf(ip_str, 30, "%u.%u.%u.%u", user_ip>>24, ((user_ip>>16)&0xFF), ((user_ip>>8)&0xFF), (user_ip&0xFF));
    SDSnprintf(ip_str, 30, "%u.%u.%u.%u", (user_ip&0xFF), ((user_ip>>8)&0xFF), ((user_ip>>16)&0xFF), user_ip>>24);

    // user_id \t char_id \t user_name \t user_ip \t login_time \t pid \t rid
    SDSnprintf(logmsg, LOG_MESSAGE_LEN, "%s\t%llu\t%s\t%s\t%u\t%u\t%u\n",
        user_id.c_str(), char_id, user_name.c_str(), ip_str, login_time, 
        ConfigLoader::Instance()->GetKunlunPid(), ConfigLoader::Instance()->GetKunlunRid());

    len = (unsigned int)strlen(logmsg);
    fwrite(logmsg, sizeof(char), len, m_login_file);
    fflush(m_login_file);
}

void KunlunLog::LogActive(const std::string& user_id, uint64 char_id, const std::string& user_name, uint32 user_ip)
{
    char logmsg[LOG_MESSAGE_LEN];
    char ip_str[30] = {0};
    uint32 len = 0;
    uint32 now = 0;

    if(!IsSameDay(m_active_logtime))
        CreateActiveLog();

    if(!m_active_file) return;

    now = (uint32)Crown::SDNow().GetTimeValue();

    //SDSnprintf(ip_str, 30, "%u.%u.%u.%u", user_ip>>24, ((user_ip>>16)&0xFF), ((user_ip>>8)&0xFF), (user_ip&0xFF));
    SDSnprintf(ip_str, 30, "%u.%u.%u.%u", (user_ip&0xFF), ((user_ip>>8)&0xFF), ((user_ip>>16)&0xFF), user_ip>>24);

    // user_id \t char_id \t user_ip \t active_time \t user_name \t pid \t rid
    SDSnprintf(logmsg, LOG_MESSAGE_LEN, "%s\t%llu\t%s\t%u\t%s\t%u\t%u\n",
        user_id.c_str(), char_id, ip_str, now, user_name.c_str(), 
        ConfigLoader::Instance()->GetKunlunPid(), ConfigLoader::Instance()->GetKunlunRid());

    len = (unsigned int)strlen(logmsg);
    fwrite(logmsg, sizeof(char), len, m_active_file);
    fflush(m_active_file);
}

void KunlunLog::LogOnline(uint32 charNums, uint32 accountNums)
{
    char logmsg[LOG_MESSAGE_LEN];
    uint32 len = 0;
    uint32 now = 0;

    if(!IsSameDay(m_online_logtime))
        CreateOnlineLog();

    if(!m_online_file) return;

    now = (uint32)Crown::SDNow().GetTimeValue();
    // outtime \t charNums \t accountNums \t pid \t rid
    SDSnprintf(logmsg, LOG_MESSAGE_LEN, "%u\t%u\t%u\t%u\t%u\n", 
        now, charNums, accountNums, 
        ConfigLoader::Instance()->GetKunlunPid(), ConfigLoader::Instance()->GetKunlunRid());

    len = (unsigned int)strlen(logmsg);
    fwrite(logmsg, sizeof(char), len, m_online_file);
    fflush(m_online_file);
}

void KunlunLog::LogBehavior(uint64 player_id, const std::string& log)
{
    char logmsg[LOG_MESSAGE_LEN];
    char idstr[30];
    Crown::SDUtoa64<30>(idstr, player_id);

    if(!IsSameDay(m_behavior_logtime))
        CreateBehaviorLog();

    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
    SDSnprintf(logmsg, LOG_MESSAGE_LEN, "%d\t%s\t%s\n", nowTime, idstr, log.c_str());
    uint32 len = (unsigned int)strlen(logmsg);
    fwrite(logmsg, sizeof(char), len, m_behavior_file);
    fflush(m_behavior_file);
}

bool KunlunLog::IsFolderExist(const char* path)
{
    if(0 != ACCESS(path,0)) return false;
    return true;
}

bool KunlunLog::CreateFolder(const char* path)
{
    if(0 != MKDIR(path)) return false;
    return true;
}
