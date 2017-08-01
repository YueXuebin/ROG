#pragma once

class NameLimit
{
public:
    NameLimit(void);
    ~NameLimit(void);

    DECLARE_SINGLETON(NameLimit)

    bool ReadFile(const std::string& strfile);
    uint16 IsValidNameStr(const std::string& namestr);
    uint16 IsValidGuildNameStr(const std::string& namestr);

private:
    bool IsContation(const std::string& str);
    bool IsSpecialNameCh(char ch);
    bool IsDigit(char ch);

private:
    std::vector<std::string> Wordlist;
};
