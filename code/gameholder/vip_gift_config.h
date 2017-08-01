#ifndef vip_gift_config_h
#define vip_gift_config_h
class Player;
class VipGiftConfig
{
public:
    VipGiftConfig(void);
    ~VipGiftConfig(void);

    DECLARE_SINGLETON(VipGiftConfig)

    void Clear();                               ///< ���

    bool LoadConfig(const char* path);          ///< ���������ļ�

    int32 GetRewardItemList(int32 level);

    bool AddVipGift(Player* player, int32 level);

private:
    typedef std::vector<REWARD_ITEM> RewardItemList;
    std::map<uint32, REWARD_TABLE> vip_git_list;

};
#endif
