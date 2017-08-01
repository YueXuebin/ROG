#include "centerserver_pch.h"
#include "dbupdater.h"
#include "dbdriver.h"
#include "config_loader.h"

IMPLEMENT_SINGLETON(DBUpdater)

bool less_version(const DBUpdater::UpdateContent* v1, const DBUpdater::UpdateContent* v2)
{
    return (v1->m_version < v2->m_version);
}

DBUpdater::UpdateContent DBUpdater::m_update_list[] =
{
    // +==========+==========+============+==========+======+
    // | 主版本号 | 次版本号 | 修正版本号 | 执行函数 | 注释 |
    // +==========+==========+============+==========+======+
    { DB_VERSION(0, 0, 1),  &DBUpdater::V_1, "demo db" },
    { DB_VERSION(0, 0, 2),  &DBUpdater::V_2, "player item db" },
    { DB_VERSION(0, 0, 3),  &DBUpdater::V_3, "player equip db" },
    { DB_VERSION(0, 0, 4),  &DBUpdater::V_4, "player skill db" },
	{ DB_VERSION(0, 0, 5),  &DBUpdater::V_5, "player passive skill db" },
    { DB_VERSION(0, 0, 6),  &DBUpdater::V_6, "friend db" },
    { DB_VERSION(0, 0, 7),  &DBUpdater::V_7, "add player_dungeon_info" },
    { DB_VERSION(0, 0, 8),  &DBUpdater::V_8, "modify player_dungeon_info" },
	{ DB_VERSION(0, 0, 9),  &DBUpdater::V_9, "add table head_portrait " },
    { DB_VERSION(0, 0, 10), &DBUpdater::V_10, "add table pet " },
    { DB_VERSION(0, 0, 11), &DBUpdater::V_11, "add table store" },
	{ DB_VERSION(0, 0, 12), &DBUpdater::V_12, "add table online_activity " },
    { DB_VERSION(0, 0, 13), &DBUpdater::V_13, "add table potion " },
    { DB_VERSION(0, 0, 14), &DBUpdater::V_14, "add table festival " },
    { DB_VERSION(0, 0, 15), &DBUpdater::V_15, "add table task " },
    { DB_VERSION(0, 0, 16), &DBUpdater::V_16, "add table mall_info " },
    { DB_VERSION(0, 0, 17), &DBUpdater::V_17, "add table player_mall "},
    { DB_VERSION(0, 0, 18), &DBUpdater::V_18, "add table wing "},
    { DB_VERSION(0, 0, 19), &DBUpdater::V_19, "add table pay_order and pay_order_recharge"},
    { DB_VERSION(0, 0, 20), &DBUpdater::V_20, "add table system_version"},
    { DB_VERSION(0, 0, 21), &DBUpdater::V_21, "modify table mall_info to add money_type field" },
    { DB_VERSION(0, 0, 22), &DBUpdater::V_22, "add table guide"},
    { DB_VERSION(0, 0, 23), &DBUpdater::V_23, "modify player table to add vigor and vigor_reftime field"},
    { DB_VERSION(0, 0, 24), &DBUpdater::V_24, "modify guide table to add open system"},
    { DB_VERSION(0, 0, 25), &DBUpdater::V_25, "modify player_dungeon_info table to add dungeon_record"},
    { DB_VERSION(0, 0, 26), &DBUpdater::V_26, "add player_mail table"},
    { DB_VERSION(0, 0, 27), &DBUpdater::V_27, "add system_mail and system_mail_flag table"},
    { DB_VERSION(0, 0, 28), &DBUpdater::V_28, "modify player table to add daily_reftime field"},
    { DB_VERSION(0, 0, 29), &DBUpdater::V_29, "add ranking_all_first/ranking_player_level/ranking_schedule/ranking_dungeon_score/ranking_achive table"},
    { DB_VERSION(0, 0, 30), &DBUpdater::V_30, "add offline_player table"},
    { DB_VERSION(0, 0, 31), &DBUpdater::V_31, "add player_package table content_dictionary table"}
};


DBUpdater::DBUpdater()
{
    m_running   = true;
    m_executor  = 0;
}

DBUpdater::~DBUpdater()
{
    UnInit();
}

bool DBUpdater::Init(DBExecutor* executor)
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

void DBUpdater::UnInit()
{
    m_executor = 0;
}

bool DBUpdater::CheckVersionTable()
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
bool DBUpdater::HasVersionTable()
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
void DBUpdater::AddAllUpdateVersion()
{
    for(std::vector<UpdateContent*>::iterator it = m_sys_version_list.begin(); it != m_sys_version_list.end(); ++it)
    {
        m_update_version_list.push_back(*it);
    }
}

// ==================================================
// 挑选出更新版本
// ==================================================
void DBUpdater::PickOutUpdateVersion()
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
bool DBUpdater::InsertUpdateVersion(UpdateContent* content)
{
    DBRecorder recorder;
    recorder = m_executor->Query(
        "INSERT INTO `version` SET `major_version`=%u, `minor_version`=%u, `revision_version`=%u, `comment`='%s' ", 
        DB_MAJOR(content->m_version), DB_MINOR(content->m_version), DB_REVISION(content->m_version), content->m_comment);

    if(!recorder.HasExecut())
    {
        CnError("Insert DB version failed: %u-%u-%u-%s\n", 
            DB_MAJOR(content->m_version), DB_MINOR(content->m_version), DB_REVISION(content->m_version), content->m_comment);
        return false;
    }

    return true;
}


bool DBUpdater::V_1()
{
    DBRecorder recorder;

    // 版本表
    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `version`;"
        "CREATE TABLE `version` ("
        "`major_version` int(10) unsigned NOT NULL DEFAULT '0',"
        "`minor_version` int(10) unsigned NOT NULL DEFAULT '0',"
        "`revision_version` int(10) unsigned NOT NULL DEFAULT '0',"
        "`update_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,"
        "`comment` char(128) NOT NULL DEFAULT ''," 
        "PRIMARY KEY (`major_version`,`minor_version`,`revision_version`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    // player表
    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `player`;"
        "CREATE TABLE `player` ("
        "`account_name` char(128) NOT NULL,"
        "`player_name` char(128) NOT NULL,"
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`platform_uid` char(128) NOT NULL DEFAULT '',"
        "`platform_uname` char(128) NOT NULL DEFAULT '',"
        "`platform` char(128) NOT NULL DEFAULT '',"
        "`platform_server_name` char(128) NOT NULL DEFAULT '',"
        "`region` int(11) NOT NULL DEFAULT '0',"
        "`sex` tinyint(4) NOT NULL DEFAULT '1',"
        "`career` tinyint(4) NOT NULL DEFAULT '1',"
        "`level` int(11) NOT NULL DEFAULT '1',"
        "`exp` int(11) NOT NULL DEFAULT '0',"
        "`money_point` int(11) NOT NULL DEFAULT '0',"
        "`game_point` int(11) NOT NULL DEFAULT '0',"
        "`hp` int(11) NOT NULL DEFAULT '0',"
        "`mp` int(11) NOT NULL DEFAULT '0',"
        "`unionmap_id` int(10) unsigned NOT NULL DEFAULT '0',"
        "`x` int(11) NOT NULL DEFAULT '0',"
        "`y` int(11) NOT NULL DEFAULT '0',"
        "`angle` int(11) NOT NULL DEFAULT '0',"
        "`right_flag` int(10) unsigned NOT NULL DEFAULT '1',"
        "`create_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',"
        "`logout_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',"
        "PRIMARY KEY (`player_id`),"
        "UNIQUE KEY `player_name_idx` (`player_name`) USING HASH,"
        "UNIQUE KEY `player_id_idx` (`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;


    return true;
}

bool DBUpdater::V_2()
{
    DBRecorder recorder;

    // item表
    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `item`;"
        "CREATE TABLE `item` ("
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`item_info` text NOT NULL,"
        "PRIMARY KEY (`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_3()
{
    DBRecorder recorder;

    // equip表
    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `equip`;"
        "CREATE TABLE `equip` ("
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`equip_info` text NOT NULL,"
        "PRIMARY KEY (`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_4()
{
    DBRecorder recorder;

    // skill表
    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `skill`;"
        "CREATE TABLE `skill` ("
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`skill_info` text NOT NULL,"
        "PRIMARY KEY (`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_5()
{
	DBRecorder recorder;

	// passive_skill表
	recorder = m_executor->Query(
		"DROP TABLE IF EXISTS `passive_skill`;"
		"CREATE TABLE `passive_skill` ("
		"`player_id` bigint(20) unsigned NOT NULL,"
		"`available_points` int(11) NOT NULL DEFAULT '0',"
		"`passive_skill_info` text NOT NULL,"
		"PRIMARY KEY (`player_id`)"
		") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
		);

	if(!recorder.HasExecut()) 
		return false;

	return true;
}

bool DBUpdater::V_6()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `friend`;"
        "CREATE TABLE `friend` ("
        "`player_id_a` bigint(20) unsigned NOT NULL,"
        "`player_id_b` bigint(20) unsigned NOT NULL"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_7()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `player_dungeon_info`;"
        "CREATE TABLE `player_dungeon_info` ("
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`plot_dungeon_info` text NOT NULL,"
        "`soul_dungeon_info` text NOT NULL ,"
        "PRIMARY KEY (`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_8()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "ALTER TABLE `player_dungeon_info`"
        "ADD COLUMN `open_dungeon_info` text NOT NULL AFTER `player_id`,"
        "ADD COLUMN `daily_dungeon_info` text NOT NULL AFTER `soul_dungeon_info`;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_9()
{
	DBRecorder recorder;

	recorder = m_executor->Query(
		"DROP TABLE IF EXISTS `head_portrait`;"
		"CREATE TABLE `head_portrait` ("
		"`player_id` bigint(20) unsigned NOT NULL,"
		"`head_portrait_open` text NOT NULL,"
		"PRIMARY KEY (`player_id`)"
		") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
		);

	if(!recorder.HasExecut()) 
		return false;

	return true;
}

bool DBUpdater::V_10()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `pet`;"
        "CREATE TABLE `pet` ("
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`mercenary_info` text NOT NULL,"
        "PRIMARY KEY (`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_11()
{
    DBRecorder recorder;
    // store表
    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `store`;"
        "CREATE TABLE `store` ("
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`item_info` text NOT NULL,"
        "PRIMARY KEY (`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_12()
{
	DBRecorder recorder;
	recorder = m_executor->Query(
		"DROP TABLE IF EXISTS `online_activity`;"
		"CREATE TABLE `online_activity` ("
		"`player_id` bigint(20) unsigned NOT NULL,"
		"`sign_info` text NOT NULL,"
		"PRIMARY KEY (`player_id`)"
		") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
		);

	if(!recorder.HasExecut()) 
		return false;

	return true;
}

bool DBUpdater::V_13()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `potion`;"
        "CREATE TABLE `potion` ("
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`potion_info` text NOT NULL,"
        "PRIMARY KEY (`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_14()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `festival`;"
        "CREATE TABLE `festival` ("
        "`id` int(11) unsigned NOT NULL AUTO_INCREMENT,"
        "`name` char(128) NOT NULL DEFAULT '',"
        "`type` tinyint(3) unsigned NOT NULL,"
        "`show_pos` tinyint(3) unsigned NOT NULL,"
        "`state` tinyint(3) unsigned NOT NULL,"
        "`sort` tinyint(3) unsigned NOT NULL,"
        "`loop_hours` int(11) unsigned NOT NULL,"
        "`begin_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',"
        "`end_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',"
        "`trigger_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',"
        "`desc` text NOT NULL,"
        "`task` text,"
        "`condition` text,"
        "`rewards` text,"
        "`servers` text NOT NULL,"
        "`flag` tinyint(3) unsigned NOT NULL DEFAULT '1',"
        "PRIMARY KEY (`id`)"
        ") ENGINE=InnoDB AUTO_INCREMENT=4682 DEFAULT CHARSET=utf8"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_15()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `task`;"
        "CREATE TABLE `task` ("
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`game_info` text NOT NULL,"
        "`festival_info` mediumtext,"
        "`guild_info` text NOT NULL,"
        "PRIMARY KEY (`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_16()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `mall_info`;"
        "CREATE TABLE `mall_info` ("
        "`product_id` int(10) unsigned NOT NULL DEFAULT '0',"
        "`product_num` int(11) NOT NULL DEFAULT '0',"
        "`sell_type` int(11) NOT NULL DEFAULT '0',"
        "`tag_type` int(11) NOT NULL DEFAULT '0',"
        "`home_page` int(11) NOT NULL DEFAULT '0',"
        "`discount` int(11) NOT NULL DEFAULT '0',"
        "`show_flag` tinyint(4) NOT NULL DEFAULT '0',"
        "`hot_flag` tinyint(4) NOT NULL DEFAULT '0',"
        "`new_flag` tinyint(4) NOT NULL DEFAULT '0',"
        "`present_flag` tinyint(4) NOT NULL DEFAULT '0',"
        "`modify_flag` tinyint(4) NOT NULL DEFAULT '0',"
        "`item_id` int(10) unsigned NOT NULL DEFAULT '0',"
        "`item_num` int(11) NOT NULL DEFAULT '0',"
        "`price` int(11) NOT NULL DEFAULT '0',"
        "`show_price` int(11) NOT NULL DEFAULT '0',"
        "`vip_price` int(11) NOT NULL DEFAULT '0',"
        "`show_vip_price` int(11) NOT NULL DEFAULT '0',"
        "`again_discount` int(11) NOT NULL DEFAULT '0',"
        "`again_price` int(11) NOT NULL DEFAULT '0',"
        "`show_again_price` int(11) NOT NULL DEFAULT '0',"
        "`again_start_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',"
        "`again_end_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',"
        "`personal_limit_num` int(11) NOT NULL DEFAULT '0',"
        "`global_limit_num` int(11) NOT NULL DEFAULT '0',"
        "`global_limit_counter` int(11) NOT NULL DEFAULT '0',"
        "`start_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',"
        "`end_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',"
        "PRIMARY KEY (`product_id`),"
        "KEY `show_flag_idx` (`show_flag`),"
        "KEY `hot_flag_idx` (`hot_flag`),"
        "KEY `new_flag_idx` (`new_flag`),"
        "KEY `present_flag_idx` (`present_flag`),"
        "KEY `modify_flag_idx` (`modify_flag`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_17()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `player_mall`;"
        "CREATE TABLE `player_mall` ("
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`mall_info` text NOT NULL,"
        "PRIMARY KEY (`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_18()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `wing`;"
        "CREATE TABLE `wing` ("
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`wing_info` text NOT NULL,"
        "PRIMARY KEY (`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_19()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `pay_order`;"
        "CREATE TABLE `pay_order` ("
        "`payid` bigint(20) unsigned NOT NULL,"
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`item_id` int(10) unsigned NOT NULL,"
        "`before_pay_moneypoint` int(11) NOT NULL DEFAULT '0',"
        "`after_pay_moneypoint` int(11) NOT NULL DEFAULT '0',"
        "`money_point` int(11) NOT NULL,"
        "`reason` int(11) NOT NULL DEFAULT '0',"
        "`paystate` int(11) NOT NULL DEFAULT '0',"
        "PRIMARY KEY (`payid`),"
        "KEY `player_id_idx` (`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `pay_order_recharge`;"
        "CREATE TABLE `pay_order_recharge` ("
        "`oid` char(128) NOT NULL,"
        "`uid` char(128) NOT NULL,"
        "`amount` char(128) NOT NULL,"
        "`coins` int(11) NOT NULL DEFAULT '0',"
        "`before_pay_moneypoint` int(11) NOT NULL DEFAULT '0',"
        "`after_pay_moneypoint` int(11) NOT NULL DEFAULT '0',"
        "`dtime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',"
        "`sign` char(128) NOT NULL,"
        "`ext` char(128) NOT NULL,"
        "`partner` char(128) NOT NULL,"
        "`serverid` char(128) NOT NULL,"
        "`paystate` int(11) NOT NULL DEFAULT '0',"
        "`pay_type` int(11) NOT NULL,"
        "PRIMARY KEY (`oid`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_20()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "DROP TABLE IF EXISTS `system_version`;"
        "CREATE TABLE `system_version` ("
        "`system_id` int(10) unsigned NOT NULL DEFAULT '0',"
        "`version` int(10) unsigned NOT NULL DEFAULT '0',"
        "PRIMARY KEY (`system_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_21()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "ALTER TABLE `mall_info`"
        "ADD COLUMN `money_type`  tinyint(5) NOT NULL DEFAULT 1 AFTER `product_num`;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_22()
{
    DBRecorder recorder;

    recorder = m_executor->Query(

        "DROP TABLE IF EXISTS `guide`;"
        "CREATE TABLE `guide` ("
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`guide_info` text NOT NULL,"
        "PRIMARY KEY (`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_23()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "ALTER TABLE `player`"
        "ADD COLUMN `vigor`  int(11) NOT NULL DEFAULT 0 AFTER `game_point`,"
        "ADD COLUMN `vigor_reftime`  int(11) NOT NULL DEFAULT 0 AFTER `vigor`;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_24()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "ALTER TABLE `guide`"
        "ADD COLUMN `open_system_info` text NOT NULL AFTER `guide_info`;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_25()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "ALTER TABLE `player_dungeon_info`"
        "ADD COLUMN `dungeon_record` text NOT NULL AFTER `daily_dungeon_info`;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_26()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "CREATE TABLE `player_mail` ("
        "`mail_id` bigint(20) unsigned NOT NULL,"
        "`receiver_id` bigint(20) unsigned NOT NULL,"
        "`sender_id` bigint(20) unsigned NOT NULL,"
        "`sender_name` char(128) NOT NULL,"
        "`order_id` bigint(20) unsigned NOT NULL DEFAULT '0',"
        "`send_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,"
        "`mail_type` tinyint(3) unsigned NOT NULL,"
        "`mail_flag` tinyint(3) unsigned NOT NULL,"
        "`title` char(128) NOT NULL,"
        "`message` text NOT NULL,"
        "`attachment_flag` tinyint(3) unsigned NOT NULL DEFAULT '0',"
        "`attachment` text NOT NULL,"
        "PRIMARY KEY (`mail_id`,`receiver_id`),"
        "KEY `receiver_id_idx` (`receiver_id`),"
        "KEY `sender_id_idx` (`sender_id`),"
        "KEY `order_id_idx` (`order_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_27()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "CREATE TABLE `system_mail` ("
        "`mail_id` bigint(20) unsigned NOT NULL,"
        "`sender_id` bigint(20) unsigned NOT NULL,"
        "`mail_type` tinyint(3) unsigned NOT NULL,"
        "`send_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,"
        "`title` char(128) NOT NULL,"
        "`message` text NOT NULL,"
        "`attachment` text NOT NULL,"
        "PRIMARY KEY (`mail_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    recorder = m_executor->Query(
        "CREATE TABLE `system_mail_flag` ("
        "`mail_id` bigint(20) unsigned NOT NULL,"
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`mail_flag` tinyint(3) unsigned NOT NULL DEFAULT '0',"
        "`attachment_flag` tinyint(3) unsigned NOT NULL DEFAULT '0',"
        "PRIMARY KEY (`mail_id`,`player_id`),"
        "KEY `player_id_idx` (`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_28()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "ALTER TABLE `player` "
        "ADD COLUMN `daily_reftime` timestamp NOT NULL DEFAULT 0 AFTER `logout_time`;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_29()
{
    DBRecorder recorder;

    recorder = m_executor->Query(
        "CREATE TABLE `ranking_all_first` ("
        "`rank_type` int(10) NOT NULL DEFAULT '0',"
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`sex` tinyint(4) NOT NULL,"
        "`level` int(11) NOT NULL DEFAULT '1',"
        "`career` tinyint(4) NOT NULL DEFAULT '1',"
        "`guild_name` char(128) NOT NULL DEFAULT '',"
        "`player_name` char(255) NOT NULL DEFAULT '',"
        "PRIMARY KEY (`rank_type`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );
    if(!recorder.HasExecut()) 
        return false;

    recorder = m_executor->Query(
        "CREATE TABLE `ranking_player_level` ("
        "`rank` int(10) NOT NULL,"
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`sex` tinyint(4) NOT NULL,"
        "`level` int(11) NOT NULL DEFAULT '1',"
        "`career` tinyint(4) NOT NULL DEFAULT '1',"
        "`guild_name` char(128) NOT NULL DEFAULT '',"
        "`player_name` char(255) NOT NULL DEFAULT '',"
        "`battle_power` int(11) NOT NULL DEFAULT '0',"
        "PRIMARY KEY (`rank`,`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    recorder = m_executor->Query(
        "CREATE TABLE `ranking_schedule` ("
        "`ranking_type` int(10) unsigned NOT NULL DEFAULT '0',"
        "`execute_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',"
        "PRIMARY KEY (`ranking_type`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );
    if(!recorder.HasExecut()) 
        return false;

    recorder = m_executor->Query(
        "CREATE TABLE `ranking_dungeon_score` ("
        "`rank` int(10) NOT NULL DEFAULT '0',"
        "`player_id` bigint(20) unsigned NOT NULL DEFAULT '0',"
        "`sex` tinyint(4) NOT NULL,"
        "`level` int(11) NOT NULL DEFAULT '1',"
        "`career` tinyint(4) NOT NULL DEFAULT '1',"
        "`guild_name` char(128) NOT NULL DEFAULT '',"
        "`score` int(11) NOT NULL DEFAULT '0',"
        "`player_name` char(255) NOT NULL DEFAULT '',"
        "PRIMARY KEY (`rank`,`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );
    if(!recorder.HasExecut()) 
        return false;

    recorder = m_executor->Query(
        "CREATE TABLE `ranking_achive` ("
        "`rank` int(10) NOT NULL DEFAULT '0',"
        "`player_id` bigint(20) unsigned NOT NULL DEFAULT '0',"
        "`sex` tinyint(4) NOT NULL,"
        "`level` int(11) NOT NULL DEFAULT '1',"
        "`career` tinyint(4) NOT NULL DEFAULT '1',"
        "`guild_name` char(128) NOT NULL DEFAULT '',"
        "`count` int(11) NOT NULL DEFAULT '0',"
        "`player_name` char(255) NOT NULL DEFAULT '',"
        "PRIMARY KEY (`rank`,`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );
    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_30()
{
    DBRecorder recorder;
    recorder = m_executor->Query(
        "CREATE TABLE `offline_player` ("
        "`player_id` bigint(20) unsigned NOT NULL,"
        "`player_name` char(128) NOT NULL DEFAULT '',"
        "`sex` tinyint(4) NOT NULL DEFAULT '0',"
        "`career` tinyint(4) NOT NULL DEFAULT '0',"
        "`battle_power` int(11) NOT NULL DEFAULT '0',"
        "`vip_level` int(11) NOT NULL DEFAULT '0',"
        "`level` int(10) unsigned NOT NULL,"
        "`exp` int(10) unsigned NOT NULL," 
        "`max_exp` int(10) unsigned NOT NULL,"
        "`equip_info` text NOT NULL,"
        "`wing` int(10) unsigned NOT NULL,"
        "`battle_power_info` text NOT NULL,"
        "`guild_name` text NOT NULL,"
        "`achive_count` int(10) unsigned NOT NULL DEFAULT 0," 
        "`score` int(10) unsigned NOT NULL DEFAULT 0," 
        "PRIMARY KEY (`player_id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
        );

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

bool DBUpdater::V_31()
{
     DBRecorder recorder;
     recorder = m_executor->Query(
         "CREATE TABLE `player_package` ("
         "`player_id` bigint(20) unsigned NOT NULL,"
         "`package_info` text NOT NULL,"
         "PRIMARY KEY (`player_id`)"
         ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
         );

     if(!recorder.HasExecut()) 
         return false;

     recorder = m_executor->Query(
         "CREATE TABLE `content_dictionary` ("
         "`id` int(10) unsigned NOT NULL,"
         "`content` text NOT NULL,"
         "PRIMARY KEY (`id`)"
         ") ENGINE=InnoDB DEFAULT CHARSET=utf8;"
         );

     if(!recorder.HasExecut()) 
         return false;

     return true;
}