#include "gameholder_pch.h"
#include "game_setting_manager.h"
#include "void_field_config.h"
#include "center_handler.h"
#include "player_manager.h"
#include "pay_point_lottery_config.h"
#include "gate_handler.h"
#include "donate_config.h"
#include "day_investment_config.h"
#include "buy_cheap_config.h"
#include "daily_fashion_config.h"


IMPLEMENT_SINGLETON(GameSettingManager)

GameSettingManager::GameSettingManager(void)
{
    m_CityRoomNum = 45;             // 主城频道人数上限
    m_world_boss_max_player = 15;   // 世界boss频道最大人数
    m_faction_pk_max_player = 20;   // 圣剑争锋频道最大人数
    m_ashura_max_player = 10;       // 修罗场频道最大人数

    m_server_open_time = 0;
    m_server_open_to_day_time = 0;

    m_chat_world_level = 1;
    m_chat_team_level = 1;
    m_chat_private_level = 1;
    m_chat_horn_level = 1;
    m_chat_guild_level = 1;

    m_screen_parameter = GAME_SCREEN_PARAMETER_1;       // 游戏屏幕大小参数
    m_quality_state = SYSTEM_SETTING_STATE_MIDDLE;      // 默认中配

    m_multiserver_enter_num = 0;
    m_multiserver_yesterday_num = 5;

    Json::Value info_json;
    GetJsonValueById(GAME_SETTING_TYPE_VOID_FIELD_MULTI, info_json);
    m_void_field_drop_chest_time = info_json["timerange"].asInt();
    m_void_field_world_level_need = info_json["world_level_need"].asInt();
    m_void_field_boss_player_num = info_json["regular_boss_player_num"].asInt();
    m_void_field_moonless_player_num = info_json["regular_moonless_player_num"].asInt();
    m_void_field_boss_channel_num = info_json["regular_boss_channel_num"].asInt();
    m_void_field_moonless_channel_num = info_json["regular_moonless_channel_num"].asInt();
    m_void_field_boss_cd_time = info_json["regular_boss_cd"].asInt();
    m_void_field_moonless_cd_time = info_json["regular_moonless_cd"].asInt();

    GetJsonValueById(GAME_SETTING_TYPE_VOID_FIELD_SOLO, info_json);
    m_void_field_solo_world_level_need = info_json["world_level_need"].asInt();
    m_void_field_solo_drop_chest_time = info_json["timerange"].asInt();
    m_void_field_solo_boss_player_num = info_json["regular_boss_player_num"].asInt();
    m_void_field_solo_boss_channel_num = info_json["regular_boss_channel_num"].asInt();
    m_void_field_solo_boss_cd_time = info_json["regular_boss_cd"].asInt();
}

GameSettingManager::~GameSettingManager(void)
{
}

void GameSettingManager::OnRecv( uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_GMT_ASK_REQ:           // 查询游戏参数
        {
            GMT_MSG* msg = (GMT_MSG*)data;
            OnAsk(msg);
        }
        break;
    case MSG_GMT_SET_REQ:           // 设置游戏参数
        {
            GMT_MSG* msg = (GMT_MSG*)data;
            OnSet(msg);
        }
        break;
    case MSG_GAME_SETTING_LIST_ACK:           // 游戏参数刷新
        {
            OnUpdateGameSettingInfo(data);
        }
        break;
    default:
        CnAssert(false);
        break;
    }

}

void GameSettingManager::OnAsk(GMT_MSG* data)
{
    CnDbg("game setting ask type=%d value=%d\n", data->type, data->value);

    GMT_MSG_SET_GAME_LIST sendMsg;

    GMT_MSG_SET_GAME gameset;
    gameset.type = GAME_SETTING_TYPE_CHANNEL_NUM;
    gameset.value = GetCityRoomNum();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_SERVER_OPEN_TIME;
    gameset.value = GetServerOpenTime();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_WORLD_BOSS_MAX_PLAYER;
    gameset.value = GetWorldBossMaxPlayer();
    sendMsg.gmtsetgame_list.push_back(gameset);

/*  gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_VOID_FIELD_TIME;
    gameset.value = GetVoidFieldDropChestTime();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_VOID_FIELD_WORLD_LEVEL_NEED;
    gameset.value = GetVoidFieldWorldLevelNeed();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_VOID_FIELD_BOSS_PLAYER_NUM;
    gameset.value = GetVoidFieldRegularBossPlayerNum();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_VOID_FIELD_MOONLESS_PLAYER_NUM;
    gameset.value = GetVoidFieldRegularMoonlessPlayerNum();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_VOID_FIELD_BOSS_CHANNEL_NUM;
    gameset.value = GetVoidFieldBossChannelNum();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_VOID_FIELD_MOONLESS_CHANNEL_NUM;
    gameset.value = GetVoidFieldMoonlessChannelNum();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_VOID_FIELD_BOSS_CD_TIME;
    gameset.value = GetVoidFieldBossCDTime();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_VOID_FIELD_MOONLESS_CD_TIME;
    gameset.value = GetVoidFieldMoonlessCDTime();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_VOID_FIELD_SOLO_TIME;
    gameset.value = GetVoidFieldSoloDropChestTime();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_VOID_FIELD_SOLO_WORLD_LEVEL_NEED;
    gameset.value = GetVoidFieldSoloWorldLevelNeed();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_VOID_FIELD_SOLO_BOSS_PLAYER_NUM;
    gameset.value = GetVoidFieldSoloRegularBossPlayerNum();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_VOID_FIELD_SOLO_BOSS_CHANNEL_NUM;
    gameset.value = GetVoidFieldSoloBossChannelNum();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_VOID_FIELD_SOLO_BOSS_CD_TIME;
    gameset.value = GetVoidFieldSoloBossCDTime();
    sendMsg.gmtsetgame_list.push_back(gameset);*/

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_FACTION_PK_MAX_PLAYER;
    gameset.value = GetFactionPKMaxPlayer();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_ASHURA_MAX_PLAYER;
    gameset.value = GetAshuraMaxPlayer();
    sendMsg.gmtsetgame_list.push_back(gameset);


    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_CHAT_WORLD_LEVEL;
    gameset.value = GetChatWorldLevel();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_CHAT_TEAM_LEVEL;
    gameset.value = GetChatTeamLevel();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_CHAT_PRIVATE_LEVEL;
    gameset.value = GetChatPrivateLevel();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_CHAT_HORN_LEVEL;
    gameset.value = GetChatHornLevel();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_CHAT_GUILD_LEVEL;
    gameset.value = GetChatGuildLevel();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_SCREEN_PARAMETER;
    gameset.value = GetScreenParameter();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_QUALITY_STATE;
    gameset.value = GetQualityState();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_MULTISERVER_ENTER_NUM;
    gameset.value = GetMultiserverEnterNum();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_MULTISERVER_YESTERDAY_NUM;
    gameset.value = GetMultiserverYesterdayNum();
    sendMsg.gmtsetgame_list.push_back(gameset);

    gameset.Clear();
    gameset.type = GAME_SETTING_TYPE_MULTISERVER_YESTERDAY_3V3_NUM;
    gameset.value = GetMultiserverYesterday3V3Num();
    sendMsg.gmtsetgame_list.push_back(gameset);

    sendMsg.type = GAME_SETTING_TYPE_VIP_LIMITED_PURCHASING_TIME;
    GMT_VALUE gmtVal;
    gmtVal.Clear();
    gmtVal.val = GetVIPLimitedPurchasingTimeStart();
    sendMsg.val_list.push_back(gmtVal);
    gmtVal.Clear();
    gmtVal.val = GetVIPLimitedPurchasingTimeEnd();
    sendMsg.val_list.push_back(gmtVal);

    CenterHandler::Instance()->SendToMasterCenter(MSG_GMT_ASK_ACK, &sendMsg);
}

void GameSettingManager::OnSet(GMT_MSG* data)
{
    CnDbg("game setting set type=%d value=%d\n", data->type, data->value);

    GMT_MSG sendMsg;
    sendMsg.type = data->type;

    switch(data->type)
    {
    case GAME_SETTING_TYPE_CHANNEL_NUM:
        {
            SetCityRoomNum(data->value);
            sendMsg.value = GetCityRoomNum();
        }
        break;
    case GAME_SETTING_TYPE_SERVER_OPEN_TIME:
        {
            SetServerOpenTime(data->value);
            sendMsg.value = GetServerOpenTime();
        }
        break;
    case GAME_SETTING_TYPE_WORLD_BOSS_MAX_PLAYER:
        {
            SetWorldBossMaxPlayer(data->value);
            sendMsg.value = GetWorldBossMaxPlayer();
        }
        break;
    case GAME_SETTING_TYPE_FACTION_PK_MAX_PLAYER:
        {
            SetFactionPKMaxPlayer(data->value);
            sendMsg.value = GetFactionPKMaxPlayer();
        }
        break;
    case GAME_SETTING_TYPE_ASHURA_MAX_PLAYER:
        {
            SetAshuraMaxPlayer(data->value);
            sendMsg.value = GetAshuraMaxPlayer();
        }
        break;
    case GAME_SETTING_TYPE_CHAT_WORLD_LEVEL:
        {
            SetChatWorldLevel(data->value);
            //sendMsg.value = GetAshuraMaxPlayer();
        }
        break;
    case GAME_SETTING_TYPE_CHAT_TEAM_LEVEL:
        {
            SetChatTeamLevel(data->value);
            //sendMsg.value = GetAshuraMaxPlayer();
        }
        break;
    case GAME_SETTING_TYPE_CHAT_PRIVATE_LEVEL:
        {
            SetChatPrivateLevel(data->value);
            //sendMsg.value = GetAshuraMaxPlayer();
        }
        break;
    case GAME_SETTING_TYPE_CHAT_HORN_LEVEL:
        {
            SetChatHornLevel(data->value);
            //sendMsg.value = GetAshuraMaxPlayer();
        }
        break;
    case GAME_SETTING_TYPE_CHAT_GUILD_LEVEL:
        {
            SetChatGuildLevel(data->value);
            //sendMsg.value = GetAshuraMaxPlayer();
        }
        break;
    case GAME_SETTING_TYPE_SCREEN_PARAMETER:
        {
            //参数必须是13-16,13表示全屏, 14分辨率为1500*800 15分辨率为1300*700,16分辨率为1200*600 
            if(data->value == GAME_SCREEN_PARAMETER_1 || data->value == GAME_SCREEN_PARAMETER_2 ||  data->value == GAME_SCREEN_PARAMETER_3 ||data->value == GAME_SCREEN_PARAMETER_4)
            {
                SetScreenParameter(data->value);

            }
        }
        break;
    case GAME_SETTING_TYPE_QUALITY_STATE:
        {
            if(data->value == SYSTEM_SETTING_STATE_MIDDLE ||
                data->value == SYSTEM_SETTING_STATE_HIGH ||
                data->value == SYSTEM_SETTING_STATE_LOW )
            {
                SetQualityState(data->value);
            }
        }
        break;
    case GAME_SETTING_TYPE_VIP_LIMITED_PURCHASING_TIME:
        {
            SetVIPLimitedPurchasingTimeStart(data->val_list[0].val);
            SetVIPLimitedPurchasingTimeEnd(data->val_list[1].val);
        }
        break;
    case GAME_SETTING_TYPE_MULTISERVER_ENTER_NUM:
        {
            SetMultiserverEnterNum(data->value);
            sendMsg.value = GetMultiserverEnterNum();
            ERROR_CODE enterreq;
            enterreq.errcode = ERR_SUCCEED;
            PlayerManager::Instance()->GmtUpdate(GAME_SETTING_TYPE_MULTISERVER_ENTER_NUM);
        }
        break;
    case GAME_SETTING_TYPE_MULTISERVER_YESTERDAY_NUM:
        {
            SetMultiserverYesterdayNum(data->value);
            sendMsg.value = GetMultiserverYesterdayNum();
            ERROR_CODE yesterdayreq;
            yesterdayreq.errcode = ERR_SUCCEED;
            PlayerManager::Instance()->GmtUpdate(GAME_SETTING_TYPE_MULTISERVER_YESTERDAY_NUM);
        }
        break;
    case GAME_SETTING_TYPE_MULTISERVER_YESTERDAY_3V3_NUM:
        {
            SetMultiserverYesterday3V3Num(data->value);
            sendMsg.value = GetMultiserverYesterday3V3Num();
            ERROR_CODE yesterday3v3req;
            yesterday3v3req.errcode = ERR_SUCCEED;
            PlayerManager::Instance()->GmtUpdate(GAME_SETTING_TYPE_MULTISERVER_YESTERDAY_3V3_NUM);
        }
        break;
    default:
        CnAssert(false);
        break;
    }

    CenterHandler::Instance()->SendToMasterCenter(MSG_GMT_SET_ACK, &sendMsg);
}


void GameSettingManager::UpdateSettingData(int32 id, Json::Value& jsonvalue)
{
    bool b_exe = false;

    if(m_gamesettingList.find(id) == m_gamesettingList.end())
    {
        m_gamesettingList[id] = jsonvalue; //没有找到
        b_exe = true;
        return;
    }

    if(!b_exe)
    {
        for (std::map<int32,Json::Value>::iterator iter = m_gamesettingList.begin(); iter != m_gamesettingList.end(); iter++)
        {
            if(iter->first == id)
            {
                iter->second = jsonvalue;
                break;
            }
        }
    }
}

void GameSettingManager::GetJsonValueById(int32 id, Json::Value& json)
{
    for (std::map<int32,Json::Value>::iterator iter = m_gamesettingList.begin(); iter != m_gamesettingList.end(); iter++)
    {
        if(iter->first == id)
        {
            json  = iter->second;
            break;
        }
    }

}
void GameSettingManager::SetServerOpenTime(uint32 time)
{
    m_server_open_time = time;

    // 明日凌晨时间点
    Crown::CSDDateTime today_time = m_server_open_time;
    today_time.IncDay(1);
    today_time.SetTime(0, 0, 0);
    m_server_open_to_day_time = (int32)today_time.GetTimeValue();
}

void GameSettingManager::OnUpdateGameSettingInfo( void* data )
{
    GAME_SETTING_LIST* infoList = (GAME_SETTING_LIST*)data;

    for(std::vector<GAME_SETTING_INFO>::iterator gsit = infoList->setting_list.begin(); gsit!= infoList->setting_list.end(); gsit++)
    {
        UpdateGameSettingInfo(*gsit);
    }

    m_GameSettingList = *infoList;
}

bool GameSettingManager::UpdateGameSettingInfo( GAME_SETTING_INFO& gameSettingInfo )
{
    Json::Reader    reader;
    Json::Value     info_json;

    if(!reader.parse(gameSettingInfo.info_json, info_json, false))
    {
        CnError("Parse gmt command failed:'%s'\n", gameSettingInfo.info_json.c_str());
        //CnAssert(false);
        return false;

    }

    UpdateSettingData(gameSettingInfo.id, info_json);

    // 更新数据到xml表
    GmtUpdateSetting(gameSettingInfo);

    // 通知玩家数据更新
    PlayerManager::Instance()->GmtUpdate(gameSettingInfo.id);

    return true;
}

void GameSettingManager::GmtUpdateSetting(const GAME_SETTING_INFO& settingInfo)
{
    Json::Value info_json;

    switch(settingInfo.id)
    {
    case GAME_SETTING_TYPE_VOID_FIELD_MULTI:
        GetJsonValueById(GAME_SETTING_TYPE_VOID_FIELD_MULTI, info_json);
        m_void_field_drop_chest_time = info_json["timerange"].asInt();
        m_void_field_world_level_need = info_json["world_level_need"].asInt();
        m_void_field_boss_player_num = info_json["regular_boss_player_num"].asInt();
        m_void_field_moonless_player_num = info_json["regular_moonless_player_num"].asInt();
        m_void_field_boss_channel_num = info_json["regular_boss_channel_num"].asInt();
        m_void_field_moonless_channel_num = info_json["regular_moonless_channel_num"].asInt();
        m_void_field_boss_cd_time = info_json["regular_boss_cd"].asInt();
        m_void_field_moonless_cd_time = info_json["regular_moonless_cd"].asInt();
        break;
    case GAME_SETTING_TYPE_VOID_FIELD_SOLO:
        GetJsonValueById(GAME_SETTING_TYPE_VOID_FIELD_SOLO, info_json);
        m_void_field_solo_world_level_need = info_json["world_level_need"].asInt();
        m_void_field_solo_drop_chest_time = info_json["timerange"].asInt();
        m_void_field_solo_boss_player_num = info_json["regular_boss_player_num"].asInt();
        m_void_field_solo_boss_channel_num = info_json["regular_boss_channel_num"].asInt();
        m_void_field_solo_boss_cd_time = info_json["regular_boss_cd"].asInt();
        break;
    case GAME_SETTING_TYPE_BUY_CHEAP:
        GetJsonValueById(GAME_SETTING_TYPE_BUY_CHEAP, info_json);
        BuyCheapConfig::Instance()->DBSetConfig("", &info_json);
        break;
    case GAME_SETTING_TYPE_DAILY_DRAW_FASHION:
        GetJsonValueById(GAME_SETTING_TYPE_DAILY_DRAW_FASHION, info_json);
        DailyFashionConfig::Instance()->DBSetConfig("", &info_json);
        break;
    }
}

GAME_SETTING_LIST& GameSettingManager::GetGameSettingList()
{
    return m_GameSettingList;
}
