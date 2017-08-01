#ifndef vip_gift_config_h
#define vip_gift_config_h
class Player;
class VipGiftConfig
{
public:
    VipGiftConfig(void);
    ~VipGiftConfig(void);

    DECLARE_SINGLETON(VipGiftConfig)

    void Clear();                               ///< 清空

    bool LoadConfig(const char* path);          ///< 加载配置文件

    int32 GetRewardItemList(int32 level);

    bool AddVipGift(Player* player, int32 level);

private:
    typedef std::vector<REWARD_ITEM> RewardItemList;
    std::map<uint32, REWARD_TABLE> vip_git_list;

};
#endif
