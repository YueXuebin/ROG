#include "centerserver_pch.h"
#include "game_setting_job.h"
#include "dbdriver.h"
#include "json_helper.h"
#include "game_setting_manager.h"
#include "game_common_setting.h"
#include "buy_cheap_config.h"
#include "daily_fashion_config.h"


IMPLEMENT_SINGLETON(GameSettingJob)

GameSettingJob::GameSettingJob(void)
{
}

GameSettingJob::~GameSettingJob(void)
{
}


void GameSettingJob::InitGameSettingInfo()
{
    LoadLocalGameSetting(true);
}

void GameSettingJob::LoadLocalGameSetting(bool initFlag)
{
    CnDbg("LoadLocalGameSetting %d\n", initFlag);     // 从游戏数据库加载活动数据
    DBRecorder recorder;
    recorder = m_gameExecutor->Query("SELECT `id`, `info` FROM game_setting");
    if(recorder.HasExecut())
    {
        CBLoadGameSettingInfo(recorder, initFlag, true);
    }
}

void GameSettingJob::CBLoadGameSettingInfo( DBRecorder& res, bool b_update, bool b_notify )
{
    if(!res.HasExecut())
    {
        CnError("Load festival info failed\n");
        return;  
    }

    m_gameSettingInfo.Clear();
   
    for(uint32 i=0; i< res.GetRowNum(); ++i)
    {
        GAME_SETTING_INFO info;
        res.GetRecord(info.id);                         // 设置id
        res.GetRecord(info.info_json);                  // 设置内容
        m_gameSettingInfo.setting_list.push_back(info);
    }

    if(b_update)
    {
        ParseGameSettingInfo();
    }
     else
    {
        GameSettingManager::Instance()->SetGameSettingList(m_gameSettingInfo);
        GameSettingManager::Instance()->NotifyChange();
        GameSettingManager::Instance()->SendToAllGameSvrGameSettingList();
    }
}

void GameSettingJob::SaveGameSettingInfo( GAME_SETTING_INFO& gameSettingInfo )
{
    DBRecorder recorder;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    m_gameExecutor->EscapeString(sqlTxt, gameSettingInfo.info_json.c_str(), gameSettingInfo.info_json.length());
    recorder = m_gameExecutor->Query("INSERT INTO `game_setting` VALUES(%u, '%s') ON DUPLICATE KEY UPDATE `info`='%s'", gameSettingInfo.id, sqlTxt, sqlTxt);

    if(!recorder.HasExecut())
    {
        CnError("save id:%u global setting fashion info faild\n", gameSettingInfo.id);
    }
}


// ==================================================
// 加载系统版本
// ==================================================
void GameSettingJob::LoadSystemVersion(uint32 system_id, uint32& version)
{
    DBRecorder recorder;

    recorder = m_gameExecutor->Query("SELECT `version` FROM `system_version` WHERE `system_id`=%u", system_id);
    if(recorder.HasExecut() && (recorder.GetRowNum() > 0))
    {
        recorder.GetRecord(version);
    }
}


// ==================================================
// 更新数据库中的系统版本
// ==================================================
bool GameSettingJob::UpdateSystemVersion(uint32 system_id, uint32 version)
{
    DBRecorder recorder;

    recorder = m_gameExecutor->Query("INSERT INTO `system_version` VALUES('%u', '%u') "
        "ON DUPLICATE KEY UPDATE `version`=%u", system_id, version, version);
    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}


void GameSettingJob::ParseGameSettingInfo()
{
    std::vector<int32> idList;
    idList.push_back(GAME_SETTING_TYPE_CHAT_WORLD_LEVEL);
    idList.push_back(GAME_SETTING_TYPE_DONATE);
    idList.push_back(GAME_SETTING_TYPE_VIP_STATISTICS_PLAN);
    // void field multi
    idList.push_back(GAME_SETTING_TYPE_VOID_FIELD_MULTI);
    // void field solo
    idList.push_back(GAME_SETTING_TYPE_VOID_FIELD_SOLO);
    idList.push_back(GAME_SETTING_TYPE_BUY_CHEAP);
    //idList.push_back(GAME_SETTING_TYPE_DAILY_DRAW_FASHION);

    for (uint32 i = 0; i < idList.size(); i++)
    {
        int32 curID = idList[i];
        bool haveID = false;

        for(std::vector<GAME_SETTING_INFO>::iterator gsit = m_gameSettingInfo.setting_list.begin(); gsit!= m_gameSettingInfo.setting_list.end(); gsit++)
        {
            if(curID == GAME_SETTING_TYPE_DAILY_DRAW_FASHION)
            {
                uint32 mall_version = 0;
                LoadSystemVersion(GAME_SETTING_TYPE_DAILY_DRAW_FASHION, mall_version);
                // 如果当前配置表中的版本本大于数据库中的版本，则更新数据库中的数据
                if((uint32)DailyFashionConfig::Instance()->GetDailyDrawFashion().config_version < mall_version)
                {
                    haveID = true;
                }
            }
            else if(gsit->id == curID)
            {
                haveID = true;
                //if(UpdateGameSettingInfo(*gsit))
                //{
                    //有修改才重新保存
                    //SaveGameSettingInfo(*gsit);
                //}
            }
        } 

        if(!haveID)
        {
            GAME_SETTING_INFO info;
            info.id = curID;
            UpdateGameSettingInfo(info);
            m_gameSettingInfo.setting_list.push_back(info);
            SaveGameSettingInfo(info);
        }
    }

    // 重新加载
    LoadLocalGameSetting(false);
}

bool GameSettingJob::UpdateGameSettingInfo( GAME_SETTING_INFO& gameSettingInfo )
{
    Json::Reader    reader;
    Json::Value     info_json;
    Json::FastWriter writer;
    std::string json_txt;
    
    bool updateFlag = false;

    if(!reader.parse(gameSettingInfo.info_json, info_json, false))
    {
        if(!reader.parse("{}", info_json, false))
        {
            CnError("Parse gmt command failed:'%s'\n", gameSettingInfo.info_json.c_str());
            CnAssert(false);
            return false;
        }
        
    }
    switch(gameSettingInfo.id)
    {
    case GAME_SETTING_TYPE_CHAT_WORLD_LEVEL:
        {
            if(!info_json["level"])
            {
                info_json["level"] = 1;
            }
            json_txt = writer.write(info_json);
            updateFlag = true;
        }
        break;
    case GAME_SETTING_TYPE_DONATE:
        {
            json_txt = StructToJson(GameCommonSetting::Instance()->GetDonateTmpl());
            updateFlag = true;
        }
        break;
    case GAME_SETTING_TYPE_VIP_STATISTICS_PLAN:
        {
            json_txt = StructToJson(GameCommonSetting::Instance()->GetVipStatisticsTmpl());
            updateFlag = true;
        }
        break;
    // void field multi
    case GAME_SETTING_TYPE_VOID_FIELD_MULTI:
        {
            json_txt = StructToJson(GameCommonSetting::Instance()->GetVoidFieldMultiCfg());
            updateFlag = true;
        }
        break;
    // void field solo
    case GAME_SETTING_TYPE_VOID_FIELD_SOLO:
        {
            json_txt = StructToJson(GameCommonSetting::Instance()->GetVoidFieldSoloCfg());
            updateFlag = true;
        }
        break;
    case GAME_SETTING_TYPE_BUY_CHEAP:
        {
            json_txt = StructToJson(BuyCheapConfig::Instance()->GetBuyCheaps());
            updateFlag = true;
        }
        break;
    case GAME_SETTING_TYPE_DAILY_DRAW_FASHION:
        {
            if(!UpdateSystemVersion(GAME_SETTING_TYPE_DAILY_DRAW_FASHION, DailyFashionConfig::Instance()->GetDailyDrawFashion().config_version))
            {
                CnError("Update systemID:%u version:%u failed\n", GAME_SETTING_TYPE_DAILY_DRAW_FASHION, DailyFashionConfig::Instance()->GetDailyDrawFashion().config_version);
                //return false;
            }

            json_txt = StructToJson(DailyFashionConfig::Instance()->GetDailyDrawFashion());
            updateFlag = true;
        }
        break;
    }

    gameSettingInfo.info_json = json_txt;

    return updateFlag;
}

void GameSettingJob::HandleGmtGameSetting()
{
    // 重新加载
    LoadLocalGameSetting(false);
}



