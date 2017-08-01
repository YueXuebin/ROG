/*
 *	游戏配置管理
 */
#ifndef config_manager_h__
#define config_manager_h_

#include "sdu.h"


class ConfigManager
{
    friend class CenterConfig;
private:
    ConfigManager();
    virtual ~ConfigManager();

    DECLARE_SINGLETON(ConfigManager)

public:
    bool Init(uint32 server_id);
    void UnInit();

    const std::string& GetLanguage() { return m_language; }
    const std::string& GetSdbPath() { return m_sdb_path; }
    bool IsTW();

private:
    void SetLanguage(const std::string& language);

private:
    std::string m_language;
    std::string m_sdb_path;

};

#endif
