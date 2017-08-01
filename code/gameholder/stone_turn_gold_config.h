/******************************************************************************
Copyright (C) Doingame Corporation. All rights reserved.
* @brief 加载游戏配置
******************************************************************************/

#ifndef stone_turn_gold_config_h__
#define stone_turn_gold_config_h__

class Player;

class StoneTurnGoldCost
{
public:
    void Clear()
    {
        times = 0;
        money_point = 0;
    }

    int32 times;
    int32 money_point;
};

class StoneTurnGoldConfig
{
public:
    StoneTurnGoldConfig();
    virtual ~StoneTurnGoldConfig();

    DECLARE_SINGLETON(StoneTurnGoldConfig)

    bool LoadConfig(const char* path);

    bool LoadStoneTurnGoldConfig(const char* path);

    int32 GetCostMoneyPoint(int32 times);

    int32 CalAllCostMoneyPoint(int32 times);

    int32 GetMaxLen();

private:
    std::vector<StoneTurnGoldCost> m_stoneTurnGoldCost;
};

#endif
