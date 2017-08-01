#ifndef giftcode_config_h__
#define giftcode_config_h__


class GiftCodeConfig
{
public:
    typedef std::map< uint32, std::vector<REWARD_ITEM> > RewardMap;
    GiftCodeConfig();
    ~GiftCodeConfig();
    DECLARE_SINGLETON(GiftCodeConfig);

    bool LoadConfig(const char *fileName);

    bool GetRewards(uint32 type, std::vector<REWARD_ITEM>& rewards);

    uint32 GetMaxGiftCodeType() { return m_maxGiftCodeType; }

private:
    RewardMap       m_rewards;
    uint32          m_maxGiftCodeType;
};


#endif  //giftscode_config_h__