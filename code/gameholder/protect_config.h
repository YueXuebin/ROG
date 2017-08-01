#ifndef protect_config_h__
#define protect_config_h__



typedef struct ProtectMoney
{
    int32 count;   //����
    int32 money;	//ħʯ

    void Clear()
    {
        count = 0;
        money = 0;
    }
}ProtectMoney;

class ProtectConfig
{
public:
    ProtectConfig(void);
    ~ProtectConfig(void);

    DECLARE_SINGLETON(ProtectConfig)

    bool LoadConfig(const char* path);

    int32 GetProtectMoney(int32 count);

private:
    typedef std::map<uint32, struct SaleCondition> SaleConditionMap;
    typedef std::vector<ProtectMoney> ProtectMoneyList;
    ProtectMoneyList m_protectMoney;
};

#endif