#include "centerserver_pch.h"
#include "game_common_setting.h"
#include "config_reader.h"

IMPLEMENT_SINGLETON(GameCommonSetting)

GameCommonSetting::GameCommonSetting(void)
{
    m_pay_point_lotter_start_time = "";
    m_pay_point_lotter_end_time  = "";

    m_day_investments_start_time = "";
    m_day_investments_end_time = "";

    m_vip_statistics_config.Clear();
}

GameCommonSetting::~GameCommonSetting(void)
{
}

bool GameCommonSetting::LoadConfig(const char* fileName)
{
    TiXmlDocument xmlDoc;

    if(!xmlDoc.LoadFile(fileName))
    {
        CnError("Load %s file failed\n", fileName);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    TiXmlElement* pay_point_lotterinfo = xmlRoot->FirstChild("pay_point_lotter")->ToElement();
    if(pay_point_lotterinfo)
    {
        ConfigReader::Read(pay_point_lotterinfo, "start", m_pay_point_lotter_start_time);
        ConfigReader::Read(pay_point_lotterinfo, "end", m_pay_point_lotter_end_time);
    }

    TiXmlElement* day_investmentsinfo = xmlRoot->FirstChild("day_investments")->ToElement();
    if(day_investmentsinfo)
    {
        ConfigReader::Read(day_investmentsinfo, "start", m_day_investments_start_time);
        ConfigReader::Read(day_investmentsinfo, "end", m_day_investments_end_time);
    }

    TiXmlElement* donateInfo = xmlRoot->FirstChild("donate")->ToElement();
    if(donateInfo)
    {
        donateInfo->QueryUnsignedAttribute("after_minute", &m_donate_config.after_minute);
        donateInfo->QueryUnsignedAttribute("add_value", &m_donate_config.add_value);
    }

    TiXmlElement* vip_statisticsInfo = xmlRoot->FirstChild("vip_statistics")->ToElement();
    if(vip_statisticsInfo)
    {
        VIP_STATISTICS_DATA data;
        for(TiXmlElement* vipLevelEle = vip_statisticsInfo->FirstChildElement(); vipLevelEle; vipLevelEle = vipLevelEle->NextSiblingElement())
        {
            data.Clear();
            vipLevelEle->QueryUnsignedAttribute("level", &data.level);
            vipLevelEle->QueryUnsignedAttribute("max_value", &data.max_value);
            vipLevelEle->QueryUnsignedAttribute("cycles_minute", &data.cycles_minute);
            vipLevelEle->QueryUnsignedAttribute("auto_value", &data.auto_value);
            m_vip_statistics_config.vip_list.push_back(data);
        }
    }

    TiXmlElement* voidFieldMulti = xmlRoot->FirstChild("void_field_multi")->ToElement();
    if (voidFieldMulti)
    {
        voidFieldMulti->QueryUnsignedAttribute("world_level_need", &m_voidFieldMultiCfg.world_level_need);
        voidFieldMulti->QueryUnsignedAttribute("regular_boss_player_num", &m_voidFieldMultiCfg.regular_boss_player_num);
        voidFieldMulti->QueryUnsignedAttribute("regular_moonless_player_num", &m_voidFieldMultiCfg.regular_moonless_player_num);
        voidFieldMulti->QueryUnsignedAttribute("regular_boss_channel_num", &m_voidFieldMultiCfg.regular_boss_channel_num);
        voidFieldMulti->QueryUnsignedAttribute("regular_moonless_channel_num", &m_voidFieldMultiCfg.regular_moonless_channel_num);
        voidFieldMulti->QueryUnsignedAttribute("regular_boss_cd", &m_voidFieldMultiCfg.regular_boss_cd);
        voidFieldMulti->QueryUnsignedAttribute("regular_moonless_cd", &m_voidFieldMultiCfg.regular_moonless_cd);
        voidFieldMulti->QueryUnsignedAttribute("timerange", &m_voidFieldMultiCfg.timerange);
    }

    TiXmlElement* voidFieldSolo = xmlRoot->FirstChild("void_field_solo")->ToElement();
    if (voidFieldSolo)
    {
        voidFieldSolo->QueryUnsignedAttribute("world_level_need", &m_voidFieldSoloCfg.world_level_need);
        voidFieldSolo->QueryUnsignedAttribute("regular_boss_player_num", &m_voidFieldSoloCfg.regular_boss_player_num);
        voidFieldSolo->QueryUnsignedAttribute("regular_boss_channel_num", &m_voidFieldSoloCfg.regular_boss_channel_num);
        voidFieldSolo->QueryUnsignedAttribute("regular_boss_cd", &m_voidFieldSoloCfg.regular_boss_cd);
        voidFieldSolo->QueryUnsignedAttribute("timerange", &m_voidFieldSoloCfg.timerange);
    }

   return true;
}
