#ifndef COLORLOG_H
#define COLORLOG_H

#include <stdio.h>
#include <time.h>
#include "sdthread.h"
#include "sdstring.h"

#ifdef WIN
#include "windows.h"
#endif

//############################
#ifdef WIN
//============================
#define FC_BLACK        0x0000
#define FC_DARK_BLUE    0x0001
#define FC_DARK_GREEN   0x0002
#define FC_DARK_CRAY    0x0003
#define FC_DARK_RED     0x0004
#define FC_DARK_MAGENTA 0x0005
#define FC_DARK_YELLOW  0x0006
#define FC_GRAY         0x0007
#define FC_DARK_GRAY    0x0008
#define FC_BLUE         0x0009
#define FC_GREEN        0x000A
#define FC_CRAY         0x000B
#define FC_RED          0x000C
#define FC_MAGENTA      0x000D
#define FC_YELLOW       0x000E
#define FC_WHITE        0x000F
//============================
#define BC_BLACK        0x0000
#define BC_DARK_BLUE    0x0010
#define BC_DARK_GREEN   0x0020
#define BC_DARK_CRAY    0x0030
#define BC_DARK_RED     0x0040
#define BC_DARK_MAGENTA 0x0050
#define BC_DARK_YELLOW  0x0060
#define BC_GRAY         0x0070
#define BC_DARK_GRAY    0x0080
#define BC_BLUE         0x0090
#define BC_GREEN        0x00A0
#define BC_CRAY         0x00B0
#define BC_RED          0x00C0
#define BC_MAGENTA      0x00D0
#define BC_YELLOW       0x00E0
#define BC_WHITE        0x00F0
//============================
#else
//============================
#define FC_BLACK        30
#define FC_DARK_BLUE    34
#define FC_DARK_GREEN   32
#define FC_DARK_CRAY    36
#define FC_DARK_RED     31
#define FC_DARK_MAGENTA 35
#define FC_DARK_YELLOW  33
#define FC_GRAY         37
#define FC_DARK_GRAY    90
#define FC_BLUE         94
#define FC_GREEN        92
#define FC_CRAY         96
#define FC_RED          91
#define FC_MAGENTA      95
#define FC_YELLOW       93
#define FC_WHITE        97
//============================
#define BC_BLACK        40
#define BC_DARK_BLUE    44
#define BC_DARK_GREEN   42
#define BC_DARK_CRAY    46
#define BC_DARK_RED     41
#define BC_DARK_MAGENTA 45
#define BC_DARK_YELLOW  43
#define BC_GRAY         47
#define BC_DARK_GRAY    100
#define BC_BLUE         104
#define BC_GREEN        102
#define BC_CRAY         106
#define BC_RED          104
#define BC_MAGENTA      105
#define BC_YELLOW       103
#define BC_WHITE        107
//============================
#endif
//############################

#ifdef DEBUG
#define CnDbg(format, ...)      Crown::ColorLog::Instance()->Debug(format, ##__VA_ARGS__)       // 调试信息
#define CnInfo(format, ...)     Crown::ColorLog::Instance()->Info(format, ##__VA_ARGS__)        // 记录信息
#define CnWarn(format, ...)     Crown::ColorLog::Instance()->Warn(format, ##__VA_ARGS__)        // 警告(游戏仍然可以正常运行)
#define CnError(format, ...)    Crown::ColorLog::Instance()->Error(format, ##__VA_ARGS__)       // 错误(游戏仍然可以运行,但不符合预期行为)
#define CnFatal(format, ...)    Crown::ColorLog::Instance()->Fatal(format, ##__VA_ARGS__)       // 严重错误(游戏无法运行)
#else
#define CnDbg(format, ...)      Crown::ColorLog::Instance()->Debug(format, ##__VA_ARGS__)
#define CnInfo(format, ...)     Crown::ColorLog::Instance()->Info(format, ##__VA_ARGS__)
#define CnWarn(format, ...)     Crown::ColorLog::Instance()->Warn(format, ##__VA_ARGS__)
#define CnError(format, ...)    Crown::ColorLog::Instance()->Error(format, ##__VA_ARGS__)
#define CnFatal(format, ...)    Crown::ColorLog::Instance()->Fatal(format, ##__VA_ARGS__)
#endif

namespace Crown
{

class ColorLog
{
public:
    ColorLog();
    ~ColorLog();

    DECLARE_SINGLETON(ColorLog)

    bool Init(const char* folder, const char* file);
    void Uninit();
    
    void Update();

    void SetLogTime(bool val);

    void Debug(const char * format, ...);           // 调试用输出
    void Info(const char * format, ...);            // 信息记录
    void Warn(const char * format, ...);            // 警告(程序仍然可以正常运行)
    void Error(const char * format, ...);           // 错误(程序不能正常运行了，必须马上修改)
    void Fatal(const char * format, ...);           // 严重错误(同上)

    void ColorPrint(const char* label, unsigned int fc, unsigned int bc, const char * format, ...);

    static void SetColor(unsigned int fc, unsigned int bc);

    void Sample();


private:
    ColorLog(const ColorLog&);
    ColorLog& operator=(const ColorLog&);
    ColorLog* operator&();
    const ColorLog operator&() const;

    static bool IsFolderExist(const char* path);
    static bool CreateFolder(const char* path);

        
    enum
    {
        LOG_FILE_NAME_LEN = 256,
        LOG_MESSAGE_LEN = 1024
    };

    bool mLogTime;
    char mFolderName[LOG_FILE_NAME_LEN];
    char mFileName[LOG_FILE_NAME_LEN];
    FILE* mFile;

    tm*     m_LocalTime;
    uint64  m_TickCountStart;
    uint32  m_TickCountTime;                // 当前的毫秒数
    Crown::SDTHREADID  m_MainThreadID;

#ifdef WIN
    HANDLE m_hConsole;
    CONSOLE_SCREEN_BUFFER_INFO m_ScreenInfo;
#endif
};

};

#endif