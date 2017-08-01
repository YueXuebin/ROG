#include "centerserver_pch.h"
#include "global_setting_job.h"
#include "global_setting_manager.h"
#include "dbdriver.h"
#include "proclamation_manager.h"

IMPLEMENT_SINGLETON(GlobalSettingJob)

void GlobalSettingJob::LoadGlobalSettingInfo()
{
    Json::Reader reader;
    Json::Value global_setting_info_json;
    std::string global_setting_info_str;
    uint32 id = 0;
    DBRecorder res;

    res = m_dbExecutor->Query("SELECT `id`, `fashion_info` FROM global_setting");
    if(!res.HasExecut())
    {
        CnError("Load global setting info failed\n");
        return;
    }

    if(res.GetRowNum() <= 0)
    {
        return;
    }

    GLOBAL_CONFIG_BAG& bag = GlobalSettingManager::Instance()->GetFashionInfoList();
    bag.Clear();

    for(uint32 i = 0; i < res.GetRowNum(); ++i)
    {
        res.GetRecord(id);
        if(id == 0)
        {
            res.GetRecord(global_setting_info_str);
            if(!reader.parse(global_setting_info_str, global_setting_info_json, false))
            {
                CnError("Parse world boss info faild\n");
                return;
            }

            PACKAGE_TIME_INFO time_info;
            uint32 count = global_setting_info_json["fashion_info"]["package_time_info"].size();
            for(uint32 i = 0; i < count; ++i)
            {
                Json::Value& val = global_setting_info_json["fashion_info"]["package_time_info"][i];
                time_info.package_id = val["package_id"].asUInt();
                time_info.item_id = val["item_id"].asUInt();
                time_info.item_time = val["item_time"].asUInt();
                bag.fashion_info.package_time.push_back(time_info);
            }
        }
    }
}

void GlobalSettingJob::SaveGlobalSettingInfo()
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value global_setting_info;
    Json::Value list_info;
    std::string global_setting_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};
    uint32 index = 0;

    GLOBAL_CONFIG_BAG& bag = GlobalSettingManager::Instance()->GetFashionInfoList();

    global_setting_info.clear();

    for (std::vector<PACKAGE_TIME_INFO>::iterator timei = bag.fashion_info.package_time.begin(); timei != bag.fashion_info.package_time.end(); ++timei)
    {
        list_info.clear();
        list_info["package_id"] = timei->package_id;
        list_info["item_id"] = timei->item_id;
        list_info["item_time"] = timei->item_time;
        global_setting_info["fashion_info"]["package_time_info"].append(list_info);
    }

    global_setting_txt = writer.write(global_setting_info);
    m_dbExecutor->EscapeString(sqlTxt, global_setting_txt.c_str(), global_setting_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `global_setting` VALUES(%u, '%s','{}') ON DUPLICATE KEY UPDATE `fashion_info`='%s'", index, sqlTxt, sqlTxt);

    if(!recorder.HasExecut())
    {
        CnError("save id:%u global setting fashion info faild\n", index);
    }
   
}

void GlobalSettingJob::LoadProclamationInfo()
{
    Json::Reader reader;
    Json::Value global_setting_info_json;
    std::string global_setting_info_str;
    uint32 id = 0;
    DBRecorder res;

    res = m_dbExecutor->Query("SELECT `id`, `proclamation_info` FROM global_setting");
    if(!res.HasExecut())
    {
        CnError("Load global_setting proclamation_info failed\n");
        return;
    }

    if(res.GetRowNum() <= 0)
    {
        return;
    }

    GLOBAL_PROCLAMATION_BAG& bag = ProclamationManager::Instance()->GetProclamationList();
    bag.Clear();

    for(uint32 i=0; i<res.GetRowNum(); ++i)
    {
        res.GetRecord(id);
        res.GetRecord(global_setting_info_str);
        if(!reader.parse(global_setting_info_str, global_setting_info_json, false))
        {
            CnError("Parse world boss info faild\n");
            return;
        }

        PROCLAMATION_INFO time_info;
        uint32 count = global_setting_info_json.size();
        for(uint32 i = 0; i < count; ++i)
        {
            Json::Value& val = global_setting_info_json[i];
            time_info.id = val["id"].asUInt();
            time_info.type = val["type"].asUInt();
            time_info.state = val["state"].asUInt();
            time_info.time = val["time"].asUInt();
            time_info.msg = (std::string)val["msg"].asString();
            bag.chat_list.push_back(time_info);
        }
    }
}