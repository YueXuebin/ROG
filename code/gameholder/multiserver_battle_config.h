#ifndef multiserver_battle_config_h__
#define multiserver_battle_vonfig_h__


typedef struct MultiserverLevelItem
{
    uint32 lv;
    uint32 point;
    uint32 exploit;
    uint32 alliance;
    void Clear()
    {
        lv = 0;
        point = 0;
        exploit = 0;
        alliance = 0;
    }
}MultiserverLevelItem;

typedef struct MultiserverSaleInfo
{
    uint32 id;
    uint32 level;
    uint32 limit;
    uint32 product_id;
    uint32 exploit;
    void Clear()
    {
        id = 0;
        level = 0;
        limit = 0;
        product_id = 0;
        exploit = 0;
    }
}MultiserverSaleInfo;

typedef struct MultiserverBatttleItem
{
    uint32 type;
    uint32 win_integral;
    uint32 win_exploit;
    uint32 fail_integral;
    uint32 fail_exploit;
    uint32 wait;
    uint32 need_level;
    uint32 enter_num;
    uint32 start_range_integral;                    ///< 开始随机积分
    uint32 stop_range_integral;                     ///< 结束随机积分
    uint32 fail_start_range_integral;                    ///< 失败开始随机积分
    uint32 fail_stop_range_integral;                     ///< 失败结束随机积分
    PropertyList open_timer_list;                   ///< 开启时间列表
    void Clear()
    {
        type = 0;
        win_integral = 0;
        win_exploit = 0;
        fail_integral = 0;
        fail_exploit = 0;
        wait = 0;
        need_level = 0;
        enter_num = 0;
        start_range_integral = 0;
        stop_range_integral = 0;
        fail_start_range_integral = 0;
        fail_stop_range_integral = 0;
        open_timer_list.clear();
    }
}MultiserverBatttleItem;

typedef std::vector<MultiserverLevelItem>  MultiserverLevelItemList;
typedef std::vector<MultiserverBatttleItem>  MultiserverBatttleItemList;


class MultiserverBattleConfig
{
public:
    MultiserverBattleConfig(void);
    ~MultiserverBattleConfig(void);

    DECLARE_SINGLETON(MultiserverBattleConfig)

    bool LoadConfig(const char* path);

    const MultiserverLevelItem* GetLevelInfo(int32 integral);                           // 获取等级信息
    const MultiserverBatttleItem* GetBattleInfo(int32 type);                            // 获取战斗信息
    const struct MultiserverSaleInfo* GetSaleItem(uint32 itemID);                       // 获取商店物品

    const struct MultiserverSaleInfo* GetAllianceSaleItem(uint32 itemID);
    const struct MultiserverSaleInfo* GetSeasonSaleItem(uint32 itemID);

    int32 GetYesterdayCount();                                                          // 昨天战斗次数
    uint32 GetMaxIntegral();                                                            // 最大积分
    int32 GetRangeIntegral(int32 type, uint32 resultType);                              // 获取随机积分
    bool IsOpenTimer(int32 type);                                                       // 获取随机积分
private:
    bool LoadBattleConfig(const char* path);                                            // 读取配置
    void LoadOpenTime(const std::string& base_value, MultiserverBatttleItem &item);
    void LoadRangeIntegral(const std::string& base_value, MultiserverBatttleItem &item, uint32 loadType);
    int32 GetTodayTime();

private:
    MultiserverLevelItemList  m_levelList;                                              // 排位信息
    MultiserverBatttleItemList  m_battleList;                                           // 战斗信息

    typedef std::vector<MultiserverSaleInfo> SaleList;
    SaleList m_saleList;
    int32 m_yesterdayCount;                                                             // 昨天战斗次数
    int32 m_yesterday3V3Count;                                                          // 昨天3V3战斗次数

    typedef std::vector<MultiserverSaleInfo> AllianceList;
    AllianceList m_allianceList;

    typedef std::vector<MultiserverSaleInfo> SeasonList;
    SeasonList m_seasonList;

};

#endif