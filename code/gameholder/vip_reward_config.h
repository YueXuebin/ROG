/******************************************************************************
Copyright (C) Doingame Corporation. All rights reserved.
* @brief VIP¿Ò∞¸≈‰÷√
******************************************************************************/

#ifndef vip_reward_config_h__
#define vip_reward_config_h__

#include <vector>

//struct ItemTemplate;

typedef struct VipReward
{
    void Clear()
    {
        vip             = 0;
        items.clear();
    }

    uint32      vip;     ///< VIP Ãÿ»®
    std::vector<REWARD_ITEM> items;
}VipReward;

class Player;

class VipRewardConfig
{
public:
    VipRewardConfig();
    virtual ~VipRewardConfig();

    DECLARE_SINGLETON(VipRewardConfig)

    void Clear();

    bool LoadConfig(const char* path);

    const VipReward* GetVipReward(uint32 vip);


private:
    std::map<uint32, VipReward>  m_vip_reward_map;
};

#endif
