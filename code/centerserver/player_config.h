/*
 *	角色创建时的初始信息
 */
#ifndef player_config_h__
#define player_config_h__

class PlayerConfig
{
public:
    PlayerConfig();
    ~PlayerConfig();

    DECLARE_SINGLETON(PlayerConfig)

    bool LoadConfig();

    void FillCreateProperyInfo(PROPERTY_INFO& info);
};
#endif // player_config_h__
