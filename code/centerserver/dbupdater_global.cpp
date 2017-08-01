#include "centerserver_pch.h"
#include "dbupdater_global.h"
#include "dbdriver.h"
#include "config_loader.h"

IMPLEMENT_SINGLETON(DBUpdaterGlobal)

bool less_version(const DBUpdaterGlobal::UpdateContent* v1, const DBUpdaterGlobal::UpdateContent* v2)
{
    return (v1->m_version < v2->m_version);
}

DBUpdaterGlobal::UpdateContent DBUpdaterGlobal::m_update_list[] =
{
    // +==========+==========+============+==========+======+
    // | 主版本号 | 次版本号 | 修正版本号 | 执行函数 | 注释 |
    // +==========+==========+============+==========+======+
    { DB_VERSION(0, 0, 1),  &DBUpdaterGlobal::V_1, "2015-9-17 first global db" },

    // { DB_VERSION(0, 0, 44),  &DBUpdater::V_44, "Add table `activity_update`"}
    //{ NULL, "" }
};


DBUpdaterGlobal::DBUpdaterGlobal()
{
    m_running   = true;
    m_executor  = 0;
}

DBUpdaterGlobal::~DBUpdaterGlobal()
{
    UnInit();
}

bool DBUpdaterGlobal::Init(DBExecutor* executor)
{
    uint32 updateCount = 0;
    UpdateContent* v  = 0;

    if(!executor) return false;
    m_executor = executor;

    updateCount = sizeof(m_update_list)/sizeof(m_update_list[0]);
    for(uint32 i=0; i< updateCount; ++i)
    {
        v = &m_update_list[i];

        for(std::vector<UpdateContent*>::iterator it = m_sys_version_list.begin(); it != m_sys_version_list.end(); ++it)
        {
            if((*it)->m_version == v->m_version)
            {
                CnFatal("DB repeat version %u,%u,%u\n", DB_MAJOR(v->m_version), DB_MINOR(v->m_version), DB_REVISION(v->m_version));
                return false;
            }
        }

        m_sys_version_list.push_back(v);
    }

    std::sort(m_sys_version_list.begin(), m_sys_version_list.end(), less_version);

    return true;
}

void DBUpdaterGlobal::UnInit()
{
    m_executor = NULL;
}

bool DBUpdaterGlobal::CheckVersionTable()
{
    bool ret = false;

    if(HasVersionTable())
    {
        PickOutUpdateVersion();
    }
    else
    {
        AddAllUpdateVersion();
    }

    for(std::vector<UpdateContent*>::iterator it = m_update_version_list.begin(); it != m_update_version_list.end(); ++it)
    {
        CnInfo("Update DB version to: %u-%u-%u\n", 
            DB_MAJOR((*it)->m_version), DB_MINOR((*it)->m_version), DB_REVISION((*it)->m_version));

        ret = (this->*(*it)->m_func)();
        if(!ret) return false;

        ret = InsertUpdateVersion(*it);
        if(!ret) return false;
    }

    return true;
}


// ==================================================
// 检查是否存在版本表
// ==================================================
bool DBUpdaterGlobal::HasVersionTable()
{
    DBRecorder recorder;
    recorder = m_executor->Query("SHOW TABLES LIKE 'version'");
    if(recorder.HasRecord())
    {
        return true;
    }
    return false;
}

// ==================================================
// 将所有版本都添加到更新列表中
// ==================================================
void DBUpdaterGlobal::AddAllUpdateVersion()
{
    for(std::vector<UpdateContent*>::iterator it = m_sys_version_list.begin(); it != m_sys_version_list.end(); ++it)
    {
        m_update_version_list.push_back(*it);
    }
}

// ==================================================
// 挑选出更新版本
// ==================================================
void DBUpdaterGlobal::PickOutUpdateVersion()
{
    DBRecorder recorder;
    uint64 version = 0;
    uint32 major_version = 0;
    uint32 minor_version = 0;
    uint32 revision_version = 0;

    recorder = m_executor->Query("SELECT `major_version`, `minor_version`, `revision_version` FROM `version` "
        "ORDER BY `major_version` ASC, `minor_version` ASC, `revision_version` ASC");

    for(uint64 i=0; i<recorder.GetRowNum(); ++i)
    {
        recorder.GetRecord(major_version);
        recorder.GetRecord(minor_version);
        recorder.GetRecord(revision_version);
        version = DB_VERSION(major_version, minor_version, revision_version);
        m_db_version_list.push_back(version);
    }

    for(std::vector<UpdateContent*>::iterator it = m_sys_version_list.begin(); it != m_sys_version_list.end(); ++it)
    {
        if(find(m_db_version_list.begin(), m_db_version_list.end(), (*it)->m_version) == m_db_version_list.end())
        {
            m_update_version_list.push_back(*it);
        }
    }
}

// ==================================================
// 插入新的版本信息
// ==================================================
bool DBUpdaterGlobal::InsertUpdateVersion(UpdateContent* content)
{
    DBRecorder recorder;
    recorder = m_executor->Query(
        "INSERT INTO `version` SET `major_version`=%u, `minor_version`=%u, `revision_version`=%u", 
        DB_MAJOR(content->m_version), DB_MINOR(content->m_version), DB_REVISION(content->m_version));

    if(!recorder.HasExecut())
    {
        CnError("Insert DB version failed: %u-%u-%u\n", 
            DB_MAJOR(content->m_version), DB_MINOR(content->m_version), DB_REVISION(content->m_version));
        return false;
    }

    return true;
}

bool DBUpdaterGlobal::V_1()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `chat_info`;"
        "CREATE TABLE `chat_info` ("
        "`id` int(10) unsigned NOT NULL AUTO_INCREMENT,"
        "`server_id` int(10) unsigned NOT NULL,"
        "`time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',"
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`player_name` char(128) NOT NULL,"
        "`chat_type` int(10) NOT NULL,"
        "`content` text NOT NULL,"
        "`plaform_name` char(128) NOT NULL DEFAULT '""',"
        "PRIMARY KEY (`id`)"
        ") ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;


    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `gift_code`;"
        "CREATE TABLE `gift_code` ("
        "`active_code` char(255) NOT NULL DEFAULT '',"
        "`flag` int(11) NOT NULL DEFAULT '0',"
        "`player_id` bigint(20) unsigned NOT NULL DEFAULT '0',"
        "`gift_id` int(11) NOT NULL,"
        "`use_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',"
        "PRIMARY KEY (`active_code`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;


    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `gift_code_info`;"
        "CREATE TABLE `gift_code_info` ("
        "`player_id` bigint(20) NOT NULL,"
        "`gift_code_state` text,"
        "PRIMARY KEY (`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;


    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `gift_code_item`;"
        "CREATE TABLE `gift_code_item` ("
        "`uid` int(11) NOT NULL AUTO_INCREMENT,"
        "`title` text NOT NULL,"
        "`items` text NOT NULL,"
        "`createtime` timestamp NULL DEFAULT CURRENT_TIMESTAMP,"
        "PRIMARY KEY (`uid`)"
        ") ENGINE=InnoDB AUTO_INCREMENT=95 DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;


    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `global_record`;"
        "CREATE TABLE `global_record` ("
        "`key` char(128) NOT NULL,"
        "`info` text,"
        "PRIMARY KEY (`key`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;"
        );

    if(!recorder.HasExecut()) 
        return false;


    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `ranking_alliance`;"
        "CREATE TABLE `ranking_alliance` ("
        "`rank` int(10) NOT NULL DEFAULT '0',"
        "`player_id` bigint(20) unsigned NOT NULL DEFAULT '0',"
        "`sex` tinyint(4) NOT NULL,"
        "`level` int(11) NOT NULL DEFAULT '1',"
        "`vip_level` int(11) NOT NULL DEFAULT '1',"
        "`career` tinyint(4) NOT NULL DEFAULT '1',"
        "`player_name` char(255) NOT NULL DEFAULT '',"
        "`server_id` int(11) NOT NULL DEFAULT '1',"
        "`plaform_name` char(255) NOT NULL DEFAULT '',"
        "`win_pro` int(11) NOT NULL DEFAULT '0',"
        "`integral` int(11) NOT NULL DEFAULT '0',"
        "`season_time` int(11) NOT NULL DEFAULT '0',"
        "`old_rank` int(11) NOT NULL DEFAULT '0',"
        "`battle_power` int(11) NOT NULL DEFAULT '0',"
        "PRIMARY KEY (`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;"
        );

    if(!recorder.HasExecut()) 
        return false;


    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `version`;"
        "CREATE TABLE `version` ("
        "`major_version` int(10) unsigned NOT NULL DEFAULT '0',"
        "`minor_version` int(10) unsigned NOT NULL DEFAULT '0',"
        "`revision_version` int(10) unsigned NOT NULL DEFAULT '0',"
        "`update_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,"
        "PRIMARY KEY (`major_version`,`minor_version`,`revision_version`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}
