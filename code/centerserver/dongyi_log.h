#ifndef DOINGAMELOG_H
#define DOINGAMELOG_H

enum
{
    LOG_FILE_NAME_LEN = 256,
    LOG_MESSAGE_LEN = 1024,
    LOG_RECIRD_ONLINE_TICK = 600000,
};

typedef struct LogFile
{
    LogFile()
    {
        Clear();
    }
    size_t file_len;

    void Clear()
    {
        m_type          = 0;
        m_log_time      = 0;
        m_log_file      = NULL;
        m_log_len       = 0;
        m_log_name      = "";
        m_record_id     = 0;
        memset(m_log_path, 0, sizeof(m_log_path));
        file_len = 0;
        m_is_open       = true;
    }

    uint32          m_type;         // 文件标识的类型是哪个模块的
    uint32          m_log_time;
    FILE*           m_log_file;
    int32           m_log_len;
    std::string     m_log_name;
    char            m_log_path[LOG_FILE_NAME_LEN];
    uint32          m_record_id;    // 文件记录ID
    bool            m_is_open;      // 是否将该日志条目写入文件
}LogFile;

#define DgLog(idflag, playerID, format, ...)     DoingGameLog::Instance()->WriteCenterLog(idflag, playerID, format, ##__VA_ARGS__);

class DoingGameLog
{
public:
    DoingGameLog();
    ~DoingGameLog();

    DECLARE_SINGLETON (DoingGameLog)

    bool Init();
    void Uninit();

    uint32 GetLogDate();
    std::string GenLogDir();

    void WriteCenterLog(uint32 id, uint64 player_id, const char * format, ...);
    void WriteCenterLog(uint32 id, uint64 player_id, std::string logTxt);

    void Update();

private:
    static bool IsFolderExist(const char* path);
    static bool CreateFolder(const char* path);

    LogFile* CreateLog(uint32 type);

    void WriteLog(uint32 type, uint64 player_id, const std::string& log);

    LogFile* CloseFile(uint32 type);

    LogFile* GetLogFile(uint32 type);

    void ResetFileRecordID(LogFile* logFile);      //检查文件是否存在

private:
    char m_dir_split;
    std::string m_log_dir;

    char m_ym[LOG_FILE_NAME_LEN];
    char m_date[LOG_FILE_NAME_LEN];

    typedef struct LogTypeNameMap
    {
        uint32      type;
        std::string name;
        bool        is_open;
    }LogTypeNameMap;

    static LogTypeNameMap m_log_type_name_map[];

    uint32 m_recode_time;
    uint32 m_pcu_time;
    std::vector<LogFile> m_file_list;
};


#endif // DOINGAMELOG_H
