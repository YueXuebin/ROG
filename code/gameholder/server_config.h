#ifndef server_config_h__
#define server_config_h__

#include <list>
#include <string>
#include "tinyxml.h"


/**
* 服务器的配置信息，端口号和地址
*/
class ServerInfo
{
public:
    void Clear()
    {
        m_id            = 0;
        m_type          = 0;
        m_center_id     = 0;
        m_port          = 0;
        m_ip            = "";
        m_outer_ip      = "";
    }
    uint32          m_id;
    int32           m_type;
    uint32          m_center_id;
    uint32          m_port;
    std::string     m_ip;           // 内网IP
    std::string     m_outer_ip;     // 外网IP
};

/**
* @brief 读取配置文件的类
*/
class ServerConfig
{
public:
    ServerConfig();
    virtual ~ServerConfig();

    DECLARE_SINGLETON(ServerConfig)

    bool LoadConfig();

   // 要连接的CenterServer列表
    const std::vector<ServerInfo>& GetCenterList() { return m_centerList; }

    uint32 GetMyID() { return m_myServer.m_id; }
    void SetMyID(uint32 id) { m_myServer.m_id = id; }

    uint32 GetRegionID() {return CalRegionID(m_myServer.m_center_id, m_myServer.m_id);}

    // GameServer类型: GAMESVR_TYPE_INDIE(单服), GAMESVR_TYPE_CROSS(跨服)
    int32 GetMyType() { return m_myServer.m_type; }
    void SetMyType(int32 val) { m_myServer.m_type = val;}

    uint32 GetListenGatePort() { return m_myServer.m_port; }
    void SetListenGatePort(uint32 val) { m_myServer.m_port = val; }

    const std::string& GetListenGateIP() { return m_myServer.m_ip; }
    void SetListenGateIP(const std::string& val) { m_myServer.m_ip = val; }

    const std::string& GetListenGateOuterIP() {return m_myServer.m_outer_ip;}
    void SetListenGateOuterIP(const std::string& val) { m_myServer.m_outer_ip = val; }

    // 对应的CenterServerID
    uint32 GetCenterID() { return m_myServer.m_center_id; }
    void  SetCenterID(uint32 val) { m_myServer.m_center_id = val; }

    // 获得自动回存周期
    uint32  GetAutoSavePeriod() {return m_autoSavePeriod;}

    const std::string& GetLogin360Key() { return m_login360key; }

    bool IsMergeServer() {return m_IsMergeServer;}

private:
    bool LoadGameSvrConfig();                   // 加载 GameSvr 配置文件
    bool LoadCenterSvrConfig();                 // 加载 CenterSvr 配置文件
    bool LoadCrossCenterSvrConfig();            // 加载 跨服CenterServer列表配置
    bool LoadVoidFieldGroupConfig();            // 加载 跨服虚空分组列表

private:
    ServerInfo              m_myServer;         // 游戏服务器的Port和IP

    std::vector<ServerInfo> m_centerList;       // 中心服务器监听Game

    uint32                  m_autoSavePeriod;   // 自动回存周期(秒)

    std::string             m_login360key;

    bool                    m_IsMergeServer;    // 是否为合服的服务器
};

#endif
