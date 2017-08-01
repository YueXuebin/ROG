#include "SDuPCH.h"
#include "colorlog.h"
#include <memory.h>
#include <stdarg.h>
#include <string.h>
#include "sdtime.h"

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

namespace Crown
{


#ifdef WIN
#define COLORLOG(LABEL) \
    memset(logMsg, 0, LOG_MESSAGE_LEN); \
    \
    if(mLogTime) \
    { \
        _snprintf(logMsg, LOG_MESSAGE_LEN, "[%s][%04d-%02d-%02d %02d:%02d:%02d %03d]: ", LABEL, \
            local->tm_year+1900, local->tm_mon+1, local->tm_mday, \
            local->tm_hour, local->tm_min, local->tm_sec, \
            tickCount); \
        nLen = (unsigned int)strlen(logMsg); \
    } \
    \
    vsnprintf(logMsg+nLen, LOG_MESSAGE_LEN-nLen, format, arglist); \
    std::string utf8Str = logMsg; \
    std::string asciiStr = Crown::Utf8ToAscii(utf8Str); \
    _snprintf(logMsg, LOG_MESSAGE_LEN, "%s", asciiStr.c_str()); \
    \
    if(mFile) \
    { \
        nLen = (unsigned int)strlen(logMsg); \
        fwrite(logMsg, sizeof(char), nLen, mFile); \
        fflush(mFile); \
    }
#else
#define COLORLOG(LABEL) \
    memset(logMsg, 0, LOG_MESSAGE_LEN); \
    \
    if(mLogTime) \
    { \
        snprintf(logMsg, LOG_MESSAGE_LEN, "[%s][%04d-%02d-%02d %02d:%02d:%02d %03d]: ", LABEL, \
            local->tm_year+1900, local->tm_mon+1, local->tm_mday, \
            local->tm_hour, local->tm_min, local->tm_sec, \
            tickCount); \
        nLen = (unsigned int)strlen(logMsg); \
    } \
    \
    vsnprintf(logMsg+nLen, LOG_MESSAGE_LEN-nLen, format, arglist); \
    \
    if(mFile) \
    { \
        nLen = (unsigned int)strlen(logMsg); \
        fwrite(logMsg, sizeof(char), nLen, mFile); \
        fflush(mFile); \
    }
#endif


IMPLEMENT_SINGLETON(ColorLog)


ColorLog::ColorLog()
{
    mLogTime = false;
    memset(mFolderName, 0, sizeof(mFolderName));
    memset(mFileName, 0, sizeof(mFileName));
    mFile = NULL;
    
    m_LocalTime = NULL;
    m_MainThreadID = 0;
}

ColorLog::~ColorLog()
{
    Uninit();
}

bool ColorLog::Init(const char* folder, const char* file)
{
    time_t t;
    t = time(NULL);
    m_LocalTime = localtime(&t);
    m_TickCountStart = SDGetTickCount();
    m_TickCountTime = 0;

    m_MainThreadID = Crown::SDGetThreadId();

#ifdef WIN32
    m_hConsole= GetStdHandle(STD_OUTPUT_HANDLE);
    if(!m_hConsole){ return false; }
#endif

    if(folder == NULL || file == NULL)
        return true;

    if(!IsFolderExist(folder))
    {
        if(!CreateFolder(folder))
        {
            printf("Create folder \"%s\" failed.", folder);
            return false;
        }
    }
    tm* local = m_LocalTime;
    sprintf(mFileName, "%s/%s[%04d-%02d-%02d %02d-%02d-%02d].log",
        folder, file,
        local->tm_year+1900,local->tm_mon+1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);

    if(0 == (mFile=fopen(mFileName,"at")))
    {
        Fatal("Create log file \"%s\" failed.", mFileName);
        return false;
    }

    return true;
}

void ColorLog::Uninit()
{
#ifdef WIN32
    if(m_hConsole){ FreeConsole(); m_hConsole=0; }
#endif

    if(NULL != mFile)
    {
        fclose(mFile);
        mFile = NULL;
    }

    memset(mFolderName, 0, LOG_FILE_NAME_LEN);
    memset(mFileName, 0, LOG_FILE_NAME_LEN);
}

void ColorLog::Update()
{
    time_t t = time(NULL);
    m_LocalTime = localtime(&t);      // 不可在log中调用此函数，会引起malloc死锁
    m_TickCountTime = (SDGetTickCount() - m_TickCountStart)%1000;
}

void ColorLog::SetLogTime(bool val)
{
    mLogTime = true;
}

void ColorLog::Debug(const char * format, ...)
{
    //if(m_MainThreadID != Crown::SDGetThreadId())
    //{   // 非主线程log
    //    return;
    //}

    tm *local = m_LocalTime;
    uint32 tickCount = m_TickCountTime;

    char logMsg[LOG_MESSAGE_LEN];
    memset(logMsg, 0, LOG_MESSAGE_LEN);

    unsigned int nLen = 0;
    va_list arglist;
    va_start(arglist, format);

    COLORLOG("DBG");

#ifdef WIN
    SetColor(FC_DARK_GREEN, BC_BLACK);
    printf(logMsg);
    SetColor(FC_GRAY, BC_BLACK);
#else
    printf(logMsg);
#endif

    va_end(arglist);

#ifdef WIN
    _RPT1(_CRT_WARN, "%s", logMsg);
#endif
}

void ColorLog::Info(const char * format, ...)
{
    //if(m_MainThreadID != Crown::SDGetThreadId())
    //{   // 非主线程log
    //    return;
    //}

    tm *local = m_LocalTime;
    uint32 tickCount = m_TickCountTime;

    char logMsg[LOG_MESSAGE_LEN];
    memset(logMsg, 0, LOG_MESSAGE_LEN);

    unsigned int nLen = 0;
    va_list arglist;
    va_start(arglist, format);

    COLORLOG("INFO");

#ifdef WIN
    printf(logMsg);
#else
    printf(logMsg);
#endif

    va_end(arglist);

#ifdef WIN
    _RPT1(_CRT_WARN, "%s", logMsg);
#endif
}

void ColorLog::Warn(const char * format, ...)
{
    //if(m_MainThreadID != Crown::SDGetThreadId())
    //{   // 非主线程log
    //    return;
    //}

    tm *local = m_LocalTime;
    uint32 tickCount = m_TickCountTime;

    char logMsg[LOG_MESSAGE_LEN];
    memset(logMsg, 0, LOG_MESSAGE_LEN);

    unsigned int nLen = 0;
    va_list arglist;
    va_start(arglist, format);

    COLORLOG("WARN");

#ifdef WIN
    SetColor(FC_YELLOW, BC_BLACK);
    printf(logMsg);
    SetColor(FC_GRAY, BC_BLACK);
#else
    printf(logMsg);
#endif

    va_end(arglist);

#ifdef WIN
    _RPT1(_CRT_WARN, "%s", logMsg);
#endif
}

void ColorLog::Error(const char * format, ...)
{
    //if(m_MainThreadID != Crown::SDGetThreadId())
    //{   // 非主线程log
    //    return;
    //}

    tm *local = m_LocalTime;
    uint32 tickCount = m_TickCountTime;

    char logMsg[LOG_MESSAGE_LEN];
    memset(logMsg, 0, LOG_MESSAGE_LEN);

    unsigned int nLen = 0;
    va_list arglist;
    va_start(arglist, format);

    COLORLOG("ERROR");

#ifdef WIN
    SetColor(FC_MAGENTA, FC_BLACK);
    printf(logMsg);
    SetColor(FC_GRAY, BC_BLACK);
#else
    printf(logMsg);
#endif

    va_end(arglist);

#ifdef WIN
    _RPT1(_CRT_WARN, "%s", logMsg);
#endif
}

void ColorLog::Fatal(const char * format, ...)
{
    //if(m_MainThreadID != Crown::SDGetThreadId())
    //{   // 非主线程log
    //    return;
    //}

    tm *local = m_LocalTime;
    uint32 tickCount = m_TickCountTime;

    char logMsg[LOG_MESSAGE_LEN];
    memset(logMsg, 0, LOG_MESSAGE_LEN);

    unsigned int nLen = 0;
    va_list arglist;
    va_start(arglist, format);

    COLORLOG("FATAL");

#ifdef WIN
    SetColor(FC_RED, FC_BLACK);
    printf(logMsg);
    SetColor(FC_GRAY, BC_BLACK);
#else
    printf(logMsg);
#endif

    va_end(arglist);

#ifdef WIN
    _RPT1(_CRT_WARN, "%s", logMsg);
#endif
}

void ColorLog::SetColor(unsigned int fc, unsigned int bc)
{
#ifdef WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if(!hConsole) { return; }
    WORD color = fc | bc;
    SetConsoleTextAttribute(hConsole, color);
#else
    printf("\e[%d;%dm", fc, bc);
#endif
}

void ColorLog::ColorPrint(const char* label, unsigned int fc, unsigned int bc, const char* format, ...)
{
    tm *local = m_LocalTime;

    unsigned int nLen = 0;
    va_list arglist;
#ifdef WIN32
    if(!m_hConsole) { return; }
#endif

    va_start(arglist, format);

    char logMsg[LOG_MESSAGE_LEN];
    memset(logMsg, 0, LOG_MESSAGE_LEN);

    if(mLogTime)
    {
        sprintf(logMsg, "[%s][%04d-%02d-%02d %02d:%02d:%02d]: ", label?label:"",
            local->tm_year+1900,local->tm_mon+1, local->tm_mday,
            local->tm_hour,local->tm_min, local->tm_sec);
        nLen = (unsigned int)strlen(logMsg);
    }

    vsnprintf(logMsg+nLen, LOG_MESSAGE_LEN, format, arglist);

    if(mFile)
    {
        nLen = (unsigned int)strlen(logMsg);
        fwrite(logMsg, sizeof(char), nLen, mFile);
        fflush(mFile);
    }

#ifdef WIN32
    WORD color = fc | bc;
    GetConsoleScreenBufferInfo(m_hConsole, (PCONSOLE_SCREEN_BUFFER_INFO)&m_ScreenInfo);
    SetConsoleTextAttribute(m_hConsole, color);
    printf(logMsg);
    SetConsoleTextAttribute(m_hConsole, m_ScreenInfo.wAttributes);
#else
    printf("\e[%d;%dm", fc, bc);
    printf("%s", logMsg);
    printf("\e[%d;%dm", FC_WHITE, BC_BLACK); // 假设 Linux 默认为 前景白，背景黑
#endif

    va_end(arglist);
}

void ColorLog::Sample()
{
#ifdef WIN32
    if(!m_hConsole) { return; }
#endif

    Debug("##########\n");
    Info("##########\n");
    Warn("##########\n");
    Error("##########\n");
    Fatal("##########\n");

    ColorPrint("", FC_BLACK, BC_WHITE, "%-20s ##########\n", "FC_BLACK");
    ColorPrint("", FC_DARK_BLUE, BC_DARK_GRAY, "%-20s ##########\n", "FC_DARK_BLUE");
    ColorPrint("", FC_DARK_GREEN, BC_BLACK, "%-20s ##########\n", "FC_DARK_GREEN");
    ColorPrint("", FC_DARK_CRAY, BC_BLACK, "%-20s ##########\n", "FC_DARK_CRAY");

    ColorPrint("", FC_DARK_RED, BC_BLACK, "%-20s ##########\n", "FC_DARK_RED");
    ColorPrint("", FC_DARK_MAGENTA, BC_BLACK, "%-20s ##########\n", "FC_DARK_MAGENTA");
    ColorPrint("", FC_DARK_YELLOW, BC_BLACK, "%-20s ##########\n", "FC_DARK_YELLOW");
    ColorPrint("", FC_GRAY, BC_BLACK, "%-20s ##########\n", "FC_GRAY");

    ColorPrint("", FC_DARK_GRAY, BC_BLACK, "%-20s ##########\n", "FC_DARK_GRAY");
    ColorPrint("", FC_BLUE, BC_BLACK, "%-20s ##########\n", "FC_BLUE");
    ColorPrint("", FC_GREEN, BC_BLACK, "%-20s ##########\n", "FC_GREEN");
    ColorPrint("", FC_CRAY, BC_BLACK, "%-20s ##########\n", "FC_CRAY");

    ColorPrint("", FC_RED, BC_BLACK, "%-20s ##########\n", "FC_RED");
    ColorPrint("", FC_MAGENTA, BC_BLACK, "%-20s ##########\n", "FC_MAGENTA");
    ColorPrint("", FC_YELLOW, BC_BLACK, "%-20s ##########\n", "FC_YELLOW");
    ColorPrint("", FC_WHITE, BC_BLACK, "%-20s ##########\n", "FC_WHITE");

    // ================================================================================

    ColorPrint("", FC_WHITE, BC_BLACK, "%-20s ##########\n", "BC_BLACK");
    ColorPrint("", FC_WHITE, BC_DARK_BLUE, "%-20s ##########\n", "BC_DARK_BLUE");
    ColorPrint("", FC_WHITE, BC_DARK_GREEN, "%-20s ##########\n", "BC_DARK_GREEN");
    ColorPrint("", FC_WHITE, BC_DARK_CRAY, "%-20s ##########\n", "BC_DARK_CRAY");

    ColorPrint("", FC_WHITE, BC_DARK_RED, "%-20s ##########\n", "BC_DARK_RED");
    ColorPrint("", FC_WHITE, BC_DARK_MAGENTA, "%-20s ##########\n", "BC_DARK_MAGENTA");
    ColorPrint("", FC_WHITE, BC_DARK_YELLOW, "%-20s ##########\n", "BC_DARK_YELLOW");
    ColorPrint("", FC_WHITE, BC_GRAY, "%-20s ##########\n", "BC_GRAY");

    ColorPrint("", FC_WHITE, BC_DARK_GRAY, "%-20s ##########\n", "BC_DARK_GRAY");
    ColorPrint("", FC_WHITE, BC_BLUE, "%-20s ##########\n", "BC_BLUE");
    ColorPrint("", FC_WHITE, BC_GREEN, "%-20s ##########\n", "BC_GREEN");
    ColorPrint("", FC_WHITE, BC_CRAY, "%-20s ##########\n", "BC_CRAY");

    ColorPrint("", FC_WHITE, BC_RED, "%-20s ##########\n", "BC_RED");
    ColorPrint("", FC_WHITE, BC_MAGENTA, "%-20s ##########\n", "BC_MAGENTA");
    ColorPrint("", FC_DARK_GRAY, BC_YELLOW, "%-20s ##########\n", "BC_YELLOW");
    ColorPrint("", FC_BLACK, BC_WHITE, "%-20s ##########\n", "BC_WHITE");
}

bool ColorLog::IsFolderExist(const char* path)
{
    if(0 != ACCESS(path,0)) return false;
    return true;
}

bool ColorLog::CreateFolder(const char* path)
{
    if(0 != MKDIR(path)) return false;
    return true;
}

};