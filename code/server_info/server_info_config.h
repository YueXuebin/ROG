/******************************************************************************
Copyright (C) Doingame Corporation. All rights reserved.
* @brief 加载监控配置
******************************************************************************/

#ifndef server_info_config_h__
#define server_info_config_h__

#include <list>
#include <string>

/**
* @file config_loader.h
* @author littlefeet
* @brief 服务器的配置信息，端口号和地址
*/
class ServerConfig
{
public:
    void Clear()
    {
        m_active = false;
        m_port = 0;
        m_ip = "";
        m_outer_ip = "";
    }
    bool m_active;
    int32 m_port;
    std::string m_ip;           // 内网IP
    std::string m_outer_ip;     // 外网IP
};

/**
* @brief 读取配置文件的类
*/
class ConfigLoader
{
public:
    ConfigLoader();
    virtual ~ConfigLoader();

     DECLARE_SINGLETON(ConfigLoader)

    /**
    * @brief 加载配置文件
    * @param[in] strFile : 配置文件路径
    * @return bool : true 成功，false失败
    */
     bool LoadConfig(const std::string& strFile);

    /**
    * @brief 获得服务器配置
    * @return ServerConfig : 服务器配置
    */
    ServerConfig& GetListenInfo() { return m_listenInfo; }


private:
    ServerConfig                m_myServer;         ///< 游戏服务器的Port和IP
    ServerConfig                m_listenInfo;       ///< 中心服务器监听Info    CenterServer
    //std::list<ServerConfig>     m_gateServer;     ///< 代理服务器的Port和IP
    //std::list<ServerConfig>     m_gameServer;     ///< 游戏服务器的Port和IP
};

#endif
