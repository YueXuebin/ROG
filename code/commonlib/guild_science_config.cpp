#include "commonlib_pch.h"
#include "guild_science_config.h"

IMPLEMENT_SINGLETON(GuildScienceConfig)

GuildScienceConfig::GuildScienceConfig()
{
    m_additions.clear();
    m_promoteGamePoints.clear();
    m_promoteMoneyPoints.clear();
}

GuildScienceConfig::~GuildScienceConfig()
{

}

bool GuildScienceConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load guild science file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    //服务器配置信息
    for(TiXmlElement* xmlEle = xmlRoot->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if(strcmp(xmlEle->Value(), "promote") == 0)
        {
            if(!LoadPromote(xmlEle))
                return false;
        }
        else if(strcmp(xmlEle->Value(), "donate_game_point") == 0)
        {
            if(!LoadDonateGamePoint(xmlEle))
                return false;
        }
        else if(strcmp(xmlEle->Value(), "donate_money_point") == 0)
        {
            if(!LoadDonateMoneyPoint(xmlEle))
                return false;
        }
    }

    return true;
}

bool GuildScienceConfig::LoadPromote(TiXmlElement* xmlEle)
{
    GuildAdditionTmpl       guildAddition;
    GuildPromoteLevelTmpl   guildPromoteLevel;

    for(TiXmlElement* additionEle = xmlEle->FirstChildElement(); additionEle; additionEle = additionEle->NextSiblingElement())
    {
        guildAddition.Clear();

        guildAddition.id        = SDAtoi(additionEle->Attribute("id"));
        //guildAddition.maxlevel  = SDAtoi(additionEle->Attribute("maxlevel"));

        for(TiXmlElement* levelEle = additionEle->FirstChildElement(); levelEle; levelEle = levelEle->NextSiblingElement())
        {
            guildPromoteLevel.Clear();

            guildPromoteLevel.level     = SDAtoi(levelEle->Attribute("level"));
            guildPromoteLevel.addvalue  = SDAtoi(levelEle->Attribute("addvalue"));
            guildPromoteLevel.exp       = SDAtoi(levelEle->Attribute("exp"));

            guildAddition.levels.push_back(guildPromoteLevel);
            guildAddition.maxlevel  = guildPromoteLevel.level;
        }

        m_additions.push_back(guildAddition);
    }

    return true;
}

bool GuildScienceConfig::LoadDonateGamePoint(TiXmlElement* xmlEle)
{
    GuildPromoteDonatePointTmpl guildPromoteTmpl;

    for(TiXmlElement* promoteEle = xmlEle->FirstChildElement(); promoteEle; promoteEle = promoteEle->NextSiblingElement())
    {
        guildPromoteTmpl.Clear();

        guildPromoteTmpl.times              = SDAtoi(promoteEle->Attribute("times"));
        guildPromoteTmpl.guild_point        = SDAtoi(promoteEle->Attribute("guild_point"));
        guildPromoteTmpl.exp                = SDAtoi(promoteEle->Attribute("exp"));
        guildPromoteTmpl.donate_num         = SDAtoi(promoteEle->Attribute("donate_game_point"));

        m_promoteGamePoints.push_back(guildPromoteTmpl);
    }

    return true;
}

bool GuildScienceConfig::LoadDonateMoneyPoint(TiXmlElement* xmlEle)
{
    GuildPromoteDonatePointTmpl guildPromoteTmpl;

    for(TiXmlElement* promoteEle = xmlEle->FirstChildElement(); promoteEle; promoteEle = promoteEle->NextSiblingElement())
    {
        guildPromoteTmpl.Clear();

        guildPromoteTmpl.times              = SDAtoi(promoteEle->Attribute("times"));
        guildPromoteTmpl.guild_point        = SDAtoi(promoteEle->Attribute("guild_point"));
        guildPromoteTmpl.exp                = SDAtoi(promoteEle->Attribute("exp"));
        guildPromoteTmpl.donate_num         = SDAtoi(promoteEle->Attribute("donate_money_point"));

        m_promoteMoneyPoints.push_back(guildPromoteTmpl);
    }

    return true;
}

const GuildAdditionTmpl* GuildScienceConfig::GetScienceAdditionTmpl(int32 id)
{
    for(GuildAdditionVert::iterator additionIt = m_additions.begin(); additionIt != m_additions.end(); ++additionIt)
    {
        if(additionIt->id == id)
        {
            return &(*additionIt);
        }
    }
    return NULL;
}

const GuildPromoteLevelTmpl* GuildScienceConfig::GetPromoteLevelTmpl(int32 id, int32 level)
{
    const GuildAdditionTmpl* additonTmp = GetScienceAdditionTmpl(id);
    if(!additonTmp)
        return NULL;

    for(std::vector<GuildPromoteLevelTmpl>::const_iterator additionIt = additonTmp->levels.begin(); additionIt != additonTmp->levels.end(); ++additionIt)
    {
        if(additionIt->level == level)
        {
            return &(*additionIt);
        }
    }
    return NULL;
}

const GuildPromoteDonatePointTmpl* GuildScienceConfig::GetPromoteDonateGamePointTmpl(int32 times)
{
    for(GuildPromoteDonatePointVert::iterator it = m_promoteGamePoints.begin(); it != m_promoteGamePoints.end(); ++it)
    {
        if(it->times == times)
        {
            return &(*it);
        }
    }

    return NULL;
}

const GuildPromoteDonatePointTmpl* GuildScienceConfig::GetPromoteDonateMoneyPointTmpl(int32 times)
{
    for(GuildPromoteDonatePointVert::iterator it = m_promoteMoneyPoints.begin(); it != m_promoteMoneyPoints.end(); ++it)
    {
        if(it->times == times)
        {
            return &(*it);
        }
    }

    return NULL;
}
