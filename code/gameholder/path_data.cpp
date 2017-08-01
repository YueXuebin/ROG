#include "gameholder_pch.h"
#include "path_data.h"

PathData ParsePath(const char* PathString)
{
    PathData pathData;

    if(!PathString)
        return pathData;

    // 解析路径字符串
    const char* pattern = "[0-9]+";
    pcre *re;
    const char *error;
    int32 erroffset;
    re = pcre_compile(pattern, PCRE_MULTILINE, &error, &erroffset, NULL);
    CnAssert(re);

    int32 options = PCRE_NOTEMPTY_ATSTART;
    int32 start_offset = 0;

    const int32 overCount = 30;
    int32 ovector[overCount];

    char buffString[SD_MAX_PATH];
    std::list<std::string>    cmdSplitStrings;

    const char* cmdString = PathString;
    int32 cmdStringLen = Crown::SDStrlen(PathString);

    int rc;
    while (start_offset < cmdStringLen && (rc = pcre_exec(re, 0, cmdString, cmdStringLen, start_offset, 0, ovector, sizeof(ovector))) >= 0)
    {
        for(int i = 0; i < rc; ++i)
        {
            int32 fileNameLen = ovector[2*i+1] - ovector[2*i];
            memcpy(buffString, (char*)cmdString+ovector[2*i], fileNameLen);
            buffString[fileNameLen] = '\0';
            // 全部转小写
            std::string lowerCase = buffString;
            CommonLib::ToLower(lowerCase);
            cmdSplitStrings.push_back(lowerCase);
            //printf("%2d: %.*s\n", i, fileNameLen, fileString + ovector[2*i]);
        }
        start_offset = ovector[1];
    }

    pcre_free(re);

    
    int32 pinI=0;
    for(std::list<std::string>::iterator iter = cmdSplitStrings.begin(); iter != cmdSplitStrings.end(); iter++)
    {
        if(pinI >= MAX_PATH_PIN)
            break;

        const std::string& splitString = *iter;
        int32 pinId = SDAtoi(splitString.c_str());

        pathData.CheckPointList[pinI] = pinId;

        pinI++;
    }

    return pathData;
}