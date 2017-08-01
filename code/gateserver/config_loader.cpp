#include "gateserver_pch.h"
#include "config_loader.h"

IMPLEMENT_SINGLETON(ConfigLoader)

ConfigLoader::ConfigLoader()
{
    m_encrypt = false;          // 加密标志位
    m_center_id = 0;            // 中心服务器ID
    m_gate_id = 0;
    m_listenGate.Clear();       // 中心服务器监听Gate配置
    m_compress = false;
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
        if(Crown::SDStrcmp(i->Value(), "listen_gate") == 0)
        {
            m_listenGate.m_ip = i->Attribute("ip");
            if(i->Attribute("outer_ip") == 0)
                m_listenGate.m_outer_ip = m_listenGate.m_ip;
            else
                m_listenGate.m_outer_ip = i->Attribute("outer_ip");
            m_listenGate.m_port = SDAtoi(i->Attribute("port"));
        }
        else if(Crown::SDStrcmp(i->Value(), "encrypt") == 0)
        {
            if(Crown::SDStrcmp(i->Attribute("value"), "1") == 0)
            {
                SetEncrypt(true);
                CnInfo("Encrypt message true\n");
            }
            else
            {
                CnInfo("Encrypt message false\n");
            }
        }
        else if (Crown::SDStrcmp(i->Value(), "compress") == 0)
        {
            if (Crown::SDStrcmp(i->Attribute("value"), "1") == 0)
            {
                SetCompress(true);
                CnInfo("Compress message enable\n");
            }
            else
            {
                CnInfo("Compress message disable\n");
            }
        }
        // 监听监控端口
        else if (Crown::SDStrcmp(i->Value(), "listen_flash_security") == 0)
        {
            m_listenFlashSecurity.m_ip = i->Attribute("ip");
            if(i->Attribute("outer_ip") == 0)
                m_listenFlashSecurity.m_outer_ip = m_listenFlashSecurity.m_ip;
            else
                m_listenFlashSecurity.m_outer_ip = i->Attribute("outer_ip");
            m_listenFlashSecurity.m_port = SDAtoi(i->Attribute("port"));
        }
        else if(Crown::SDStrcmp(i->Value(), "udp_log") == 0)
        {
            m_listenUdpLog.m_ip = i->Attribute("ip");
            if(i->Attribute("outer_ip") == 0)
                m_listenUdpLog.m_outer_ip = m_listenUdpLog.m_ip;
            else
                m_listenUdpLog.m_outer_ip = i->Attribute("outer_ip");
            m_listenUdpLog.m_port = SDAtoi(i->Attribute("port"));
        }
        else if(Crown::SDStrcmp(i->Value(),"center_id") == 0)
        {
            // 等连接Center并激活后获得
        }
    }

    return true;
}
