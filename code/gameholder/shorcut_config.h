#ifndef shorcut_config_h__
#define shorcut_config_h__

typedef std::vector<REWARD_TABLE>  ShorcutConfigList;

class ShorcutConfig
{
public:
    ShorcutConfig(void);
    ~ShorcutConfig(void);

    DECLARE_SINGLETON(ShorcutConfig) 

    bool LoadConfig(const char* path);
    const REWARD_TABLE* GetRewardList(int32 type);
private:
    bool LoadShorcutConfig(const char* path);                                          // ��ȡ�µĻ�������
    void LoadFashionItemList(std::vector<REWARD_ITEM>* lsit, const std::string& base_value);     // ����������Ʒ

    ShorcutConfigList  m_rewardList;
};
#endif