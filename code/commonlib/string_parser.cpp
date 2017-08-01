#include "commonlib_pch.h"
#include "string_parser.h"

using namespace CommonLib;

void CommonLib::FindStrings( const char* pattern, const char* inString, std::vector<std::string>& outString )
{
    outString.clear();
    uint32 inStringLen = Crown::SDStrlen(inString);

    // 正则表达式解析
    pcre *re;
    const char *error;
    int32 erroffset;
    re = pcre_compile(pattern, PCRE_MULTILINE, &error, &erroffset, NULL);
    CnAssert(re);

    int32 options = PCRE_NOTEMPTY_ATSTART;
    uint32 start_offset = 0;

    const int32 overCount = 30;
    int32 ovector[overCount];

    char skillFileName[SD_MAX_PATH];

    int rc;
    while (start_offset < inStringLen && (rc = pcre_exec(re, 0, inString, inStringLen, start_offset, 0, ovector, sizeof(ovector))) >= 0)
    {
        for(int i = 0; i < rc; ++i)
        {
            int32 fileNameLen = ovector[2*i+1] - ovector[2*i];
            memcpy(skillFileName, inString+ovector[2*i], fileNameLen);
            skillFileName[fileNameLen] = '\0';
            outString.push_back(skillFileName);
            //printf("%2d: %.*s\n", i, fileNameLen, fileString + ovector[2*i]);
        }
        start_offset = ovector[1];
    }

    pcre_free(re);
}

void CommonLib::ToLower(std::string& str)
{
    transform (str.begin(), str.end(), str.begin(), tolower);
}