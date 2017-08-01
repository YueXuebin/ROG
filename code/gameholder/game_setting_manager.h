#ifndef game_setting_manager_h__
#define game_setting_manager_h__

/*
 *	游戏参数配置管理器(可通过gmt动态修改参数)
 */
class GameSettingManager
{
public:
    GameSettingManager(void);
    ~GameSettingManager(void);

    DECLARE_SINGLETON(GameSettingManager)

    void OnRecv(uint32 msgID, void* data);

    void OnUpdateGameSettingInfo(void* data);                           // 通知设置刷新
    bool UpdateGameSettingInfo( GAME_SETTING_INFO& gameSettingInfo );   // 刷新设置

    // 主城频道人数
    void    SetCityRoomNum(int32 num) {m_CityRoomNum = num;}
    uint32  GetCityRoomNum(){ return m_CityRoomNum;}

    //------------------------void field--------------------

    // 虚空掉落宝箱时间
    uint32  GetVoidFieldDropChestTime() {return m_void_field_drop_chest_time;}

    // 虚空珍兽所需人数
    int32   GetVoidFieldRegularBossPlayerNum() { return m_void_field_boss_player_num; }

    // 虚空月黑所需人数
    int32   GetVoidFieldRegularMoonlessPlayerNum() { return m_void_field_moonless_player_num; }

    // 虚空所需世界等级
    int32   GetVoidFieldWorldLevelNeed() { return m_void_field_world_level_need; }

    // 虚空幻境boss频道数量
    int32   GetVoidFieldBossChannelNum() {return m_void_field_boss_channel_num;}

    // 虚空幻境月黑频道数量
    int32   GetVoidFieldMoonlessChannelNum() {return m_void_field_moonless_channel_num;}

    // 虚空幻境boss cd
    int32   GetVoidFieldBossCDTime() {return m_void_field_boss_cd_time;}

    // 虚空幻境月黑 cd
    int32   GetVoidFieldMoonlessCDTime() {return m_void_field_moonless_cd_time;}

    //------------------------void field solo-----------------------------

    // 虚空SOLO所需世界等级
    int32   GetVoidFieldSoloWorldLevelNeed() { return m_void_field_solo_world_level_need; }

    // 虚空SOLO掉落宝箱时间
    uint32  GetVoidFieldSoloDropChestTime() {return m_void_field_solo_drop_chest_time;}

    // 虚空SOLO珍兽所需人数
    int32   GetVoidFieldSoloRegularBossPlayerNum() { return m_void_field_solo_boss_player_num; }

    // 虚空幻境SOLO boss频道数量
    int32   GetVoidFieldSoloBossChannelNum() {return m_void_field_solo_boss_channel_num;}

    // 虚空幻境SOLO boss cd
    int32   GetVoidFieldSoloBossCDTime() {return m_void_field_solo_boss_cd_time;}

    //---------------------------------------------------------------------
    // 世界boss频道人数
    void    SetWorldBossMaxPlayer(uint32 num) { m_world_boss_max_player = num; }
    uint32  GetWorldBossMaxPlayer() { return m_world_boss_max_player; }

    // 圣剑争锋频道人数
    void    SetFactionPKMaxPlayer(uint32 num) {m_faction_pk_max_player = num;}
    uint32  GetFactionPKMaxPlayer() {return m_faction_pk_max_player;}

    // 修罗场频道人数
    void    SetAshuraMaxPlayer(uint32 num) {m_ashura_max_player = num;}
    uint32  GetAshuraMaxPlayer() {return m_ashura_max_player;}

    // 开服时间
    void    SetServerOpenTime(uint32 time);
    uint32  GetServerOpenTime() {return m_server_open_time;}
    uint32  GetServerOpenToDayTime() {return m_server_open_to_day_time;}
    
    //聊天设置
    void    SetChatWorldLevel(uint32 num) {m_chat_world_level = num;}
    uint32  GetChatWorldLevel() {return m_chat_world_level;}

    void    SetChatTeamLevel(uint32 num) {m_chat_team_level = num;}
    uint32  GetChatTeamLevel() {return m_chat_team_level;}

    void    SetChatPrivateLevel(uint32 num) {m_chat_private_level = num;}
    uint32  GetChatPrivateLevel() {return m_chat_private_level;}

    void    SetChatHornLevel(uint32 num) {m_chat_horn_level = num;}
    uint32  GetChatHornLevel() {return m_chat_horn_level;}

    void    SetChatGuildLevel(uint32 num) {m_chat_guild_level = num;}
    uint32  GetChatGuildLevel() {return m_chat_guild_level;}

    //游戏屏幕大小参数
    void SetScreenParameter(int32 parma) {m_screen_parameter = parma;}
    int32 GetScreenParameter() {return m_screen_parameter;}

    //设置屏幕高中低的配置
    int32 GetQualityState(){return m_quality_state;}
    void SetQualityState(int32 parma) {m_quality_state = parma;}


    // VIP限购时间设置
    void SetVIPLimitedPurchasingTimeStart(uint32 val) {m_vip_limited_purchasing_time_start = val;}
    uint32 GetVIPLimitedPurchasingTimeStart() {return m_vip_limited_purchasing_time_start;}

    void SetVIPLimitedPurchasingTimeEnd(uint32 val) {m_vip_limited_purchasing_time_end = val;}
    uint32 GetVIPLimitedPurchasingTimeEnd() {return m_vip_limited_purchasing_time_end;}
     
    void SetMultiserverEnterNum(uint32 val) {m_multiserver_enter_num = val;}
    uint32 GetMultiserverEnterNum() {return m_multiserver_enter_num;}

    void SetMultiserverYesterdayNum(uint32 val) {m_multiserver_yesterday_num = val;}
    uint32 GetMultiserverYesterdayNum() {return m_multiserver_yesterday_num;}

    void SetMultiserverYesterday3V3Num(uint32 val) { m_multiserver_yesterday_3v3_num = val; }
    uint32 GetMultiserverYesterday3V3Num() { return m_multiserver_yesterday_3v3_num; }

    void UpdateSettingData(int32 id, Json::Value& jsonvalue);
    void GetJsonValueById(int32 id, Json::Value& json);

    GAME_SETTING_LIST& GetGameSettingList();
private:
    void    OnAsk(GMT_MSG* data);           // 查询
    void    OnSet(GMT_MSG* data);           // 设置

    void    GmtUpdateSetting(const GAME_SETTING_INFO& settingInfo);

private:
    uint32  m_CityRoomNum;                          // 主城频道人数
    uint32   m_world_boss_max_player;                // 世界boss频道最大人数
    uint32  m_faction_pk_max_player;                // 圣剑争锋频道最大人数
    uint32  m_ashura_max_player;                    // 修罗场频道最大人数

    // void field
    int32   m_void_field_drop_chest_time;           // 设置虚空幻境的掉落宝箱时间，多少秒后，概率自动减少
    int32   m_void_field_boss_player_num;           // 珍兽状态开启人数
    int32   m_void_field_moonless_player_num;       // 月黑状态开启人数
    int32   m_void_field_world_level_need;          // 进入虚空所需的世界等级
    int32   m_void_field_boss_channel_num;          // 虚空幻境boss频道开启数量
    int32   m_void_field_moonless_channel_num;      // 虚空幻境月黑频道开启数量
    int32   m_void_field_boss_cd_time;              // 虚空幻境boss cd
    int32   m_void_field_moonless_cd_time;          // 虚空幻境月黑 cd

    // void field solo
    int32   m_void_field_solo_world_level_need;
    int32   m_void_field_solo_drop_chest_time;
    int32   m_void_field_solo_boss_player_num;
    int32   m_void_field_solo_boss_channel_num;
    int32   m_void_field_solo_boss_cd_time;

    uint32  m_server_open_time;                     // 开服时间(时间戳)
    uint32  m_server_open_to_day_time;              // 开服时间第二天的零点

    //聊天设置
    uint32 m_chat_world_level;                      // 世界频道等级
    uint32 m_chat_team_level;                       // 组队频道等级
    uint32 m_chat_private_level;                    // 私聊频道等级
    uint32 m_chat_horn_level;                       // 喇叭频道等级
    uint32 m_chat_guild_level;                      // 公会频道等级

    int32 m_screen_parameter;                       // 游戏屏幕参数
    int32 m_quality_state;                          // 游戏屏幕分辨率

    // VIP 限购时间
    uint32 m_vip_limited_purchasing_time_start;     // 开始时间
    uint32 m_vip_limited_purchasing_time_end;       // 结束时间

    uint32 m_multiserver_enter_num;                 // 跨服1v1进入次数
    uint32 m_multiserver_yesterday_num;             // 昨天战斗领奖次数
    uint32 m_multiserver_yesterday_3v3_num;         // 昨天战斗3v3领奖次数


    std::map<int32,Json::Value> m_gamesettingList;
    GAME_SETTING_LIST m_GameSettingList;


};

#endif
