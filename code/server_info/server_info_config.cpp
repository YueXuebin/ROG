#include "server_info_pch.h"
#include "server_info_config.h"

IMPLEMENT_SINGLETON(ConfigLoader)

ConfigLoader::ConfigLoader()
{
    m_myServer.Clear();         ///< 游戏服务器的Port和IP
    m_listenInfo.Clear();       ///< 中心服务器监听Info
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
        // TODO log
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    //服务器配置信息
    for(TiXmlElement* i = xmlRoot->FirstChildElement(); i; i = i->NextSiblingElement())
    {
        // 监听端口
        if(Crown::SDStrcmp(i->Value(), "listen_info") == 0)
        {
            m_listenInfo.m_ip = i->Attribute("ip");
            if(i->Attribute("outer_ip") == 0)
                m_listenInfo.m_outer_ip = m_listenInfo.m_ip;
            else
                m_listenInfo.m_outer_ip = i->Attribute("outer_ip");
            m_listenInfo.m_port = SDAtoi(i->Attribute("port"));

            //m_listenInfo.m_ip = "192.168.1.111";
            //CnInfo("ip: %s ,port: %d \n",m_listenInfo.m_ip.c_str(),m_listenInfo.m_port);
        }
  
    }

    return true;
}
