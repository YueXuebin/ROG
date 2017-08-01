/*
*	结构化对象的json读与写
*/
#ifndef json_helper_h__
#define json_helper_h__

void read_REWARD_ITEM(REWARD_ITEM& item, const Json::Value& jsonValue);
void write_REWARD_ITEM(const REWARD_ITEM& item, Json::Value& jsonValue);

void read_EVIL_ITEM(CREATURE_EVIL& item, const Json::Value& jsonValue);
void write_EVIL_ITEM(const CREATURE_EVIL& item, Json::Value& jsonValue);

void read_REWARD_TABLE(REWARD_TABLE& table, const Json::Value& jsonValue);
void write_REWARD_TABLE(const REWARD_TABLE& table, Json::Value& jsonValue);

void read_DUNGEON_SCORE_INFO(DUNGEON_SCORE_INFO& info, const Json::Value& jsonValue);
void write_DUNGEON_SCORE_INFO(const DUNGEON_SCORE_INFO& info, Json::Value& jsonValue);

void read_BATTLE_POWER_INFO(BATTLE_POWER_INFO& info, const Json::Value& jsonValue);
void write_BATTLE_POWER_INFO(const BATTLE_POWER_INFO& info, Json::Value& jsonValue);

void read_OFFLINE_EQUIP_INFO(OFFLINE_EQUIP_INFO& info, const Json::Value& jsonValue);
void write_OFFLINE_EQUIP_INFO(const OFFLINE_EQUIP_INFO& info, Json::Value& jsonValue);

void write_VIP_INFO_BUFF(VIP_INFO& vip_info,std::string& vip_str);                                              // 写 VIP 信息Buff

//void write_OPEN_SYSTEM_INFO(OPEN_SYSTEM_INFO& open_system_info,std::string& open_system_str);                   // 写 开启系统 信息Buff

void write_GUILD_WELFARE_INFO(GUILD_WELFARE_INFO& guild_welfare_info, std::string& guild_welfare_str);           // 公会福利信息

void write_CITY_FIGHT_INFO( CITY_FIGHT_PLAYER_INFO& city_fight_info, std::string& info_txt);                     // 城市争夺战信息

void  write_FCM_INFO(FCM_INFO& fcm_info,std::string& fcm_str);                                                  // 防沉迷信息

void write_VIGOR_INFO(VIGOR_INFO& vigor_info, std::string& vigor_str);                                         // 写 体力信息 buff

void write_OFFLINE_VIGOR_INFO(OFF_LINE_VIGOR_INFO& offvigor_info, std::string& offvigor_str);                  //离线玩家的信息

void read_ENEMY_INFO(ENEMY_INFO& record, const Json::Value& jsonValue);
void write_ENEMY_INFO(const ENEMY_INFO& record, Json::Value& jsonValue);

//void read_SOUL_DUNGEON_RAIDS(std::vector<RAIDS_SOUL_DUNGEON_INFO>& list, const std::string& txt);
//void write_SOUL_DUNGEON_RAIDS(const std::vector<RAIDS_SOUL_DUNGEON_INFO>& list, std::string& txt);

void read_GUILD_ADDITION(GUILD_ADDITION& addition, const Json::Value& jsonValue);
void write_GUILD_ADDITION(const GUILD_ADDITION& addition, Json::Value& jsonValue);

void read_GUILD_PLAYER_ADDITION(GUILD_PLAYER_ADDITION& addition, const Json::Value& jsonValue);
void write_GUILD_PLAYER_ADDITION(const GUILD_PLAYER_ADDITION& addition, Json::Value& jsonValue);

void read_PERSONAL_TURNTABLE_ID(uint32& id, const std::string& txt);
void write_PERSONAL_TURNTABLE_ID(uint32 id, std::string& txt);

void read_SYSTEM_SETTING_INFO(SYSTEM_SETTING_INFO& info, const Json::Value& jsonValue);
void write_SYSTEM_SETTING_INFO(const SYSTEM_SETTING_INFO& info, Json::Value& jsonValue);

void read_TaskGameInfo(TASK_GAME_INFO& info, const Json::Value& jsonValue);
void write_TaskGameInfo(const TASK_GAME_INFO& info, Json::Value& jsonValue);

void read_TaskFestivalInfo(TASK_FESTIVAL_INFO& info, const Json::Value& jsonValue);
void write_TaskFestivalInfo(const TASK_FESTIVAL_INFO& info, Json::Value& jsonValue);

void read_TaskGuildInfo(TASK_GUILD_INFO& info, const Json::Value& jsonValue);
void write_TaskGuildlInfo(const TASK_GUILD_INFO& info, Json::Value& jsonValue);

void read_ENEMYINFO(ENEMY_INFO& info, const Json::Value& jsonValue);
void write_ENEMYINFO(const ENEMY_INFO& info, Json::Value& jsonValue);

void read_TaskRecord(TASK_RECORD& info, const Json::Value& jsonValue);
void write_TaskRecord(const TASK_RECORD& info, Json::Value& jsonValue);

void read_FestivalRecord(FESTIVAL_RECORD& info, const Json::Value& jsonValue);
void write_FestivalRecord(const FESTIVAL_RECORD& info, Json::Value& jsonValue);

void read_EnemyRecord(ENEMY_RECORD& info, const Json::Value& jsonValue);
void write_EnemyRecord(const ENEMY_RECORD& info, Json::Value& jsonValue);

uint32  toUint32(const Json::Value& val);
int32   toInt32(const Json::Value& val);
std::string toString(const Json::Value& val);

void write_TaskTarget(TASK_TEMPLATE* taskIt, Json::Value& task_json);
void write_TaskTarget_ValueNum(TASK_TARGET* target, std::string& target_value);
void write_TaskTarget_Num(TASK_TARGET* target, std::string& target_value);
void write_TaskTarget_1(TASK_TARGET* target, std::string& target_value);
void write_TaskTarget_3(TASK_TARGET* target, std::string& target_value);
void write_TaskTarget_4(TASK_TARGET* target, std::string& target_value);
void write_TaskTarget_6(TASK_TARGET* target, std::string& target_value);
void write_TaskTarget_17(TASK_TARGET* target, std::string& target_value);

#endif // json_helper_h__
