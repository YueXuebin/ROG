/******************************************************************************
Copyright (C) Doingame Corporation. All rights reserved.
* @brief 加载游戏配置
******************************************************************************/

#ifndef vigor_config_h__
#define vigor_config_h__

#include <map>

class Player;

typedef struct MealTimeTemplate
{
    void Clear()
    {
        start_time = "";
        end_time = "";
        add_vigor = 0;
    }

    std::string     start_time;  // 饭点开始时间
    std::string     end_time;   //  饭点结束时间
    uint32  add_vigor;  //  饭点增加体力数
}MealTimeTemplate;

class VigorConfig
{
public:
    VigorConfig();
    virtual ~VigorConfig();

    DECLARE_SINGLETON(VigorConfig)

    void Clear();                               ///< 清空

    bool LoadConfig(const char* path);          ///< 加载配置文件

    uint32 GetVigorInterval() { return m_vigor_interval; }      // 获得体力刷新时间间隔
    int32  GetVigorAddVal() { return m_vigor_addval; }          // 获得体力增加值

    int32 GetMaxVigor(Player* player);          ///< 获得最大等级

    int32 GetMealVigorStartLevel() {return m_meal_vigor_start_level;}   // 获得开始领取饭点的等级

    const std::map<uint32, MealTimeTemplate>& GetMealTime() { return m_mealtime_map; }	//获取所有饭点返体力时间

private:
    std::map<int32, int32>  m_vigor_map;        ///< 等级，最大体力
    int32                   m_max_level;        ///< 最大等级对应的

    uint32                  m_vigor_interval;   ///< 添加体力间隔
    int32                   m_vigor_addval;     ///< 添加体力数值

    std::map<uint32, MealTimeTemplate>  m_mealtime_map;        ///< 所有的饭点时间

    int32                   m_meal_vigor_start_level;         ///< 开始领取饭点的等级
};

#endif
