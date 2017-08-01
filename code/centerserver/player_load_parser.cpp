#include "centerserver_pch.h"
#include "player_load_parser.h"
#include "player.h"
#include "player_manager.h"
#include "dbdriver.h"
#include "gate_handler.h"
#include "json_helper.h"
#include "season_manager.h"


IMPLEMENT_SINGLETON(PlayerLoadParser)

bool PlayerLoadParser::ParseHoroscope(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value horoscope_json;
    std::string horoscope_str;
    Player* player = NULL;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    HOROSCOPE_INFO& horoscope = player->GetHoroscopeInfo();
    horoscope.Clear();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(horoscope.horoscope_level);
        res.GetRecord(horoscope_str);

        if (!reader.parse(horoscope_str, horoscope_json, false))
        {
            CnError("Parse player:%llu horoscope info failed\n", playerID);
            return false;
        }

        horoscope.levelup_id = horoscope_json["levelup_id"].asUInt();
        horoscope.levelup_time = horoscope_json["levelup_time"].asUInt();
    }

    return true;
}
//技能
bool  PlayerLoadParser::ParseSkillInfo(DBRecorder& res, uint64 playerID)
{
    std::string skill_str("{}");
    Player* player = NULL;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    SKILL_COMPONENT_INFO & skill = player->GetSkillInfo();
    skill.Clear();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(skill_str);
    }

    if (!JsonToStruct(skill, skill_str))
    {
        CnError("Parse player:%llu skill info failed\n", playerID);
        return false;
    }

    return true;
}
//天赋
bool PlayerLoadParser::ParsePassiveSkillInfo(DBRecorder& res, uint64 playerID)
{
	Player* player = 0;
	player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
	if (!player) return false;

	std::string passive_str("{}");

	PASSIVE_SKILL_LIST& passive_skill_info = player->GetPassiveSkillInfo();
	passive_skill_info.Clear();

	if (res.GetRowNum() > 0)
	{
		res.GetRecord(passive_skill_info.available_points);
		res.GetRecord(passive_str);

		if (!JsonToStruct(passive_skill_info, passive_str))
		{
			CnError("Parse player:%llu passive skill failed\n", playerID);
			return false;
		}
	}

	return true;
}
//坐骑
bool  PlayerLoadParser::ParseRideInfo(DBRecorder& res, uint64 playerID)
{
    Player* player = 0;
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return false;

    Json::Reader reader;
    std::string ride_info_str;
    RIDE_INFO& rideInfo =  player->GetRideInfo();
    rideInfo.Clear();

    Json::Value pet_json;
    RIDE_PET_INFO ride_pet;

    RIDE_TRAIN ride_train;
    RIDE_ALTAS ride_altas;
    RIDE_MARKT ride_mark;
    RIDE_QUALITY_INFO ridequalityinfo;
    REWARD_ITEM object_item;
    std::string strpet_info;

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(strpet_info);
        if(!reader.parse(strpet_info, pet_json, false))
        {
             CnError("Parse player:%llu ridepet version failed\n", playerID);
             return false;
        }

        if (pet_json["version"].isNull()) 
        {
            res.GetRecord(ride_info_str);
            if (!JsonToStruct(rideInfo,ride_info_str))
            {
                CnError("Parse player:%llu ridepet failed\n", playerID);
                return false;
            }
        }
        else
        {
            rideInfo.ride_id = pet_json["ride_id"].asUInt();

            // 骑宠信息
            uint32 count = pet_json["ride_pet"].size();
            for(uint32 i=0; i<count; ++i)
            {
                Json::Value& val = pet_json["ride_pet"][i];
                ride_pet.pet_id = val["pet_id"].asUInt();
                ride_pet.name = val["name"].asString();
                ride_pet.express = val["express"].asInt();
                ride_pet.level = val["level"].asInt();
                ride_pet.state = val["state"].asInt();
                //ride_pet.b_free = val["b_free"].asInt();
                ride_pet.b_get = val["b_get"].asInt();
                ride_pet.b_mark = val["b_mark"].asInt();
                ride_pet.b_train = val["b_train"].asInt();
                rideInfo.ride_pet_list.push_back(ride_pet);
            }

            count = pet_json["ride_pet_random"].size();
            for(uint32 i=0; i<count; ++i)
            {
                Json::Value& val = pet_json["ride_pet_random"][i];
                ride_pet.pet_id = val["pet_id"].asUInt();
                ride_pet.name = val["name"].asString();
                ride_pet.express = val["express"].asInt();
                ride_pet.level = val["level"].asInt();
                ride_pet.state = val["state"].asInt();
                ride_pet.b_get = val["b_get"].asInt();
                ride_pet.b_mark = val["b_mark"].asInt();
                ride_pet.b_train = val["b_train"].asInt();
                rideInfo.ride_random_list.push_back(ride_pet);
            }

            count = pet_json["object_random"].size();
            for(uint32 i=0; i<count; ++i)
            {
                Json::Value& val = pet_json["object_random"][i];
                read_REWARD_ITEM(object_item, val);
                rideInfo.object_random_list.push_back(object_item);
            }

            count = pet_json["ride_train_list"].size();
            for(uint32 i=0; i<count; ++i)
            {
                Json::Value& val = pet_json["ride_train_list"][i];
                ride_train.id = val["id"].asUInt();
                ride_train.state = val["state"].asInt();
                ride_train.pet_id = val["pet_id"].asUInt();
                ride_train.mode_id = val["mode_id"].asUInt();
                ride_train.start_time = val["start_time"].asUInt();
                rideInfo.ride_train_list.push_back(ride_train);
            }


            count = pet_json["ride_altas_list"].size();
            for (uint32 i= 0; i< count; i++)
            {
                Json::Value& val = pet_json["ride_altas_list"][i];
                ride_altas.id = val["id"].asUInt();
                ride_altas.num = val["num"].asInt();
                ride_altas.state = val["state"].asInt();
                rideInfo.ride_altas_list.push_back(ride_altas);
            }

            count = pet_json["mark_list"].size();
            for (uint32 i= 0; i< count; i++)
            {
                ride_mark.Clear();
                Json::Value& val = pet_json["mark_list"][i];
                ride_mark.id = val["id"].asUInt();
                ride_mark.state = val["state"].asInt();
                ride_mark.pet_id = val["pet_id"].asUInt();
                rideInfo.mark_list.push_back(ride_mark);
            }

            count = pet_json["quality_list"].size();
            for (uint32 i= 0; i< count; i++)
            {
                ridequalityinfo.Clear();
                Json::Value& val = pet_json["quality_list"][i];
                ridequalityinfo.type = val["type"].asInt();
                ridequalityinfo.id = val["id"].asUInt();
                ridequalityinfo.num = val["num"].asInt();
                rideInfo.ride_quality_list.push_back(ridequalityinfo);
            }

            rideInfo.filed_num = pet_json["filed_num"].asInt();

            rideInfo.b_free =pet_json["b_free"].asInt();

            rideInfo.time =pet_json["time"].asUInt();

            rideInfo.critnum = pet_json["critnum"].asInt();

        }

    }
    return true;
}
// ==================================================
// 装备信息
// ==================================================
bool PlayerLoadParser::ParseEquipInfo(DBRecorder& res, const uint64& playerID)
{
    std::string equip_str("{}");
    Player* player = NULL;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    EQUIP_INFO & equip = player->GetEquipInfo();
    equip.Clear();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(equip_str);
    }

    if (!JsonToStruct(equip, equip_str))
    {
        CnError("Parse player:%llu equip info failed\n", playerID);
        return false;
    }

    return true;
}
// 药剂
bool PlayerLoadParser::ParsePotionInfo(DBRecorder& res, const uint64& playerID)
{
    std::string potion_str("{}");
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    POTION_INFO& potion = player->GetPotionInfo();
    potion.Clear();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(potion_str);
    }

    if (!JsonToStruct(potion, potion_str))
    {
        CnError("Parse player:%llu potion info failed\n", playerID);
        return false;
    }

    return true;
}

// 化身
bool PlayerLoadParser::ParseFashionInfo( DBRecorder& res, uint64 playerID )
{
    Json::Reader reader;
    Json::Value fashion_json;

    Player* player =0;
    std::string fashion_json_info;
    FASHION_INFO fashion_info;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return false;

    FASHION_BAG& equip_bag = player->GetFashionInfo();
    equip_bag.Clear();

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(fashion_json_info);

        if(!reader.parse(fashion_json_info, fashion_json, false))
        {
            CnError("Parse player:%llu fashion info failed\n", playerID);
            return  false;
        }

        equip_bag.fashion_id = fashion_json["fashion_id"].asUInt();
        equip_bag.last_finish_time = fashion_json["last_finish_time"].asUInt();
        equip_bag.fashion_skill_id = fashion_json["fashion_skill_id"].asUInt();

        // 时装列表
        uint32 count = fashion_json["fashion"].size();
        for(uint32 i = 0; i < count; ++i)
        {
            Json::Value& val = fashion_json["fashion"][i];
            fashion_info.fashion_id = val["fashion_id"].asUInt();
            fashion_info.fashion_level = val["fashion_level"].asUInt();
            equip_bag.fashion_list.push_back(fashion_info);
        }

        PACKAGE_COUNT_INFO count_info;
        count = fashion_json["package_count_info"].size();
        for(uint32 i = 0; i < count; ++i)
        {
            Json::Value& val = fashion_json["package_count_info"][i];
            count_info.package_id = val["package_id"].asUInt();
            count_info.package_count = val["package_count"].asUInt();
            equip_bag.package_count.push_back(count_info);
        }

        PACKAGE_TIME_INFO time_info;
        count = fashion_json["package_count_info"].size();
        for(uint32 i = 0; i < count; ++i)
        {
            Json::Value& val = fashion_json["package_time_info"][i];
            time_info.package_id = val["package_id"].asUInt();
            time_info.item_id = val["item_id"].asUInt();
            time_info.item_time = val["item_time"].asUInt();
            equip_bag.package_time.push_back(time_info);
        }


        // 时装列表
        count = fashion_json["random_list"].size();
        for(uint32 i=0; i<count; ++i)
        {  
            equip_bag.random_list.push_back(fashion_json["random_list"][i].asUInt());
        }
    }
    return true;
}
// 加载跟宠信息
bool PlayerLoadParser::ParsePetInfo(DBRecorder& res, uint64 playerID)
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    std::string soldier_str("{}");

    PET_INFO& petinfo = player->GetPetInfo();
    petinfo.Clear();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(soldier_str);
    }

    if (!JsonToStruct(petinfo.soldier_info, soldier_str))
    {
        CnError("Parse player:%llu pet info failed\n", playerID);
        return false;
    }

    return true;
}

//获得物品信息
bool PlayerLoadParser::ParseItemInfo( DBRecorder& res, const uint64 & playerID )
{
    std::string item_str("{}");
    Player* player = NULL;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    ITEM_INFO & item = player->GetItemInfo();
    item.Clear();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(item_str);
    }

    if (!JsonToStruct(item, item_str))
    {
        CnError("Parse player:%llu item info failed\n", playerID);
        return false;
    }

    return true;
}

// ==================================================
// 获得仓库信息
// ==================================================
bool PlayerLoadParser::ParseStoreInfo(DBRecorder& res, const uint64& playerID)
{
    std::string item_str("{}");
    Player* player = NULL;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    ITEM_INFO & item = player->GetStoreInfo();
    item.Clear();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(item_str);
    }

    if (!JsonToStruct(item, item_str))
    {
        CnError("Parse player:%llu item info failed\n", playerID);
        return false;
    } 

    return true;

}

// ==================================================
// 获得抽奖信息
// ==================================================
bool PlayerLoadParser::ParseRandomPackageInfo( DBRecorder& res, uint64 playerID )
{
    Player* player = NULL;
    std::string info_str("");

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    RANDOM_PACKAGE_RECORD_INFO& info = player->GetRandomPackageInfo();
    info.Clear();

    // 初始的时候角色在物品表中没有数据
    if(res.GetRowNum() > 0)
    {
        res.GetRecord(info_str);

        JsonToStruct(info, info_str);
    }

    return true;
}

// ==================================================
// 获得隐藏物品
// ==================================================
bool PlayerLoadParser::ParseHideItemInfo( DBRecorder& res, uint64 playerID )
{
    //Json::Reader reader;
    //Json::Value item_json;

    //Player* player = NULL;
    //std::string item_info;
    //ITEM_INFO item;

    //player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    //if(!player) 
    //    return false;

    //HIDE_BAG_INFO& hide_bag_info = player->GetHideBagInfo();
    //hide_bag_info.Clear();

    //// 初始的时候角色在物品表中没有数据
    //if(res.GetRowNum() > 0)
    //{
    //    res.GetRecord(item_info);
    //    if(item_info.length() > 0)
    //    {
    //        if(!reader.parse(item_info, item_json, false))
    //        {
    //            CnError("Parse player:%llu hide_item failed\n", playerID);
    //            return false;
    //        }

    //        // 物品的列表信息
    //        uint32 count = item_json["item"].size();
    //        for(uint32 i=0; i<count; ++i)
    //        {
    //            Json::Value& val = item_json["item"][i];
    //            read_ITEM_INFO(item, val);
    //            if(item.id == 0)
    //                continue;
    //            hide_bag_info.list.push_back(item);
    //        }
    //    }
    //}

    return true;
}

// 加载好友数据
bool PlayerLoadParser::ParseFriend(DBRecorder& res, uint64 playerID )
{
    FRIEND_INFO friend_info;
    uint64 player_id;
    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    FRIEND_LIST& friend_list = player->GetFriendInfo();
    friend_list.Clear();
    for(uint32 i=0; i<res.GetRowNum(); ++i)
    {
        friend_info.Clear();

        res.GetRecord(player_id);
        friend_info.player_id_l = U64ID_LOW(player_id);
        friend_info.player_id_h = U64ID_HIGH(player_id);
        res.GetRecord(friend_info.name);
        res.GetRecord(friend_info.sex);
        res.GetRecord(friend_info.level);
        res.GetRecord(friend_info.logout_time, false);

        std::string head_portrait_str;
        res.GetRecord(head_portrait_str);
        JsonToStruct(friend_info.head_portrait, head_portrait_str);
        
        friend_list.list.push_back(friend_info);
    }

    return true;
}

// 加载好友数据
bool PlayerLoadParser::ParseFriendHasMe(DBRecorder& res, uint64 playerID )
{
    FRIEND_INFO friend_info;
    uint64 player_id;
    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    FRIEND_LIST& friend_list = player->GetFriendHasMeInfo();
    friend_list.Clear();
    for(uint32 i=0; i<res.GetRowNum(); ++i)
    {
        friend_info.Clear();

        res.GetRecord(player_id);
        friend_info.player_id_l = U64ID_LOW(player_id);
        friend_info.player_id_h = U64ID_HIGH(player_id);
        res.GetRecord(friend_info.name);
        res.GetRecord(friend_info.sex);
        res.GetRecord(friend_info.level);
        res.GetRecord(friend_info.logout_time, false);
        friend_list.list.push_back(friend_info);
    }

    return true;
}

// 获得任务信息
bool PlayerLoadParser::ParseTaskInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value task_json;
    std::string task_game_str;
    std::string task_activity_str;
    std::string task_guild_str;
    Player* player =0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return false;
    }

    TASK_INFO& taskInfo = player->GetTaskInfo();
    TASK_GAME_INFO& task_game_info = taskInfo.task_game_info;
    TASK_FESTIVAL_INFO& task_festival_info = taskInfo.task_festival_info;
    TASK_GUILD_INFO& guild_info = task_game_info.guild_task;

    taskInfo.Clear();

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(task_game_str);
        res.GetRecord(task_activity_str);
        res.GetRecord(task_guild_str);

        if(!reader.parse(task_game_str, task_json, false))
        {
            CnError("Parse player:%llu task game info failed\n", playerID);
            //goto _ERR;
            return false;
        }

        // 读取任务游戏数据
        read_TaskGameInfo(task_game_info, task_json);

        if(!reader.parse(task_activity_str, task_json, false))
        {
            CnError("Parse player:%llu task activity info failed\n", playerID);
            return false;
        }

        // 读取任务活动数据
        read_TaskFestivalInfo(task_festival_info, task_json);

        if(!reader.parse(task_guild_str, task_json, false))
        {
            CnError("Parse player:%llu task guild info failed\n", playerID);
            return false;
        }

        // 读取任务公会数据
        read_TaskGuildInfo(guild_info, task_json);
    }
    else
    {
        taskInfo.db_flag |= DB_OPT_INSERT;
    }
    return true;
}

bool PlayerLoadParser::ParseGuideAndOpenSysInfo(DBRecorder& res, uint64 playerID)
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    std::string guide_str("{}");
    std::string open_system_str("{}");
    if (res.GetRowNum() > 0)
    {
        res.GetRecord(guide_str);
        res.GetRecord(open_system_str);
    }

    GUIDE_INFO& guideInfo = player->GetGuideInfo();
    guideInfo.Clear();

    if (!JsonToStruct(guideInfo, guide_str))
    {
        CnError("Parse player:%llu guide info failed\n", playerID);
        return false;
    }

    OPEN_SYSTEM_INFO& openSysInfo = player->GetOpenSystemInfo();
    openSysInfo.Clear();

    if(!JsonToStruct(openSysInfo, open_system_str))
    {
        CnError("Parse player:%llu open system info failed\n", playerID);
        return false;
    }
    
    return true;
}

// 获得善恶信息
bool PlayerLoadParser::ParsetEnemyInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value enemy_json;
    std::string enemy_str;
    Player* player =0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return false;
    }

    ENEMY_INFO& enemyInfo = player->GetEnemyInfo();
    enemyInfo.Clear();

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(enemy_str);

        if(!reader.parse(enemy_str, enemy_json, false))
        {
            CnError("Parse player:%llu enemy info failed\n", playerID);
            //goto _ERR;
            return false;
        }

        // 读取任务游戏数据
        read_ENEMY_INFO(enemyInfo, enemy_json);
    }
    else
    {
        enemyInfo.db_flag |= DB_OPT_INSERT;
    }
    return true;
}
// ==================================================
// 获得角色奖励池
// ==================================================
bool PlayerLoadParser::ParseRewardPool(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value reward_json;

    Player* player =0;
    std::string reward_info;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    REWARD_INFO& reward = player->GetRewardInfo();
    reward.Clear();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(reward_info);

        if (!reader.parse(reward_info, reward_json, false))
        {
            CnError("Parse player:%llu reward info failed\n", playerID);
            return false;
        }

        uint32 table_count = reward_json["tables"].size();
        for(uint32 i=0; i<table_count; i++)
        {
            REWARD_TABLE table;

            Json::Value table_json = reward_json["tables"][i];
            read_REWARD_TABLE(table, table_json);

            reward.tables.push_back(table);
        }
    }

    return true;
}

// ==================================================
// 获得通天塔信息
// ==================================================
bool PlayerLoadParser::ParseBabelInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value babel_json;

    Player* player =0;
    std::string babel_info;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    BABEL_INFO& babel = player->GetBabelInfo();
    babel.Clear();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(babel_info);

        if (!reader.parse(babel_info, babel_json, false))
        {
            CnError("Parse player:%llu babel info failed\n", playerID);
            return false;
        }

        babel.last_refresh_time = babel_json["last_refresh_time"].asUInt();
        babel.left_reset_times = babel_json["left_reset_times"].asInt();
        babel.babel_tier = babel_json["babel_tier"].asInt();
        babel.babel_stage = babel_json["babel_stage"].asInt();
        babel.pass_tier = babel_json["pass_tier"].asInt();
        babel.pass_stage = babel_json["pass_stage"].asInt();
    }

    return true;
}

// ==================================================
// 获取充值积分抽奖信息
// ==================================================
bool PlayerLoadParser::ParsePayPointLotteryInfo(DBRecorder &res, uint64 playerID)
{
    Player *player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if (!player)
    {
        return false;
    }

    if (res.GetRowNum() > 0)
    {
        std::string jsonStr;
        res.GetRecord(jsonStr);

        PAY_POINT_LOTTERY_INFO &info = player->GetPayPointLotteryInfo();

        if (!JsonToStruct(info, jsonStr))
        {
            CnError("Parse player:%llu pay_point_lottery_info info failed\n", playerID);
            return false;
        }
    }

    return true;
}

// ==================================================
// 获取VIP独享副本信息
// ==================================================
bool PlayerLoadParser::ParseVIPOnlyDungeonInfo(DBRecorder &res, uint64 playerID)
{
    Player *player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if (!player)
    {
        return false;
    }

    if (res.GetRowNum() > 0)
    {
        std::string jsonStr;
        res.GetRecord(jsonStr);

        VIP_ONLY_DUNGEON_INFO &info = player->GetVIPOnlyDungeonInfo();

        if (!JsonToStruct(info, jsonStr))
        {
            CnError("Parse player:%llu vip_only_dungeon_info info failed\n", playerID);
            return false;
        }
    }

    return true;
}

// ==================================================
// 加载勇敢的心数据
// ==================================================
bool PlayerLoadParser::ParseBhInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value bh_json;

    Player* player =0;
    std::string bh_info;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    BH_INFO& bh = player->GetBhInfo();
    bh.Clear();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(bh_info);

        if (!reader.parse(bh_info, bh_json, false))
        {
            CnError("Parse player:%llu brave heart info failed\n", playerID);
            return false;
        }

        bh.today_count = bh_json["today_count"].asUInt();
        bh.is_selected = bh_json["is_selected"].asUInt();
        bh.cur_type = bh_json["cur_type"].asInt();
        bh.pass_type = bh_json["pass_type"].asInt();
        bh.pass_stage = bh_json["pass_stage"].asInt();
        bh.heart_point = bh_json["heart_point"].asInt();
        bh.heart_point_sum = bh_json["heart_point_sum"].asInt();
        // attributes
        Json::Value attributes_json = bh_json["attributes"];
        for (uint32 i = 0; i<attributes_json.size(); i++)
        {
            Json::Value attr_json = attributes_json[i];
            BH_ADDITION add;
            add.add_prop = attr_json["attr"].asUInt();
            add.add_value = attr_json["val"].asInt();
            bh.addition_list.push_back(add);
        }
    }

    return true;
}

// ==================================================
// 获得活动信息
// ==================================================
bool PlayerLoadParser::ParseDailylActivityInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value activity_json;
    std::string activity_str;

    Player* player =0;
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    if (res.GetRowNum() > 0)
    {
        // racing
        // ---------------------------------------------------------------
        res.GetRecord(activity_str);
        if (!reader.parse(activity_str, activity_json, false))
        {
            CnError("Parse player:%llu activity info failed\n", playerID);
            return false;
        }

        RACING_INFO& racing_info = player->GetRacingInfo();
        racing_info.Clear();
        //Json::Value& racing_json = activity_json["racing"];
        racing_info.refresh         = activity_json["refresh"].asInt();
        racing_info.count           = activity_json["count"].asInt();

        // mining
        // ---------------------------------------------------------------
        res.GetRecord(activity_str);
        if (!reader.parse(activity_str, activity_json, false))
        {
            CnError("Parse player:%llu activity info failed\n", playerID);
            return false;
        }

        MINING_INFO& mining_info = player->GetMiningInfo();
        mining_info.Clear();
        //Json::Value& mining_json = activity_json["mining"];
        mining_info.index           = activity_json["index"].asInt();
        mining_info.state           = activity_json["state"].asInt();
        mining_info.time            = activity_json["time"].asInt();
        mining_info.type            = activity_json["type"].asInt();

        // treasury
        // ---------------------------------------------------------------
        res.GetRecord(activity_str);
        if (!reader.parse(activity_str, activity_json, false))
        {
            CnError("Parse player:%llu activity info failed\n", playerID);
            return false;
        }

        TREASURY_INFO& treasury_info = player->GetTreasuryInfo();
        treasury_info.Clear();
        //Json::Value& treasury_json  = activity_json["treasury"];
        treasury_info.count         = activity_json["count"].asInt();
        treasury_info.daily_time    = activity_json["daily_time"].asInt();
        treasury_info.enter_time    = activity_json["enter_time"].asInt();
        treasury_info.buy_count     = activity_json["buy_count"].asInt();

        // goldtree
        // ---------------------------------------------------------------
        res.GetRecord(activity_str);
        if (!reader.parse(activity_str, activity_json, false))
        {
            CnError("Parse player:%llu activity info failed\n", playerID);
            return false;
        }

        GOLDTREE_INFO& goldtree_info = player->GetGoldtreeInfo();
        goldtree_info.Clear();
        //Json::Value& treasury_json  = activity_json["goldtree"];
        goldtree_info.count         = activity_json["count"].asInt();
        goldtree_info.daily_time    = activity_json["daily_time"].asInt();
        goldtree_info.enter_time    = activity_json["enter_time"].asInt();
        goldtree_info.buy_count     = activity_json["buy_count"].asInt();

        // Adventure
        // ---------------------------------------------------------------
        res.GetRecord(activity_str);
        if (!reader.parse(activity_str, activity_json, false))
        {
            CnError("Parse player:%llu activity info failed\n", playerID);
            return false;
        }

        ADVENTURE_INFO& adventure_info = player->GetAdventureInfo();
        adventure_info.Clear();
        //Json::Value& adventure_json     = activity_json["adventure"];
        adventure_info.map_id           = activity_json["map_id"].asUInt();
        adventure_info.adventure_times  = activity_json["adventure_times"].asInt();
        adventure_info.refresh_time     = activity_json["refresh_time"].asUInt();
        adventure_info.end_time         = activity_json["end_time"].asUInt();

        // Answer
        // ---------------------------------------------------------------
        res.GetRecord(activity_str);
        if (!reader.parse(activity_str, activity_json, false))
        {
            CnError("Parse player:%llu activity info failed\n", playerID);
            return false;
        }

        ANSWER_INFO& answer_info = player->GetAnswerInfo();
        answer_info.Clear();
        answer_info.answer_version = activity_json["answer_version"].asUInt();
        answer_info.reward_version = activity_json["reward_version"].asUInt();
        answer_info.question_id = activity_json["question_id"].asUInt();
        answer_info.result_id = activity_json["result_id"].asUInt();
        answer_info.score = activity_json["score"].asInt();
        answer_info.continuation = activity_json["continuation"].asInt();
        answer_info.use_double_flag = activity_json["use_double_flag"].asInt();
        answer_info.buy_double_num = activity_json["buy_double_num"].asInt();
        answer_info.sys_double_num = activity_json["sys_double_num"].asInt();
        // TODO:

        res.GetRecord(activity_str);
        if(activity_str != "")
        {
            if(!reader.parse(activity_str, activity_json, false))
            {
                CnError("Parse player:%llu activity info failed\n", playerID);
                return false;
            }
            SINGLE_ANSWER_INFO& single_answerinfo = player->GetSingleAnswerInfo();
            single_answerinfo.Clear();
            single_answerinfo.cd_start_time = activity_json["cd_start_time"].asUInt();
            single_answerinfo.into_answer = activity_json["into_answer"].asInt();
            single_answerinfo.change_num = activity_json["change_num"].asInt();
            single_answerinfo.num = activity_json["num"].asInt();
        }
    }

    return true;
}

// ==================================================
// 获得在线活动信息
// ==================================================
bool PlayerLoadParser::ParseOnlineActivityInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value sign_info_json;
    Json::Value online_reward_json;
    std::string sign_info_str;
    std::string online_reward_str;
    //Crown::CSDDateTime now = Crown::SDNow().GetTimeValue();
    //Crown::CSDDateTime lastDay;

    Player* player =0;
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    if (res.GetRowNum() > 0)
    {
        // sign
        // ---------------------------------------------------------------
        res.GetRecord(sign_info_str);
        if (!reader.parse(sign_info_str, sign_info_json, false))
        {
            CnError("Parse player:%llu activity info failed\n", playerID);
            return false;
        }

        SIGN_INFO& sign_info = player->GetSignInfo();
        sign_info.sign_gain_sheet_idx = sign_info_json["sign_gain_sheet_idx"].asInt();
        sign_info.last_sign_time = sign_info_json["last_sign_time"].asUInt();
        sign_info.days = sign_info_json["days"].asUInt();


        // online_reward
        // ---------------------------------------------------------------
       /* res.GetRecord(online_reward_str);
        if (!reader.parse(online_reward_str, online_reward_json, false))
        {
            CnError("Parse player:%llu activity info failed\n", playerID);
            return false;
        }

        ONLINE_REWARD_INFO& online_reward_info  = player->GetOnlineRewardInfo();
        online_reward_info.online_reward_flag   = online_reward_json["online_reward_flag"].asUInt();
        online_reward_info.online_reward_day   = online_reward_json["online_reward_day"].asUInt();
        online_reward_info.online_reward_time   = online_reward_json["online_reward_time"].asUInt();
        online_reward_info.level                = online_reward_json["level"].asInt();
        online_reward_info.day                  = online_reward_json["day"].asUInt();
        online_reward_info.grow_reward_num      = online_reward_json["grow_reward_num"].asInt();
        online_reward_info.fixed_reward_num     = online_reward_json["fixed_reward_num"].asInt();
        online_reward_info.cirtnum              = online_reward_json["cirtnum"].asInt();
        online_reward_info.open_time            = online_reward_json["open_time"].asUInt();
        online_reward_info.interval_time           = online_reward_json["interval_time"].asUInt();
        online_reward_info.online_level_reward_flag = online_reward_json["online_level_reward_flag"].asInt();


        uint32 countnum = 0;
        countnum = online_reward_json["growlist"].size();

        for (uint32 i=0; i<countnum; i++)
        {
            REWARD_ITEM object_item;
            Json::Value& val = online_reward_json["growlist"][i];
            read_REWARD_ITEM(object_item, val);
            online_reward_info.grow_list.push_back(object_item);
        }*/






    }

    return true;
}

// ==================================================
// 加载个人邮件数据
// ==================================================
bool PlayerLoadParser::ParsePlayerMailData(DBRecorder& res, uint64 playerID)
{
    MAIL_INFO mail_info;
    uint64 mail_id = 0;
    uint64 receiver_id = 0;
    uint64 sender_id = 0;
    int64 send_time = 0;
    Player* player = 0;
    std::string attachment;
    REWARD_ITEM mail_attach;
    Json::Reader reader;
    Json::Value json_val;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    MAIL_SYSTEM_INFO& mail_system = player->GetMailSysInfo();

    for(uint32 i=0; i<res.GetRowNum(); ++i)
    {
        mail_info.Clear();

        res.GetRecord(mail_id);
        mail_info.mail_id.id_h = U64ID_HIGH(mail_id);
        mail_info.mail_id.id_l = U64ID_LOW(mail_id);

        res.GetRecord(receiver_id);
        mail_info.receiver_id.id_h = U64ID_HIGH(receiver_id);
        mail_info.receiver_id.id_l = U64ID_LOW(receiver_id);

        res.GetRecord(sender_id);
        mail_info.sender_id.id_h = U64ID_HIGH(sender_id);
        mail_info.sender_id.id_l = U64ID_LOW(sender_id);

        res.GetRecord(mail_info.sender_name);

        res.GetRecord(send_time);
        mail_info.send_time = (uint32)send_time;

        res.GetRecord(mail_info.mail_type);
        res.GetRecord(mail_info.mail_flag);
        res.GetRecord(mail_info.title);
        res.GetRecord(mail_info.message);
        res.GetRecord(mail_info.attachment_flag);
        res.GetRecord(attachment);

        if(!reader.parse(attachment, json_val, false))
        {
            // 如果解析错误，客户端不会显示此邮件亦不会删除，等解析正确后玩家上线又会显示它
            CnError("Parse player:%llu mail%llu failed\n", playerID, mail_id);
            continue;
        }

        uint32 mail_info_attachment_count = json_val["attach"].size();
        for(uint32 i=0; i<mail_info_attachment_count; ++i)
        {
            mail_attach.Clear();
            Json::Value& attach_json = json_val["attach"][i];
            read_REWARD_ITEM(mail_attach, attach_json);
            mail_info.attachment_list.push_back(mail_attach);
        }

        mail_system.player_mail_list.push_back(mail_info);
    }

    return true;
}

// ==================================================
// 加载系统邮件数据
// ==================================================
bool PlayerLoadParser::ParseSystemMailData(DBRecorder& res, uint64 playerID)
{
    MAIL_INFO mail_info;
    uint64 mail_id = 0;
    uint64 receiver_id = 0;
    uint64 sender_id = 0;
    int64 send_time = 0;
    Player* player = 0;
    std::string attachment;
    REWARD_ITEM mail_attach;
    Json::Reader reader;
    Json::Value json_val;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    MAIL_SYSTEM_INFO& mail_system = player->GetMailSysInfo();

    for(uint32 i=0; i<res.GetRowNum(); ++i)
    {
        mail_info.Clear();

        res.GetRecord(mail_id);
        mail_info.mail_id.id_h = U64ID_HIGH(mail_id);
        mail_info.mail_id.id_l = U64ID_LOW(mail_id);

        res.GetRecord(send_time);
        mail_info.send_time = (uint32)send_time;
        res.GetRecord(mail_info.mail_type);
        res.GetRecord(mail_info.mail_flag, false);          // 使用了IFNULL
        res.GetRecord(mail_info.title);
        res.GetRecord(mail_info.message);
        res.GetRecord(mail_info.attachment_flag, false);    // 使用了IFNULL
        res.GetRecord(attachment);

        if(!reader.parse(attachment, json_val, false))
        {
            // 如果解析错误，客户端不会显示此邮件亦不会删除，等解析正确后玩家上线又会显示它
            CnError("Parse player:%llu mail%llu failed\n", playerID, mail_id);
            continue;
        }

        uint32 mail_info_attachment_count = json_val["attach"].size();
        for(uint32 i=0; i<mail_info_attachment_count; ++i)
        {
            mail_attach.Clear();
            Json::Value& attach_json = json_val["attach"][i];
            read_REWARD_ITEM(mail_attach, attach_json);
            mail_info.attachment_list.push_back(mail_attach);
        }

        mail_system.system_mail_list.push_back(mail_info);
    }

    return true;
}

// ==================================================
// 加载组邮件数据
// ==================================================
bool PlayerLoadParser::ParseGroupMailData(DBRecorder& res, uint64 playerID)
{
    MAIL_INFO mail_info;
    uint64 mail_id = 0;
    uint64 receiver_id = 0;
    uint64 sender_id = 0;
    int64 send_time = 0;
    Player* player = 0;
    std::string attachment;
    REWARD_ITEM mail_attach;
    Json::Reader reader;
    Json::Value json_val;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    MAIL_SYSTEM_INFO& mail_system = player->GetMailSysInfo();

    for(uint32 i=0; i<res.GetRowNum(); ++i)
    {
        mail_info.Clear();

        res.GetRecord(mail_id);
        mail_info.mail_id.id_h = U64ID_HIGH(mail_id);
        mail_info.mail_id.id_l = U64ID_LOW(mail_id);

        res.GetRecord(send_time);
        mail_info.send_time = (uint32)send_time;
        res.GetRecord(mail_info.mail_type);
        res.GetRecord(mail_info.mail_flag);
        res.GetRecord(mail_info.title);
        res.GetRecord(mail_info.message);
        res.GetRecord(mail_info.attachment_flag);
        res.GetRecord(attachment);

        if(!reader.parse(attachment, json_val, false))
        {
            // 如果解析错误，客户端不会显示此邮件亦不会删除，等解析正确后玩家上线又会显示它
            CnError("Parse player:%llu mail%llu failed\n", playerID, mail_id);
            continue;
        }

        uint32 mail_info_attachment_count = json_val["attach"].size();
        for(uint32 i=0; i<mail_info_attachment_count; ++i)
        {
            mail_attach.Clear();
            Json::Value& attach_json = json_val["attach"][i];
            read_REWARD_ITEM(mail_attach, attach_json);
            mail_info.attachment_list.push_back(mail_attach);
        }

        mail_system.system_mail_list.push_back(mail_info);
    }

    return true;
}
// ==================================================
// 获得怪物之门信息
// ==================================================
bool PlayerLoadParser::ParseMonsterDoorInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value battle_json;

    Player* player =0;
    std::string battle_info_str;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    MONSTER_DOOR_INFO& monster_door_info = player->GetMonsterDoorInfo();
    monster_door_info.Clear();

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(battle_info_str);

        if(!reader.parse(battle_info_str, battle_json, false))
        {
            CnError("Parse player:%llu offline battle info failed\n", playerID);
            return false;
        }
        monster_door_info.battle_boss_flag = battle_json["battle_boss_flag"].asUInt();
        monster_door_info.battle_count = battle_json["battle_count"].asUInt();
        monster_door_info.combat_count = battle_json["combat_count"].asUInt();
        monster_door_info.combat_flag = battle_json["combat_flag"].asUInt();
        monster_door_info.combat_history_flag = battle_json["combat_history_flag"].asUInt();
        monster_door_info.reset_count = battle_json["reset_count"].asUInt();
        monster_door_info.auto_cd_timestamp = battle_json["auto_cd_timestamp"].asUInt();
        monster_door_info.auto_index = battle_json["auto_index"].asUInt();
    }

    return true;
    // LoadPlayerDoorInfo(playerID);
}

//获得勇者争霸信息
bool PlayerLoadParser::ParsePlayerDoorInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value battle_json;

    Player* player =0;
    std::string battle_info_str;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    PLAYER_DOOR_INFO& player_door_info = player->GetPlayerDoorInfo();
    player_door_info.Clear();

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(battle_info_str);

        if(!reader.parse(battle_info_str, battle_json, false))
        {
            CnError("Parse player:%llu player door info failed\n", playerID);
            return false;
        }

        player_door_info.buy_challenge_count = battle_json["buy_challenge_count"].asUInt();
        player_door_info.challenge_count = battle_json["challenge_count"].asUInt();
        player_door_info.winning_steak = battle_json["winning_steak"].asUInt();
        player_door_info.cd_timestamp = battle_json["cd_timestamp"].asUInt();
        player_door_info.rewarded_info.rewarded_today = battle_json["rewarded_today"].asUInt();
        player_door_info.is_init = battle_json["is_init"].asUInt();
    }

    return true;
}
// ==================================================
// 加载翅膀信息
// ==================================================
bool PlayerLoadParser::ParseWingInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value wing_info_json;
    std::string wing_info_str;
  
    Player* player =0;
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    // 用户身上的翅膀
    WING_INFO& wing_info = player->GetWingInfo();

    if (res.GetRowNum() > 0)
    {
       /* wing_info的json结构
        {
            "transmogrification": 1,  // 当前穿戴的翅膀id
            "count": 10,              // 用户拥有几个翅膀
            "list": [
                {
                    "id": 1,          // 拥有的翅膀id
                    "level":1,        // 翅膀当前等级
                    "step":1,         // 翅膀当前阶数
                    "star":2          // 翅膀当前星级
                },
                {
                    "id": 2,
                    "step":1,         // 翅膀当前阶数
                    "star":3
                }
            ]
        }
        */

        res.GetRecord(wing_info_str);
        if (!reader.parse(wing_info_str, wing_info_json, false))
        {
            CnError("Parse player:%llu wing info failed\n", playerID);
            return false;
        }
        
        wing_info.transmogrification    = wing_info_json["transmogrification"].asInt();
        uint32 wing_info_count          = wing_info_json["count"].asInt();
        wing_info.count = wing_info_count;
        // 循环翅膀信息
        for(uint32 i = 0; i < wing_info_count; i++)
        {
            Json::Value& val = wing_info_json["list"][i];

            WING_ONE_INFO tmpWingOneInfo;
            tmpWingOneInfo.id = val["id"].asInt();
            tmpWingOneInfo.level = val["level"].asInt();
            tmpWingOneInfo.step = val["step"].asInt();
            tmpWingOneInfo.star = val["star"].asInt();
           
            wing_info.list.push_back(tmpWingOneInfo);
        }
    }
    
    return true;
}

// 加载修罗场信息
bool PlayerLoadParser::ParseAshuraInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value ashura_info_json;
    std::string ashura_info_str;

    Player* player =0;
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    ASHURA_INFO& ashura_info = player->GetAshuraInfo();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(ashura_info_str);
        if (!reader.parse(ashura_info_str, ashura_info_json, false))
        {
            CnError("Parse player:%llu activity info failed\n", playerID);
            return false;
        }

        ashura_info.kill_count          = ashura_info_json["kill_count"].asInt();
        ashura_info.dead_count          = ashura_info_json["dead_count"].asInt();
        ashura_info.refresh_time        = ashura_info_json["refresh_time"].asInt();
        ashura_info.delay_count         = ashura_info_json["delay_count"].asInt();

        res.GetRecord(ashura_info.all_kill_count);

    }

    return true;
}

// 获得充值信息
bool PlayerLoadParser::ParseRechargeInfo(DBRecorder& res, uint64 playerID)
{
    int32 rechargeType = 0;
    PLAYER_RECHARGE_RECORDER rechargeRecord;
    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    PLAYER_RECHARGE_INFO& rechargeInfo = player->GetRechargeInfo();

    for(uint32 i =0; i < res.GetRowNum(); ++i)
    {
        res.GetRecord(rechargeType, false);
        res.GetRecord(rechargeRecord.money_point);
        res.GetRecord(rechargeRecord.max_single);
        res.GetRecord(rechargeRecord.update_time, false);

        if(rechargeType == RECHARGE_ID_TOTAL_RECHARGE)
        {
            rechargeInfo.recharge_total = rechargeRecord;
        }
        else if(rechargeType == RECHARGE_ID_DAILY_RECHARGE)
        {
            rechargeInfo.recharge_daily_list.push_back(rechargeRecord);
        }
    }

    return true;
}

// 加载商城信息
bool PlayerLoadParser::ParseMallInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value mall_info_json;
    std::string mall_info_str;
    RESTRICTION mall_info;

    Player* player =0;
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(mall_info_str);

        if(!reader.parse(mall_info_str, mall_info_json, false))
        {
            CnError("Parse player:%llu achive failed\n", playerID);
            return false;
        }

        RESTRICTION_LIST& mall_info_list = player->GetMallInfo();
        mall_info_list.Clear();

        uint32 count = mall_info_json["mall_info"].size();
        for(uint32 i = 0; i < count; ++i)
        {
            mall_info.Clear();
            Json::Value& val = mall_info_json["mall_info"][i];
            mall_info.product_id = val["product_id"].asUInt();
            mall_info.num = val["num"].asInt();
            mall_info_list.restriction_list.push_back(mall_info);
        }
    }

    return true;
}

// 获得称号信息
bool PlayerLoadParser::ParseTitleInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value title_info_json;
    std::string title_info_str;
    TITLE_INFO title_info;

    Player* player =0;
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(title_info_str);

        if(!reader.parse(title_info_str, title_info_json, false))
        {
            CnError("Parse player:%llu title failed\n", playerID);
            return  false;
        }

        TITLE_LIST& title_info_list = player->GetTitleInfo();
        title_info_list.Clear();

        uint32 count = title_info_json["list"].size();
        for(uint32 i=0; i<count; ++i)
        {
            title_info.Clear();
            Json::Value& val = title_info_json["list"][i];

            title_info.titlid = val["titlid"].asUInt();
            title_info.starttime = val["starttime"].asUInt();
            title_info.b_state  = val["b_state"].asInt();
            title_info.time = val["time"].asUInt();
            title_info.num  = val["num"].asInt();

            title_info_list.list.push_back(title_info);
        }
    }

    return true;
}

//每日投资理财
bool PlayerLoadParser::ParseDayInvestmentInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value day_investment_info_json;
    std::string day_investment_info_str;
    ONE_DAY_INVESTMENT_INFO one_day_investment_info;

    Player* player =0;
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
        return false;

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(day_investment_info_str);

        if(!reader.parse(day_investment_info_str, day_investment_info_json, false))
        {
            CnError("Parse player:%llu day_investment failed\n", playerID);
            return  false;
        }

        DAY_INVESTMENT_INFO& day_investment_info = player->GetDayInvestmentInfo();
        day_investment_info.Clear();

        uint32 count = day_investment_info_json["list"].size();
        for(uint32 i=0; i<count; ++i)
        {
            one_day_investment_info.Clear();
            Json::Value& val = day_investment_info_json["list"][i];

            one_day_investment_info.day = val["day"].asInt();
            one_day_investment_info.status = val["status"].asInt();

            day_investment_info.info_list.push_back(one_day_investment_info);
        }
        day_investment_info.invest_begin_time = day_investment_info_json["invest_begin_time"].asUInt();
        day_investment_info.recharge_num =  day_investment_info_json["recharge_num"].asInt();
    }

    return true;
}

//投资理财等级
bool PlayerLoadParser::ParseLevelInvestmentInfo( DBRecorder& res, uint64 playerID )
{
    Json::Reader reader;
    Json::Value info_json;
    std::string info_str;
    ONE_LEVEL_INVESTMENT_INFO one_level_investment_info;

    Player* player =0;
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
        return  false;

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(info_str);

        if(!reader.parse(info_str, info_json, false))
        {
            CnError("Parse player:%llu level_investment failed\n", playerID);
            return false;
        }

        LEVEL_INVESTMENT_INFO& level_investment_info = player->GetLevelInvestmentInfo();
        level_investment_info.Clear();

        uint32 count = info_json["list"].size();
        for(uint32 i=0; i<count; ++i)
        {
            one_level_investment_info.Clear();
            Json::Value& val = info_json["list"][i];

            one_level_investment_info.level = val["level"].asInt();
            one_level_investment_info.status = val["status"].asInt();

            level_investment_info.info_list.push_back(one_level_investment_info);
        }

        level_investment_info.invested_money = info_json["invested_money"].asUInt();
    }


    return true;
}
//获得等级活动
bool PlayerLoadParser::ParseLevelActivityInfo( DBRecorder& res, uint64 playerID )
{
    Json::Reader reader;
    Json::Value info_json;
    std::string info_str;

    Player* player =0;
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
        return false;

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(info_str);

        if(!reader.parse(info_str, info_json, false))
        {
            CnError("Parse player:%llu level_activity failed\n", playerID);
            return  false;
        }

        LEVEL_ACTIVITY_INFO& level_activity_info = player->GetLevelActivityInfo();
        level_activity_info.Clear();
        level_activity_info.accept_flag = info_json["accept_flag"].asInt();
    }

    return true;
}

//设置系统
bool PlayerLoadParser::ParseSettingInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value auto_fight_json;
    Json::Value system_json;
    Json::Value player_json;
    Json::Value autobuy_json;
    Json::Value voidfield_json;

    Player* player = 0;
    std::string autofight_info;
    std::string system_info;
    std::string player_info;
    std::string autobuy_info;
    std::string voidfield_info;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    // auto fight setting
    AUTO_FIGHT_SETTING_INFO& autoFightInfo = player->GetAutoFightSettingInfo();
    autoFightInfo.Clear();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(autofight_info);

        if (!reader.parse(autofight_info, auto_fight_json, false))
        {
            CnError("Parse player:%llu auto fight setting info failed\n", playerID);
            return false;
        }

        autoFightInfo.hp             = auto_fight_json["hp"].asUInt();
        autoFightInfo.hp_order       = auto_fight_json["hp_order"].asUInt();
        autoFightInfo.mp             = auto_fight_json["mp"].asUInt();
        autoFightInfo.mp_order       = auto_fight_json["mp_order"].asUInt();
        autoFightInfo.skills         = auto_fight_json["skills"].asUInt();
        autoFightInfo.auto_revival   = auto_fight_json["auto_revival"].asUInt();
        autoFightInfo.auto_fight     = auto_fight_json["auto_fight"].asUInt();
        autoFightInfo.auto_buy_hp_drug      = auto_fight_json["auto_buy_hp_drug"].asUInt();
        autoFightInfo.auto_buy_mp_drug      = auto_fight_json["auto_buy_mp_drug"].asUInt();
        autoFightInfo.auto_buy_attack_drug  = auto_fight_json["auto_buy_attack_drug"].asUInt();
        autoFightInfo.auto_buy_defence_drug = auto_fight_json["auto_buy_defence_drug"].asUInt();
        autoFightInfo.auto_buy_speed_drug   = auto_fight_json["auto_buy_speed_drug"].asUInt();
        autoFightInfo.auto_use_attack_drug  = auto_fight_json["auto_use_attack_drug"].asUInt();
        autoFightInfo.auto_use_defence_drug = auto_fight_json["auto_use_defence_drug"].asUInt();
        autoFightInfo.auto_use_speed_drug   = auto_fight_json["auto_use_speed_drug"].asUInt();
    }

    // system setting
    SYSTEM_SETTING_INFO& SystemInfo = player->GetSystemSettingInfo();
    SystemInfo.Clear();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(system_info);

        if (!reader.parse(system_info, system_json, false))
        {
            CnError("Parse player:%llu system setting info failed\n", playerID);
            return false;
        }

        read_SYSTEM_SETTING_INFO(SystemInfo, system_json);
    }

    // player setting
    PLAYER_SETTING_INFO& PlayerInfo = player->GetPlayerSettingInfo();
    PlayerInfo.Clear();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(player_info);

        if (!reader.parse(player_info, player_json, false))
        {
            CnError("Parse player:%llu player setting info failed\n", playerID);
            return false;
        }

        PlayerInfo.hp   = player_json["hp"].asUInt();
        PlayerInfo.mp   = player_json["mp"].asUInt();
    }

    // autobuy setting
    AUTOBUY_SETTING_INFO& AutobuyInfo = player->GetAutobuySettingInfo();
    AutobuyInfo.Clear();

    AutobuyInfo.autobuyList.resize(AUTOBUY_SETTING_ENUM_SUM);
    for(uint32 i=0; i<AUTOBUY_SETTING_ENUM_SUM; ++i)
    {
        AutobuyInfo.autobuyList[i] = 0;
    }

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(autobuy_info);

        if (!reader.parse(autobuy_info, autobuy_json, false))
        {
            CnError("Parse player:%llu autobuy setting info failed\n", playerID);
            return false;
        }

        uint32 listcount = autobuy_json["autobuyList"].size();
        listcount = Crown::SDMin((uint32)AutobuyInfo.autobuyList.size(), listcount);
        if (listcount > 0)
        {
            for(uint32 i=0; i<listcount; ++i)
            {
                AutobuyInfo.autobuyList[i] = autobuy_json["autobuyList"][i].asUInt();
            }
        }
    }

    // void field setting
    VOID_FIELD_SETTING_INFO& voidFieldInfo = player->GetVoidFieldSettingInfo();
    voidFieldInfo.Clear();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(voidfield_info);

        if (!reader.parse(voidfield_info, voidfield_json, false))
        {
            CnError("Parse player:%llu void field setting info failed\n", playerID);
            return false;
        }

        voidFieldInfo.auto_use_revival  = voidfield_json["auto_use_revival"].asUInt();
        voidFieldInfo.auto_buy_revival  = voidfield_json["auto_buy_revival"].asUInt();
        voidFieldInfo.current_revival   = voidfield_json["current_revival"].asUInt();
        voidFieldInfo.reconnect_on_loss = voidfield_json["reconnect_on_loss"].asUInt();
    }
    else // init
    {
        voidFieldInfo.auto_use_revival  = 99;
        voidFieldInfo.auto_buy_revival  = 99;
        voidFieldInfo.current_revival   = 99;
        voidFieldInfo.reconnect_on_loss = 99;
    }


    return true;

    // 加载跳舞信息
    //LoadDanceInfo(playerID);
}

//加载活跃度信息
bool PlayerLoadParser::ParseLivenessInfo( DBRecorder& res, uint64 playerID )
{
    Json::Reader reader;
    Json::Value info_json;
    std::string info_str;

    Player* player =0;
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
        return false;

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(info_str);

        if(!reader.parse(info_str, info_json, false))
        {
            CnError("Parse player:%llu liveness failed\n", playerID);
            return false;
        }

        LIVENESS_TARGET_INFO targetInfo;
        LIVENESS_INFO& liveness_info = player->GetLivenessInfo();
        liveness_info.Clear();

        uint32 count = info_json["info_list"].size();
        for(uint32 i=0; i<count; ++i)
        {
            targetInfo.Clear();
            Json::Value& val = info_json["info_list"][i];
            targetInfo.target = val["target"].asInt();
            targetInfo.cur_count = val["cur_count"].asInt();
            targetInfo.finish = val["finish"].asInt();

            liveness_info.info_list.push_back(targetInfo);
        }

        count = info_json["reward_list"].size();
        for(uint32 i=0; i<count; ++i)
        {
            Json::Value& val = info_json["reward_list"][i];

            liveness_info.reward_list.push_back(val["flag"].asInt());
        }
    }

    return true;
}

//加载副本次数信息
bool PlayerLoadParser::ParseDungeonTimesInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value info_json;
    Json::Value list_json;
    std::string list_str;
    DUNGEON_TIMES_INFO dungeonInfo;

    Player* player =0;
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(list_str);
        if (!reader.parse(list_str, list_json, false))
        {
            CnError("Parse player:%llu tower defence info failed\n", playerID);
            return false;
        }

        DUNGEON_TIMES_LIST& dungeon_times_info = player->GetDungeonTimesInfo();

        for(uint32 i = 0; i < list_json["list"].size(); ++i)
        {
            info_json = list_json["list"][i];
            dungeonInfo.dungeon_id = info_json["dungeon_id"].asInt();
            dungeonInfo.enter_count = info_json["enter_count"].asInt();
            dungeonInfo.refresh_time = info_json["refresh_time"].asInt();
            dungeon_times_info.list.push_back(dungeonInfo);
        }
    }

    return true;
}

//获得副本信息
bool PlayerLoadParser::ParsePlayerDungeonInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value info_json;
    std::string open_dungeon_str = "{}";
    std::string plot_dungeon_str = "{}";
    std::string soul_dungeon_str = "{}";
    std::string daily_dungeon_str = "{}";
    std::string dungeon_record_str = "{}";

    Player* player = NULL;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(open_dungeon_str);
        res.GetRecord(plot_dungeon_str);
        res.GetRecord(soul_dungeon_str);
        res.GetRecord(daily_dungeon_str);
        res.GetRecord(dungeon_record_str);
    }

    PLAYER_DUNGEON_INFO& dungeon_info = player->GetPlayerDungeonInfo();

    if (!JsonToStruct(dungeon_info.open_dungeon_info, open_dungeon_str))
    {
        CnError("Parse player:%llu open_dungeon_info info failed\n", playerID);
        return false;
    }

    if (!JsonToStruct(dungeon_info.plot_dungeon_info, plot_dungeon_str))
    {
        CnError("Parse player:%llu plot_dungeon_info info failed\n", playerID);
        return false;
    }

    if (!JsonToStruct(dungeon_info.soul_dungeon_info, soul_dungeon_str))
    {
        CnError("Parse player:%llu soul_dungeon_info info failed\n", playerID);
        return false;
    }

    if (!JsonToStruct(dungeon_info.daily_dungeon_info, daily_dungeon_str))
    {
        CnError("Parse player:%llu daily_dungeon_info info failed\n", playerID);
        return false;
    }

    if (!JsonToStruct(dungeon_info.dungeon_record, dungeon_record_str))
    {
        CnError("Parse player:%llu dungeon_record info failed\n", playerID);
        return false;
    }

    return true;
}

//获得购买限制
bool PlayerLoadParser::ParseLimitBuyInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value limit_info_json;
    std::string limit_info_str;
    LIMIT_BUY limit_buy_info;

    Player* player =0;
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(limit_info_str);

        if(!reader.parse(limit_info_str, limit_info_json, false))
        {
            CnError("Parse player:%llu limit buy failed\n", playerID);
            return false;
        }

        LIMIT_BUY_LIST& limit_info_list = player->GetLimitBuyInfo();
        limit_info_list.Clear();

        uint32 count = limit_info_json["malllist"].size();
        for(uint32 i=0; i<count; ++i)
        {
            limit_buy_info.Clear();
            Json::Value& val = limit_info_json["malllist"][i];

            limit_buy_info.product_id = val["product_id"].asUInt();
            limit_buy_info.num = val["num"].asUInt();
            limit_buy_info.type = val["type"].asUInt();

            limit_info_list.mall_buy_list.push_back(limit_buy_info);
        }

        uint32 countbind = limit_info_json["bindmoneylist"].size();
        for(uint32 i=0; i<countbind; ++i)
        {
            limit_buy_info.Clear();
            Json::Value& val = limit_info_json["bindmoneylist"][i];

            limit_buy_info.product_id = val["product_id"].asUInt();
            limit_buy_info.num = val["num"].asUInt();
            limit_buy_info.type = val["type"].asUInt();

            limit_info_list.bind_money_list.push_back(limit_buy_info);
        }
    }

    return true;
}
// ==================================================
// 加载资源挽回信息的回调
// ==================================================
bool PlayerLoadParser::ParseGainbackResourceInfo(DBRecorder& res, uint64 playerID)
{
    //Json::Reader reader;
    //Json::Value info_json;
    //Json::Value gb_json;
    //std::string gb_str;
    //YESTERDAY_COUNT yesterdayInfo;

    //Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    //if(!player)
    //    return false;

    //if (res.GetRowNum() > 0)
    //{
    //    res.GetRecord(gb_str);
    //    if (!reader.parse(gb_str, gb_json, false))
    //    {
    //        CnError("Parse player:%llu gain_back_resource info failed\n", playerID);
    //        return false;
    //    }

    //    GAINBACK_RESOURCE_INFO& gainback_info = player->GetGainbackResourceInfo();

    //    for(uint32 i = 0; i < gb_json["yesterday_list"].size(); ++i)
    //    {
    //        yesterdayInfo.Clear();
    //        info_json = gb_json["yesterday_list"][i];
    //        yesterdayInfo.id = info_json["id"].asInt();
    //        yesterdayInfo.count = info_json["count"].asInt();
    //        yesterdayInfo.state = info_json["state"].asInt();
    //        gainback_info.yesterday_list.push_back(yesterdayInfo);
    //    }

    //    ACTIVITY_ENTER acEnterInfo;
    //    for(uint32 i = 0; i < gb_json["activity_enter_list"].size(); ++i)
    //    {
    //        acEnterInfo.Clear();
    //        info_json = gb_json["activity_enter_list"][i];
    //        acEnterInfo.id = info_json["id"].asInt();
    //        acEnterInfo.is_enter = info_json["is_enter"].asInt();
    //        gainback_info.activity_enter_list.push_back(acEnterInfo);

    //    }
    //}
    return true;
}

// ==================================================
// 加载圣剑信息
// ==================================================
bool PlayerLoadParser::ParseSwordInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value sword_info_json;
    std::string sword_info_str;
    int32 sword_version;

    Player* player =0;
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    SWORD_INFO sword_info;
    SWORD_NEW_INFO& swordNewInfo = player->GetSwordInfo();
    swordNewInfo.Clear();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(sword_info_str);
        res.GetRecord(sword_version);

        if(sword_version == 0)
        {
            if (!reader.parse(sword_info_str, sword_info_json, false))
            {
                CnError("Parse player:%llu sword info failed\n", playerID);
                return false;
            }
            sword_info.advance          = sword_info_json["advance"].asInt();
            sword_info.star             = sword_info_json["star"].asInt();
            sword_info.star_exp         = sword_info_json["star_exp"].asInt();
            sword_info.advance_level    = sword_info_json["advance_level"].asInt();
            sword_info.advance_exp      = sword_info_json["advance_exp"].asInt();
            sword_info.battle           = sword_info_json["battle"].asInt();
            sword_info.upgrade_level    = sword_info_json["upgrade_level"].asInt();

            SWORD_ONE_INFO swordOneInfo;
            swordOneInfo.id = 1;
            swordOneInfo.level = sword_info.advance_level == 0 ? 1 : sword_info.advance_level;
            swordOneInfo.star = sword_info.star / 2;
            swordOneInfo.lock.push_back(1);
            swordOneInfo.lock.push_back(2);
            swordOneInfo.lock.push_back(3);
            swordNewInfo.sword_list.push_back(swordOneInfo);
        }
        else if(sword_version == 1)
        {
            if(!JsonToStruct(swordNewInfo, sword_info_str))
            {
                CnError("Parse player:%llu sword info failed\n", playerID);
                return false;
            }
        }
        else
            CnAssert(false);
    }

    return true;
}
//加载僵尸信息
bool PlayerLoadParser::ParseZombieInfo(DBRecorder& res, uint64 playerID)
{
    Player *player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    Json::Reader reader;
    Json::Value zombieJson;
    std::string jsonStr;

    ZOMBIE_INFO& zombieInfo = player->GetZombieInfo();
    zombieInfo.Clear();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(jsonStr);

        if (!reader.parse(jsonStr, zombieJson, false))
        {
            CnError("Parse player:%llu zombie failed\n", playerID);
            return false;
        }

        zombieInfo.enter_count = zombieJson["enter_count"].asUInt();
        zombieInfo.cumulative_total_time = zombieJson["cumulative_time"].asUInt();

        uint32 size = zombieJson["finish_dungeon"].size();
        for (uint32 i = 0; i < size; ++i)
        {
            const Json::Value& val = zombieJson["finish_dungeon"][i];
            zombieInfo.finish_zombie_dungeons.push_back(val["dungeonId"].asUInt());
        }
    }

    return true;
}

//加载礼品卡使用信息
bool PlayerLoadParser::ParseGiftCodeInfo(DBRecorder& res, uint64 playerID)
{
    Player *player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    Json::Reader reader;
    Json::Value giftJson;
    std::string jsonStr;

    GIFT_CODE_INFO& giftCodesinfo = player->GetGiftCodeInfo();
    giftCodesinfo.Clear();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(jsonStr);

        if (!reader.parse(jsonStr, giftJson, false))
        {
            CnError("Parse player:%llu giftscode info failed\n", playerID);
            return false;
        }

        uint32 size = giftJson["giftcodes"].size();
        for (uint32 i = 0; i < size; ++i)
        {
            const Json::Value& val = giftJson["giftcodes"][i];
            giftCodesinfo.use_info.push_back(val["giftcode_type"].asUInt());
        }
    }

    return true;
}

//加载公会科技信息
bool PlayerLoadParser::ParseGuildScienceInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value science_json;

    std::string science_info;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return false;

    GUILD_PLAYER_SCIENCE_INFO& guildPlayerScienceInfo = player->GetGuildPlayerScienceInfo();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(science_info);

        if (!reader.parse(science_info, science_json, false))
        {
            CnError("Parse player:%llu guild player science info failed\n", playerID);
            return false;
        }

        Json::Value addition_json;
        GUILD_PLAYER_ADDITION playerAddition;
        Json::Value guild_player_addition_list_json = science_json["guild_player_addition_list"];
        for(uint32 i = 0; i < guild_player_addition_list_json.size(); ++i)
        {
            playerAddition.Clear();
            addition_json = guild_player_addition_list_json[i];
            playerAddition.id = addition_json["id"].asInt();
            playerAddition.promote_time_gamepoint = addition_json["promote_time_gamepoint"].asInt();
            playerAddition.promote_time_moneypoint = addition_json["promote_time_moneypoint"].asInt();
            guildPlayerScienceInfo.guild_player_addition_list.push_back(playerAddition);
        }
    }

    return true;
}

// 获得收藏有理
bool PlayerLoadParser::ParseShorcutInfo( DBRecorder& res, uint64 playerID )
{
    Json::Reader reader;
    Json::Value fashion_json;

    Player* player =0;
    std::string fashion_json_info;
    FASHION_INFO fashion_info;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return false;

    SHORCUT_BAG& equip_bag = player->GetShorcutInfo();
    equip_bag.Clear();


    if (res.GetRowNum() > 0)
    {
        res.GetRecord(fashion_json_info);

        if (!reader.parse(fashion_json_info, fashion_json, false))
        {
            CnError("Parse player:%llu guild player science info failed\n", playerID);
            return false;
        }

        equip_bag.shorcut_flag = fashion_json["shorcut_info"].asUInt();
        equip_bag.taste_vip = fashion_json["taste_vip"].asUInt();
        equip_bag.like_flag = fashion_json["like_flag"].asUInt();
        equip_bag.dftime = toUint32(fashion_json["dftime"]);
    }

    return true;
}

// 解析公会信息
void PlayerLoadParser::ParseGuildInfoForMgr(DBRecorder& res,GUILD_INFO& guildInfo)
{
    // GUILD_INFO guildInfo;
    uint64 guildID = 0;

    Json::Reader reader;
    Json::Value additions_json;
    std::string additions;

    guildInfo.Clear();

    if(res.GetRowNum() < 1) return;

    guildInfo.opt_flag = OPT_INIT_FLAG;

    res.GetRecord(guildID);
    guildInfo.guild_id_h = U64ID_HIGH(guildID);
    guildInfo.guild_id_l = U64ID_LOW(guildID);

    res.GetRecord(guildInfo.guild_name);
    res.GetRecord(guildInfo.level);
    res.GetRecord(guildInfo.exp);
    res.GetRecord(additions);

    if (!reader.parse(additions, additions_json, false))
    {
        CnError("Parse guild additions failed\n");
        return;
    }

    uint32 adds_count = additions_json["additions"].size();
    for (uint32 i = 0; i < adds_count; i++)
    {
        Json::Value add_json = additions_json["additions"][i];
        GUILD_ADDITION guildAddition;
        read_GUILD_ADDITION(guildAddition, add_json);
        guildInfo.addition_list.push_back(guildAddition);
    }

    res.GetRecord(guildInfo.flag);
    res.GetRecord(guildInfo.battle_power);
    res.GetRecord(guildInfo.bulletin);
    res.GetRecord(guildInfo.guild_boss_level);
    res.GetRecord(guildInfo.guild_boss_exp);
    res.GetRecord(guildInfo.guild_boss_start_time);
    res.GetRecord(guildInfo.guild_boss_create_time);
    res.GetRecord(guildInfo.guild_boss_dead_time);
}
// 加载公会成员信息
void PlayerLoadParser::ParseGuildMemberForMgr(DBRecorder& res, GUILD_INFO& guildInfo)
{
    GUILD_MEMBER member;
    uint64 playerID = 0;
    uint64 guildID = 0;

    Json::Reader reader;
    Json::Value vip_info_json;
    std::string vip_info_str;

    for(uint32 i=0; i<res.GetRowNum(); ++i)
    {
        member.Clear();
        res.GetRecord(playerID);
        member.player_id_h = U64ID_HIGH(playerID);
        member.player_id_l = U64ID_LOW(playerID);
        res.GetRecord(member.name);
        res.GetRecord(member.sex);
        res.GetRecord(member.career);
        res.GetRecord(member.level);
        res.GetRecord(member.battle_power);

        int64 logout_time = 0;
        res.GetRecord(logout_time); // 最后在线时间
        member.last_online = (uint32)logout_time;

        res.GetRecord(vip_info_str);
        vip_info_json.clear();
        if (!reader.parse(vip_info_str, vip_info_json, false))
        {
            CnAssert(false);
        }
        ParseVipInfo(vip_info_json, member.vip_info);
        res.GetRecord(member.vip_info.growth_point_recharge);

        res.GetRecord(member.office);
        res.GetRecord(member.contribute_value);

        guildInfo.member_list.push_back(member);
    }

}

// ==================================================
// 解析VIP信息
// ==================================================
void PlayerLoadParser::ParseVipInfo(Json::Value& json_val, VIP_INFO& vipInfo)
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

bool PlayerLoadParser::ParseMultiserverBattleInfo(DBRecorder& res, uint64 playerID)
{
    Json::Reader reader;
    Json::Value item_json;

    Player* player = NULL;
    std::string item_info;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
        return false;

    MULTISERVER_BATTLE_BAG& bag_info = player->GetMultiserverBattleInfo();
    bag_info.Clear();

    // 初始的时候角色在物品表中没有数据
    if(res.GetRowNum() > 0)
    {
        res.GetRecord(item_info);

        if(!reader.parse(item_info, item_json, false))
        {
            CnError("Parse player:%llu package info failed\n", playerID);
            return false;
        }

        bag_info.integral       = toInt32(item_json["integral"]);
        bag_info.win_count      = toUint32(item_json["win_count"]);
        bag_info.lose_count     = toUint32(item_json["lose_count"]);
        bag_info.exploit_point  = toUint32(item_json["exploit_point"]);
        bag_info.alliance_point = toUint32(item_json["alliance_point"]);
        bag_info.season_point   = toUint32(item_json["season_point"]);
        bag_info.exploit_time   = toUint32(item_json["exploit_time"]);
        bag_info.alliance_time  = toUint32(item_json["alliance_time"]);
        bag_info.yesterday_count = toUint32(item_json["yesterday_count"]);
        bag_info.yesterday_3v3_count = toUint32(item_json["yesterday_3v3_count"]);
        bag_info.yesterday_integral = toUint32(item_json["yesterday_integral"]);
        bag_info.enter_num      = toUint32(item_json["enter_num"]);
        bag_info.yesterday_num  = toUint32(item_json["yesterday_num"]);
        bag_info.season_time  = toUint32(item_json["season_time"]);

        MULTISERVER_BATTLE_ITEM_INFO battleInfo;
        uint32 count = item_json["battle_info"].size();
        for(uint32 i = 0; i < count; ++i)
        {
            Json::Value& val = item_json["battle_info"][i];
            battleInfo.type         = toUint32(val["type"]);
            battleInfo.today_count  = toUint32(val["today_count"]);
            battleInfo.today_exploit = toUint32(val["today_exploit"]);
            battleInfo.last_time    = toUint32(val["last_time"]);
            bag_info.list.push_back(battleInfo);
        }

        LIMIT_BUY itemInfo;
        uint32 item_count = item_json["item_info"].size();
        for(uint32 j = 0; j < item_count; ++j)
        {
            Json::Value& val = item_json["item_info"][j];
            itemInfo.product_id = toUint32(val["product_id"]);
            itemInfo.num        = toUint32(val["num"]);
            bag_info.item_list.push_back(itemInfo);
        }
    }

    return true;
}

bool PlayerLoadParser::ParsePetFormation(DBRecorder& res, uint64 playerID)
{
    std::string pet_formation_info_str;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return false;

    PET_FORMATION_INFO& pet_formation_info = player->GetPetFormation();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(pet_formation_info_str);

        if(!JsonToStruct(pet_formation_info, pet_formation_info_str))
        {
            CnError("Parse player:%llu pet_formation_info info failed\n", playerID);
            return false;
        }
    }
    return true;
}

bool PlayerLoadParser::ParseInvestmentPlan(DBRecorder& res, uint64 playerID)
{
    std::string info_str;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return false;

    INVESTMENT_PLAN_INFO& info = player->GetInvestmentPlanInfo();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(info_str);

        if(!JsonToStruct(info, info_str))
        {
            CnError("Parse player:%llu pet_formation_info info failed\n", playerID);
            return false;
        }
    }
    return true;
}

bool PlayerLoadParser::ParseDonate( DBRecorder& res, uint64 playerID )
{
    std::string info_str;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return false;

    DONATE_INFO& info = player->GetDonateInfo();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(info_str);

        if(!JsonToStruct(info, info_str))
        {
            CnError("Parse player:%llu DonateInfo info failed\n", playerID);
            return false;
        }
    }
    return true;
}

bool PlayerLoadParser::ParseEgg( DBRecorder& res, uint64 playerID )
{
    std::string info_str;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return false;

    EGG_ACTIVITY_INFO& info = player->GetEggInfo();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(info_str);

        if(!JsonToStruct(info, info_str))
        {
            CnError("Parse player:%llu DonateInfo info failed\n", playerID);
            return false;
        }
    }
    return true;
}

bool PlayerLoadParser::ParseBuyCheap(DBRecorder &res, uint64 playerID)
{
    Player *player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if (!player)
    {
        return false;
    }

    std::string info_str;

    BUY_CHEAP_INFO& info = player->GetBuyCheapInfo();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(info_str);

        if (!JsonToStruct(info, info_str))
        {
            CnError("Parse player:%llu buy cheap info failed\n", playerID);
            return false;
        }
    }

    return true;
}

bool PlayerLoadParser::ParsePlayerStateInfo( DBRecorder& res, uint64 playerID )
{
    Player *player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if (!player)
    {
        return false;
    }

    std::string info_str;

    PLAYER_STATE_INFO& info = player->GetStateInfo();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(info_str);

        if (!JsonToStruct(info, info_str))
        {
            CnError("Parse player:%llu buy cheap info failed\n", playerID);
            return false;
        }
    }

    return true;
}

bool PlayerLoadParser::ParseHeadPortraitInfo(DBRecorder& res, uint64 playerID)
{
    Player *player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if (!player)
    {
        return false;
    }

    std::string info_str;

    HEAD_PORTRAIT_INFO& info = player->GetHeadPortraitInfo();

    if (res.GetRowNum() > 0)
    {
        res.GetRecord(info_str);

        if (!JsonToStruct(info, info_str))
        {
            CnError("Parse player:%llu head portrait failed\n", playerID);
            return false;
        }
    }

    return true;
}
