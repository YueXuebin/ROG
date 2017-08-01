#ifndef config_loader_h__
#define config_loader_h__

/**
*   数据库的配置信息
*/
class DBConfig
{
public:
    void Clear()
    {
        m_db = "";
        m_user = "";
        m_passwd = "";
        m_ip = "";
        m_port = 0;
        m_charset = "";
        m_threadNum = 0;
    }
    std::string m_db;         // 数据库名
    std::string m_user;       // 用户名
    std::string m_passwd;     // 密码
    std::string m_ip;         // 数据库的IP
    int32 m_port;             // 数据库的端口
    std::string m_charset;    // 字符集
    int32 m_threadNum;        // 数据库的线程数

    DBConfig() {Clear();}
};

/**
* 服务器的配置信息，端口号和地址
*/
class ServerConfig
{
public:
    void Clear()
    {
        m_activate = false;
        m_is_config = false;
        m_id = 0;
        m_port = 0;
        m_ip = "";              // 内网地址
        m_outer_ip = "";        // 外网地址
        m_center_id = 0;
    }
    bool        m_activate;     // 激活状态
    bool        m_is_config;    // 是否为本地配置(跨服Game为false)
    uint32      m_id;           // 服务ID
    int32       m_port;         // 端口号
    std::string m_ip;           // IP地址
    std::string m_outer_ip;     // 外网地址
    uint32      m_center_id;    // 所属中心服务器id;

    ServerConfig() {Clear();}
};

/**
* 读取配置文件的类
*/
class ConfigLoader
{
public:
    ConfigLoader();
    virtual ~ConfigLoader();

    DECLARE_SINGLETON(ConfigLoader)

    // 加载配置文件
    bool LoadConfig(const std::string& strFile);

    const std::string& GetServerPreName() { return m_server_pre_name; }

    // 获得好友机器人名称
    const std::string& GetFriendRobotName() { return m_friend_robot_name; }

    // 获得玩家数据自动回存周期
    uint32 GetAutoSavePeriod() { return m_auto_save_period; }

    // CenterServer监听Gate配置
    ServerConfig& GetListenGate() { return m_listenGate; }

    // CenterServer监听Game配置
    ServerConfig& GetListenGame() { return m_listenGame; }

    // CenterServer监听Timer配置
    ServerConfig& GetListenTimer() { return m_listenTimer; }

    // CenterServer监听Flash配置
    ServerConfig& GetListenFlash() { return m_listenFlash; }

    // CenterServer监听Info配置
    ServerConfig& GetListenInfo() { return m_listenInfo; }


    // 返回中心服务器ID
    uint32 GetCenterID() { return m_center_id; }

    // 返回登陆模式
    int32 GetLoginMode() { return m_loginmode; }

    // 360登陆验证码
    const std::string& GetLogin360Key() { return m_login_360_key; }

    // 9377登陆验证码
    const std::string& GetLogin9377Key() {return m_login_9377_key; }

    // 推送360消息线程数
    int32 GetPush360ThreadNum() { return m_push_360_thread_num; }

    // 360推送角色等级URL
    const std::string& GetPush360LevelUrl() { return m_push_360_level_url; }

    // 360推送聊天信息URL
    const std::string& GetPush360ChatUrl() { return m_push_360_chat_url; }

    // 服务器名称
    const std::string& GetServerName() { return m_server_name; }

    // 昆仑登陆API
    const std::string& GetKunlunApiUrl() { return m_kunlunapi_url; }

    // 动艺WIN日志目录
    const std::string& GetDongyiWinLogDir() { return m_dongyi_win_log_dir; }

    // 动艺LINUX日志目录
    const std::string& GetDongyiLinuxLogDir() { return m_dongyi_linux_log_dir; }

    // 昆仑WIN日志目录
    const std::string& GetKunlunWinLogDir() { return m_kunlun_win_log_dir; }

    // 昆仑LINUX日志目录
    const std::string& GetKunlunLinuxLogDir() { return m_kunlun_linux_log_dir; }

    // 昆仑PID
    uint32 GetKunlunPid() { return m_kunlun_pid; }

    // 昆仑RID
    uint32 GetKunlunRid() { return m_kunlun_rid; }

    // 昆仑登陆线程数
    int32 GetKunlunThreadNum() { return m_kunlun_thread_num; }

    // 昆仑充值监听IP
    const char* GetKunlunPayIp() { return m_kunlun_pay_ip.c_str(); }

    // 昆仑充值监听端口
    uint32 GetKunlunPayPort() { return m_kunlun_pay_port; }

    // GMT 监听 IP
    const char* GetGmtIP() { return m_gmt_ip.c_str(); }

    // GMT 端口号
    uint32 GetGmtPort() { return m_gmt_port; }

    // 返回游戏区号
    int32 GetRegionID() { return m_region_code; }

    // 返回游戏的名称
    const std::string& GetGameName() {return m_gamename;}

    // 日志的版本号
    int32 GetDongyiLogVesion() { return m_dongyi_log_vesion;}

    // log日志的最大长度
    int32 GetDongyiLogMaxLen() {return m_dongyi_log_maxlen;}

    // log更新时间
    uint32 GetDongyiLogUpdateTime() {return m_dongyi_log_updatetime;}

    // pcu 日志时间
    uint32 GetDongyiLogPcuTime() {return m_dongyi_log_pcutime;}

    // 获得数据库配置
    DBConfig& GetCenterDBConfig() { return m_center_db; }
    DBConfig& GetGmtDBConfig() { return m_gmt_db; }
    DBConfig& GetGlobalDBConfig() { return m_global_db; }

    // 激活服务器相关
    uint16 ActivateServer(uint8 type, uint32 id, uint32 centerID);
    void UnactiveServer(uint8 type, uint32 id, uint32 centerID);
    bool IsActiveAllServer();

    // 获取服务器配置
    ServerConfig* GetServerConfig(uint8 type, uint32 id);

    std::list<ServerConfig>* GetServerConfig(uint8 type);

    // 添加服务器配置项
    bool AddServerConfig(SERVER_INFO_REQ* serverInfo);

    // 得到所在平台名字
    const std::string& GetPlatformName() {return m_platform_name;}

    // 是否是跨服服务器
    bool IsCrossCenterSrv() {return m_isCrossCentersrv;}

    // 获取区服对应的GameServer列表
    const std::vector<uint32>* GetRegionGameServerIDs(uint32 regionId);

    const std::string& GetLanguage() { return m_language; }

    const std::string& GetSdbPath() { return m_sdb_path; }

    // 主播名称
    const std::string& GetAnchorName() { return m_anchor_name; }
    // 主播url
    const std::string& GetTwUrl() { return m_anchor_url; }
    // 台服视讯聊天IP
    const std::string GetTwIP() { return m_tw_ip; }
    // 台服视讯聊天端口号
    uint32 GetTwPort() { return m_tw_port; }
    // 台服视讯key
    std::string GetTwMD5Key() { return m_tw_md5_key; }
private:
    // 读取数据库的配置项
    bool ReadDBConfig(TiXmlElement* xmlEle, DBConfig& config);

    // 读取GateServer的配置
    bool ReadGateConfig(TiXmlElement* key);

    // 读取GameServer的配置
    bool ReadGameConfig(TiXmlElement* key);

    // 读取跨服GameServer的配置
    bool ReadCrossGameConfig(TiXmlElement* key);

    void SetLanguage(const std::string& language);

    void ReadServerConfig(TiXmlElement* key, ServerConfig* config);

private:
    uint32                      m_center_id;            // 中心服务器ID
    std::string                 m_server_pre_name;      // 服务器前缀名
    std::string                 m_friend_robot_name;    // 好友机器人名称
    uint32                      m_auto_save_period;     // 自动回存周期(秒)
    int32                       m_loginmode;            // 登陆模式

    std::string                 m_dongyi_win_log_dir;   // 昆仑Win登陆日志目录
    std::string                 m_dongyi_linux_log_dir; // 昆仑Linux登陆日志目录

    std::string                 m_login_360_key;        // 360登陆验证码
    int32                       m_push_360_thread_num;  // 360推送消息线程数
    std::string                 m_push_360_level_url;   // 360服务器推送角色等级URL
    std::string                 m_push_360_chat_url;    // 360服务器推送聊天URL

    std::string                 m_login_9377_key;       // 9377登陆验证码

    std::string                 m_server_name;          // 服务器名称

    std::string                 m_kunlunapi_url;        // 昆仑登陆API
    std::string                 m_kunlun_win_log_dir;   // 昆仑Win登陆日志目录
    std::string                 m_kunlun_linux_log_dir; // 昆仑Linux登陆日志目录
    uint32                      m_kunlun_pid;           // 昆仑PID
    uint32                      m_kunlun_rid;           // 昆仑RID
    int32                       m_kunlun_thread_num;    // 昆仑登陆线程数
    std::string                 m_kunlun_pay_ip;        // 昆仑充值监听IP
    uint32                      m_kunlun_pay_port;      // 昆仑充值监听端口
    std::string                 m_gmt_ip;               // GMT监听IP
    uint32                      m_gmt_port;             // GMT监听端口
    int32                       m_region_code;          // 游戏区号
    std::string                 m_gamename;             // 游戏的名称
    int32                       m_dongyi_log_vesion;    // log的版本号
    int32                       m_dongyi_log_maxlen;    // log文件的最大长度
    int32                       m_dongyi_log_updatetime;// 动艺log的更新时间
    uint32                      m_dongyi_log_pcutime;   // 动艺log记录pcu时间
    DBConfig                    m_center_db;            // 游戏数据库配置地址
    DBConfig                    m_gmt_db;               // GMT数据库配置地址
    ServerConfig                m_listenGate;           // 中心服务器监听Gate
    ServerConfig                m_listenGame;           // 中心服务器监听Game
    ServerConfig                m_listenTimer;          // 中心服务器监听Timer
    ServerConfig                m_listenFlash;          // 中心服务器监听Flash
    ServerConfig                m_listenInfo;           // 监控服务器监听Info
    std::list<ServerConfig>     m_gateServer;           // 代理服务器的Port和IP
    std::list<ServerConfig>     m_gameServer;           // 游戏服务器的Port和IP
    std::list<ServerConfig>     m_crossGameServer;      // 跨服游戏服务器的Port和IP
    DBConfig                    m_global_db;            // global数据库配置地址
    std::string                 m_platform_name;        // 平台名字
    bool                        m_isCrossCentersrv;     // 是否是跨服服务器

    std::string                 m_anchor_name;          // 主播名称
    std::string                 m_anchor_url;           // 主播url
    std::string                 m_tw_ip;                // 台服视讯IP
    uint32                      m_tw_port;              // 台服视讯端口
    std::string                 m_tw_md5_key;           // 台服视讯md5 key

    std::string m_language;                             // 语言版本
    std::string m_sdb_path;                             // 配置文件路径

    typedef std::map<uint32, std::vector<uint32> > RegionToGSMap;
    RegionToGSMap       m_RegionToGameServer;           // 各区对应的GameServerID

};

#endif // config_loader_h__
