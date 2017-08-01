#include "commonlib_pch.h"
#include "json_helper.h"

int32   toInt32(const Json::Value& val)
{
    if(val.isNumeric())
    {
        int32 ret = 0;
        if(val.isInt())
            return val.asInt();
        else if(val.isUInt())
            return val.asUInt();
        else if(val.isDouble())
        {
            ret = int32(val.asDouble());
            return ret > 0 ? ret : 0;
        }
        else
            return val.asUInt();
    }
    else if(val.isString())
    {
        return SDAtoi(val.asString().c_str());
    }
    else if(val.isNull())
        return 0;

    CnAssert(false);
    return 0;
}

uint32 toUint32(const Json::Value& val)
{
    if(val.isNumeric())
    {
        int32 ret = 0;
        if(val.isUInt())
            return val.asUInt();
        else if(val.isInt())
        {
            ret = val.asInt();
            return ret > 0 ? ret : 0;
        }
        else if(val.isDouble())
        {
            ret = int32(val.asDouble());
            return ret > 0 ? ret : 0;
        }
        else
            return val.asUInt();
    }
    else if(val.isString())
    {
        return Crown::SDAtou(val.asString().c_str());
    }
    else if(val.isNull())
        return 0;

    CnAssert(false);
    return 0;
}

std::string toString(const Json::Value& val)
{
    if(val.isNumeric())
    {
        char pstr[100];
        Crown::SDItoa(pstr, 100, val.asInt());
        return pstr;
    }
    else if(val.isString())
    {
        return val.asString();
    }
    else if(val.isNull())
        return "";

    CnAssert(false);
    return "";
}

void read_REWARD_ITEM( REWARD_ITEM& item, const Json::Value& jsonValue )
{
    item.Clear();

    //item.flag = jsonValue.get("flag", 0).asUInt();    // VIP等级获得标记

    // 物品类型(游戏币、物品)
    //if(jsonValue["type"].isString())
    //    item.type = SDAtoi(jsonValue["type"].asString().c_str());
    //else
    //    item.type = jsonValue["type"].asInt();

    // 物品ID
    if(jsonValue["id"].isString())
        item.id = SDAtoi(jsonValue["id"].asString().c_str());
    else
        item.id = jsonValue["id"].asUInt();

    // 物品数量
    if(jsonValue["num"].isString())
        item.num = SDAtoi(jsonValue["num"].asString().c_str());
    else
        item.num = jsonValue["num"].asInt();
}

void write_REWARD_ITEM( const REWARD_ITEM& item, Json::Value& jsonValue )
{
    jsonValue.clear();
    //jsonValue["type"]   = item.type;
    jsonValue["id"]     = item.id;
    jsonValue["num"]    = item.num;
    //jsonValue["flag"]   = item.flag;
}

void read_EVIL_ITEM(CREATURE_EVIL& item, const Json::Value& jsonValue)
{
    item.Clear();

    // 对象ID
    if (jsonValue["object_id"].isString())
        item.object_id = SDAtoi(jsonValue["object_id"].asString().c_str());
    else
        item.object_id = jsonValue["object_id"].asUInt();

    // 对象罪恶值
    if (jsonValue["evil"].isString())
        item.evil = SDAtoi(jsonValue["evil"].asString().c_str());
    else
        item.evil = jsonValue["evil"].asInt();
}

void write_EVIL_ITEM(const CREATURE_EVIL& item, Json::Value& jsonValue)
{
    jsonValue.clear();
    jsonValue["object_id"] = item.object_id;
    jsonValue["evil"] = item.evil;
}

void read_REWARD_TABLE( REWARD_TABLE& table, const Json::Value& jsonValue )
{
    table.Clear();
    uint32 item_count = jsonValue["items"].size();
    for(uint32 j=0; j<item_count; j++)
    {
        REWARD_ITEM item;
        Json::Value item_json = jsonValue["items"][j];

        read_REWARD_ITEM(item, item_json);

        table.items.push_back(item);
    }

    int32 argreasion = jsonValue["reason_arg"].asInt();
    table.reason_arg = argreasion;
    
    int32 reason = jsonValue["reason"].asInt();
    table.reason = reason;
}

void write_REWARD_TABLE( const REWARD_TABLE& table, Json::Value& jsonValue )
{
    jsonValue.clear();
    for(uint32 j = 0; j < table.items.size(); j++)
    {
        const REWARD_ITEM& item = table.items[j];
        Json::Value js_item;

        write_REWARD_ITEM(item, js_item);

        jsonValue["items"].append(js_item);
    }

    jsonValue["reason"] = table.reason;
    jsonValue["reason_arg"] = table.reason_arg;
}


void read_DUNGEON_SCORE_INFO(DUNGEON_SCORE_INFO& info, const Json::Value& jsonValue)
{
    info.Clear();
    SCORE_MEMBER_INFO scoreMemberInfo;

    info.dungeon_id = jsonValue["dungeon_id"].asUInt();
    info.cost_time = jsonValue["cost_time"].asUInt();
    uint32 member_count = jsonValue["member_list"].size();
    for(uint32 i=0; i<member_count; ++i)
    {
        scoreMemberInfo.player_name = jsonValue["member_list"][i]["player_name"].asString();
        scoreMemberInfo.level = jsonValue["member_list"][i]["level"].asInt();
        scoreMemberInfo.vip = jsonValue["member_list"][i]["vip"].asInt();
        info.member_list.push_back(scoreMemberInfo);
    }
}

void write_DUNGEON_SCORE_INFO(const DUNGEON_SCORE_INFO& info, Json::Value& jsonValue)
{
    jsonValue.clear();
    jsonValue["dungeon_id"] = info.dungeon_id;
    jsonValue["cost_time"] = info.cost_time;
    for(uint32 i = 0; i < info.member_list.size(); i++)
    {
        Json::Value js_member;
        //= info.member_list[i];
        js_member["player_name"] = info.member_list[i].player_name;
        js_member["level"] = info.member_list[i].level;
        js_member["vip"] = info.member_list[i].vip;
        jsonValue["member_list"].append(js_member);
    }
}

void read_BATTLE_POWER_INFO( BATTLE_POWER_INFO& info, const Json::Value& jsonValue )
{
    info.Clear();
    info.total      = toUint32(jsonValue["total"]);
    info.base_prop  = toUint32(jsonValue["base_prop"]);
    info.equip      = toUint32(jsonValue["equip"]);
    info.skill      = toUint32(jsonValue["skill"]);
    info.horoscope  = toUint32(jsonValue["horoscope"]);
    info.jewel      = toUint32(jsonValue["jewel"]);
    info.fashion    = toUint32(jsonValue["fashion"]);
    info.title      = toUint32(jsonValue["title"]);
    info.pet_base_prop = toUint32(jsonValue["pet_base_prop"]);
    info.pro_soul   = toUint32(jsonValue["pro_soul"]);
    info.ride_mark  = toUint32(jsonValue["ride_mark"]);
    info.ride_altas = toUint32(jsonValue["ride_altas"]);
    info.wing_base_prop = toUint32(jsonValue["wing_base_prop"]);
    info.wing_altas = toUint32(jsonValue["wing_altas"]);
}

void write_BATTLE_POWER_INFO( const BATTLE_POWER_INFO& info, Json::Value& jsonValue )
{
    jsonValue.clear();
    jsonValue["total"] = info.total;
    jsonValue["base_prop"] = info.base_prop;
    jsonValue["equip"] = info.equip;
    jsonValue["skill"] = info.skill;    
    jsonValue["horoscope"] = info.horoscope;
    jsonValue["jewel"] = info.jewel;
    jsonValue["fashion"] = info.fashion;
    jsonValue["title"] = info.title;
    jsonValue["pet_base_prop"] = info.pet_base_prop;
    jsonValue["pro_soul"] = info.pro_soul;
    jsonValue["ride_mark"] = info.ride_mark;
    jsonValue["ride_altas"] = info.ride_altas;
    jsonValue["wing_base_prop"] = info.wing_base_prop;
    jsonValue["wing_altas"] = info.wing_altas;
}

void read_OFFLINE_EQUIP_INFO( OFFLINE_EQUIP_INFO& info, const Json::Value& jsonValue )
{
    info.Clear();
    info.appear_id  = toUint32(jsonValue["id"]);
    info.enchant1   = toUint32(jsonValue["enchant1"]);
    info.enchant2   = toUint32(jsonValue["enchant2"]);
    info.enchant3   = toUint32(jsonValue["enchant3"]);
    info.cur_slot   = toUint32(jsonValue["cur_slot"]);
    info.max_slot   = toUint32(jsonValue["max_slot"]);
    info.jewel1     = toUint32(jsonValue["jewel1"]);
    info.jewel2     = toUint32(jsonValue["jewel2"]);
    info.jewel3     = toUint32(jsonValue["jewel3"]);
    info.jewel4     = toUint32(jsonValue["jewel4"]);
    info.level      = toUint32(jsonValue["level"]);
    info.quality    = toUint32(jsonValue["quality"]);
    info.star       = toUint32(jsonValue["star"]);
}

void write_OFFLINE_EQUIP_INFO( const OFFLINE_EQUIP_INFO& info, Json::Value& jsonValue )
{
    jsonValue.clear();
    jsonValue["id"] = info.appear_id;
    jsonValue["enchant1"] = info.enchant1;
    jsonValue["enchant2"] = info.enchant2;
    jsonValue["enchant3"] = info.enchant3;
    jsonValue["cur_slot"] = info.cur_slot;
    jsonValue["max_slot"] = info.max_slot;
    jsonValue["jewel1"] = info.jewel1;
    jsonValue["jewel2"] = info.jewel2;
    jsonValue["jewel3"] = info.jewel3;
    jsonValue["jewel4"] = info.jewel4;
    jsonValue["level"] = info.level;
    jsonValue["quality"] = info.quality;
    jsonValue["star"] = info.star;
}

void write_VIP_INFO_BUFF(VIP_INFO& vip_info, std::string& vip_str)
{
    Json::FastWriter writer;
    Json::Value json_val;

    // VIP信息
    // ================================================================================
    json_val.clear();
    json_val["vip_first_ts"] = vip_info.vip_first_ts;
    json_val["vip_ts"] = vip_info.vip_ts;
    json_val["is_vip"] = vip_info.is_vip;
    json_val["vip_card_1_num"] = vip_info.vip_card_1_num;
    json_val["vip_card_2_num"] = vip_info.vip_card_2_num;
    json_val["vip_card_3_num"] = vip_info.vip_card_3_num;
    json_val["is_vip_reward_today"] = vip_info.is_vip_reward_today;
    json_val["vip_gif_flag"] = vip_info.vip_gif_flag;
    json_val["experience_vip_state"] = vip_info.experience_vip_state;
    json_val["growth_point_no_recharge"] = vip_info.growth_point_no_recharge;

    vip_str = writer.write(json_val);
}


//void write_OPEN_SYSTEM_INFO(OPEN_SYSTEM_INFO& open_system_info,std::string& open_system_str)
//{
//    Json::FastWriter writer;
//    Json::Value json_val;
//    Json::Value item;
//
//    // 开启系统功能信息
//    // ================================================================================
//    json_val.clear();
//    for(std::vector<OPEN_SYSTEM_ITEM>::iterator sysIt = open_system_info.list.begin(); 
//        sysIt != open_system_info.list.end(); ++sysIt)
//    {
//        item.clear();
//        item["id"] = sysIt->id;
//        item["v"] = sysIt->version;
//        json_val["sys_item"].append(item);
//    }
//
//    open_system_str = writer.write(json_val);
//}

void write_GUILD_WELFARE_INFO(GUILD_WELFARE_INFO& guild_welfare_info, std::string& guild_welfare_str)
{
    Json::FastWriter writer;
    Json::Value json_val;
    Json::Value item;

    // 公会福利信息
    // ================================================================================
    json_val.clear();
    json_val["reward_welfare_time"] = guild_welfare_info.reward_welfare_time;
    json_val["reward_turntalbe_time"] = guild_welfare_info.reward_turntalbe_time;
    json_val["member_turntable_buy_times"] = guild_welfare_info.member_turntable_buy_times;

    for(std::vector<unsigned int>::iterator it = guild_welfare_info.turntable_item_list.begin(); 
        it != guild_welfare_info.turntable_item_list.end(); ++it)
    {
        item.clear();
        item["id"] = *it;
        json_val["turntable_items"].append(item);
    }

    guild_welfare_str = writer.write(json_val);
}



void write_CITY_FIGHT_INFO( CITY_FIGHT_PLAYER_INFO& city_fight_info, std::string& info_txt)
{
    Json::FastWriter writer;
    Json::Value json_val;
    Json::Value item;

    for (uint32 i = 0; i < city_fight_info.city_list.size(); ++i)
    {
        item["city_id"] = city_fight_info.city_list[i].city_id;
        item["timestamp"] = city_fight_info.city_list[i].timestamp;
        json_val.append(item);
    }

    info_txt = writer.write(json_val);
}


void  write_FCM_INFO(FCM_INFO& fcm_info,std::string& fcm_str)
{
    Json::FastWriter writer;
    Json::Value json_val;
    Json::Value item;
   
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    // 防沉迷信息
    // ================================================================================
    json_val.clear();
    if(now > fcm_info.ingame_time)
    {
        fcm_info.total_ingame_time += now - fcm_info.ingame_time;
        fcm_info.ingame_time = now;
    }

    json_val["fcm_flag"] = fcm_info.fcm_flag;
    json_val["fcm_gain_rate"] = fcm_info.fcm_gain_rate;
    json_val["ingame_time"] = fcm_info.ingame_time;
    json_val["outgame_time"] = fcm_info.outgame_time;
    json_val["total_ingame_time"] = fcm_info.total_ingame_time;
    json_val["total_outgame_time"] = fcm_info.total_outgame_time;
    fcm_str = writer.write(json_val);
}


void write_VIGOR_INFO(VIGOR_INFO& vigor_info, std::string& vigor_str)
{
    Json::FastWriter writer;
    Json::Value json_val;

    // 体力信息
    // ================================================================================
    json_val.clear();
    json_val["last_time"] = vigor_info.last_time;

    vigor_str = writer.write(json_val);
}

void write_OFFLINE_VIGOR_INFO(OFF_LINE_VIGOR_INFO& offvigor_info, std::string& offvigor_str)
{
    Json::FastWriter writer;
    Json::Value json_val;

    // 离线体力信息
    // ================================================================================

    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    //m_off_line_vigor_info.start_time = now;

    json_val.clear();
    json_val["start_time"] = now;
    json_val["b_get"] = offvigor_info.b_get;
    json_val["off_voigor_point"] =offvigor_info.off_voigor_point;
    json_val["count_time"] =offvigor_info.count_time;

    offvigor_str = writer.write(json_val);
}

void read_ENEMY_INFO(ENEMY_INFO& info, const Json::Value& jsonValue)
{
    ENEMY_RECORD record;

    info.evil_point = toUint32(jsonValue["evil_point"]);
    info.update_time = toUint32(jsonValue["update_time"]);

    uint32 count = jsonValue["record"].size();
    for(uint32 i=0; i<count; ++i)
    {
        Json::Value jsonRecord = jsonValue["record"][i];
        record.Clear();

        record.player_id.id_h = toUint32(jsonRecord["player_id_h"]);
        record.player_id.id_l = toUint32(jsonRecord["player_id_l"]);
        record.flag = toUint32(jsonRecord["flag"]);
        record.opt = toUint32(jsonRecord["opt"]);
        record.state = toUint32(jsonRecord["state"]);
        record.record_time = toUint32(jsonRecord["record_time"]);

        info.enemy_list.push_back(record);
    }
}

void write_ENEMY_INFO(const ENEMY_INFO& info, Json::Value& jsonValue)
{
    Json::Value json_record;

    jsonValue["version"] = 1;

    jsonValue["evil_point"] = info.evil_point;
    jsonValue["update_time"] = info.update_time;

    for(uint32 i=0; i<info.enemy_list.size(); i++)
    {
        const ENEMY_RECORD& record = info.enemy_list[i];
        json_record.clear();

        json_record["player_id_h"]    = record.player_id.id_h;
        json_record["player_id_l"]    = record.player_id.id_l;
        json_record["flag"]           = record.flag;
        json_record["opt"]            = record.opt;
        json_record["state"]          = record.state;
        json_record["record_time"]    = record.record_time;

        jsonValue["record"].append(json_record);
    }
}
//
//void read_SOUL_DUNGEON_RAIDS(std::vector<RAIDS_SOUL_DUNGEON_INFO>& list, const std::string& txt)
//{
//    Json::Value jsonValue;
//    Json::Value dungeon_json;
//    Json::Reader reader;
//    RAIDS_SOUL_DUNGEON_INFO raidsInfo;
//
//    list.clear();
//    jsonValue.clear();
//
//    if(!reader.parse(txt, jsonValue, false))
//    {
//        CnError("Parse RAIDS_SOUL_DUNGEON_INFO failed\n");
//        CnAssert(false);
//        return;
//    }
//
//    for(uint32 i=0; i<jsonValue["RAIDS_SOUL_DUNGEON_INFO"].size(); ++i)
//    {
//        dungeon_json = jsonValue["RAIDS_SOUL_DUNGEON_INFO"][i];
//        raidsInfo.dungeon_id = dungeon_json["dungeon_id"].asUInt();
//        raidsInfo.radis_times = dungeon_json["radis_times"].asInt();
//        list.push_back(raidsInfo);
//    }
//}
//
//void write_SOUL_DUNGEON_RAIDS(const std::vector<RAIDS_SOUL_DUNGEON_INFO>& list, std::string& txt)
//{
//    Json::Value jsonValue;
//    Json::Value dungeon_json;
//    Json::FastWriter jsonWriter;
//    RAIDS_SOUL_DUNGEON_INFO dungeonInfo;
//
//    jsonValue.clear();
//
//    for(std::vector<RAIDS_SOUL_DUNGEON_INFO>::const_iterator raidsIt=list.begin();
//            raidsIt!=list.end(); ++raidsIt)
//    {
//        dungeonInfo.Clear();
//        dungeon_json["dungeon_id"] = raidsIt->dungeon_id;
//        dungeon_json["radis_times"] = raidsIt->radis_times;
//        jsonValue["RAIDS_SOUL_DUNGEON_INFO"].append(dungeon_json);
//    }
//
//    txt = jsonWriter.write(jsonValue);
//}

void read_GUILD_ADDITION( GUILD_ADDITION& addition, const Json::Value& jsonValue )
{
    addition.Clear();

    // id
    if(jsonValue["id"].isString())
        addition.id = SDAtoi(jsonValue["id"].asString().c_str());
    else
        addition.id = jsonValue["id"].asInt();

    // 等级
    if(jsonValue["level"].isString())
        addition.level = SDAtoi(jsonValue["level"].asString().c_str());
    else
        addition.level = jsonValue["level"].asInt();

    // 当前值
    if(jsonValue["exp"].isString())
        addition.exp = SDAtoi(jsonValue["exp"].asString().c_str());
    else
        addition.exp = jsonValue["exp"].asUInt();

    // 金币次数
    if(jsonValue["promote_count_gamepoint"].isString())
        addition.promote_count_gamepoint = SDAtoi(jsonValue["promote_count_gamepoint"].asString().c_str());
    else
        addition.promote_count_gamepoint = jsonValue["promote_count_gamepoint"].asInt();

    // 魔石次数
    if(jsonValue["promote_count_moneypoint"].isString())
        addition.promote_count_moneypoint = SDAtoi(jsonValue["promote_count_moneypoint"].asString().c_str());
    else
        addition.promote_count_moneypoint = jsonValue["promote_count_moneypoint"].asInt();
}

void write_GUILD_ADDITION( const GUILD_ADDITION& addition, Json::Value& jsonValue )
{
    jsonValue.clear();
    jsonValue["id"]             = addition.id;
    jsonValue["level"]          = addition.level;
    jsonValue["exp"]            = addition.exp;
    jsonValue["promote_count_gamepoint"]    = addition.promote_count_gamepoint;
    jsonValue["promote_count_moneypoint"]   = addition.promote_count_moneypoint;
}

void read_GUILD_PLAYER_ADDITION(GUILD_PLAYER_ADDITION& addition, const Json::Value& jsonValue)
{
    addition.Clear();

    // id
    if(jsonValue["id"].isString())
        addition.id = SDAtoi(jsonValue["id"].asString().c_str());
    else
        addition.id = jsonValue["id"].asInt();

    // 金币次数
    if(jsonValue["promote_time_gamepoint"].isString())
        addition.promote_time_gamepoint = SDAtoi(jsonValue["promote_time_gamepoint"].asString().c_str());
    else
        addition.promote_time_gamepoint = jsonValue["promote_time_gamepoint"].asInt();

    // 魔石次数
    if(jsonValue["promote_time_moneypoint"].isString())
        addition.promote_time_moneypoint = SDAtoi(jsonValue["promote_time_moneypoint"].asString().c_str());
    else
        addition.promote_time_moneypoint = jsonValue["promote_time_moneypoint"].asInt();
}

void write_GUILD_PLAYER_ADDITION(const GUILD_PLAYER_ADDITION& addition, Json::Value& jsonValue)
{
    jsonValue.clear();
    jsonValue["id"]                                   = addition.id;
    jsonValue["promote_time_gamepoint"]               = addition.promote_time_gamepoint;
    jsonValue["promote_time_moneypoint"]              = addition.promote_time_moneypoint;
}

void read_PERSONAL_TURNTABLE_ID(uint32& id, const std::string& txt)
{
    Json::Value jsonValue;
    Json::Reader reader;

    jsonValue.clear();

    if(!reader.parse(txt, jsonValue, false))
    {
        CnError("Parse PERSONAL_TURNTABLE_ID failed\n");
        CnAssert(false);
        return;
    }

    id = jsonValue["id"].asUInt();
}

void write_PERSONAL_TURNTABLE_ID(uint32 id, std::string& txt)
{
    Json::Value jsonValue;
    Json::FastWriter jsonWriter;

    jsonValue["id"] = id;

    txt = jsonWriter.write(jsonValue);
}


void read_SYSTEM_SETTING_INFO( SYSTEM_SETTING_INFO& info, const Json::Value& jsonValue )
{
    info.volume         = jsonValue["volume"].asUInt();
    info.se_volume      = jsonValue["se_volume"].asUInt();
    info.show_player    = jsonValue["show_player"].asUInt();
    info.show_pet       = jsonValue["show_pet"].asUInt();
    info.show_shadow    = jsonValue["show_shadow"].asUInt();
    info.show_particle  = jsonValue["show_particle"].asUInt();
    info.mipmap         = jsonValue["mipmap"].asUInt();
    info.ride_pet       = jsonValue["ride_pet"].asUInt();

    int32 fullscreen = jsonValue["fullscreen"].asUInt();
    if(fullscreen == 1)
    {
        info.fullscreen = SYSTEM_SETTING_ENUM_VARIABILITY_LOW;
    }
    else if (fullscreen == 0)
    {
        info.fullscreen = SYSTEM_SETTING_ENUM_VARIABILITY_FULL_SCREEN;
    }
    else 
    {
         info.fullscreen = fullscreen;
    }

    if(jsonValue.isMember("illumination"))
        info.illumination   = jsonValue["illumination"].asInt();
    else
        info.illumination = 1;

    if(jsonValue.isMember("light"))
        info.light  = jsonValue["light"].asInt();
    else
        info.light = 1;

    if(jsonValue.isMember("quality_state"))
        info.quality_state  = jsonValue["quality_state"].asInt();
    else
        info.quality_state = SYSTEM_SETTING_STATE_LOW;

    if (jsonValue.isMember("effect_level"))
        info.effect_level = jsonValue["effect_level"].asInt();
    else
        info.effect_level = 0;  // 默认效果为中

    if (jsonValue.isMember("fog"))
        info.fog = jsonValue["fog"].asInt();
    else
        info.fog = 1;   // 默认雾为开启

    if (jsonValue.isMember("frame"))
        info.frame = jsonValue["frame"].asInt();
    else
        info.frame = 0; // 默认中品质

}

void write_SYSTEM_SETTING_INFO( const SYSTEM_SETTING_INFO& info, Json::Value& jsonValue )
{
    jsonValue["volume"]        = info.volume;
    jsonValue["se_volume"]     = info.se_volume;
    jsonValue["show_player"]   = info.show_player;
    jsonValue["show_pet"]      = info.show_pet;
    jsonValue["show_shadow"]   = info.show_shadow;
    jsonValue["show_particle"] = info.show_particle;
    jsonValue["mipmap"]        = info.mipmap;
    jsonValue["fullscreen"]    = info.fullscreen;
    jsonValue["ride_pet"]      = info.ride_pet;
    jsonValue["light"]         = info.light;
    jsonValue["illumination"]  = info.illumination;
    jsonValue["quality_state"] = info.quality_state;
    jsonValue["effect_level"]  = info.effect_level;
    jsonValue["fog"] = info.fog;
    jsonValue["frame"] = info.frame;
}

void read_TaskGameInfo(TASK_GAME_INFO& info, const Json::Value& jsonValue)
{
    TASK_RECORD task_record;
    uint32 task_num = 0;
    uint32 taskInfo_branch_flag_num = 0;

    // 任务标记
    info.complete_main_task_id = jsonValue["complete_main_task_id"].asUInt();
    info.daily_task_flash_time = jsonValue["daily_task_flash_time"].asUInt();
    info.daily_task_count = jsonValue["daily_task_count"].asUInt();
    info.daily_task_reset_count = jsonValue["daily_task_reset_count"].asUInt();
    info.crusade_task_flash_time = jsonValue["crusade_task_flash_time"].asUInt();
    info.crusade_task_count = jsonValue["crusade_task_count"].asUInt();
    info.guard_task_count = jsonValue["guard_task_count"].asUInt();
    info.guard_buy_count = jsonValue["guard_buy_count"].asUInt();
    info.open_box = jsonValue["open_box"].asUInt();
    info.guard_buy_day_count = jsonValue["guard_buy_day_count"].asInt();

    // 支线任务标记
    taskInfo_branch_flag_num = jsonValue["branch_flag"].size();
    for(uint32 i=0; i<taskInfo_branch_flag_num; ++i)
    {
        info.branch_task_flag.push_back(jsonValue["branch_flag"][i].asUInt());
    }

    task_num = jsonValue["task"].size();
    for(uint32 i=0; i<task_num; ++i)
    {
        task_record.Clear();
        read_TaskRecord(task_record, jsonValue["task"][i]);
        info.list.push_back(task_record);
    }
}

void write_TaskGameInfo(const TASK_GAME_INFO& info, Json::Value& jsonValue)
{
    Json::Value task_record_json;

    jsonValue["version"] = 1;
    jsonValue["complete_main_task_id"] = info.complete_main_task_id;               // 已完成的主线任务号
    jsonValue["daily_task_flash_time"] = info.daily_task_flash_time;               // 每日任务刷新时间
    jsonValue["daily_task_count"] = info.daily_task_count;                         // 每日任务完成次数
    jsonValue["daily_task_reset_count"] = info.daily_task_reset_count;             // 每日任务重置次数
    jsonValue["crusade_task_flash_time"] = info.crusade_task_flash_time;           // 讨伐令任务刷新时间
    jsonValue["crusade_task_count"] = info.crusade_task_count;                     // 讨伐令任务完成次数
    jsonValue["guard_task_count"] = info.guard_task_count;                         // 护卫任务完成次数
    jsonValue["guard_buy_count"] = info.guard_buy_count;                           // 护卫任务购买次数
    jsonValue["open_box"] = info.open_box;                                         // 领取宝箱
    jsonValue["guard_buy_day_count"] = info.guard_buy_day_count;                   // 护卫每天购买的总的次数

    // 支线任务标记
    for(std::vector<uint32>::const_iterator flagIt = info.branch_task_flag.begin(); 
        flagIt != info.branch_task_flag.end(); ++flagIt)
    {
        jsonValue["branch_flag"].append(*flagIt);
    }

    // 记录已接任务列表
    for(std::vector<TASK_RECORD>::const_iterator taskIt = info.list.begin(); 
        taskIt != info.list.end(); ++taskIt)
    {
        task_record_json.clear();
        write_TaskRecord(*taskIt, task_record_json);
        jsonValue["task"].append(task_record_json);
    }
}

void read_TaskFestivalInfo(TASK_FESTIVAL_INFO& info, const Json::Value& jsonValue)
{
    FESTIVAL_RECORD festive_record;
    TASK_RECORD task_record;
    uint32 festive_num = 0;
    uint32 task_num = 0;

    festive_num = jsonValue["festival"].size();
    for(uint32 i=0; i<festive_num; ++i)
    {
        festive_record.Clear();
        read_FestivalRecord(festive_record, jsonValue["festival"][i]);
        info.festival_list.push_back(festive_record);
    }

    task_num = jsonValue["task"].size();
    for(uint32 i=0; i<task_num; ++i)
    {
        task_record.Clear();
        read_TaskRecord(task_record, jsonValue["task"][i]);
        info.task_list.push_back(task_record);
    }
}

void write_TaskFestivalInfo(const TASK_FESTIVAL_INFO& info, Json::Value& jsonValue)
{
    Json::Value record_json;

    for(std::vector<FESTIVAL_RECORD>::const_iterator festivalIt = info.festival_list.begin(); 
        festivalIt != info.festival_list.end(); ++festivalIt)
    {
        record_json.clear();
        write_FestivalRecord(*festivalIt, record_json);
        jsonValue["festival"].append(record_json);
    }

    // 记录已接任务列表
    for(std::vector<TASK_RECORD>::const_iterator taskIt = info.task_list.begin(); 
        taskIt != info.task_list.end(); ++taskIt)
    {
        record_json.clear();
        write_TaskRecord(*taskIt, record_json);
        jsonValue["task"].append(record_json);
    }
}

void read_TaskGuildInfo(TASK_GUILD_INFO& info, const Json::Value& jsonValue)
{
    info.guild_sroces       = jsonValue["guild_sroces"].asUInt();
    info.guild_task_count   = jsonValue["guild_task_count"].asUInt();

    for(uint32 i = 0; i < jsonValue["open_box_list"].size(); ++i)
    {
        info.open_box_list.push_back(jsonValue["open_box_list"][i].asUInt());
    }
}

void write_TaskGuildlInfo(const TASK_GUILD_INFO& info, Json::Value& jsonValue)
{
    Json::Value openBoxIt;

    jsonValue["guild_sroces"]       = info.guild_sroces;
    jsonValue["guild_task_count"]   = info.guild_task_count;

    for(std::vector<int32>::const_iterator boxIt = info.open_box_list.begin();
        boxIt != info.open_box_list.end(); ++boxIt)
    {
        jsonValue["open_box_list"].append(*boxIt);
    }
}

void read_ENEMYINFO(ENEMY_INFO& info, const Json::Value& jsonValue)
{
    info.evil_point = jsonValue["evil_point"].asInt();
    info.update_time = jsonValue["update_time"].asUInt();

    ENEMY_RECORD enemy_record;
    for (uint32 i = 0; i < jsonValue["enemy_list"].size(); ++i)
    {
        enemy_record.Clear();
        read_EnemyRecord(enemy_record, jsonValue["enemy_list"][i]);
        info.enemy_list.push_back(enemy_record);
    }
}

void write_ENEMYINFO(const ENEMY_INFO& info, Json::Value& jsonValue)
{
    jsonValue["evil_point"] = info.evil_point;
    jsonValue["update_time"] = info.update_time;

    Json::Value record_json;
    for (std::vector<ENEMY_RECORD>::const_iterator recordIt = info.enemy_list.begin(); recordIt != info.enemy_list.end(); ++recordIt)
    {
        record_json.clear();
        write_EnemyRecord(*recordIt, record_json);
        jsonValue["enemy_list"].append(record_json);
    }
}

void read_TaskRecord(TASK_RECORD& info, const Json::Value& jsonValue)
{
    TASK_TARGET_RECORD target_record;
    uint32 target_num = 0;

    info.task_id = jsonValue["id"].asUInt();
    info.state = jsonValue["state"].asUInt();
    info.quality = jsonValue["quality"].asUInt();
    info.loop_num = jsonValue["loop_num"].asInt();
    info.position = jsonValue["position"].asInt();
    target_num = jsonValue["target"].size();
    for(uint32 i=0; i<target_num; ++i)
    {
        target_record.Clear();
        target_record.id = jsonValue["target"][i]["id"].asUInt();
        target_record.num = jsonValue["target"][i]["num"].asInt();
        target_record.max_num = jsonValue["target"][i]["max_num"].asInt();
        info.list.push_back(target_record);
    }
}

void write_TaskRecord(const TASK_RECORD& info, Json::Value& jsonValue)
{
    Json::Value target;

    jsonValue["id"] = info.task_id;
    jsonValue["state"] = info.state;
    jsonValue["quality"] = info.quality;
    jsonValue["loop_num"] = info.loop_num;
    jsonValue["position"] = info.position;
    for(std::vector<TASK_TARGET_RECORD>::const_iterator targetIt = info.list.begin(); 
        targetIt != info.list.end(); ++targetIt)
    {
        target.clear();
        target["id"] = targetIt->id;
        target["num"] = targetIt->num;
        target["max_num"] = targetIt->max_num;
        jsonValue["target"].append(target);
    }
}

void read_FestivalRecord(FESTIVAL_RECORD& info, const Json::Value& jsonValue)
{
    info.festival_id = jsonValue["id"].asUInt();
    info.state = jsonValue["state"].asUInt();
}

void write_FestivalRecord(const FESTIVAL_RECORD& info, Json::Value& jsonValue)
{
    jsonValue["id"] = info.festival_id;
    jsonValue["state"] = info.state;
}

void read_EnemyRecord(ENEMY_RECORD& info, const Json::Value& jsonValue)
{
    info.player_id.id_h = jsonValue["player_id"]["id_h"].asUInt();
    info.player_id.id_l = jsonValue["player_id"]["id_l"].asUInt();
    info.flag = jsonValue["flag"].asUInt();
    info.opt = jsonValue["opt"].asUInt();
    info.record_time = jsonValue["record_time"].asUInt();
}

void write_EnemyRecord(const ENEMY_RECORD& info, Json::Value& jsonValue)
{
    jsonValue["player_id"]["id_h"] = info.player_id.id_h;
    jsonValue["player_id"]["id_l"] = info.player_id.id_l;
    jsonValue["flag"] = info.flag;
    jsonValue["opt"] = info.opt;
    jsonValue["record_time"] = info.record_time;
}

void write_TaskTarget(TASK_TEMPLATE* taskIt, Json::Value& task_json)
{
    Json::Value target_json;
    std::string target_value = "";

    if(!taskIt) return;

    for(std::vector<TASK_TARGET>::iterator targetIt = taskIt->targets.begin();
        targetIt != taskIt->targets.end(); targetIt++)
    {
        target_json.clear();
        target_value = "";

        switch(targetIt->type)
        {
        case TASK_TARGET_1:     // 杀怪物收集道具
            {
                write_TaskTarget_1(&(*targetIt), target_value);
            }
            break;
        case TASK_TARGET_3:     // 杀怪物
            {
                write_TaskTarget_3(&(*targetIt), target_value);
            }
            break;
        case TASK_TARGET_4:     // 通关副本
            {
                write_TaskTarget_4(&(*targetIt), target_value);
            }
            break;
        case TASK_TARGET_6:     // 对话
            {
                write_TaskTarget_6(&(*targetIt), target_value);
            }
            break;
        case TASK_TARGET_17:    // 跳转场景
            {
                write_TaskTarget_17(&(*targetIt), target_value);
            }
            break;
        case TASK_TARGET_9:
        case TASK_TARGET_10:
        case TASK_TARGET_12:
        case TASK_TARGET_13:
        case TASK_TARGET_20:

        case TASK_TARGET_23:    // 装备升星（特定）
        case TASK_TARGET_24:    // 装备升级（特定）
        case TASK_TARGET_25:    // 品质强化（特定）
        case TASK_TARGET_41:    // 参加活动
        case TASK_TARGET_77:    // 符文等级（特定）
        case TASK_TARGET_78:    // 获得坐骑
        case TASK_TARGET_79:    // 累计充值（开服）
        case TASK_TARGET_81:    // 全身镶嵌宝石等级
        case TASK_TARGET_82:    // 坐骑升级
        case TASK_TARGET_83:    // 被动技能升级
        case TASK_TARGET_85:    // 激活护魂迷宫
        case TASK_TARGET_86:    // 提升宠物等级
        case TASK_TARGET_87:    // 镶嵌特定宝石数
        case TASK_TARGET_104:   // vip等级value达到num人数
        case TASK_TARGET_109:   // 充值排行发奖区间
        case TASK_TARGET_111:   // 单笔充值无限奖励
        case TASK_TARGET_112:   // 翅膀排行发奖区间
        case TASK_TARGET_113:   // 宠物排行发奖区间
        case TASK_TARGET_114:   // 坐骑排行发奖区间
        case TASK_TARGET_115:   // 战力排行发奖区间
        case TASK_TARGET_116:   // 等级排行发奖区间
        case TASK_TARGET_118:   // 砸金蛋次数
        case TASK_TARGET_121:   // 翅膀升阶
        case TASK_TARGET_122:   // 圣剑升星
        case TASK_TARGET_123:   // 重置活动
        case TASK_TARGET_130:   // 化身战力排行活动
        case TASK_TARGET_124:   // 勇者争霸排行活动
        case TASK_TARGET_125:   // 化身战力
        case TASK_TARGET_126:   // 宠物战力
        case TASK_TARGET_127:   // 翅膀战力
        case TASK_TARGET_128:   // 坐骑战力
        case TASK_TARGET_129:   // 总战力
        case TASK_TARGET_131:   // 升级任意化身，多少级
        case TASK_TARGET_132:
            {
                write_TaskTarget_ValueNum(&(*targetIt), target_value);
            }
            break;
        case TASK_TARGET_26:    // 镶嵌装备宝石
        case TASK_TARGET_27:    // 装备合成宝石
        case TASK_TARGET_28:    // 宠物激发
        case TASK_TARGET_36:    // 设置骑宠
        case TASK_TARGET_38:    // 添加好友
        case TASK_TARGET_39:    // 开启星点
        case TASK_TARGET_42:    // 翅膀升阶
        case TASK_TARGET_43:    // NPC购买物品
        case TASK_TARGET_45:    // 勇者争霸
        case TASK_TARGET_46:    // 完成一轮日常任务
        case TASK_TARGET_47:    // 讨伐令
        case TASK_TARGET_49:    // 砸金像
        case TASK_TARGET_50:    // 进入宝库
        case TASK_TARGET_52:    // 切磋 改为组队一次
        case TASK_TARGET_54:    // 护卫
        case TASK_TARGET_55:    // 召唤坐骑
        case TASK_TARGET_56:    // 装备升星次数
        case TASK_TARGET_57:    // 开启星点
        case TASK_TARGET_58:    // 翅膀提升
        case TASK_TARGET_59:    // 技能符文升级
        case TASK_TARGET_62:    // 加入公会
        case TASK_TARGET_63:    // 捐赠公会贡献点
        case TASK_TARGET_64:    //完成多少次公会任务
        case TASK_TARGET_65:    //多少次宠物一件激发
        case TASK_TARGET_66:    //圣剑升星多少次
        case TASK_TARGET_69:    //装备升级（次数）
        case TASK_TARGET_70:    //装备升品（次数）
        case TASK_TARGET_71:    // 每日消费
        case TASK_TARGET_72:    // 累计充值
        case TASK_TARGET_73:    // 单笔充值
        case TASK_TARGET_74:    // 每日战力比拼
        case TASK_TARGET_75:    // 每日充值
        case TASK_TARGET_76:    // 角色等级
        case TASK_TARGET_80:    // 累计消费
        case TASK_TARGET_97:    // 累计在线时长
        case TASK_TARGET_88:    // 微端登陆
        case TASK_TARGET_7:     // 领取勇者争霸奖励次数
        case TASK_TARGET_8:     // 领取XX等级成长礼包
        case TASK_TARGET_11:    // 获得XX点公会贡献点
        case TASK_TARGET_15:    // 领取僵尸生存宝箱XX次
        case TASK_TARGET_98:    // 微端奖励
        case TASK_TARGET_99:    // 微端登陆
        case TASK_TARGET_95:    // 每日登陆
        case TASK_TARGET_100:   // 所有使用宝藏卡和魔石的抽取行为抽XX次
        case TASK_TARGET_101:   // 铂金抽取XX次
        case TASK_TARGET_102:   // 至尊抽取XX次
        case TASK_TARGET_103:   // 单笔充值
        case TASK_TARGET_105:   // vip等级
        case TASK_TARGET_106:   // 珍兽挑战 参与次数
        case TASK_TARGET_107:   // 月黑时刻 参与次数
        case TASK_TARGET_108:   // 珍兽克星 击杀次数
        case TASK_TARGET_110:   // 充值排行上榜
        case TASK_TARGET_117:   // 每日登陆(重置)
        case TASK_TARGET_119:   // 累计消费(循环)
        case TASK_TARGET_120:   // 累计充值(循环)
            {
                write_TaskTarget_Num(&(*targetIt), target_value);
            }
            break;
        default:
            CnAssert(false);
            break;
        }

        target_json["target_type"] = targetIt->type;
        target_json["value"] = target_value;

        task_json["targets"].append(target_json);
    }
}

void write_TaskTarget_ValueNum(TASK_TARGET* target, std::string& target_value)
{
    if(!target)
        return;

    std::string str_value = Crown::ToString(target->value);

    std::string str_num = Crown::ToString(target->num);

    target_value = str_value + "|" + str_num;
}

void write_TaskTarget_Num(TASK_TARGET* target, std::string& target_value)
{
    if(!target)
        return;

    std::string str_num = Crown::ToString(target->num);
    target_value =  str_num;
}

void write_TaskTarget_1(TASK_TARGET* target, std::string& target_value)
{
    if(!target)
        return;

    std::string str_npc_id = Crown::ToString(target->npc_id);
    std::string str_map_id = Crown::ToString(target->map_id);
    std::string str_value = Crown::ToString(target->value);
    std::string str_item_id = Crown::ToString(target->item_id);
    std::string str_rate = Crown::ToString(target->rate);
    std::string str_num = Crown::ToString(target->num);

    target_value = str_npc_id + "|" + str_map_id + "|" + str_value + "|" +
        str_item_id + "|" + str_rate + "|" + str_num;
}

void write_TaskTarget_3(TASK_TARGET* target, std::string& target_value)
{
    if(!target)
        return;

    std::string str_npc_id = Crown::ToString(target->npc_id);
    std::string str_map_id = Crown::ToString(target->map_id);
    std::string str_value = Crown::ToString(target->value);
    std::string str_num = Crown::ToString(target->num);

    target_value =  str_npc_id + "|" + str_map_id + "|" + str_value + "|" + str_num;
}

void write_TaskTarget_4(TASK_TARGET* target, std::string& target_value)
{
    if(!target)
        return;

    std::string str_npc_id = Crown::ToString(target->npc_id);
    std::string str_value = Crown::ToString(target->value);
    std::string str_num = Crown::ToString(target->num);

    target_value =  str_npc_id + "|" + str_value + "|" + str_num;
}

void write_TaskTarget_6(TASK_TARGET* target, std::string& target_value)
{
    if(!target)
        return;

    std::string str_npc_id = Crown::ToString(target->npc_id);

    target_value =  str_npc_id;
}

void write_TaskTarget_17(TASK_TARGET* target, std::string& target_value)
{
    if(!target)
        return;

    std::string str_npc_id = Crown::ToString(target->npc_id);
    std::string str_map_id = Crown::ToString(target->map_id);
    std::string str_value = Crown::ToString(target->value);

    target_value =  str_npc_id + "|" + str_map_id + "|" + str_value;
}
