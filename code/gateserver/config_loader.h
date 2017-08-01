#ifndef config_loader_h__
#define config_loader_h__

#include <list>
#include <string>
#include "tinyxml.h"
#include "sdu.h"

/**
* @file config_loader.h
* @brief 服务器的配置信息，端口号和地址
*/
class ServerConfig
{
public:
    void Clear()
    {
        m_active = false;
        m_id = 0;
        m_port = 0;
        m_ip = "";
        m_outer_ip = "";
    }
    bool m_active;
    uint32 m_id;
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

    // Center监听Gate的配置
    const ServerConfig& GetListenGate() { return m_listenGate; }

    // 获得服务器ID
    uint32 GetMyID() { return m_gate_id; }
    void SetMyID(uint32 val) { m_gate_id = val; }

    uint32 GetCenterID() { return m_center_id; }
    void SetCenterID(uint32 val) { m_center_id = val; }

    // 获得加密标记
    bool GetEncrypt() { return m_encrypt; }
    void SetEncrypt(bool val) { m_encrypt = val; }

    bool GetCompress() { return m_compress; }
    void SetCompress(bool val) { m_compress = val; }

    // flash 验证端口(843)的监听
    const ServerConfig& GetListenFlashSecurity() { return m_listenFlashSecurity; }

    // udp log 的监听端口
    const ServerConfig& GetListenUdpLog() { return m_listenUdpLog; }

private:
    bool                        m_encrypt;      // 加密标志位
    uint32                      m_center_id;    // 中心服务器ID
    uint32                      m_gate_id;      // 改GateServer的ID
    ServerConfig                m_listenGate;   // 中心服务器监听Gate
    bool                        m_compress;     // 压缩配置
    ServerConfig                m_listenFlashSecurity;  // 监听Flash安全检测
    ServerConfig                m_listenUdpLog;         // 监听udplog
};

#endif
