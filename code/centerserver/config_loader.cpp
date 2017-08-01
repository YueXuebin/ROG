#include "centerserver_pch.h"
#include "config_loader.h"
#include "config_reader.h"

IMPLEMENT_SINGLETON(ConfigLoader)

ConfigLoader::ConfigLoader()
{
    m_center_id             = 0;        // 中心服务器 ID
    m_server_pre_name       = "";       // 服务器前缀名
    m_friend_robot_name     = "";       // 机器人好友名
    m_auto_save_period      = 1200;     // 20分钟玩家数据自动回写DB
    m_loginmode             = 0;        // 登陆模式
    m_login_9377_key        = "";       // 9377登陆验证码
    m_login_360_key         = "";       // 360登陆验证码
    m_push_360_thread_num   = 0;        // 360推送消息线程数
    m_push_360_level_url    = "";       // 360服务器推送角色等级URL
    m_push_360_chat_url     = "";       // 360服务器推送聊天信息URL
    m_dongyi_win_log_dir    = "";       // 动艺WIN日志目录
    m_dongyi_linux_log_dir  = "";       // 动艺LINUX日志目录
    m_kunlunapi_url         = "";       // 昆仑登陆API
    m_kunlun_win_log_dir    = "";       // 昆仑WIN日志目录
    m_kunlun_linux_log_dir  = "";       // 昆仑LINUX日志目录
    m_kunlun_pid            = 0;        // 昆仑PID
    m_kunlun_rid            = 0;        // 昆仑RID
    m_kunlun_thread_num     = 0;        // 昆仑登陆线程数
    m_kunlun_pay_ip         = "";       // 昆仑充值奖励监听IP
    m_kunlun_pay_port       = 0;        // 昆仑充值监听端口
    m_region_code           = 0;        // 游戏区号
    m_gamename              = "";       // 游戏名称
    m_dongyi_log_maxlen     = 0;        // 动艺日志文件最大长度
    m_dongyi_log_updatetime = 0;        // 动艺日志更新时间
    m_dongyi_log_pcutime    = 0;        // 动艺日志pcu时间
    m_center_db.Clear();                // 数据库配置地址
    m_gmt_db.Clear();                   // 数据库配置地址
    m_listenGate.Clear();               // 中心服务器监听Gate
    m_listenGame.Clear();               // 中心服务器监听Game
    m_listenTimer.Clear();              // 中心服务器监听Timer
    m_listenFlash.Clear();              // 中心服务器监听Flash
    m_listenInfo.Clear();               // 监控服务器监听Info
    m_gateServer.clear();               // 代理服务器的Port和IP
    m_gameServer.clear();               // 游戏服务器的Port和IP
    m_global_db.Clear();                // 数据库配置地址
    m_platform_name          ="";       // 得到平台的名字

    m_anchor_name = "";                 // 主播名称
    m_anchor_url= "";                   // 主播url
    m_tw_ip = "";                       // 台服视讯IP
    m_tw_port = 0;                      // 台服视讯端口
    m_tw_md5_key = "";                  // 台服视讯md5 key

    m_dongyi_log_vesion     = 2;        // 动艺日志版本号

    m_language = "";                    // 语言版本
    m_sdb_path = "";                    // 配置文件路径
}

ConfigLoader::~ConfigLoader()
{

}

// ==================================================
// 读取配置文件
// ==================================================
bool ConfigLoader::LoadConfig(const std::string& strFile)
{
    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(strFile.c_str()))
    {
        CnError("Error: load '%s' failed\n", strFile.c_str());
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot)
    {
        CnError("Error: can't get root node in '%s' failed\n", strFile.c_str());
        return false;
    }

    //服务器配置信息
    for(TiXmlElement* i = xmlRoot->FirstChildElement(); i; i = i->NextSiblingElement())
    {
            // 机器人名称
        if (Crown::SDStrcmp(i->Value(), "language") == 0)
        {
            std::string language = i->Attribute("value");
            SetLanguage(language);
        }

        // 登陆模式
        else if (Crown::SDStrcmp(i->Value(), "center_id") == 0)
        {
            m_center_id = Crown::SDAtou(i->Attribute("value"));
        }

        // 服务器前缀名称
        else if (Crown::SDStrcmp(i->Value(), "server_pre_name") == 0)
        {
            m_server_pre_name = i->Attribute("value");
        }

        // 机器人名称
        else if (Crown::SDStrcmp(i->Value(), "friend_robot_name") == 0)
        {
            m_friend_robot_name = i->Attribute("value");
        }

        // 自动回存周期
        else if (Crown::SDStrcmp(i->Value(), "auto_save") == 0)
        {
            m_auto_save_period = SDAtoi(i->Attribute("value"));
        }

        // 登陆模式
        else if (Crown::SDStrcmp(i->Value(), "loginmode") == 0)
        {
            m_loginmode = SDAtoi(i->Attribute("value"));
        }

        // 9377登陆验证码
        else if(Crown::SDStrcmp(i->Value(), "login_9377_key") == 0)
        {
            m_login_9377_key = i->Attribute("value");
        }

        // 360登陆验证码
        else if(Crown::SDStrcmp(i->Value(), "login_360_key") == 0)
        {
            m_login_360_key = i->Attribute("value");
        }

        // 推送360消息线程数
        else if(Crown::SDStrcmp(i->Value(), "push_360_thread_num") == 0)
        {
            m_push_360_thread_num = SDAtoi(i->Attribute("value"));
        }

        // 360推送角色等级URL
        else if(Crown::SDStrcmp(i->Value(), "push_360_level_url") == 0)
        {
            m_push_360_level_url = i->Attribute("value");
        }

        // 360推送聊天信息URL
        else if(Crown::SDStrcmp(i->Value(), "push_360_chat_url") == 0)
        {
            m_push_360_chat_url = i->Attribute("value");
        }

        // 服务器名称
        else if (Crown::SDStrcmp(i->Value(), "server_name") == 0)
        {
            m_server_name = i->Attribute("value");
        }

        // 昆仑登陆接口
        else if (Crown::SDStrcmp(i->Value(), "kunlun_api_url") == 0)
        {
            m_kunlunapi_url = i->Attribute("type");
        }

        // 昆仑日志接口
        else if (Crown::SDStrcmp(i->Value(), "kunlun_log_dir") == 0)
        {
            m_kunlun_win_log_dir = i->Attribute("win");
            m_kunlun_linux_log_dir = i->Attribute("linux");
        }

        // 动艺日志接口
        else if (Crown::SDStrcmp(i->Value(), "dongyilog_log_dir") == 0)
        {
            m_dongyi_win_log_dir = i->Attribute("win");
            m_dongyi_linux_log_dir = i->Attribute("linux");
        }

        // 昆仑PID
        else if (Crown::SDStrcmp(i->Value(), "kunlun_pid") == 0)
        {
            m_kunlun_pid = Crown::SDAtou(i->Attribute("value"));
        }

        // 昆仑RID
        else if(Crown::SDStrcmp(i->Value(), "kunlun_rid") == 0)
        {
            m_kunlun_rid = Crown::SDAtou(i->Attribute("value"));
        }

        // 昆仑登陆线程数
        else if (Crown::SDStrcmp(i->Value(), "kunlun_thread_num") == 0)
        {
            m_kunlun_thread_num = SDAtoi(i->Attribute("value"));
        }

        // 昆仑充值监听端口
        else if (Crown::SDStrcmp(i->Value(), "kunlun_pay") == 0)
        {
            m_kunlun_pay_ip = i->Attribute("ip");
            m_kunlun_pay_port = Crown::SDAtou(i->Attribute("port"));
        }

        // GMT监听端口
        else if (Crown::SDStrcmp(i->Value(), "gmt") == 0)
        {
            m_gmt_ip = i->Attribute("ip");
            m_gmt_port = Crown::SDAtou(i->Attribute("port"));
        }

        // 游戏区号
        if (Crown::SDStrcmp(i->Value(), "region_id") == 0)
        {
            m_region_code = SDAtoi(i->Attribute("value"));
        }
        // 游戏的名称
        if (Crown::SDStrcmp(i->Value(), "gamename") == 0)
        {
            m_gamename = i->Attribute("value");
        }
        //平台名字
        if(Crown::SDStrcmp(i->Value(), "platform_name") == 0)
        {
            m_platform_name = i->Attribute("value");
        }

        // 动艺LOG
        if (Crown::SDStrcmp(i->Value(), "dongyi_log") == 0)
        {
            for(TiXmlElement* j = i->FirstChildElement(); j; j = j->NextSiblingElement())
            {
                if(Crown::SDStrcmp(j->Value(), "update_time") == 0)
                {   // 日志更新时间
                    m_dongyi_log_updatetime = Crown::SDAtou(j->Attribute("value"));
                }
                else if(Crown::SDStrcmp(j->Value(), "max_log_len") == 0)
                {   // 日志最大长度
                    m_dongyi_log_maxlen = SDAtoi(j->Attribute("value"));
                }
                else if(Crown::SDStrcmp(j->Value(), "pcu_time") == 0)
                {   // PCU日志记录间隔
                    m_dongyi_log_pcutime = Crown::SDAtou(j->Attribute("value"));
                }
            }
        }

        // 监听监控
        if (Crown::SDStrcmp(i->Value(), "listen_info") == 0)
        {
            ReadServerConfig(i, &m_listenInfo);
        }

        // 监听Gate
        else if(Crown::SDStrcmp(i->Value(), "listen_gate") == 0)
        {
            ReadServerConfig(i, &m_listenGate);
        }

        // 监听Game端口
        else if(Crown::SDStrcmp(i->Value(), "listen_game") == 0)
        {
            ReadServerConfig(i, &m_listenGame);
        }

        // 监听Timer端口
        else if(Crown::SDStrcmp(i->Value(), "listen_timer") == 0)
        {
            ReadServerConfig(i, &m_listenTimer);
        }

        // 监听FlashServer端口
        else if(Crown::SDStrcmp(i->Value(), "listen_flash") == 0)
        {
            ReadServerConfig(i, &m_listenFlash);
        }

        // 游戏数据库配置
        else if(Crown::SDStrcmp(i->Value(), "game_database") == 0)
        {
            if(!ReadDBConfig(i, m_center_db))
                return false;
        }

        // GMT数据库配置
        else if(Crown::SDStrcmp(i->Value(), "gmt_database") == 0)
        {
            if(!ReadDBConfig(i, m_gmt_db)) 
                return false;
        }
        
        // global数据库配置
        else if(Crown::SDStrcmp(i->Value(), "global_database") == 0)
        {
            if(!ReadDBConfig(i, m_global_db))
                return false;
        }

        // 代理服务器
        else if(Crown::SDStrcmp(i->Value(), "gateserver") == 0)
        {
            if(!ReadGateConfig(i)) 
                return false;
        }

        // 游戏服务器
        else if(Crown::SDStrcmp(i->Value(), "gameserver") == 0)
        {
            if(!ReadGameConfig(i)) 
                return false;
        }

        // 跨服游戏服务器
        else if(Crown::SDStrcmp(i->Value(), "cross_gameserver") == 0)
        {
            if(!ReadCrossGameConfig(i)) 
                return false;
        }

        // 是否是跨服服务器
        else if(Crown::SDStrcmp(i->Value(), "is_cross_centersrv") == 0)
        {
            int32 CrossSrv = 0;
            i->QueryIntAttribute("value", &CrossSrv);
            m_isCrossCentersrv = CrossSrv == 1 ? true : false;
        }

        // 主播名
        else if(Crown::SDStrcmp(i->Value(), "anchor_name") == 0)
        {
            m_anchor_name = i->Attribute("value");
        }
        // 主播url
        else if(Crown::SDStrcmp(i->Value(), "anchor_url") == 0)
        {
            m_anchor_url = i->Attribute("value");
        }
        // 台服视讯IP
        else if(Crown::SDStrcmp(i->Value(), "tw_ip") == 0)
        {
            m_tw_ip = i->Attribute("ip");
            m_tw_port = Crown::SDAtou(i->Attribute("port"));
        }
        // 台服视讯md5 key
        else if(Crown::SDStrcmp(i->Value(), "tw_md5_key") == 0)
        {
            m_tw_md5_key = i->Attribute("value");
        }
    }

    return true;
}

// ==================================================
// 读取数据库的配置项
// ==================================================
bool ConfigLoader::ReadDBConfig(TiXmlElement* xmlEle, DBConfig& config)
{
    if(!xmlEle)
    {
        CnError("Read db config failed, input null key\n");
        return false;
    }

    if(!ConfigReader::Read(xmlEle, "db", config.m_db))
        return false;

    if(!ConfigReader::Read(xmlEle, "user", config.m_user))
        return false;

    if(!ConfigReader::Read(xmlEle, "passwd", config.m_passwd))
        return false;

    if(!ConfigReader::Read(xmlEle, "ip", config.m_ip))
        return false;

    if(!ConfigReader::Read(xmlEle, "port", config.m_port))
        return false;

    if(!ConfigReader::Read(xmlEle, "charset", config.m_charset))
        return false;

    if(!ConfigReader::Read(xmlEle, "threadnum", config.m_threadNum))
        return false;

    return true;
}

// ==================================================
// 读取代理服务器的配置项
// ==================================================
bool ConfigLoader::ReadGateConfig(TiXmlElement* key)
{
    ServerConfig config;
    if(!key) return false;

    for(TiXmlElement* i = key->FirstChildElement(); i != NULL; i = i->NextSiblingElement())
    {
        config.Clear();
        config.m_is_config = true;
        config.m_id = SDAtoi(i->Attribute("id"));
        ReadServerConfig(i, &config);
        m_gateServer.push_back(config);
    }

    return true;
}

// ==================================================
// 读取游戏服务器的配置项
// ==================================================
bool ConfigLoader::ReadGameConfig(TiXmlElement* key)
{
    if(!key) return false;

    ServerConfig config;
    for(TiXmlElement* i = key->FirstChildElement(); i != NULL; i = i->NextSiblingElement())
    {
        config.Clear();
        config.m_is_config = true;
        config.m_id = SDAtoi(i->Attribute("id"));
        ReadServerConfig(i, &config);
        m_gameServer.push_back(config);

        // 读取各GameServer上的区服设置
        for(TiXmlElement* j = i->FirstChildElement(); j; j = j->NextSiblingElement())
        {
            uint32 regionID = SDAtoi(j->Attribute("id"));
            
            RegionToGSMap::iterator regionIter = m_RegionToGameServer.find(regionID);
            if(regionIter == m_RegionToGameServer.end())
            {
                std::vector<uint32> GameServerIDs;
                m_RegionToGameServer[regionID] = GameServerIDs;
            }
            m_RegionToGameServer[regionID].push_back(config.m_id);
        }
    }

    return true;
}

bool ConfigLoader::ReadCrossGameConfig(TiXmlElement* key)
{
    ServerConfig config;
    if(!key) return false;

    for(TiXmlElement* i = key->FirstChildElement(); i != NULL; i = i->NextSiblingElement())
    {
        config.Clear();
        config.m_is_config = true;
        config.m_id = SDAtoi(i->Attribute("id"));
        ReadServerConfig(i, &config);
        m_crossGameServer.push_back(config);
    }

    return true;
}

// 读取服务器IP和Port
void ConfigLoader::ReadServerConfig(TiXmlElement* key, ServerConfig* config)
{
    // 内网IP
    config->m_ip = key->Attribute("ip");
    // 外网IP
    if(key->Attribute("outer_ip") == 0)
        config->m_outer_ip = config->m_ip;        // 没有配置,则外网地址=内网地址
    else
        config->m_outer_ip = key->Attribute("outer_ip");
    // 监听端口
    std::string port = key->Attribute("port");
    config->m_port = SDAtoi(port.c_str());
    config->m_center_id = m_center_id;
}

// ==================================================
// 激活服务器
// ==================================================
uint16 ConfigLoader::ActivateServer(uint8 type, uint32 id, uint32 centerID)
{
    std::list<ServerConfig>* list = nullptr;

    switch(type)
    {
    case SERVER_TYPE_GATE:
        list = &m_gateServer;
        break;
    case SERVER_TYPE_GAME:
        list = &m_gameServer;
        break;
    case SERVER_TYPE_FLASH:
        {
            if(m_listenFlash.m_activate)
            {
                return ERR_SERVER_ACTIVATED;
            }
            m_listenFlash.m_activate = true;
            return ERR_SUCCEED;
        }
        break;
    default:
        CnAssert(false);
        return ERR_FAILED;
        break;
    }

    for(std::list<ServerConfig>::iterator it=list->begin(); it != list->end(); ++it)
    {
        if(it->m_id != id || it->m_center_id != centerID)
            continue;                       // id 不相同继续查询
        
        if(it->m_activate)
        {
            CnWarn("Server has been activated type=%u, center=%u id=%u %s(%s):%d\n", type, centerID, id, it->m_ip.c_str(), it->m_outer_ip.c_str(), it->m_port);
            return ERR_SERVER_ACTIVATED;    // 如果处于已激活状态，则激活失败
        }
        it->m_activate = true;              // 在配置列表中将服务器设置为激活状态
        CnDbg("active server type=%d center=%d id=%d %s(%s):%d\n", type, centerID, id, it->m_ip.c_str(), it->m_outer_ip.c_str(), it->m_port);
        return ERR_SUCCEED;
    }

    CnError("Server type:%u, id:%u can't find\n", type, id);
    return ERR_UNFOUND_CONFIG;
}

void ConfigLoader::UnactiveServer(uint8 type, uint32 id, uint32 centerID)
{
    std::list<ServerConfig>* list = nullptr;

    switch(type)
    {
    case SERVER_TYPE_GATE:
        list = &m_gateServer;
        break;
    case SERVER_TYPE_GAME:
        list = &m_gameServer;
        break;
    case SERVER_TYPE_FLASH:
        {
            CnAssert(m_listenFlash.m_activate);
            m_listenFlash.m_activate = false;
            return;
        }
        break;
    default:
        CnAssert(false);
        return;
        break;
    }

    for(std::list<ServerConfig>::iterator it=list->begin(); it != list->end(); ++it)
    {
        // id 不相同继续查询
        if(it->m_id != id || it->m_center_id != centerID)
            continue;

        // 在配置列表中将服务器设置为激活状态
        CnAssert(it->m_activate);
        it->m_activate = false;
        CnDbg("unactive server type=%d center=%d id=%d %s:%d\n", type, centerID, id, it->m_ip.c_str(), it->m_port);

        if((type == SERVER_TYPE_GAME) && !it->m_is_config)
        {
            list->erase(it);
        }

        return;
    }
}
// 检查是否全部服务器都激活了
bool ConfigLoader::IsActiveAllServer()
{
    std::list<ServerConfig>::iterator it;
    for(it=m_gateServer.begin(); it != m_gateServer.end(); ++it)
    {
        if(!it->m_activate)
            return false;
    }
    
    for(it=m_gameServer.begin(); it != m_gameServer.end(); ++it)
    {
        if(!it->m_activate)
            return false;
    }
    return true;
}

// ==================================================
// 获得服务器配置
// ==================================================
ServerConfig* ConfigLoader::GetServerConfig(uint8 type, uint32 id)
{
    std::list<ServerConfig>* list = nullptr;

    if(SERVER_TYPE_GATE == type)
    {
        list = &m_gateServer;
    }
    else if(SERVER_TYPE_GAME == type)
    {
        list = &m_gameServer;
    }
    else
    {
        CnAssert(false);
        return nullptr;
    }

    for(std::list<ServerConfig>::iterator it=list->begin(); it != list->end(); ++it)
    {
        if(it->m_id == id)
            return &(*it);
    }

    return nullptr;
}

// ==================================================
// 获得服务器列表
// ==================================================
std::list<ServerConfig>* ConfigLoader::GetServerConfig(uint8 type)
{
    std::list<ServerConfig>* list = 0;

    switch(type)
    {
    case SERVER_TYPE_GATE:
        list = &m_gateServer;
        break;
    case SERVER_TYPE_GAME:
        list = &m_gameServer;
        break;
    default:
        CnAssert(false);
        break;
    }

    return list;
}

// ==================================================
// 获得服务器列表
// ==================================================
bool ConfigLoader::AddServerConfig(SERVER_INFO_REQ* serverInfo)
{
    std::list<ServerConfig>* list = NULL;
    ServerConfig config;

    if(!serverInfo)
    {
        return false;
    }

    CnAssert(serverInfo->center_id);

    // 目前只有 GameServer 才会动态添加配置项
    if(SERVER_TYPE_GAME == serverInfo->server_type)
    {
        list = &m_gameServer;
    }
    else if(SERVER_TYPE_GATE == serverInfo->server_type)
    {
        list = &m_gateServer;
    }
    else
        CnAssert(false);

    // 检查是否已存在此配置信息（不应该出现，除非同一个id跨服的服务器开启了2次）
    for(std::list<ServerConfig>::iterator it=list->begin(); it != list->end(); ++it)
    {
        if(it->m_id == serverInfo->server_id && it->m_center_id == serverInfo->center_id)
        {
            CnAssert(false);
            CnWarn("AddServerConfig duplicate server id=%d old %s(%s):%d\n", it->m_id, it->m_ip.c_str(), it->m_outer_ip.c_str(), it->m_port);
            CnWarn("AddServerConfig duplicate server id=%d new %s(%s):%d\n", serverInfo->server_id, serverInfo->server_ip.c_str(), serverInfo->server_outer_ip.c_str(), serverInfo->server_port);
            it->m_id    = serverInfo->server_id;
            it->m_port  = serverInfo->server_port;
            it->m_ip    = serverInfo->server_ip;
            it->m_outer_ip  = serverInfo->server_outer_ip;
            it->m_center_id = serverInfo->center_id;
            return true;
        }
    }

    // 查找该跨服GameServer的配置
    const ServerConfig* thisConfig;
    for(std::list<ServerConfig>::iterator cgIter=m_crossGameServer.begin(); cgIter != m_crossGameServer.end(); ++cgIter)
    {
        if(cgIter->m_id == serverInfo->server_id)
        {
            thisConfig = &(*cgIter);
            break;
        }
    }

    CnAssert(thisConfig->m_port != 0);

    if(serverInfo->server_port == 0)
    {   // 由主CenterServer来确定跨服GameServer的ip和port
        serverInfo->server_port = thisConfig->m_port;
        serverInfo->server_ip = thisConfig->m_ip;       // 监听内网地址
        serverInfo->server_outer_ip = thisConfig->m_outer_ip;       // 外网地址
        CnDbg("read config %s(%s):%d\n", thisConfig->m_ip.c_str(), thisConfig->m_outer_ip.c_str(), thisConfig->m_port);
    }
    else
    {   // 非主CenterServer
    }


    CnAssert(serverInfo->server_port != 0);
    CnAssert(serverInfo->server_ip.length() != 0);      // 没有找到对应的跨服配置
    CnAssert(serverInfo->server_outer_ip.length() != 0);

    // 添加配置项
    config.m_id     = serverInfo->server_id;
    config.m_port   = serverInfo->server_port;
    config.m_ip     = serverInfo->server_ip;
    config.m_outer_ip = serverInfo->server_outer_ip;
    config.m_center_id = serverInfo->center_id;
    list->push_back(config);

    CnInfo("add server config active=%d %s(%s):%d\n", config.m_activate?1:0, config.m_ip.c_str(), config.m_outer_ip.c_str(), config.m_port);

    return true;
}

const std::vector<uint32>* ConfigLoader::GetRegionGameServerIDs(uint32 regionId)
{
    RegionToGSMap::iterator regionIter = m_RegionToGameServer.find(regionId);
    if(regionIter == m_RegionToGameServer.end())
        return NULL;
    else
        return &regionIter->second;
}

void ConfigLoader::SetLanguage( const std::string& language )
{
    if(language.length() == 0)          // 默认中文版本
        m_language = "zh_cn";
    else
        m_language = language;

    if(m_language == "en")
    {
        m_sdb_path = "../res/sdb_en/";
    }
    else if(m_language == "tw")
    {
        m_sdb_path = "../res/sdb_tw/";
    }
    else if(m_language == "ina")
    {
        m_sdb_path = "../res/sdb_ina/";
    }
    else if(m_language == "pt")
    {
        m_sdb_path = "../res/sdb_pt/";
    }
    else if(m_language == "sp")
    {
        m_sdb_path = "../res/sdb_sp/";
    }
    else
    {
        m_sdb_path = "../res/sdb/";
    }

    CnInfo("language=%s\n", m_language.c_str());
    CnInfo("sdb_path=%s\n", m_sdb_path.c_str());
}
