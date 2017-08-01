#include "gameholder_pch.h"
#include "vigor_config.h"
#include "vip_config.h"
#include "player.h"

IMPLEMENT_SINGLETON(VigorConfig)

VigorConfig::VigorConfig()
{
    Clear();
}

VigorConfig::~VigorConfig()
{
    
}

void VigorConfig::Clear()
{
    m_vigor_map.clear();
	m_mealtime_map.clear();
    m_max_level             = 0;

    m_vigor_interval        = 0;
    m_vigor_addval          = 0;

    m_meal_vigor_start_level = 0;
}

// ==================================================
// 加载配置文件
// ==================================================
bool VigorConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;
    int32 level = 0;
    int32 vigor = 0;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load vigor file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    // 服务器配置信息
    for(TiXmlElement* xmlEle = xmlRoot->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if(strcmp(xmlEle->Value(), "add_vigor_interval") == 0)
        {
            m_vigor_interval = Crown::SDAtou(xmlEle->Attribute("time"));
            m_vigor_addval = SDAtoi(xmlEle->Attribute("addval"));
        }
        else if (strcmp(xmlEle->Value(), "meal_vigor_start_level") == 0)
        {
            m_meal_vigor_start_level = SDAtoi(xmlEle->Attribute("level"));
        }
        else if(strcmp(xmlEle->Value(), "max_vigor") == 0)
        {
            for(TiXmlElement* vigorEle = xmlEle->FirstChildElement(); vigorEle; vigorEle = vigorEle->NextSiblingElement())
            {
                level = SDAtoi(vigorEle->Attribute("level"));
                vigor = SDAtoi(vigorEle->Attribute("vigor"));

                // 发现更大的等级
                if(level > m_max_level)
                {
                    m_max_level = level;
                }

                if(m_vigor_map.find(level) == m_vigor_map.end())
                {
                    m_vigor_map[level] = vigor;
                }
                else
                {
                    CnWarn("Duplicate vigor level: %d\n", level);
                }
            }
        }
		else if(strcmp(xmlEle->Value(), "meal_time_vigor") == 0)
		{
			for(TiXmlElement* vigorEle = xmlEle->FirstChildElement(); vigorEle; vigorEle = vigorEle->NextSiblingElement())
			{
				level = SDAtoi(vigorEle->Attribute("level"));
                std::string start_time = vigorEle->Attribute("start_time");
				std::string end_time = vigorEle->Attribute("end_time");
				uint32 add_vigor = SDAtoi(vigorEle->Attribute("add_vigor"));

				if(m_mealtime_map.find(level) == m_mealtime_map.end())
				{
					MealTimeTemplate mealtime;
					mealtime.start_time = start_time;
					mealtime.end_time = end_time;
					mealtime.add_vigor = add_vigor;
					m_mealtime_map[level] = mealtime;
				}
				else
				{
					CnWarn("Duplicate vigor level: %d\n", level);
				}
			}
		}
    }

    return true;
}

// ==================================================
// 获得最大等级
// ==================================================
int32 VigorConfig::GetMaxVigor(Player* player)
{
    std::map<int32, int32>::iterator vigorIt;
    int32 level = 0;
    int32 vip_level = 0;
    int32 max_vigor = 0;

    if(!player) return 0;

    level = player->GetLevel();
    vip_level = player->GetVipLevel();

    // 根据等级的体力值
    if(level > m_max_level)
    {
        level = max_vigor;
    }

    vigorIt = m_vigor_map.find(level);
    if(vigorIt != m_vigor_map.end())
    {
        max_vigor = vigorIt->second;
    }

    // VIP 的体力值
    if(vip_level > 0)
    {
         max_vigor += VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_1,vip_level);
    }

    return max_vigor;
}

