#include "robot_pch.h"
#include "robot_config.h"

IMPLEMENT_SINGLETON(RobotConfig)

RobotConfig::RobotConfig()
{
    m_PORT1 = m_PORT2 = 0;
    Clear();
}

RobotConfig::~RobotConfig()
{

}

void RobotConfig::Clear()
{
    m_file_path_map = "../res/sdb/dungeon_config.xml";
    m_center_config_path = "../res/config/center_config.xml";

#ifdef _WIN32
    m_CaseFileName = "../robot/sample9";
    m_prefix_name = "robot_";
    m_num = 1;
    m_svr_ip = "127.0.0.1";
    m_svr_port = 0;
#else
    m_CaseFileName = "../res/robot/sample9";
    m_prefix_name = "robot_";
    m_num = 1;
    m_svr_ip = "192.168.1.203";
    m_svr_port = 0;
#endif
}

bool RobotConfig::LoadConfig()
{
    if(!LoadCenterConfig())
    {
        CnError("Load center config failed\n");
        return false;
    }

    if(!LoadDungeonConfig())
    {
        CnError("Load dungeon config failed\n");
        return false;
    }

    // 技能数据
    m_PlayerSkillIDs.push_back(1001);
    m_PlayerSkillIDs.push_back(1002);
    m_PlayerSkillIDs.push_back(1003);
    m_PlayerSkillIDs.push_back(1004);
    m_PlayerSkillIDs.push_back(1005);
    m_PlayerSkillIDs.push_back(1006);

    return true;
}

bool RobotConfig::LoadDungeonConfig()
{
    TiXmlDocument xmlDocMap;

    if (!xmlDocMap.LoadFile(m_file_path_map.c_str()))
    {
        CnError("Load item file: %s failed\n", m_file_path_map.c_str());
        return false;
    }

    TiXmlElement* xmlRoot1 = xmlDocMap.RootElement();   //xml文件的根元素
    if(!xmlRoot1) return false;

    //地图配置信息
    for(TiXmlElement* mapInfo = xmlRoot1->FirstChildElement(); xmlRoot1; mapInfo = mapInfo->NextSiblingElement())
    {
        int32 gameserver_id=0;
        mapInfo->Attribute("id", &gameserver_id);        
        for(TiXmlElement* pDungeon = mapInfo->FirstChild("dungeon")->ToElement(); pDungeon; pDungeon = pDungeon->NextSiblingElement())
        {
            int32 dungeon_id=0;
            pDungeon->Attribute("id", &dungeon_id);
            int32 type=0;
            pDungeon->Attribute("type", &type);
            if(dungeon_id >= 7000)      // 7000是单机副本，8000是测试用副本
                continue;
            m_UsedDungeonIDs.push_back(dungeon_id);

            if(type != 2)
                m_WorldDungeonIDs.push_back(dungeon_id);

        }
        break;
    }

    return true;
}

bool RobotConfig::LoadCenterConfig()
{
    // 读取端口配置
    TiXmlDocument xmlCenterConfig;
    if (!xmlCenterConfig.LoadFile(m_center_config_path.c_str()))
    {
        CnError("Load item file: %s failed\n", m_center_config_path.c_str());
        return false;
    }

    TiXmlElement* xmlRoot1 = xmlCenterConfig.RootElement();   //xml文件的根元素
    if(!xmlRoot1)
        return false;

    // 读取gateserver的端口配置
    for(TiXmlElement* gateInfo = xmlRoot1->FirstChild("gateserver")->ToElement(); gateInfo; gateInfo = gateInfo->NextSiblingElement("gateserver"))
    {
        for(TiXmlElement* pServer = gateInfo->FirstChild("server")->ToElement(); pServer; pServer = pServer->NextSiblingElement("server"))
        {
            int32 id=0;
            int32 port=0;
            pServer->Attribute("id", &id);
            pServer->Attribute("port", &port);
            if(id == 1)
                m_PORT1 = port;
            else if(id == 2)
                m_PORT2 = port;
            else
                CnAssert(false);
        }
    }

    return true;
}

void RobotConfig::SetIp(const std::string& ip)
{
    m_svr_ip = ip;
}

void RobotConfig::SetPort(int32 port)
{
    m_svr_port = port;
}

void RobotConfig::SetPrefixName(const std::string& name)
{
    m_prefix_name = name;
}