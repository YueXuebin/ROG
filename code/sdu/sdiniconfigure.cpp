#include "SDuPCH.h"
#include "sdiniconfigure.h"
#include <iostream>
#include <fstream>
#include <clocale>
#include "sdfile.h"
#include "sdstring.h"

using namespace Crown;

//#include <locale.h>

#if(defined LINUX || defined MACOS)
//static const char *linefeed = "\n";
static const int signlen = 1;
static const char fsign = '\n';
static const char ssign = '\n';
#else
//static const char *linefeed = "\r\n";
static const int signlen = 2;
static const char fsign = '\r';
static const char ssign = '\n';
#endif

CSDIniConfigure::CSDIniConfigure(const char * pCfgFile )
{
    if ( pCfgFile != nullptr )
    {
        this->ReadConfig(pCfgFile);
    }
}

CSDIniConfigure::~CSDIniConfigure(void)
{
}

bool CSDIniConfigure::WriteConfig(const char * pFilePath)
{
    if(nullptr == pFilePath)
    {
        return false;
    }
    CSDFile file;
    bool ret = file.Open(pFilePath,"w+");
    if (ret)
    {
        file.SeekToBegin();
        std::string txt = ToString();
        file.Write(txt.c_str(),txt.length());
        file.Flush();
        file.Close();
    }
    return true;
}

bool CSDIniConfigure::ReadConfig(const char * pFilePath,const char* /*pLocate*/)
{
    const long bufferlen = 65535;
    if(nullptr == pFilePath)
    {
        return false;
    }
    //使用std实现，由于考虑到国际化的问题，暂时保留此段代码
    //std::locale locl = std::locale::global(std::locale(".936"));
    //std::ifstream ifs(pFilePath);
    //if(!ifs)
    //{
    //	return -2;
    //}
    //std::string s;
    //int32 count = 0;
    //while(std::getline(ifs, s))
    //{
    //	if('#' == s[0])
    //	{
    //		continue;
    //	}
    //	if('/' == s[0] && '/' == s[1])
    //	{
    //		continue;
    //	}
    //	std::string::size_type key_end_pos = s.find("=");
    //	if(std::string::npos == key_end_pos)
    //	{
    //		continue;
    //	}
    //	std::string key = s.substr(0, key_end_pos);
    //	std::string value = s.substr(key_end_pos + 1);
    //	if(0 == value.length())
    //	{
    //		continue;
    //	}
    //	this->SetProperty(key, value);
    //	count++;
    //}
    //std::locale::global(std::locale(locl));
    //return count;

    CSDFile file;
    //int32 count = 0;
    bool ret = file.Open(pFilePath, "r");
    if (ret)
    {
        file.SeekToBegin();

        char buffer[bufferlen];

        //获取文件长度
        SFileStatus fs;
        if(!file.GetFileStatus(fs))
        {
            file.Close();
            return false;
        }
        uint32 filesize = fs.size;
        //获取文件长度结束

        do
        {
            uint32 len = file.Read(buffer, bufferlen - 1);
            if(len == bufferlen - 1)
            {
                char * bufferend = SDStrrchr(buffer, ssign);
                *(bufferend) = '\0';
                file.Seek(bufferend - buffer - len, CSDFile::SK_CUR);
                filesize -= bufferend - buffer + 1;
            }
            else if(0 == len)
            {
                file.Close();
                return true;
            }
            else
            {
                filesize -= len;
            }

            char * start = buffer;
            char * end = buffer;
            char * middle = buffer;
            while(start <  buffer + len)
            {
                end = SDStrchr(start, fsign);
                if(nullptr == end)
                {
                    end = buffer + len;
                }
                *end = '\0';

                if(*start != '#')
                {
                    middle = SDStrchr(start, '=');
                    if(nullptr != middle)
                    {
                        *middle = '\0';
                        std::string key(start);
                        std::string value(middle + 1);
                        this->SetProperty(key, value);
                    }
                }
                start = end + signlen;
            }
        }
        while(filesize > 0);
    }
    file.Close();
    return true;
}

bool CSDIniConfigure::ReadConfigFromMem(const char * pBuffer)
{
    if(nullptr == pBuffer)
    {
        return false;
    }
    //获取文件长度
    uint32 len = SDStrlen(pBuffer);
    //获取文件长度结束
    char * buffer = CnNew char[len];
    if(nullptr == buffer)
    {
        return false;
    }
    SDStrncpy(buffer, pBuffer, len);

    char * start = buffer;
    char * end = buffer;
    char * middle = buffer;
    while(start <  buffer + len)
    {
        end = SDStrchr(start, fsign);
        if(nullptr == end)
        {
            end = buffer + len;
        }
        *end = '\0';

        if(*start != '#')
        {
            middle = SDStrchr(start, '=');
            if(nullptr != middle)
            {
                *middle = '\0';
                std::string key(start);
                std::string value(middle + 1);
                this->SetProperty(key, value);
            }
        }
        start = end + signlen;
    }
    CnDelete [] buffer;
    return true;
}

void CSDIniConfigure::SetProperty(const std::string & key, const std::string & value)
{
    m_properties.insert(std::make_pair(key, value));
}

void CSDIniConfigure::RemoveProperty(const std::string & key)
{
    this->m_properties.erase(key);
}

std::string CSDIniConfigure::GetProperty(const std::string&  key)
{
    Iterator i = this->m_properties.find(key);
    if(this->m_properties.end() == i)
    {
        return "";
    }
    return i->second;
}

std::string CSDIniConfigure::ToString()
{
    std::string toString;
    Iterator i = this->m_properties.begin();
    for(; i != this->m_properties.end(); ++i)
    {
        toString.append(i->first);
        toString.append("=");
        toString.append(i->second);
#if (defined LINUX || defined MACOS)
        toString.append("\n");
#else
        toString.append("\r\n");
#endif
    }
    return toString;
}
