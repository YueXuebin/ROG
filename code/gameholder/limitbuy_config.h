#pragma once


typedef struct LimitBuyTemp
{
    int32 num;
    int32 type;
    void Clear()
    {
        num = 0;
        type = 0;
    }

}LimitBuyTemp;
class LimitBuyConfig
{
public:
    LimitBuyConfig(void);
    virtual ~LimitBuyConfig(void);
    DECLARE_SINGLETON(LimitBuyConfig)

    bool LoadConfig(const char* path);

    LimitBuyTemp* FindLimitTemp(int32 productid);
private:
    std::map<uint32,LimitBuyTemp> m_buyLimitList;
};
