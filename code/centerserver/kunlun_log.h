#ifndef KUNLUNLOG_H
#define KUNLUNLOG_H

#include <stdio.h>
#include <time.h>

#ifdef WIN
#include "windows.h"
#endif

class KunlunLog
{
public:
    KunlunLog();
    ~KunlunLog();

    DECLARE_SINGLETON(KunlunLog)

    bool Init();
    void Uninit();

    void Update(uint32 tick);

    uint32 GenLogDate();
    std::string GenLogDir();

    bool CreateLoginLog();
    bool CreateActiveLog();
    bool CreateOnlineLog();
    bool CreateBehaviorLog();

    void LogLogin(const std::string& user_id, uint64 char_id, const std::string& user_name, uint32 user_ip, uint32 login_time);
    void LogActive(const std::string& user_id, uint64 char_id, const std::string& user_name, uint32 user_ip);
    void LogOnline(uint32 charNums, uint32 accountNums);
    void LogBehavior(uint64 player_id, const std::string& log);

private:
    KunlunLog(const KunlunLog&);
    KunlunLog& operator=(const KunlunLog&);
    KunlunLog* operator&();
    const KunlunLog operator&() const;

    static bool IsFolderExist(const char* path);
    static bool CreateFolder(const char* path);

    enum
    {
        //GMT_8 = 28800,
        LOG_FILE_NAME_LEN = 256,
        LOG_MESSAGE_LEN = 1024,
        LOG_RECORD_ONLINE_TICK = 600000,
    };

    char m_dir_split;
    std::string m_logDir;

    uint32 m_update_time;

    char m_yyyymm[LOG_FILE_NAME_LEN];
    char m_yyyymmdd[LOG_FILE_NAME_LEN];

    uint32 m_login_logtime;
    uint32 m_active_logtime;
    uint32 m_online_logtime;
    uint32 m_behavior_logtime;

    char m_login_name[LOG_FILE_NAME_LEN];
    char m_active_name[LOG_FILE_NAME_LEN];
    char m_online_name[LOG_FILE_NAME_LEN];
    char m_behavior_name[LOG_FILE_NAME_LEN];

    FILE* m_login_file;
    FILE* m_active_file;
    FILE* m_online_file;
    FILE* m_behavior_file;
};

#endif