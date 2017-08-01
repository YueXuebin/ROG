#ifndef gm_config_h__
#define gm_config_h__

class GMConfig
{
public:
    GMConfig() { }
    ~GMConfig() { }

    DECLARE_SINGLETON(GMConfig);

    bool LoadConfig(const char *fileName);

    bool IsPermitAddr(const char *ip);

private:
    std::set<std::string> m_WhiteTable;
};


#endif