#include "gameholder_pch.h"
#include "server_config.h"
#include "config_reader.h"
#include "void_field_manager.h"

IMPLEMENT_SINGLETON(ServerConfig)

ServerConfig::ServerConfig()
{
    m_myServer.Clear();         // 游戏服务器的Port和IP

    m_centerList.clear();       // 中心服务器监听Game

    m_autoSavePeriod = 900;     // 默认15分钟回存一次

    m_IsMergeServer = false;
}

ServerConfig::~ServerConfig()
{

}

// ==================================================
// 读取配置文件
// ==================================================
bool ServerConfig::LoadConfig()
{
    if(!LoadGameSvrConfig())
    {
        return false;
    }

    if(!LoadCenterSvrConfig())
    {
        return false;
    }

    if(!LoadVoidFieldGroupConfig())
    {
        return false;
    }

    CnAssert(GetMyType() != 0);
    if(GetMyType() == GAMESVR_TYPE_CROSS)
    {   // 跨服GameServer要读取跨服center列表
        if(!LoadCrossCenterSvrConfig())
        {
            return false;
        }
    }

    if(m_centerList.size() < 1)
    {
        CnError("No center server be config\n");
        return false;
    }

    return true;
}

// ==================================================
// 读取游戏服务器配置文件
// ==================================================
bool ServerConfig::LoadGameSvrConfig()
{
    TiXmlDocument xmlDoc;
    TiXmlElement* rooXml = NULL;
    TiXmlElement* gamesvrXml = NULL;
    uint32 serverID = 0;
    uint32 serverType = 0;

    if (!xmlDoc.LoadFile(RES_CONFIG_PATH""GAMESVR_CONFIG_FILE))
    {
        goto _ERR;
    }

    // 根节点, Gameserver节点
    if(!(rooXml = xmlDoc.RootElement()) || !(gamesvrXml = rooXml->FirstChildElement("gameserver")))
    {
        goto _ERR;
    }


    return true;

_ERR:
    CnFatal("%s load failed\n", RES_CONFIG_PATH""GAMESVR_CONFIG_FILE);
    return false;
}

// ==================================================
// 读取中心服务器配置文件
// ==================================================
bool ServerConfig::LoadCenterSvrConfig()
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(RES_CONFIG_PATH""CENTER_CONFIG_FILE))
    {
        CnFatal("Load %s failed\n", RES_CONFIG_PATH""CENTER_CONFIG_FILE);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    //服务器配置信息
    for(TiXmlElement* i = xmlRoot->FirstChildElement(); i; i = i->NextSiblingElement())
    {
        if(Crown::SDStrcmp(i->Value(), "listen_game") == 0)
        {   // 主Center监听Game配置
            ServerInfo serverInfo;
            serverInfo.Clear();
            serverInfo.m_ip = i->Attribute("ip");
            if(i->Attribute("outer_ip") == 0)
                serverInfo.m_outer_ip = serverInfo.m_ip;
            else
                serverInfo.m_outer_ip = i->Attribute("outer_ip");
            serverInfo.m_port = SDAtoi(i->Attribute("port"));
            serverInfo.m_type = CENTERSVR_TYPE_MASTER;
            m_centerList.push_back(serverInfo);
        }
        // 检查自己是非跨服Game
        else if(Crown::SDStrcmp(i->Value(), "gameserver") == 0)
        {
            for(TiXmlElement* j = i->FirstChildElement(); j; j = j->NextSiblingElement())
            {
                if(Crown::SDStrcmp(j->Value(), "server") == 0)
                {
                    uint32 id = SDAtoi(j->Attribute("id"));
                    if(id == m_myServer.m_id)
                    {
                        CnAssert(m_myServer.m_type == 0);
                        m_myServer.m_type = GAMESVR_TYPE_INDIE;
                    }
                }
            }
        }
        // 检查自己是跨服Game
        else if(Crown::SDStrcmp(i->Value(), "cross_gameserver") == 0)
        {
            for(TiXmlElement* j = i->FirstChildElement(); j; j = j->NextSiblingElement())
            {
                if(Crown::SDStrcmp(j->Value(), "server") == 0)
                {
                    uint32 id = SDAtoi(j->Attribute("id"));
                    if(id == m_myServer.m_id)
                    {
                        CnAssert(m_myServer.m_type == 0);
                        m_myServer.m_type = GAMESVR_TYPE_CROSS;
                    }
                }
            }
        }
        else if(Crown::SDStrcmp(i->Value(), "merge_server") == 0)
        {   // 是否为合服的服务器
            std::string merge_server_str = i->Attribute("value");
            if(merge_server_str == "1")
                m_IsMergeServer = true;
        }
        else if(Crown::SDStrcmp(i->Value(), "auto_save") == 0)
        {   // 自动回存周期
            m_autoSavePeriod = SDAtoi(i->Attribute("value"));
            m_autoSavePeriod /= 2;          // gameserver上自动回存周期减半
        }
        else if(Crown::SDStrcmp(i->Value(), "login_360_key") == 0)
        {   // 360平台登录key
            m_login360key = i->Attribute("value");
        }
        else if(Crown::SDStrcmp(i->Value(), "center_id") == 0)
        {
            // centerID由连接了Center并激活后,发送过来
        }
    }

    return true;
}

bool ServerConfig::LoadCrossCenterSvrConfig()
{
    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(RES_CONFIG_PATH""CROSS_CENTER_CONFIG_FILE))
    {
        CnFatal("Error: load '%s' failed\n", RES_CONFIG_PATH""CROSS_CENTER_CONFIG_FILE);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    CnAssert(m_centerList.size() == 1);     // 有且必有一个主CenterServer
    ServerInfo masterCenterInfo = m_centerList[0];


    // 读取从centerserver_list表
    TiXmlElement* CenterServerListXml = xmlRoot->FirstChildElement("centerserver_list");

    for(TiXmlElement* i = CenterServerListXml->FirstChildElement(); i; i = i->NextSiblingElement())
    {
        if(strcmp(i->Value(), "centerserver") == 0)
        {
            ServerInfo serverInfo;
            serverInfo.Clear();

            ConfigReader::Read(i, "ip", serverInfo.m_ip);       // 内网地址
            if(i->Attribute("outer_ip") == 0)                   // 外网地址
                serverInfo.m_outer_ip = serverInfo.m_ip;
            else
                serverInfo.m_outer_ip = i->Attribute("outer_ip");

            ConfigReader::Read(i, "port", serverInfo.m_port);

            if(i->Attribute("master") == 0)
                serverInfo.m_type = CENTERSVR_TYPE_WORKER;
            else
                serverInfo.m_type = CENTERSVR_TYPE_MASTER;
            
            if (i->Attribute("id"))
            {
                serverInfo.m_center_id = SDAtoi(i->Attribute("id"));
            }

            // 检查配置
            if(serverInfo.m_type == CENTERSVR_TYPE_MASTER)
            {
                if(serverInfo.m_ip == masterCenterInfo.m_ip && serverInfo.m_outer_ip == masterCenterInfo.m_outer_ip && serverInfo.m_port == masterCenterInfo.m_port)
                {
                    CnDbg("master center config again\n");
                }
                else
                {   // master center 配置两次
                    CnError("master center %s(%s):%d\n", masterCenterInfo.m_ip.c_str(), masterCenterInfo.m_outer_ip.c_str(), masterCenterInfo.m_port);
                    CnError("master center %s(%s):%d\n", serverInfo.m_ip.c_str(), serverInfo.m_outer_ip.c_str(), serverInfo.m_port);
                    CnError("master center config twice\n");
                }
                continue;
            }
            else
            {
                if(serverInfo.m_ip == masterCenterInfo.m_ip && serverInfo.m_outer_ip == masterCenterInfo.m_outer_ip && serverInfo.m_port == masterCenterInfo.m_port)
                {   // work center 和 master center 配置一样
                    CnError("master center %s(%s):%d\n", masterCenterInfo.m_ip.c_str(), masterCenterInfo.m_outer_ip.c_str(), masterCenterInfo.m_port);
                    CnError("work center %s(%s):%d\n", serverInfo.m_ip.c_str(), serverInfo.m_outer_ip.c_str(), serverInfo.m_port);
                    CnError("work center same as master center\n");
                    continue;
                }
            }

            m_centerList.push_back(serverInfo);
        }
    }

    return true;
}

bool ServerConfig::LoadVoidFieldGroupConfig()
{
    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(RES_CONFIG_PATH""CROSS_CENTER_CONFIG_FILE))
    {
        CnFatal("Error: load '%s' failed\n", RES_CONFIG_PATH""CROSS_CENTER_CONFIG_FILE);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    // 读取虚空分组
    TiXmlElement* VoidGroupXml = xmlRoot->FirstChildElement("voidfield_group");
    for(TiXmlElement* i = VoidGroupXml->FirstChildElement(); i; i = i->NextSiblingElement())
    {
        if(strcmp(i->Value(), "group") == 0)
        {
            uint32 dungeonId = 0;
            uint32 region_id_min = 0;
            uint32 region_id_max = 0;

            ConfigReader::Read(i, "dungeon_id", dungeonId);
            ConfigReader::Read(i, "region_id_min", region_id_min);
            ConfigReader::Read(i, "region_id_max", region_id_max);

            VoidFieldManager::Instance()->AddVoidFieldGroup(dungeonId, region_id_min, region_id_max);
        }
    }

    return true;

}