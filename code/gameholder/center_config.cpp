#include "gameholder_pch.h"
#include "center_config.h"
#include "game_setting_manager.h"
#include "config_manager.h"


IMPLEMENT_SINGLETON(CenterConfig)

CenterConfig::CenterConfig(void)
{
}

CenterConfig::~CenterConfig(void)
{
}

bool CenterConfig::ReadFile(const std::string& strfile)
{
    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(strfile.c_str()))
    {
        CnError("Error: load '%s' failed\n", strfile.c_str());
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
    {
        CnError("Error: can't get root node in '%s' failed\n", strfile.c_str());
        return false;
    }

    //服务器配置信息
    for(TiXmlElement* i = xmlRoot->FirstChildElement(); i; i = i->NextSiblingElement())
    {
        // 开服时间
        if(strcmp(i->Value(), "server_open_time") == 0)
        {
            // 读取开服时间
            std::string startTimeString;
            Crown::CSDDateTime start_time;                  ///< 开服时间
            startTimeString = i->Attribute("value");
            start_time.FromString(startTimeString);
            //明日凌晨时间点
            uint32 server_open_time = (int32)start_time.GetTimeValue();
            GameSettingManager::Instance()->SetServerOpenTime(server_open_time);
        }
        // 语言版本
        if(strcmp(i->Value(), "language") == 0)
        {
            std::string language = i->Attribute("value");
            ConfigManager::Instance()->SetLanguage(language);
        }
    }
    return true;
}