#ifndef center_config_h__
#define center_config_h__


class CenterConfig
{
public:
    CenterConfig(void);
    ~CenterConfig(void);

    DECLARE_SINGLETON(CenterConfig)

    bool ReadFile(const std::string& strfile);
};

#endif // center_config_h__