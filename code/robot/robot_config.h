/*
 *	机器人配置
 */
#ifndef robot_config_h__
#define robot_config_h__

class RobotConfig
{
public:
    RobotConfig();
    virtual ~RobotConfig();

    DECLARE_SINGLETON(RobotConfig)

    void Clear();

    bool LoadConfig();

    std::vector<uint32>& GetWorldMapDungoen() {return m_WorldDungeonIDs;}
    std::vector<uint32>& GetConfigFileMap(){return m_UsedDungeonIDs;}
    std::vector<uint32>& GetSkillIDs() {return m_PlayerSkillIDs;}

    void SetNumber(int32 n) {m_num = n;}
    int32 GetNumber() {return m_num;}

    void SetIp(const std::string& ip);
    std::string GetSvrIP() { return m_svr_ip; }

    void SetPort(int32 port);
    int32 GetSvrPort() { return m_svr_port; }

    void SetCaseFileName(const std::string& filename) {m_CaseFileName = filename;}
    const std::string& GetCaseFileName() {return m_CaseFileName;}

    void SetPrefixName(const std::string& name);
    std::string GetPrefixName() { return m_prefix_name; }

    int32       m_PORT1;            // 服务器gate端口
    int32       m_PORT2;            

private:
    bool    LoadCenterConfig();
    bool    LoadDungeonConfig();

private:
    std::string m_file_path_map;    // 地图配置文件位置
    std::string m_center_config_path;       // center_config文件

    std::vector<uint32> m_UsedDungeonIDs;   // 副本集
    std::vector<uint32> m_WorldDungeonIDs;  // 大地图副本集
    std::vector<uint32> m_PlayerSkillIDs;   // 玩家的所有技能

    std::string m_prefix_name;      // 前缀名
    std::string m_svr_ip;           // 服务器ip
    int32       m_svr_port;         // 服务器port
    int32       m_num;              // 机器人数量
    std::string m_CaseFileName;     // 用例文件名
};

#endif
