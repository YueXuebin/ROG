/*
 *	vip时效卡
 */
#ifndef vip_card_config_h__
#define vip_card_config_h__

typedef struct VipCardTemplate
{
    uint32                  id;		            ///< 等级
    uint32                  duration;           ///< 时长，单位月
    uint32                  money_point;        ///< 购买消耗魔石
    uint32                  vip;                ///< 购买后直通的VIP特权等级
    std::string             name;               ///< 名字

    void Clear()
    {
        id                  = 0;
        duration            = 0;
        money_point         = 0;
        vip                 = 0;
        name                = "";
    }
} VipCardTemplate;

class VipCardConfig
{
public:
    VipCardConfig();
    ~VipCardConfig();

    DECLARE_SINGLETON(VipCardConfig)

    bool LoadConfig(const char* path);

    const VipCardTemplate*  GetVipCardTemplate(uint32 cardId);

    uint32 GetVipCardDuration(uint32 cardId);           ///< 获得时长
    uint32 GetVipCardMoneyPoint(uint32 cardId);         ///< 获得购买所需魔石
    uint32 GetVipCardDirectVIP(uint32 cardId);          ///< 获得购买VIP时效卡后直通的VIP特权等级

private:
    std::map<uint32, VipCardTemplate> m_vip_cards;
};

#endif // vip_card_config_h__
