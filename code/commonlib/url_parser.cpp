#include "commonlib_pch.h"
#include "url_parser.h"


UrlParser::UrlParser( const char* url )
{
    SetUrl(url);
}

UrlParser::~UrlParser()
{

}

void UrlParser::SetUrl( const char* pUrl )
{
    if(pUrl == NULL)
        return;

    std::string url = pUrl;

    m_args.clear();
    // 解析URL
    int p = url.find_first_of("?");
    if(p>0)
    {
        std::string only_url = url.substr(0, p);
        std::string args = url.substr(p+1);

        m_Address = only_url;

        std::string remain_args = args;
        int p1 = args.find_first_of("&");
        while(p1 > 0)
        {
            std::string arg_txt = args.substr(0, p1);
            remain_args = args.substr(p1+1);

            p1 = remain_args.find_first_of("&");
            args = remain_args;

            AddArgs(arg_txt);
        }

        AddArgs(remain_args);
    }
    else
    {
        m_Address = url;
    }
}

std::string UrlParser::GetUrl()
{
    std::string url = m_Address;
    for(std::map<std::string, std::string>::iterator iter = m_args.begin(); iter != m_args.end(); iter++)
    {
        std::string arg_txt = "";
        if(iter->second.size() > 0)
        {
            arg_txt += iter->first;
            arg_txt += "=";
            arg_txt += iter->second;
        }
        else
        {
            arg_txt += iter->first;
        }
        
        if(iter == m_args.begin())
            url += "?";
        else
            url += "&";
        url += arg_txt;
    }
    return url;
}

void UrlParser::AddArgument(const char* key, const char* value)
{
    m_args[key] = value;
}

bool UrlParser::SetArgument( const char* key, const char* value )
{
    std::map<std::string, std::string>::iterator iter = m_args.find(key);
    if(iter == m_args.end())
        return false;

    iter->second = value;
    return true;
}

std::string UrlParser::GetArgument( const char* key )
{
    std::map<std::string, std::string>::iterator iter = m_args.find(key);
    if(iter == m_args.end())
        return "";

    return iter->second;
}

bool UrlParser::HasArgument(const char* key)
{
    std::map<std::string, std::string>::iterator iter = m_args.find(key);
    if(iter == m_args.end())
        return false;

    return true;
}

void UrlParser::AddArgs(const std::string& str)
{
    std::string arg_txt = str;
    int p = arg_txt.find_first_of("=");
    if(p > 0)
    {
        std::string key = arg_txt.substr(0, p);
        std::string value = arg_txt.substr(p+1);
        m_args[key] = value;
    }
    else
    {
        m_args[arg_txt] = "";
    }
}


