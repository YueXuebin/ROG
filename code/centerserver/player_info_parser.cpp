#include "centerserver_pch.h"
#include "player_info_parser.h"
#include "player.h"
#include "player_manager.h"
#include "dbdriver.h"
#include "gate_handler.h"
#include "json_helper.h"
#include "config_loader.h"
#include "dungeon_manager.h"


IMPLEMENT_SINGLETON(PlayerInfoParser)


uint16 PlayerInfoParser::ParsePlayerInfo(DBRecorder&res, uint64 loginID)
{
    Player* player = NULL;
    player = PlayerManager::Instance()->GetPlayerByLoginID(loginID);
    if(!player)
    {
        CnInfo("no loginID player\n");
        return  ERR_UNFOUND_PLAYER;
    }

    // 重新对应player_manager中的帐号列表(解决帐号大小写的问题)
    const std::string& oldAccount = player->GetAccount();

    PROPERTY_INFO& prop_info = player->GetPropInfo();

    std::string old_platform_server_name = prop_info.platform_server_name;
    int32 old_region_id = prop_info.region;

    prop_info.Clear();

    uint16 errCode = ERR_FAILED;
    uint64 guildID = 0;
    uint64 playerID = 0;
    std::string open_dungeon_str;
    std::string guild_welfare_str;
    std::string city_fight_str;
    std::string vip_info_str;
    std::string open_system_str;
    std::string fcm_str;
    std::string vigor_str;
    std::string offline_vigor_str;
    Json::Reader reader;
    Json::Value json_val;

    // 读取账户信息
    res.GetRecord(prop_info.account);

    PlayerManager::Instance()->ReMapPlayerAccount(player, oldAccount, prop_info.account);

    res.GetRecord(prop_info.name);
    res.GetRecord(playerID);

    player->SetPlayerID(playerID);          // 设置PlayerID

    res.GetRecord(prop_info.platform_server_name);
    CnAssert(prop_info.platform_server_name == old_platform_server_name);           // 加载的角色服务器名一致检查

    res.GetRecord(prop_info.region);
    if(prop_info.region != old_region_id)
    {
        CnError("Error:region is error!\n");
        return errCode;
    }

    res.GetRecord(prop_info.sex);
    res.GetRecord(prop_info.career);
    res.GetRecord(prop_info.level);
    res.GetRecord(prop_info.exp);
    res.GetRecord(prop_info.money_point);
    res.GetRecord(prop_info.game_point);
    res.GetRecord(prop_info.vigor);
    res.GetRecord(prop_info.vigor_reftime, false);
    res.GetRecord(prop_info.hp);
    res.GetRecord(prop_info.mp);

    //res.GetRecord(guildID);
    //prop_info.guild_id_h = U64ID_HIGH(guildID);
    //prop_info.guild_id_l = U64ID_LOW(guildID);

    //res.GetRecord(prop_info.guild_join_time);
    //res.GetRecord(prop_info.turntable_pos);
    //res.GetRecord(prop_info.member_turn_pos);

    res.GetRecord(prop_info.unionmap_id);
    res.GetRecord(prop_info.x);
    res.GetRecord(prop_info.y);
    res.GetRecord(prop_info.angle);
    res.GetRecord(prop_info.right_flag);

    uint32 daily_reftime;
    res.GetRecord(daily_reftime, false);
    player->SetDailyReftime(daily_reftime);

    // 如果频道不存在，重置频道为1
    uint32  channelId = CHANNEL_ID(prop_info.unionmap_id);
    uint32  dungeonId = DUNGEON_ID(prop_info.unionmap_id);
    uint32  sceneId = SCENE_ID(prop_info.unionmap_id);

    // 账号被封了
    if(prop_info.right_flag & RIGHT_FLAG_FORBID_LOGIN)
    {
        return ERR_BAN_ACCOUNT_LOGIN;
    }

    // 副本不存在
    if(!DungeonManager::Instance()->HasUnionId(prop_info.unionmap_id))
    {
        CnWarn("dungeonId %d not exist\n", dungeonId);
        // 尝试将玩家设置到主城
        prop_info.unionmap_id = UNION_ID(1, CITY_MAIN_CITY_MAP, 1);
        if(!DungeonManager::Instance()->HasUnionId(prop_info.unionmap_id))
        {
            CnError("dungeonId %d not exist\n", dungeonId);
            return ERR_DUNGEON_NOT_EXIST;       // 竟然连主城的副本都不存在,一定是服务器挂了
        }
    }

    // 解析VIP信息
    // ======================================================================
    //json_val.clear();
    //if(!reader.parse(vip_info_str, json_val, false))
    //{
    //    CnError("Parse player:%llu vip_info failed\n", playerID);
    //    CnAssert(false);
    //    return errCode;
    //}

    //ParseVipInfo(json_val, prop_info.vip_info);
    //prop_info.vip_info.growth_point_recharge = prop_info.growth_point;

    // 解析开启系统信息
    // ======================================================================
    //OPEN_SYSTEM_INFO& open_system_info = player->GetOpenSystemInfo();

    //json_val.clear();
    //if(!reader.parse(open_system_str, json_val, false))
    //{
    //    CnError("Parse player:%llu open_system_info failed\n", playerID);
    //    CnAssert(false);
    //    return errCode;
    //}

    //ParseOpenSystemInfo(json_val, open_system_info);

    // 解析公会福利信息
    // ======================================================================

    //GUILD_WELFARE_INFO& guild_welfare_info = player->GetGuildWelfareInfo();

    //json_val.clear();
    //if(!reader.parse(guild_welfare_str, json_val, false))
    //{
    //    CnError("Parse player:%llu guild_welfare_info failed\n", playerID);
    //    CnAssert(false);
    //    return errCode;
    //}

    //ParseGuildWelfareInfo(json_val, guild_welfare_info);

    // 解析城市争夺战信息
    // ======================================================================
    //CITY_FIGHT_PLAYER_INFO& city_fight_info = player->GetCityFightInfo();
    //json_val.clear();
    //if(!reader.parse(city_fight_str, json_val, false))
    //{
    //    CnError("Parse player:%llu city_fight_str failed\n", playerID);
    //    CnAssert(false);
    //    return errCode;
    //}

    //ParseCityFightInfo(json_val, city_fight_info);

    // 解析防沉迷信息
    // ======================================================================
    //json_val.clear();
    //if(!reader.parse(fcm_str, json_val, false))
    //{
    //    CnError("Parse player:%llu fcm_info failed\n", playerID);
    //    CnAssert(false);
    //    return errCode;
    //}

    //ParseFcmInfo(json_val, prop_info.fcm_info);


    // 解析体力信息
    // ======================================================================
    //json_val.clear();
    //if(!reader.parse(vigor_str, json_val, false))
    //{
    //    CnError("Parse player:%llu vigor_info failed\n", playerID);
    //    CnAssert(false);
    //    return errCode;
    //}

    //ParseVigorInfo(json_val, prop_info.vigor_info);

    //离线体力
    //json_val.clear();
    //if(!reader.parse(offline_vigor_str, json_val, false))
    //{
    //    CnError("Parse player:%llu offline_vigor_info failed\n", playerID);
    //    CnAssert(false);
    //    return errCode;
    //}

    //ParseOffLineVigorInfo(json_val, prop_info.off_line_vigor_info);
    return ERR_SUCCEED;
}
// ==================================================
// 解析VIP信息
// ==================================================
void PlayerInfoParser::ParseVipInfo(Json::Value& json_val, VIP_INFO& vipInfo)
{
    vipInfo.vip_first_ts = toUint32(json_val["vip_first_ts"]);
    vipInfo.vip_ts = toUint32(json_val["vip_ts"]);
    vipInfo.is_vip = toUint32(json_val["is_vip"]);
    vipInfo.vip_card_1_num = toUint32(json_val["vip_card_1_num"]);
    vipInfo.vip_card_2_num = toUint32(json_val["vip_card_2_num"]);
    vipInfo.vip_card_3_num = toUint32(json_val["vip_card_3_num"]);
    vipInfo.is_vip_reward_today = toUint32(json_val["is_vip_reward_today"]);
    vipInfo.vip_gif_flag = toUint32(json_val["vip_gif_flag"]);
    vipInfo.experience_vip_state = toInt32(json_val["experience_vip_state"]);
    vipInfo.growth_point_no_recharge = toInt32(json_val["growth_point_no_recharge"]);
}

// ==================================================
// 解析开启系统信息
// ==================================================
void PlayerInfoParser::ParseOpenSystemInfo(Json::Value& json_val, OPEN_SYSTEM_INFO& openSystemInfo)
{
    //OPEN_SYSTEM_ITEM sys_item;

    //// 开启系统信息
    //uint32 count = json_val["sys_item"].size();
    //for(uint32 i=0; i<count; ++i)
    //{
    //    Json::Value& val = json_val["sys_item"][i];
    //    sys_item.Clear();
    //    sys_item.id = val["id"].asUInt();
    //    sys_item.version = val["v"].asInt();
    //    openSystemInfo.list.push_back(sys_item);
    //}
}


// ==================================================
// 解析公会福利信息
// ==================================================
void PlayerInfoParser::ParseGuildWelfareInfo(Json::Value& json_val, GUILD_WELFARE_INFO& guildWelfareInfo)
{
    uint32 id = 0;

    // 公会福利信息
    guildWelfareInfo.reward_welfare_time = json_val["reward_welfare_time"].asUInt();
    guildWelfareInfo.reward_turntalbe_time = json_val["reward_turntalbe_time"].asUInt();
    guildWelfareInfo.member_turntable_buy_times = json_val["member_turntable_buy_times"].asInt();

    uint32 count = json_val["turntable_items"].size();
    for(uint32 i=0; i<count; ++i)
    {
        Json::Value& val = json_val["turntable_items"][i];
        id = val["id"].asUInt();
        guildWelfareInfo.turntable_item_list.push_back(id);
    }
}

void PlayerInfoParser::ParseCityFightInfo(Json::Value& json_val, CITY_FIGHT_PLAYER_INFO& cityFightInfo )
{     
    CITY_FIGHT_ONE_CITY_INFO info;
    uint32 cityFightInfo_city_num = json_val.size();
    for (uint32 i = 0; i < cityFightInfo_city_num; i++)
    {
        info.Clear();

        Json::Value& val = json_val[i];
        info.city_id = val["city_id"].asUInt();
        info.timestamp = val["timestamp"].asUInt();

        cityFightInfo.city_list.push_back(info);
    }
}
// ==================================================
// 解析防沉迷信息
// ==================================================
void PlayerInfoParser::ParseFcmInfo(Json::Value& json_val, FCM_INFO& fcmInfo)
{
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    fcmInfo.fcm_flag = json_val["fcm_flag"].asUInt();
    fcmInfo.fcm_gain_rate = json_val["fcm_gain_rate"].asInt();
    fcmInfo.ingame_time = json_val["ingame_time"].asUInt();
    fcmInfo.outgame_time = json_val["outgame_time"].asUInt();
    fcmInfo.total_ingame_time = json_val["total_ingame_time"].asUInt();
    fcmInfo.total_outgame_time = json_val["total_outgame_time"].asUInt();

    // 登陆时间
    fcmInfo.ingame_time = now;

    // 计算累积上下线时间
    if(now > fcmInfo.total_outgame_time)
    {
        fcmInfo.total_outgame_time += now - fcmInfo.total_outgame_time;
    }
}

// ==================================================
// 解析体力信息
// ==================================================
void PlayerInfoParser::ParseVigorInfo(Json::Value& json_val,VIGOR_INFO& vigorInfo)
{
    vigorInfo.last_time = json_val["last_time"].asUInt();
}

// ==================================================
// 解析离线体力
// ==================================================
void PlayerInfoParser::ParseOffLineVigorInfo(Json::Value& json_val,OFF_LINE_VIGOR_INFO& vigorInfo)
{
    vigorInfo.start_time  = json_val["start_time"].asUInt();
    vigorInfo.b_get = json_val["b_get"].asUInt();
    vigorInfo.off_voigor_point =  json_val["off_voigor_point"].asUInt();
    vigorInfo.count_time =  json_val["count_time"].asUInt();
}





