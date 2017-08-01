/******************************************************************************
补签次数与消耗魔石表
******************************************************************************/

#ifndef sign_makeup_config_h__
#define sign_makeup_config_h__

typedef struct SignMakeupTemplate
{
public:
    void Clear()
    {
        time = 0;
        money_point = 0;
    }

    int32 time;
    int32 money_point;
} SignMakeupTemplate;

class SignMakeupConfig
{
public:
    SignMakeupConfig();
    virtual ~SignMakeupConfig();

    DECLARE_SINGLETON(SignMakeupConfig)

    bool LoadConfig(const char* path);

    int32 GetMoneyPointByMakeupTime(int32 time);

private:
    std::vector<SignMakeupTemplate> m_sign_makeups;
};

#endif
