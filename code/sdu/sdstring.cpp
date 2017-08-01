#include "SDuPCH.h"
#include "sdstring.h"
#include <algorithm>
#include <string.h>
#include <sstream>

namespace Crown
{

    char* SDAPI SDStrlwr(char *str)
    {
#if (defined(WIN32) || defined(WIN64))
        return strlwr(str);
#else
        std::transform(str, str + SDStrlen(str),
                       str, ::tolower);
        return str;
#endif
    }

    char* SDAPI SDStrupr(char *str)
    {
#if (defined(WIN32) || defined(WIN64))
        return strupr(str);
#else
        std::transform(str, str + SDStrlen(str),
                       str, ::toupper);
        return str;
#endif
    }

    wchar_t* SDAPI SDWcslwr(wchar_t *str)
    {
#if (defined(WIN32) || defined(WIN64))
        return wcslwr(str);
#else
        std::transform(str, str + SDWcslen(str),
                       str, ::tolower);
        return str;
#endif
    }

    wchar_t* SDAPI SDWcsupr(wchar_t *str)
    {
#if (defined(WIN32) || defined(WIN64))
        return wcsupr(str);
#else
        std::transform(str, str + SDWcslen(str),
                       str, ::towupper);
        return str;
#endif
    }

    std::vector<std::string> SDAPI SDSplitStrings(const std::string& sSource, char delim)
    {
        std::string::size_type pos1, pos2;
        std::vector<std::string> strParams;

        pos2 = 0;
        while (pos2 != std::string::npos)
        {
            pos1 = sSource.find_first_not_of(delim, pos2);
            if (pos1 == std::string::npos)
                break;
            pos2 = sSource.find_first_of(delim, pos1 + 1);
            if (pos2 == std::string::npos)
            {
                if (pos1 != sSource.size())
                    strParams.push_back(sSource.substr(pos1));
                break;
            }
            strParams.push_back(sSource.substr(pos1, pos2 - pos1));
        }
        return strParams;
    }

    std::string  SDAPI SDStrupr(std::string& str)
    {
        return SDStrupr(const_cast<char*>(str.c_str()));
    }

    std::string  SDAPI SDStrlwr(std::string& str)
    {
        return SDStrlwr(const_cast<char*>(str.c_str()));
    }

    std::string SDAPI SDBufferToHex(const char *pBuf, uint32 bufLen, const char *pSplitter, int32 lineLen)
    {
		static unsigned char szHex[] = "0123456789abcdef";

		char* p = (char*)pBuf;
		std::string strResult;
		for (unsigned int i = 0; i < bufLen; ++i)
		{
			strResult += szHex[(p[i] >> 4) & 0xF];
			strResult += szHex[p[i] & 0xF];
			if (pSplitter != nullptr && *pSplitter && i < bufLen - 1)
			{
				strResult.append(pSplitter);
			}
			if (lineLen > 0  && (i + 1) % lineLen == 0)
            {
                strResult += "\n";
            }
		}
		return strResult;
    }

    std::string SDAPI SDIntToHex(int32 num)
    {
        char szBuf[16] = {0};
        SDSnprintf(szBuf, sizeof(szBuf), "0x%x",num);
        return szBuf;
    }

    std::string  SDAPI SDStrTrim(const std::string& str, const char *trimStr )
    {
        int32 len = str.length();
        const char* pchar1 = str.c_str();
        char* pchar2 = (char*)CnMalloc(sizeof(char)*(len+1));
        pchar2[len] = '\0';
        
        int32 i2 = 0;
        for(int32 i=0; i<len; i++)
        {
            char src = *(pchar1+i);
            
            // 判断该字符是否在排除字符内
            bool isIn = false;
            uint32 j=0;
            while( *(trimStr+j) != '\0' )
            {
                char trimChar = *(trimStr+j);
                if(trimChar == src)
                {
                    isIn = true;
                    break;
                }
                j++;
            }

            if(isIn)
                continue;

            *(pchar2+i2) = src;
            i2++;
        }

        *(pchar2+i2) = '\0';
        std::string resultStr = pchar2;
        CnFree(pchar2);
        return resultStr;
    }

    uint32 SDAPI SDStrTimeStamp(std::string& pszStr)
    {
        struct tm time_tm;
        uint32 time_stamp = 0;

        memset(&time_tm, 0, sizeof(time_tm));

        sscanf(pszStr.c_str(), "%d-%d-%d %d:%d:%d", 
            &time_tm.tm_year, &time_tm.tm_mon, &time_tm.tm_mday, 
            &time_tm.tm_hour, &time_tm.tm_min, &time_tm.tm_sec);

        if(time_tm.tm_year >= 1900) time_tm.tm_year -= 1900;
        if(time_tm.tm_mon >= 1) time_tm.tm_mon -= 1;
        time_tm.tm_isdst = -1;

        time_stamp = (uint32)mktime(&time_tm);

        return time_stamp;
    }

    uint32 SDAPI SDAtou(const char* pStr)
    {
        uint32 result = 0;
        const char * pos = pStr;
        while (*pos != 0)
        {
            if (isdigit(*pos) || *pos == '+')
            {
                uint32 d = *pos - '0';
                result = result * 10 + d;
            }
            else
            {
                return 0;
            }
            pos ++;
        }
        return result;
    }

    uint64 SDAPI SDAtou64(const char* pStr)
    {
        uint64 result = 0;
        const char * pos = pStr;
        while (*pos != 0 )
        {
            if (isdigit(*pos) || *pos == '+')
            {
                uint64 d = *pos - '0';
                result = result * 10 + d;
            }
            else
            {
                return 0;
            }
            pos ++;
        }
        return result;
    }

    std::string ToString(int32 number)
    {
        std::string str = "";
        std::stringstream strStream;
        strStream << number;
        strStream >> str;
        return str;
    }

    std::string ToString(uint32 number)
    {
        std::string str = "";
        std::stringstream strStream;
        strStream << number;
        strStream >> str;
        return str;
    }

    std::string ToString(int64 number)
    {
        std::string str = "";
        std::stringstream strStream;
        strStream << number;
        strStream >> str;
        return str;
    }

    std::string ToString(uint64 number)
    {
        std::string str = "";
        std::stringstream strStream;
        strStream << number;
        strStream >> str;
        return str;
    }

    std::string ToString(double number)
    {
        std::string str = "";
        std::stringstream strStream;
        strStream << number;
        strStream >> str;
        return str;
    }

#if (defined(WIN32) || defined(WIN64))
    //utf8 转为 unicode
    std::wstring Utf8ToUnicode(const std::string& utf8string)
    {
        int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);
        if (widesize == ERROR_NO_UNICODE_TRANSLATION)
        {
            throw std::exception("Invalid UTF-8 sequence.");
        }

        if (widesize == 0)  
        {
            throw std::exception("Error in conversion.");
        }

        std::vector<wchar_t> resultstring(widesize);

        int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);

        if (convresult != widesize)
        {
            throw std::exception("La falla!");
        }

        return std::wstring(&resultstring[0]);
    }

    //unicode 转为 ascii
    std::string WideByteToAcsii(std::wstring& wstrcode)
    {
        int asciisize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, NULL, 0, NULL, NULL);
        if (asciisize == ERROR_NO_UNICODE_TRANSLATION)  
        {
            throw std::exception("Invalid UTF-8 sequence.");
        }
        if (asciisize == 0)
        {
            throw std::exception("Error in conversion.");
        }
        std::vector<char> resultstring(asciisize);
        int convresult =::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, &resultstring[0], asciisize, NULL, NULL);

        if (convresult != asciisize)
        {
            throw std::exception("La falla!");
        }

        return std::string(&resultstring[0]);
    }

    //utf-8 转 ascii
    std::string Utf8ToAscii(std::string& strUtf8Code)
    {
        std::string strRet("");
        //先把 utf8 转为 unicode
        std::wstring wstr = Utf8ToUnicode(strUtf8Code);
        //最后把 unicode 转为 ascii
        strRet = WideByteToAcsii(wstr);
        return strRet;
    }

    //ascii 转 Unicode
    std::wstring AcsiToWideByte(std::string& strascii)
    {
        int widesize = MultiByteToWideChar (CP_ACP, 0, (char*)strascii.c_str(), -1, NULL, 0);
        if (widesize == ERROR_NO_UNICODE_TRANSLATION)
        {
            throw std::exception("Invalid UTF-8 sequence.");
        }
        if (widesize == 0)
        {
            throw std::exception("Error in conversion.");
        }
        std::vector<wchar_t> resultstring(widesize);
        int convresult = MultiByteToWideChar (CP_ACP, 0, (char*)strascii.c_str(), -1, &resultstring[0], widesize);


        if (convresult != widesize)
        {
            throw std::exception("La falla!");
        }

        return std::wstring(&resultstring[0]);
    }

    //Unicode 转 Utf8
    std::string UnicodeToUtf8(const std::wstring& widestring)
    {
        int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);
        if (utf8size == 0)  
        {
            throw std::exception("Error in conversion.");
        }

        std::vector<char> resultstring(utf8size);

        int convresult = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, &resultstring[0], utf8size, NULL, NULL);

        if (convresult != utf8size)
        {
            throw std::exception("La falla!");
        }

        return std::string(&resultstring[0]);
    }

    //ascii 转 Utf8
    std::string AsciiToUtf8(std::string& strAsciiCode)
    {
        std::string strRet("");
        //先把 ascii 转为 unicode
        std::wstring wstr = AcsiToWideByte(strAsciiCode);
        //最后把 unicode 转为 utf8
        strRet = UnicodeToUtf8(wstr);
        return strRet;
    }
#endif
}


