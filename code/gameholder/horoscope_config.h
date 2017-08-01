/*
 *	星空图配置horoscope.xml
 */
#ifndef horoscope_config_h__
#define horoscope_config_h__

typedef struct HoroscopeLimit
{
    void Clear()
    {
        horoLowLevel = 0;
        horoHighLevel = 0;
        playerLevel = 0;
    }
    int32 horoLowLevel;
    int32 horoHighLevel;
    int32 playerLevel;
}HoroscopeLimit;

typedef struct ConstellationTemplate
{
    void Clear()
    {
        level = 0;
        game_point = 0;
        soul_point = 0;
        time = 0;
        player_level = 0;
        start_point = 0;
        props.clear();
    }
    uint8 level;                            // 等级
    int32 game_point;                       // 游戏金币
    int32 soul_point;                       // 精魂
    uint32 time;                            // 使用时间
    int32 player_level;                     // 玩家的级别
    int32 start_point;                      // 星魂
    std::vector<Property> props;            // 属性类型, 属性值
}ConstellationTemplate;

typedef struct HoroscopeTemplate
{
    void Clear()
    {
        id = 0;
        max_level = 0;
        constellation_info.clear();
    }

    ConstellationTemplate* GetConstellation(int32 level)
    {
        for(std::vector<ConstellationTemplate>::iterator it = constellation_info.begin();
            it != constellation_info.end(); ++it)
        {
            if(it->level == level)
            {
                return &(*it);
            }
        }
        return 0;
    }

    uint32 id;
    int32 max_level;
    std::vector<ConstellationTemplate> constellation_info;
}HoroscopeTemplate;

class HoroscopeConfig
{
public:
    HoroscopeConfig();
    virtual ~HoroscopeConfig();

    DECLARE_SINGLETON(HoroscopeConfig)

    bool LoadConfig(const char* path);                          // 读取配置文件

    void LoadHoroscopeLimit();                                  // 加载星空图限制

    const ConstellationTemplate* GetConstellation(int32 level); // 根据星空图等级，获得星座模板

    int32 GetMaxHoroscopeLevel();                               // 获得星空图能升级的最大等级

    void GetConstellationProps(int32 level, std::vector<const ConstellationTemplate*>& list);   // 根据角色的星空图等级，返回模板

    bool CheckHoroscopeLevel(int32 HoroscopeLevel, int32 playerLevel);

    int32 GetMaxBronzeLevel() {return m_max_bronze_level;}
    int32 GetMaxSilverLevel() {return m_max_silver_level;}
    int32 GetMaxGoldLevel() {return m_max_gold_level;}

private:
    std::map<int32, ConstellationTemplate>  m_constellation_map;

    std::vector<HoroscopeLimit>             m_horoscopeLimit;

    int32                                   m_max_horoscope_level;
    //白银，青铜，黄金
    int32                                   m_max_bronze_level;
    int32                                   m_max_silver_level;
    int32                                   m_max_gold_level;
};

#endif
