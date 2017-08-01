#include "commonlib_pch.h"
#include "namelimit.h"

IMPLEMENT_SINGLETON(NameLimit)
NameLimit::NameLimit(void)
{
}

NameLimit::~NameLimit(void)
{
}

bool NameLimit::ReadFile(const std::string& strfile)
{
    char ch[200];

    std::ifstream fileout(strfile.c_str(), std::ios::in);

    if(!fileout)
        return false;

    while (!fileout.eof())
    {
        fileout.getline(ch, 200, '\n');
        std::string strvalue = ch; 
        if( strvalue != "")
            Wordlist.push_back(strvalue);
    }

    fileout.close();
    if(Wordlist.size() <= 0)
        return false;
    return true;
}

bool NameLimit::IsContation(const std::string& str)
{
    std::vector<std::string>::iterator itervec;
    for (itervec = Wordlist.begin(); itervec != Wordlist.end(); itervec++)
    {
        std::string strcal = *itervec;
        const char* strvalue = strstr(str.c_str(),strcal.c_str());
        if(strvalue)
            return true;
    }

    return false;
}

uint16 NameLimit::IsValidGuildNameStr(const std::string& nameStr)
{
    char ch = 0;

    // 检查名字的长度是否有效
    int32 curlen = nameStr.length();
    if((curlen <= 0) || (curlen < GUILD_NAME_MIN_LEN))
    {
        return ERR_LESS_MIN_GUILD_NAME_LENGTH;
    }
    //3-8 个汉字，utf8的传输，一个汉字是3
    if(curlen > SERVER_NAME_MAX_LEN)
    {
        return ERR_MORE_MAX_NAME_LENGTH;
    }

    // 检查首字母;
    ch = nameStr.at(0);
    if(IsDigit(ch))
    {
        return ERR_FIRST_NAME_CHAR_DIGIT;
    }

    // 检查名字中是否包含特殊字符
    for(uint32 i=0 ; i < nameStr.size(); ++i)
    {
        ch = nameStr.at(i);
        if(IsSpecialNameCh(ch))
        {
            return ERR_USE_ILLEGAL_NAME_CHAR;
        }
    }

    if(IsContation(nameStr))
        return ERR_USE_ILLEGAL_NAME_CHAR;

    return ERR_SUCCEED;
}

uint16 NameLimit::IsValidNameStr(const std::string& nameStr)
{
    char ch = 0;

    // 检查名字的长度是否有效
    int32 curlen = nameStr.length();
    if((curlen <= 0) || (curlen < NAME_MIN_LEN))
    {
        return ERR_LESS_MIN_NAME_LENGTH;
    }
    //3-8 个汉字，utf8的传输，一个汉字是3
    if(curlen > SERVER_NAME_MAX_LEN)
    {
        return ERR_MORE_MAX_NAME_LENGTH;
    }

    // 检查首字母;
    ch = nameStr.at(0);
    if(IsDigit(ch))
    {
        return ERR_FIRST_NAME_CHAR_DIGIT;
    }

    // 检查名字中是否包含特殊字符
    for(uint32 i=0 ; i < nameStr.size(); ++i)
    {
        ch = nameStr.at(i);
        if(IsSpecialNameCh(ch))
        {
            return ERR_USE_ILLEGAL_NAME_CHAR;
        }
    }

    if(IsContation(nameStr))
        return ERR_USE_ILLEGAL_NAME_CHAR;

    return ERR_SUCCEED;
}

bool NameLimit::IsDigit(char ch)
{
    if(('0' <= ch) && (ch <= '9'))
    {
        return true;
    }
    return false;
}

bool NameLimit::IsSpecialNameCh(char ch)
{
    switch(ch)
    {
    case '~':   case '`':   case '!':   case '@':   case '#':
    case '$':   case '%':   case '^':   case '&':   case '*':
    case '(':   case ')':   case '-':   case '+':   case '=':
    case '{':   case '[':   case '}':   case ']':   case '|':
    case '\\':  case ':':   case ';':   case '\"':  case '\'':
    case '<':   case ',':   case '>':   case '.':   case '?':
    case '/':
        // ------------------------------------------------------
    case ' ':   case '\t':  case '\r':  case '\n':
        return true;
        break;
    default:
        break;
    }

    return false;
}