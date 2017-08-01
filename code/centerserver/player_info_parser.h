#ifndef parse_player_info_job_
#define parse_player_info_job_
class DBRecorder;
class Player;

//解析玩家信息
class PlayerInfoParser
{
    PlayerInfoParser() {}
    ~PlayerInfoParser() { }

    DECLARE_SINGLETON(PlayerInfoParser)

    uint16 ParsePlayerInfo(DBRecorder&res, uint64 loginID);          //解析玩家信息

private:

    void ParseGuildWelfareInfo(Json::Value& json_val, GUILD_WELFARE_INFO& guildWelfareInfo);

    void ParseCityFightInfo( Json::Value& json_val, CITY_FIGHT_PLAYER_INFO& cityFightInfo );

    void ParseFcmInfo(Json::Value& json_val, FCM_INFO& fcmInfo);

    void ParseVigorInfo(Json::Value& json_val,VIGOR_INFO& vigorInfo);

    void ParseVipInfo(Json::Value& json_val, VIP_INFO& vipInfo);

    void ParseOpenSystemInfo(Json::Value& json_val, OPEN_SYSTEM_INFO& openSystemInfo);

    void ParseOffLineVigorInfo(Json::Value& json_val,OFF_LINE_VIGOR_INFO& vigorInfo);

};

#endif
