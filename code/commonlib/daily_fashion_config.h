// 每日化身
#ifndef daily_draw_fashion_config_h__
#define daily_draw_fashion_config_h__

class DailyFashionConfig
{
public:
    DailyFashionConfig(void);
    ~DailyFashionConfig(void);

    DECLARE_SINGLETON(DailyFashionConfig)

    bool LoadConfig(const char* path);

    bool DBSetConfig(std::string dailyDrawFashionConfig, Json::Value* jsonValue = NULL);         // 先读出XML文件数据 后通过GMT更新掉数据

    const DAILY_DRAW_FASHION_CONFIG& GetDailyDrawFashion(){return m_dailyDrawFashionConfig;}

    uint32 GetDailyDrawFashionPackageID(uint32 week, uint32 day, uint32 drawType);
    REWARD_ITEM GetDailyDrawFashionWillItem(uint32 week);
    uint32 GetConsumeMoney(uint32 type);

    uint32 GetNextDayId(DAILY_DRAW_FASHION_INFO* info);


private:
    void LoadItemToString(const std::string& base_value, REWARD_ITEM& reward_item);     // 解析物品

    DAILY_DRAW_FASHION_CONFIG m_dailyDrawFashionConfig;
};
#endif