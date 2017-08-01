/******************************************************************************
Copyright (C) Doingame Corporation. All rights reserved.
* @brief 加载游戏配置
******************************************************************************/

#ifndef synthetize_config_h__
#define synthetize_config_h__

// 合成花费
typedef struct SynthetizeCost
{
    int32 lower_item_num;       // 低级道具数量
    int32 material_item_id;     // 材料道具id
    int32 material_item_num;    // 材料道具数量
    int32 game_point;           // 消耗金币
    int32 critical_ratio;       // 暴击机率

    void Clear()
    {
        lower_item_num = 0;
        material_item_id = 0;
        material_item_num = 0;
        game_point = 0;
        critical_ratio = 0;
    }
}SynthetizeCost;

// 合成宝石
typedef struct SynthetizeJewel
{
    int32 level;
    std::vector<SynthetizeCost> list;
}SynthetizeJewel;

class SynthetizeConfig
{
public:
    SynthetizeConfig();
    virtual ~SynthetizeConfig();

    DECLARE_SINGLETON(SynthetizeConfig)

    bool LoadConfig(const char* path);                  ///< 读取配置文件

    bool CanUseJewel(int32 jewel_level, int32 player_level);            ///< 使用宝石

    //bool CanSynthetizeJewel(int32 jewel_level, int32 player_level);     ///< 合成宝石

    SynthetizeJewel* GetSynthJewel(int32 level);        ///< 根据宝石的等级获得合成率和花费

    uint32 GetCirtTime() {return cirt_time;}
    std::string GetPublicString() {return m_public;}


private:

    std::map<int32, int32>  m_use_jewel_level;                  ///< 使用宝石的角色等级限制

    //std::map<int32, int32>  m_synthetize_jewel_level;           ///< 合成宝石的角色等级限制

    typedef std::map<int32, SynthetizeJewel> SynthJewelMap;     ///< [等级,合成宝石列表]
    SynthJewelMap m_synthjewel_map;                             ///< 合成宝石映射表

    uint32 cirt_time;                                          ///<.暴击多时间发一次公告
    std::string m_public;                                      ///<.公告内容
};

#endif
