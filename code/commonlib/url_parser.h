#ifndef url_parser_h__
#define url_parser_h__

/*
 *	URL解析器
 */
class UrlParser
{
public:
    UrlParser(const char* url = "");
    ~UrlParser();

    void SetUrl(const char* pUrl);

    bool SetArgument(const char* key, const char* value);
    void AddArgument(const char* key, const char* value);
    std::string GetArgument(const char* key);
    bool HasArgument(const char* key);

    std::string GetUrl();
    const std::string& GetAddress() {return m_Address;}

private:
    void AddArgs(const std::string& str);

private:
    std::string m_Address;
    std::map<std::string, std::string>  m_args;

};


#endif // url_parser_h__
