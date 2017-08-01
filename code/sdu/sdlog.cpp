#include "SDuPCH.h"
#include "sdlog.h"
#include "sdtime.h"
#include "colorlog.h"

const int logStrLength = 200;

#ifdef WIN

void WinLogger_CRITICAL(const char* format, ...)
{
    char p[logStrLength];

    char t[logStrLength];
    Crown::SDNow().ToString(t, logStrLength);

    va_list args;
    va_start(args, format);
    vsnprintf(p, logStrLength, format, args);
    va_end(args);

    Crown::ColorLog::SetColor(FC_RED, FC_BLACK);
    _RPT2(_CRT_WARN, "CRITICAL:(%s)%s\n", t, p);
    printf("CRITICAL:(%s)%s\n", t, p);
    Crown:: ColorLog::SetColor(FC_GRAY, BC_BLACK);
}

void WinLogger_WARN(const char* format, ...)
{
    char p[logStrLength];

    char t[logStrLength];
    Crown::SDNow().ToString(t, logStrLength);

    va_list args;
    va_start(args, format);
    vsnprintf(p, logStrLength, format, args);
    va_end(args);

    Crown::ColorLog::SetColor(FC_YELLOW, FC_BLACK);
    _RPT2(_CRT_WARN, "WARN:(%s)%s\n", t, p);
    printf("WARN:(%s)%s\n", t, p);
    Crown::ColorLog::SetColor(FC_GRAY, BC_BLACK);
}

void WinLogger_INFO(const char* format, ...)
{
    char p[logStrLength];

    char t[logStrLength];
    Crown::SDNow().ToString(t, logStrLength);

    va_list args;
    va_start(args, format);
    vsnprintf(p, logStrLength, format, args);
    va_end(args);

    _RPT2(_CRT_WARN, "INFO:(%s)%s\n", t, p);
    printf("INFO:(%s)%s\n", t, p);
}

void WinLogger_DBG(const char* format, ...)
{
    char p[logStrLength];

    char t[logStrLength];
    Crown::SDNow().ToString(t, logStrLength);

    va_list args;
    va_start(args, format);
    vsnprintf(p, logStrLength, format, args);
    va_end(args);

    Crown::ColorLog::SetColor(FC_GREEN, FC_BLACK);
    _RPT2(_CRT_WARN, "DBG:(%s)%s\n", t, p);
    printf("DBG:(%s)%s\n", t, p);
    Crown::ColorLog::SetColor(FC_GRAY, BC_BLACK);
}

void WinLogger_PY(const char* pylog, bool isErr)
{
    static bool pyNewLine = true;

    char p[logStrLength];

    int Pos = 0;
    int StartPos = 0;
    while(pylog[Pos] != '\0')
    {
        if(pyNewLine)
        {
            _RPT0(_CRT_WARN, "PY: ");
            printf("PY: ");
            pyNewLine = false;
        }

        if(pylog[Pos++] == '\n' || (Pos-StartPos)>=(logStrLength-1))
        {
            strcpy(p, pylog+StartPos);
            p[Pos-StartPos] = '\0';

            _RPT1(_CRT_WARN, "%s", p);
            printf("%s", p);

            pyNewLine = true;
            StartPos = Pos;
        }
    }

    if(StartPos != Pos)
    {
        strcpy(p, pylog+StartPos);
        p[Pos+1] = '\0';

        _RPT1(_CRT_WARN, "%s", p);
        printf("%s", p);
    }

}

#else // LINUX
void LinuxLogger_CRITICAL(const char* format, ...)
{
    printf("CRITICAL: ");
    char p[logStrLength];

    va_list args;
    va_start(args, format);
    vsnprintf(p, logStrLength, format, args);
    va_end(args);

    Crown::ColorLog::SetColor(FC_RED, FC_BLACK);
    printf("%s\n", p);
    Crown:: ColorLog::SetColor(FC_GRAY, BC_BLACK);
}

void LinuxLogger_WARN(const char* format, ...)
{
    printf("WARN: ");

    char p[logStrLength];

    va_list args;
    va_start(args, format);
    vsnprintf(p, logStrLength, format, args);
    va_end(args);

    Crown::ColorLog::SetColor(FC_YELLOW, FC_BLACK);
    printf("%s\n", p);
    Crown:: ColorLog::SetColor(FC_GRAY, BC_BLACK);
}

void LinuxLogger_INFO(const char* format, ...)
{
    printf("INFO: ");
    char p[logStrLength];

    char t[logStrLength];
    Crown::SDNow().ToString(t, logStrLength);

    va_list args;
    va_start(args, format);
    vsnprintf(p, logStrLength, format, args);
    va_end(args);

    printf("(%s)%s\n", t, p);
}

void LinuxLogger_DBG(const char* format, ...)
{
    printf("DBG: ");
    char p[logStrLength];

    char t[logStrLength];
    Crown::SDNow().ToString(t, logStrLength);

    va_list args;
    va_start(args, format);
    vsnprintf(p, logStrLength, format, args);
    va_end(args);

    Crown::ColorLog::SetColor(FC_GREEN, FC_BLACK);
    printf("(%s)%s\n", t, p);
    Crown:: ColorLog::SetColor(FC_GRAY, BC_BLACK);
}

#endif
