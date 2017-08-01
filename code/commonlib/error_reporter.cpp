#include "commonlib_pch.h"
#include "error_reporter.h"

void ErrorReporter( const char* message )
{
    // 创建错误输出文件
    char errorFileName[100];
    SDSnprintf(errorFileName, 100, "%d.err", Crown::SDGetCurrentProcessId());

    FILE* errFile = fopen(errorFileName,"at");
    if(errFile == NULL)
    {
        return;     // 连错误输出文件都无法创建
    }
    
    // 写当前时间到文件中
    uint32 nLen = 0;
    char errString[100];

    time_t t = time(NULL);
    tm *local = localtime(&t);
    SDSnprintf(errString, 100, "[%04d-%02d-%02d %02d:%02d:%02d]: ",
        local->tm_year+1900,local->tm_mon+1, local->tm_mday,
        local->tm_hour,local->tm_min, local->tm_sec);

    nLen = (uint32)strlen(errString);
    fwrite(errString, sizeof(char), nLen, errFile);

    // 写错误信息到文件中
    nLen = (uint32)strlen(message);
    fwrite(message, sizeof(char), nLen, errFile);

    // 关闭文件
    fclose(errFile);
    errFile = NULL;
}

