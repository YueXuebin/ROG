// 越买越便宜
#ifndef buy_cheap_config_h__
#define buy_cheap_config_h__

class BuyCheapConfig
{
public:
    BuyCheapConfig();
    ~BuyCheapConfig();

    DECLARE_SINGLETON(BuyCheapConfig)

    bool LoadConfig(const char* path);

    bool DBSetConfig(std::string buyCheaps, Json::Value* jsonValue = NULL);         // 先读出XML文件数据 后通过GMT更新掉数据

    const BUY_CHEAP_CONFIG& GetBuyCheaps(){return m_buyCheapsTmp;}
    const BUY_CHEAPS_ITEM* GetBuyCheapByDay(int32 day);
private:
    BUY_CHEAP_CONFIG m_buyCheapsTmp;
};

#endif // buy_cheap_config_h__
