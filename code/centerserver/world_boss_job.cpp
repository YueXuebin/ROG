#include "centerserver_pch.h"
#include "world_boss_job.h"
#include "world_boss_manager.h"
#include "dbdriver.h"

IMPLEMENT_SINGLETON(WorldBossJob)

void WorldBossJob::LoadWorldBossInfo()
{
    Json::Reader reader;
    Json::Value world_boss_info_json;
    std::string world_boss_info_str;
    WORLD_BOSS_INFO worldBossInfo;
    uint32 id = 0;
    DBRecorder res;

    res = m_dbExecutor->Query("SELECT `id`, `world_boss_info` FROM world_boss");
    if(!res.HasExecut())
    {
        CnError("Load world boss info failed\n");
        return;
    }

    if(res.GetRowNum() <= 0)
    {
        return;
    }

    WORLD_BOSS_INFO_LIST& worldBossList = WorldBossManager::Instance()->GetWorldBossList();
    worldBossList.Clear();

    for(uint32 i=0; i<res.GetRowNum(); ++i)
    {
        res.GetRecord(id);
        res.GetRecord(world_boss_info_str);
        if(!reader.parse(world_boss_info_str, world_boss_info_json, false))
        {
            CnError("Parse world boss info faild\n");
            return;
        }

        uint32 worldBossList_boss_count = world_boss_info_json["boss_count"].asInt();
        for(uint32 i = 0; i < worldBossList_boss_count; ++i)
        {
            worldBossInfo.Clear();
            Json::Value& val = world_boss_info_json["list"][i];
            worldBossInfo.save_id = id;
            worldBossInfo.boss_id = val["boss_id"].asInt();
            worldBossInfo.dead_time = val["dead_time"].asUInt();
            worldBossInfo.strong_level = val["strong_level"].asInt();
            worldBossList.boss_list.push_back(worldBossInfo);
        }
    }
}

void WorldBossJob::SaveWorldBossInfo()
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value world_boss_info;
    Json::Value list_info;
    std::string world_boss_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};
    uint32 maxSaveID = 0;

    WORLD_BOSS_INFO_LIST& list = WorldBossManager::Instance()->GetWorldBossList();

    for(std::vector<WORLD_BOSS_INFO>::iterator it = list.boss_list.begin(); 
        it != list.boss_list.end(); ++it)
    {
        if(maxSaveID < it->save_id)
        {
            maxSaveID = it->save_id;
        }
    }

    // 保存的 ID 从 1 开始
    for(uint32 i=1; i<=maxSaveID; ++i)
    {
        world_boss_info.clear();

        for(std::vector<WORLD_BOSS_INFO>::iterator it = list.boss_list.begin(); 
            it != list.boss_list.end(); ++it)
        {
            if(it->save_id != i)
            {
                continue;
            }

            list_info.clear();
            list_info["boss_id"] = it->boss_id;
            list_info["dead_time"] = it->dead_time;
            list_info["strong_level"] = it->strong_level;
            world_boss_info["list"].append(list_info);
        }

        if(world_boss_info["list"].size() < 1)
        {
            continue;
        }

        world_boss_info["boss_count"] = world_boss_info["list"].size();
        world_boss_txt = writer.write(world_boss_info);
        m_dbExecutor->EscapeString(sqlTxt, world_boss_txt.c_str(), world_boss_txt.length());
        recorder = m_dbExecutor->Query("INSERT INTO `world_boss` VALUES(%u, '%s') ON DUPLICATE KEY UPDATE `world_boss_info`='%s'", i, sqlTxt, sqlTxt);

        if(!recorder.HasExecut())
        {
            CnError("save id:%u world boss info faild\n", i);
        }
    }
}
